/*
 *  sobj.c - Multi level double linked lists
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

#include "debug.h"
#include "gc.h"

DEBUG_CREATE_CTX(GC, DBG_QUIET);

#define GC_POOL_COUNT	100

typedef struct gc_pool_s {
	gcobj_t *pool[GC_POOL_COUNT];
} gc_pool_t;

typedef enum gc_mem_type_e {
	GC_MEM_SYSTEM = 0,
	GC_MEM_2D,
} gc_mem_type_t;

typedef struct gc_mem_s {
	int32_t		size;
	uint32_t	index;
	gc_mem_type_t	mem_type;
	void		*d_ptr;
	void		*phys_ptr;
} gc_mem_t;

typedef struct gcobj_private_s {
	void 		**sp;
	uint32_t	sp_index;
	uint32_t	sp_top;
	uint32_t	memused;
} gcobj_private_t;

static gc_pool_t gc_pool = { .pool = {NULL, }};

static int gc_alloc2d_dummy(int w, int h, void **physical_addr_p, void **virtual_addr_p);
static int gc_free2d_dummy(void *physical_addr_p);

static gc_alloc2d_f alloc2d_cb_p = gc_alloc2d_dummy;
static gc_free2d_f free2d_cb_p = gc_free2d_dummy;

static void *gc_malloc(void *this, size_t memsize);
static void gc_free(void *this, void *memp);
static char *gc_strdup(void *this, const char *str_p);

static int gc_malloc2d(void *this, int w, int h);
static void gc_free2d(void *this, int id);

static int gc_alloc2d_dummy(int w, int h, void **physical_addr_p, void **virtual_addr_p)
{
	EPRN("ALLOC2D Allocator not registered!\n");
	return -1;
}

static int gc_free2d_dummy(void *physical_addr_p)
{
	EPRN("FREE2D Allocator not registered!\n");
	return -1;
}

static void gc_pool_add(gcobj_t *gc_p)
{
	int i;

	for (i = 0; i < GC_POOL_COUNT; i++) {
		if (gc_pool.pool[i] == NULL) {
			gc_pool.pool[i] = gc_p;
			break;
		}
	}
}

static void gc_pool_del(gcobj_t *gc_p)
{
	int i;

	for (i = 0; i < GC_POOL_COUNT; i++) {
		if (gc_pool.pool[i] == gc_p) {
			gc_pool.pool[i] = NULL;
			break;
		}
	}
}

static void gc_dump(void *gcobj_p);

static void gc_pool_dump(void)
{
	int i;
	int total_mem_used = 0;
	gcobj_private_t *gc_prv_p;

	IPRN("------------------------- GC dump pool start -------------------------\n");
	for (i = 0; i < GC_POOL_COUNT; i++) {
		if (gc_pool.pool[i] != NULL) {
			gc_dump(gc_pool.pool[i]);
			gc_prv_p = (gcobj_private_t *) gc_pool.pool[i]->private_p;
			total_mem_used += gc_prv_p->memused;
		}
	}
	IPRN("TOTAL = %d\n", total_mem_used);
	IPRN("------------------------- GC dump pool end ---------------------------\n");
}


static void gc_dump(void *gcobj_p)
{
	gcobj_t *gc_p = (gcobj_t *) gcobj_p;
	gcobj_private_t *gc_prv_p;
	if (gc_p == NULL) {
		gc_pool_dump();
		return;
	}
	gc_prv_p = (gcobj_private_t *) gc_p->private_p;
	IPRN("\t[%p] GC dump start -------------------------\n", gc_p);
	IPRN("\tmemused     = %d B\n", gc_prv_p->memused);
	IPRN("\tsp_top      = %d B\n", gc_prv_p->sp_top);
	IPRN("\tsp_index    = %d B\n", gc_prv_p->sp_index);
	IPRN("\t[%p] GC dump end ---------------------------\n", gc_p);
}

__attribute__ ((visibility ("default")))
gcobj_t *gc_objnew(void)
{
	gcobj_t *tobj = NULL;
	gcobj_private_t *gc_prv_p;

	tobj = malloc(sizeof(gcobj_t));
	if (tobj == NULL) {
		return NULL;
	}
	tobj->private_p = malloc(sizeof(gcobj_private_t));
	if (tobj->private_p == NULL) {
		free(tobj);
		return NULL;
	}
	gc_prv_p = (gcobj_private_t *) tobj->private_p;
	gc_prv_p->sp_index = 0;
	gc_prv_p->memused = 0;
	gc_prv_p->sp_top = 19;
	tobj->dump = gc_dump;
	tobj->memalloc = gc_malloc;
	tobj->memfree = gc_free;
	tobj->stringdup = gc_strdup;
	tobj->malloc2d = gc_malloc2d;
	tobj->free2d = gc_free2d;
	gc_prv_p->sp = calloc(gc_prv_p->sp_top + 1, sizeof(void *));
	if (gc_prv_p->sp == NULL) {
		free(gc_prv_p);
		free(tobj);
		return NULL;
	}
	gc_pool_add(tobj);
	return tobj;
}

__attribute__ ((visibility ("default")))
void gc_objdel(gcobj_t *this)
{
	int i;
	gcobj_private_t *gc_prv_p;

	gc_prv_p = (gcobj_private_t *) this->private_p;
	for (i = 0; i < gc_prv_p->sp_index; i++) {
		if (gc_prv_p->sp[i] != NULL) {
			free(gc_prv_p->sp[i]);
			gc_prv_p->sp[i] = NULL;
		}
	}
	gc_pool_del(this);
	free(gc_prv_p->sp);
	free(gc_prv_p);
	free(this);
}

static void *gc_malloc(void *this, size_t memsize)
{
	void *memres = NULL;
	void *tempmemp = NULL;
	gc_mem_t	*gc_mem = NULL;
	int i;
	gcobj_t *this_p = (gcobj_t *)this;
	gcobj_private_t *gc_prv_p;

	if (this_p == NULL) {
		return NULL;
	}

	gc_prv_p = (gcobj_private_t *) this_p->private_p;

	gc_mem = malloc(memsize + sizeof(gc_mem_t));
	if (gc_mem == NULL) {
		return NULL;
	}
	gc_mem->size = memsize;
	gc_mem->mem_type = GC_MEM_SYSTEM;
	memres = gc_mem + 1;
	gc_mem->d_ptr = memres;
	for (i = 0; i < gc_prv_p->sp_index; i++) {
		if (gc_prv_p->sp[i] == NULL) {
			gc_prv_p->sp[i] = gc_mem;
			gc_prv_p->memused += memsize;
			gc_mem->index = i;
			return memres;
		}
	}

	if (gc_prv_p->sp_index > gc_prv_p->sp_top) {
		tempmemp = realloc(gc_prv_p->sp, (gc_prv_p->sp_top + 1 + 20) * sizeof(void *));
		if (tempmemp == NULL) {
			free(gc_mem);
			return NULL;
		}
		gc_prv_p->sp = tempmemp;
		memset(&gc_prv_p->sp[gc_prv_p->sp_top + 1], 0, 20 * sizeof(void *));
		gc_prv_p->sp_top += 20;
	}
	gc_prv_p->sp[gc_prv_p->sp_index] = gc_mem;
	gc_prv_p->memused += memsize;
	gc_mem->index = gc_prv_p->sp_index;
	gc_prv_p->sp_index++;
	return memres;
}

static void gc_free(void *this, void *memp)
{
	int i;
	gcobj_t *this_p = (gcobj_t *)this;
	gc_mem_t	*gc_mem = NULL;
	gcobj_private_t *gc_prv_p;

	gc_prv_p = (gcobj_private_t *) this_p->private_p;

	gc_mem = (gc_mem_t *)memp;
	gc_mem--;
	i = gc_mem->index;
	if (gc_prv_p->sp[i] != NULL) {
		gc_prv_p->memused -= gc_mem->size;
		free(gc_prv_p->sp[i]);
		gc_prv_p->sp[i] = NULL;
	}
}

static char *gc_strdup(void *this, const char *str_p)
{
	char *str_rp = NULL;
	int str_len;

	if (str_p == NULL) {
		return NULL;
	}
	str_len = strlen(str_p) + 1;
	str_rp = gc_malloc(this, str_len);
	if (str_rp) {
		memcpy(str_rp, str_p, str_len);
	}
	return str_rp;
}

__attribute__ ((visibility ("default")))
void gc_register_alloc2d(gc_alloc2d_f alloc2d_cb)
{
	if (alloc2d_cb == NULL) {
		alloc2d_cb_p = gc_alloc2d_dummy;
	} else {
		alloc2d_cb_p = alloc2d_cb;
	}
}

__attribute__ ((visibility ("default")))
void gc_register_free2d(gc_free2d_f free2d_cb)
{
	if (free2d_cb == NULL) {
		free2d_cb_p = gc_free2d_dummy;
	} else {
		free2d_cb_p = free2d_cb;
	}
}

static int gc_malloc2d(void *this, int w, int h)
{
	void *tempmemp = NULL;
	gc_mem_t	*gc_mem = NULL;
	int i;
	gcobj_t *this_p = (gcobj_t *)this;
	gcobj_private_t *gc_prv_p;

	if (this_p == NULL) {
		return -1;
	}

	gc_prv_p = (gcobj_private_t *) this_p->private_p;

	gc_mem = malloc(sizeof(gc_mem_t));
	if (gc_mem == NULL) {
		return -1;
	}
	gc_mem->size = w * h;
	gc_mem->mem_type = GC_MEM_2D;
	if (alloc2d_cb_p(w, h, &gc_mem->phys_ptr, &gc_mem->d_ptr) < 0) {
		free(gc_mem);
		return -1;
	}

	for (i = 0; i < gc_prv_p->sp_index; i++) {
		if (gc_prv_p->sp[i] == NULL) {
			gc_prv_p->sp[i] = gc_mem;
			gc_prv_p->memused += w * h;
			gc_mem->index = i;
			return i;
		}
	}

	if (gc_prv_p->sp_index > gc_prv_p->sp_top) {
		tempmemp = realloc(gc_prv_p->sp, (gc_prv_p->sp_top + 1 + 20) * sizeof(void *));
		if (tempmemp == NULL) {
			free2d_cb_p(gc_mem->phys_ptr);
			free(gc_mem);
			return -1;
		}
		gc_prv_p->sp = tempmemp;
		memset(&gc_prv_p->sp[gc_prv_p->sp_top + 1], 0, 20 * sizeof(void *));
		gc_prv_p->sp_top += 20;
	}
	gc_prv_p->sp[gc_prv_p->sp_index] = gc_mem;
	gc_prv_p->memused += w * h;
	gc_mem->index = gc_prv_p->sp_index;
	gc_prv_p->sp_index++;

	return gc_mem->index;
}

static void gc_free2d(void *this, int id)
{
	gcobj_t *this_p = (gcobj_t *)this;
	gc_mem_t	*gc_mem = NULL;
	gcobj_private_t *gc_prv_p;

	gc_prv_p = (gcobj_private_t *) this_p->private_p;

	if (gc_prv_p->sp[id] != NULL) {
		gc_mem = gc_prv_p->sp[id];
		gc_prv_p->memused -= gc_mem->size;
		free(gc_prv_p->sp[id]);
		gc_prv_p->sp[id] = NULL;
		free2d_cb_p(gc_mem->phys_ptr);
	}
}

int gc_test(void)
{
	gcobj_t *tobj = NULL;
	int32_t *tmem[4010];
	int i;

	tobj = gc_objnew();
	for (i = 0; i < 10; i++) {
		tmem[i] = tobj->memalloc(tobj, 100);
	}
	gc_pool_dump();
	for (i = 2; i < 4; i++) {
		tobj->memfree(tobj, tmem[i]);
	}
	gc_pool_dump();
	for (i = 2; i < 4; i++) {
		tmem[i] = tobj->memalloc(tobj, 100);
	}
	gc_pool_dump();
	for (i = 5; i < 7; i++) {
		tobj->memfree(tobj, tmem[i]);
	}
	gc_pool_dump();

	gc_objdel(tobj);

	tobj = gc_objnew();
	for (i = 0; i < 4002; i++) {
		tmem[i] = tobj->memalloc(tobj, 100);
	}
	i = tobj->malloc2d(tobj, 100, 100);
	gc_pool_dump();
	gc_objdel(tobj);
	gc_pool_dump();


	memset(tmem, 0, sizeof(tmem));

	return 0;
}

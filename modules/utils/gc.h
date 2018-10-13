/*
 *  gc.h - Simple garbage colector
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef __GC_H
#define __GC_H

#include <stdint.h>
#include <unistd.h>

typedef struct gcobj_s {
	void (*dump)(void *this);

	void *(*memalloc)(void *this, size_t memsize);
	void (*memfree)(void *this, void *memptr);
	char *(*stringdup)(void *this, const char *str_p);

	int (*malloc2d)(void *this, int w, int h);
	void (*free2d)(void *this, int id);

	void *private_p;
} gcobj_t;

typedef int (*gc_alloc2d_f)(int w, int h, void **physical_addr_p, void **virtual_addr_p);
typedef int (*gc_free2d_f)(void *physical_addr_p);

gcobj_t *gc_objnew(void);
void gc_objdel(gcobj_t *this);

void gc_register_alloc2d(gc_alloc2d_f alloc2d_cb);
void gc_register_free2d(gc_free2d_f free2d_cb);

int gc_test(void);

#endif /* __GC_H */

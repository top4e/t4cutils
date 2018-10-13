/*
 *  sobj.h - Multi level double linked lists
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef __SOBJ_H
#define __SOBJ_H

#include <stdint.h>
#include <stdbool.h>

typedef struct SObj_s {
	char		*name;
	uint32_t	child_count;
	struct SObj_s	*parent;
	struct SObj_s	*parent_dbg;
	struct SObj_s	*child;
	struct SObj_s	*child_last;
	struct SObj_s	*next;
	struct SObj_s	*previous;
	void		*gc;

	void		*private_data;
} SObj_t;

SObj_t *sobj_get_parent(SObj_t *sobj);
SObj_t *sobj_get_previous(SObj_t *sobj);
SObj_t *sobj_get_next(SObj_t *sobj);
SObj_t *sobj_get_child(SObj_t *parent);
SObj_t *sobj_get_last_child(SObj_t *parent);

SObj_t *sobj_create(SObj_t *parent, const char *name);
void sobj_destroy(SObj_t *sobj);
void sobj_destroy_childs(SObj_t *sobj);
bool sobj_valid(SObj_t *sobj);

void sobj_add_child(SObj_t *parent, SObj_t *child);
void sobj_remove_child(SObj_t *child);

void sobj_add_child_after(SObj_t *base, SObj_t *new);
void sobj_add_child_before(SObj_t *base, SObj_t *new);

void sobj_swap_next(SObj_t *sobj);
void sobj_swap_previous(SObj_t *sobj);

void sobj_set_private(SObj_t *sobj, void *data);
void *sobj_get_private(SObj_t *sobj);

void *sobj_malloc(SObj_t *sobj_p, int memsize);
void *sobj_calloc(SObj_t *sobj_p, int count, int memsize);
char *sobj_strdup(SObj_t *sobj_p, const char *str_p);
void sobj_free(SObj_t *sobj_p, void *ptr);
void sobj_print_mem(SObj_t *sobj_p);
void sobj_print_mem_full(SObj_t *sobj_p);

void sobj_print(const char *tag, SObj_t *sobj, int (*cb)(void*));

int test_sobj(void);

#endif /* __SOBJ_H */

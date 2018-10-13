/*
 *  sobj.c - Multi level double linked lists
 *
 *  Copyright (C) 2018 Atanas Tulbenski <top4ester@gmail.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sobj.h"
#include "gc.h"
#include "debug.h"

DEBUG_CREATE_CTX(SOBJ, DBG_QUIET);

//#define SOBJ_DBG_VERBOSE

__attribute__ ((visibility ("default")))
void sobj_print(const char *tag, SObj_t *sobj, int (*cb)(void*))
{
	SObj_t *ittr = sobj;
	SObj_t *child = NULL;

	while (ittr) {
		IPRN("%s P[%15s] || \"%15s\" ||<--[\"%15s\"]-->|| \"%15s\" || Child=%s Child_last=%s\n",
		     tag, (ittr->parent) ? ittr->parent->name : "N/A",
		     (ittr->previous) ? ittr->previous->name : "N/A      ",
		     ittr->name, (ittr->next) ? ittr->next->name : "N/A      ",
		     (ittr->child) ? ittr->child->name : "N/A      ",
		     (ittr->child_last) ? ittr->child_last->name : "N/A      ");
		if (cb) {
			if (cb(sobj_get_private(ittr))) {
				return;
			}
		}
		child = ittr->child;
		if (child) {
			sobj_print("\t\t\t", child, cb);
		}
		ittr = ittr->next;
	}
}

__attribute__ ((visibility ("default")))
void *sobj_malloc(SObj_t *sobj_p, int memsize)
{
	gcobj_t	*gc_p;

	if (sobj_p == NULL) {
		return NULL;
	}

	if (sobj_p->gc == NULL) {
		return NULL;
	}

	gc_p = sobj_p->gc;

	return gc_p->memalloc(gc_p, memsize);
}

__attribute__ ((visibility ("default")))
void *sobj_calloc(SObj_t *sobj_p, int count, int memsize)
{
	void *ptr = NULL;

	ptr = sobj_malloc(sobj_p, count * memsize);
	if (ptr) {
		memset(ptr, 0, count * memsize);
	}
	return ptr;
}

__attribute__ ((visibility ("default")))
char *sobj_strdup(SObj_t *sobj_p, const char *str_p)
{
	gcobj_t	*gc_p;

	if (str_p == NULL) {
		return NULL;
	}
	if (sobj_p == NULL) {
		return NULL;
	}

	if (sobj_p->gc == NULL) {
		return NULL;
	}

	gc_p = sobj_p->gc;

	return (char *)gc_p->stringdup(gc_p, str_p);
}

__attribute__ ((visibility ("default")))
void sobj_free(SObj_t *sobj_p, void *ptr)
{
	gcobj_t	*gc_p;

	if (sobj_p == NULL) {
		return;
	}

	if (sobj_p->gc == NULL) {
		return;
	}

	gc_p = sobj_p->gc;

	gc_p->memfree(gc_p, ptr);
}

__attribute__ ((visibility ("default")))
void sobj_print_mem(SObj_t *sobj_p)
{
	gcobj_t	*gc_p;

	if (sobj_p == NULL) {
		return;
	}

	if (sobj_p->gc == NULL) {
		return;
	}

	gc_p = sobj_p->gc;

	gc_p->dump(gc_p);
}

__attribute__ ((visibility ("default")))
void sobj_print_mem_full(SObj_t *sobj_p)
{
	gcobj_t	*gc_p;

	if (sobj_p == NULL) {
		return;
	}

	if (sobj_p->gc == NULL) {
		return;
	}

	gc_p = sobj_p->gc;

	gc_p->dump(NULL);
}

__attribute__ ((visibility ("default")))
void sobj_add_child(SObj_t *parent, SObj_t *child)
{
	SObj_t	*tsobj = NULL;

	if (parent != NULL) {
		parent->child_count++;
		tsobj = parent->child;
		if (tsobj == NULL) {
			parent->child = child;
#ifdef SOBJ_DBG_VERBOSE
			IPRN("[%s] (%p) First child <%s>\n", __FUNCTION__, parent, child->name);
#endif
		} else {
			while (tsobj) {
				if (tsobj->next != NULL) {
					tsobj = tsobj->next;
				} else {
					tsobj->next = child;
					child->previous = tsobj;
#ifdef SOBJ_DBG_VERBOSE
					IPRN("[%s] (%p) Last Child <%s>\n", __FUNCTION__, parent, tsobj->name);
#endif
					break;
				}
			}
		}
		parent->child_last = child;
#ifdef SOBJ_DBG_VERBOSE
		sobj_print("[sobj_add_child]: parent", parent, NULL);
		sobj_print("[sobj_add_child]:  child", child, NULL);
#endif
	}
}

__attribute__ ((visibility ("default")))
SObj_t *sobj_create(SObj_t *parent, const char *name)
{
	SObj_t	*sobj = NULL;
	gcobj_t	*gc_p;

	gc_p = gc_objnew();
	sobj = gc_p->memalloc(gc_p, sizeof(SObj_t));
	if (sobj == NULL) {
		return NULL;
	}

	sobj->child_count = 0;
	sobj->parent = parent;
	sobj->child = NULL;
	sobj->child_last = NULL;
	sobj->next = NULL;
	sobj->previous = NULL;
	sobj->private_data = NULL;
	sobj->gc = gc_p;
	if (name != NULL) {
		sobj->name = gc_p->stringdup(gc_p, name);
	} else {
		sobj->name = gc_p->stringdup(gc_p, "undefined");
	}

	sobj_add_child(parent, sobj);

	return (sobj);
}

__attribute__ ((visibility ("default")))
void sobj_remove_child(SObj_t *child)
{
	SObj_t	*next_sobj = NULL;
	SObj_t	*prev_sobj = NULL;
	SObj_t	*parent;

	parent = child->parent;

	if (parent == NULL) {
		//WPRN("[%s] Warning object <%s> does not have a parent!\n", __FUNCTION__, child->name);
		return;
	}

	//sobj_print("B parent", parent);
	//sobj_print("B child", child);

	if (parent->child_count == 1) {
		if (parent->child == child) {
			parent->child = NULL;
			parent->child_last = NULL;
		} else {
			EPRN("Parent %s only child %s is not equal to %s\n", parent->name, parent->child->name, child->name);
		}
	} else {
		next_sobj = child->next;
		prev_sobj = child->previous;

		if (prev_sobj == NULL) { /* first child */
			if (next_sobj == NULL) { /* only child */
				//WPRN("Only child\n");
				parent->child = NULL;
				parent->child_last = NULL;
			} else {
				//WPRN("First child\n");
				parent->child = next_sobj;
				next_sobj->previous = NULL;
			}
		} else {
			if (next_sobj == NULL) { /* last child */
				//WPRN("Last child\n");
				prev_sobj->next = NULL;
				parent->child_last = prev_sobj;
			} else {
				//WPRN("Child in the middle\n");
				next_sobj->previous = prev_sobj;
				prev_sobj->next = next_sobj;
			}
		}
	}

	child->parent = NULL;
	child->parent_dbg = parent;
#ifdef SOBJ_DBG_VERBOSE
	//sobj_print("A parent", parent);
	//sobj_print("A child", child);
	IPRN("[%s] Removed <%s>\n", __FUNCTION__, child->name);
#endif
	parent->child_count--;
}

__attribute__ ((visibility ("default")))
void sobj_add_child_before(SObj_t *base, SObj_t *new)
{
	SObj_t	*prev_sobj;

	if (new->parent != NULL) {
		EPRN("[%s] Error object <%s> already has a parent!\n", __FUNCTION__, new->name);
		return;
	}

	prev_sobj = base->previous;

	new->next = base;
	new->previous = prev_sobj;

	base->previous = new;
	new->parent = base->parent;

	if (prev_sobj == NULL) {
		if (base->parent != NULL) {
			base->parent->child = new;
		}
	} else {
		prev_sobj->next = new;
	}

	if (new->parent != NULL) {
		new->parent->child_count++;
	}
#ifdef SOBJ_DBG_VERBOSE
	sobj_print(__func__, new, NULL);
#endif
}

__attribute__ ((visibility ("default")))
void sobj_add_child_after(SObj_t *base, SObj_t *new)
{
	SObj_t	*next_sobj;

	if (new->parent != NULL) {
		EPRN("[%s] Error object <%s> already has a parent!\n", __FUNCTION__, new->name);
		return;
	}
	next_sobj = base->next;

	new->previous = base;
	new->next = base->next;
	new->parent = base->parent;

	base->next = new;

	if (next_sobj != NULL) {
		next_sobj->previous = new;
	} else {
		if (new->parent != NULL) {
			new->parent->child_last = new;
		}
	}

	if (new->parent != NULL) {
		new->parent->child_count++;
	}
#ifdef SOBJ_DBG_VERBOSE
	sobj_print(__func__, new, NULL);
#endif
}

/* TODO: fix child_last */
__attribute__ ((visibility ("default")))
void sobj_swap_next(SObj_t *sobj)
{
	SObj_t	*swap_sobj;
	SObj_t	*next_sobj;
	SObj_t	*prev_sobj;

	swap_sobj = sobj->next;
	if (swap_sobj == NULL) {
		EPRN("[%s] Error object <%s> is last object!\n", __FUNCTION__, sobj->name);
		return;
	}

	next_sobj = swap_sobj->next;
	prev_sobj = sobj->previous;

	if (prev_sobj != NULL) {
		prev_sobj->next = swap_sobj;
	}
	if (next_sobj != NULL) {
		next_sobj->previous = sobj;
	}

	sobj->next = next_sobj;
	sobj->previous = swap_sobj;

	swap_sobj->next = sobj;
	swap_sobj->previous = prev_sobj;
#ifdef SOBJ_DBG_VERBOSE
	sobj_print(__func__, sobj, NULL);
#endif
}

__attribute__ ((visibility ("default")))
void sobj_swap_previous(SObj_t *sobj)
{
#ifdef SOBJ_DBG_VERBOSE
	sobj_print(__func__, sobj, NULL);
#endif
}

__attribute__ ((visibility ("default")))
void sobj_destroy_childs(SObj_t *sobj)
{
	SObj_t	*tsobj = NULL;
	SObj_t	*next_sobj = NULL;
	gcobj_t	*gc_p;
#ifdef SOBJ_DBG_VERBOSE
	IPRN("[%s] Trace <%s>\n", __FUNCTION__, sobj->name);
#endif
	tsobj = sobj->child;
	while (tsobj) {
		next_sobj = tsobj->next;
		if (tsobj->child != NULL) {
			sobj_destroy_childs(tsobj);
		}
		sobj_remove_child(tsobj);
		gc_p = tsobj->gc;
		gc_p->memfree(gc_p, tsobj);
		gc_objdel(gc_p);
		tsobj = next_sobj;
	}
	sobj->child = NULL;
}

__attribute__ ((visibility ("default")))
void sobj_destroy(SObj_t *sobj)
{
	gcobj_t	*gc_p;

	if (sobj == NULL) {
		return;
	}
#ifdef SOBJ_DBG_VERBOSE
	IPRN("[%s] Trace [%p]<%s>\n", __FUNCTION__, sobj, sobj->name);
#endif
	if (sobj->child != NULL) {
		sobj_destroy_childs(sobj);
	}

	sobj_remove_child(sobj);

	gc_p = sobj->gc;
	sobj->gc = NULL;
	gc_p->memfree(gc_p, sobj);
	gc_objdel(gc_p);
}

__attribute__ ((visibility ("default")))
bool sobj_valid(SObj_t *sobj)
{
	if (sobj == NULL) {
		return false;
	}
	if (sobj->gc == NULL) {
		EPRN("Invalid SOBJ (%s)\n", sobj->name);
		if (sobj->parent_dbg) {
			SObj_t *parent;
			parent = sobj->parent_dbg;
			EPRN("Parent = %s\n", parent->name);
			if (parent->child) {
				EPRN("Parent.child = %s\n", parent->child->name);
			}
			if (parent->child_last) {
				EPRN("Parent.child_last = %s\n", parent->child_last->name);
			}
			if (parent->next) {
				EPRN("Parent.next = %s\n", parent->next->name);
			}
			if (parent->previous) {
				EPRN("Parent.previous = %s\n", parent->previous->name);
			}
		} else {
			EPRN("No parent\n");
		}
		return false;
	}
	return true;
}

__attribute__ ((visibility ("default")))
void sobj_set_private(SObj_t *sobj, void *data)
{
	sobj->private_data = data;
}

__attribute__ ((visibility ("default")))
void *sobj_get_private(SObj_t *sobj)
{
	sobj_valid(sobj);
	return (sobj->private_data);
}

__attribute__ ((visibility ("default")))
SObj_t *sobj_get_child(SObj_t *parent)
{
	return (parent->child);
}

__attribute__ ((visibility ("default")))
SObj_t *sobj_get_last_child(SObj_t *parent)
{
	return (parent->child_last);
}

__attribute__ ((visibility ("default")))
SObj_t *sobj_get_next(SObj_t *sobj)
{
	return (sobj->next);
}

__attribute__ ((visibility ("default")))
SObj_t *sobj_get_previous(SObj_t *sobj)
{
	return (sobj->previous);
}

__attribute__ ((visibility ("default")))
SObj_t *sobj_get_parent(SObj_t *sobj)
{
	return (sobj->parent);
}

int test_sobj(void)
{
	SObj_t *master_node = NULL;
	SObj_t *child_node = NULL;
	SObj_t *t_node = NULL;
	int i;

	master_node = sobj_create(NULL, "master");
	if (master_node == NULL) {
		printf("Failed to create master node\n");
	}
	sobj_print("Master node: ", master_node, NULL);
	for (i = 0; i < 5; i++) {
		char nbuf[80];
		sprintf(nbuf, "child %d", i);
		if (sobj_create(master_node, nbuf) == NULL) {
			printf("Failed to create child node\n");
		}
	}
	sobj_print("5 childs   : ", master_node, NULL);

	child_node = sobj_create(NULL, "new node");
	for (i = 0; i < 5; i++) {
		char nbuf[80];
		sprintf(nbuf, "sub child %d", i);
		if (sobj_create(child_node, nbuf) == NULL) {
			printf("Failed to create child node\n");
		}
	}

	t_node = sobj_get_child(master_node);
	t_node = sobj_get_next(t_node);

	sobj_add_child_after(t_node, child_node);
	sobj_print("Final      : ", master_node, NULL);

	sobj_destroy(master_node);

	return (0);
}

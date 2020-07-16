/**
 * @file bl.h 链表数组
 */

#ifndef SRC_BL_H_
#define SRC_BL_H_

#include <stddef.h>
#include <stdint.h>

typedef struct bl_node {
	int N;
	struct bl_node* next;
} bl_node;

typedef struct {
	bl_node* head;
	bl_node* tail;
	size_t N;
	int blocksize;
	int datasize;
	bl_node* last_access;
	size_t last_access_n;
} bl;

#define BL_NOT_FOUND (ptrdiff_t)(-1)

bl* bl_new(int blocksize, int datasize);
void  bl_init(bl* list, int blocksize, int datasize);
void  bl_free(bl* list);
void  bl_remove_all(bl* list);
size_t bl_size(const bl* list);
int bl_datasize(const bl* list);
void* bl_append(bl* list, const void* data);
void  bl_append_node(bl* list, bl_node* node);
bl_node* bl_new_node(bl* list);
void* bl_node_append(bl* list, bl_node* node, const void* data);
void  bl_get(bl* list, size_t n, void* dest);
void* bl_access(bl* list, size_t n);
void* bl_access_const(const bl* list, size_t n);
void* bl_push(bl* list, const void* data);
void  bl_pop(bl* list, void* into);
void* bl_extend(bl* list);
void  bl_remove_index(bl* list, size_t index);
void  bl_remove_from_node(bl* list, bl_node* node, bl_node* prev, int index_in_node);
void* bl_access(bl* list, size_t n);
void  bl_split(bl* src, bl* dest, size_t split);
void  bl_reverse(bl* list);
void  bl_append_list(bl* list1, bl* list2);
void  bl_insert(bl* list, size_t index, const void* data);
void  bl_set(bl* list, size_t index, const void* data);
void  bl_print_structure(bl* list);
void  bl_copy(bl* list, size_t start, size_t length, void* vdest);

/**
 * Inserts the given datum into the list in such a way that the list
 * stays sorted in ascending order according to the given comparison
 * function (assuming it was sorted to begin with!).
 *
 * The inserted element will be placed _after_ existing elements with
 * the same value.
 *
 * The comparison function is the same as qsort's: it should return
 * 1 if the first arg is greater than the second arg
 * 0 if they're equal
 * -1 if the first arg is smaller.
 *
 * The index where the element was inserted is returned.
 */
size_t bl_insert_sorted(bl* list, const void* data, int (*compare)(const void* v1, const void* v2));

/**
 If the item already existed in the list (ie, the compare function
 returned zero), then -1 is returned.  Otherwise, the index at which
 the item was inserted is returned.
 */
ptrdiff_t bl_insert_unique_sorted(bl* list, const void* data,
                                  int (*compare)(const void* v1, const void* v2));

/*
 Removes all the elements, but doesn't free the first block, which makes
 it slightly faster for the case when you're going to add more elements
 right away, since you don't have to free() the old block then immediately
 malloc() a new block.
 */
void  bl_remove_all_but_first(bl* list);
void  bl_remove_index_range(bl* list, size_t start, size_t length);
void* bl_find(bl* list, const void* data, int (*compare)(const void* v1, const void* v2));
ptrdiff_t   bl_find_index(bl* list, const void* data, int (*compare)(const void* v1, const void* v2));

// returns 0 if okay, 1 if an error is detected.
int   bl_check_consistency(bl* list);

// returns 0 if okay, 1 if an error is detected.
int   bl_check_sorted(bl* list, int (*compare)(const void* v1, const void* v2), int isunique);
void  bl_find_ind_and_element(bl* list, const void* data,
                                    int (*compare)(const void* v1, const void* v2),
                                    void** presult, ptrdiff_t* pindex);

void  clear_list(bl* list);
void  memswap(void* v1, void* v2, int len);
bl_node* find_node(const bl* list, size_t n, size_t* p_nskipped);

#endif /* SRC_BL_H_ */

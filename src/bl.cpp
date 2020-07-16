/**
 * @file bl.cpp 链表数组接口
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bl.h"

#define NODE_DATA(node)			((void*)(((bl_node*) (node)) + 1))
#define NODE_CHARDATA(node)		((char*)(((bl_node*) (node)) + 1))
#define NODE_INTDATA(node)		((int*)(((bl_node*) (node)) + 1))
#define NODE_DOUBLEDATA(node)	((double*)(((bl_node*) (node)) + 1))
#define NLFGLUE2(n, f)			n ## _ ## f
#define NLFGLUE(n, f)			NLFGLUE2(n, f)
#define NLF(func)				NLFGLUE(nl, func)

bl* bl_new(int blocksize, int datasize) {
	bl* rtn = (bl*) malloc(sizeof(bl));
	if (!rtn) {
		printf ("Couldn't allocate memory for bl\n");
		return NULL;
	}
	bl_init(rtn, blocksize, datasize);
	return rtn;
}

void bl_init(bl* list, int blocksize, int datasize) {
	list->head = NULL;
	list->tail = NULL;
	list->N = 0;
	list->blocksize = blocksize;
	list->datasize  = datasize;
	list->last_access   = NULL;
	list->last_access_n = 0;
}

void bl_free(bl* list) {
	if (list) {
		bl_remove_all(list);
		free(list);
	}
}

void bl_remove_all(bl* list) {
	bl_node *n, *lastnode;
	lastnode = NULL;

	for (n = list->head; n != NULL; n = n->next) {
		if (lastnode) free(lastnode);
		lastnode = n;
	}
	if (lastnode) free(lastnode);
	list->head = NULL;
	list->tail = NULL;
	list->last_access = NULL;
}

size_t bl_size(const bl* list) {
	return list->N;
}

int bl_datasize(const bl* list) {
	return !list ? 0 : list->datasize;
}

void* bl_append(bl* list, const void* data) {
	if (!list->tail) bl_append_node(list, bl_new_node(list));
	return bl_node_append(list, list->tail, data);
}

bl_node* bl_new_node(bl* list) {
	bl_node* rtn;

	rtn = (bl_node*) malloc(sizeof(bl_node) + list->datasize * list->blocksize);
	if (!rtn) {
		printf("Couldn't allocate memory for a bl node!\n");
		return NULL;
	}
	rtn->N = 0;
	rtn->next = NULL;
	return rtn;
}

void bl_append_node(bl* list, bl_node* node) {
    node->next = NULL;
    if (!list->head) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->N += node->N;
}

void* bl_node_append(bl* list, bl_node* node, const void* data) {
	void* dest;
	if (node->N == list->blocksize) {
		bl_node* newnode;
		newnode = bl_new_node(list);
		newnode->next = node->next;
		node->next = newnode;
		if (list->tail == node) list->tail = newnode;
		node = newnode;
	}

	dest = NODE_CHARDATA(node) + node->N * list->datasize;
	if (data) memcpy(dest, data, list->datasize);
	++node->N;
	++list->N;
	return dest;
}

void  bl_get(bl* list, size_t n, void* dest) {
	if (list->N) memcpy(dest, bl_access(list, n), list->datasize);
}

void* bl_access(bl* list, size_t n) {
	void* rtn;
	bl_node* node;
	size_t nskipped;

	node = find_node(list, n, &nskipped);
	rtn = NODE_CHARDATA(node) + (n - nskipped) * list->datasize;
	list->last_access = node;
	list->last_access_n = nskipped;
	return rtn;
}

void* bl_access_const(const bl* list, size_t n) {
	bl_node* node;
	size_t nskipped;

	node = find_node(list, n, &nskipped);
	return NODE_CHARDATA(node) + (n - nskipped) * list->datasize;
}

void* bl_push(bl* list, const void* data) {
	return bl_append(list, data);
}

void  bl_pop(bl* list, void* into) {
	if (list->N) {
		bl_get(list, list->N - 1, into);
		bl_remove_index(list, list->N - 1);
	}
}

void* bl_extend(bl* list) {
	return bl_append(list, NULL);
}

void bl_remove_index(bl* list, size_t index) {
	bl_node *node, *prev;
	size_t nskipped = 0;
	for (node=list->head, prev=NULL; node; prev=node, node=node->next) {
		if (index < (nskipped + node->N)) break;
		nskipped += node->N;
	}
	if (node) {
		bl_remove_from_node(list, node, prev, index-nskipped);
		list->last_access   = NULL;
		list->last_access_n = 0;
	}
}

bl_node* find_node(const bl* list, size_t n, size_t* p_nskipped) {
	bl_node* node;
	size_t nskipped;

	if (list->last_access && n >= list->last_access_n) {
		nskipped = list->last_access_n;
		node = list->last_access;
	}
	else {
		node = list->head;
		nskipped = 0;
	}
	for (; node; ) {
		if (n < (nskipped + node->N)) break;
		nskipped += node->N;
		node = node->next;
	}
	assert(node);
	if (p_nskipped) *p_nskipped = nskipped;
	return node;
}

void bl_remove_from_node(bl* list, bl_node* node, bl_node* prev, int index_in_node) {
	if (node->N == 1) {
		if (prev == NULL) {
			list->head = node->next;
			if (list->head == NULL) list->tail = NULL;
		}
		else {
			if (node == list->tail) list->tail = prev;
			prev->next = node->next;
		}
		free(node);
	}
	else {
		int ncopy = node->N - index_in_node - 1;
		if (ncopy > 0) {
			memmove(NODE_CHARDATA(node) + index_in_node * list->datasize,
					NODE_CHARDATA(node) + (index_in_node+1) * list->datasize,
                    ncopy * list->datasize);
		}
		--node->N;
	}
	--list->N;
}

void bl_split(bl* src, bl* dest, size_t split) {
	bl_node* node;
	size_t nskipped;
	size_t ind;
	size_t ntaken = src->N - split;

	node = find_node(src, split, &nskipped);
	ind = split - nskipped;
	if (ind == 0) {
		if (split) {
			bl_node* last = find_node(src, split - 1, NULL);
			last->next = NULL;
			src->tail  = last;
		}
		else {
			src->head = NULL;
			src->tail = NULL;
		}
	}
	else {
		bl_node* newnode = bl_new_node(dest);
		newnode->N = (node->N - ind);
		newnode->next = node->next;
		memcpy(NODE_CHARDATA(newnode),
				NODE_CHARDATA(node) + (ind * src->datasize),
				newnode->N * src->datasize);
		node->N -= (node->N - ind);
		node->next = NULL;
		src->tail  = node;
		node = newnode;
	}
	if (dest->tail) {
		dest->tail->next = node;
		dest->N += ntaken;
	}
	else {
		dest->head = node;
		dest->tail = node;
		dest->N += ntaken;
	}

	src->N -= ntaken;
	src->last_access = NULL;
}

void bl_reverse(bl* list) {
	bl* blocks;
	bl_node* node;
	bl_node* lastnode;
	int i;

	blocks = new bl[256];
	for (node = list->head; node; node = node->next) {
		for (i = 0; i < (node->N / 2); ++i) {
			memswap(NODE_CHARDATA(node) + i * list->datasize,
					NODE_CHARDATA(node) + (node->N - 1 - i) * list->datasize,
					list->datasize);
		}
		bl_append_node(blocks, node);
	}

	lastnode = NULL;
	for (i = bl_size(blocks) - 1; i >= 0; --i) {
		node = (bl_node*) bl_access(blocks, i);
		if (lastnode) lastnode->next = node;
		lastnode = node;
	}
	if (lastnode) lastnode->next = NULL;
	free(blocks);

	node = list->head;
	list->head = list->tail;
	list->tail = node;
	list->last_access = NULL;
	list->last_access_n = 0;
}

void memswap(void* v1, void* v2, int len) {
	unsigned char tmp;
	unsigned char* c1 = (unsigned char*) v1;
	unsigned char* c2 = (unsigned char*) v2;
	int i;

	for (i = 0; i < len; ++i) {
		tmp = *c1;
		*c1 = *c2;
		*c2 = tmp;
		++c1;
		++c2;
	}
}

void  bl_append_list(bl* list1, bl* list2) {
	list1->last_access = NULL;
	list1->last_access_n = 0;
	if (list1->datasize != list2->datasize) {
		printf("Error: cannot append bls with different data sizes!\n");
		assert(0);
		exit(0);
	}
	if (list1->blocksize != list2->blocksize) {
		printf("Error: cannot append bls with different block sizes!\n");
		assert(0);
		exit(0);
	}

	// if list1 is empty, then just copy over list2's head and tail.
	if (list1->head == NULL) {
		list1->head = list2->head;
		list1->tail = list2->tail;
		list1->N = list2->N;
        // remove everything from list2 (to avoid sharing nodes)
		clear_list(list2);
		return;
	}

	// if list2 is empty, then do nothing.
	if (list2->head == NULL) return;

	// otherwise, append list2's head to list1's tail.
	list1->tail->next = list2->head;
	list1->tail = list2->tail;
	list1->N += list2->N;
	// remove everything from list2 (to avoid sharing nodes)
	clear_list(list2);
}

void bl_insert(bl* list, size_t index, const void* data) {
	bl_node* node;
	size_t nskipped;

	if (list->N == index) {
		bl_append(list, data);
		return;
	}
	node = find_node(list, index, &nskipped);
	list->last_access = node;
	list->last_access_n = nskipped;

	// if the node is full:
	//   if we're inserting at the end of this node, then create a new node.
	//   else, shift all but the last element, add in this element, and
	//     add the last element to a new node.
	if (node->N == list->blocksize) {
		int localindex, nshift;
		bl_node* next = node->next;
		bl_node* destnode;
		localindex = index - nskipped;

		// if the next node exists and is not full, then insert the overflowing
		// element at the front.  otherwise, create a new node.
		if (next && (next->N < list->blocksize)) {
			// shift the existing elements up by one position...
			memmove(NODE_CHARDATA(next) + list->datasize,
					NODE_CHARDATA(next),
					next->N * list->datasize);
			destnode = next;
		}
		else {
			// create and insert a new node.
			bl_node* newnode = bl_new_node(list);
			newnode->next = next;
			node->next = newnode;
			if (!newnode->next) list->tail = newnode;
			destnode = newnode;
		}

		if (localindex == node->N) {
			// the new element becomes the first element in the destination node.
			memcpy(NODE_CHARDATA(destnode), data, list->datasize);
		}
		else {
			// the last element in this node is added to the destination node.
			memcpy(NODE_CHARDATA(destnode), NODE_CHARDATA(node) + (node->N-1)*list->datasize, list->datasize);
			// shift the end portion of this node up by one...
			nshift = node->N - localindex - 1;
			memmove(NODE_CHARDATA(node) + (localindex+1) * list->datasize,
					NODE_CHARDATA(node) + localindex * list->datasize,
					nshift * list->datasize);
			// insert the new element...
			memcpy(NODE_CHARDATA(node) + localindex * list->datasize, data, list->datasize);
		}

		++destnode->N;
		++list->N;
	}
	else {
		// shift...
		int localindex, nshift;
		localindex = index - nskipped;
		nshift = node->N - localindex;
		memmove(NODE_CHARDATA(node) + (localindex+1) * list->datasize,
				NODE_CHARDATA(node) + localindex * list->datasize,
				nshift * list->datasize);
		// insert...
		memcpy(NODE_CHARDATA(node) + localindex * list->datasize,
				data, list->datasize);
		++node->N;
		++list->N;
	}
}

void  bl_set(bl* list, size_t index, const void* data) {
	bl_node* node;
	size_t nskipped;
	void* dataloc;

	node = find_node (list, index, &nskipped);
	dataloc = NODE_CHARDATA(node) + (index - nskipped) * list->datasize;
	memcpy(dataloc, data, list->datasize);
	// update the last_access member...
	list->last_access = node;
	list->last_access_n = nskipped;
}

void  bl_print_structure(bl* list) {
	bl_node* n;
	printf("bl: head %p, tail %p, N %zu\n", list->head, list->tail, list->N);
	for (n=list->head; n; n=n->next) {
		printf("[N=%i] ", n->N);
	}
	printf("\n");
}

void  bl_copy(bl* list, size_t start, size_t length, void* vdest) {
	bl_node* node;
	size_t nskipped;
	char* dest;
	if (length <= 0) return;
	node = find_node(list, start, &nskipped);

	// we've found the node containing "start".  keep copying elements and
	// moving down the list until we've copied all "length" elements.
	dest = (char*) vdest;
	while (length > 0) {
		size_t take, avail;
		char* src;
		// number of elements we want to take.
		take = length;
		// number of elements available at this node.
		avail = node->N - (start - nskipped);
		if (take > avail) take = avail;
		src = NODE_CHARDATA(node) + (start - nskipped) * list->datasize;
		memcpy(dest, src, take * list->datasize);

		dest += take * list->datasize;
		start += take;
		length -= take;
		nskipped += node->N;
		node = node->next;
	}
	// update the last_access member...
	list->last_access = node;
	list->last_access_n = nskipped;
}

size_t bl_insert_sorted(bl* list, const void* data, int (*compare)(const void* v1, const void* v2)) {
	ptrdiff_t lower, upper;
	lower = -1;
	upper = list->N;
	while (lower < (upper-1)) {
		ptrdiff_t mid;
		int cmp;
		mid = (upper + lower) / 2;
		cmp = compare(data, bl_access(list, mid));
		if (cmp >= 0) lower = mid;
		else upper = mid;
	}
	bl_insert(list, lower+1, data);
	return lower+1;
}

ptrdiff_t bl_insert_unique_sorted(bl* list, const void* data, int (*compare)(const void* v1, const void* v2)) {
	// This is just straightforward binary search - really should
	// use the block structure...
	ptrdiff_t lower, upper;
	lower = -1;
	upper = list->N;
	while (lower < (upper-1)) {
		ptrdiff_t mid;
		int cmp;
		mid = (upper + lower) / 2;
		cmp = compare(data, bl_access(list, mid));
		if (cmp >= 0) lower = mid;
		else upper = mid;
	}

	if (lower >= 0) {
		if (compare(data, bl_access(list, lower)) == 0) {
			return BL_NOT_FOUND;
		}
	}
	bl_insert(list, lower+1, data);
	return lower+1;
}

void bl_remove_all_but_first(bl* list) {
	bl_node *n, *lastnode;
	lastnode = NULL;

	if (list->head) {
		for (n = list->head->next; n; n = n->next) {
			if (lastnode) free(lastnode);
			lastnode = n;
		}
		if (lastnode) free(lastnode);
		list->head->next = NULL;
		list->head->N = 0;
		list->tail = list->head;
	}
	else {
		list->head = NULL;
		list->tail = NULL;
	}
	list->N = 0;
	list->last_access = NULL;
	list->last_access_n = 0;
}

void bl_remove_index_range(bl* list, size_t start, size_t length) {
	// find the node (and previous node) at which element 'start'
	// can be found.
	bl_node *node, *prev;
	size_t nskipped = 0;
	list->last_access = NULL;
	list->last_access_n = 0;
	for (node = list->head, prev = NULL; node; prev = node, node = node->next) {
		if (start < (nskipped + node->N)) break;
		nskipped += node->N;
	}

	// begin by removing any indices that are at the end of a block.
	if (start > nskipped) {
		// we're not removing everything at this node.
		size_t istart;
		size_t n;
		istart = start - nskipped;
		if ((istart + length) < node->N) {
			// we're removing a chunk of elements from the middle of this
			// block.  move elements from the end into the removed chunk.
			memmove(NODE_CHARDATA(node) + istart * list->datasize,
					NODE_CHARDATA(node) + (istart + length) * list->datasize,
					(node->N - (istart + length)) * list->datasize);
			// we're done!
			node->N -= length;
			list->N -= length;
			return;
		}
		else {
			// we're removing everything from 'istart' to the end of this
			// block.  just change the "N" values.
			n = (node->N - istart);
			node->N -= n;
			list->N -= n;
			length -= n;
			start += n;
			nskipped = start;
			prev = node;
			node = node->next;
		}
	}

	// remove complete blocks.
	for (;;) {
		size_t n;
		bl_node* todelete;
		if (length == 0 || length < node->N) break;
		// we're skipping this whole block.
		n = node->N;
		length -= n;
		start += n;
		list->N -= n;
		nskipped += n;
		todelete = node;
		node = node->next;
		free(todelete);
	}
	if (prev) prev->next = node;
	else list->head = node;
	if (!node) list->tail = prev;

	// remove indices from the beginning of the last block.
	// note that we may have removed everything from the tail of the list,
	// no "node" may be null.
	if (node && length>0) {
		//printf("removing %i from end.\n", length);
		memmove(NODE_CHARDATA(node),
				NODE_CHARDATA(node) + length * list->datasize,
				(node->N - length) * list->datasize);
		node->N -= length;
		list->N -= length;
	}
}

void* bl_find(bl* list, const void* data, int (*compare)(const void* v1, const void* v2)) {
	void* rtn;
	bl_find_ind_and_element(list, data, compare, &rtn, NULL);
	return rtn;
}

ptrdiff_t bl_find_index(bl* list, const void* data, int (*compare)(const void* v1, const void* v2)) {
	void* val;
	ptrdiff_t ind;
	bl_find_ind_and_element(list, data, compare, &val, &ind);
	return ind;
}

void bl_find_ind_and_element(bl* list, const void* data,
		int (*compare)(const void* v1, const void* v2),
		void** presult, ptrdiff_t* pindex) {
	ptrdiff_t lower, upper;
	int cmp = -2;
	void* result;
	lower = -1;
	upper = list->N;
	while (lower < (upper-1)) {
		ptrdiff_t mid;
		mid = (upper + lower) / 2;
		cmp = compare(data, bl_access(list, mid));
		if (cmp >= 0) lower = mid;
		else upper = mid;
	}
	if (lower == -1 || compare(data, (result = bl_access(list, lower)))) {
		*presult = NULL;
		if (pindex) *pindex = -1;
		return;
	}
	*presult = result;
	if (pindex) *pindex = lower;
}

int bl_check_consistency(bl* list) {
	bl_node* node;
	size_t N;
	int tailok = 1;
	int nempty = 0;
	int nnull = 0;

	// if one of head or tail is NULL, they had both better be NULL!
	if (!list->head) ++nnull;
	if (!list->tail) ++nnull;
	if (nnull == 1) {
		fprintf(stderr, "bl_check_consistency: head is %p, and tail is %p.\n",
				list->head, list->tail);
		return 1;
	}

	N = 0;
	for (node = list->head; node; node = node->next) {
		N += node->N;
		if (!node->N) ++nempty;
		// are we at the last node?
		if (!node->next) tailok = (list->tail == node) ? 1 : 0;
	}
	if (!tailok) {
		fprintf(stderr, "bl_check_consistency: tail pointer is wrong.\n");
		return 1;
	}
	if (nempty) {
		fprintf(stderr, "bl_check_consistency: %i empty blocks.\n", nempty);
		return 1;
	}
	if (N != list->N) {
		fprintf(stderr, "bl_check_consistency: list->N is %zu, but sum of blocks is %zu.\n",
				list->N, N);
		return 1;
	}
	return 0;
}

int bl_check_sorted(bl* list,
		int (*compare)(const void* v1, const void* v2),
		int isunique) {
	size_t i, N;
	size_t nbad = 0;
	void* v2 = NULL;
	N = bl_size(list);
	if (N) v2 = bl_access(list, 0);
	for (i = 1; i < N; ++i) {
		void* v1;
		int cmp;
		v1 = v2;
		v2 = bl_access(list, i);
		cmp = compare(v1, v2);
		if (isunique) {
			if (cmp >= 0) ++nbad;
		}
		else if (cmp > 0) ++nbad;
	}
	if (nbad) {
		fprintf(stderr, "bl_check_sorted: %zu are out of order.\n", nbad);
		return 1;
	}
	return 0;
}

void clear_list(bl* list) {
	list->head = NULL;
	list->tail = NULL;
	list->N = 0;
	list->last_access = NULL;
	list->last_access_n = 0;
}

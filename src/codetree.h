/**
 * @file codetree.h 声明codetree数据结构和接口
 */

#ifndef SRC_CODETREE_H_
#define SRC_CODETREE_H_

#include "kdtree.h"

typedef struct {
	kdtree_t*	tree;
	int*		invperm;
} codetree_t;



#endif /* SRC_CODETREE_H_ */

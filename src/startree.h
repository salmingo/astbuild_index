/**
 * @file startree.h 声明数据结构和接口
 */

#ifndef SRC_STARTREE_H_
#define SRC_STARTREE_H_

#include <stdint.h>
#include "kdtree.h"

typedef struct {
	kdtree_t*		tree;
//	qfits_header*	header;
	int*			inv_perm;
	uint8_t*		sweep;
	int				writting;
//	fitstable_t*	tagalong;
} startree_t;

#endif /* SRC_STARTREE_H_ */

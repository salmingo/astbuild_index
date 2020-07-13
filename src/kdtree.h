/**
 * @file kdtree.h 声明K-D树数据结构和接口
 */

#ifndef SRC_KDTREE_H_
#define SRC_KDTREE_H_

#include <stdint.h>

struct kdtree;
typedef struct kdtree	kdtree_t;

struct kdtree_qres;
typedef struct kdtree_qres kdtree_qres_t;

struct kdtree_funcs {
    void* (*get_data)(const kdtree_t* kd, int i);
    void  (*copy_data_double)(const kdtree_t* kd, int start, int N, double* dest);
    double (*get_splitval)(const kdtree_t* kd, int nodeid);
    int (*get_bboxes)(const kdtree_t* kd, int node, void* bblo, void* bbhi);

    int (*check)(const kdtree_t* kd);
    void (*fix_bounding_boxes)(kdtree_t* kd);

    void  (*nearest_neighbour_internal)(const kdtree_t* kd, const void* query, double* bestd2, int* pbest);
    kdtree_qres_t* (*rangesearch)(const kdtree_t* kd, kdtree_qres_t* res, const void* pt, double maxd2, int options);

    void (*nodes_contained)(const kdtree_t* kd,
                            const void* querylow, const void* queryhi,
                            void (*callback_contained)(const kdtree_t* kd, int node, void* extra),
                            void (*callback_overlap)(const kdtree_t* kd, int node, void* extra),
                            void* cb_extra);

    // a node was enqueued to be searched during nearest-neighbour.
    void (*nn_enqueue)(const kdtree_t* kd, int nodeid, int place);
    // a node was pruned during nearest-neighbour.
    void (*nn_prune)(const kdtree_t* kd, int nodeid, double d2, double bestd2, int place);
    // a node is being explored during nearest-neighbour.
    void (*nn_explore)(const kdtree_t* kd, int nodeid, double d2, double bestd2);
    // a point is being examined during nearest-neighbour.
    void (*nn_point)(const kdtree_t* kd, int nodeid, int pointindex);
    // a new best point has been found.
    void (*nn_new_best)(const kdtree_t* kd, int nodeid, int pointindex, double d2);
};
typedef struct kdtree_funcs kdtree_funcs;

struct kdtree {
	uint32_t	type;	// kd树类型
	int32_t*	lr;		// 叶节点
	uint32_t*	perm;	// Permutation index

	union {
		float*		f;
		double*		d;
		uint32_t*	u;
		uint16_t*	s;
		void*		any;
	} bb;
	int n_bb;

	union {
		float*		f;
		double*		d;
		uint32_t*	u;
		uint16_t*	s;
		void*		any;
	} split;
	uint8_t* 	splitdim;

	uint8_t		dimbits;
	uint32_t	dimmask;
	uint32_t	splitmask;

	union {
		float*		f;
		double*		d;
		uint32_t*	u;
		uint16_t*	s;
		void*		any;
	} data;
	int		free_data;

	double*	minval;
	double*	maxval;
	double	scale;
	double	invscale;

	int		ndata;
	int		ndim;
	int		nnodes;
	int		nbottom;
	int		ninterior;
	int		nlevels;
	int		has_linear_lr;

	char*	name;
	void*	io;

	struct kdtree_funcs funcs;
};

struct kdtree_qres {
    unsigned int	nres;
    unsigned int	capacity; /* Allocated size. */
    union {
        double*		d;
        float*		f;
        uint32_t*	u;
        uint16_t*	s;
        void*		any;
    } results;
    double*		sdists;  /* Squared distance from query point */
    uint32_t*	inds;    /* Indexes into original data set */
};

#endif /* SRC_KDTREE_H_ */

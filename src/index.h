/**
 * @file index.h 声明索引数据结构和接口
 */

#ifndef SRC_INDEX_H_
#define SRC_INDEX_H_

#include "codetree.h"
#include "quadfile.h"
#include "startree.h"

typedef struct {
	// 索引树关键/实际数据结构
	codetree_t*		codekd;
	quadfile_t*		quads;
	startree_t*		starkd;

//	anqfits_t*		fits;

	char*			idxfn;		/// file name
	char*			idxname;	/// metadata about the index
	int				idx_id;
	int				healpix;
	int				hpnside;
	double			idx_jitter;
	int				cut_nside;
	int				cut_nsweep;
	double			cut_dedup;
	char*			cut_band;
	int				cut_margin;

	bool			circle;
	bool			cx_less_than_dx;
	bool			meanx_less_than_half;

	double			idx_scale_upper;
	double			idx_scale_lower;

	int				dimquads;
	int				nstars;
	int				nquads;
} index_t;

#endif /* SRC_INDEX_H_ */

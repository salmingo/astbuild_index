/**
 * @file build_index.h 声明相关数据结构和接口函数
 */

#ifndef SRC_BUILD_INDEX_H_
#define SRC_BUILD_INDEX_H_

#include "codetree.h"
#include "quadfile.h"
#include "startree.h"

/*!
 * @struct index_param 生成索引文件的控制参数
 */
typedef struct {
	char pathcat[100];	/// 临时FITS星表路径
	int filter_band;	/// 滤光片波段. 0-4: BVgri; 5-7: JHK
	double jitter;	// 位置误差阈值; 量纲: arcsec

	// 均匀化
	double brightcut;
	int bighp;
	int bignside;
	int sweeps;
	int margin;
	int UNside;
	double dedup;

	// 天区?
	int Nside;

	// 天区大小, 量纲: 角分
	bool scanoccupied;
	int passes;
	int Nreuse, Nloosen;
	int dimquads;
	int indexid;
	double qlo, qhi;

	// 常规参数
	char output[200];	// 输出路径
	// 命令行参数
	int argc;
	char** argv;
} index_param;

typedef struct {
	// The actual components of an index.
	codetree_t* codekd;
	quadfile_t* quads;
	startree_t* starkd;
	// FITS file access
//    anqfits_t* fits;

	// filename
	char* indexfn;

	// Below here: metadata about the index.
	char* indexname;

	// Unique id for this index.
	int indexid;
	int healpix;
	int hpnside;

	// Jitter in the index, in arcseconds.
	double index_jitter;

	// cut-an parameters:
	int cutnside;
	int cutnsweep;
	double cutdedup;
	char* cutband;
	int cutmargin;

	// Does the index have the CIRCLE header - (codes live in the circle, not the box)?
	bool circle;
	// Does the index have the CX <= DX property
	bool cx_less_than_dx;
	// Does the index have the CX + DX <= 1/2 property
	bool meanx_less_than_half;

	// Limits of the size of quads in the index, in arcseconds.
	double index_scale_upper;
	double index_scale_lower;

	int dimquads;
	int nstars;
	int nquads;
} index_t;

// 初始化参数
void init_index_param(index_param& param);
/*!
 * @brief 构建索引文件
 * @param param    索引构建参数
 */
void build_index_files(index_param& param);
int build_index(index_param& p,
                index_t** p_index, const char* indexfn);
int merge_index(quadfile_t* quads, codetree_t* codekd, startree_t* starkd,
                const char* indexfn);

#endif /* SRC_BUILD_INDEX_H_ */

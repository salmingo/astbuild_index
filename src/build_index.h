/**
 * @file build_index.h 声明相关数据结构和接口函数
 */

#ifndef SRC_BUILD_INDEX_H_
#define SRC_BUILD_INDEX_H_

/*!
 * @struct index_param 生成索引文件的控制参数
 */
typedef struct {
	char pathcat[100];	/// 临时FITS星表路径
	int filband;		/// 滤光片波段. 1-5: BVgri; 6-8: JHK
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
	// 命令行参数
	int argc;
	char** argv;
} index_param;

// 初始化参数
void init_index_param(index_param& param);
/*!
 * @brief 构建索引文件
 * @param pathcat  星表根路径
 * @param fpout    输出文件路径
 * @param param    索引构建参数
 */
void build_index_files(const char* pathcat, const char* fpout, index_param& param);

#endif /* SRC_BUILD_INDEX_H_ */

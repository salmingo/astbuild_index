/**
 * @file build_index.cpp 定义相关接口函数
 */
#include <string.h>
#include "build_index.h"

void init_index_param(index_param& param) {
	memset(&param, 0, sizeof(index_param));
	param.filter_band = 1;
	param.jitter    = 1.0;
	param.sweeps	= 10;
	param.passes	= 16;
	param.Nreuse	= 8;
	param.Nloosen	= 20;
	param.dimquads	= 4;
	param.brightcut	= 0.1;
	param.bighp		= -1;
}

void build_index_files(const char* pathcat, const char* fpout, index_param& param) {
	// 构建临时星表
}

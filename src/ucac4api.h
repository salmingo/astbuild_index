/**
 * @file ucac4_to_fits.h 由UCAC4星表生成以二进制表格式存储的FITS文件
 */

#ifndef SRC_UCAC4API_H_
#define SRC_UCAC4API_H_

#include <string.h>
#include "build_index.h"
#include "cat_index.h"

#define MILLISEC		3600000		//< 1度=3600000毫角秒
#define MILLISEC360		1296000000	// 360度对应的毫角秒
#define UCAC4_UNIT		78			//< UCAC4每个条目的占用空间, 量纲: 字节

// UCAC4原始星表结构：
typedef struct ucac4_item {
    int	ra;			// 赤经, 量纲: mas; 坐标系: J2000               ***
    int	spd;		// 与南极点距离, 量纲: mas; 坐标系: J2000         ***
    short	magm;	// UCAC拟合星等, 量纲: millimag     -- 仪器星等
    short	maga;	// UCAC孔径测光星等, 量纲: millimag  -- 仪器星等
    char	sigmag;	// UCAC星等误差, 量纲: 0.01mag
    char	objt;	// 目标类型                                               ***
    char	cdf;	// 双星组合标记                                          ***
    char	sigra;	// 中心历元坐标系RA*cosDEC方向的统计误差
    char	sigdc;	// 中心历元坐标系DEC方向统计误差
    char	na1;	// 该星拍摄获得的CCD图像数量
    char	nu1;	// 标定该星使用的CCD图像数量
    char	cu1;	// 计算自行使用的星表数
    short	cepra;	// 平赤经中心历元， 减去1900后的值, 量纲: 0.01年
    short	cepdc;	// 平赤纬中心历元，减去1900后的值, 量纲: 0.01年
    short	pmrac;	// RA*cos(DEC)方向的自行, 量纲: 0.1mas/yr        ***
    short	pmdc;	// DEC方向的自行, 量纲: 0.1mas/yr                ***
    char	sigpmr;	// RA*cos(DEC)方向自行的误差, 量纲: 0.1mas/yr
    char	sigpmd;	// DEC方向自行的误差， 量纲: 0.1mas/yr
    int		pts_key;	// 2MASS星表中的编号
    short	j_m;		// J星等, 量纲: millimag                       ***
    short	h_m;		// H星等, 量纲: millimag                       ***
    short	k_m;		// K_s星等, 量纲: millimag                     ***
    char	icqflg[3];	// cc_flg * 10 + ph_qual_flg
    char	e2mpho[3];	// 2MASS星等误差, 量纲: 0.01mag                ***
    // 0: J
    // 1: H
    // 2: K_s
    short apasm[5];		// APASS星表中的星等, 量纲: millimag          ***
    char	apase[5];	// APASS星等误差, 量纲: 0.01mag           ***
                            // 0: B星等
                            // 1: V
                            // 2: g
                            // 3: r
                            // 4: i
    char	gcflg;		// Yale SPM星表, g-flag * 10 + c-flag
    int		icf;		// 数据来源标志
    char	leda;		// LEDA星系匹配标志
    char	x2m;		// 2MASS扩展源标记
    int		rnm;		// 星表唯一标示
    short	zn2;		// UCAC2天区编号
    int		rn2;		// 沿UCAC2天区运行记录编号

public:
    ucac4_item& operator=(ucac4_item &other) {
    	if (this != &other) memcpy(this, &other, sizeof(ucac4_item));
    	return *this;
    }
}* ucac4item_ptr;
///////////////////////////////////////////////////////////////////////////////
static const char* ucac4_band[] = {// UCAC4星表波段名称定义
//   0    1    2    3    4    5    6    7
	"B", "V", "g", "r", "i", "J", "H", "K"
};

/*!
 * @brief 由UCAC4星表构建临时FITS文件
 */
void build_ucac4_fits(const char* pathcat, index_param& param);
/*!
 * @brief 解析星表条目
 * @param band 滤光片波段索引
 */
void ucac4_resolve_item(char *buff, int band, CatStar& star);

#endif /* SRC_UCAC4API_H_ */

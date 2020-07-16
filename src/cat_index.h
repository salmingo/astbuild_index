/**
 * @file cat_index.h 声明用来生成索引的星表的数据结构和接口
 */

#ifndef SRC_CAT_INDEX_H_
#define SRC_CAT_INDEX_H_

typedef struct {
	int ra, spd;	//< 赤经; 南天极距离. 量纲: 毫角秒
	short mag;		//< 星等. 量纲: 毫星等
} CatStar;

#endif /* SRC_CAT_INDEX_H_ */

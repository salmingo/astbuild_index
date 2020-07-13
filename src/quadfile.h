/**
 * @file quadfile.h 声明数据结构和接口
 */

#ifndef SRC_QUADFILE_H_
#define SRC_QUADFILE_H_

#include <stdint.h>

typedef struct {
	unsigned int	numquads;
	unsigned int	numstars;
	int				dimquads;
	double			idx_scale_upper;
	double			idx_scale_lower;
	int				idx_id;
	int				healpix;
	int				hpnside;
//	fitsbin_t*		fb;
	uint32_t*		quad_array;
} quadfile_t;


#endif /* SRC_QUADFILE_H_ */

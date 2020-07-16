/**
 * @file ucac4_to_fits.cpp 由UCAC4星表生成以二进制表格式存储的FITS文件
 */

#include <stdio.h>
#include <vector>
#include "ucac4api.h"

using namespace std;

void build_ucac4_fits(const char* pathcat, index_param& param) {
	char filename[10];
	char filepath[100];
	char pathtmp[] = "/Users/lxm/Data/Temp/ucac4.tmp";
	int nbuf(1000), nread, nzone, nvalid, ntot, i, j;
	CatStar star;
	int band = param.filter_band;
	char *buff = new char[nbuf * UCAC4_UNIT];
	char *ptr;
	FILE *fpcat, *fptmp;
	short brightcut = short(param.brightcut * 1000.);	// 亮端截断

	ntot  = 0;
	fptmp = fopen(pathtmp, "wb");
	// 遍历文件, 将符合条件的数据写入临时文件
	for (j = 1; j <= 900; ++j) {
		sprintf (filename, "z%03d", j);
		sprintf (filepath, "%s/u4b/%s", pathcat, filename);
		printf ("Reading: %s\n", filename);

		if ((fpcat = fopen(filepath, "rb")) == NULL) {
			printf ("\t FAIL\n");
			break;
		}

		nzone = nvalid = 0;
		while ((nread = fread(buff, UCAC4_UNIT, nbuf, fpcat)) > 0) {
			nzone += nread;
			for (i = 0, ptr = buff; i < nread; ++i, ptr += UCAC4_UNIT) {
				ucac4_resolve_item(ptr, band, star);
				if (star.mag >= brightcut) {
					++nvalid;
					++ntot;
					fwrite (&star, sizeof(CatStar), 1, fptmp);
				}
			}
		}
		printf ("\t %i of %i are valid\n", nvalid, nzone);

		fclose(fpcat);
	}
	delete []buff;
	fflush(fptmp);
	printf ("\nTotal count: %i\n\n", ntot);

	fclose(fptmp);
	// 结束: 释放资源
	remove(pathtmp);
}

void ucac4_resolve_item(char *buff, int band, CatStar& star) {
	star.ra     = ((int*) buff)[0];
	star.spd    = ((int*) buff)[1];
	if (band < 5) star.mag = ((short*)(buff + 46))[band];
	else star.mag = ((short*)(buff + 34))[band - 5];
}

/**
 * @file ucac4_to_fits.cpp 由UCAC4星表生成以二进制表格式存储的FITS文件
 */

#include <stdio.h>
#include <vector>
#include "ucac4api.h"
#include "FITSHandler.hpp"

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

		while ((nread = fread(buff, UCAC4_UNIT, nbuf, fpcat)) > 0) {
			for (i = 0, ptr = buff; i < nread; ++i, ptr += UCAC4_UNIT) {
				ucac4_resolve_item(ptr, band, star);
				if (star.mag >= brightcut) {
					++ntot;
					fwrite (&star, sizeof(CatStar), 1, fptmp);
				}
			}
		}

		fclose(fpcat);
	}
	delete []buff;
	fflush(fptmp);
	fseek(fptmp, 0, SEEK_SET);

	// 写入FITS文件
	FITSHandler hfit;
	sprintf(param.pathcat, "%s/astindex-%d.fit", pathcat, param.indexid);
	if (!hfit(param.pathcat, 2)) {
		printf ("Failed to create temporary catalog index\n");
	}
	else {
		// HDU 1
		fits_create_img(hfit(), BYTE_IMG, 0, NULL, hfit.Status());
		fits_write_comment(hfit(),
				"simplified binary TYCHO2 catalog, just stores RA/DEC and Magnitude, expected to be used for celestial fix. "
				"including all-sky stars from tycho2 and its' supplementary catalog. "
				"RA is stored as 32-bit unsigned integer in milli arcsecs. "
				"DEC is transformed to distance from SOUTH Pole and then stored as 32-bit unsigned ingeter in milli arcsecs. "
				"Magnitude refers to V band from BT and VT, is stored as 16-bit integer in milli mag.",
				hfit.Status());
		fits_write_comment(hfit(),
				"RA is stored as 32-bit unsigned integer in milli arcsecs. ",
				hfit.Status());
		fits_write_comment(hfit(),
				"DEC is transformed to distance from SOUTH Pole and then stored as 32-bit unsigned ingeter in milli arcsecs. ",
				hfit.Status());
		fits_write_comment(hfit(),
				"Magnitude refers to V band from BT and VT, is stored as 16-bit integer in milli mag.",
				hfit.Status());

		// HDU 2
		char *ttype3[] = {"RA", "DEC", "Mag"};
		char *tform3[] = {"1J", "1J", "1I"};
		char *tunit3[] = {"mas", "mas", "millimag"};
		CatStar* stars = new CatStar[nbuf];
		uint32_t* ra  = new uint32_t[nbuf];
		uint32_t* spd = new uint32_t[nbuf];
		short* mag = new short[nbuf];
		int haswrite(1);

		fits_create_tbl(hfit(), BINARY_TBL, ntot, 3, ttype3, tform3, tunit3, NULL, hfit.Status());
		while ((nread = fread(stars, sizeof(CatStar), nbuf, fptmp)) > 0) {
			for (i = 0; i < nread; ++i) {
				ra[i]  = stars[i].ra;
				spd[i] = stars[i].spd;
				mag[i] = stars[i].mag;
			}

			fits_write_col(hfit(), TUINT, 1, haswrite, 1, nread, ra,  hfit.Status());
			fits_write_col(hfit(), TUINT, 2, haswrite, 1, nread, spd, hfit.Status());
			fits_write_col(hfit(), TUINT, 3, haswrite, 1, nread, mag, hfit.Status());
			haswrite += nread;
		}

		// 处理结果
		if (hfit.Success()) printf ("catalog UCAC4 is saved to [%s]\n", param.pathcat);
		else printf ("build_ucac4_fits() failed: %s\n", hfit.GetError());

		delete []stars;
		delete []ra;
		delete []spd;
		delete []mag;
	}

	// 结束: 释放资源
	fclose(fptmp);
	remove(pathtmp);
}

void ucac4_resolve_item(char *buff, int band, CatStar& star) {
	star.ra     = ((uint32_t*) buff)[0];
	star.spd    = ((uint32_t*) buff)[1];
	if (band < 5) star.mag = ((short*)(buff + 46))[band];
	else star.mag = ((short*)(buff + 34))[band - 5];
}

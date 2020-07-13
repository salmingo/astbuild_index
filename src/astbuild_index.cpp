/**
 * astbuild_index 为天文定位生成参考星表
 * - 由UCAC4生成参考星表
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "build_index.h"
#include "keywords.h"

///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief 显示使用说明
 */
void print_help(const char* progname) {
	printf ("\nUsage: %s\n\n"
			"    -h                   print help\n"
			"    -o <output-index>    output filename for index\n"
			"    (\n"
			"    -P <scale-number>    use 'preset' values for '-N', '-l' and '-u'\n"
			"        -P 0    should be good for images about 2 arcmin in size\n"
			"              and it goes in steps of sqrt(2), so:\n"
			"        -P 2    should work for images about 4 arcmin across\n"
			"        -P 4    should work for images about 8 arcmin across\n"
			"        -P 6    should work for images about 16 arcmin across\n"
			"        -P 8    should work for images about 30 arcmin across\n"
			"        -P 10   should work for images about 1 degree across\n"
			"        -P 12   should work for images about 2 degree across\n"
			"        -P 14   should work for images about 4 degree across\n"
			"        -P 16   should work for images about 8 degree across\n"
			"        -P 18   should work for images about 17 degree across\n"
			"         etc... upto -P 19\n"
			"  OR,\n"
			"    -N <nside>           healpix Nside for quad-building\n"
			"    -l <min-quad-size>   minimum quad size (arcminutes)\n"
			"    -u <max-quad-size>   maximum quad size (arcminutes)\n"
			"    )\n"
			"    [-b <band>]          filter band for magnitude. using alphabet or digit number.\n"
			"                       1-5 correspond to BVgri and 6-8 with JHK. default: V\n"
			"    [-B <val>]           the brightest mag value\n"
			"    [-U <Nside>]         healpix Nside for uniformization; default: same as -N\n"
			"    [-H <big healpix>]   default: all-sky\n"
			"    [-s <big healpix Nside]        default: 0\n"
			"    [-m <margin>]        add ad margin of <margin> healpixels; default: 0\n"
			"    [-n <sweeps>]        number of stars per fine healpix grid cell; default: 10\n"
			"    [-r <dedup-raidus>]  deduplication radius in arcsec; default: no deduplication\n"
			"    [-j <jitter>]        positional error of stars in the reference catalog (in arcsec; default: 1)\n"
			"    [-d <dimquads>]      number of stars in a \"quad\" (default: 4)\n"
			"    [-p <passes>]        number of rounds of quad-building (ie, #quads per healpix cell, default: 16)\n"
			"    [-R <reuse-times]    number of times a star can be used (default: 8)\n"
			"    [-L <max-reuses>]    make extra passes through the healpixes, increasing the \"-r\" reuse\n"
			"                       limit each time, up to \"max-reuses\"\n"
			"    [-E]                 scan through the catalog, checking which healpixes are occupied.\n"
			"    [-I <unique-id]      set the unique ID of this index\n"
			"\n",
			progname);
}

int main(int argc, char **argv) {
	index_param param;
	char *idxfn = NULL;	// index文件名称
	int preset = -100;

	init_index_param(param);
	/* 解析命令行参数 */
	const char optstr[] = "d:hj:l:m:n:o:p:r:s:u:B:EH:I:L:N:P:R:U:";
	int ch;

	while ((ch = getopt(argc, argv, optstr)) != -1) {
		switch(ch) {
		case 'h':
			print_help(argv[0]);
			return -1;
		case 'b':
			if (isdigit(optarg[0])) {
				param.filband = atoi(optarg);
			}
			else if (optarg[0] == 'B') param.filband = 1;
			else if (optarg[0] == 'V') param.filband = 2;
			else if (optarg[0] == 'g') param.filband = 3;
			else if (optarg[0] == 'r') param.filband = 4;
			else if (optarg[0] == 'i') param.filband = 5;
			else if (optarg[0] == 'J') param.filband = 6;
			else if (optarg[0] == 'H') param.filband = 7;
			else if (optarg[0] == 'K') param.filband = 8;
			else param.filband = 0;
			break;
		case 'd': param.dimquads = atoi(optarg);
			break;
		case 'j': param.jitter = atof(optarg);
			break;
		case 'l': param.qlo = atof(optarg);
			break;
		case 'm': param.margin = atoi(optarg);
			break;
		case 'n': param.sweeps = atoi(optarg);
			break;
		case 'o': idxfn = optarg;
			break;
		case 'p': param.passes = atoi(optarg);
			break;
		case 'r': param.dedup = atof(optarg);
			break;
		case 's': param.bignside = atoi(optarg);
			break;
		case 'u': param.qhi = atof(optarg);
			break;
		case 'B': param.brightcut = atof(optarg);
			break;
		case 'E': param.scanoccupied = true;
			break;
		case 'H': param.bighp = atoi(optarg);
			break;
		case 'I': param.indexid = atoi(optarg);
			break;
		case 'L': param.Nloosen = atoi(optarg);
			break;
		case 'N': param.Nside = atoi(optarg);
			break;
		case 'P': preset = atoi(optarg);
			break;
		case 'R': param.Nreuse = atoi(optarg);
			break;
		case 'U': param.UNside = atoi(optarg);
			break;
		default:
			break;
		}
	}

	if (param.filband < 1 || param.filband > 8) {
		printf ("wrong filter band %i exceeds ranged from %i to %i\n",
				param.filband, 1, 8);
		return -2;
	}
	if (!idxfn) {
		printf ("requires to specify outpu filename\n");
		print_help(argv[0]);
		return -3;
	}
	if (optind != argc) {
		print_help(argv[0]);
		printf ("\nExtra command-line args were given: ");
		for (int i = optind; i < argc; ++i) printf ("%s ", argv[i]);
		printf ("\n");
		return -4;
	}
	if (param.dimquads > DQMAX) {
		printf ("Quad dimension %i exceeds compiled-in max %i\n", param.dimquads, DQMAX);
		return -5;
	}
	if (preset > -100) {
		/* 存疑:
		 * - hpbase预设值如何确定
		 * - bignside的含义: Nside整除bignside
		 * - Nside的含义: 视场越大，Nside越小
		 * - dimquads的含义: 天区的维度?
		 */
		const int min_preset = -5;
		const double scales[] = {
			   0.35,   0.5,    0.7,   1.0,   1.4,   2.0,  2.8, 4.0, 5.6, 8.0, // 10
			  11.0,   16.0,   22.0,  30.0,  42.0,  60.0, 85.0,                // 7
			 120.0,  170.0,  240.0, 340.0, 480.0, 680.0,                      // 6
			1000.0, 1400.0, 2000.0                                            // 3
		};
		const double hpbase = 1760;
		double nside;
		int P = sizeof(scales) / sizeof(double) - 1;
		int max_preset = P + min_preset;
		int prei = preset - min_preset;
		if (preset >= max_preset) {
			printf ("Error: oonly presets %i through %i are defined\n", min_preset, max_preset - 1);
			return -6;
		}
		if (preset < min_preset) {
			printf ("Preset must be >= %i\n", min_preset);
			return -7;
		}
		param.qlo = scales[prei];
		param.qhi = scales[prei + 1];
		nside = hpbase * pow(1.0/sqrt(2.0), preset);
		printf ("nside = %g\n", nside);
		if (param.bignside)
			param.Nside = int(param.bignside * ceil((double) nside / param.bignside));
		else
			param.Nside = int(ceil(nside));
		printf ("Preset %i: quad scales %g to %g, Nside %i\n",
				preset, param.qlo, param.qhi, param.Nside);
	}
	param.argc = argc;
	param.argv = argv;

	build_index_files("/Users/lxm/Catalogue/UCAC4", idxfn, param);

	return 0;
}

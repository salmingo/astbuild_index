/**
 * astbuild_index 为天文定位生成参考星表
 */

#include <getopt.h>
#include <stdio.h>
#include <string>

using namespace std;

/*!
 * @brief 显示使用说明
 */
void Usage() {
	printf("Usage:\n"
		   " astbuild_index [options] <catalog_name.fit>\n"
		   "\nOptions\n"
		   " -h / --help    : print this help message\n"
		   " -d / --default : generate default configuration file here\n"
		   " -c / --config  : specify configuration file\n");
}

int main(int argc, char **argv) {
	/* 解析命令行参数 */
	struct option longopts[] = {
		{ "help",    no_argument,       NULL, 'h' },
		{ "config",  required_argument, NULL, 'c' },
		{ NULL,      0,                 NULL,  0  }
	};
	char optstr[] = "hdc:";
	int ch, optndx;

	while ((ch = getopt_long(argc, argv, optstr, longopts, &optndx)) != -1) {
		switch(ch) {
		case 'h':
			Usage();
			return -1;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!argc) {
		printf("requires FITS file which to be processed\n");
		return -3;
	}

	return 0;
}

/**
 * @file build_index.cpp 定义相关接口函数
 */

#include <string.h>
#include <stdio.h>
#include "build_index.h"
#include "bl.h"

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

int build_index(index_param& p, index_t** p_index, const char* indexfn) {
//	fitstable_t* uniform;

	// star kdtree
	startree_t* starkd = NULL;
//	fitstable_t* startag = NULL;

	// hpquads
//	codefile_t* codes = NULL;
	quadfile_t* quads = NULL;

	// codetree
	codetree_t* codekd = NULL;

	// unpermute-stars
	startree_t* starkd2 = NULL;
	quadfile_t* quads2 = NULL;
//	fitstable_t* startag2 = NULL;

	// unpermute-quads
	quadfile_t* quads3 = NULL;
	codetree_t* codekd2 = NULL;

	bl* tempfiles;
    char* unifn=NULL;
    char* skdtfn=NULL;
    char* quadfn=NULL;
    char* codefn=NULL;
    char* ckdtfn=NULL;
    char* skdt2fn=NULL;
    char* quad2fn=NULL;
    char* quad3fn=NULL;
    char* ckdt2fn=NULL;

    if (!p.UNside) p.UNside = p.Nside;

	return 0;
}

void build_index_files(index_param& param) {
	index_t* index;

	build_index(param, &index, NULL);
}

int merge_index(quadfile_t* quad, codetree_t* code, startree_t* star,
                const char* indexfn) {
    FILE* fout;
    fitstable_t* tag = NULL;

    fout = fopen(indexfn, "wb");
    if (!fout) {
        printf("Failed to open output file");
        return -1;
    }

    if (quadfile_write_header_to(quad, fout)) {
        printf("Failed to write quadfile header to index file %s", indexfn);
        return -1;
    }
    if (quadfile_write_all_quads_to(quad, fout)) {
        printf("Failed to write quads to index file %s", indexfn);
        return -1;
    }
    if (fits_pad_file(fout)) {
        printf("Failed to pad index file %s", indexfn);
        return -1;
    }

    if (codetree_append_to(code, fout)) {
        printf("Failed to write code kdtree to index file %s", indexfn);
        return -1;
    }
    if (fits_pad_file(fout)) {
        printf("Failed to pad index file %s", indexfn);
        return -1;
    }

    if (startree_append_to(star, fout)) {
        printf("Failed to write star kdtree to index file %s", indexfn);
        return -1;
    }
    if (fits_pad_file(fout)) {
        printf("Failed to pad index file %s", indexfn);
        return -1;
    }

    if (startree_has_tagalong(star))
        tag = startree_get_tagalong(star);
    if (tag) {
        if (fitstable_append_to(tag, fout)) {
            printf("Failed to write star kdtree tag-along data to index file %s", indexfn);
            return -1;
        }
        if (fits_pad_file(fout)) {
            printf("Failed to pad index file %s", indexfn);
            return -1;
        }
    }

    if (fclose(fout)) {
        printf("Failed to close index file %s", indexfn);
        return -1;
    }
    return 0;
}

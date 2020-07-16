/**
 * @file quadfile.cpp 定义quadfile接口
 */

#include "quadfile.h"

int  quadfile_write_header_to(quadfile_t* qf, FILE* fid) {
//    fitsbin_t* fb = qf->fb;
//    fitsbin_chunk_t* chunk = quads_chunk(qf);
//    qfits_header* hdr;
//    chunk->itemsize = qf->dimquads * sizeof(uint32_t);
//    chunk->nrows = qf->numquads;
//    hdr = fitsbin_get_primary_header(fb);
//    add_to_header(hdr, qf);
//
//    if (fitsbin_write_primary_header_to(fb, fid) ||
//        fitsbin_write_chunk_header_to(fb, chunk, fid)) {
//        ERROR("Failed to write quadfile header");
//        return -1;
//    }
    return 0;
}

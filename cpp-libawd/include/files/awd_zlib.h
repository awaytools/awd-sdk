#ifndef _LIBAWD_AWD_ZLIB_H
#define _LIBAWD_AWD_ZLIB_H
#ifdef USECOMPRESSION
#include <zlib.h>
voidpf awd_zalloc(voidpf, uInt, uInt);
void awd_zfree(voidpf, voidpf);
#endif
#endif
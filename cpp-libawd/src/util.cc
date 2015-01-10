#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include "Share.h."
#include "util.h"
#include "awd_types.h"

// Get mkstemp replacement
#include "platform.h"

awd_float64 *
awdutil_id_mtx4x4(awd_float64 *mtx)
{
    if (mtx == NULL) {
        mtx = (awd_float64*)malloc(16 * sizeof(awd_float64));
    }

    mtx[0]  = 1.0; mtx[1]  = 0.0; mtx[2]  = 0.0; mtx[3]  = 0.0;
    mtx[4]  = 0.0; mtx[5]  = 1.0; mtx[6]  = 0.0; mtx[7]  = 0.0;
    mtx[8]  = 0.0; mtx[9]  = 0.0; mtx[10] = 1.0; mtx[11] = 0.0;
    mtx[12] = 0.0; mtx[13] = 0.0; mtx[14] = 0.0; mtx[15] = 1.0;

    return mtx;
}

//TODO: Consider replacing with macro
size_t
awdutil_get_type_size(AWD_field_type type, bool wide_mtx)
{
    size_t elem_size;

    switch (type) {
        case AWD_FIELD_INT8:
        case AWD_FIELD_BOOL:
        case AWD_FIELD_UINT8:
            elem_size = sizeof(awd_int8);
            break;

        case AWD_FIELD_INT16:
        case AWD_FIELD_UINT16:
            elem_size = sizeof(awd_int16);
            break;

        case AWD_FIELD_INT32:
        case AWD_FIELD_UINT32:
        case AWD_FIELD_FLOAT32:
        case AWD_FIELD_COLOR:
        case AWD_FIELD_BADDR:
            elem_size = sizeof(awd_int32);
            break;

        case AWD_FIELD_FLOAT64:
            elem_size = sizeof(awd_float64);
            break;

        case AWD_FIELD_VECTOR2x1:
            elem_size = VEC2_SIZE(wide_mtx);
            break;

        case AWD_FIELD_VECTOR3x1:
            elem_size = VEC3_SIZE(wide_mtx);
            break;

        case AWD_FIELD_VECTOR4x1:
            elem_size = VEC4_SIZE(wide_mtx);
            break;

        case AWD_FIELD_MTX3x2:
            elem_size = MTX32_SIZE(wide_mtx);
            break;

        case AWD_FIELD_MTX3x3:
            elem_size = MTX33_SIZE(wide_mtx);
            break;

        case AWD_FIELD_MTX4x3:
            elem_size = MTX43_SIZE(wide_mtx);
            break;

        case AWD_FIELD_MTX4x4:
            elem_size = MTX44_SIZE(wide_mtx);
            break;

        case AWD_FIELD_STRING:
        case AWD_FIELD_BYTEARRAY:
            // Can't know
            elem_size = 0;
            break;
    }

    return elem_size;
}

awd_uint32
awdutil_write_floats(int fd, awd_float64 *list, int len, bool wide)
{
    int i;
    awd_uint32 bytes_written = 0;

    for (i=0; i<len; i++) {
        if (wide) {
            awd_float64 n;
            n = F64(list[i]);
            write(fd, &n, sizeof(awd_float64));
            bytes_written += sizeof(awd_float64);
        }
        else {
            awd_float32 n;
            n = F32((awd_float32)list[i]);
            write(fd, &n, sizeof(awd_float32));
            bytes_written += sizeof(awd_float32);
        }
    }

    return bytes_written;
}

awd_uint32
awdutil_write_varstr(int fd, const char *str, awd_uint16 str_len)
{
    awd_uint16 len_be;

    if (str != NULL) {
        len_be = UI16(str_len);
        write(fd, &len_be, sizeof(awd_uint16));
        write(fd, str, str_len);
    }
    else {
        len_be = 0;
        write(fd, &len_be, sizeof(awd_uint16));
    }

    return str_len + sizeof(awd_uint16);
}

awd_color
awdutil_float_color(double r, double g, double b, double a)
{
    return awdutil_int_color((int)(r*255), (int)(g*255), (int)(b*255), (int)(a*255));
}

awd_color
awdutil_int_color(int r, int g, int b, int a)
{
    return ((r&0xff)<<24) | ((g&0xff)<<16) | ((b&0xff)<<8) | (a&0xff);
}

int
awdutil_mktmp(char **path)
{
    int fd;
    int tpl_len;
    char *tmp_path;

#ifdef WIN32
    int ret;
    int path_len;

    tmp_path = (char*)malloc(TMPPATH_MAXLEN);

    tpl_len = (int) strlen(TMPFILE_TEMPLATE);
    path_len = GetTempPath(TMPPATH_MAXLEN, tmp_path);
    if (path_len==0 || (path_len+tpl_len) > TMPPATH_MAXLEN)
        return -1;

    // Concatenate path and template and null-terminate.
    //strncpy_s(tmp_path+path_len, tpl_len, TMPFILE_TEMPLATE, tpl_len);
    strncpy(tmp_path+path_len, TMPFILE_TEMPLATE, tpl_len);
    memset(tmp_path+path_len+tpl_len, 0, 1);

    ret = _mktemp_s(tmp_path, path_len+tpl_len+1);
    if (ret==0){
        int error = _sopen_s(&fd, tmp_path,  _O_CREAT|_O_TEMPORARY|_O_RDWR|_O_BINARY, _SH_DENYNO, _S_IWRITE);
        if (error!=0)
            fd=-1;
    }
    else fd = -1;

#else
    tpl_len = strlen(TMPFILE_TEMPLATE) + 1;
    tmp_path = (char *)malloc(tpl_len);
    strncpy_s(tmp_path, tpl_len, TMPFILE_TEMPLATE, tpl_len);

    fd = mkstemp(tmp_path);
#endif

    if (path)
        *path = tmp_path;
    else free(tmp_path);

    return fd;
}

awd_uint16
awdutil_swapui16(awd_uint16 n)
{
    union {
        awd_uint16 i;
        awd_uint8 b[2];
    } in, out;

    in.i = n;

    out.b[0] = in.b[1];
    out.b[1] = in.b[0];

    return out.i;
}

awd_uint32
awdutil_swapui32(awd_uint32 n)
{
    union {
        awd_uint32 i;
        awd_uint8 b[4];
    } in, out;

    in.i = n;
    out.b[0] = in.b[3];
    out.b[1] = in.b[2];
    out.b[2] = in.b[1];
    out.b[3] = in.b[0];

    return out.i;
}

awd_float32
awdutil_swapf32(awd_float32 n)
{
    union {
        awd_float32 f;
        awd_uint8 b[4];
    } in, out;

    in.f = n;
    out.b[0] = in.b[3];
    out.b[1] = in.b[2];
    out.b[2] = in.b[1];
    out.b[3] = in.b[0];

    return out.f;
}

awd_float64
awdutil_swapf64(awd_float64 n)
{
    union {
        awd_float64 f;
        awd_uint8 b[8];
    } in, out;

    in.f = n;
    out.b[0] = in.b[7];
    out.b[1] = in.b[6];
    out.b[2] = in.b[5];
    out.b[3] = in.b[4];
    out.b[4] = in.b[3];
    out.b[5] = in.b[2];
    out.b[6] = in.b[1];
    out.b[7] = in.b[0];

    return out.f;
}
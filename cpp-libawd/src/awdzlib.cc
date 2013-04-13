#include <stdlib.h>
#include "awdzlib.h"

voidpf
awd_zalloc(voidpf opaque, uInt items, uInt size)
{
    return malloc(items*size);
}


void
awd_zfree(voidpf opaque, voidpf addr)
{
    return free(addr);
}

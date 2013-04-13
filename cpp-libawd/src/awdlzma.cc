#include <stdlib.h>

void *
awd_SzAlloc(void *p, size_t size)
{
    p = p;
    return malloc(size);
}

void
awd_SzFree(void *p, void *address)
{
    p = p;
    free(address);
}


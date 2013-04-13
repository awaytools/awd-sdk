#ifndef PYAWD_BCACHE_H
#define PYAWD_BCACHE_H

#include <Python.h>
#include <awd/libawd.h>


typedef struct _cache_elem {
    PyObject *py_obj;
    AWDBlock *block;
    struct _cache_elem *next;
} pyawd_bcache_item;

typedef struct _bcache {
    pyawd_bcache_item *first_item;
    pyawd_bcache_item *last_item;
} pyawd_bcache;


void pyawd_bcache_init(pyawd_bcache *);
void pyawd_bcache_free(pyawd_bcache *);
void pyawd_bcache_add(pyawd_bcache *, PyObject *, AWDBlock *);
AWDBlock *pyawd_bcache_get(pyawd_bcache *, PyObject *);


#endif

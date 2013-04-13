#include <bcache.h>

void 
pyawd_bcache_init(pyawd_bcache *cache)
{
    cache->first_item = NULL;
    cache->last_item = NULL;
}


void
pyawd_bcache_free(pyawd_bcache *cache)
{
    // TODO: Implement this
}


void
pyawd_bcache_add(pyawd_bcache *cache, PyObject *py_obj, AWDBlock *block)
{
    pyawd_bcache_item *item;

    item = (pyawd_bcache_item *)malloc(sizeof(pyawd_bcache_item));
    item->py_obj = py_obj;
    item->block = block;
    item->next = NULL;

    if (cache->first_item == NULL) {
        cache->first_item = item;
    }
    else {
        cache->last_item->next = item;
    }

    cache->last_item = item;
}


AWDBlock *
pyawd_bcache_get(pyawd_bcache *cache, PyObject *py_obj)
{
    pyawd_bcache_item *cur;

    cur = cache->first_item;
    while (cur != NULL) {
        if (cur->py_obj == py_obj)
            return cur->block;

        cur = cur->next;
    }

    return NULL;
}


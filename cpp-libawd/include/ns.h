#ifndef _LIBAWD_NS_H
#define _LIBAWD_NS_H

#include "block.h"
#include "awd_types.h"

class AWDNamespace :
    public AWDBlock
{
    private:
        const char *uri;
        awd_uint16 uri_len;
        awd_nsid handle;

    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDNamespace(const char *, awd_uint16);
        ~AWDNamespace();

        const char *get_uri(int *);

        awd_nsid get_handle();
        void set_handle(awd_nsid handle);
};

#endif

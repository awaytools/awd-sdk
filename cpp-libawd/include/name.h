#ifndef _LIBAWD_NAME_H
#define _LIBAWD_NAME_H

#include "awd_types.h"

class AWDNamedElement
{
    private:
        awd_uint16 name_len;
        char *name;

    public:
        AWDNamedElement(const char *, awd_uint16);
        ~AWDNamedElement();

        char *get_name();
        void set_name(const char *, awd_uint16);
        awd_uint16 get_name_length();
};

#endif

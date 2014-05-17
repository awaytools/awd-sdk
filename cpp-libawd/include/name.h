#ifndef _LIBAWD_NAME_H
#define _LIBAWD_NAME_H

#include "awd_types.h"

class AWDNamedElement
{
    private:
        awd_uint16 name_len;
        char *name;
        awd_uint16 awdID_len;
        char *awdID;

        awd_uint16 targetAWD_len;
        char *targetAWD;

    public:
        AWDNamedElement(const char *, awd_uint16);
        ~AWDNamedElement();

        char *get_name();
        void set_name(const char *, awd_uint16);
        char *get_targetAWD();
        void set_targetAWD(const char *, awd_uint16);
        char *get_awdID();
        void set_awdID(const char *, awd_uint16);
        awd_uint16 get_name_length();
        awd_uint16 get_awdID_length();
        awd_uint16 get_targetAWD_length();
        char * generate_awdID();
};

#endif

#ifndef _LIBAWD_META_H
#define _LIBAWD_META_H

#include "block.h"
#include "attr.h"
#include "awd_types.h"

#define PROP_META_TIMESTAMP 1
#define PROP_META_ENCODER_NAME 2
#define PROP_META_ENCODER_VER 3
#define PROP_META_GENERATOR_NAME 4
#define PROP_META_GENERATOR_VER 5



class AWDMetaData :
    public AWDBlock,
    public AWDAttrElement
{
    protected:
        char *encoder_name;
        char *encoder_version;
        void prepare_write();
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDMetaData();
        char *generator_name;
        char *generator_version;

        void override_encoder_metadata(char *, char *);
};


#endif

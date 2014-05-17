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
    private:
        char * encoder_name;
        int encoder_name_len;
        char *encoder_version;
        int encoder_version_len;
        char * generator_name;
        int generator_name_len;
        char * generator_version;
        int generator_version_len;
    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDMetaData();
        ~AWDMetaData();

        void override_encoder_metadata(const char *, const char *);
        void set_generator_metadata(const char *, const char *);
};

#endif

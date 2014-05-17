#ifndef _LIBAWD_MESSAGE_H
#define _LIBAWD_MESSAGE_H

#include "block.h"
#include "awd_types.h"

#define PROP_META_TIMESTAMP 1
#define PROP_META_ENCODER_NAME 2
#define PROP_META_ENCODER_VER 3
#define PROP_META_GENERATOR_NAME 4
#define PROP_META_GENERATOR_VER 5

class AWDMessageBlock :
    public AWDBlock
{
    private:
        char * message;
        int message_len;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDMessageBlock(char * blockName, const char * warningMessage);
        ~AWDMessageBlock();
        char * get_message();
        int get_message_len();
};

#endif

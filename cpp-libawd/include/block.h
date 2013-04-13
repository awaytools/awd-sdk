#ifndef _LIBAWD_BLOCK_H
#define _LIBAWD_BLOCK_H

#include <stdlib.h>

//#include "awd.h"
#include "awd_types.h"

class AWDBlock
{
    private:
        awd_baddr addr;
        awd_uint8 flags;

    protected:
        AWD_block_type type;
        virtual void prepare_write();
        virtual awd_uint32 calc_body_length(bool)=0;
        virtual void write_body(int,bool)=0;

    public:
        AWDBlock(AWD_block_type);

        awd_baddr get_addr();
        AWD_block_type get_type();

        //virtual void add_dependencies(AWD *);

        size_t write_block(int, awd_baddr);
};

typedef struct _list_block
{
    AWDBlock *block;
    struct _list_block *next;
} list_block;

class AWDBlockList
{
    private:
        int num_blocks;

    public:
        list_block *first_block;
        list_block *last_block;

        AWDBlockList();
        ~AWDBlockList();

        bool append(AWDBlock *);
        void force_append(AWDBlock *);
        bool contains(AWDBlock *);

        int get_num_blocks();
};

class AWDBlockIterator
{
    private:
        AWDBlockList * list;
        list_block * cur_block;

    public:
        AWDBlockIterator(AWDBlockList *);
        AWDBlock * next();
        void reset();
};


/*
#include "attr.h"

class AWDAttrBlock :
    public AWDAttrElement,
    public AWDBlock
{
    public:
        //void add_dependencies(AWD *);
}

*/
#endif

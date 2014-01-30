#ifndef _LIBAWD_BLOCK_H
#define _LIBAWD_BLOCK_H

#include <stdlib.h>

//#include "awd.h"
#include "awd_types.h"
class BlockSettings
{
    private:
        // File header fields
        bool wideMatrix;
        bool wideGeom;
        bool wideProps;
        bool wideAttributes;
        double scale;

    public:
        BlockSettings(bool,bool, bool, bool, double);
        ~BlockSettings();
		
        bool get_wide_matrix();
        void set_wide_matrix(bool);
        bool get_wide_geom();
        void set_wide_geom(bool);
        bool get_wide_props();
        void set_wide_props(bool);
        bool get_wide_attributes();
        void set_wide_attributes(bool);
        double get_scale();
        void set_scale(double);
};

class AWDBlockList;
class AWDBlock
{
    private:
        awd_baddr addr;
        awd_uint8 flags;
		bool isValid;

    protected:
        AWD_block_type type;
        virtual void prepare_and_add_dependencies(AWDBlockList *);
        virtual awd_uint32 calc_body_length(BlockSettings *)=0;
        virtual void write_body(int, BlockSettings *)=0;

    public:
        AWDBlock(AWD_block_type);
		bool isExported;
		bool isExportedToFile;

        awd_baddr get_addr();
        AWD_block_type get_type();
        bool get_isValid();
        void set_isValid(bool);

        void prepare_and_add_with_dependencies(AWDBlockList *);

		size_t write_block(int, BlockSettings *);
};

typedef struct _list_block
{
    AWDBlock *block;
	int blockIdx;
    struct _list_block *next;
} list_block;


class AWDBlockList
{
    private:
        int num_blocks;
		bool weakReference;

    public:
        list_block *first_block;
        list_block *last_block;

        AWDBlockList(bool weakReference=true);
        ~AWDBlockList();

        bool append(AWDBlock *);
        void force_append(AWDBlock *);
        bool contains(AWDBlock *);
        bool replace(AWDBlock *, AWDBlock *oldBlock);
		
		AWDBlock* getByIndex(int);

        int get_num_blocks();
};

class AWDBlockIterator
{
    private:
        AWDBlockList * list;
        list_block * cur_block;

    public:
        AWDBlockIterator(AWDBlockList *);
		~AWDBlockIterator();
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

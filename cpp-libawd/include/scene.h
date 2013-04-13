#ifndef _LIBAWD_SCENE_H
#define _LIBAWD_SCENE_H

#include "name.h"
#include "block.h"
#include "awd_types.h"
#include "attr.h"


class AWDSceneBlock : 
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        awd_float64 * transform_mtx;
        AWDSceneBlock *parent;
        AWDBlockList *children;

    protected:
        void write_scene_common(int, bool);
        awd_uint32 calc_common_length(bool);

    public:
        AWDSceneBlock(AWD_block_type, const char *, awd_uint16, awd_float64 *);
        ~AWDSceneBlock();

        void set_transform(awd_float64 *);

        AWDBlock *get_parent();
        void set_parent(AWDSceneBlock *);

        void add_child(AWDSceneBlock *);
        void remove_child(AWDSceneBlock *);

        AWDBlockIterator *child_iter();
};


class AWDScene :
    public AWDSceneBlock
{
    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDScene(const char *, awd_uint16);
        ~AWDScene();
};


class AWDContainer :
    public AWDSceneBlock
{
    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDContainer(const char *, awd_uint16);
        ~AWDContainer();
};

#endif

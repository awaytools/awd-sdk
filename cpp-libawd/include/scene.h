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
        void write_scene_common(int, BlockSettings *);
        awd_uint32 calc_common_length(bool);

    public:
        AWDSceneBlock(AWD_block_type, const char *, awd_uint16, awd_float64 *);
        ~AWDSceneBlock();

        void set_transform(awd_float64 *);
        awd_float64 * get_transform();

        AWDBlock *get_parent();
        void set_parent(AWDSceneBlock *);
        bool isEmpty();

        void add_child(AWDSceneBlock *);
        void remove_child(AWDSceneBlock *);
        void make_children_invalide();

        AWDBlockIterator *child_iter();
};

class AWDScene :
    public AWDSceneBlock
{
    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int,BlockSettings *);

    public:
        AWDScene(const char *, awd_uint16);
        ~AWDScene();
};

class AWDContainer :
    public AWDSceneBlock
{
    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDContainer(const char *, awd_uint16);
        AWDContainer(const char *, awd_uint16, awd_float64 *);
        ~AWDContainer();
};

class AWDCommandBlock :
    public AWDSceneBlock
{
    private:
        awd_uint8 hasSceneInfos;
        AWDNumAttrList *commandProperties;

    protected:
        awd_uint32 calc_body_length( BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDCommandBlock(const char *, awd_uint16);
        ~AWDCommandBlock();
        void add_target_light(awd_baddr);
};

class AWDSkyBox :
    public AWDSceneBlock
{
    private:
        AWDBlock * cubeTex;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDSkyBox(const char *, awd_uint16);
        ~AWDSkyBox();

        void set_cube_tex(AWDBlock *);
        AWDBlock* get_cube_tex();
};
#endif

#ifndef _LIBAWD_VERTEXANIM_H
#define _LIBAWD_VERTEXANIM_H

#include "attr.h"
#include "name.h"
#include "block.h"
#include "anim.h"

class AWDVertexGeom
{
    private:
        unsigned int num_subs;
        AWDSubGeom * first_sub;
        AWDSubGeom * last_sub;

    public:
        AWDVertexGeom();
        ~AWDVertexGeom();

        unsigned int get_num_subs();
        AWDSubGeom *get_sub_at(unsigned int);
        void add_sub_mesh(AWDSubGeom *);
};

typedef struct _AWD_vertexanim_fr {
    AWDVertexGeom *pose;
    awd_uint16 duration;
    struct _AWD_vertexanim_fr *next;
} AWD_vertexanim_fr;

class AWDVertexAnimation : public AWDNamedElement,
    public AWDAttrElement, public AWDBlock, public AWDAnimationClipNode
{
    private:
        awd_uint16 num_frames;
        AWDBlock * targetGeo;

        _AWD_vertexanim_fr *first_frame;
        _AWD_vertexanim_fr *last_frame;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDVertexAnimation(const char *, awd_uint16, int, int, int, bool, const char *, bool, bool);
        ~AWDVertexAnimation();

        void append_duration_to_last_frame(awd_uint16);
        AWDVertexGeom * get_last_frame_geo();
        AWDBlock * get_targetGeo();
        void set_targetGeo(AWDBlock *);
        void set_next_frame_pose(AWDVertexGeom *, awd_uint16);
};

#endif

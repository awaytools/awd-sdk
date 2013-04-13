#ifndef _LIBAWD_SKELANIM_H
#define _LIBAWD_SKELANIM_H

#include "attr.h"
#include "name.h"
#include "block.h"


typedef struct _AWD_joint_tf {
    awd_float64 *transform_mtx;
    struct _AWD_joint_tf *next;
} AWD_joint_tf;


class AWDSkeletonPose : public AWDNamedElement,
    public AWDAttrElement, public AWDBlock
{
    private:
        awd_uint16 num_transforms;
        AWD_joint_tf *first_transform;
        AWD_joint_tf *last_transform;

    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDSkeletonPose(const char *, awd_uint16);
        ~AWDSkeletonPose();

        void set_next_transform(awd_float64 *);
};



typedef struct _AWD_skelanim_fr {
    AWDSkeletonPose *pose;
    awd_uint16 duration;
    struct _AWD_skelanim_fr *next;
} AWD_skelanim_fr;


class AWDSkeletonAnimation : public AWDNamedElement, 
    public AWDAttrElement, public AWDBlock
{
    private:
        awd_uint16 num_frames;

        AWD_skelanim_fr *first_frame;
        AWD_skelanim_fr *last_frame;

    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDSkeletonAnimation(const char *, awd_uint16);
        ~AWDSkeletonAnimation();

        void set_next_frame_pose(AWDSkeletonPose *, awd_uint16);
};

#endif

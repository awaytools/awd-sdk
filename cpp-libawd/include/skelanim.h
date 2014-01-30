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
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

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
        int start_frame;
        int end_frame;
        int skip_frame;		
        bool stitch_final;
        bool is_processed;

        char * sourceID;
		int sourceID_len;

        AWD_skelanim_fr *first_frame;
        AWD_skelanim_fr *last_frame;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *);

    public:
        AWDSkeletonAnimation(const char *, awd_uint16, int, int, int, bool, const char *);
        ~AWDSkeletonAnimation();
		
        bool get_is_processed();
        void set_is_processed(bool);
        int get_start_frame();
        int get_end_frame();
        int get_skip_frame();
        bool get_stitch_final();
        char * get_sourceID();
		void set_sourceID(const char *name, awd_uint16 name_len);
        void set_next_frame_pose(AWDSkeletonPose *, awd_uint16);
};

#endif

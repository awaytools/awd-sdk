#include <cstdio>
#include "uvanim.h"
#include "util.h"

// Get mkstemp replacement
#include "platform.h"

AWDUVAnimation::AWDUVAnimation(const char *name, awd_uint16 name_len, int start_frame, int end_frame, int skip_frame, bool stitch_final, const char * sourceID, bool loop, bool useTransforms) :
    AWDNamedElement(name, name_len),
    AWDAttrElement(),
    AWDBlock(UV_ANIM),
    AWDAnimationClipNode(start_frame, end_frame, skip_frame, stitch_final, sourceID, loop, useTransforms)
{
    this->num_frames = 0;
    this->first_frame = NULL;
    this->last_frame = NULL;
}

AWDUVAnimation::~AWDUVAnimation()
{
    AWD_uvanim_fr *cur;

    cur = this->first_frame;
    while (cur) {
        AWD_uvanim_fr *next = cur->next;
        cur->next = NULL;
        if (cur->transform_mtx) {
            free(cur->transform_mtx);
            cur->transform_mtx = NULL;
        }

        free(cur);
        cur = next;
    }

    this->num_frames = 0;
    this->first_frame = NULL;
    this->last_frame = NULL;
}

void
AWDUVAnimation::append_duration_to_last_frame(awd_uint16 duration)
{
    AWD_uvanim_fr * frame = this->first_frame;
    // at this time we have to be shure, that all the VertexGeometry have the same number of subGeos (num_subMeshes)
    while (frame) {
        AWD_uvanim_fr * nextFrame = frame->next;
        if(nextFrame==NULL){
            frame->duration=(frame->duration+duration);
            return;
        }
        frame = frame->next;
    }
    return;
}

void
AWDUVAnimation::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    this->add_bool_property(1,this->get_loop(),true);
    this->add_bool_property(2,this->get_stitch_final(),false);
}
awd_uint32
AWDUVAnimation::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    int blockLength=2 + this->get_name_length(); //name
    blockLength+=2; //numframes
    blockLength+=(this->num_frames * (sizeof(awd_uint16) + MTX32_SIZE(curBlockSettings->get_wide_matrix())));
    blockLength+=this->calc_attr_length(true,true, curBlockSettings);
    return blockLength;
}

void
AWDUVAnimation::write_body(int fd, BlockSettings * curBlockSettings)
{
    AWD_uvanim_fr *cur_fr;

    awd_uint16 num_frames;
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    num_frames = UI16(this->num_frames);
    write(fd, &num_frames, sizeof(awd_uint16));

    this->properties->write_attributes(fd, curBlockSettings);

    cur_fr = this->first_frame;
    while (cur_fr) {
        awd_uint16 dur_be = UI16(cur_fr->duration);

        awdutil_write_floats(fd, cur_fr->transform_mtx, 6, curBlockSettings->get_wide_matrix());
        write(fd, &dur_be, sizeof(awd_uint16));

        cur_fr = cur_fr->next;
    }

    this->user_attributes->write_attributes(fd, curBlockSettings);
}

void
AWDUVAnimation::set_next_frame_tf(awd_float64 *mtx, awd_uint16 duration)
{
    AWD_uvanim_fr *frame = (AWD_uvanim_fr *)malloc(sizeof(AWD_uvanim_fr));
    frame->transform_mtx = mtx;
    frame->duration = duration;

    if (this->first_frame == NULL) {
        this->first_frame = frame;
    }
    else {
        this->last_frame->next = frame;
    }

    this->num_frames++;

    this->last_frame = frame;
    this->last_frame->next = NULL;
}
awd_float64 *
AWDUVAnimation::get_last_frame_tf()
{
    AWD_uvanim_fr * frame = this->first_frame;
    while (frame) {
        AWD_uvanim_fr * nextFrame = frame->next;
        if(nextFrame==NULL)
            return frame->transform_mtx;
        frame = frame->next;
    }
    return NULL;
}
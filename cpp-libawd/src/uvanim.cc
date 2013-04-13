#include <cstdio>
#include "uvanim.h"
#include "util.h"

// Get mkstemp replacement
#include "platform.h"


AWDUVAnimation::AWDUVAnimation(const char *name, awd_uint16 name_len) :
    AWDNamedElement(name, name_len),
    AWDAttrElement(),
    AWDBlock(UV_ANIM)
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


awd_uint32
AWDUVAnimation::calc_body_length(bool wide_mtx)
{
    return 2 + this->get_name_length() + 2 + 
        (this->num_frames * (sizeof(awd_uint16) + MTX32_SIZE(wide_mtx))) + 
        this->calc_attr_length(true,true, wide_mtx);
}


void
AWDUVAnimation::write_body(int fd, bool wide_mtx)
{
    AWD_uvanim_fr *cur_fr;

    awd_uint16 num_frames;
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    num_frames = UI16(this->num_frames);
    write(fd, &num_frames, sizeof(awd_uint16));

    this->properties->write_attributes(fd, wide_mtx);

    cur_fr = this->first_frame;
    while (cur_fr) {
        awd_uint16 dur_be = UI16(cur_fr->duration);

        awdutil_write_floats(fd, cur_fr->transform_mtx, 12, wide_mtx);
        write(fd, &dur_be, sizeof(awd_uint16));

        cur_fr = cur_fr->next;
    }

    this->user_attributes->write_attributes(fd, wide_mtx);
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

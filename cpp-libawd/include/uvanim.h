#ifndef _LIBAWD_UVANIM_H
#define _LIBAWD_UVANIM_H

#include "attr.h"
#include "name.h"
#include "block.h"

typedef struct _AWD_uvanim_fr {
    awd_float64 *transform_mtx;
    awd_uint16 duration;
    struct _AWD_uvanim_fr *next;
} AWD_uvanim_fr;

class AWDUVAnimation :
    public AWDNamedElement,
    public AWDAttrElement,
    public AWDBlock,
    public AWDAnimationClipNode
{
    private:
        awd_uint16 num_frames;
        AWDBlock * targetGeo;

        AWD_uvanim_fr *first_frame;
        AWD_uvanim_fr *last_frame;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDUVAnimation(const char *, awd_uint16, int, int, int, bool, const char * , bool ,bool);
        ~AWDUVAnimation();

        void append_duration_to_last_frame(awd_uint16);
        awd_float64 *get_last_frame_tf();
        void set_next_frame_tf(awd_float64 *, awd_uint16);
};

#endif

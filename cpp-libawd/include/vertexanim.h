#ifndef _LIBAWD_VERTEXANIM_H
#define _LIBAWD_VERTEXANIM_H

#include "attr.h"
#include "name.h"
#include "block.h"

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
    public AWDAttrElement, public AWDBlock
{
    private:
        awd_uint16 num_frames;
        int start_frame;
        int end_frame;
        int skip_frame;		
        bool stitch_final;
        bool is_processed;
		AWDBlock * targetGeo;
        char * sourceID;
		int sourceID_len;

        _AWD_vertexanim_fr *first_frame;
        _AWD_vertexanim_fr *last_frame;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);
		void prepare_and_add_dependencies(AWDBlockList *);

    public:
        AWDVertexAnimation(const char *, awd_uint16, int, int, int, bool, const char * );
        ~AWDVertexAnimation();
		
        AWDBlock * get_targetGeo();
        void set_targetGeo(AWDBlock *);
        bool get_is_processed();
        void set_is_processed(bool);
        int get_start_frame();
        int get_end_frame();
        int get_skip_frame();
        bool get_stitch_final();
		char * get_sourceID();
		void set_sourceID(const char *name, awd_uint16 name_len);
		void set_next_frame_pose(AWDVertexGeom *, awd_uint16);
};

#endif

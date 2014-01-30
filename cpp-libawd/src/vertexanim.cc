#include "util.h"
#include "vertexanim.h"

#include "platform.h"

AWDVertexGeom::AWDVertexGeom()
{
    this->first_sub = NULL;
    this->last_sub = NULL;
    this->num_subs = 0;
}

AWDVertexGeom::~AWDVertexGeom()
{
    AWDSubGeom *cur;

    cur = this->first_sub;
    while (cur) {
        AWDSubGeom *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }
    this->first_sub = NULL;
    this->last_sub = NULL;
}

void 
AWDVertexGeom::add_sub_mesh(AWDSubGeom *sub)
{
    if (this->first_sub == NULL) {
        this->first_sub = sub;
    }
    else {
        this->last_sub->next = sub;
    }
    
    this->num_subs++;
    this->last_sub = sub;
}


unsigned int
AWDVertexGeom::get_num_subs()
{
    return this->num_subs;
}


AWDSubGeom *
AWDVertexGeom::get_sub_at(unsigned int idx)
{
    if (idx < this->num_subs) {
        unsigned int cur_idx;
        AWDSubGeom *cur;

        cur_idx = 0;
        cur = this->first_sub;
        while (cur) {
            if (cur_idx == idx)
                return cur;

            cur_idx++;
            cur = cur->next;
        }
    }

    return NULL;
}



AWDVertexAnimation::AWDVertexAnimation(const char *name, awd_uint16 name_len, int start_frame, int end_frame, int skip_frame, bool stitch_final, const char * sourceID) :
    AWDNamedElement(name, name_len), AWDAttrElement(), AWDBlock(VERTEX_ANIM)
{
    this->num_frames = 0;
    this->first_frame = NULL;
    this->last_frame = NULL;
	this->start_frame = start_frame;
	this->end_frame = end_frame;
	this->skip_frame = skip_frame;
	this->stitch_final = stitch_final;
	this->sourceID_len=0;
	this->set_sourceID(sourceID, strlen(sourceID));
	this->is_processed=false;
	this->targetGeo=NULL;
}


AWDVertexAnimation::~AWDVertexAnimation()
{
    AWD_vertexanim_fr *cur;

    cur = this->first_frame;
    while (cur) {
        AWD_vertexanim_fr *next = cur->next;
        cur->next = NULL;
        delete cur->pose;
        free(cur);
        cur = next;
    }
	if(this->sourceID_len>0){
		free(this->sourceID);
		this->sourceID_len=0;
	}
    this->first_frame = NULL;
    this->last_frame = NULL;
	this->targetGeo = NULL;
}

char *
AWDVertexAnimation::get_sourceID()
{
    return this->sourceID;
}

void
AWDVertexAnimation::set_sourceID(const char *name, awd_uint16 name_len)
{
	if(this->sourceID_len>0)
		free(this->sourceID);
    this->sourceID_len = name_len;
    if (name != NULL) {
        this->sourceID = (char*)malloc(this->sourceID_len+1);
        strncpy(this->sourceID, name, this->sourceID_len);
        this->sourceID[this->sourceID_len] = 0;
    }
}

AWDBlock *
AWDVertexAnimation::get_targetGeo()
{
	return this->targetGeo;
}

void
AWDVertexAnimation::set_targetGeo(AWDBlock * targetGeo)
{
    this->targetGeo = targetGeo;
}
bool
AWDVertexAnimation::get_is_processed()
{
	return this->is_processed;
}

void
AWDVertexAnimation::set_is_processed(bool isProcessed)
{
    this->is_processed = isProcessed;
}

int
AWDVertexAnimation::get_start_frame()
{
	return this->start_frame;
}
int
AWDVertexAnimation::get_end_frame()
{
	return this->end_frame;
}
int
AWDVertexAnimation::get_skip_frame()
{
	return this->skip_frame;
}
bool
AWDVertexAnimation::get_stitch_final()
{
	return this->stitch_final;
}

void
AWDVertexAnimation::set_next_frame_pose(AWDVertexGeom *pose, awd_uint16 duration)
{
    AWD_vertexanim_fr *fr;

    fr = (AWD_vertexanim_fr *)malloc(sizeof(AWD_vertexanim_fr));
    fr->pose = pose;
    fr->duration = duration;

    if (this->first_frame == NULL) {
        this->first_frame = fr;
    }
    else {
        this->last_frame->next = fr;
    }

    this->num_frames++;
    this->last_frame = fr;
    this->last_frame->next = NULL;
}

void
AWDVertexAnimation::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->targetGeo != NULL)
		this->targetGeo->prepare_and_add_with_dependencies(export_list);	
}
awd_uint32
AWDVertexAnimation::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len=0;
	

    int streamCnt=1;//streamcnt is allways 1 for now

	len = sizeof(awd_uint16) + this->get_name_length();              // Name varstr
    len += sizeof(awd_uint32);                                      // geo addr 
    len += sizeof(awd_uint16);                                      // num frames
    len += sizeof(awd_uint16);                                      // num submeshes 
    len += sizeof(awd_uint16);                                      // num streams per submesh
    len += sizeof(awd_uint16) * streamCnt;                               // streamtypes
	AWD_vertexanim_fr * frame = this->first_frame;
	AWDVertexGeom * framegeom = NULL;
	int numSubs=0;
	if(frame!=NULL){
		framegeom=frame->pose;
		if(framegeom!=NULL){
			numSubs=framegeom->get_num_subs();
		}
	}
	if (numSubs>0){
		int lenSubmesh = 0;
		for (int i = 0; i<numSubs; i++){
			lenSubmesh += sizeof(awd_uint32) + frame->pose->get_sub_at(i)->calc_animations_streams_length();
		}
		len += (this->num_frames * (lenSubmesh + sizeof(awd_uint16))); // for each frame: datalength + length + duration
		len += this->calc_attr_length(true,true, curBlockSettings->get_wide_matrix());             // Props and attributes
	}
    return len;
}


void
AWDVertexAnimation::write_body(int fd, BlockSettings * curBlockSettings)
{
	
    AWD_vertexanim_fr *frame;
	AWDSubGeom *sub;
    awd_uint16 num_frames_be;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    awd_uint32 geom_addr = 0;
    if (this->targetGeo != NULL)
        geom_addr = UI32(this->targetGeo->get_addr());
    write(fd, &geom_addr, sizeof(awd_uint32));
	
    frame = this->first_frame;

    num_frames_be = UI16(this->num_frames);
    write(fd, &num_frames_be, sizeof(awd_uint16));

    awd_uint16 num_subMeshes = UI16(frame->pose->get_num_subs());
    write(fd, &num_subMeshes, sizeof(awd_uint16));

    awd_uint16 streamsPerSubMesh = 1;// for now only one stream = vertexPosition
    write(fd, &streamsPerSubMesh, sizeof(awd_uint16));

    awd_uint16 streamDataType = 1;//this is so we can change the style of parsing later 
    write(fd, &streamDataType, sizeof(awd_uint16));
	
    this->properties->write_attributes(fd, curBlockSettings->get_wide_matrix());

	// at this time we have to be shure, that all the VertexGeometry have the same number of subGeos (num_subMeshes)
    while (frame) {
        awd_uint16 dur_be = UI16(frame->duration);
        write(fd, &dur_be, sizeof(awd_uint16));
		for (int i = 0; i<num_subMeshes ; i++){
			sub=frame->pose->get_sub_at(i);
			awd_uint32 s_m_length = UI32(sub->calc_animations_streams_length());
			write(fd, &s_m_length, sizeof(awd_uint32));
			sub->write_anim_sub(fd, curBlockSettings->get_wide_matrix(), curBlockSettings->get_scale());
		}
        frame = frame->next;
    }
    this->user_attributes->write_attributes(fd, curBlockSettings->get_wide_matrix());
	
}

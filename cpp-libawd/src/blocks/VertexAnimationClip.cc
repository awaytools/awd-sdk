#include "blocks/vertex_anim_clip.h"

#include "blocks/scene.h"
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_frame.h"
#include "base/anim_clip.h"
#include "elements/subgeometry.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

VertexAnimationClip::VertexAnimationClip(std::string& name) :
	AttrElementBase(), 
	AWDBlock(block_type::VERTEX_ANIM, name), 
	AnimationClipBase()
{
}
VertexAnimationClip::VertexAnimationClip():
	AttrElementBase(), 
	AWDBlock(block_type::VERTEX_ANIM), 
	AnimationClipBase()
{
}
VertexAnimationClip::~VertexAnimationClip()
{
	/*
	AWD_vertexanim_fr *cur;

	cur = this->first_frame;
	while (cur) {
		AWD_vertexanim_fr *next = cur->next;
		cur->next = NULL;
		delete cur->pose;
		free(cur);
		cur = next;
	}
	this->targetGeo = NULL;
	*/
}

result VertexAnimationClip::create_pose_blocks()
{
	return result::AWD_SUCCESS;
}

AWDBlock *
VertexAnimationClip::get_targetGeo()
{
	return this->targetGeo;
}

TYPES::state VertexAnimationClip::validate_block_state()
{
	return TYPES::state::VALID;
}
void
VertexAnimationClip::set_targetGeo(AWDBlock * targetGeo)
{
	this->targetGeo = targetGeo;
}

VertexPose *
VertexAnimationClip::get_last_frame_geo()
{
	/*
	AWD_vertexanim_fr * frame = this->first_frame;
	while (frame) {
		AWD_vertexanim_fr * nextFrame = frame->next;
		if(nextFrame==NULL)
			return frame->pose;
		frame = frame->next;
	}
	*/
	return NULL;
}
void
VertexAnimationClip::append_duration_to_last_frame(TYPES::UINT16 duration)
{
	/*
	AWD_vertexanim_fr * frame = this->first_frame;
	// at this time we have to be shure, that all the VertexGeometry have the same number of subGeos (num_subMeshes)
	while (frame) {
		AWD_vertexanim_fr * nextFrame = frame->next;
		if(nextFrame==NULL){
			frame->duration=(frame->duration+duration);
			return;
		}
		frame = frame->next;
	}
	*/
	return;
}
void
	VertexAnimationClip::set_next_frame_pose(VertexPose *pose, TYPES::UINT16 duration)
{
	/*
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
	*/
}

result
VertexAnimationClip::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	if (this->targetGeo != NULL)
		this->targetGeo->add_with_dependencies(target_file, instance_type);
//	this->add_bool_property(1,this->get_loop(),true);
//	this->add_bool_property(2,this->get_stitch_final(),false);
	return result::AWD_SUCCESS;
}
TYPES::UINT32
VertexAnimationClip::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len=0;

	int streamCnt=1;//streamcnt is allways 1 for now

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); // Name varstr
	len += sizeof(TYPES::UINT32);                                      // geo addr
	len += sizeof(TYPES::UINT16);                                      // num frames
	len += sizeof(TYPES::UINT16);                                      // num submeshes
	len += sizeof(TYPES::UINT16);                                      // num streams per submesh
	len += sizeof(TYPES::UINT16) * streamCnt;                               // streamtypes
	/*AWD_vertexanim_fr * frame = this->first_frame;
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
			lenSubmesh += sizeof(TYPES::UINT32) + frame->pose->get_sub_at(i)->calc_animations_streams_length();
		}
		len += (this->num_frames * (lenSubmesh + sizeof(TYPES::UINT16))); // for each frame: datalength + length + duration
		len += this->calc_attr_length(true,true, settings);             // Props and attributes
	}
	*/
	return len;
}

result
VertexAnimationClip::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
{
	/*
	AWD_vertexanim_fr *frame;
	SubGeomInternal *sub;

	fileWriter->writeSTRING(this->get_name(), 1);

	TYPES::UINT32 geom_addr = 0;
	if (this->targetGeo != NULL)
		geom_addr = this->targetGeo->get_addr();
	fileWriter->writeUINT32(geom_addr);

	frame = this->first_frame;

	fileWriter->writeUINT16(this->num_frames);

	fileWriter->writeUINT16(frame->pose->get_num_subs());

	TYPES::UINT16 streamsPerSubMesh = 1;// for now only one stream = vertexPosition
	fileWriter->writeUINT16(streamsPerSubMesh);

	TYPES::UINT16 streamDataType = 1;//this is so we can change the style of parsing later
	fileWriter->writeUINT16(streamDataType);

	this->properties->write_attributes(fileWriter, settings);

	// at this time we have to be shure, that all the VertexGeometry have the same number of subGeos (num_subMeshes)
	while (frame) {
		fileWriter->writeUINT16(frame->duration);
		for (int i = 0; i<frame->pose->get_num_subs() ; i++){
			sub=frame->pose->get_sub_at(i);
			fileWriter->writeUINT32(sub->calc_animations_streams_length());
			sub->write_anim_sub(fileWriter, settings->get_wide_matrix(), settings->get_scale());
		}
		frame = frame->next;
	}
	*/
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

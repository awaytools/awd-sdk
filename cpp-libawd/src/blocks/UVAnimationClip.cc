#include "blocks/uv_anim_clip.h"
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_clip.h"
#include "base/anim_frame.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

UVAnimationClip::UVAnimationClip(std::string& name) :
	AttrElementBase(),
	AWDBlock(block_type::UV_ANIM, name),
	AnimationClipBase()
{
}

UVAnimationClip::UVAnimationClip():
	AttrElementBase(), 
	AWDBlock(block_type::UV_ANIM), 
	AnimationClipBase()
{
}
UVAnimationClip::~UVAnimationClip()
{
	/*
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
	*/
}

result UVAnimationClip::create_pose_blocks()
{
	return result::AWD_SUCCESS;
}

TYPES::state UVAnimationClip::validate_block_state()
{
	return TYPES::state::VALID;
}

void
UVAnimationClip::append_duration_to_last_frame(TYPES::UINT16 duration)
{
	/*
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
	*/
}

result
UVAnimationClip::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
//	this->add_bool_property(1,this->get_loop(),true);
//	this->add_bool_property(2,this->get_stitch_final(),false);
	return result::AWD_SUCCESS;
}
TYPES::UINT32
UVAnimationClip::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	int blockLength=int(2 + this->get_name().size()); //name
	blockLength+=2; //numframes
	//blockLength+=(this->num_frames * (sizeof(TYPES::UINT16) + MTX32_SIZE(settings->get_wide_matrix())));
	blockLength+=this->calc_attr_length(true,true, settings);
	return blockLength;
}

result
UVAnimationClip::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
{
	/*AWD_uvanim_fr *cur_fr;

	TYPES::UINT16 num_frames;
	fileWriter->writeSTRING(this->get_name(), 1);

	num_frames = this->num_frames;
	fileWriter->writeUINT16(num_frames);

	this->properties->write_attributes(fileWriter, settings);

	cur_fr = this->first_frame;
	while (cur_fr) {
		TYPES::UINT16 dur_be = cur_fr->duration;
		fileWriter->writeNumbers(cur_fr->transform_mtx, 6, settings->get_wide_matrix());
		fileWriter->writeUINT16(dur_be);

		cur_fr = cur_fr->next;
	}
	*/
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

void
UVAnimationClip::set_next_frame_tf(TYPES::F64 *mtx, TYPES::UINT16 duration)
{
	/*AWD_uvanim_fr *frame = (AWD_uvanim_fr *)malloc(sizeof(AWD_uvanim_fr));
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
	this->last_frame->next = NULL;*/
}
TYPES::F64 *
UVAnimationClip::get_last_frame_tf()
{
	/*AWD_uvanim_fr * frame = this->first_frame;
	while (frame) {
		AWD_uvanim_fr * nextFrame = frame->next;
		if(nextFrame==NULL)
			return frame->transform_mtx;
		frame = frame->next;
	}*/
	return NULL;
}

#include "blocks/skeleton_anim_clip.h"

#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"
#include "base/anim_frame.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

SkeletonAnimationClip::SkeletonAnimationClip(std::string& name) :	 
	AttrElementBase(), 
	AWDBlock(block_type::SKELETON_ANIM, name), 
	AnimationClipBase()
{
}
SkeletonAnimationClip::SkeletonAnimationClip():
	AttrElementBase(), 
	AWDBlock(block_type::SKELETON_ANIM), 
	AnimationClipBase()
{
}

SkeletonAnimationClip::~SkeletonAnimationClip()
{
	/*
	AWD_skelanim_fr *cur;

	cur = this->first_frame;
	while (cur) {
		AWD_skelanim_fr *next = cur->next;
		cur->next = NULL;
		delete cur->pose;
		free(cur);
		cur = next;
	}
	*/
}

result SkeletonAnimationClip::create_pose_blocks()
{
	return result::AWD_SUCCESS;
}

TYPES::state SkeletonAnimationClip::validate_block_state()
{
	return TYPES::state::VALID;
}
SkeletonPose *
SkeletonAnimationClip::get_last_frame()
{
	/*
	AWD_skelanim_fr * frame = this->first_frame;
	while (frame) {
		AWD_skelanim_fr * nextFrame = frame->next;
		if(nextFrame==NULL)
			return frame->pose;
		frame = frame->next;
	}
	*/
	return NULL;
}
void
SkeletonAnimationClip::append_duration_to_last_frame(TYPES::UINT16 duration)
{
	/*
	AWD_skelanim_fr * frame = this->first_frame;
	// at this time we have to be shure, that all the VertexGeometry have the same number of subGeos (num_subMeshes)
	while (frame) {
		AWD_skelanim_fr * nextFrame = frame->next;
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
SkeletonAnimationClip::set_next_frame_pose(SkeletonPose *pose, TYPES::UINT16 duration)
{
	/*
	AWD_skelanim_fr *fr;

	fr = (AWD_skelanim_fr *)malloc(sizeof(AWD_skelanim_fr));
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
SkeletonAnimationClip::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	/*
	AWD_skelanim_fr *frame;
	frame = this->first_frame;
	while (frame) {
		frame->pose->add_with_dependencies(target_file, instance_type);
		frame = frame->next;
	}
	*/
	return result::AWD_SUCCESS;
}

TYPES::UINT32
SkeletonAnimationClip::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	TYPES::UINT32 len;
	TYPES::UINT8 pose_len;

	pose_len = sizeof(BADDR)+sizeof(TYPES::UINT16);

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size());             // Name varstr
	len += sizeof(TYPES::UINT16);                                      // num frames
	//len += (this->num_frames * pose_len);                           // Pose list
	len += this->calc_attr_length(true,true, settings);             // Props and attributes

	return len;
}

result
SkeletonAnimationClip::write_body(FileWriter * fileWriter, BlockSettings * settings, AWDFile* file)
{
	/*
	AWD_skelanim_fr *frame;

	fileWriter->writeSTRING(this->get_name(), 1);

	fileWriter->writeUINT16(this->num_frames);

	this->properties->write_attributes(fileWriter, settings);

	frame = this->first_frame;
	while (frame) {

		fileWriter->writeUINT32(frame->pose->get_addr());
		fileWriter->writeUINT16(frame->duration);

		frame = frame->next;
	}
	*/
	this->user_attributes->write_attributes(fileWriter,  settings);
	return result::AWD_SUCCESS;
}


#include "blocks/skeleton_pose.h"

#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

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

SkeletonPose::SkeletonPose(std::string& name, TYPES::UINT32 duration, TYPES::UINT32 external_start_time) :
	AttrElementBase(), 
	AWDBlock(block_type::SKELETON_POSE, name),
	AnimationFrameBase(duration, external_start_time)
{
}
SkeletonPose::SkeletonPose() :
	AttrElementBase(), 
	AWDBlock(BLOCK::block_type::NULL_REF),
	AnimationFrameBase(0, 0)
{
}

SkeletonPose::~SkeletonPose()
{
	/*
	AWD_joint_tf *cur;

	cur = this->first_transform;
	while (cur) {
		AWD_joint_tf *next = cur->next;
		cur->next = NULL;
		if (cur->transform_mtx) {
			free(cur->transform_mtx);
			cur->transform_mtx = NULL;
		}
		free(cur);
		cur = next;
	}
	*/
}

result
SkeletonPose::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	return result::AWD_SUCCESS;
}


TYPES::state SkeletonPose::validate_block_state()
{
	return TYPES::state::VALID;
}
TYPES::F64 *
SkeletonPose::get_transform_by_idx(int indx)
{
	/*
	int cnt=0;
	AWD_joint_tf *cur;
	cur = this->first_transform;
	while (cur) {
		if (cnt==indx)
			return cur->transform_mtx;
		cnt++;
		cur = cur->next;
	}
	*/
	return NULL;

}
void
SkeletonPose::set_next_transform(TYPES::F64 *mtx)
{
	/*AWD_joint_tf *tf;

	tf = (AWD_joint_tf*)malloc(sizeof(AWD_joint_tf));
	tf->transform_mtx = mtx;
	if (this->first_transform == NULL) {
		this->first_transform = tf;
	}
	else {
		this->last_transform->next = tf;
	}
	*/
}

TYPES::UINT32
SkeletonPose::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = TYPES::UINT32(this->get_name().size() + 4); // strlen field + num transforms
	len += this->calc_attr_length(true,true, settings);
	/*
	cur = this->first_transform;
	while (cur) {
		len += sizeof(BOOL);
		if (cur->transform_mtx != NULL)
			len += MTX43_SIZE(settings->get_wide_matrix());

		cur = cur->next;
	}
	*/

	return len;
}

result
SkeletonPose::write_body(FileWriter * fileWriter, BlockSettings * settings, AWDFile* file)
{
	/*
	fileWriter->writeSTRING(this->get_name(), 1);
	fileWriter->writeUINT16(this->num_transforms);

	this->properties->write_attributes(fileWriter,  settings);

	cur = this->first_transform;
	while (cur) {
		if (cur->transform_mtx!=NULL) {
			fileWriter->writeBOOL(AWD_TRUE);
			fileWriter->writeNumbers( cur->transform_mtx, 9, settings->get_wide_matrix());
			if (!settings->get_wide_matrix()){
				TYPES::F32 offX=TYPES::F32(cur->transform_mtx[9]*settings->get_scale());
				TYPES::F32 offY=TYPES::F32(cur->transform_mtx[10]*settings->get_scale());
				TYPES::F32 offZ=TYPES::F32(cur->transform_mtx[11]*settings->get_scale());
				fileWriter->writeFLOAT32(offX);
				fileWriter->writeFLOAT32(offY);
				fileWriter->writeFLOAT32(offZ);
			}
			else{
				TYPES::F64 offX=TYPES::F64(cur->transform_mtx[9]*settings->get_scale());
				TYPES::F64 offY=TYPES::F64(cur->transform_mtx[10]*settings->get_scale());
				TYPES::F64 offZ=TYPES::F64(cur->transform_mtx[11]*settings->get_scale());
				fileWriter->writeFLOAT64(offX);
				fileWriter->writeFLOAT64(offY);
				fileWriter->writeFLOAT64(offZ);
			}
		}
		else {
			fileWriter->writeBOOL(AWD_FALSE);
		}

		cur = cur->next;
	}
	*/
	this->user_attributes->write_attributes(fileWriter,  settings);
	return result::AWD_SUCCESS;
}


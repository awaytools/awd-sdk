
#include "blocks/vertex_pose.h"

#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_frame.h"
#include "base/state_element_base.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::GEOM;

VertexPose::VertexPose(std::string& name, TYPES::UINT32 duration, TYPES::UINT32 external_start_time) :
	AttrElementBase(), 
	AWDBlock(BLOCK::block_type::SKELETON_POSE, name),
	AnimationFrameBase(duration, external_start_time)
{
}
VertexPose::VertexPose() :
	AttrElementBase(), 
	AWDBlock(BLOCK::block_type::SKELETON_POSE),
	AnimationFrameBase(0, 0)
{
}

VertexPose::~VertexPose()
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
	
TYPES::state VertexPose::validate_block_state()
{
	return TYPES::state::VALID;
}
		unsigned int get_num_subs();
		void add_sub_mesh(SubGeomInternal *);
unsigned int
VertexPose::get_num_subs()
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
	return 0;

}
SubGeomInternal *
VertexPose::get_sub_at(unsigned int){
	return NULL;
}
void
VertexPose::add_sub_mesh(SubGeomInternal *subMesh){
}
	
result 
VertexPose::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
}

TYPES::UINT32
VertexPose::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
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
VertexPose::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
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


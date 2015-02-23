#include "blocks/skeleton.h"
#include <vector>
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::ANIM;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

Skeleton::Skeleton(std::string& name, int neutralTime) :
	AWDBlock(block_type::SKELETON, name),
	AttrElementBase()
{
	this->root_joint = NULL;
	this->joints_per_vert = 0;
	this->simpleMode=false;
	this->shareAutoAnimator=true;
	this->neutralPose=neutralTime;
}

Skeleton::Skeleton():
	AWDBlock(block_type::SKELETON),
	AttrElementBase()
{
	this->root_joint = NULL;
	this->joints_per_vert = 0;
	this->simpleMode=false;
	this->shareAutoAnimator=true;
	this->neutralPose=0;
}
Skeleton::~Skeleton()
{
	if (this->root_joint!=NULL) {
		delete this->root_joint;
		this->root_joint = NULL;
	}
}

result
Skeleton::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	return result::AWD_SUCCESS;
}

TYPES::state Skeleton::validate_block_state()
{
	return TYPES::state::VALID;
}
bool
Skeleton::get_simpleMode(){
	return this->simpleMode;
}
void
Skeleton::set_simpleMode(bool newSimpleMode){
	this->simpleMode=newSimpleMode;
}
bool
Skeleton::get_shareAutoAnimator(){
	return this->shareAutoAnimator;
}
void
Skeleton::set_shareAutoAnimator(bool shareAutoAnimator){
	this->shareAutoAnimator=shareAutoAnimator;
}
std::vector<AWDBlock*>&
Skeleton::get_clip_blocks(){
	return this->clip_blocks;
}
void
Skeleton::set_clip_blocks(std::vector<AWDBlock*>& newBlocklist){
	this->clip_blocks=newBlocklist;
}
int
Skeleton::get_joints_per_vert(){
	return this->joints_per_vert;
}
void
Skeleton::set_joints_per_vert(int jpv){
	this->joints_per_vert=jpv;
}
int
Skeleton::get_neutralPose(){
	return this->neutralPose;
}
void
Skeleton::set_neutralPose(int newNeutralpose){
	this->neutralPose=newNeutralpose;
}
TYPES::UINT32
Skeleton::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size() + sizeof(TYPES::UINT16));
	len += this->calc_attr_length(true,true, settings);

	if (this->root_joint != NULL)
		len += this->root_joint->calc_length(settings);

	return len;
}


SkeletonJoint *
Skeleton::get_root_joint()
{
	return this->root_joint;
}

SkeletonJoint *
Skeleton::set_root_joint(SkeletonJoint *joint)
{
	this->root_joint = joint;
	if (this->root_joint != NULL)
		this->root_joint->set_parent(NULL);

	return joint;
}

result
Skeleton::write_body(FileWriter * fileWriter, BlockSettings * settings, AWDFile* file)
{
	TYPES::UINT16 num_joints_be;

	num_joints_be = 0;
	if (this->root_joint != NULL)
		num_joints_be = 1 + this->root_joint->calc_num_children();

	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	fileWriter->writeUINT16(num_joints_be);
	

	// Write optional properties
	this->properties->write_attributes(fileWriter,  settings);

	// Write joints (if any)
	if (this->root_joint != NULL)
		this->root_joint->write_joint(fileWriter, 1,  settings);

	// Write user attributes
	this->user_attributes->write_attributes(fileWriter,  settings);
	return result::AWD_SUCCESS;
}

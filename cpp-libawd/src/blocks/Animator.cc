#include "blocks/animator.h"

#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "blocks/mesh_inst.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::ATTR;

using namespace AWD::SETTINGS;
using namespace AWD::ANIM;

Animator::Animator(std::string& name, AnimationSet *animSet, anim_type animType ) :
	AWDBlock(block_type::ANIMATOR, name),
	
	AttrElementBase()
{
	this->animator_type = animType;// 1: skeleton    2: vertex
	this->animationSet=animSet;
	this->skeletonBlock = NULL;
	this->animatorProperties = new NumAttrList();
}
Animator::Animator():
	AWDBlock(BLOCK::block_type::ANIMATOR),
	AttrElementBase()
{
}
Animator::~Animator()
{
	delete this->animatorProperties;
}

TYPES::state Animator::validate_block_state()
{
	return TYPES::state::VALID;

}
Skeleton *
Animator::get_skeleton()
{
	return this->skeletonBlock;
}

void
Animator::set_skeleton(Skeleton *skeleton)
{
	this->skeletonBlock = skeleton;
}

AnimationSet *
Animator::get_animationSet()
{
	return this->animationSet;
}

void
Animator::set_animationSet(AnimationSet *animationSet)
{
	this->animationSet = animationSet;
}

anim_type
Animator::get_anim_type()
{
	return this->animator_type;
}

std::vector<BASE::AWDBlock*>&
Animator::get_targets()
{
	return this->animator_targets;
}
void
Animator::set_anim_type(anim_type animType)
{
	this->animator_type = animType;
}
void
Animator::add_target(Mesh *newTarget)
{
	for(AWDBlock* awd_block : this->animator_targets){
		if(awd_block == newTarget)
			return;
	}
	this->animator_targets.push_back(newTarget);
}

result
Animator::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	/*
	if (this->animationSet != NULL){
		this->animator_type=this->animationSet->get_anim_type();
		this->animationSet->add_with_dependencies(target_file, instance_type);
		if (this->animationSet->get_anim_type()==ANIMTYPESKELETON){
			if (this->skeletonBlock==NULL)
				this->skeletonBlock=this->animationSet->get_skeleton();
			if (this->skeletonBlock==NULL){
				//error
			}
			else{
				this->skeletonBlock->add_with_dependencies(target_file, instance_type);
				union_ptr tex_val;
				tex_val.addr = (BADDR *)malloc(sizeof(BADDR));
				*tex_val.addr = this->skeletonBlock->get_addr();
				this->animatorProperties->set(1, tex_val, sizeof(BADDR), BADDR);
			}
		}
	}
	for(AWDBlock *block : this->animator_targets->get_blocks())
		block->add_with_dependencies(target_file, instance_type);
		*/
	return result::AWD_SUCCESS;
}
TYPES::UINT32
Animator::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size());
	len += sizeof(TYPES::UINT16); //animatortype
	len += this->calc_attr_length(true,true, settings);
	len += this->animatorProperties->calc_length(settings);
	len += sizeof(TYPES::UINT32);//animSetID
	len += sizeof(TYPES::UINT16);//num_targets
	len += sizeof(TYPES::UINT16);//activeState
	len += sizeof(TYPES::UINT8);//autoPlay
	//len += sizeof(TYPES::UINT32);//autoPlay
	len += TYPES::UINT32(this->animator_targets.size()*sizeof(TYPES::UINT32));//target IDs

	return len;
}
result
Animator::write_body(FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
		/*
	BADDR animSet_addr;
	TYPES::UINT16 num_targets;
	TYPES::UINT16 animatorType=this->animator_type;

	fileWriter->writeSTRING(this->get_name(), 1);

	fileWriter->writeUINT16(animatorType);

	this->animatorProperties->write_attributes(fileWriter, settings);
	
	// Write animSet id - TODO-> ERROR IF NULL
	animSet_addr = 0;
	if (this->animationSet != NULL)
		animSet_addr = this->animationSet->get_addr();
	fileWriter->writeUINT32(animSet_addr);

	num_targets = (TYPES::UINT16)this->animator_targets->get_num_blocks();
	fileWriter->writeUINT16(num_targets);
	for(AWDBlock *block : this->animator_targets->get_blocks()){
		fileWriter->writeUINT32(block->get_addr());
	}
	*/
	int activeState=0;
	fileWriter->writeUINT16(activeState);
	int autoplay=0;
	fileWriter->writeUINT8(autoplay);

	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

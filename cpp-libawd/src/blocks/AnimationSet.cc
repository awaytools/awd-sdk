#include "blocks/anim_set.h"
#include "utils/util.h"

#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/state_element_base.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::ANIM;

AnimationSet::AnimationSet(std::string& name, anim_type animtype, std::string& sourcePreID, std::vector<AWDBlock*>& sourcClipList ) :
	AWDBlock(BLOCK::block_type::ANIMATION_SET, name),	
	AttrElementBase()
{
	this->sourcePreID=sourcePreID;
	this->target_geom=NULL;
	this->animSet_type = animtype;// 1: skeleton    2: vertex
	
	this->preAnimationClipNodes=sourcClipList;
	this->is_processed=false;
	this->simple_mode=true;
	this->originalPointCnt = 0;
	this->skeletonBlock =NULL;
}
AnimationSet::AnimationSet():
	AWDBlock(BLOCK::block_type::ANIMATION_SET),
	AttrElementBase()
{
}
AnimationSet::~AnimationSet()
{
}

TYPES::state AnimationSet::validate_block_state()
{
	return TYPES::state::VALID;
}

AWDBlock*
AnimationSet::get_target_geom()
{
	return this->target_geom;
}
void
AnimationSet::set_target_geom(AWDBlock* target_geom)
{
	this->target_geom = target_geom;
}
int
AnimationSet::get_originalPointCnt()
{
	return this->originalPointCnt;
}
void
AnimationSet::set_originalPointCnt(int newPointCnt)
{
	this->originalPointCnt = newPointCnt;
}

bool
AnimationSet::get_simple_mode()
{
	return this->simple_mode;
}

void
AnimationSet::set_simple_mode(bool simple_mode)
{
	this->simple_mode = simple_mode;
}
bool
AnimationSet::get_is_processed()
{
	return this->is_processed;
}

void
AnimationSet::set_is_processed(bool isProcessed)
{
	this->is_processed = isProcessed;
}
std::vector<AWDBlock*>&
AnimationSet::get_preAnimationClipNodes()
{
	return this->preAnimationClipNodes;
}

void
AnimationSet::set_preAnimationClipNodes(std::vector<AWDBlock*>& preAnimationClipNodes)
{
	this->preAnimationClipNodes = preAnimationClipNodes;
}
std::vector<AWDBlock*>&
AnimationSet::get_animationClipNodes()
{
	return this->animationClipNodes;
}

void
AnimationSet::set_animationClipNodes(std::vector<AWDBlock*>& newanimClipNodes)
{
	this->animationClipNodes = newanimClipNodes;
}

anim_type
AnimationSet::get_anim_type()
{
	return this->animSet_type;
}

void
AnimationSet::set_anim_type(anim_type animType)
{
	this->animSet_type = animType;
}

std::string&
AnimationSet::get_sourcePreID()
{
	return this->sourcePreID;
}
void
AnimationSet::set_sourcePreID(std::string& sourcePreID)
{
	this->sourcePreID = sourcePreID;
}

Skeleton *
AnimationSet::get_skeleton()
{
	return this->skeletonBlock;
}

void
AnimationSet::set_skeleton(Skeleton *skeleton)
{
	this->skeletonBlock = skeleton;
}

TYPES::UINT32
AnimationSet::calc_body_length(AWDFile* awd_file, BlockSettings *settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size());
	len += sizeof(TYPES::UINT16);
	//len += this->animationClipNodes->get_num_blocks() * sizeof(BADDR);
	len += this->calc_attr_length(true,true, settings);
	return len;
}

result
AnimationSet::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	/*
	AWDBlock *block;
	AWDBlockIterator *it;
	it = new AWDBlockIterator(this->animationClipNodes);
	while ((block = it->next()) != NULL)
		block->add_with_dependencies(target_file, instance_type);
	if (this->animSet_type==ANIMTYPESKELETON){
		if (this->skeletonBlock) {
			this->skeletonBlock->add_with_dependencies(target_file, instance_type);
			union_ptr tex_val;
			tex_val.ui16 = (TYPES::UINT16 *)malloc(sizeof(TYPES::UINT16));
			*tex_val.ui16 = this->skeletonBlock->get_joints_per_vert();
			this->properties->set(1, tex_val, sizeof(TYPES::UINT16), TYPES::UINT16);
		}
	}
	delete it;
	*/
	return result::AWD_SUCCESS;
}
result
AnimationSet::write_body(FILES::FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
	/*
	AWDBlock *block;
	AWDBlockIterator *it;

	fileWriter->writeSTRING(this->get_name(), 1);

	fileWriter->writeUINT16((TYPES::UINT16)this->animationClipNodes->get_num_blocks());

	this->properties->write_attributes(fileWriter, settings);

	it = new AWDBlockIterator(this->animationClipNodes);
	while ((block = it->next()) != NULL) {
		BADDR addr = block->get_addr();
		fileWriter->writeUINT32(addr);
	}
	delete it;
	this->user_attributes->write_attributes(fileWriter, settings);
	*/
	return result::AWD_SUCCESS;
}



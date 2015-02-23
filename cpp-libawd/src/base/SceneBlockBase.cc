#include "base/scene_base.h"

#include "utils/awd_types.h"
#include "utils/util.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

SceneBlockBase::SceneBlockBase(BLOCK::block_type type, const std::string& name,  TYPES::F64 *mtx) :
	AWDBlock(type, name),
	AttrElementBase()
{
	this->parent = NULL;
	this->transform_mtx = new GEOM::MATRIX4x3();
	if(mtx!=NULL)
		this->transform_mtx->set(mtx);
	this->add_category(BLOCK::category::SCENE_OBJECT);
}

SceneBlockBase::~SceneBlockBase()
{
	delete this->transform_mtx;
}

result
SceneBlockBase::write_scene_common(FileWriter* fileWriter, BlockSettings *settings, AWDFile* awd_file)
{
	result res = result::AWD_SUCCESS;
	
	BADDR parent_addr = 0;
	if (this->parent != NULL){
		res=this->parent->get_block_addr(awd_file, parent_addr);
		if(res!=result::AWD_SUCCESS)
			return res;
	}

	res = fileWriter->writeUINT32(parent_addr);
	if(res!=result::AWD_SUCCESS)
		return result::WRITE_ERROR;
	
	res = this->transform_mtx->write_to_file(fileWriter, settings);
	if(res!=result::AWD_SUCCESS)
		return res;

	res = fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	if(res!=result::AWD_SUCCESS)
		return res;

	return res;
}

result
SceneBlockBase::add_children_to_file(AWDFile* awd_file, BLOCK::instance_type instance_type)
{
	if(instance_type==BLOCK::instance_type::BLOCK_EXTERN){
		return result::AWD_ERROR;// should not happen
	}
	result res=result::AWD_SUCCESS;

	for(AWDBlock* child : this->children){
		if(instance_type==BLOCK::instance_type::BLOCK_EXTERN_DEPENDENCIES)
			res=awd_file->add_block(child, BLOCK::instance_type::BLOCK_EXTERN, NULL);
		else if(instance_type==BLOCK::instance_type::BLOCK_EMBBED){
			child->add_with_dependencies(awd_file, BLOCK::instance_type::BLOCK_EMBBED);
			res=awd_file->add_block(child, BLOCK::instance_type::BLOCK_EMBBED, NULL);
		}
	}
	return res;
}

TYPES::UINT32
SceneBlockBase::calc_common_length(bool wide_mtx)
{
	return TYPES::UINT32(4 + MTX43_SIZE(wide_mtx) + sizeof(TYPES::UINT16) + this->get_name().size());
}

void
SceneBlockBase::set_transform(TYPES::F64 *mtx)
{
	this->transform_mtx->set(mtx);
}

GEOM::MATRIX4x3 *
SceneBlockBase::get_transform()
{
	return this->transform_mtx;
}

AWDBlock *
SceneBlockBase::get_parent()
{
	return this->parent;
}

void
SceneBlockBase::set_parent(SceneBlockBase *parent)
{
	if (parent != this->parent) 
		this->parent = parent;
}

std::vector<AWDBlock*>& 
SceneBlockBase::get_children()
{
	return this->children;
}

void
SceneBlockBase::add_child(SceneBlockBase *child)
{
	for(AWDBlock* awd_block: this->children){
		if(awd_block==child){
			child->set_parent(this);
			return;
		}
	}
	child->set_parent(this);
	this->children.push_back(child);
}

void
SceneBlockBase::remove_child(SceneBlockBase *child)
{
	// TODO: Implement remove()
}
void
SceneBlockBase::make_children_invalide()
{
	//this->children->remove(child);    
	/*SceneBlockBase * block;
	AWDBlockIterator * it = new AWDBlockIterator(this->children);
	while ((block = (SceneBlockBase *)it->next()) != NULL) {
		block->set_isValid(false);
		block->make_children_invalide();
	}
	*/
}

bool
SceneBlockBase::isEmpty()
{
	if (this->get_type()!=BLOCK::block_type::CONTAINER)
		return false;
	SceneBlockBase * scene_block;
	for(AWDBlock* awd_block : this->children){
		scene_block = reinterpret_cast<SceneBlockBase*>(awd_block);
		if(scene_block!=NULL){
			bool isemtpy=scene_block->isEmpty();
			if (!isemtpy)
				return false;
		}
	}
	return true;
}
#include "elements/block_instance.h"
#include "files/awd_file.h"
#include "base/scene_base.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

BlockInstance::BlockInstance(AWDBlock* awdBlock, AWDFile* awdFile, BlockSettings* exportSettings, instance_type instanceType, bool settingsOwner):
	BASE::StateElementBase()
{
	this->awdBlock=awdBlock;
	this->awdFile=awdFile;
	this->settingsOwner=settingsOwner;
	this->settings=exportSettings;
	this->instanceType=instanceType;
	this->addr = 0;
}
BlockInstance::~BlockInstance()
{
	if(this->settingsOwner)
		delete this->settings;
	
}
	
TYPES::state BlockInstance::validate_state()
{
	if(this->awdBlock==NULL)
		return TYPES::state::INVALID;
	if(this->awdFile==NULL)
		return TYPES::state::INVALID;
	if(this->awdBlock->check_state()!=state::VALID)
		return TYPES::state::INVALID;
	return TYPES::state::VALID;
}


TYPES::BADDR
BlockInstance::get_addr()
{
	return this->addr;
}

void
BlockInstance::set_addr(TYPES::BADDR addr)
{
	this->addr = addr;
}


BlockSettings*
BlockInstance::get_settings()
{
	return this->settings;
}

void
BlockInstance::set_settings(BlockSettings* settings)
{
	this->settings = settings;
}

AWDBlock*
BlockInstance::get_awdBlock()
{
	return this->awdBlock;
}

void
BlockInstance::set_awdBlock(AWDBlock* awdBlock)
{
	this->awdBlock = awdBlock;
}

instance_type
BlockInstance::get_instanceType()
{
	return this->instanceType;
}

void
BlockInstance::set_instanceType(instance_type instanceType)
{
	this->instanceType = instanceType;
}

TYPES::UINT32
BlockInstance::write_block(FileWriter * fileWriter)
{
	if(fileWriter==NULL)
		return 0;	//should never happen
		
	block_type thisBlockType = this->awdBlock->get_type();
	if(this->instanceType==instance_type::BLOCK_EXTERN)
		thisBlockType=block_type::EXTERNAL_RESSOURCE;

	result res = result::AWD_SUCCESS;

	TYPES::UINT8 ns_addr = 0; 
	TYPES::INT8 ns_handle = this->awdBlock->get_name_space_handle();
	if(ns_handle<0){
		// use the default name_space-id for this AWDFile
		//ns_addr = awdFile->get_default_namespace_id();
	}
	else{
		// make sure that the namespace is available for the file.
		res = awdFile->add_namespace_by_id(TYPES::UINT8(ns_handle));
		if(res==result::AWD_SUCCESS)
			ns_addr = TYPES::UINT8(ns_handle);
		else{
			// TODO: ERROR BECAUE NAMESPACE NOT AVAILABLE!
			// For now use the default name_space
			//ns_addr = awdFile->get_default_namespace_id();
		}
	}
	
	// Write header
	fileWriter->writeUINT32(this->addr);	// from BlockInstance
	fileWriter->writeUINT8(ns_addr);		// from Block
	fileWriter->writeUINT8(TYPES::UINT8(thisBlockType));		// from AWDBlock
	fileWriter->writeUINT8(this->settings->get_block_header_flag());

	// if this block_instance is set to EXTERNAL RESSOURCE, we only need to write the name (id ) for the block:

		if(thisBlockType==block_type::EXTERNAL_RESSOURCE){		
			if(this->get_awdBlock()->get_name().empty()){
				// TODO:: ERROR : NAME IS EMPTY
			}
			TYPES::UINT32 length = TYPES::UINT32(this->get_awdBlock()->get_name().size());
			fileWriter->writeUINT32(length);	// from BlockInstance
			fileWriter->writeSTRING(this->get_awdBlock()->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
			return length;
		}
	
	// If this block_instance is not set to EXTERNAL RESSOURCE, we write the full block-body: 

	return TYPES::UINT32(7 + this->awdBlock->write_block(fileWriter, this->settings, this->awdFile));
		
}

result 
BlockInstance::add_with_dependencies(){
	if(this->get_state()==state::INVALID)
		return result::AWD_ERROR;
	result res = result::AWD_SUCCESS;
	if(this->instanceType!=BLOCK::instance_type::BLOCK_EXTERN){
		//recursiv add dependencies to the file.
		res = this->get_awdBlock()->add_with_dependencies(this->awdFile, this->instanceType);
	}
	else{
		// check if this block is assigned to be embbed in another AWDFile. If not we (optionally) have a error.
	}
	/*
	BLOCK::BlockInstance* thisInstance = NULL;
	res = this->awdFile->find_block_instance(this->get_awdBlock(), &thisInstance);
	if(res==result::AWD_SUCCESS){
		// if this was already in file, we do nothing. children should already be in file also.
		// todo: revisit this for block_instance behaviour.
	}
	else if(res==result::AWDFILE_BLOCKINSTANCE_NOT_FOUND){
		// add the block-instance to the final-block-list of the AWDFile.
		res = this->awdFile->add_block_instance(this); 

		if(this->get_awdBlock()->is_category(BLOCK::category::SCENE_OBJECT)){
			SceneBlockBase* scene_block = reinterpret_cast<SceneBlockBase*>(this->get_awdBlock());
			if(scene_block!=NULL){
				res = scene_block->add_children_to_file(this->awdFile, this->instanceType);
			}
		}
	}
	*/
	return res;
}

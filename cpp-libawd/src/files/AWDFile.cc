#include "files/awd_file.h"

#ifdef _WIN32	
#include <Windows.h>
#include "ShellApi.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "awd_project.h"
#include "utils/awd_globals.h"
#include "elements/block_instance.h"
#include "files/file_reader.h"




using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;
using namespace AWD::BLOCKS;

AWDFile::AWDFile(AWDProject* awd_project, const std::string& url, bool own_settings)
{
	this->settings=awd_project->get_settings();
	this->own_settings = own_settings;
	this->awd_project = awd_project;
	this->url = url;   
	this->meta_data_block = NULL;
	this->name_space_block = NULL;
	this->meta_data_block_inst = NULL;
	this->namespace_block_inst = NULL;
}

AWDFile::~AWDFile()
{
	if(this->own_settings)
		delete this->settings;
	for(BLOCK::BlockInstance* block_inst:this->block_instances){
		//todo: remove the block_instance from all AWDBlocks
		delete block_inst;
	}

}
result
AWDFile::clear()
{
	return result::AWD_SUCCESS; 
}


TYPES::state 
AWDFile::validate_state()
{	
	if(this->name_space_block==NULL)
		this->name_space_block = new Namespace();
	if(this->meta_data_block==NULL)
		this->meta_data_block = new MetaData(this->settings->get_generator_name(), this->settings->get_generator_version());
	if(this->meta_data_block_inst==NULL)
		this->meta_data_block_inst = new BlockInstance(this->meta_data_block, this, reinterpret_cast<BlockSettings*>(this->settings), instance_type::BLOCK_EMBBED);
	if(this->namespace_block_inst==NULL)
		this->namespace_block_inst = new BlockInstance(this->name_space_block, this, reinterpret_cast<BlockSettings*>(this->settings), instance_type::BLOCK_EMBBED);
		
	return TYPES::state::VALID;
}

result 
AWDFile::process()
{	
	if(this->check_state()==state::INVALID)
		return result::AWDFILE_ERROR;
	if(this->get_state()==state::EMPTY)
		return result::AWDFILE_ERROR;	

	if(this->get_process_state()!=process_state::UNPROCESSED)
		return result::AWD_SUCCESS;

	this->set_process_state(process_state::PROCESS_ACTIV);

	std::vector<BlockInstance*> unordered_blocks;
	for(BlockInstance* blockInstance : this->block_instances){
		blockInstance->get_awdBlock()->set_process_state(process_state::UNPROCESSED);
		unordered_blocks.push_back(blockInstance);
	}

	// clear the list of block_instances, beause we need them in correct order, including the dependencies
	this->block_instances.clear();

	// add the meta_data and the name_space block to the block_instance list and give them the according address.
	this->block_instances.push_back(this->meta_data_block_inst);

	if(this->namespace_block_inst->check_state()==state::VALID)
		this->block_instances.push_back(this->namespace_block_inst);
	
	// we have a setting to change the order of the blocks based on Block-category.
	// all blocks will collect their dependencies, so that they have a smaller block-addr (index in block-instances list)
	// by default, the scenegraph is parsed first.
	std::vector<BLOCK::category> block_export_categories_order;
	result res = this->settings->get_export_block_catergories_order(block_export_categories_order);
	if(res!=result::AWD_SUCCESS)
		return res;
	for(BLOCK::category block_category : block_export_categories_order){
		for(BlockInstance* blockInstance : unordered_blocks){
			if((!this->settings->get_export_materials())&&(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::SIMPLE_MATERIAL)){
				continue;
			}
			else{
				if(blockInstance->get_state()!=state::INVALID){
					if(blockInstance->get_awdBlock()->is_category(block_category)){
						if(blockInstance->get_process_state()==process_state::UNPROCESSED){
							if(block_category==BLOCK::category::SCENE_OBJECT){
								if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::TIMELINE){
									BLOCKS::Timeline* timeline = reinterpret_cast<BLOCKS::Timeline*>( blockInstance->get_awdBlock());
									if(timeline->get_is_scene())
										blockInstance->add_with_dependencies();
								}
								else{
									SceneBlockBase* sceneblock = reinterpret_cast<SceneBlockBase*>( blockInstance->get_awdBlock());
									if(sceneblock->get_parent()==NULL)
										blockInstance->add_with_dependencies();
								}
							}
							else
								blockInstance->add_with_dependencies();
						}
					}
				}
			}
		}
	}
	// if all blocks should be exported, we also export the blocks that are still left.
	if(this->settings->get_export_all()){
		for(BlockInstance* blockInstance : unordered_blocks){
			if((!this->settings->get_export_materials())&&(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::SIMPLE_MATERIAL)){
				continue;
			}
			else{
				if(blockInstance->get_state()!=state::INVALID){
					if(blockInstance->get_process_state()==process_state::UNPROCESSED){
						if(blockInstance->get_awdBlock()->is_category(BLOCK::category::SCENE_OBJECT)){
							SceneBlockBase* sceneblock = reinterpret_cast<SceneBlockBase*>( blockInstance->get_awdBlock());
							if(sceneblock->get_parent()==NULL)
								blockInstance->add_with_dependencies();
						}
						else
							blockInstance->add_with_dependencies();
					}
				}
			}
		}
	}
	TYPES::UINT32 addr_cnt=0;
	for(BlockInstance* blockInstance : this->block_instances){
		blockInstance->set_addr(addr_cnt);
		addr_cnt++;
	}
	this->set_process_state(process_state::PROCESSED);
	return result::AWD_SUCCESS;
}

result 
AWDFile::add_block_instance(BLOCK::BlockInstance* block_instance){
	if(block_instance==NULL)
		return result::AWDFILE_ERROR; 
	if(block_instance->get_awdBlock()==NULL)
		return result::AWDFILE_ERROR; 
	BLOCK::BlockInstance* existing_instance=NULL;
	result res = this->find_block_instance(block_instance->get_awdBlock(), &existing_instance);
	if(res==result::AWD_SUCCESS){
		if(existing_instance==block_instance)
			return result::AWD_SUCCESS;		
		if(existing_instance->get_instanceType()!=block_instance->get_instanceType()){
			return result::AWD_SUCCESS;
		}
	}
	this->block_instances.push_back(block_instance);
	return result::AWD_SUCCESS;
}

result 
AWDFile::find_block_instance(BASE::AWDBlock* awd_block, BLOCK::BlockInstance** block_instance){
	if(awd_block==NULL)
		return result::AWDFILE_ERROR; 

	for(BLOCK::BlockInstance* fileblockInstance:this->block_instances){
		if(fileblockInstance->get_awdBlock()==awd_block){
			if(block_instance)
				*block_instance=fileblockInstance;
			return result::AWD_SUCCESS;
		}
	}
	return result::AWDFILE_BLOCKINSTANCE_NOT_FOUND;
}

result 
AWDFile::add_dependency_block(BASE::AWDBlock* awd_block, BLOCK::instance_type instance_type){
	if(awd_block==NULL)
		return result::AWDFILE_ERROR; 
	BLOCK::BlockInstance* block_instance = NULL;
	result res=this->find_block_instance(awd_block, &block_instance);
	if(res==result::AWDFILE_ERROR)
		return res;

	if(res==result::AWDFILE_BLOCKINSTANCE_NOT_FOUND) {
		block_instance=new BlockInstance(awd_block, this, this->settings->clone_block_settings(), instance_type);
		return res;
	}
	else{
		// TODO: Check state of block_instance ?
		// if the block is already embbed, we leave it like this.
		// otherwise we update the instance_type.
		if(block_instance->get_instanceType()!=BLOCK::instance_type::BLOCK_EMBBED){
			block_instance->set_instanceType(instance_type);
		}
	}

	return result::AWD_SUCCESS;
}

TYPES::UINT32 
AWDFile::get_block_cnt(){
	this->check_state();
	return TYPES::UINT32(this->block_instances.size());
}

SETTINGS::Settings* 
AWDFile::get_settings(){
	return this->settings;
}

result 
AWDFile::add_block(BASE::AWDBlock* awd_block, BLOCK::instance_type instance_type, BLOCK::BlockInstance** block_inst=NULL){
	if(awd_block==NULL)
		return result::AWDFILE_ERROR; 
	if(this->get_state()==state::INVALID)
		return result::AWDFILE_ERROR;

	// try finding a already existing block-instance for the block.
	BLOCK::BlockInstance* block_instance=NULL;
	result res=this->find_block_instance(awd_block, &block_instance);
	if(res==result::AWDFILE_ERROR)
		return res;

	if(res==result::AWDFILE_BLOCKINSTANCE_NOT_FOUND) {
		block_instance=new BlockInstance(awd_block, this, this->settings->clone_block_settings(), instance_type);
		if(block_inst)
			*block_inst=block_instance;
		this->block_instances.push_back(block_instance);
		return result::AWD_SUCCESS;
	}
	else{
		if(block_inst)
			*block_inst=block_instance;
		// TODO: Check state of block_instance ?
		block_instance->set_instanceType(instance_type);
	}
	this->check_state();

	return result::AWD_SUCCESS;
}

result 
AWDFile::get_statistics(std::vector<std::string>& output_list)
{
	std::string stat_str ="		AWDFile stats\n\n";
	stat_str+="			Output-url = '"+this->get_settings()->get_root_directory()+"'\n";
	stat_str+="			Total AWDBlockInstance count =	"+std::to_string(this->get_block_cnt())+"\n\n";
	
	output_list.push_back(stat_str);
	for(BlockInstance* block_inst : this->block_instances){
		std::string stat_str_block ="				Blockid = "+std::to_string(block_inst->get_addr())+" type = "+std::to_string(TYPES::UINT32(block_inst->get_awdBlock()->get_type()))+" name = '"+block_inst->get_awdBlock()->get_name()+"'";
		output_list.push_back(stat_str_block);
		
	}

	return result::AWD_SUCCESS;
}

std::string& 
AWDFile::get_url(){
	return this->url;
}

result 
AWDFile::open()
{
#ifdef _WIN32
	
	#ifdef _UNICODE
		std::wstring tail;
		tail.assign(this->url.begin(), this->url.end());
		ShellExecute(NULL, (LPCWSTR)"open", tail.c_str(), NULL, NULL, SW_SHOWNORMAL);
	#else
		std::string tail;
		tail.assign(this->url.begin(), this->url.end());
		ShellExecute(NULL, (LPCSTR)"open", tail.c_str(), NULL, NULL, SW_SHOWNORMAL);
	#endif

#else

    std::string str = "/usr/bin/open " + this->url;
    system(str.c_str());

#endif // _WINDOWS
	return result::AWD_SUCCESS;
}

result 
AWDFile::write_to_disc()
{
	// make sure blocks are correctly ordered, all dependencies are resolved, and all blocks are prepared
	result res = process();
	if(res!=result::AWD_SUCCESS)
		return res;
	// set the filewriter to write to a tmp file.
	
	// on windows this gives a warning that we should use tmpfile_s instead, but because this works on mac and win, i left it as is
	FILES::FileWriter* fileWriter = new FileWriter();
	res = fileWriter->set_file(tmpfile());
	if(res!=result::AWD_SUCCESS)
		return res;
	
	// write all blocks into the tmp file
	TYPES::UINT32 body_length = 0;
	for(BlockInstance* blockInstance : this->block_instances){
	//	if((blockInstance->get_awdBlock()->get_type()!=BLOCK::block_type::TIMELINE)&&(blockInstance->get_awdBlock()->get_type()!=BLOCK::block_type::SIMPLE_MATERIAL))
		body_length += blockInstance->write_block(fileWriter);
	}
	
	// get the optionally compressed byte-data for the body of the file.
	TYPES::UINT8* body_buf = NULL;
	res = fileWriter->get_compressed_bytes(&body_buf, this->settings->get_compression(), body_length);
	if(res!=result::AWD_SUCCESS)
		return res;

	// set the file-writer to use the output file
	res = fileWriter->open_file(this->url);
	if(res!=result::AWD_SUCCESS)
		return res;
	
	// write the file-header into the output file
	std::string magic_string="AWD";
	res = fileWriter->writeSTRING(magic_string, write_string_with::NO_LENGTH_VALUE);
	if(res!=result::AWD_SUCCESS)
		return res;
	res =fileWriter->writeUINT8(AWD::VERSION_MAJOR);
	if(res!=result::AWD_SUCCESS)
		return res;
	res =fileWriter->writeUINT8(AWD::VERSION_MINOR);
	if(res!=result::AWD_SUCCESS)
		return res;
	res =fileWriter->writeUINT16(this->settings->get_file_header_flag());
	if(res!=result::AWD_SUCCESS)
		return res;
	res =fileWriter->writeUINT8(TYPES::UINT8(this->settings->get_compression()));
	if(res!=result::AWD_SUCCESS)
		return res;
	
	// write length of body
	res = fileWriter->writeUINT32(body_length);	
	if(res!=result::AWD_SUCCESS)
		return res;	

	// write body
	res = fileWriter->writeBytes(body_buf, body_length);	
	if(res!=result::AWD_SUCCESS)
		return res;	
	
	//delete body_buffer
	free(body_buf);
	
	return result::AWD_SUCCESS;
}

result 
AWDFile::read_from_disc(bool allow_overwrite)
{
	if(this->get_state()==state::INVALID)
		return result::FILE_IS_INVALID;

	if(this->get_state()!=state::EMPTY){
		if(allow_overwrite){
			return result::FILE_IS_NOT_EMPTY;
		}
		else{
			this->clear();
		}			
	}
	FILES::FileReader fileReader = FileReader();

	FILE* temp_file = tmpfile();
	if(temp_file==NULL)
		return result::AWDFILE_ERROR_GETTING_TEMPFILE;

	fileReader.set_file(temp_file);

	std::string magicString;
	result res=fileReader.readSTRING_FIXED(magicString, 3);
	if(res!=result::AWD_SUCCESS)
		return res;	
    if (magicString.empty())
		return result::FILEREADER_ERROR_READING_BYTES;

    if (magicString != "AWD")
        return result::FILEREADER_ERROR_READING_BYTES;

    TYPES::UINT8 majorVersion;
	res=fileReader.readUINT8(&majorVersion);
	if(res!=result::AWD_SUCCESS)
		return res;	

    TYPES::UINT8 minVersion;
	res=fileReader.readUINT8(&minVersion);
	if(res!=result::AWD_SUCCESS)
		return res;	
		
    TYPES::UINT16 headerflag;
	res=fileReader.readUINT16(&headerflag);
	if(res!=result::AWD_SUCCESS)
		return res;	
	this->settings->set_file_header_flag(headerflag);
	
    TYPES::UINT8 compression;
	res=fileReader.readUINT8(&compression);
	if(res!=result::AWD_SUCCESS)
		return res;	
	this->settings->set_compression(SETTINGS::compression(compression));
	
    TYPES::UINT32 compressed_body_length;
	res=fileReader.readUINT32(&compressed_body_length);
	if(res!=result::AWD_SUCCESS)
		return res;	

	res=fileReader.get_uncompress_data(compressed_body_length, this->settings->get_compression());
	if(res!=result::AWD_SUCCESS)
		return res;	

	TYPES::UINT32 parsed_bytes_cnt=0;
    TYPES::UINT32 file_pos=0;
    // if no compression, we can directly read the blocks
    while (parsed_bytes_cnt<compressed_body_length){
		
		// parser block-header:

				TYPES::UINT32 blockID;
				res=fileReader.readUINT32(&blockID);
				if(res!=result::AWD_SUCCESS)
					return res;// No data!

				TYPES::UINT8 nameSpaceID;
				res=fileReader.readUINT8(&nameSpaceID);
				if(res!=result::AWD_SUCCESS)
					return res;// No data!

				// todo: get the namespace-block

				TYPES::UINT8 blockType;
				res=fileReader.readUINT8(&blockType);
				if(res!=result::AWD_SUCCESS)
					return res;// No data!
		
				TYPES::UINT8 blockFlags;
				res=fileReader.readUINT8(&blockFlags);
				if(res!=result::AWD_SUCCESS)
					return res;// No data!

				TYPES::UINT32 blockLength;
				res=fileReader.readUINT32(&blockLength);
				if(res!=result::AWD_SUCCESS)
					return res;// No data!

		// add the  (11 is the count of bytes parsed for the block-header)
		parsed_bytes_cnt += 11+blockLength;

		// try to get a AWDBLock for this Block or continue to next block
		AWDBlock* new_awd_block=NULL;
		res=BLOCK::create_block_for_block_type(&new_awd_block, BLOCK::block_type(blockType), this);
		if(res!=result::AWD_SUCCESS){
			// todo: add a message to the AWDFile.
			goto skip_to_next_block;
		}
        
		
		// parse the block-body block using the derived read_body functions on the newly created AWDBlock.
		// if the parsing fails, we delete the block and continue to next block.
		res = new_awd_block->read_block(&fileReader, this->settings->clone_block_settings(), this);
		if(res!=result::AWD_SUCCESS){
			delete new_awd_block;
			goto skip_to_next_block;
		}
		res=fileReader.get_pos(file_pos);
		if(res!=result::AWD_SUCCESS)
			return res;// No data!

		// at this point we check if the byte position matches with our byte_counter. if not, parsing has been invalid.
		if(file_pos!=parsed_bytes_cnt){
skip_to_next_block:
			res=fileReader.set_pos(parsed_bytes_cnt);
			if(res!=result::AWD_SUCCESS)
				return res;// No data!
			continue;

		}
		
	}

	return result::AWD_SUCCESS;
}

result 
AWDFile::add_namespace_by_id(TYPES::UINT8 name_space_id){
	if(this->get_state()==state::INVALID)
		return result::AWD_ERROR;
	

	result res = this->name_space_block->find_namespace_by_handle(name_space_id);
	if(res!=result::AWD_SUCCESS){
		return result::AWD_ERROR;
	}
	return result::AWD_SUCCESS;

}

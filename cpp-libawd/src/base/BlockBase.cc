#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/util.h"

using namespace AWD;
using namespace AWD::BASE;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;

AWDBlock::AWDBlock(BLOCK::block_type type) :
	StateElementBase()
{
	this->inc_id=0;
	this->type = type;
	this->external_id="";
	this->external_object=NULL;
	this->set_state(TYPES::state::VALID);
	this->script_name="-";
	this->byte_cnt=0;
}
AWDBlock::AWDBlock(BLOCK::block_type type, const std::string& name) :
	StateElementBase()
{
	this->inc_id=0;
	this->type = type;
	this->external_id="";
	this->external_object=NULL;
	this->name=name;
	this->set_state(TYPES::state::VALID);
	this->script_name="-";
	this->byte_cnt=0;
}

AWDBlock::~AWDBlock()
{
}

void 
AWDBlock::get_scene_names(std::string& scene_names_str){
	scene_names_str="";
	for(std::string id:this->scene_names){
		scene_names_str+=id;
		if(id!=this->scene_names.back())
			scene_names_str+=", ";
	}
}

				void set_ressource_ids(std::vector<std::string>& ids);
				std::vector<std::string>& get_ressource_ids();
void 
AWDBlock::set_ressource_ids(std::vector<std::string>& ids)
{	
	this->ressource_ids=ids;
}
std::vector<std::string>
AWDBlock::get_ressource_ids(){
	return this->ressource_ids;
}
void 
AWDBlock::add_scene_name(const std::string& scene_name){
	for(std::string id:this->scene_names){
		if(id==scene_name)
			return;
	}
	this->scene_names.push_back(scene_name);
}
void 
AWDBlock::add_res_id(const std::string& new_res){
	if(has_res_id(new_res))
		return;
	else
		this->ressource_ids.push_back(new_res);
}

bool 
AWDBlock::has_res_id(const std::string& new_res){
	for(std::string id:this->ressource_ids){
		if(id==new_res)
			return true;
	}
	return false;
}


void 
AWDBlock::clear_res_ids()
{
	this->ressource_ids.clear();
	//this->external_id="";
}

TYPES::state 
AWDBlock::validate_state()
{
	return this->validate_block_state();
}

	
result
AWDBlock::add_with_dependencies(FILES::AWDFile *file, BLOCK::instance_type instance_type)
{
	if(this->get_process_state()==process_state::UNPROCESSED){	
		this->set_process_state(process_state::PROCESS_ACTIV);
		if(instance_type==BLOCK::instance_type::BLOCK_EXTERN_DEPENDENCIES)
			this->collect_dependencies(file, BLOCK::instance_type::BLOCK_EXTERN);
		else if(instance_type==BLOCK::instance_type::BLOCK_EMBBED)
			this->collect_dependencies(file, BLOCK::instance_type::BLOCK_EMBBED);
		file->add_block(this, instance_type, NULL);
		this->set_process_state(process_state::PROCESSED);
	}

	return result::AWD_SUCCESS;
}

result
AWDBlock::read_body(FILES::FileReader*, SETTINGS::BlockSettings *, FILES::AWDFile*)
{
	// Read the body using concrete implementation in block sub-classes, and output the result
	return result::AWD_SUCCESS;
}
result
AWDBlock::read_block(FILES::FileReader* fileWriter, SETTINGS::BlockSettings *settings, FILES::AWDFile* file)
{
	// Read the body using concrete implementation in block sub-classes, and output the result
	return this->read_body(fileWriter, settings, file);
}

size_t
AWDBlock::write_block(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings *settings, FILES::AWDFile* file)
{
	if(settings->get_use_compression_per_block()){
		//TODO: add compression per block so we finally can support streaming
		/*
		FILE* inputfile=fileWriter->get_file();
		fileWriter->set_file(tmpfile());
		*/
	}
	else{
		// Calculate length of body using concrete implementation in block sub-classes
		TYPES::UINT32 length = this->calc_body_length(file, settings);
		this->byte_cnt=length;
		fileWriter->writeUINT32(length); // write length of body	
		// Write body using concrete implementation in block sub-classes
		this->write_body(fileWriter, settings, file);
		return length + 4;
	}
	return 0;
}
	
std::string&
AWDBlock::get_encountered_at()
{
	return this->encountered_at;
}
void
AWDBlock::set_encountered_at(const std::string& encountered_at)
{
	this->encountered_at = encountered_at;
}
std::string&
AWDBlock::get_script_name()
{
	return this->script_name;
}
void
AWDBlock::set_script_name(const std::string& script_name)
{
	this->script_name = script_name;
}
std::string&
AWDBlock::get_name()
{
	return this->name;
}
void
AWDBlock::set_name(const std::string& name)
{
	this->name = name;
}

std::string&
AWDBlock::get_external_id()
{
	return this->external_id;
}

void
AWDBlock::set_external_id(const std::string&external_id)
{
	this->external_id = external_id;
}

void*
AWDBlock::get_external_object()
{
	return this->external_object;
}

void
AWDBlock::set_external_object(void* external_object)
{
	this->external_object = external_object;
}

BLOCK::block_type
AWDBlock::get_type()
{
	return this->type;
}

void
AWDBlock::set_type(BLOCK::block_type type)
{
	this->type = type;
}

void
AWDBlock::add_category(BLOCK::category catergory)
{
	for(BLOCK::category cat:this->catergories){
		if(cat == catergory)
			return;
	}
	this->catergories.push_back(catergory);
}

bool
AWDBlock::is_category(BLOCK::category catergory)
{
	for(BLOCK::category cat:this->catergories){
		if(cat == catergory)
			return true;
	}
	return false;
}

result
AWDBlock::find_instance_for_file(FILES::AWDFile* file, BLOCK::BlockInstance** out_block_instance )
{
	if(file==NULL)
		return result::AWD_ERROR;
	if(!out_block_instance)
		return result::AWD_ERROR;
	for(BLOCK::blockInstance_file_connection block_file_connect : this->block_instances){
		if(block_file_connect.file==file){
			*out_block_instance = block_file_connect.block_instance;
			return result::AWD_SUCCESS;
		}
	}
	return result::AWD_ERROR;
}

result
AWDBlock::get_block_addr(FILES::AWDFile* file , TYPES::UINT32& output_address)
{	
	BLOCK::BlockInstance* blockInstance_from_Block;
	result res = file->find_block_instance(this, &blockInstance_from_Block);
	if(res==result::AWD_SUCCESS)
		output_address=blockInstance_from_Block->get_addr();
	return res;
}

result
AWDBlock::add_block_instance_for_file(FILES::AWDFile* file, BLOCK::BlockInstance* block_instance )
{
	if(file == NULL)
		return result::AWD_ERROR;
	if(block_instance==NULL)
		return result::AWD_ERROR;
	BLOCK::BlockInstance* existing_block_instance = NULL;
	result res = this->find_instance_for_file(file, &existing_block_instance);
	if(res == result::AWD_SUCCESS){
		// there already exists a block_instance for this AWDFile.
		// todo: we cannot acces BlockInstance properties or AWDfile properties from here, because both are only pre-declared.
		// use a util function to savly resolve this.
		return result::AWD_ERROR;
	}
	BLOCK::blockInstance_file_connection new_fileconnection = BLOCK::blockInstance_file_connection();
	new_fileconnection.block_instance = block_instance;
	new_fileconnection.file = file;
	this->block_instances.push_back(new_fileconnection);
	return result::AWD_SUCCESS;
}

TYPES::INT8 
AWDBlock::get_name_space_handle(){
	return this->ns_handle;
}

void
AWDBlock::set_name_space_handle(TYPES::INT8 ns_handle ){
	this->ns_handle = ns_handle;
}
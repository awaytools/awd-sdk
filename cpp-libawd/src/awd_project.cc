#include "awd_project.h"

#include <string>
#include <vector>

#include "blocks/namespace.h"
#include "blocks/mesh_library.h"
#include "blocks/billboard.h"
#include "base/state_element_base.h"
#include "files/awd_file.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

AWDProject::AWDProject(project_type new_project_type, const std::string& initial_path, const std::string& generator_name, const std::string& generator_version):
	BASE::StateElementBase()
{
	this->block_id_cnt=0;
	// initial NULL-pointer so deconstructor knows what to delete.
	this->name_space = NULL;
	this->active_file = NULL;

	/// \todo: fuck it
	// check if the path contains a directory
	std::string root_directory;
	result res = FILES::extract_directory_from_path(initial_path, root_directory);
	if(res!=result::AWD_SUCCESS){
		this->set_state(state::INVALID);
		this->add_message("AWDProject (constructor): inital path contains no valid directory", TYPES::message_type::ERROR_MESSAGE);
		return;
	}
	
	if((generator_name.empty())||(generator_version.empty())){
		this->set_state(state::INVALID);
		this->add_message("AWDProject (constructor): either generator_name or generator_version is empty", TYPES::message_type::ERROR_MESSAGE);
		return;
	}

	this->settings=new SETTINGS::Settings(root_directory, generator_name, generator_version);
	
	res = this->get_file_for_path(initial_path, &this->active_file);
	if(res!=result::AWD_SUCCESS){
		if(new_project_type!=project_type::MULTI_FILE_PROJECT){
			this->set_state(state::INVALID);
			this->add_message("AWDProject (constructor): No AWDFile was created (no valid string found for file-name)", TYPES::message_type::ERROR_MESSAGE);
			return;
		}
		this->add_message("AWDProject (constructor): No AWDFile was created", TYPES::message_type::WARNING_MESSAGE);
	}
	
}	

AWDProject::~AWDProject()
{
	for(blocktype_type iterator = this->all_blocks.begin(); iterator !=  this->all_blocks.end(); iterator++){
		for(AWDBlock* awd_block : iterator->second)
			delete awd_block;
	}
	for(AWDFile* awd_file : this->files)
		delete awd_file;
	if(this->name_space!=NULL)
		delete this->name_space;
}

// OVERWRITE STATEELEMENTBASE:

TYPES::state AWDProject::validate_state()
{

	/*
	this->set_state(state::VALID);
	for(AWDBlock* awd_block: this->blocks->get()){
		StateElementBase* state_element = reinterpret_cast<StateElementBase*>(awd_block);
		if(state_element!=NULL){
			TYPES::state block_state = state_element->get_state();
			if(block_state!=state::VALID){
				return;
			}
		}
		else{
			//Error
		}
	}
	*/
	return TYPES::state::VALID;
}

// PUBLIC:


result
AWDProject::get_blocks_for_external_ids()
{
	result res = result::AWD_SUCCESS;
	if (this->all_blocks.find(BLOCK::block_type::TIMELINE) == this->all_blocks.end())
		return result::AWD_ERROR;
	
	for(AWDBlock* one_awd_block : this->all_blocks[BLOCK::block_type::TIMELINE]){
		BLOCKS::Timeline* this_timeline = reinterpret_cast<BLOCKS::Timeline*>(one_awd_block);
		if(!this_timeline->is_finalized){
			this_timeline->is_finalized=true;
			this_timeline->set_fps(this->settings->get_fps());
			std::vector<ANIM::TimelineFrame*> thisFrames=this_timeline->get_frames();
			TYPES::UINT32 frame_cnt=1;
			for (ANIM::TimelineFrame * f : thisFrames) 
			{
				TYPES::UINT32 shape_cnt=1;
				std::vector<ANIM::FrameCommandBase*> thisCommands=f->get_commands();
				for (ANIM::FrameCommandBase * c : thisCommands) 
				{
					if((c->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD)||(c->get_command_type()==ANIM::frame_command_type::AWD_FRAME_COMMAND_SOUND)){
						std::string resID=c->get_resID();
						if (resID.c_str()!=NULL) {
							AWDBlock * thisBlock = this->get_block_by_external_id_shared(resID);
							if(thisBlock==NULL)
								res=result::AWD_ERROR;//todo: better error-managment
							else{
								if(thisBlock->get_type()==BLOCK::block_type::TEXT_ELEMENT){
									//ANIM::FrameCommandDisplayObject* dfc = reinterpret_cast<ANIM::FrameCommandDisplayObject*>(c);
									//thisBlock->set_name(dfc->get_instanceName());
								}
								else if(thisBlock->get_type()==BLOCK::block_type::SIMPLE_MATERIAL){
									// this is a bitmap texture. create a rectangle shape for it.
									BLOCKS::Material* thisMat = reinterpret_cast<BLOCKS::Material*>(thisBlock);
									BLOCKS::Billboard* thisBillBoard = reinterpret_cast<BLOCKS::Billboard*>(this->get_block_by_external_id_and_type(thisBlock->get_external_id(), block_type::BILLBOARD, true));
									thisBillBoard->set_name("billboard_"+thisMat->get_texture()->get_name());
									thisBillBoard->set_external_id(thisBlock->get_external_id());
									thisBillBoard->set_material(thisMat);
									thisBlock=thisBillBoard;
								}
								else if(thisBlock->get_type()==BLOCK::block_type::TRI_GEOM){
									BLOCKS::Geometry* this_geom = reinterpret_cast<BLOCKS::Geometry*>(thisBlock);
									ANIM::FrameCommandDisplayObject* dfc = reinterpret_cast<ANIM::FrameCommandDisplayObject*>(c);
									GEOM::MATRIX2x3* this_shape_mtx2x3=this_geom->has_res_id_geom(resID);
									if(this_shape_mtx2x3!=NULL){
										TYPES::F64* this_shape_mtx=this_shape_mtx2x3->get();
										TYPES::F64* this_mesh_mtx=dfc->get_display_matrix()->get();
										TYPES::F64* new_mesh_mtx=(TYPES::F64*)malloc(6*8);
										TYPES::F64 a1 =  this_mesh_mtx[0];
										TYPES::F64 b1 =  this_mesh_mtx[1];
										TYPES::F64 c1 =  this_mesh_mtx[2];
										TYPES::F64 d1 =  this_mesh_mtx[3];
										TYPES::F64 tx1 =  this_mesh_mtx[4];
										TYPES::F64 ty1 =  this_mesh_mtx[5];

										TYPES::F64 a2 =  this_shape_mtx[0];
										TYPES::F64 b2 =  this_shape_mtx[1];
										TYPES::F64 c2 =  this_shape_mtx[2];
										TYPES::F64 d2 =  this_shape_mtx[3];
										TYPES::F64 tx2 =  this_shape_mtx[4];
										TYPES::F64 ty2 =  this_shape_mtx[5];
										new_mesh_mtx[0]= a1 * a2 + b1 * c2;
										new_mesh_mtx[1]= a1 * b2 + b1 * d2;

										new_mesh_mtx[2]= c1 * a2 + d1 * c2;
										new_mesh_mtx[3]= c1 * b2 + d1 * d2;

										new_mesh_mtx[4]= tx1 * a2 + ty1 * c2 + tx2;
										new_mesh_mtx[5]= tx1 * b2 + ty1 * d2 + ty2;

										dfc->set_display_matrix(new_mesh_mtx);
									}
									// apply the matrix to already existing matrix, or apply as new one.
									thisBlock=this_geom->get_mesh_instance();
									shape_cnt++;
								}
								else if(thisBlock->get_type()==BLOCK::block_type::TIMELINE){
									BLOCKS::Timeline* this_timeline = reinterpret_cast<BLOCKS::Timeline*>(thisBlock);
									this_timeline->instance_cnt++;
								}
								c->set_object_block(thisBlock);
							}
						}
					}
				}
				frame_cnt+=f->get_frame_duration();
			}
		}
	}
	this->clear_external_ids();
	return res;
}


result 
AWDProject::open_preview(std::string& preview_file, std::vector<std::string> change_ids, std::vector<std::string> change_values, std::string& open_path, bool append_name)
{
	if(this->active_file==NULL)
		return result::AWDFILE_ERROR;
	return FILES::open_preview(this->active_file, preview_file, change_ids, change_values, open_path, append_name);
}

result 
AWDProject::get_file_for_path(const std::string& path, AWDFile**  awdFile){
	
	// check if the path contains a file
	std::string file_path = path;
	result res = validate_file_path(file_path, "awd");
	if(res != result::AWD_SUCCESS)
		return res;
	AWDFile* return_awd;
	if(awdFile!=NULL)
		return_awd=*awdFile;
	for(AWDFile* file : this->files){
		if(file->get_url()==file_path){
			return_awd = file;
			return result::AWD_SUCCESS;
		}
	}
	return_awd=new AWDFile(this, file_path);
	this->active_file = return_awd;
	this->files.push_back(return_awd);
	return result::AWD_SUCCESS;
}

result
AWDProject::export_file()
{
	if(this->active_file==NULL)
		return result::AWDFILE_ERROR;
	return this->active_file->write_to_disc();
}

SETTINGS::Settings* 
AWDProject::get_settings()
{
	return this->settings;
}

result 
AWDProject::find_block(BASE::AWDBlock* awd_block)
{
	if(awd_block==NULL)
		return result::AWD_ERROR;	
	if (this->all_blocks.find(awd_block->get_type()) == this->all_blocks.end())
		return result::AWD_ERROR;
	
	for(AWDBlock* one_awd_block : this->all_blocks[awd_block->get_type()]){
		if(one_awd_block==awd_block)
			return result::AWD_SUCCESS;
	}
	return result::AWD_ERROR;
}

result 
AWDProject::get_statistics(std::vector<std::string>& output_list)
{

	std::string stat_str = "\nAWDProject stats\n\n";
	stat_str+="	   Project-root-directory		=	'"+this->settings->get_root_directory()+"'\n";
	stat_str+="	   Project-texture-directory	=	'"+this->settings->get_texture_directory()+"'\n";
	stat_str+="	   Project-audio-directory		=	'"+this->settings->get_audio_directory()+"'\n";
	int block_cnt=0;
	for(blocktype_type iterator = this->all_blocks.begin(); iterator !=  this->all_blocks.end(); iterator++)
		block_cnt += iterator->second.size();
	stat_str+="	   Total AWDBlock count =	"+std::to_string(block_cnt)+"\n";
	stat_str+="	   Total AWDFiles count =	"+std::to_string(this->files.size())+"\n\n";
	
	output_list.push_back(stat_str);

	for(AWDFile* awd_file:this->files)
		awd_file->get_statistics(output_list);

	return result::AWD_SUCCESS;
}

result 
AWDProject::add_library_block(BASE::AWDBlock* awd_block)
{
	if(awd_block==NULL)
		return result::AWD_ERROR;
	if(this->find_block(awd_block)!=result::AWD_SUCCESS){
		// block doesn not yet exist in AWDProject
		if (this->all_blocks.find(awd_block->get_type()) == this->all_blocks.end())
			this->all_blocks[awd_block->get_type()] = std::vector<AWDBlock*>();		
		this->all_blocks[awd_block->get_type()].push_back(awd_block);
	}
	if(awd_block->is_category(BLOCK::category::SCENE_OBJECT)){
		if(awd_block->get_type()!=BLOCK::block_type::TIMELINE){
			SceneBlockBase* scene_block = reinterpret_cast<SceneBlockBase*>(awd_block);
			if(scene_block!=NULL){
				for(AWDBlock* child : scene_block->get_children()){
					this->add_library_block(child);
				}
			}
		}
	}
	return result::AWD_SUCCESS;

}

result 
AWDProject::add_block(BASE::AWDBlock* awd_block, BLOCK::BlockInstance** blockInstance)
{
	if(this->active_file==NULL)
		return result::AWD_ERROR;

	if(awd_block==NULL)
		return result::AWD_ERROR;

	BLOCK::BlockInstance* this_block_inst=NULL;
	if(blockInstance)
		*blockInstance=this_block_inst;

	awd_block->inc_id = this->block_id_cnt++;;
	if(this->active_file->add_block(awd_block, BLOCK::instance_type::BLOCK_EMBBED, &this_block_inst)!=result::AWD_SUCCESS)
		return result::AWD_ERROR;

	if(awd_block->add_block_instance_for_file(this->active_file, this_block_inst)!=result::AWD_SUCCESS)
		return result::AWD_ERROR;

	this->add_library_block(awd_block);

	return result::AWD_SUCCESS;
}

result
AWDProject::get_blocks_by_type(std::vector<AWDBlock*>& blocks , BLOCK::block_type blocktype){
	if (this->all_blocks.find(blocktype) == this->all_blocks.end())
		return result::NO_BLOCKS_FOUND;
	blocks=this->all_blocks[blocktype];
	return result::AWD_SUCCESS;
}

result
AWDProject::clear_external_ids(){
	for(blocktype_type iterator = this->all_blocks.begin(); iterator !=  this->all_blocks.end(); iterator++) {
		if(iterator->second.size()>0){
			for(AWDBlock* awd_block : iterator->second){
				awd_block->clear_res_ids();
				if(awd_block->get_type()==block_type::TRI_GEOM){
					BLOCKS::Geometry* this_geom = reinterpret_cast<BLOCKS::Geometry* >(awd_block);
					this_geom->clear_res_ids_geom();
				}
			}
		}
	}
	return result::AWD_SUCCESS;
}
result
AWDProject::get_blocks(std::vector<AWDBlock*>& blocks){
	blocks = std::vector<AWDBlock*>();
	for(blocktype_type iterator = this->all_blocks.begin(); iterator !=  this->all_blocks.end(); iterator++) {
		if(iterator->second.size()>0){
			for(AWDBlock* awd_block : iterator->second)
				blocks.push_back(awd_block);
		}
	}
	return result::AWD_SUCCESS;
}

Material*
AWDProject::get_default_material_by_color(TYPES::COLOR this_color, bool create_if_not_exists, MATERIAL::type mat_type)
{
	if (this->all_blocks.find(BLOCK::block_type::SIMPLE_MATERIAL) == this->all_blocks.end()){
		if(create_if_not_exists){
			this->all_blocks[BLOCK::block_type::SIMPLE_MATERIAL]=std::vector<AWDBlock*>();
		}
		else
			return NULL;
	}
	for(AWDBlock* block : this->all_blocks[BLOCK::block_type::SIMPLE_MATERIAL]){
		Material* mat = reinterpret_cast<Material*>(block);
		if(mat!=NULL){
			if(mat->get_isDefault()){
				if(mat->get_material_type()==mat_type){
					if(mat->get_color()==this_color)		
						return mat;
				}
			}
		}
	}
	if(create_if_not_exists){
		Material* new_mat = new BLOCKS::Material();
		new_mat->set_material_type(mat_type);
		new_mat->set_color(this_color);
		this->add_block(new_mat);
		return new_mat;
	}
	return NULL;
}

	
		
BLOCKS::Timeline*
AWDProject::get_timeline_by_symbol_name(const std::string& symbol_name)
{
	if (this->all_blocks.find(block_type::TIMELINE) == this->all_blocks.end()){
		return NULL;
	}
	for(AWDBlock* block : this->all_blocks[block_type::TIMELINE]){
		BLOCKS::Timeline* this_timeline=reinterpret_cast<BLOCKS::Timeline*>(block);
		if(this_timeline!=NULL){
			if(this_timeline->get_symbol_name()==symbol_name){	
				return this_timeline;
			}
		}
	}
	return NULL;
}

AWDBlock*
AWDProject::get_block_by_external_object_and_type(void* extern_obj, BLOCK::block_type blocktype, bool create_if_not_exists)
{
	if (this->all_blocks.find(blocktype) == this->all_blocks.end()){
		if(create_if_not_exists){
			this->all_blocks[blocktype]=std::vector<AWDBlock*>();
		}
		else
			return NULL;
	}
	for(AWDBlock* block : this->all_blocks[blocktype]){
		if(block->get_external_object()==extern_obj){			
			return block;
		}
	}
	if(create_if_not_exists){
		AWDBlock* newBlock;
		result res = BLOCK::create_block_for_block_type(&newBlock, blocktype, this->active_file);
		if(res!=result::AWD_SUCCESS)
			return NULL;
		res=this->add_block(newBlock);
		newBlock->set_external_object(extern_obj);
		return newBlock;
	}
	return NULL;
}
AWDBlock*
AWDProject::get_block_by_name_and_type(const std::string& name, BLOCK::block_type blocktype, bool create_if_not_exists)
{
	if (this->all_blocks.find(blocktype) == this->all_blocks.end()){
		if(create_if_not_exists){
			this->all_blocks[blocktype]=std::vector<AWDBlock*>();
		}
		else
			return NULL;
	}
	for(AWDBlock* block : this->all_blocks[blocktype]){
		if(block->get_name()==name){			
			return block;
		}
	}
	if(create_if_not_exists){
		AWDBlock* newBlock;
		result res = BLOCK::create_block_for_block_type(&newBlock, blocktype, this->active_file);
		if(res!=result::AWD_SUCCESS)
			return NULL;
		res=this->add_block(newBlock);
		newBlock->set_name(name);
		return newBlock;
	}
	return NULL;
}

AWDBlock*
AWDProject::get_block_by_external_id_shared(const std::string& externalID)
{
	for(blocktype_type iterator = this->all_blocks.begin(); iterator !=  this->all_blocks.end(); iterator++) {
		if(iterator->second.size()>0){
			for(AWDBlock* awd_block : iterator->second){
				if(awd_block->has_res_id(externalID))
					return awd_block;
			}
		}
	}
	return NULL;
}
AWDBlock*
AWDProject::get_block_by_external_id(const std::string& externalID)
{
	for(blocktype_type iterator = this->all_blocks.begin(); iterator !=  this->all_blocks.end(); iterator++) {
		if(iterator->second.size()>0){
			for(AWDBlock* awd_block : iterator->second){
				if(awd_block->get_external_id()==externalID)
					return awd_block;
			}
		}
	}
	return NULL;
}

AWDBlock*
AWDProject::get_block_by_external_id_and_type_shared(const std::string& externalID, BLOCK::block_type blocktype, bool create_if_not_exists)
{
	if (this->all_blocks.find(blocktype) == this->all_blocks.end()){
		if(create_if_not_exists){
			this->all_blocks[blocktype]=std::vector<AWDBlock*>();
		}
		else
			return NULL;
	}
	for(AWDBlock* block : this->all_blocks[blocktype]){
		if(block->has_res_id(externalID)){			
			return block;
		}
	}
	if(create_if_not_exists){
		AWDBlock* newBlock;
		result res = BLOCK::create_block_for_block_type(&newBlock, blocktype, this->active_file);
		if(res!=result::AWD_SUCCESS)
			return NULL;
		res=this->add_block(newBlock);
		newBlock->add_res_id(externalID);
		return newBlock;
	}
	return NULL;
}
AWDBlock*
AWDProject::get_block_by_external_id_and_type(const std::string& externalID, BLOCK::block_type blocktype, bool create_if_not_exists)
{
	if (this->all_blocks.find(blocktype) == this->all_blocks.end()){
		if(create_if_not_exists){
			this->all_blocks[blocktype]=std::vector<AWDBlock*>();
		}
		else
			return NULL;
	}
	for(AWDBlock* block : this->all_blocks[blocktype]){
		if(block->get_external_id()==externalID){			
			return block;
		}
	}
	if(create_if_not_exists){
		AWDBlock* newBlock;
		result res = BLOCK::create_block_for_block_type(&newBlock, blocktype, this->active_file);
		if(res!=result::AWD_SUCCESS)
			return NULL;
		res=this->add_block(newBlock);
		newBlock->set_external_id(externalID);
		return newBlock;
	}
	return NULL;
}


result
AWDProject::open(){
	if(this->active_file==NULL)
		return result::AWD_ERROR;
	return this->active_file->open();
}

result
AWDProject::get_files(std::vector<AWDFile*>& files){
	files = this->files;
	return result::AWD_SUCCESS;
}

result
AWDProject::get_namespace(BLOCKS::Namespace * name_space)
{
	if(this->name_space==NULL)
		return result::AWD_ERROR;
	name_space=this->name_space;
	return result::AWD_SUCCESS;
}


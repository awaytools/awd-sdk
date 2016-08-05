#include "files/awd_file.h"

#ifdef _WIN32	
#include <Windows.h>
#include "ShellApi.h"
#endif

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
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
	/*
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
								if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::MOVIECLIP){
									BLOCKS::MovieClip* timeline = reinterpret_cast<BLOCKS::MovieClip*>( blockInstance->get_awdBlock());
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
	}*/
	if((this->awd_project->get_settings()->get_bool(SETTINGS::bool_settings::ExportLibSounds))||(this->awd_project->get_settings()->get_bool(SETTINGS::bool_settings::ExportTimelineSounds))){

		for(BlockInstance* blockInstance : unordered_blocks){
			if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::SOUND_SOURCE){
				if(blockInstance->get_awdBlock()->get_state()==state::VALID){
					if(blockInstance->get_process_state()==process_state::UNPROCESSED)
						blockInstance->add_with_dependencies();
				}
			}
		}
	}
	
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::BITMAP_TEXTURE){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::SIMPLE_MATERIAL){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::BILLBOARD){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::FONT){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				BLOCKS::Font * awd_font = reinterpret_cast<BLOCKS::Font *>(blockInstance->get_awdBlock());
				if(awd_font->delegate_to_font==NULL){
					if(blockInstance->get_process_state()==process_state::UNPROCESSED)
						blockInstance->add_with_dependencies();
				}
			}
		}
	}
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::TEXT_FORMAT){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::TEXT_ELEMENT){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}

	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::TRI_GEOM){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::MESH_INSTANCE_2){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED)
					blockInstance->add_with_dependencies();
			}
		}
	}
	
	// now export all library-symbols. 
	// this are timelines that are not on the stage of any other timeline, and are no scene	
	for(BlockInstance* blockInstance : unordered_blocks){
		if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::MOVIECLIP){
			if(blockInstance->get_awdBlock()->get_state()==state::VALID){
				if(blockInstance->get_process_state()==process_state::UNPROCESSED){
					BLOCKS::MovieClip* timeline = reinterpret_cast<BLOCKS::MovieClip*>( blockInstance->get_awdBlock());
					if(!timeline->is_grafic_instance){
						if((timeline->instance_cnt==0)&&(!timeline->get_is_scene()))
							blockInstance->add_with_dependencies();
					}
				}
			}
		}
	}

	// now export all scenes 
	//if(this->settings->get_export_all()){
		for(BlockInstance* blockInstance : unordered_blocks){
			//if((!this->settings->get_export_materials())&&(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::SIMPLE_MATERIAL)){
			//	continue;
			//}
			//else{
				if(blockInstance->get_state()!=state::INVALID){
					if(blockInstance->get_process_state()==process_state::UNPROCESSED){
						if(blockInstance->get_awdBlock()->is_category(BLOCK::category::SCENE_OBJECT)){
							if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::MOVIECLIP){
								BLOCKS::MovieClip* timeline = reinterpret_cast<BLOCKS::MovieClip*>( blockInstance->get_awdBlock());
								if(!timeline->is_grafic_instance){
									if(timeline->get_is_scene())
										blockInstance->add_with_dependencies();
								}
							}
						}
					}
				}
			//}
		}
	//}
		if((this->settings->get_bool(bool_settings::ExportFrameScript))||(this->settings->get_bool(bool_settings::ExternalScripts))){
			
		int scripts_cnt=0;
		for(BlockInstance* blockInstance : this->block_instances){
			if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::MOVIECLIP){
				BLOCKS::MovieClip* thisMC = reinterpret_cast<BLOCKS::MovieClip*>(blockInstance->get_awdBlock());
				scripts_cnt+=thisMC->frame_scripts_stream.size();
			}
		}
		if(scripts_cnt>0){

			std::ofstream ousrfile;
			std::string path_no_extension;
			FILES::extract_path_without_file_extension( this->url, path_no_extension);
			std::string script_path=path_no_extension+"_scripts.js";
			std::string file_name;
			FILES::extract_name_from_path( path_no_extension, file_name);
		
			if(this->settings->get_bool(bool_settings::ExternalScripts)){
				ousrfile.open(script_path.c_str());
				//ousrfile << "(function e(t,n,r){function s(o,u){if(!n[o]){if(!t[o]){var a=typeof require==\"function\"&&require;if(!u&&a)return a(o,!0);if(i)return i(o,!0);var f=new Error(\"Cannot find module '\"+o+\"'\");throw f.code=\"MODULE_NOT_FOUND\",f}var l=n[o]={exports:{}};t[o][0].call(l.exports,function(e){var n=t[o][1][e];return s(n?n:e)},l,l.exports,e,t,n,r)}return n[o].exports}var i=typeof require==\"function\"&&require;for(var o=0;o<r.length;o++)s(r[o]);return s})({\"./src/Main_scripts.ts\":[function(require,module,exports){\n";
    			ousrfile << "Color = require('awayjs-player/lib/adapters/AS2ColorAdapter');\n";
				ousrfile << "System = require('awayjs-player/lib/adapters/AS2SystemAdapter');\n";
				ousrfile << "Sound = require('awayjs-player/lib/adapters/AS2SoundAdapter');\n";
				ousrfile << "Key = require('awayjs-player/lib/adapters/AS2KeyAdapter');\n";
				ousrfile << "Mouse = require('awayjs-player/lib/adapters/AS2MouseAdapter');\n";
				ousrfile << "Stage = require('awayjs-player/lib/adapters/AS2StageAdapter');\n";
				ousrfile << "SharedObject = require('awayjs-player/lib/adapters/AS2SharedObjectAdapter');\n";
				ousrfile << "int = function (value) { return Math.floor(value) | 0; };\n";
				ousrfile << "string = function (value) { return value.toString(); };\n";
				ousrfile << "getURL = function (value) { return value; };\n";
				ousrfile << "\nvar "+file_name+"_scripts = {\n";
			}

			for(BlockInstance* blockInstance : this->block_instances){
				if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::MOVIECLIP){
					BLOCKS::MovieClip* timeline = reinterpret_cast<BLOCKS::MovieClip*>(blockInstance->get_awdBlock());
					int cnt_scripts=0;
					std::vector<std::string> new_script_stream;
					for(std::string original_script: timeline->frame_scripts_stream){

						std::string functionMCName=timeline->frame_scripts_mc_names[cnt_scripts];
						TYPES::UINT32 functionindexe=timeline->frame_scripts_frameindex[cnt_scripts++];
						std::string functionName=FILES::RemoveSpecialCharacters(functionMCName) + "_f" + std::to_string(functionindexe);
					
						if(this->settings->get_bool(bool_settings::ExternalScripts)){
							ousrfile << "\n    //AnimateCC: Symbol: "+functionMCName+" #frame: "+std::to_string(functionindexe);
							ousrfile << "\n    '"+ functionName +"' : function(){\n";
							std::istringstream iss(original_script);
							for (std::string line; std::getline(iss, line); )
							{
								//if(line!="var _this = this;"){
								ousrfile << "        "+line+"\n";
								//}
							}
							ousrfile << "    },\n";
						}
						std::string updatedScript="//"+functionName+"\n"+original_script;
						new_script_stream.push_back(updatedScript);
					}
					timeline->frame_scripts_stream.clear();
					if(this->settings->get_bool(bool_settings::ExportFrameScript)){
						for(std::string new_script: new_script_stream){
							timeline->frame_scripts_stream.push_back(new_script);
						}
					}
				}
			}
			
			if(this->settings->get_bool(bool_settings::ExternalScripts)){
				ousrfile << "};\n";
				//ousrfile << "export = Framescripts;\n";
				/*
				ousrfile << "	return Framescripts;\n";
				ousrfile << "})();\n";
				ousrfile << "module.exports = Framescripts;";
				ousrfile << "},{\"awayjs-player/lib/adapters/AS2ColorAdapter\":undefined,\"awayjs-player/lib/adapters/AS2KeyAdapter\":undefined,\"awayjs-player/lib/adapters/AS2MouseAdapter\":undefined,\"awayjs-player/lib/adapters/AS2SharedObjectAdapter\":undefined,\"awayjs-player/lib/adapters/AS2SoundAdapter\":undefined,\"awayjs-player/lib/adapters/AS2StageAdapter\":undefined,\"awayjs-player/lib/adapters/AS2SystemAdapter\":undefined}]},{},[\"./src/Main_scripts.ts\"])";
		
				*/

				ousrfile.close();
			}
		}
	}
	TYPES::UINT32 addr_cnt=0;
	for(BlockInstance* blockInstance : this->block_instances){
		blockInstance->set_addr(addr_cnt);
		addr_cnt++;
	}
	if(this->settings->get_bool(SETTINGS::bool_settings::CreateAudioMap)){
		std::string path_no_extension;
		FILES::extract_path_without_file_extension( this->url, path_no_extension);
		std::string audio_map_path=path_no_extension+"_audio_map.txt";
		std::ofstream ousrfile_audio;
		ousrfile_audio.open(audio_map_path.c_str());
		ousrfile_audio << "Audio-map:";
		for(BlockInstance* blockInstance : this->block_instances){
			if(blockInstance->get_awdBlock()->get_type()==BLOCK::block_type::SOUND_SOURCE){
				BLOCKS::Audio* audio_block = reinterpret_cast<BLOCKS::Audio*>(blockInstance->get_awdBlock());
				std::string filename;
				FILES::extract_name_from_path(audio_block->get_url(), filename);
				std::string map_string=audio_block->get_name()+"="+filename+"\n";
				ousrfile_audio << map_string;
			}
		}
		ousrfile_audio.close();
	}
	//this->awd_project->shared_geom->set_addr_on_subgeom(this);
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
AWDFile::get_statistics_for_type(BLOCK::block_type block_type, std::vector<std::string>& output_list)
{
	std::vector<BASE::AWDBlock*> blocks;
	this->awd_project->get_blocks_by_type(blocks, block_type);
	
	std::string output_str="	";
	std::string sizeLength=std::to_string(blocks.size());
	for(int i=0; i<6;i++){
		if(sizeLength.size()>i)
			output_str+=sizeLength[i];
		else
			output_str+=" ";
	}
	output_str+="x ";
	std::string asset_type_str;
	get_asset_type_as_string(block_type, asset_type_str);
	for(int i=0; i<15;i++){
		if(asset_type_str.size()>i)
			output_str+=asset_type_str[i];
		else
			output_str+=" ";
	}
	
	int this_bytes=0;
	for(AWDBlock* awd_block : blocks){
		this_bytes += awd_block->byte_cnt;
	}
	output_str+=" - bytes: ";
	std::string bytes_length=std::to_string(this_bytes);
	for(int i=0; i<20;i++){
		if(bytes_length.size()>i)
			output_str+=bytes_length[i];
		else
			output_str+=" ";
	}
	output_list.push_back(output_str);
	return result::AWD_SUCCESS;

}
result 
AWDFile::get_statistics(std::vector<std::string>& output_list)
{
	std::string stat_str =	"AWDFile stats\n";
	stat_str += "	Output-url = '"+this->get_settings()->get_root_directory()+"'\n";
	stat_str += "	AWDBlockInstances =	"+std::to_string(this->get_block_cnt())+"\n";
	int all_bytes=0;
	output_list.push_back(stat_str);
	for(BlockInstance* block_inst : this->block_instances){
		all_bytes+=block_inst->get_awdBlock()->byte_cnt;
		std::string asset_type_str;
		get_asset_type_as_string(block_inst->get_awdBlock()->get_type(), asset_type_str);
		
		std::string asset_type_output;
		for(int i=0; i<15;i++){
			if(asset_type_str.size()>i)
				asset_type_output+=asset_type_str[i];
			else
				asset_type_output+=" ";
		}
		std::string additional_infos="";
		
		std::string usage_str;
		block_inst->get_awdBlock()->get_scene_names(usage_str);
		std::string usage = "";
		if(usage_str.size()>0)
			usage=" | usage = '"+usage_str+"'";
		std::string this_name="";
		if(block_inst->get_awdBlock()->get_name().size()>0)
			this_name=" | name = '"+block_inst->get_awdBlock()->get_name()+"'";;
		
		if(block_inst->get_awdBlock()->get_type()==block_type::TRI_GEOM){
			BLOCKS::Geometry* thisGeom=reinterpret_cast<BLOCKS::Geometry*>(block_inst->get_awdBlock());
			additional_infos+=" | subgeo-count = '"+std::to_string(thisGeom->get_num_subs());
		}
		if(block_inst->get_awdBlock()->get_type()==block_type::TEXT_ELEMENT){
			BLOCKS::TextElement* thisText=reinterpret_cast<BLOCKS::TextElement*>(block_inst->get_awdBlock());
			additional_infos+=" | text = '"+thisText->get_text()+"'";
		}
		if(block_inst->get_awdBlock()->get_type()==block_type::FONT){
			BLOCKS::Font* thisfont=reinterpret_cast<BLOCKS::Font*>(block_inst->get_awdBlock());
			additional_infos+=" | font-styles: '";
			for(FONT::FontStyle* fs: thisfont->get_font_styles()){
				additional_infos+=fs->get_style_name();
				if(fs!=thisfont->get_font_styles().back())
					additional_infos+=", ";
			}
			additional_infos+="'";
		}
		if(block_inst->get_awdBlock()->get_type()==block_type::TEXT_FORMAT){
			BLOCKS::TextFormat* thisfont=reinterpret_cast<BLOCKS::TextFormat*>(block_inst->get_awdBlock());
			additional_infos+=" | font: "+thisfont->get_font()->get_name();
			additional_infos+=" | font-style: "+thisfont->get_fontStyle();
			additional_infos+=" | size: "+std::to_string(thisfont->get_fontSize());
			additional_infos+=" | spacing: "+std::to_string(thisfont->get_letterSpacing());
		}
		if(block_inst->get_awdBlock()->get_type()==block_type::BITMAP_TEXTURE){
			BLOCKS::BitmapTexture* thisbitmap=reinterpret_cast<BLOCKS::BitmapTexture*>(block_inst->get_awdBlock());
			if(thisbitmap->get_storage_type()==BLOCK::storage_type::EMBEDDED){
				additional_infos+=" | embbed ("+std::to_string(double(double(thisbitmap->embed_data_len)/double(1024)))+" kb)";
			}
			else{
				additional_infos+=" | external url = '"+thisbitmap->get_url()+"'";
			}
		}
		if(block_inst->get_awdBlock()->get_type()==block_type::SOUND_SOURCE){
			BLOCKS::Audio* this_sound=reinterpret_cast<BLOCKS::Audio*>(block_inst->get_awdBlock());
			if(this_sound->get_storage_type()==BLOCK::storage_type::EMBEDDED){
				additional_infos+=" | embbed ("+std::to_string(double(double(this_sound->embed_data_len)/double(1024)))+" kb)";
			}
			else{
				additional_infos+=" | external url = '"+this_sound->get_url()+"'";
			}
		}

		std::string stat_str_block = "	ID = "+std::to_string(block_inst->get_addr())+" | "+asset_type_output + " | "+std::to_string((block_inst->get_awdBlock()->byte_cnt))+" b" + usage + this_name + additional_infos;
		output_list.push_back(stat_str_block);
		
	}
	std::string stat_str_block = "\n All blocks bytes: "+std::to_string(all_bytes)+"\n";
	output_list.push_back(stat_str_block);
	get_statistics_for_type(block_type::MOVIECLIP, output_list);
	get_statistics_for_type(block_type::BITMAP_TEXTURE, output_list);
	get_statistics_for_type(block_type::SOUND_SOURCE, output_list);
	get_statistics_for_type(block_type::FONT, output_list);
	get_statistics_for_type(block_type::TEXT_ELEMENT, output_list);
	get_statistics_for_type(block_type::TEXT_FORMAT, output_list);
	get_statistics_for_type(block_type::TRI_GEOM, output_list);
	get_statistics_for_type(block_type::BILLBOARD, output_list);
	get_statistics_for_type(block_type::SIMPLE_MATERIAL, output_list);
	get_statistics_for_type(block_type::MESH_INSTANCE_2, output_list);

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
		return result::FILEREADER_ERROR_READING_BYTES;
	// set the filewriter to write to a tmp file.
	
	// on windows this gives a warning that we should use tmpfile_s instead, but because this works on mac and win, i left it as is
	FILES::FileWriter* fileWriter = new FileWriter();
	res = fileWriter->set_file(tmpfile());
	if(res!=result::AWD_SUCCESS)
		return result::FILEREADER_ERROR_GETTING_POS;
	
	// write all blocks into the tmp file
	TYPES::UINT32 body_length = 0;
	for(BlockInstance* blockInstance : this->block_instances){
	//	if((blockInstance->get_awdBlock()->get_type()!=BLOCK::block_type::MOVIECLIP)&&(blockInstance->get_awdBlock()->get_type()!=BLOCK::block_type::SIMPLE_MATERIAL))
		body_length += blockInstance->write_block(fileWriter);
	}
	
	// get the optionally compressed byte-data for the body of the file.
	TYPES::UINT8* body_buf = NULL;
	res = fileWriter->get_compressed_bytes(&body_buf, this->settings->get_compression(), body_length);
	if(res!=result::AWD_SUCCESS)
		return result::PATH_NOT_CLOSING;

	// set the file-writer to use the output file
	res = fileWriter->open_file(this->url);
	if(res!=result::AWD_SUCCESS)
		return result::FILEREADER_ERROR_CLOSING_FILE;
	
	// write the file-header into the output file
	std::string magic_string="AWD";
	res = fileWriter->writeSTRING(magic_string, write_string_with::NO_LENGTH_VALUE);
	if(res!=result::AWD_SUCCESS)
		return result::RESULT_STRING_NOT_FOUND;
	res =fileWriter->writeUINT8(AWD::VERSION_MAJOR);
	if(res!=result::AWD_SUCCESS)
		return result::READ_ERROR;
	res =fileWriter->writeUINT8(AWD::VERSION_MINOR);
	if(res!=result::AWD_SUCCESS)
		return result::COULD_NOT_DECIDE_ON_CURVE_TYPE;
	res =fileWriter->writeUINT16(this->settings->get_file_header_flag());
	if(res!=result::AWD_SUCCESS)
		return result::BLOCK_INVLAID;
	res =fileWriter->writeUINT8(TYPES::UINT8(this->settings->get_compression()));
	if(res!=result::AWD_SUCCESS)
		return result::DIFFERENT_PROPERTY_VALUE;
	
	// write length of body
	res = fileWriter->writeUINT32(body_length);	
	if(res!=result::AWD_SUCCESS)
		return result::WRITE_WARNING;

	// write body
	res = fileWriter->writeBytes(body_buf, body_length);	
	if(res!=result::AWD_SUCCESS)
		return result::DIFFERENT_PROPERTY_VALUE;
	
	//delete body_buffer
    // todo: on mac freeing the buffer causes unexpected behaviour
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


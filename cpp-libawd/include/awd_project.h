#ifndef _LIBAWD_AWD_PROJECT_H
#define _LIBAWD_AWD_PROJECT_H

#include <string>
#include <vector>
#include <map>

#include "base/block.h"
#include "base/state_element_base.h"

#include "base/attr.h"
#include "files/awd_file.h"

#include "blocks/anim_set.h"
#include "blocks/animator.h"
#include "blocks/audio.h"
#include "blocks/bitmap_texture.h"
#include "blocks/billboard.h"
#include "blocks/camera.h"
#include "blocks/command.h"
#include "blocks/cube_texture.h"
#include "blocks/effect_method.h"
#include "blocks/font.h"
#include "blocks/geometry.h"
#include "blocks/light.h"
#include "blocks/light_picker.h"
#include "blocks/material.h"
#include "blocks/mesh_inst.h"
#include "blocks/meta.h"
#include "blocks/namespace.h"
#include "blocks/objectcontainer.h"
#include "blocks/primitive.h"
#include "blocks/scene.h"
#include "blocks/ConcanetedStream.h"
#include "blocks/shadows.h"
#include "blocks/skeleton.h"
#include "blocks/skeleton_anim_clip.h"
#include "blocks/skeleton_pose.h"
#include "blocks/skybox.h"
#include "blocks/texture_projector.h"
#include "blocks/text_element.h"
#include "blocks/text_format.h"
#include "blocks/movieclip.h"
#include "blocks/uv_anim_clip.h"
#include "blocks/uv_pose.h"
#include "blocks/vertex_anim_clip.h"
#include "blocks/vertex_pose.h"
#include <iostream>

#ifdef _WIN32
#include <ctime>
#endif
//#include "utils/libAWD.h"



/** \namespace AWD
*	\brief The namespace used for the complete libawd.
*/
namespace AWD
{
	

	typedef std::map<BLOCK::block_type, std::vector<AWDBlock*> >::iterator blocktype_type;
	/** \class AWDProject
	*	\brief The main interface into the libawd. \n
	* AWDProject holds a list of pointers to BLOCKS and a list of pointers to #AWD::FILES::AWDFile.
	* 
	*/
	class AWDProject:
		public BASE::StateElementBase
	{
		private:

			std::map<BLOCK::block_type, std::vector<BASE::AWDBlock*> > all_blocks;	/// < all blocks in the AWDProject. ordered by BLOCK::block_type
			
			std::vector<FILES::AWDFile*> files;		/// < The list of files. all AWDFiles will be deleted on deconstruct
			int block_id_cnt;
			result find_block(BASE::AWDBlock*);		/// < Add a AWDBLock to the list of blocks
			BLOCKS::Namespace * name_space;			/// < This name_space_block should contain all names_spaces used on all AWDfile. It acts as name_space-library and is not added to the block-list.
			SETTINGS::Settings * settings;			/// < the settings for this project. They might be shared or copied for the files. Files that share settings do not need to delete them in deconstructor.
			FILES::AWDFile* active_file;			/// < the file that is currently active for this project.	
			double current_time;
			std::clock_t clock;
			
		protected:
			TYPES::state validate_state();
			

		public:
			/**
			*	\brief Create a new AWDProject.
			* There is no singleton behaviour enforced, but the libAWD was not created with multiton behaviour in mind.
			*	@param[in] new_project_type The TYPES::project_type for this AWDProject. Defines the inital behaviour of the AWDPoroject.
			*	@param[in] initial_path Defines the root_directory for this AWDProject. If the path contains a file-name, a AWDFile for this filename will be created. If the path contains no file-name no AWDFile will be created.
			*	@param[in] generator_name The name of the application thats making use of the libAWD. This information will be written into the meta-dat of all created AWDFile.
			*	@param[in] generator_version The version of the application thats making use of the libAWD. This information will be written into the meta-dat of all created AWDFile.
			* 
			*/
			AWDProject(TYPES::project_type new_project_type, const std::string& initial_path, const std::string& generator_name, const std::string& generator_version );
			~AWDProject();
			
			result open_preview(std::string& source_directory,  std::vector<std::string> change_ids, std::vector<std::string> change_values, std::string& open_path, bool append_name);
			
			/**
			* \brief Open the current active file with system default application
			* @return result 
			*/
			result open();
			
			BLOCKS::Geometry* shared_geom;
			/**
			* \brief Framecommands might generated using ressource-ids for referencing the objects instead of connected them directly to the objects AWDBlocks.\n
			* In this case, this function must be called before export.
			* @return result 
			*/
			int get_time_since_last_call();
			/**
			* \brief Get the statistic of the AWDProject.
			*	@param[out] statistics_str The string that should get the statistics copied into it.
			*	@resultAWD::result
			*/
			result get_statistics(std::vector<std::string>& statistics_str);	
			
			/**
			* \brief get the settings used for this file
			*/
			SETTINGS::Settings* get_settings();

			/**
			* \brief Set a AWDFile as active-file.
			*	@param[in] awd_file The AWDFile to make active
			*	@resultAWD::result
			*/
			result set_active_file(FILES::AWDFile* awd_file);		

			/**
			* \brief Exports the active-file to disc. Before exporting, the AWDFile will be processed if needed.
			* 
			*	@resultAWD::result
			*/
			result export_file();
			
			result exchange_timeline_by_name(BLOCKS::MovieClip*);	
			
			result create_merged_streams();

			result finalize_timelines();	
			/**
			* \brief Imports the active-file. This should result in a AWDFile that is fully processed and ready to export again.
			* 
			*	@resultAWD::result
			*/
			result import_file();						

			/**
			* \brief Get a AWDFile for a given path. The AWDFile is created if it does not exists. If the path is invalid, no AWDFile will be created.
			*	@param[in] path The global path of the AWDFile.
			* 
			*	@param[out] files [optional] Pointer to the AWDFile for the path
			*	@resultAWD::result
			*/
			result get_file_for_path(const std::string& path, FILES::AWDFile** = NULL);
			/**
			* \brief Get a AWDFile for a given relative path. The AWDFile is created if it does not exists. If the path is invalid, no AWDFile will be created.
			*	@param[in] path The relative path of the AWDFile. The path must be relative to the Root 
			*	@param[out] files [optional] Pointer to the AWDFile for the path
			*	@resultAWD::result
			*/
			result get_file_for_relative_path(const std::string& path, FILES::AWDFile* = NULL);
			
			/**
			* \brief Get the list of AWDFiles
			*	@param[out] files The pointer to the FILES::FilesList that holds all AWDFile existing inside the AWDProject
			*	@return AWD::result
			*/
			result get_files(std::vector<FILES::AWDFile*>& files);
			
			/**
			* \brief Get the list of AWDBlock
			*	@param[out] files The list of AWDBlock inside the AWDProject
			*	@return AWD::result
			*/
			result get_blocks(std::vector<BASE::AWDBlock*>& blocks);
			
			/**
			* \brief Clear all external ids that are set on AWDBlocks.
			*/
			result clear_external_ids();
			/**
			* \brief Get the list of AWDBlock by type
			*	@param[out] files The list of AWDBlock inside the AWDProject
			*	@return AWD::result
			*/
			result get_blocks_by_type(std::vector<BASE::AWDBlock*>& blocks , BLOCK::block_type);
			
			/**
			* \brief Get AWDBlock for a external-id (string) and type
			*	@return AWD::result
			*/
			BASE::AWDBlock* get_block_by_external_id(const std::string& externalID);
			BASE::AWDBlock* get_block_by_external_id_shared(const std::string& externalID);
			
			/**
			* \brief Get AWDBlock for a external-id (string) and type
			*	@return AWD::result
			*/
			BASE::AWDBlock* get_block_by_external_id_and_type(const std::string& externalID, BLOCK::block_type,  bool create_if_not_exists);
			BASE::AWDBlock* get_block_by_external_id_and_type_shared(const std::string& externalID, BLOCK::block_type,  bool create_if_not_exists);
			/**
			* \brief Get AWDBlock for a name (string) and type
			*	@return AWD::result
			*/
			BASE::AWDBlock* get_block_by_name_and_type(const std::string& name, BLOCK::block_type,  bool create_if_not_exists);
			
			BLOCKS::MovieClip* get_timeline_by_symbol_name(const std::string& name);
			BASE::AWDBlock* get_block_by_external_object_and_type(void* extern_obj, BLOCK::block_type blocktype, bool create_if_not_exists);
			/**
			* \brief Get Material for a color 
			*	@return AWD::result
			*/
			BLOCKS::Material* get_default_material_by_color(TYPES::COLOR this_color, bool create_if_not_exists, MATERIAL::type mat_type);
			/**
			* \brief Adds a AWDBlock to the AWDproject. The Block will be added to the AWDProject, but not to any AWDFile.
			*	@param[in] awd_block The AWDBlock to add
			*	@resultAWD::result
			*/
			result add_library_block(BASE::AWDBlock* awd_block);

			/**
			* \brief Adds a AWDBlock to the AWDproject. If a AWDfile is set as active-file, the block will be added into this AWDFile.
			*	@param[in] awd_block The AWDBlock to add
			*	@param[out] blockInstance [optional] The pointer to the BlockInstance that will be created for the AWDBlock.
			*	@resultAWD::result
			*/
			result add_block(BASE::AWDBlock* awd_block, BLOCK::BlockInstance** blockInstance = NULL);	

			/**
			* \brief Remove a AWDBlock. If a file is set as active-file, the Block will only be removed from this AWDFile. Otherwise the AWDBlock will savly be removed from all AWDProject
			*	@param[in] awd_block The AWDBlock to remove
			*	@resultAWD::result
			*/
			result remove_block(BASE::AWDBlock* awd_block);	

			/**
			* \brief Adds a AWDBlock (BLOCKS) to a AWDFile (and the AWDProject). All dependencies will be included in the AWDFile.
			*	@param[in] awdBlock Pointer to the AWDBlock that should be added to the AWDFile
			*	@param[in] path The path of the AWDFile
			*	@param[out] blockInstance [optional] The pointer to the BlockInstance that will be created for the AWDBlock.
			* @return AWD::result
			*/
			result add_block_to_file(BASE::AWDBlock* awdBlock, std::string& path, BLOCK::BlockInstance* blockInstance = NULL);		

			/**
			* \brief Adds a AWDBlock to the AWDproject. If no AWDfile is set as active-file, the function will fail. If a AWDfile is set as active-file, the block will be added into this AWDFile. Dependencies of the block will be handled as external assets, provided that they are not already added to the AWDFile.
			*	@param[in] awdBlock Pointer to the AWDBlock that should be added to the AWDFile
			*	@param[out] blockInstance [optional] The pointer to the BlockInstance that will be created for the AWDBlock
			* @return AWD::result
			*/
			result add_block_with_shared_dependencies(BASE::AWDBlock* awdBlock, BLOCK::BlockInstance* blockInstance = NULL);
			

			/**
			* \brief Adds a AWDBlock to the AWDproject. If no AWDfile is set as active-file, the function will fail. The AWDBlock must already be part of the AWDproject, otherwise the function will fail. If a AWDfile is set as active-file, the block will be added into this AWDFile. The block and all its dependencies will be handled as external assets, provided that they are not already added to the AWDFile.
			* \brief Adds a AWDBlock (BLOCKS) as a shared asset to a AWDFile (and the AWDProject). Dependencies will be handled as external assets, provided that they are not already added to the AWDFile.
			* the function will check if the block has a appearence in another AWDFile. If this is not the case, the Block_Instance will not be added.
			*	@param[in] awdBlock Pointer to the AWDBlock that should be added to the AWDFile
			*	@param[out] blockInstance [optional] The pointer to the BlockInstance that will be created for the AWDBlock
			* @return The BlockInstance that has been created in the AWDFile, or NULL if opperation failed.
			*/
			result add_block_as_shared_asset(BASE::AWDBlock* awdBlock, BLOCK::BlockInstance* blockInstance = NULL);
	
						
			/**
			* \brief Get the AWDNameSpace for a specific URI. If no AWDNameSpace exists for the URI, it will be created.
			*	@param[in] namespace_uri The URI for the AWDNameSpace to get.
			*	@return Pointer to the Namespace
			*/
			result get_namespace(BLOCKS::Namespace *);
        
			/**
			* \brief Get the default Material
			*/
			BASE::AWDBlock* get_default_material();

			/**
			* \brief Set the default Material
			*/
			result set_default_material(BASE::AWDBlock* material);

			/**
			* \brief Get the settings that are used for this AWDproject. AWDFiles will clone this settings when they are created.
			*/
			//SETTINGS::Settings* settings();

			/**
			* \brief Set the default subgeomsettings
			*/
			result set_default_subgeomsettings(SETTINGS::BlockSettings* settings);
	};
}
#endif

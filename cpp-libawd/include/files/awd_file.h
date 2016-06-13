#ifndef _LIBAWD_AWDFILE_H
#define _LIBAWD_AWDFILE_H

#include <string>
#include <vector>
#include "utils/awd_types.h"
#include "base/block.h"
#include "base/state_element_base.h"
#include "blocks/meta.h"
#include "blocks/namespace.h"
#include "elements/block_instance.h"

namespace AWD
{
	// predeclare AWDProject, so AWDFile can use it
	class AWDProject;
	namespace FILES
	{
		/**
		* \class AWDFile
		* \brief A AWDFile is used to export or import one AWD file.
		*
		* The AWDFile contains a list of BlockInstances.
		*
		*/
		class AWDFile:
			BASE::StateElementBase
		{
			private:
				AWDProject* awd_project;			/// < Pointer to AWDProject. Allows to update the AWD_project from within a AWDFile
				SETTINGS::Settings* settings;		/// < Pointer to the settings to use for this AWDFile.
				bool own_settings;
				std::string url;					/// < The url of this AWDFile.	
				BLOCKS::MetaData*	meta_data_block;					/// < The Meta-data-blockInstance for this AWDFile. Should never be added to the blocklist of AWDProject.	
				BLOCK::BlockInstance*	meta_data_block_inst;					/// < The Meta-data-blockInstance for this AWDFile. Should never be added to the blocklist of AWDProject.	
				BLOCKS::Namespace*	name_space_block;					/// < The NameSpace-blockInstance for this AWDFile. Should never be added to the blocklist of AWDProject.						
				BLOCK::BlockInstance*	namespace_block_inst;					/// < The NameSpace-blockInstance for this AWDFile. Should never be added to the blocklist of AWDProject.						
				std::vector<BLOCK::BlockInstance*> block_instances;			///< contains all BlockInstances that have been assigned to the AWDFile.
				
			protected:
				TYPES::state validate_state();
				
			public:
				/**
				*\brief Create a new AWDFile. Should be only called from AWDProject::create_file()
				* @param[in] awd_project The pointer to the AWDProject
				* @param[in] url The url of the AWDFile
				*
				*/
				AWDFile(AWDProject* awd_project, const std::string& url, bool own_settings=false);
				~AWDFile(); 
					
				result get_statistics_for_type(BLOCK::block_type block_type, std::vector<std::string>& output_list);
				/**
				* \brief Get the statistic of the AWDProject.
				*	@param[out] statistics_str The string that should get the statistics copied into it.
				*	@resultAWD::result
				*/
				result get_statistics(std::vector<std::string>& statistics_str);			
				/**
				* \brief Process the AWDFile if needed. 
				*
				* Process will take care that the list of block_instances is in correct order for export, and that each block_instance has the correct block_addr set. 
				* @return result 
				*/
				result process();
				
				/**
				* \brief get the number of blocks in the AWDfile
				*/
				TYPES::UINT32  get_block_cnt();

				/**
				* \brief get the settings used for this file
				*/
				SETTINGS::Settings* get_settings();
				/**
				* \brief Find the BlockInstance for a specific AWDBlock
				* @param[in]  awd_block Pointer to the BASE::AWDBlock* to find.
				* @param[out] block_instance Pointer to the BASE::BlockInstance* that will be filled.
				* @return result 
				*/
				result find_block_instance(BASE::AWDBlock* awd_block, BLOCK::BlockInstance** block_instance);

				/**
				* \brief Add a Block into the AWDFile
				* @param[in]  awd_block Pointer to the BASE::AWDBlock that should be added.
				* @param[in] instance_type [optional] The Tye of instance to add.
				* @param[out] block_inst [optional] The Tye of instance to add.
				* @return result 
				*/
				result add_block(BASE::AWDBlock* awd_block, BLOCK::instance_type instance_type, BLOCK::BlockInstance** block_inst);
				
				/**
				* \brief Add a Block into the AWDFile
				* @param[in]  awd_block Pointer to the BASE::AWDBlock that should be added.
				* @param[in] instance_type [optional] The Tye of instance to add.
				* @return result 
				*/
				result add_dependency_block(BASE::AWDBlock* awd_block, BLOCK::instance_type instance_type);

				/**
				* \brief Add a BlockInstance into the AWDFile
				* @param[in]  awd_block Pointer to the BASE::BlockInstance that should be added.
				* @return result 
				*/
				result add_block_instance(BLOCK::BlockInstance* block_instance);
				
				/**
				* \brief Open the file with system default application
				* @return result 
				*/
				result open();

				/**
				* \brief Export the AWDFile to disc. 
				* This function will manage the writing of the individual blocks, and writes the file-header according to the AWDSpecs \ref awd_pt2_file_header
				* @return result 
				*/
				result write_to_disc();
				
				/**
				* \brief Clear the AWDFile. Will remove all BlockInstances from AWDLibrary that have been set for this AWDFile. The AWDBLock will not be remove from AWDLibrary.
				* @return result 
				*/
				result clear();
				/**
				* \brief Read the AWDFile from disc. 
				* @return result 
				*/
				result read_from_disc(bool allow_overwrite);

				
				/**
				*\brief Gets a pointer to the FILE that is set for this AWDFile.
				* @return url The Pointer to the FILE
				*/
				std::string& get_url();

				/**
				*\brief Make sure the namespace with given id exists in AWDFile .
				* @return url The Pointer to the FILE
				*/
				result add_namespace_by_id(TYPES::UINT8 name_space_id);

				/**
				*\brief Gets the ID of a name_spcae inside a AWDFile.
				* @return the id of the name-space to use for this 
				*/
				TYPES::UINT8 get_default_namespace_id();

		};
	}
}
#endif

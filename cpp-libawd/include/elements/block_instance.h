#ifndef _LIBAWD_BLOCK_INSTANCE_H
#define _LIBAWD_BLOCK_INSTANCE_H

#include <string>
#include <vector>

#include "base/state_element_base.h"
#include "utils/awd_types.h"
#include "utils/settings.h"
#include "files/file_writer.h"
#include "files/awd_file.h"
namespace AWD
{
	namespace BLOCK
	{
	
		/**\brief BlockInstance is a class used to descripe the occurence of a AWDBlock inside a AWDFile.
		*
		* 
		*/
		class BlockInstance:
			public BASE::StateElementBase
		{
			private:
				TYPES::BADDR addr;							///< The Address of the AWDBlock inside the AWDFile. Should be available after the block was added to the file.					
				SETTINGS::BlockSettings* settings;			///< BlockSettings used when exporting the AWDBlock into the AWDFile.
				bool settingsOwner;							///< If this is true, than the BlockSettings is owned by the block, and must be deleted in Deconstructor
				FILES::AWDFile* awdFile;					///< The AWDFile that holds this BlockInstance. Must be set.
				BASE::AWDBlock* awdBlock;					///< The AWDBlock that is referneced by this BlockInstance. Must be set.
				instance_type instanceType;					///< Defines how the data for this block should be stored into the AWDFile.

			
			protected:
				TYPES::state validate_state();
				
			public:
				/**
				*	\brief Creates a new BlockInstance. For imported AWDBlocks, the BlockInstances will be created by the AWDFile.
				*	@param[in] awdBlock Pointer to the AWDBlock that is referenced by this BlockInstance.
				*	@param[in] awdFile Pointer to the AWDFile that contains this BlockInstance.
				*	@param[in] blocksettings Pointer to the BlockSettings that should be used when exporting / importing this BlockInstance.
				*	@param[in] instanceType The instance_type for this BlockInstance
				* 
				*/
				BlockInstance(BASE::AWDBlock* awdBlock, FILES::AWDFile* awdFile, SETTINGS::BlockSettings* blocksettings, instance_type instanceType,bool settingsOwner = false);
				~BlockInstance();
		
				/**
				*	\brief Get the BlockAddr for this BlockInstance
				*	@return The adress of the block_instance inside the AWDFile
				*/
				TYPES::BADDR get_addr();

				/**
				*	\brief Set the BlockAddr for this BlockInstance
				*	@param[in] addr Pointer to the AWDFile
				*/
				void set_addr(TYPES::BADDR addr);

				/**
				*	\brief Get the BlockSettings that should be used for writing / reading this BlockInstance
				*	@return Pointer to the BlockSettings
				*/
				SETTINGS::BlockSettings* get_settings();
				
				/**
				*	\brief Set the BlockSettings that should be used for writing / reading this BlockInstance
				*	@param[in] settings Pointer to the BlockSettings
				*/
				void set_settings(SETTINGS::BlockSettings* settings);

				/**
				*	\brief Get the AWDFile that this BlockInstance is assigned to
				*	@return Pointer to the AWDFile
				*/
				FILES::AWDFile* get_awdFile();
				
				/**
				*	\brief Set the AWDFile that this BlockInstance is assigned to
				*	@param[in] awd_file Pointer to the AWDFile
				*/
				void set_awdFile(FILES::AWDFile* awd_file);
				
				/**
				*	\brief Get the AWDBlock that this Block_instance should reference
				*	@return Pointer to the AWDBlock
				*/
				BASE::AWDBlock* get_awdBlock();

				/**
				*	\brief Set the AWDBlock that this Block_instance should reference
				*	@param[in] awd_block Pointer to the AWDBlock
				*/
				void set_awdBlock(BASE::AWDBlock* awd_block);
				
				/**
				*	\brief Get the instance_type of the blockInstance. Defines if the block should be embbed in the file, or be stored as external ressource
				*	@return The instance_type
				*/
				instance_type get_instanceType();

				/**
				*	\brief Set the instance_type of the blockInstance. Defines if the block should be embbed in the file, or be stored as external ressource
				*	@param[in] block_instance_type The instance_type to use.
				*/
				void set_instanceType(instance_type block_instance_type);
				
				/**
				*	\brief Writes the BlockInstance to file. This will write the block-header and than call the write_body of the AWDBlock
				*	@param[in] fileWriter FileWriter use for writing.
				*	@return The number of bytes that have been written to file
				*/
				TYPES::UINT32 write_block(FILES::FileWriter * fileWriter);

				/**
				*	\brief Puts the BlockInstance in the correct order into the AWD-File, collecting dependencies first.
				* This is called for all the block_instances that are already assigned to the AWDFile. The dependencies might not be added to the AWDFile already.
				*/
				result add_with_dependencies();
		};
	
	}
}
#endif

#ifndef _LIBAWD_BLOCK_H
#define _LIBAWD_BLOCK_H

#include <string>
#include <vector>

#include "utils/awd_types.h"
#include "base/state_element_base.h"
#include "utils/settings.h"
#include "files/file_writer.h"
#include "files/file_reader.h"

namespace AWD{	
	
	namespace FILES	{	
		class AWDFile;
	}
	// pre-declare, so AWDBlock can use them:
	namespace BLOCK	{	
		class BlockInstance;
		struct blockInstance_file_connection
		{
			BLOCK::BlockInstance* block_instance;
			FILES::AWDFile* file;
		};
	}
	namespace BASE	{

		/**\brief All classes that are considered a AWDBlock must derive from AWDBlock. \n 
		* AWDBlock is a pure virtual class, and thus should never be instantiated directly.\n
		* All classes that derive from AWDBlock are assigned to the AWD::BLOCKS namespace.
		* 
		*/
		class AWDBlock:
			public StateElementBase
		{
			private:
				std::vector<BLOCK::category> catergories;				/// < List of BLOCK::category that this AWDBlock is assigned to
				std::vector<BLOCK::blockInstance_file_connection> block_instances;		/// < List of BlockInstance for this AWDBlock
				TYPES::INT8 ns_handle;
				std::string external_id;
				void* external_object;
				std::string name;
				std::string encountered_at;
				std::string script_name;
				
				
			protected:
				
				std::vector<std::string> ressource_ids;

				TYPES::state validate_state();
				/**\brief The block_type of this AWDBlock. Deriving classes must set this in the constructor.
				*/
				BLOCK::block_type type;
				/**\brief Make sure all dependencies of a AWDBLock are put into the AWDFile before this AWDBlock.
				*/
				virtual TYPES::state validate_block_state()=0;
				/**\brief Make sure all dependencies of a AWDBLock are put into the AWDFile before this AWDBlock.
				*/
				virtual result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)=0;
				/**\brief Calculate the length of the body of this AWDBlock.
				*/
				virtual TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *)=0;
				/**\brief Write the body of this AWDBlock into a AWDFile.
				*/
				virtual result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*)=0;

				//virtual result write_body_log(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*)=0;

				/**\brief Read the body of this AWDBlock from a AWDFile.
				*/
				result read_body(FILES::FileReader*, SETTINGS::BlockSettings *, FILES::AWDFile*);

			public:
				/**\brief AWDBlock is a base-class and should never be created directly.
				*/
				AWDBlock(BLOCK::block_type);
				AWDBlock(BLOCK::block_type, const std::string&);
				virtual ~AWDBlock();
			
				void set_ressource_ids(std::vector<std::string>& ids);
				std::vector<std::string> get_ressource_ids();
				int inc_id;
				TYPES::UINT32 byte_cnt;
				std::vector<std::string> scene_names;
				void add_scene_name(const std::string&);
				void get_scene_names( std::string&);

				void add_res_id(const std::string&);
				bool has_res_id(const std::string&);
				void clear_res_ids();

				std::string& get_encountered_at();
				void set_encountered_at(const std::string& encountered_at);
				std::string& get_script_name();
				void set_script_name(const std::string& script_name);

				/**
				*\brief Get the name.
				*/
				std::string& get_name();

				/**
				*\brief Set the name.
				*/
				void set_name(const std::string& name);

				/**\brief Get the AWD::BLOCK::block_type of this AWDBlock
				* @return The #AWD::BLOCK::block_type for this AWDBlock
				*/
				BLOCK::block_type get_type();

				/**\brief Set the AWD::BLOCK::block_type of this AWDBlock
				* @param[in] block_type The #AWD::BLOCK::block_type for this AWDBlock
				*/
				void set_type(BLOCK::block_type block_type);
			
				/**\brief Get the external id. can be used to cache AWDBlock using external id as key.
				* @return The #AWD::BLOCK::block_type for this AWDBlock
				*/
				std::string& get_external_id();
				
				/**\brief Set the external id. can be used to cache AWDBlock using external id as key.
				* @param[in] block_type The #AWD::BLOCK::block_type for this AWDBlock
				*/
				void set_external_id(const std::string& external_id);
				
				/**\brief Get the pointer to a external object. can be used to cache AWDBlock using external Objects as key.
				* @return The #AWD::BLOCK::block_type for this AWDBlock
				*/
				void* get_external_object();

				/**\brief Set a pointer to a external object. can be used to cache AWDBlock using external Objects as key.
				* @param[in] block_type The #AWD::BLOCK::block_type for this AWDBlock
				*/
				void set_external_object(void* external_object);

				/**\brief Get all AWD::BLOCK::category that are assigned to this AWDBlock
				* @return List of #AWD::BLOCK::category for this AWDBlock
				*/
				std::vector<BLOCK::category>& get_categories();

				/**\brief Check if the AWDBlock is assigned to a category
				* @param[in] category the AWD::BLOCK::category to test against
				* @return True if the AWD::BLOCK::category exists on the AWDBlock
				*/
				bool is_category(BLOCK::category category);

				/**\brief Assign a category to the AWDBlock
				* @param[in] category the category to add
				*/
				void add_category(BLOCK::category category);
			
				/**\brief Prepare the AWDBlock for export into the specified AWDFile
				* @param[in] file the Pointer to the AWDFile
				* @param[in] file The BLOCK::instance_type to use for the dependencies of this AWDBlock
				*/
				result add_with_dependencies(FILES::AWDFile * file, BLOCK::instance_type instance_type);

				/**\brief Write the AWDBlock into the specified AWDFile. This will call the write_body function fo the derived classes.
				* @param[in] fileWriter the Pointer to the fileWriter of the AWDFile
				* @param[in] settings the Pointer to the BlockSettings to use for writing
				* @param[in] file the Pointer to the AWDFile
				*/
				size_t write_block(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

				/**\brief Read the AWDBlock from the specified AWDFile. This will call the read_body function fo the derived classes.
				* @param[in] fileWriter the Pointer to the fileWriter of the AWDFile
				* @param[in] settings the Pointer to the BlockSettings to use for writing
				* @param[in] file the Pointer to the AWDFile
				*/
				result read_block(FILES::FileReader* fileReader, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
						
				/**\brief Get the BlockInstance that associates this AWDBlock with the specified AWDFile.
				* @param[in] file the Pointer to the AWDFile
				* @return The pointer to the BlockInstance, or NULL if this AWDBlock is not assigned to the AWDFile.
				*
				* Internally, the BlockBase has only a predeclared AWDFile, so it cannot access any 
				*/
				result find_instance_for_file(FILES::AWDFile* file, BLOCK::BlockInstance** out_block_instance);
				
				/**\brief Get the Addr of this Block for the specific file.
				*/
				result get_block_addr(FILES::AWDFile* file, TYPES::UINT32&);
			
				/**\brief Adds a BlockInstance for this AWDBlock and a specific AWDFile. 
				* @param[in] file the Pointer to the AWDFile
				* @return A pointer to the BlockInstance that has been created to represent the occurence of the AWDBlock inside the AWDFile.
				*
				* A BlockInstance will be created for this AWDBlock. This BlockInstance will be added to the block_instances of the AWDBlock, and to the block_instances of the AWDFile.
				* When using AWD::BLOCK::instance_type other than  AWD::BLOCK::instance_type::BLOCK_EMBBED, keep in mind, that the AWDBlock must contain at least one BlockInstance with  AWD::BLOCK::instance_type::BLOCK_EMBBED, so it will be exported.
				*
				*/
				result add_block_instance_for_file(FILES::AWDFile* file, BLOCK::BlockInstance* block_instance );

				/**\brief Get all BlockInstance that exists for this AWDBlock 
				* @return 
				*/
				std::vector<BLOCK::BlockInstance*>& get_block_instances();

				/**
				*\brief Get the name-space handle.
				*/
				TYPES::INT8 get_name_space_handle();
				
				/**
				*\brief Set the name-space handle to use.
				*/
				void set_name_space_handle(TYPES::INT8 name_space);
		};	
	}
}

#endif

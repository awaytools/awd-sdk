#ifndef _LIBAWD_NS_H
#define _LIBAWD_NS_H

#include <string>
#include <vector>
#include "utils/awd_types.h"
#include "base/block.h"

#include "files/file_writer.h"
#include "utils/settings.h"

namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class Namespace
		*	\brief Descripes a AWDBlock that stores a list of Namespaces for a AWDFile as defined by AWDSpecs: \ref awd_pt2_15_2
		*
		*/
		class Namespace :
			public BASE::AWDBlock
		{
			private:
				std::vector<TYPES::NameSpace_struct> name_spaces;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				Namespace();
				~Namespace();
				/**
				*\brief Get the id for a name_space inside one AWDFile. If the name_space was not found, it will be added.
				*/
				result add_namespace(const std::string& name_space, TYPES::UINT8  name_space_handle);

				/**
				*\brief Get the id for a name_space inside one AWDFile. If the name_space was not found, it will be added.
				*/
				result find_namespace_by_handle(TYPES::UINT8  name_space_handle);

		};
	}
}
#endif

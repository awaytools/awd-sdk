#ifndef _LIBAWD_META_H
#define _LIBAWD_META_H

#include <string>
#include "utils/awd_types.h"
#include "base/block.h"
#include "base/attr.h"

#include "files/file_writer.h"
#include "utils/settings.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
	
		/** \class MetaData
		*	\brief Descripes a AWDBlock that stores the Meta-data for a AWDFile as defined by AWDSpecs: \ref awd_pt2_15_3
	
		*/
		class MetaData :
			public BASE::AWDBlock,
			public BASE::AttrElementBase
		{
			private:
				std::string encoder_name;
				std::string encoder_version;
				std::string generator_name;
				std::string generator_version;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				MetaData(const std::string& generator_name, const std::string& generator_version);
				~MetaData();

				void override_encoder_metadata(std::string&, std::string&);
				void set_generator_metadata(std::string&, std::string&);
		};
	}
}
#endif

#ifndef _LIBAWD_COMMAND_H
#define _LIBAWD_COMMAND_H

#include "utils/awd_types.h"

#include "base/block.h"
#include "base/attr.h"
#include "base/scene_base.h"
#include "utils/util.h"

#include "files/file_writer.h"
#include "base/state_element_base.h"
#include "utils/settings.h"


namespace AWD
{
	namespace BLOCKS
	{	
		/** \class CommandBlock
		*	\brief Descripes a AWDBlock that contains commands that can be executet on import. Defined by AWDSpecs: \ref awd_pt2_15_1
		*
		*/
		class CommandBlock :
			public BASE::SceneBlockBase
		{
			private:
				TYPES::UINT8 hasSceneInfos;
				ATTR::NumAttrList *commandProperties;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*,  SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				CommandBlock(std::string&);
				CommandBlock();
				~CommandBlock();
				void add_target_light(TYPES::BADDR);
		};
	}
}
#endif

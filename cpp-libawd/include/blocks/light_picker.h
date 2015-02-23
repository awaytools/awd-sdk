#ifndef _LIBAWD_LIGHT_PICKER_H
#define _LIBAWD_LIGHT_PICKER_H

#include <string>
#include <vector>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/scene_base.h"

#include "files/file_writer.h"
#include "utils/settings.h"
#include "blocks/shadows.h"
#include "blocks/light.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		/** \class LightPicker
		*	\brief Descripes a AWDBlock for [Away3D LightPicker](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_9
		*
		*/
		class LightPicker :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				std::vector<Light*> lights;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				LightPicker(std::string&);
				LightPicker();
				~LightPicker();
				void set_lights(std::vector<Light*>);
				std::vector<Light*> get_lights();
				BASE::AWDBlock * get_shadowMethod();
				bool check_shadowMethod(ShadowMethod *);
		};
	}
}
#endif

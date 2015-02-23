#ifndef _LIBAWD_TEXTURE_PROJECTOR_H
#define _LIBAWD_TEXTURE_PROJECTOR_H

#include <string>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/scene_base.h"

#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class TextureProjector
		*	\brief Descripes a AWDBlock for [Away3D TextureProjector](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_8
		*
		*/
		class TextureProjector :
			public BASE::SceneBlockBase
		{
			private:
				BASE::AWDBlock * texture;
				TYPES::F64 field_of_view;
				TYPES::F64 aspect_ratio;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				TextureProjector(std::string&,  TYPES::F64 *);
				TextureProjector();
				~TextureProjector();

				TYPES::F64 get_field_of_view();
				void set_field_of_view(TYPES::F64);
				TYPES::F64 get_aspect_ratio();
				void set_aspect_ratio(TYPES::F64);
				BASE::AWDBlock * get_texture();
				void set_texture(BASE::AWDBlock *);
		};
	}
}
#endif

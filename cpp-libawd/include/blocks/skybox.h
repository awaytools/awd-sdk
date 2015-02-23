#ifndef _LIBAWD_SKYBOX_H
#define _LIBAWD_SKYBOX_H

#include "utils/awd_types.h"

#include "base/block.h"
#include "base/attr.h"
#include "base/scene_base.h"
#include "utils/util.h"

#include "files/file_writer.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
	
		/** \class SkyBox
		*	\brief Descripes a AWDBlock for [Away3D SkyBox](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_5
		*
		*/
		class SkyBox :
			public BASE::SceneBlockBase
		{
			private:
				BASE::AWDBlock * cubeTex;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				SkyBox(std::string&);
				SkyBox();
				~SkyBox();

				void set_cube_tex(BASE::AWDBlock *);
				BASE::AWDBlock* get_cube_tex();
		};
	
	}
}
#endif

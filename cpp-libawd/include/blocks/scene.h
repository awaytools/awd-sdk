#ifndef _LIBAWD_SCENE_H
#define _LIBAWD_SCENE_H

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
	
		/** \class Scene
		*	\brief Descripes a AWDBlock for [Away3D Scene](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_2 
		*
		*/
		class Scene :
			public BASE::SceneBlockBase
		{
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				Scene(std::string&);
				Scene();
				~Scene();
		};

	}
}
#endif

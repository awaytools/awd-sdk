#ifndef _LIBAWD_OBJECTCONTAINER_H
#define _LIBAWD_OBJECTCONTAINER_H

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
		/** \class ObjectContainer3D
		*	\brief Descripes a AWDBlock for [Away3D ObjectContainer3D](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_3 
		*
		*/
		class ObjectContainer3D :
			public BASE::SceneBlockBase
		{
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				ObjectContainer3D(const std::string&);
				ObjectContainer3D(const std::string&, TYPES::F64 *);
				ObjectContainer3D();
				~ObjectContainer3D();
		};
	
	}
}
#endif

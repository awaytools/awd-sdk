#ifndef _LIBAWD_BILLBOARD_H
#define _LIBAWD_BILLBOARD_H
#include <map>
#include <string>
#include "utils/awd_types.h"
#include "base/scene_base.h"

#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{	
	
		/** \class Billboard
		*	\brief Descripes a AWDBlock for [Away3D Billboard](http://www.away3d.com)  as defined by AWDSpecs: \ref awd_pt2_12_4
		*
		*/
		class Billboard:
			public AWDBlock,
			public AttrElementBase
		{
			private:
				BASE::AWDBlock * material;
				void init();

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				Billboard(std::string&);
				Billboard(std::string&, TYPES::F64 *);
				Billboard();
				~Billboard();
				
			
				
				BASE::AWDBlock * get_material();
				void set_material(BASE::AWDBlock * default_mat);

		};
	}
}
#endif

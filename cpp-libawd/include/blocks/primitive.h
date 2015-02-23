#ifndef _LIBAWD_PRIMITIVE_H
#define _LIBAWD_PRIMITIVE_H

#include "blocks/scene.h"
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class Primitive
		*	\brief Descripes a AWDBlock for [Away3D Primitive](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_11_2   
		*
		*/
		class Primitive :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				GEOM::primitive_type prim_type;
				double yoffset;
			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				Primitive(std::string&, GEOM::primitive_type);
				Primitive();
				~Primitive();

				double get_Yoffset();
				void set_Yoffset(double);
				void add_int_property(int, int, int);
				void add_number_property(int, float, float);
				void add_bool_property(int, bool, bool);
		};
	}
}
#endif

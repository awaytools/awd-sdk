#ifndef _LIBAWD_SHADING_H
#define _LIBAWD_SHADING_H

#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"

using namespace AWD::BASE;

namespace AWD
{
	namespace MATERIAL
	{
	
		/**\brief ShadingMethod describes a ShadingMethod, that can be assigned to a material.
		* In awd and away3d, shading-methods can not be shared accross materials.
		*/
		class ShadingMethod :
			public BASE::AttrElementBase
		{
			protected:
				MATERIAL::shading_type type;
				ATTR::NumAttrList * shading_props;
				BASE::AWDBlock * awdBlock1;
				BASE::AWDBlock * awdBlock2;
				BASE::AWDBlock * awdBlock3;
				BASE::AWDBlock * awdBlock4;

			public:
				ShadingMethod(MATERIAL::shading_type);
				~ShadingMethod();
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				void set_awdBlock1(BASE::AWDBlock *);
				void set_awdBlock2(BASE::AWDBlock *);
				void set_awdBlock3(BASE::AWDBlock *);
				void set_awdBlock4(BASE::AWDBlock *);
				MATERIAL::shading_type get_shading_type();
				ATTR::NumAttrList * get_shading_props();
				TYPES::UINT32 calc_method_length(SETTINGS::BlockSettings *);
				result write_method(FILES::FileWriter*, SETTINGS::BlockSettings *);
				void add_shading_color_property(int, TYPES::UINT32, TYPES::UINT32);
				void add_shading_number_property(int, float, float);
				void add_shading_int_property(int, int, int);
				void add_shading_int8_property(int, int, int);
				void add_shading_bool_property(int, bool, bool);
		};
	}
}
#endif

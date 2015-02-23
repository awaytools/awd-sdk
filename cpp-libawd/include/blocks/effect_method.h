#ifndef _LIBAWD_EFFECT_METHOD_H
#define _LIBAWD_EFFECT_METHOD_H

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
		/** \class EffectMethod
		*	\brief Descripes a AWDBlock for [Away3D EffectMethod](http://www.away3d.com)  as defined by AWDSpec: \ref awd_pt2_13_5
		*
		*/
		class EffectMethod :
			public BASE::AWDBlock,			
			public BASE::AttrElementBase
		{
			private:
				MATERIAL::effect_method_type type;
				ATTR::NumAttrList * effect_props;
				BASE::AWDBlock * awdBlock1;
				BASE::AWDBlock * awdBlock2;
				BASE::AWDBlock * awdBlock3;
				BASE::AWDBlock * awdBlock4;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				EffectMethod(std::string&, MATERIAL::effect_method_type);
				EffectMethod();
				~EffectMethod();
				void set_awdBlock1(BASE::AWDBlock *);
				void set_awdBlock2(BASE::AWDBlock *);
				void set_awdBlock3(BASE::AWDBlock *);
				void set_awdBlock4(BASE::AWDBlock *);
				ATTR::NumAttrList * get_effect_props();
				void add_color_method_prop(int, TYPES::UINT32, TYPES::UINT32);
				void add_number_method_prop(int, float, float);
				void add_int_method_prop(int, int, int);
				void add_bool_method_prop(int, bool, bool);
		};
	}
}
#endif

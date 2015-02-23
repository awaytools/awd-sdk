#ifndef _LIBAWD_SHADOWS_H
#define _LIBAWD_SHADOWS_H

#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"

#include "files/file_writer.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class ShadowMethod
		*	\brief Descripes a AWDBlock for [Away3D ShadowMethod](http://www.away3d.com) 
		*
		*/
		class ShadowMethod :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				LIGHTS::shadow_method_type shadow_type;
				ATTR::NumAttrList * shadow_props;
				BASE::AWDBlock * awdLight;
				BASE::AWDBlock * base_method;
				bool autoApply;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				ShadowMethod(std::string&, LIGHTS::shadow_method_type);
				ShadowMethod();
				~ShadowMethod();
				void set_awdLight(BASE::AWDBlock *);;
				BASE::AWDBlock * get_awdLight();
				ATTR::NumAttrList * get_shadow_props();
				void set_base_method(BASE::AWDBlock *);
				BASE::AWDBlock * get_base_method();
				void set_autoApply(bool);
				bool get_autoApply();
				void set_shadow_type(LIGHTS::shadow_method_type);
				LIGHTS::shadow_method_type get_shadow_type();
				void add_color_shadow_prop(int, TYPES::UINT32, TYPES::UINT32);
				void add_number_shadow_prop(int, float, float);
				void add_int_shadow_prop(int, int, int);
				void add_int16_shadow_prop(int, int, int);
				void add_bool_shadow_prop(int, bool, bool);
		};
	}
}
#endif

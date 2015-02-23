#ifndef _LIBAWD_LIGHT_H
#define _LIBAWD_LIGHT_H

#include <string>
#include <vector>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/scene_base.h"

#include "files/file_writer.h"
#include "blocks/shadows.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		/** \class Light
		*	\brief Descripes a AWDBlock for [Away3D Light](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_6
		*
		* 
		*/
		class Light:
			public BASE::SceneBlockBase
		{
			private:
				LIGHTS::type type;
				TYPES::F64 radius;
				TYPES::F64 falloff;
				TYPES::COLOR color;
				TYPES::F64 specular;
				TYPES::F64 diffuse;
				TYPES::COLOR ambientColor;
				TYPES::F64 ambient;
				TYPES::F64 dirX;
				TYPES::F64 dirY;
				TYPES::F64 dirZ;
				LIGHTS::shadow_mapper_type shadowmapper_type;
				int shadowmapper_depth_size;//cube default = 1, other default = 2, all="256", "512", "2048", "1024"
				TYPES::F64 shadowmapper_coverage;
				int shadowmapper_cascades;
				BASE::AWDBlock * shadowMethod;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				Light(std::string&);
				Light();
				~Light();
				void set_light_type(LIGHTS::type);
				LIGHTS::type get_light_type();
				void set_shadowmapper_type(LIGHTS::shadow_mapper_type);
				LIGHTS::shadow_mapper_type get_shadowmapper_type();
				void set_shadowmapper_depth_size(int);
				int get_shadowmapper_depth_size();
				void set_shadowmapper_coverage(TYPES::F64);
				TYPES::F64 get_shadowmapper_coverage();
				void set_shadowmapper_cascades(int);
				int get_shadowmapper_cascades();
				void set_radius(TYPES::F64);
				TYPES::F64 get_radius();
				void set_falloff(TYPES::F64);
				TYPES::F64 get_falloff();
				void set_color(TYPES::COLOR);
				TYPES::COLOR get_color();
				void set_specular(TYPES::F64);
				TYPES::F64 get_specular();
				void set_diffuse(TYPES::F64);
				TYPES::F64 get_diffuse();
				void set_ambientColor(TYPES::COLOR);
				TYPES::COLOR get_ambientColor();
				void set_ambient(TYPES::F64);
				TYPES::F64 get_ambient();
				void set_directionVec(TYPES::F64, TYPES::F64,TYPES::F64);
				void set_shadowMethod(BASE::AWDBlock *);
				BASE::AWDBlock * get_shadowMethod();
		};
	}
}
#endif

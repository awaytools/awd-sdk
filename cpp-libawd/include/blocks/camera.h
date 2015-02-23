#ifndef _LIBAWD_CAMERA_H
#define _LIBAWD_CAMERA_H

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

		/** \class Camera
		*	\brief Descripes a AWDBlock for [Away3D Camera](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_12_7
		*
		* A Camera has a cam_type.
		*/
		class Camera :
			public BASE::SceneBlockBase
		{
			private:
				CAMERA::cam_type cam_type;
				CAMERA::lens_type lens_type;
				ATTR::NumAttrList * lens_properties;
				TYPES::F64 lens_fov;
				TYPES::F64 lens_near;
				TYPES::F64 lens_far;
				TYPES::F64 lens_proj_height;
				TYPES::F64 lens_offX_pos;
				TYPES::F64 lens_offX_neg;
				TYPES::F64 lens_offY_pos;
				TYPES::F64 lens_offY_neg;

			protected:
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::state validate_block_state();				
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				Camera(std::string&, CAMERA::lens_type, TYPES::F64 *);
				Camera();
				~Camera();

				TYPES::F64 get_lens_fov();
				void set_lens_fov(TYPES::F64);
				CAMERA::cam_type get_cam_type();
				void set_cam_type(CAMERA::cam_type);
				CAMERA::lens_type get_lens_type();
				void set_lens_type(CAMERA::lens_type);
				TYPES::F64 get_lens_near();
				void set_lens_near(TYPES::F64);
				TYPES::F64 get_lens_far();
				void set_lens_far(TYPES::F64);
				TYPES::F64 get_lens_proj_height();
				void set_lens_proj_height(TYPES::F64);
				void set_lens_offset(TYPES::F64, TYPES::F64, TYPES::F64, TYPES::F64);
		};	
	}
}
#endif

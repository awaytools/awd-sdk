#ifndef _LIBAWD_UVPOSE_H
#define _LIBAWD_UVPOSE_H

#include <vector>
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_clip.h"
#include "base/anim_frame.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
	
		/** \class UVPose
		*	\brief Descripes a AWDBlock for [Away3D UVPose](http://www.away3d.com) 
		*
		*/
		class UVPose : 
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase,
			public BASE::AnimationFrameBase
		{
			private:
				std::vector<TYPES::F64 *> uv_transforms;
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				UVPose(std::string&, TYPES::UINT32, TYPES::UINT32);
				UVPose();
				~UVPose();

				TYPES::F64 * get_transform_by_idx(int);
				void set_next_transform(TYPES::F64 *);
		};
	
	}
}
#endif
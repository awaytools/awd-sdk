#ifndef _LIBAWD_UVANIMCLIP_H
#define _LIBAWD_UVANIMCLIP_H

#include <vector>
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_clip.h"
#include "base/anim_frame.h"
#include "blocks/uv_pose.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		/** \class UVAnimationClip
		*	\brief Descripes a AWDBlock for [Away3D UVAnimationClip](http://www.away3d.com) 
		*
		*/
		class UVAnimationClip :
			
			public BASE::AttrElementBase,
			public BASE::AWDBlock,
			public BASE::AnimationClipBase
		{
			private:
				BASE::AWDBlock * targetGeo;
				std::vector<UVPose*> uv_poses;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result create_pose_blocks();

			public:
				UVAnimationClip(std::string&);
				UVAnimationClip();
				~UVAnimationClip();

				void append_duration_to_last_frame(TYPES::UINT16);
				TYPES::F64 *get_last_frame_tf();
				void set_next_frame_tf(TYPES::F64 *, TYPES::UINT16);
		};
	}
}
#endif

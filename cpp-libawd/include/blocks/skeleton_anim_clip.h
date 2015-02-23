#ifndef _LIBAWD_SKELANIMCLIP_H
#define _LIBAWD_SKELANIMCLIP_H
#include <vector>
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_clip.h"
#include "base/anim_frame.h"
#include "blocks/skeleton_pose.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
		/** \class SkeletonAnimationClip
		*	\brief Descripes a AWDBlock for [Away3D SkeletonAnimationClip](http://www.away3d.com) as defined by AWDSpec: \ref awd_pt2_14_3
		*
		*/
		class SkeletonAnimationClip : 
			
			public BASE::AttrElementBase, 
			public BASE::AWDBlock, 
			public BASE::AnimationClipBase
		{
			private:
				std::vector<SkeletonPose*> skeleton_poses;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result create_pose_blocks();

			public:
				SkeletonAnimationClip(std::string&);
				SkeletonAnimationClip();
				~SkeletonAnimationClip();

				void append_duration_to_last_frame(TYPES::UINT16);
				SkeletonPose * get_last_frame();
				void set_next_frame_pose(SkeletonPose *, TYPES::UINT16);
		};
	}
}
#endif

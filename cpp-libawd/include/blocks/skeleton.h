#ifndef _LIBAWD_SKELETON_H
#define _LIBAWD_SKELETON_H

#include <vector>
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "elements/skeleton_joint.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/state_element_base.h"

namespace AWD
{
	namespace BLOCKS
	{
		/** \class Skeleton
		*	\brief Descripes a AWDBlock for [Away3D Skeleton](http://www.away3d.com)  as defined by AWDSpec: \ref awd_pt2_14_1
		*
		*/
		class Skeleton :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				ANIM::SkeletonJoint *root_joint;
				std::vector<AWDBlock*> clip_blocks;
				int joints_per_vert;
				int neutralPose;
				bool simpleMode;
				bool shareAutoAnimator;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				Skeleton(std::string&, int);
				Skeleton();
				~Skeleton();

				std::vector<AWDBlock*>& get_clip_blocks();
				void set_clip_blocks(std::vector<AWDBlock*>& );
				bool get_simpleMode();
				void set_simpleMode(bool);
				bool get_shareAutoAnimator();
				void set_shareAutoAnimator(bool);
				ANIM::SkeletonJoint *set_root_joint(ANIM::SkeletonJoint *);
				ANIM::SkeletonJoint *get_root_joint();
				int get_joints_per_vert();
				void set_joints_per_vert(int);
				int get_neutralPose();
				void set_neutralPose(int);
		};
	}
}
#endif

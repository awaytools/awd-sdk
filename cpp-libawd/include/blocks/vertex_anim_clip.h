#ifndef _LIBAWD_VERTEXANIMCLIP_H
#define _LIBAWD_VERTEXANIMCLIP_H

#include "blocks/scene.h"
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "base/anim_frame.h"
#include "base/anim_clip.h"
#include "elements/subgeometry.h"
#include "blocks/vertex_pose.h"
#include "base/state_element_base.h"

namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class VertexAnimationClip
		*	\brief Descripes a AWDBlock for [Away3D VertexAnimationClip](http://www.away3d.com) as defined by AWDSpec: \ref awd_pt2_14_4
		*
		*/
		class VertexAnimationClip : 
			
			public BASE::AttrElementBase, 
			public BASE::AWDBlock, 
			public BASE::AnimationClipBase
		{
			private:
				std::vector<VertexPose*> vertex_poses;
				AWDBlock * targetGeo;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result create_pose_blocks();

			public:
				VertexAnimationClip(std::string&);
				VertexAnimationClip();
				~VertexAnimationClip();

				void append_duration_to_last_frame(TYPES::UINT16);
				VertexPose * get_last_frame_geo();
				BASE::AWDBlock * get_targetGeo();
				void set_targetGeo(BASE::AWDBlock *);
				void set_next_frame_pose(VertexPose *, TYPES::UINT16);
		};
	}
}
#endif

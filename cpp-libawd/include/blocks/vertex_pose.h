#ifndef _LIBAWD_VERTEXPOSE_H
#define _LIBAWD_VERTEXPOSE_H

#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/anim_frame.h"
#include "base/anim_clip.h"
#include "utils/settings.h"

#include "files/file_writer.h"
#include "elements/subgeometry.h"

namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class VertexPose
		*	\brief Descripes a AWDBlock for [Away3D VertexPose](http://www.away3d.com) as defined by AWDSpec: \ref awd_pt2_14_4
		*
		*/
		class VertexPose:
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase,
			public BASE::AnimationFrameBase
		{
			private:
				std::vector<GEOM::SubGeomInternal*> subgeometries;
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				VertexPose(std::string&, TYPES::UINT32, TYPES::UINT32);
				VertexPose();
				~VertexPose();


				unsigned int get_num_subs();
				GEOM::SubGeomInternal *get_sub_at(unsigned int);
				void add_sub_mesh(GEOM::SubGeomInternal *);
		};


	}
}
#endif

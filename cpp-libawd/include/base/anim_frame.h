#ifndef _LIBAWD_ANIM_FRAME_H
#define _LIBAWD_ANIM_FRAME_H

#include <string>
#include <vector>

#include "utils/awd_types.h"
#include "files/awd_file.h"
#include "files/file_writer.h"
#include "files/file_reader.h"

namespace AWD
{
	namespace BASE
	{
	
		/** \class AnimationFrameBase
		*	\brief AnimationFrameBase defines one frame of animation. Should never be instantiated directly.
		*
		*/
		class AnimationFrameBase
		{
			private:
				TYPES::UINT32 duration;
				TYPES::UINT32 external_start_time;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				void write_frame(FILES::FileWriter *, SETTINGS::BlockSettings *);
				void read_frame(FILES::FileReader *, SETTINGS::BlockSettings *);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);

			public:
				AnimationFrameBase(TYPES::UINT32, TYPES::UINT32);
				~AnimationFrameBase();

				TYPES::UINT32 get_duration();
				TYPES::UINT32 get_external_start_time();
		};
	}
}
#endif

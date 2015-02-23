#ifndef _LIBAWD_ANIMCLIP_H
#define _LIBAWD_ANIMCLIP_H

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
		/**\class AnimationClipBase
		*\brief Provide a lists of AnimationFrame for a block. Should never be instantiated directly.
		*/
		class AnimationClipBase
		{
			private:
				TYPES::UINT16 num_frames;
				TYPES::UINT32 start_frame;
				TYPES::UINT32 end_frame;
				TYPES::UINT32 skip_frame;
				bool stitch_final;
				bool loop;
				bool useTransforms;

				bool is_processed;
				std::string sourceID;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileReader *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				virtual result create_pose_blocks()=0;

			public:
				AnimationClipBase();
				~AnimationClipBase();
				
				result set_time_range(TYPES::UINT32 start_frame, TYPES::UINT32 end_frame, TYPES::UINT32 skip_frame);
				
				result set_clip_properties(bool stitch_final, bool loop, bool useTransforms);

				bool get_is_processed();
				void set_is_processed(bool);
				TYPES::UINT32 get_start_frame();
				TYPES::UINT32 get_end_frame();
				TYPES::UINT32 get_skip_frame();
				bool get_stitch_final();
				bool get_loop();
				bool get_use_transforms();
				std::string& get_sourceID();
				void set_sourceID(const std::string& name);
		};
	}
}
#endif

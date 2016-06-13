#ifndef _LIBAWD_TIMELINE_FRAME_H
#define _LIBAWD_TIMELINE_FRAME_H

#include <vector>
#include <string>
#include "utils/awd_types.h"
#include "base/attr.h"
#include "files/awd_file.h"
#include "elements/frame_commands.h"

namespace AWD
{
	namespace ANIM
	{
		class TimelineFrame
		{
			private:
				std::string frame_code;
				TYPES::UINT32 frame_duration;
				std::vector<frame_label_type> label_types;
				std::vector<std::string> labels;
				
			public:
				TimelineFrame();
				~TimelineFrame();
		
				bool isFullConstruct;
				TYPES::UINT32 startframe;
				void set_frame_duration(TYPES::UINT32);
				TYPES::UINT32 get_frame_duration();

				std::vector<ANIM::FrameCommandDisplayObject*> display_commands;				
				std::vector<ANIM::FrameCommandRemoveObject*> remove_commands;

				// used while recieving commands from adobe:
				FrameCommandDisplayObject* add_display_object_by_id(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID);
				FrameCommandDisplayObject* get_update_command_by_id(TYPES::UINT32 obj_id);
				void remove_object_by_id(TYPES::UINT32 objectID);
				//bool test_depth_ids(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID);
				
				// used while finalizing the command data:
				void apply_add_command(FrameCommandDisplayObject*, TimelineChild_instance*);
				void apply_update_command(FrameCommandDisplayObject*);

				FrameCommandDisplayObject* get_update_command_by_child(TimelineChild_instance* child);
				void apply_remove_command(FrameCommandRemoveObject*);
				void build_final_commands();
				void calc_mask_ids();
				void create_mask_layers();
				void finalize_commands();


				void set_frame_code(const std::string&);
				std::string& get_frame_code();

				void add_command(FrameCommandDisplayObject*);
				void clear_commands();
				bool is_empty();

				void add_label(frame_label_type, const std::string&);

				std::vector<std::string>& get_labels();
				
				AWD::result get_frame_info(std::vector<std::string>& infos);
				TYPES::UINT32 calc_frame_length(SETTINGS::BlockSettings *);
				void write_frame(FILES::FileWriter * , SETTINGS::BlockSettings *, FILES::AWDFile* );
		};
	}
}
#endif
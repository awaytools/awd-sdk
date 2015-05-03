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
	
		struct FrameCommandGroup
		{
			ANIM::frame_command_type command_type;
			std::vector<ANIM::FrameCommandBase*> commands;
			FrameCommandGroup(ANIM::frame_command_type command_type)
			{
				command_type=command_type;
			}
		};
		class TimelineFrame :
			public BASE::AttrElementBase
		{
		protected:
				std::vector<ANIM::FrameCommandBase*> commands;
				std::vector<ANIM::FrameCommandBase*> inActivecommands;
				std::string frameID;
				std::string frame_code;
				TYPES::UINT32 frame_duration;
				std::vector<int> dirty_layer_idx;
				std::vector<frame_label_type> label_types;
				std::vector<std::string> labels;
				std::vector<FrameCommandGroup> command_groups;

				ATTR::NumAttrList * command_properties;

			public:
				TimelineFrame();
				~TimelineFrame();
		
				void set_frame_duration(TYPES::UINT32);
				TYPES::UINT32 get_frame_duration();
				TYPES::UINT32 max_depth;
		
				std::vector<ANIM::FrameCommandBase*> final_commands;
				std::vector<ANIM::TimelineChild_instance*> display_objects;
				TimelineChild_instance* remove_display_object_by_id(TYPES::UINT32 obj_id);
				TimelineChild_instance* get_display_object_by_id(TYPES::UINT32 obj_id);
				void add_display_object_after_id(TYPES::UINT32 obj_id, ANIM::TimelineChild_instance*);
				void copy_display_objects(std::vector<ANIM::TimelineChild_instance*>& objects_to_copy);
				
				void finalize_object_states();
				void create_command_groups();
				void set_frame_code(const std::string&);
				std::string& get_frame_code();
				void add_command(FrameCommandBase*);
				void clear_commands();
				void add_inActivecommand(FrameCommandBase*);
				FrameCommandBase* get_command(TYPES::UINT32 objectID, ANIM::frame_command_type);
				void add_label(frame_label_type, const std::string&);

				void add_dirty_layer(int);
				std::vector<int> get_dirty_layers();
				std::vector<std::string>& get_labels();
				std::vector<FrameCommandBase*> get_commands();
				std::vector<FrameCommandBase*> get_inActivecommands();	
				
				AWD::result get_frame_info(std::vector<std::string>& infos);
				void calc_mask_ids();
				void calculate_command_depths();
				TYPES::UINT32 calc_frame_length(SETTINGS::BlockSettings *);
				FrameCommandBase* get_command_for_obj_id(TYPES::UINT32 obj_id);
				void write_frame(FILES::FileWriter * , SETTINGS::BlockSettings *, FILES::AWDFile* );
		};
	}
}
#endif
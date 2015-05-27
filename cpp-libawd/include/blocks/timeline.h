#ifndef _LIBAWD_TIMELINE_H
#define _LIBAWD_TIMELINE_H

#include <vector>
#include <string>
#include "utils/awd_types.h"
#include "utils/settings.h"
#include "elements/stream.h"
#include "base/block.h"
#include "files/awd_file.h"

#include "base/attr.h"

#include "elements/timeline_frame.h"

namespace AWD
{
	namespace BLOCKS
	{
		class Timeline : 
			
			public BASE::AttrElementBase, 
			public BASE::AWDBlock
		{
			private:
				TYPES::UINT32 current_frame;
				TYPES::F64 fps;
				TYPES::UINT16 scene_id;
				bool is_scene;
				TYPES::UINT16 obj_id_cnt;
				std::vector<ANIM::TimelineFrame*> frames;
				std::string symbol_name;
				std::map<TYPES::UINT32, ANIM::TimelineChild_instance*> timeline_childs_to_obj_id;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs_one_instance;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs_multiple_instances;
				ANIM::TimelineDepthManager* depth_manager;
				std::vector<ANIM::PotentialTimelineChild*> timeline_sounds;
				
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();
				result write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);

			public:
				Timeline(const std::string& name);
				Timeline();
				~Timeline();
				
				bool is_finalized;
				TYPES::UINT32 instance_cnt;
				TYPES::UINT32 timeline_id;
				std::vector<ANIM::TimelineFrame*> adobe_frames;
				std::vector<ANIM::TimelineChild_instance*> graphic_clips;
				bool is_grafic_instance;
				bool has_grafic_instances;
				
// used while finalizing the commands:
				void add_frame(ANIM::TimelineFrame*);
				void calc_mask_ids();
				ANIM::PotentialTimelineChild* get_child_for_block(BASE::AWDBlock*);
				void advance_frame(ANIM::TimelineFrame* );
				result finalize();
				ANIM::TimelineFrame* get_frame_at(TYPES::UINT32 frame);
				bool has_frame_at(TYPES::UINT32 frame);
				
// used while recieving adobe frame commands:
				void add_adobe_frame(ANIM::TimelineFrame*);
				ANIM::FrameCommandDisplayObject* add_display_object_by_id(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID);
				void remove_object_by_id(TYPES::UINT32 objectID);
				bool test_depth_ids(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID);
				ANIM::FrameCommandBase* get_update_command_by_id(TYPES::UINT32 obj_id);
				
				std::string& get_symbol_name();
				void set_symbol_name(const std::string& symbol_name);
				TYPES::F64 get_fps();
				void set_fps(TYPES::F64);
				void set_scene_id(int);
				int get_scene_id();
				bool get_is_scene();
				result get_frames_info(std::vector<std::string>& infos);


				ANIM::TimelineFrame* get_frame();
				std::vector<ANIM::TimelineFrame*>&  get_frames();
		};
	}
}

#endif
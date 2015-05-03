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
				TYPES::F64 fps;
				TYPES::UINT16 scene_id;
				bool is_scene;
				TYPES::UINT16 obj_id_cnt;
				std::vector<ANIM::TimelineFrame*> frames;
				std::string symbol_name;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs_one_instance;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs_multiple_instances;
				ANIM::TimelineDepthManager* depth_tracks;
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
				
				
				void calc_mask_ids();
				void calculate_command_depths();
				bool is_finalized;
				TYPES::UINT32 instance_cnt;
				std::string& get_symbol_name();
				void set_symbol_name(const std::string& symbol_name);
				
				void create_timeline_childs();
				TYPES::F64 get_fps();
				void set_fps(TYPES::F64);
				void set_scene_id(int);
				int get_scene_id();
				bool get_is_scene();
				result get_frames_info(std::vector<std::string>& infos);
				
				void add_frame(ANIM::TimelineFrame*);
				ANIM::PotentialTimelineChild* get_child_for_block(BASE::AWDBlock*, ANIM::FrameCommandBase*);
				ANIM::TimelineFrame* get_frame();
				std::vector<ANIM::TimelineFrame*>&  get_frames();
		};
	}
}

#endif
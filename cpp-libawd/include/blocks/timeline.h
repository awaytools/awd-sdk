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

				std::vector<BASE::AWDBlock*> usedRessourceBlocks;
				std::vector<bool> usedReccourseBlocksUsage;

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

				TYPES::F64 get_fps();
				void set_fps(TYPES::F64);
				void set_scene_id(int);
				int get_scene_id();
				bool get_is_scene();
				result get_frames_info(std::vector<std::string>& infos);
				void reset_ressourceBlockUsage();
				void reset_ressourceBlockUsageForID(int thisID);
				int get_ressourceBlockID(BASE::AWDBlock* resBlock);
				void add_frame(ANIM::TimelineFrame*, bool modifyCommands);
				ANIM::TimelineFrame* get_frame();
				std::vector<ANIM::TimelineFrame*>&  get_frames();
		};
	}
}

#endif
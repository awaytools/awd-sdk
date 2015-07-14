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
		
		typedef std::map<TYPES::UINT32, ANIM::TimelineChild_instance*>::iterator iterate_child_map;
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

				std::vector<std::vector<TYPES::UINT32>>	all_optional_int_vecs;	//	index into command_type_stream, command_id_stream , command_index_stream	
				std::vector<std::vector<TYPES::UINT32>>	all_int_vecs;	//	index into command_type_stream, command_id_stream , command_index_stream	
				std::vector<std::vector<std::string>>	all_optional_string_vecs;	//	index into command_type_stream, command_id_stream , command_index_stream	
				std::vector<std::vector<TYPES::F32>>	all_optional_f32_vecs;	//	index into command_type_stream, command_id_stream , command_index_stream	

				// everything is defined as UINT32 in exporter, because the auto-storage-precision will store it as smallest possible datatype.

				// keyframe_stream synched
				std::vector<TYPES::UINT32>	frame_durations;
					// frame_durations are used to create frameIndex_to_keyFrame_index on import.
					// also it is used to create another keyframe_stream "frame_firstframes" on import
				std::vector<TYPES::UINT32>	frame_command_index;	// start index into the command_stream
				std::vector<TYPES::UINT32>	frame_recipe;
					// Frame recipe:	UINT8 bitflag per frame that descripes what commands to read. also stores "FullConstruct"
					// bits: 1: full contruct (no removes) / 2: executeRemoves / 3: executeAdd / 4: executeUpdate
				
				// command_stream synched
				std::vector<TYPES::UINT32>	command_length_stream;
				std::vector<TYPES::UINT32>	command_index_stream;
				
				// list of depth for remove commands (will max be UINT16)
				std::vector<TYPES::UINT32>	remove_child_stream;	
				
				// list of (childID + depth) for add commands (will max be UINT16) // stride = 2 
				// the index of a childID in the add_child_stream can be reused as sessionID
				std::vector<TYPES::UINT32>	add_child_stream;		
				

				// update_child_stream synched
				std::vector<TYPES::UINT32>	update_child_stream;
				std::vector<TYPES::UINT32>	update_child_props_indices_stream;
				std::vector<TYPES::UINT32>	update_child_props_length_array;
					
				// property_stream synched
				std::vector<TYPES::UINT32>	property_index_stream;
				std::vector<TYPES::UINT32>	property_type_stream;

				// property value streams 
				std::vector<TYPES::F32>		properties_stream_f32_mtx_pos;
				std::vector<TYPES::F32>		properties_stream_f32_mtx_scale;
				std::vector<TYPES::F32>		properties_stream_f32_mtx;
				std::vector<TYPES::F32>		properties_stream_f32_ct;
				std::vector<TYPES::UINT32>	properties_stream_int;
				std::vector<std::string>	properties_stream_strings;

				// label_stream synched. will be transformed into a dictionary to look up frame_index by label
				std::vector<std::string>	labels;
				std::vector<TYPES::UINT32>	label_indices;
				
				// frame_script_sctream synched. will be transformed into a dictionary to look up frame_script by keyframe_index
				std::vector<std::string>	frame_scripts_stream;
				std::vector<TYPES::UINT32>	frame_scripts_indices;

				std::vector<ANIM::TimelineFrame*> frames;
				std::vector<ANIM::TimelineFrame*> remove_frames;
				std::string symbol_name;
				std::map<TYPES::UINT32, ANIM::TimelineChild_instance*> timeline_childs_to_obj_id;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs_one_instance;
				std::vector<ANIM::PotentialTimelineChildGroup*> timeline_childs_multiple_instances;
				ANIM::TimelineDepthManager* depth_manager;
				ANIM::FrameCommandDisplayObject * get_prev_cmd(ANIM::TimelineChild_instance* child);
				ANIM::FrameCommandDisplayObject * get_mask_command_for_child(ANIM::TimelineChild_instance* child);
				ANIM::FrameCommandDisplayObject * get_matrix_command_by_child(ANIM::TimelineChild_instance* child);
				ANIM::FrameCommandDisplayObject * get_color_transform_command_by_child(ANIM::TimelineChild_instance* child);
				
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
				
				bool is_collected;
				bool is_finalized;
				TYPES::UINT32 instance_cnt;
				TYPES::UINT32 timeline_id;
				std::vector<ANIM::Graphic_instance*> graphic_clips;
				bool is_grafic_instance;
				bool has_grafic_instances;
				bool isButton;
				
				void create_timeline_streams();
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
				ANIM::FrameCommandDisplayObject* get_update_command_by_id(TYPES::UINT32 obj_id);
				
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
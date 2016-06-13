#ifndef _LIBAWD_TIMELINE_DEPTH_MANAGER_H
#define _LIBAWD_TIMELINE_DEPTH_MANAGER_H

#include <vector>
#include <string>
#include "base/attr.h"
#include "base/block.h"
#include "utils/awd_types.h"
#include "elements/geom_elements.h"



namespace AWD
{
	namespace BLOCKS
	{
		class MovieClip;
	}
	namespace ANIM
	{	
		class FrameCommandBase;
		class FrameCommandRemoveObject;
		class FrameCommandDisplayObject;
		class FrameCommandSoundObject;
		
		struct PotentialTimelineChild
		{
			AWD::BASE::AWDBlock* awd_block;
			bool used;
			int id;
			PotentialTimelineChild()
			{
				id=-1;
				awd_block=NULL;
				used=false;
			}
		};
		struct PotentialTimelineChildGroup
			{
			AWD::BASE::AWDBlock* awd_block;
			std::vector<PotentialTimelineChild*> childs;
			PotentialTimelineChildGroup()
			{
			}
		};
		
		struct TimelineChild_instance;
		class TimelineDepthLayer;
		struct Graphic_instance
		{
			BLOCKS::MovieClip* graphic_timeline;
			ANIM::TimelineChild_instance* graphic_child;
			ANIM::TimelineDepthLayer* insert_layer;
			std::vector<ANIM::TimelineChild_instance*> graphic_childs;
			FrameCommandDisplayObject* graphic_command;
			FrameCommandDisplayObject* current_command;
			bool dirty_masks;//used for graphic clip merging
			int offset_frames;
			Graphic_instance()
			{
				insert_layer=NULL;
				graphic_timeline=NULL;
				graphic_child=NULL;
				current_command=NULL;
				graphic_command=NULL;
				offset_frames=0;
				dirty_masks=true;
			}
			void remove_child(ANIM::TimelineChild_instance* child){
				std::vector<ANIM::TimelineChild_instance*> new_graphic_childs;
				for(ANIM::TimelineChild_instance* existingChild:graphic_childs){
					if(existingChild!=child){
						new_graphic_childs.push_back(existingChild);
					}
				}
				graphic_childs.clear();
				for(ANIM::TimelineChild_instance* existingChild:new_graphic_childs)
					graphic_childs.push_back(existingChild);
				new_graphic_childs.clear();
			}
		};
		
		struct TimelineChild_instance
		{
			ANIM::PotentialTimelineChild* child;
			ANIM::TimelineChild_instance* parent_child;
			ANIM::Graphic_instance* graphic;
			ANIM::TimelineChild_instance* parent_grafic;
			GEOM::MATRIX2x3* current_matrix;
			GEOM::ColorTransform* current_ct;
			ANIM::mask_type mask_state;
			
			bool is_masked;			//used for graphic clip merging
			bool is_mask;			//used for graphic clip merging
			int depth;
			int start_frame;
			int end_frame;
			int sessionID;
			int obj_id;
			int mask_until_objID;
			TimelineChild_instance()
			{
				mask_state=ANIM::mask_type::NONE;
				current_matrix=new GEOM::MATRIX2x3();
				current_ct=new GEOM::ColorTransform();
				parent_grafic=NULL;
				parent_child=NULL;
				is_masked=false;
				is_mask=false;
				child=NULL;
				graphic=NULL;
				depth=0;
				start_frame=1;
				end_frame=1;
				sessionID=-1;
				obj_id=-1;
				mask_until_objID=-1;
			}
			~TimelineChild_instance()
			{
				delete current_matrix;
				delete current_ct;
			}
		};
		class TimelineMaskLayer
		{
			public:
				TimelineMaskLayer();
				~TimelineMaskLayer();

				std::vector<TYPES::UINT32> mask_depths;
				std::vector<TYPES::UINT32> maskee_depths;
				std::vector<TimelineChild_instance*> mask_objects;

			
		};
		class TimelineDepthLayer
		{
			public:
				TimelineDepthLayer();
				~TimelineDepthLayer();

				std::vector<TimelineChild_instance*> depth_objs;
				int depth;
				bool in_use;
				bool is_graphic_layer;

				void add_new_child(TimelineChild_instance* child);
				void advance_frame(TYPES::UINT32 frame);
				bool is_occupied_on_frame(TYPES::UINT32 frame);
				bool is_occupied_on_frame_range(TYPES::UINT32 start, TYPES::UINT32 end);
				void apply_depth(int);
			
		};
		
		
		class TimelineDepthManager
		{
			public:
				TimelineDepthManager();
				~TimelineDepthManager();

				std::vector<TimelineDepthLayer*> depth_layers;
				bool use_as2;
				bool owns_layers;
				void apply_remove_command(FrameCommandRemoveObject* remove_cmd);
				 
				TYPES::UINT32 get_insertion_index(TimelineChild_instance* child);
				TimelineDepthLayer* get_available_layer_after_child(TimelineChild_instance* child);
				TimelineChild_instance* get_parent_for_graphic_clip(TimelineChild_instance* child);

				void add_child_after_layer(TimelineChild_instance* child, TimelineDepthLayer* after_layer);
				TimelineDepthLayer* merge_graphic_timeline(TimelineDepthManager* graphic_depth_man, TYPES::UINT32 start_frame, TimelineChild_instance* after_child);
				void reconnect_timeline_objs();
				void get_children_at_frame(TYPES::UINT32 frame_nr, std::vector<TimelineChild_instance*>&);
				void add_child_after_child(TimelineChild_instance* child, TimelineChild_instance* after_child);
				TimelineChild_instance* find_parent(TimelineChild_instance* child);
				void advance_frame(TYPES::UINT32 frame);
				void apply_depth();
		};
	}
}


#endif
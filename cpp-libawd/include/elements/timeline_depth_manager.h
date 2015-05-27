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
		class Timeline;
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
		
		
		struct TimelineChild_instance
		{
			ANIM::PotentialTimelineChild* child;
			BLOCKS::Timeline* graphic_timeline;
			FrameCommandDisplayObject* graphic_command;
			FrameCommandDisplayObject* current_command;
			ANIM::TimelineChild_instance* graphic_clip_origin;
			ANIM::TimelineChild_instance* parent_child;
			ANIM::TimelineChild_instance* insert_at_child;
			std::vector<ANIM::TimelineChild_instance*> graphic_childs;
			
			bool is_masked;//used for graphic clip merging
			bool is_mask;//used for graphic clip merging
			bool dirty_masks;//used for graphic clip merging
			bool graphic_instance;
			bool dirty;//used for graphic clip merging
			int offset_frames;
			int depth;
			int obj_id;
			int start_frame;
			int end_frame;
			TimelineChild_instance()
			{
				is_masked=false;
				is_mask=false;
				child=NULL;
				graphic_timeline=NULL;
				graphic_clip_origin=NULL;
				parent_child=NULL;
				insert_at_child=NULL;
				current_command=NULL;
				graphic_command=NULL;
				graphic_instance=false;
				dirty=false;
				offset_frames=0;
				depth=0;
				obj_id=0;
				start_frame=1;
				end_frame=1;
			}
		};
		class TimelineDepthLayer
		{
			public:
				TimelineDepthLayer();
				~TimelineDepthLayer();

				std::vector<TimelineChild_instance*> depth_objs;
				int depth;
				bool in_use;

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

				void remove_child(TimelineChild_instance* child);
				TimelineDepthLayer* get_available_layer_after_child(TimelineChild_instance* child);
				TimelineChild_instance* get_parent_for_graphic_clip(TimelineChild_instance* child);
				
				void add_child_after_child(TimelineChild_instance* child, TimelineChild_instance* after_child);
				void advance_frame(TYPES::UINT32 frame);
				void apply_depth();
		};
	}
}


#endif
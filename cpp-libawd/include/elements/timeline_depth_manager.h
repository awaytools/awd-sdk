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
			int depth;
			FrameCommandBase* command;
			PotentialTimelineChild()
			{
				command=NULL;
				id=0;
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
		
		struct TimelineDepthObject
		{
			int obj_id;
			PotentialTimelineChild* child;
			int depth;
			bool in_use;
			int start_frame;
			int end_frame;
			TimelineDepthObject()
			{
				depth=0;
				obj_id=0;
				child=NULL;
				in_use=true;
			}
		};
		struct TimelineDepthLayer
		{
			std::vector<TimelineDepthObject*> depth_objs;
			int depth;
			bool in_use;
			TimelineDepthLayer()
			{
				depth=0;
				in_use=true;
			}
			bool depth_used_for_frame_range(int start, int end)
			{
				for(TimelineDepthObject* depth_obj:depth_objs){
					if(depth_obj->obj_id==-1){
						if((depth_obj->start_frame<=start)&&(depth_obj->end_frame>=end)){
							return true; 
						}
					}
					if(depth_obj->start_frame>=end){
						// depth_objs should already be ordered by frame-range, 
						// so once startframe is out of range, we can stop searching
						return false; 
					}
				}
				return false; 
			}
			void merge_empty_objects()
			{
				std::vector<TimelineDepthObject*> new_timelinedepths_obj;
				for(TimelineDepthObject* depth_obj:depth_objs){
					if(depth_obj->obj_id<0){
						if(new_timelinedepths_obj.size()>0){
							if(new_timelinedepths_obj.back()->obj_id<0){
								new_timelinedepths_obj.back()->end_frame=depth_obj->end_frame;
								delete depth_obj;
							}
							else{
								new_timelinedepths_obj.push_back(depth_obj);
							}
						}
						else{
							new_timelinedepths_obj.push_back(depth_obj);
						}
					}
					else{
						new_timelinedepths_obj.push_back(depth_obj);
					}
				}
				this->depth_objs=new_timelinedepths_obj;
			}
			void insert_object_at(TimelineDepthObject* new_obj)
			{
				int objcnt=0;
				bool found=false;
				std::vector<TimelineDepthObject*> new_timelinedepths_obj;
				for(TimelineDepthObject* depth_obj:depth_objs){
					bool found = false;
					if(depth_obj->obj_id==-1){
						if((depth_obj->start_frame<new_obj->start_frame)&&(depth_obj->end_frame>new_obj->end_frame)){
							bool has_empty_before=false;
							if(new_timelinedepths_obj.size()>0){
								if(new_timelinedepths_obj.back()->obj_id==-1){
									new_timelinedepths_obj.back()->end_frame=new_obj->start_frame-1;
									has_empty_before=true;
								}
							}
							if(!has_empty_before){
								TimelineDepthObject* new_empty_obj=new TimelineDepthObject();
								new_empty_obj->start_frame=depth_obj->start_frame;
								new_empty_obj->end_frame=new_obj->start_frame-1;
								new_empty_obj->obj_id=-1;
								new_timelinedepths_obj.push_back(new_empty_obj);
							}
							new_timelinedepths_obj.push_back(new_obj);

							bool has_empty_after=false;
							if(objcnt<depth_objs.size()-1){
								if(depth_objs[objcnt+1]->obj_id==-1){
									depth_objs[objcnt+1]->start_frame=new_obj->end_frame+1;
									has_empty_after=true;
								}
							}
							if(!has_empty_after){
								TimelineDepthObject* new_empty_obj=new TimelineDepthObject();
								new_empty_obj->start_frame=new_obj->end_frame+1;
								new_empty_obj->end_frame= depth_obj->end_frame;
								new_empty_obj->obj_id=-1;
								new_timelinedepths_obj.push_back(new_empty_obj);
							}
							found=true;
						}
						else if((depth_obj->start_frame==new_obj->start_frame)&&(depth_obj->end_frame>new_obj->end_frame)){

							new_timelinedepths_obj.push_back(new_obj);

							bool has_empty_after=false;
							if(objcnt<depth_objs.size()-1){
								if(depth_objs[objcnt+1]->obj_id==-1){
									depth_objs[objcnt+1]->start_frame=new_obj->end_frame+1;
									has_empty_after=true;
								}
							}
							if(!has_empty_after){
								TimelineDepthObject* new_empty_obj=new TimelineDepthObject();
								new_empty_obj->start_frame=new_obj->end_frame+1;
								new_empty_obj->end_frame= depth_obj->end_frame;
								new_empty_obj->obj_id=-1;
								new_timelinedepths_obj.push_back(new_empty_obj);
							}
							found=true;
						}
						else if((depth_obj->start_frame<new_obj->start_frame)&&(depth_obj->end_frame==new_obj->end_frame)){
							bool has_empty_before=false;
							if(new_timelinedepths_obj.size()>0){
								if(new_timelinedepths_obj.back()->obj_id==-1){
									new_timelinedepths_obj.back()->end_frame=new_obj->start_frame-1;
									has_empty_before=true;
								}
							}
							if(!has_empty_before){
								TimelineDepthObject* new_empty_obj=new TimelineDepthObject();
								new_empty_obj->start_frame=depth_obj->start_frame;
								new_empty_obj->end_frame=new_obj->start_frame-1;
								new_empty_obj->obj_id=-1;
								new_timelinedepths_obj.push_back(new_empty_obj);
							}
							new_timelinedepths_obj.push_back(new_obj);
							found=true;
						}
						else if((depth_obj->start_frame==new_obj->start_frame)&&(depth_obj->end_frame==new_obj->end_frame)){
							new_timelinedepths_obj.push_back(new_obj);
							found=true;
						}
					}
					if(!found)
						new_timelinedepths_obj.push_back(depth_obj);
					objcnt++;
				}
				this->depth_objs=new_timelinedepths_obj;
			}
			
		};
		
		struct TimelineChild_instance
		{
			ANIM::PotentialTimelineChild* child;
			TimelineDepthObject* depth_obj;
			int depth;
			int obj_id;
			bool new_obj;
			ANIM::FrameCommandDisplayObject* cmd;
			TimelineChild_instance()
			{
				obj_id=0;
				child=NULL;
				depth=0;
				cmd=NULL;
				new_obj=true;
				depth_obj=NULL;
			}
		};
		class TimelineDepthManager
		{
			public:
				TimelineDepthManager();
				~TimelineDepthManager();

				std::vector<TimelineDepthLayer*> depth_layers;

				void free_unused_layers(int frame_nr);

				bool has_active_layers();
				TimelineDepthObject* get_first_active();
				int get_first_active_idx();
				std::vector<TimelineDepthObject*> timeline_depths;
				int get_depth_by_obj_id(int obj_id);
				int get_available_slots_cnt(int obj_id);
				int get_available_slots_cnt_no_target_obj();
				TimelineDepthLayer* get_depth_obj_by_id(int obj_id);
				void apply_depth();
				void set_last_frame(int frame);
				void add_objects_above_obj_id(std::vector<TimelineChild_instance*>&, int obj_id, int frame_nr);
				void add_objects(std::vector<TimelineChild_instance*>&, int frame_nr);
				void shift_depth_objects();
		};
	}
}


#endif
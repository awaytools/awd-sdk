#include "elements/frame_commands.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "blocks/geometry.h"
#include "blocks/material.h"
#include "blocks/mesh_library.h"
#include "blocks/text_element.h"
#include "utils/awd_properties.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
#include "elements/timeline_depth_manager.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "blocks/geometry.h"
#include "blocks/material.h"
#include "blocks/mesh_library.h"
#include "blocks/text_element.h"
#include "utils/awd_properties.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
 
TimelineDepthManager::TimelineDepthManager() 
{
}

TimelineDepthManager::~TimelineDepthManager()
{
}

void
TimelineDepthManager::free_unused_layers(int frame_nr)
{
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		if(!this_layer->depth_objs.back()->in_use)
			this_layer->in_use=false;
		else
			this_layer->in_use=true;
		if(!this_layer->in_use){
			if(this_layer->depth_objs.back()->obj_id>=0){
				this_layer->depth_objs.back()->end_frame=frame_nr-1;
				TimelineDepthObject* depth_obj2 = new TimelineDepthObject();
				depth_obj2->obj_id=-1;
				depth_obj2->start_frame=frame_nr;
				depth_obj2->in_use=false;
				depth_obj2->end_frame=frame_nr;
				this_layer->depth_objs.push_back(depth_obj2);
			}
		}
		//this_layer->depth_objs.back()->end_frame=frame_nr;
	}
}




bool
TimelineDepthManager::has_active_layers()
{
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		if(this_layer->in_use)
			return true;
	}
	return false;
}


void
TimelineDepthManager::set_last_frame(int frame)
{
	int cnt_obj=0;
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		this_layer->depth_objs.back()->end_frame=frame;
	}
}
void
TimelineDepthManager::apply_depth()
{
	int cnt_obj=-16383;
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		for(TimelineDepthObject* tl_depth:this_layer->depth_objs){
			tl_depth->depth=cnt_obj;
		}
		this_layer->depth=cnt_obj;
		cnt_obj+=3;
	}
}

int
TimelineDepthManager:: get_depth_by_obj_id(int obj_id)
{
	int cnt_obj=0;
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		if(this_layer->depth_objs.back()->obj_id==obj_id)
			return cnt_obj;
		if(this_layer->depth_objs.back()->obj_id==-1){
			int cur_find=this_layer->depth_objs.size()-1;
			while(cur_find--){
				if(this_layer->depth_objs[cur_find]->obj_id==-1)
					continue;
				else if(this_layer->depth_objs[cur_find]->obj_id==obj_id)
					return cnt_obj;
				else
					break;
			}
		}
		cnt_obj++;
	}
	return cnt_obj;
}
void
TimelineDepthManager::add_objects(std::vector<TimelineChild_instance*>& input_objects, int frame_nr)
{
	int count_slots = get_available_slots_cnt_no_target_obj();
	if(count_slots>input_objects.size())
		count_slots=input_objects.size();
	for(TimelineChild_instance* one_obj:input_objects){
		TimelineDepthLayer* new_depth_layer;
		if(count_slots>0){
			// a depth layer is already available and free for this object
			count_slots--;
			new_depth_layer=this->depth_layers[count_slots];

		}
		else{
			// we need to insert a new depth layer at the bottom 
			new_depth_layer = new TimelineDepthLayer();
			if(frame_nr>0){
				TimelineDepthObject* depth_obj2 = new TimelineDepthObject();
				depth_obj2->obj_id=-1;
				depth_obj2->start_frame=0;
				depth_obj2->end_frame=frame_nr-1;
				depth_obj2->in_use=false;
				new_depth_layer->depth_objs.push_back(depth_obj2);
			}
			this->depth_layers.insert(this->depth_layers.begin(), new_depth_layer);
		}		
		TimelineDepthObject* depth_obj;
		bool add_new_obj=true;
		if(new_depth_layer->depth_objs.back()->obj_id<0){
			if(new_depth_layer->depth_objs.back()->start_frame==frame_nr){
				add_new_obj=false;
				depth_obj=new_depth_layer->depth_objs.back();
			}
		}
		if(add_new_obj){
			depth_obj = new TimelineDepthObject();
			new_depth_layer->depth_objs.push_back(depth_obj);
		}		
		one_obj->depth_obj=depth_obj;
		depth_obj->obj_id=one_obj->obj_id;
		depth_obj->child=one_obj->child;
		depth_obj->start_frame=frame_nr;
		depth_obj->end_frame=frame_nr;
	}
}

void
TimelineDepthManager::add_objects_above_obj_id(std::vector<TimelineChild_instance*>& input_objects, int obj_id, int frame_nr)
{
	int layer_id = get_depth_by_obj_id(obj_id);
	int count_slots = get_available_slots_cnt(obj_id);
	bool start_inserting = false;
	int slot_cnt = 0;
	int obj_cnt=input_objects.size();
	while(obj_cnt--){
		TimelineChild_instance* one_obj=input_objects[obj_cnt];
		TimelineDepthLayer* new_depth_layer=NULL;
		slot_cnt++;
		if((count_slots>0)&&((layer_id+slot_cnt)<this->depth_layers.size())){
			// a depth layer is already available and free for this object
			count_slots--;
			new_depth_layer=this->depth_layers[layer_id+slot_cnt];
		}
		else{
			// we need to insert a new depth layer at the bottom 
			new_depth_layer = new TimelineDepthLayer();
			if(frame_nr>0){
				TimelineDepthObject* depth_obj2 = new TimelineDepthObject();
				depth_obj2->obj_id=-1;
				depth_obj2->start_frame=0;
				depth_obj2->end_frame=frame_nr-1;
				depth_obj2->in_use=false;
				new_depth_layer->depth_objs.push_back(depth_obj2);
			}
			this->depth_layers.insert(this->depth_layers.begin()+layer_id+slot_cnt, new_depth_layer);
		}
		if(new_depth_layer==NULL){

		}
		else{
			TimelineDepthObject* depth_obj;
			bool add_new_obj=true;
			if(new_depth_layer->depth_objs.back()->obj_id<0){
				if(new_depth_layer->depth_objs.back()->start_frame==frame_nr){
					add_new_obj=false;
					depth_obj=new_depth_layer->depth_objs.back();
				}
			}
			if(add_new_obj){
				depth_obj = new TimelineDepthObject();
				new_depth_layer->depth_objs.push_back(depth_obj);
			}		
			one_obj->depth_obj=depth_obj;
			depth_obj->obj_id=one_obj->obj_id;
			depth_obj->child=one_obj->child;
			depth_obj->start_frame=frame_nr;
			depth_obj->end_frame=frame_nr;
		}
	}
}

void
TimelineDepthManager::shift_depth_objects()
{
	int layer_cnt = 0;
	int obj_cnt=0;
	for(TimelineDepthLayer* depth_layer:this->depth_layers){
		if(layer_cnt>0){
			obj_cnt=0;
			depth_layer->merge_empty_objects();
			for(TimelineDepthObject* depth_obj:depth_layer->depth_objs){
				if(depth_obj->obj_id>=0){
					// check if we can move this obj to a lower layer
					int current_layer_cnt=layer_cnt;
					int possible_merge=-1;
					while (current_layer_cnt--){
						TimelineDepthLayer* lower_layer = this->depth_layers[current_layer_cnt];
						if(lower_layer->depth_used_for_frame_range(depth_obj->start_frame, depth_obj->end_frame)){
							possible_merge=current_layer_cnt;
						}
						else{
							current_layer_cnt=0;
						}
					}
					if(possible_merge>=0){
						this->depth_layers[possible_merge]->insert_object_at(depth_obj);
						this->depth_layers[possible_merge]->merge_empty_objects();
						TimelineDepthObject* new_obejct=new TimelineDepthObject();
						new_obejct->obj_id=-1;
						new_obejct->start_frame=depth_obj->start_frame;
						new_obejct->end_frame=depth_obj->end_frame;
						depth_layer->depth_objs[obj_cnt]=new_obejct;
					}
				}
				obj_cnt++;
			}
			depth_layer->merge_empty_objects();
		}
		layer_cnt++;
	}

}
int
TimelineDepthManager::get_available_slots_cnt_no_target_obj()
{
	int available_slots=0;
	for(TimelineDepthLayer* depth_layer:this->depth_layers){
		if(depth_layer->in_use){
			return available_slots;
		}
		else{
			available_slots++;
		}
	}
	return available_slots;
}
int
TimelineDepthManager:: get_available_slots_cnt(int obj_id)
{
	int available_slots=0;
	bool found_obj=false;
	for(TimelineDepthLayer* depth_layer:this->depth_layers){
		if(depth_layer->in_use){
			if(found_obj){
				return available_slots;
			}
			if(depth_layer->depth_objs.back()->obj_id==obj_id){
				found_obj=true;
			}
			available_slots=0;
		}
		else{
			available_slots++;
		}
	}
	return available_slots;
}

TimelineDepthLayer*
TimelineDepthManager:: get_depth_obj_by_id(int obj_id)
{
	for(TimelineDepthLayer* depth_layer:this->depth_layers){
		if(depth_layer->depth_objs.back()->obj_id==obj_id){
			return depth_layer;
		}
		if(depth_layer->depth_objs.back()->obj_id==-1){
			int cur_find=depth_layer->depth_objs.size()-1;
			while(cur_find--){
				if(depth_layer->depth_objs[cur_find]->obj_id==-1)
					continue;
				else if(depth_layer->depth_objs[cur_find]->obj_id==obj_id)
					return depth_layer;
				else
					break;
			}
		}
	}
	return NULL;
}

int
TimelineDepthManager:: get_first_active_idx()
{
	int cnt=0;
	for(TimelineDepthLayer* depth_layer:this->depth_layers){
		if(depth_layer->in_use)
			return cnt;
		cnt++;
	}
	return -1;
}
TimelineDepthObject*
TimelineDepthManager:: get_first_active()
{
	for(TimelineDepthLayer* depth_layer:this->depth_layers){
		if(depth_layer->in_use)
			return depth_layer->depth_objs.back();
	}
	return NULL;
}

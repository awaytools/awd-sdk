#include "elements/frame_commands.h"

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
 

TimelineDepthLayer::TimelineDepthLayer() 
{
	this->depth=0;
	this->in_use=true;
	this->is_graphic_layer=false;
}

TimelineDepthLayer::~TimelineDepthLayer()
{
}

void 
TimelineDepthLayer::advance_frame(TYPES::UINT32 frame)
{
	if(depth_objs.size()==0)
		return;
	if(depth_objs.back()->end_frame == (frame-1))
		depth_objs.back()->end_frame=frame;
}
bool 
TimelineDepthLayer::is_occupied_on_frame(TYPES::UINT32 frame)
{
	if(this->is_graphic_layer)
		return true;
	if(depth_objs.size()==0)
		return false;
	if(depth_objs.back()->end_frame >= frame)
		return true;
	return false;
}

bool 
TimelineDepthLayer::is_occupied_on_frame_range(TYPES::UINT32 start, TYPES::UINT32 end)
{
	// check all objects, because they might not be ordered by frame anymore 
	for(TimelineChild_instance* child:depth_objs){
		if((child->start_frame<=start)&&(child->end_frame>=start))
			return true; 
		if((child->start_frame<=end)&&(child->end_frame>=end))
			return true; 
	}
	return false; 
}
void
TimelineDepthLayer::apply_depth(int depth)
{
	this->depth=depth;
	for(TimelineChild_instance* tl_depth:this->depth_objs){
		tl_depth->depth=this->depth;
	}
}

void
TimelineDepthLayer::add_new_child(TimelineChild_instance* new_obj)
{
	this->depth_objs.push_back(new_obj);
}

TimelineDepthManager::TimelineDepthManager() 
{
	this->use_as2=true;
	this->owns_layers=true;
}

TimelineDepthManager::~TimelineDepthManager()
{
	if(this->owns_layers){
		for(TimelineDepthLayer* layer:this->depth_layers)
			delete layer;
	}
}


TimelineChild_instance* 
TimelineDepthManager::get_parent_for_graphic_clip(TimelineChild_instance* child)
{
	int found_layer=-1;
	int layer_cnt=0;
	TimelineChild_instance* return_child=child;
	for(TimelineDepthLayer* layer:this->depth_layers){
		if(found_layer>=0){
			if(layer->is_occupied_on_frame(child->end_frame)){
				if(layer->depth_objs.back()->parent_grafic==child)
					return_child=layer->depth_objs.back();
			}
		}
		if(layer->depth_objs.size()>0){
			if(layer->depth_objs.back()==child){
				found_layer=layer_cnt;
			}
		}

		layer_cnt++;
	}
	return return_child;
}
void
TimelineDepthManager::advance_frame(TYPES::UINT32 frame)
{
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		if(!this_layer->is_graphic_layer)
			this_layer->advance_frame(frame);
	}
}
void
TimelineDepthManager::apply_depth()
{
	
	TimelineChild_instance* parent = NULL;
	std::vector<TimelineDepthLayer*> new_layers;
	for(TimelineDepthLayer* layer:this->depth_layers){
		layer->is_graphic_layer=false;
		if(layer->depth_objs.size()>0){
			layer->is_graphic_layer=false;
			std::vector<TimelineChild_instance*> new_childs;
			for(TimelineChild_instance* child:layer->depth_objs){
				if(child->graphic==NULL){// we dont need graphic clip childs anymore
					new_childs.push_back(child);
				}
				else{
					delete child;
				}
			}
			if(new_childs.size()!=layer->depth_objs.size()){
				layer->depth_objs.clear();
				for(TimelineChild_instance* child:new_childs)
					layer->depth_objs.push_back(child);
				new_childs.clear();
			}
			if(layer->depth_objs.size()>0)
				new_layers.push_back(layer);
		}
	}
	
	/*
	// first we want to make sure that we dont waste depth space.
	// e.g. in AS2 the depth for every object should be as small as possible.
	int layer_cnt=this->depth_layers.size()-1;
	while(layer_cnt--){
		TimelineDepthLayer* layer = this->depth_layers[layer_cnt];
		std::vector<TimelineChild_instance*> new_depth_obj;
		for(TimelineChild_instance* child : layer->depth_objs){
			int cnt_up=layer_cnt+1;
			int merge_into_id=-1;
			while ((cnt_up<this->depth_layers.size())&&(!this->depth_layers[cnt_up]->is_occupied_on_frame_range(child->start_frame, child->end_frame))){
				merge_into_id=cnt_up;
				cnt_up++;
			}
			if(merge_into_id>=0)
				this->depth_layers[merge_into_id]->depth_objs.push_back(child);
			else
				new_depth_obj.push_back(child);
		}
		layer->depth_objs.clear();
		for(TimelineChild_instance* child : new_depth_obj)
			layer->depth_objs.push_back(child);
		new_depth_obj.clear();
	}
	*/
	
	this->depth_layers.clear();
	for(TimelineDepthLayer* one_layer: new_layers)
		this->depth_layers.push_back(one_layer);
	
	//this->reconnect_timeline_objs();

	int cnt_obj= ((this->depth_layers.size()-1)*3);
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		this_layer->apply_depth(cnt_obj);
		cnt_obj-=3;
	}

}

/*
* merges a graphic clip into this depth manager
*
*/
TimelineDepthLayer* 
TimelineDepthManager::merge_graphic_timeline(TimelineDepthManager* graphic_depth_man, TYPES::UINT32 frame_offset, TimelineChild_instance* after_child){

	graphic_depth_man->owns_layers=false;
	int insert_add_index=0;
	if(this->depth_layers.size()==0){
		//if(after_child!=NULL)
		//	_ASSERT(0);// error because if no layer exist, we can not find a child.
	}
	if(after_child!=NULL){
		if(after_child->graphic!=NULL){
			int lay_cnt=0;
			for(TimelineDepthLayer* layer:this->depth_layers){
				if(layer==after_child->graphic->insert_layer){
					insert_add_index=lay_cnt+1;
					break;
				}
				lay_cnt++;
			}
		}
		else{
			insert_add_index=get_insertion_index(after_child)+1;
		}
	}
	int layer_cnt=0;
	std::vector<TimelineDepthLayer*> old_layers_end;
	if(insert_add_index<this->depth_layers.size()){
		std::vector<TimelineDepthLayer*> old_layers_start;
		for(TimelineDepthLayer* old_layer:this->depth_layers){
			if(layer_cnt<insert_add_index)
				old_layers_start.push_back(old_layer);
			else
				old_layers_end.push_back(old_layer);
			layer_cnt++;
		}
		this->depth_layers.clear();
		
		for(TimelineDepthLayer* old_layer_start:old_layers_start)
			this->depth_layers.push_back(old_layer_start);
		old_layers_start.clear();
	}
	if(frame_offset>0)
		frame_offset--;
	for(TimelineDepthLayer* graphic_layer:graphic_depth_man->depth_layers){
		for(TimelineChild_instance* child:graphic_layer->depth_objs){
			child->start_frame+=frame_offset;
			child->end_frame+=frame_offset;
		}
		graphic_layer->is_graphic_layer=true;
		this->depth_layers.push_back(graphic_layer);
	}
	TimelineDepthLayer* graphic_insert_point=NULL;
	if(this->depth_layers.size()>0)
		graphic_insert_point = this->depth_layers.back();
	for(TimelineDepthLayer* old_layer_end:old_layers_end)
		this->depth_layers.push_back(old_layer_end);
	old_layers_end.clear();
	return graphic_insert_point;

}

TimelineChild_instance* 
TimelineDepthManager::find_parent(TimelineChild_instance* child)
{

	int layer_cnt=-1;
	for(TimelineDepthLayer* layer:this->depth_layers){
		layer_cnt++;
		for(TimelineChild_instance* child2:layer->depth_objs){
			if(layer->depth_objs.back()==child){
				if((child2->start_frame<=child->start_frame)&&(child2->end_frame>=child->start_frame))
					break;
			}
		}
	}
	while(layer_cnt--){
		TimelineDepthLayer* layer=this->depth_layers[layer_cnt];
		for(TimelineChild_instance* child2:layer->depth_objs){
			if((child2->start_frame<=child->start_frame)&&(child2->end_frame>=child->start_frame))
				return child2;
		}
	}
	return NULL;
}
TYPES::UINT32 
TimelineDepthManager::get_insertion_index(TimelineChild_instance* child)
{
	int layer_cnt=0;
	for(TimelineDepthLayer* layer:this->depth_layers){
		if(!layer->is_graphic_layer){
			if(layer->depth_objs.back()==child)
				return layer_cnt;
		}
		layer_cnt++;
	}
	return layer_cnt-1;
}

TimelineDepthLayer* 
TimelineDepthManager::get_available_layer_after_child(TimelineChild_instance* child)
{
	int found_layer=-1;
	int layer_cnt=0;
	for(TimelineDepthLayer* layer:this->depth_layers){
		if(found_layer>=0){
			if(layer->is_occupied_on_frame(child->end_frame)){
				int layer_cnt2=0;
				std::vector<TimelineDepthLayer*> new_layers;
				TimelineDepthLayer* new_layer = new TimelineDepthLayer();
				for(TimelineDepthLayer* layer2:this->depth_layers){
					new_layers.push_back(layer2);
					if(layer_cnt2==found_layer)
						new_layers.push_back(new_layer);
					layer_cnt2++;
				}
				this->depth_layers.clear();
				for(TimelineDepthLayer* layer2:new_layers)
					this->depth_layers.push_back(layer2);
				new_layers.clear();
				return new_layer;
			}
			else{
				// this layer is free.
				return layer;
			}
		}
		if(layer->depth_objs.back()==child)
			found_layer=layer_cnt;

		layer_cnt++;
	}
	//	_ASSERT(0); //error because parent child not found;
	TimelineDepthLayer* new_layer = new TimelineDepthLayer();
	this->depth_layers.push_back(new_layer);
	return new_layer;
}


void 
TimelineDepthManager::reconnect_timeline_objs()
{
	int max_framenr = 0;
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		if(this_layer->depth_objs.size()>0){
			for(TimelineChild_instance* child2:this_layer->depth_objs){
				if(child2->start_frame>=max_framenr){
					max_framenr=child2->start_frame;
				}
			}
		}
	}
	int cnt=0;
	for(cnt=0; cnt<=max_framenr; cnt++){
		TimelineChild_instance* parent = NULL;
		for(TimelineDepthLayer* this_layer:this->depth_layers){
			if(this_layer->depth_objs.size()>0){
				for(TimelineChild_instance* child2:this_layer->depth_objs){
					if(child2->start_frame==cnt){
						child2->parent_child=parent;
					}
					if((child2->start_frame<=cnt)&&(child2->end_frame>=cnt)){
						parent=child2;
					}
				}
			}
		}
	}
}
void 
TimelineDepthManager::get_children_at_frame(TYPES::UINT32 frame_nr, std::vector<TimelineChild_instance*>& return_childs)
{
	for(TimelineDepthLayer* this_layer:this->depth_layers){
		if(this_layer->depth_objs.size()>0){
			for(TimelineChild_instance* child:this_layer->depth_objs){
				if((child->start_frame<=frame_nr)&&(child->end_frame>=frame_nr)){
					return_childs.push_back(child);
				}
			}
		}
	}
}

void 
TimelineDepthManager::add_child_after_layer(TimelineChild_instance* child, TimelineDepthLayer* after_layer)
{
	if(this->depth_layers.size()==0){
		// no layer exists. simple. create one. add child. exit
		TimelineDepthLayer* new_layer = new TimelineDepthLayer();
		new_layer->add_new_child(child);
		this->depth_layers.push_back(new_layer);
		return;
	}
	if(after_layer==NULL){
		// should be added on top. check if first layer is empty. if yes, than add the object. if not, create a new layer on top
		TimelineDepthLayer* target_layer = this->depth_layers[0];
		if(target_layer->is_occupied_on_frame(child->end_frame)){
			std::vector<TimelineDepthLayer*> old_layers;
			for(TimelineDepthLayer* layer:this->depth_layers)
				old_layers.push_back(layer);
			// first layer is occupied, cretae a new layer and add on top.
			target_layer = new TimelineDepthLayer();
			this->depth_layers.clear();
			this->depth_layers.push_back(target_layer);
			for(TimelineDepthLayer* layer:old_layers)
				this->depth_layers.push_back(layer);
			old_layers.clear();
		}
		target_layer->add_new_child(child);
		return;
	}
	int layer_cnt1=0;
	bool layer_found=false;
	bool in=false;
	TimelineDepthLayer* target_layer;
	for(TimelineDepthLayer* layer:this->depth_layers){
		if(layer_found){
			// last layer was the parent layer - now check if layer is free
			if(layer->is_occupied_on_frame(child->end_frame)){
				// layer is not free. wee need to add a new one and insert it at this position
				std::vector<TimelineDepthLayer*> old_layers_start;
				std::vector<TimelineDepthLayer*> old_layers_end;
				int layer_cnt2=0;
				for(TimelineDepthLayer* old_layer:this->depth_layers){
					if(layer_cnt2<layer_cnt1){
						old_layers_start.push_back(old_layer);
					}
					else{
						old_layers_end.push_back(old_layer);
					}
					layer_cnt2++;
				}
				this->depth_layers.clear();
				for(TimelineDepthLayer* old_layer:old_layers_start){
					this->depth_layers.push_back(old_layer);
				}
				old_layers_start.clear();
				// first layer is occupied, cretae a new layer and add on top.
				target_layer = new TimelineDepthLayer();
				this->depth_layers.push_back(target_layer);
				target_layer->add_new_child(child);
				for(TimelineDepthLayer* old_layer:old_layers_end)
					this->depth_layers.push_back(old_layer);
				old_layers_end.clear();
				return;
			}
			layer->add_new_child(child);
			return;
		}
		if(layer==after_layer){
			layer_found=true;
		}
		layer_cnt1++;
	}
	target_layer = new TimelineDepthLayer();
	this->depth_layers.push_back(target_layer);
	target_layer->add_new_child(child);
}
void 
TimelineDepthManager::add_child_after_child(TimelineChild_instance* child, TimelineChild_instance* after_child)
{
	child->parent_child=after_child;
	if(this->depth_layers.size()==0){
		// no layer exists. simple. create one. add child. exit
		TimelineDepthLayer* new_layer = new TimelineDepthLayer();
		new_layer->add_new_child(child);
		this->depth_layers.push_back(new_layer);
		return;
	}
	if(after_child==NULL){
		// should be added on top. check if first layer is empty. if yes, than add the object. if not, create a new layer on top
		TimelineDepthLayer* target_layer = this->depth_layers[0];
		if(target_layer->is_occupied_on_frame(child->end_frame)){
			// first layer is occupied, cretae a new layer and add on top.
			target_layer = new TimelineDepthLayer();
			std::vector<TimelineDepthLayer*> old_layers;
			for(TimelineDepthLayer* layer:this->depth_layers)
				old_layers.push_back(layer);
			this->depth_layers.clear();
			this->depth_layers.push_back(target_layer);
			for(TimelineDepthLayer* layer:old_layers)
				this->depth_layers.push_back(layer);
			old_layers.clear();
		}
		target_layer->add_new_child(child);
		return;
	}
	TimelineDepthLayer* target_layer1 = get_available_layer_after_child(after_child);
	target_layer1->add_new_child(child);
}

void 
TimelineDepthManager::apply_remove_command(FrameCommandRemoveObject* remove_cmd)
{
	if(this->use_as2){
		for(TimelineDepthLayer* this_layer:this->depth_layers){
			if(this_layer->depth_objs.back()==remove_cmd->child){
				this_layer->depth_objs.back()->end_frame--;
				return;
			}
		}
	}
}

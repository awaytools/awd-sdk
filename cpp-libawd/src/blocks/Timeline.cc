#include "blocks/timeline.h"
#include <string>
//#include <crtdbg.h>
#include "elements/timeline_frame.h"
#include "elements/frame_commands.h"
#include "files/file_writer.h"

#include "base/block.h"
#include "base/attr.h"
#include "utils/util.h"


using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;


Timeline::Timeline(const std::string& name):
	BASE::AWDBlock(BLOCK::block_type::TIMELINE, name),
    BASE::AttrElementBase()
{
	depth_tracks = new TimelineDepthManager();
	this->is_scene=false;
	this->fps=0.0;
	this->is_finalized=false;
	this->instance_cnt=0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	this->has_grafic_instances=false;
	this->is_grafic_instance=true;
	
}
Timeline::Timeline():
	BASE::AWDBlock(BLOCK::block_type::TIMELINE),
    BASE::AttrElementBase()
{
	depth_tracks = new TimelineDepthManager();
	this->instance_cnt=0;
	this->is_scene=false;
	this->is_finalized=false;
	this->fps=0.0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	this->has_grafic_instances=false;
	this->is_grafic_instance=true;
	
}
Timeline::~Timeline()
{
	for (TimelineFrame * f : this->frames) 
	{
		delete f;
	}
	
	for(PotentialTimelineChildGroup* child_group: this->timeline_childs){		
		for(PotentialTimelineChild* child: child_group->childs)
			delete child;
		delete child_group;
	}
	delete depth_tracks;
}

TYPES::state Timeline::validate_block_state()
{
	return TYPES::state::VALID;
}

void
Timeline::add_frame(TimelineFrame* newFrame)
{
	this->frames.push_back(newFrame);
}


std::string&
Timeline::get_symbol_name()
{
	return this->symbol_name;
}
void
Timeline::set_symbol_name(const std::string& symbol_name)
{
	this->symbol_name = symbol_name;
}
ANIM::TimelineFrame*
Timeline:: get_frame()
{
	return this->frames.back();
}
std::vector<TimelineFrame*>&
Timeline::get_frames()
{
	return this->frames;
}

				
bool
Timeline::get_is_scene()
{
	return this->is_scene;
}

int
Timeline::get_scene_id()
{
    return this->scene_id;
}

void
Timeline::set_scene_id(int scene_id)
{
	this->is_scene=true;
    this->scene_id = scene_id;
}


TYPES::F64
Timeline::get_fps()
{
	return this->fps;
}
void
Timeline::set_fps(TYPES::F64 fps)
{
	this->fps=fps;
}



void
Timeline::calc_mask_ids()
{
	for (TimelineFrame * f : this->frames) 
		f->calc_mask_ids();
}

void
Timeline::create_timeline_childs()
{

	//	at this point each command has a obj-id.
	//	depth is defined by "place-after obj-id".
	//	now we want to translate from obj-id to child-id, and from "place after obj id" to actual depth (range top=0 - bottom = 
	
	for(PotentialTimelineChildGroup* child_group: this->timeline_childs){
		for(PotentialTimelineChild* child: child_group->childs)
			delete child;
		delete child_group;
	}

	this->timeline_childs.clear();
	int f_cnt=0;
	int obj_cnt=0;
	TimelineFrame * last_f=NULL;
	for (TimelineFrame * f : this->frames) 
	{
		if(f_cnt==0){
			// first frame should only provide "add-object" commands
			for(FrameCommandBase* newCmd : f->get_commands()){
				if(newCmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
					PotentialTimelineChild*  this_child = get_child_for_block(newCmd->get_object_block(), newCmd);
					TimelineChild_instance* timelinechild_inst = new TimelineChild_instance();
					TimelineDepthObject* depth_obj = new TimelineDepthObject();
					timelinechild_inst->child=this_child;
					timelinechild_inst->depth_obj=depth_obj;
					depth_obj->obj_id=newCmd->get_objID();
					depth_obj->start_frame=f_cnt;
					depth_obj->child=this_child;
					depth_obj->end_frame=f_cnt;
					TimelineDepthLayer* new_depth_layer = new TimelineDepthLayer();
					new_depth_layer->depth_objs.push_back(depth_obj);
					// always add at the beginning, to reverse depth
					depth_tracks->depth_layers.insert(depth_tracks->depth_layers.begin(), new_depth_layer);
					timelinechild_inst->obj_id=newCmd->get_objID();
					newCmd->child_id=this_child->id;
					newCmd->child=this_child;
					FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(newCmd);
					timelinechild_inst->cmd=fd;
					f->add_display_object_after_id(fd->get_depth(), timelinechild_inst);
				}
			}
		}
		else{
			// not first frame: all kind of commands are possible.
			f->copy_display_objects(last_f->display_objects);

			// apply all remove commands
			// this will set "is_used = false" for all remove depth_obj.
			std::vector<int> obj_ids_to_remove;
			for(FrameCommandBase* removeCmd: f->get_commands()){
				if(removeCmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_REMOVE){
					TimelineChild_instance* removed_instance=f->remove_display_object_by_id(removeCmd->get_objID());
					if(removed_instance==NULL){
						// this is a remove sound command (?)
					}
					else{
						removeCmd->child_instance=removed_instance;
						removeCmd->child=removeCmd->child_instance->child;
						removeCmd->child_id=removeCmd->child->id;
					}
				}
			}
			
			// this will set "is_used = false" for all layers that contains depth_obj with "is_used = false".
			depth_tracks->free_unused_layers(f_cnt);

			// apply the add-objects command, so that this frames contains all objects in correct depth order
			for(FrameCommandBase* newCmd: f->get_commands()){
				if(newCmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
					PotentialTimelineChild*  this_child = get_child_for_block(newCmd->get_object_block(), newCmd);
					newCmd->child=this_child;
					TimelineChild_instance* timelinechild_inst = new TimelineChild_instance();
					timelinechild_inst->child=this_child;
					timelinechild_inst->obj_id=newCmd->get_objID();
					FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(newCmd);
					timelinechild_inst->cmd=fd;
					f->add_display_object_after_id(fd->get_depth(), timelinechild_inst);
				}
				else if(newCmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE){
					TimelineChild_instance* timelinechild_inst=f->get_display_object_by_id(newCmd->get_objID());
					newCmd->prev_frame=timelinechild_inst->child->command;
					timelinechild_inst->child->command=newCmd;
					FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(newCmd);
					if(timelinechild_inst->depth_obj!=NULL){
						timelinechild_inst->depth_obj->in_use=true;
						newCmd->cur_depth=timelinechild_inst->depth;
						newCmd->child=timelinechild_inst->child;
						timelinechild_inst->cmd=fd;
					}
				}
			}
			// now we have a list of objects per frame, that is ordered by depth, depth 0 being the top
			// next step is to get the depth padded, so that we have minimum number of update commands.
			// the update-depth commands calculated by adobes generate frame commands can be ignored.


			// we can now fill the new frames objects-list into the depth_tracks and connect the display-objects with depth-objects.
			// our final depth should be reversed, so we add the commands in reversed order here

			std::vector<ANIM::TimelineChild_instance*> new_timeline_obj_to_add;
			for(ANIM::TimelineChild_instance* timeline_obj : f->display_objects){
				if(!timeline_obj->new_obj){
					// this is a object that existed in both frames.
					// we can use the obj_id to decide on what depth layer this must end up.
					// we have a list of obj that are on top of this object, but not yet added.
					// in the depth layer, this objects must be on higher depth
					TimelineDepthLayer* new_depth_layer=depth_tracks->get_depth_obj_by_id(timeline_obj->obj_id);
					if(new_depth_layer!=NULL){
						if(new_depth_layer->depth_objs.size()>0){
							timeline_obj->depth_obj = new_depth_layer->depth_objs.back();
							if(new_timeline_obj_to_add.size()>0){
								depth_tracks->add_objects_above_obj_id(new_timeline_obj_to_add, timeline_obj->obj_id, f_cnt);
							}
						}
						else{
							bool error = true;
						}
					}
					else{
						bool error = true;
					}
					new_timeline_obj_to_add.clear();
				}
				else{
					new_timeline_obj_to_add.push_back(timeline_obj);
				}
			}
			if(new_timeline_obj_to_add.size()>0){
				depth_tracks->add_objects(new_timeline_obj_to_add, f_cnt);
			}
			depth_tracks->set_last_frame(f_cnt);
			
			for(ANIM::TimelineChild_instance* timeline_obj : f->display_objects){
				if(timeline_obj->depth_obj==NULL){
					TimelineDepthLayer*  this_layer = depth_tracks->get_depth_obj_by_id(timeline_obj->obj_id);
					if(this_layer==NULL){
						bool error=true;
					}
					else{
						timeline_obj->depth_obj = this_layer->depth_objs.back();
						if(timeline_obj->depth_obj==NULL){
							bool error=false;
						}
					}
				}
			}
		}
		f_cnt++;
		last_f=f;
	}

	// at this point, each command should be connected to a Potential-Child.
	// each frame has a list of TimelineChild_instance that are connected to Depth-obejcts on the depth_tracks
	// todo: without calling shift_depth_objects() our depth is not 100% like as2. but when calling it, something gets messed up.
	//depth_tracks->shift_depth_objects();

	// if we want as3 commands, we need different path
	depth_tracks->apply_depth();
	last_f=NULL;
	for (TimelineFrame * f : this->frames) 
	{
		if(last_f!=NULL){
			std::vector<FrameCommandBase*> remove_commands;
			for(FrameCommandBase* removeCmd: f->get_commands()){
				if(removeCmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_REMOVE){
					if(removeCmd->child_instance!=NULL){
						if(removeCmd->child_instance->depth_obj!=NULL){
							removeCmd->child_instance->depth=removeCmd->child_instance->depth_obj->depth;
							removeCmd->cur_depth=removeCmd->child_instance->depth;
							remove_commands.push_back(removeCmd);
						}
					}
				}
			}
			if(remove_commands.size()>1){
				FrameCommandRemoveObject* new_framecommand_remove=new FrameCommandRemoveObject();
				new_framecommand_remove->cur_depth=remove_commands[0]->cur_depth;
				int cmd_cnt = 0;
				for(FrameCommandBase* removeCmd: remove_commands){
					if(cmd_cnt>0){
						removeCmd->is_child=true;
						new_framecommand_remove->child_commands.push_back(removeCmd);
					}
					cmd_cnt++;
				}
				f->final_commands.push_back(new_framecommand_remove);
			}
			else if(remove_commands.size()==1){
				f->final_commands.push_back(remove_commands[0]);
			}
		}
		int cnt_objs = f->display_objects.size();
		while(cnt_objs--){
			ANIM::TimelineChild_instance* tlchild_inst = f->display_objects[cnt_objs];
			if(tlchild_inst->depth_obj==NULL){
				continue;
			}
			else{
				tlchild_inst->depth=tlchild_inst->depth_obj->depth;
				if(tlchild_inst->cmd!=NULL){
					tlchild_inst->cmd->cur_depth=tlchild_inst->depth;
					f->final_commands.push_back(tlchild_inst->cmd);
				}
				else{
					bool thisIsError=true;// or is it not. there are object that migth be on timeline but have no command
				}
			}
		}
		last_f=f;
	}

	int child_id_cnt=0;
	for(PotentialTimelineChildGroup* child_group: this->timeline_childs){
		if(child_group->childs.size()==1){
			for(PotentialTimelineChild* child: child_group->childs)
				child->id=child_id_cnt++;
			this->timeline_childs_one_instance.push_back(child_group);
		}
	}
	for(PotentialTimelineChildGroup* child_group: this->timeline_childs){
		if(child_group->childs.size()>1){
			for(PotentialTimelineChild* child: child_group->childs)
				child->id=child_id_cnt++;
			this->timeline_childs_multiple_instances.push_back(child_group);
		}
	}
	for (TimelineFrame * f : this->frames) 
	{
		for(FrameCommandBase* update_command: f->get_commands()){
			if((update_command->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD)||(update_command->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE)){
				FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(update_command);
				if(fd->get_hasTargetMaskIDs()){
					if((fd->mask_ids.size()==1)&&(fd->mask_ids[0]==-1)){
					}
					else{
						std::vector<TYPES::INT32> new_mask_ids;
						for(TYPES::INT32 mask_obj_id:fd->mask_ids){
							new_mask_ids.push_back(f->get_display_object_by_id(mask_obj_id)->child->id);
						}
						fd->mask_ids=new_mask_ids;
					}
				}
			}
		}
	}
}

TYPES::UINT32
Timeline::calc_body_length(AWDFile* awd_file, BlockSettings *curBlockSettings)
{

    TYPES::UINT32 len;

    len = sizeof(TYPES::UINT16) +  TYPES::UINT16(this->get_name().size());//name
	len += sizeof(bool);// is scene
	len += sizeof(TYPES::F32);// fps
	len += sizeof(TYPES::UINT8);// sceneID
	
    len += sizeof(TYPES::UINT16);// num of potential childs with 1 instance
    len += this->timeline_childs_one_instance.size()*sizeof(TYPES::UINT32);// num of potential childs
    len += sizeof(TYPES::UINT16);// num of potential childs with multiple instance
    len += this->timeline_childs_multiple_instances.size()*(sizeof(TYPES::UINT32)+sizeof(TYPES::UINT16));// num of potential childs
	
    len += sizeof(TYPES::UINT16);// num of potential sounds
    len += this->timeline_sounds.size()*sizeof(TYPES::UINT32);// num of potential sounds

    len += sizeof(TYPES::UINT16);// num of frames
	for (TimelineFrame * f : this->frames) 
	{
		len+=f->calc_frame_length(curBlockSettings);
	}
    len += this->calc_attr_length(true,true, curBlockSettings);
	
    return len;
}


result
Timeline::get_frames_info(std::vector<std::string>& infos)
{
	int framecnt = 0;
	infos.push_back("Potential timeline childs: "+std::to_string(this->timeline_childs.size()));
	for(PotentialTimelineChildGroup* child: this->timeline_childs){
		std::string asset_type_str;
		BLOCK::get_asset_type_as_string(child->awd_block->get_type(), asset_type_str);
		std::string layer_info="	"+asset_type_str+"	'"+child->awd_block->get_name()+"' usage count = "+std::to_string(child->childs.size())+" | "; 
		for(PotentialTimelineChild* potential_child: child->childs){
			layer_info+="id: "+std::to_string(potential_child->id)+" | ";
		}
		infos.push_back(layer_info);
	}
	infos.push_back("Depth layers: "+std::to_string(this->depth_tracks->depth_layers.size()));
	for(TimelineDepthLayer* depth_layer: this->depth_tracks->depth_layers){
		std::string layer_info="	depth = "+std::to_string(depth_layer->depth); 
		for(TimelineDepthObject* depth_obj: depth_layer->depth_objs){
			std::string childID=" - ";
			if(depth_obj->child!=NULL)
				childID=std::to_string(depth_obj->child->id);
			layer_info+=" child_id: "+childID+" start: "+std::to_string(depth_obj->start_frame)+" end: "+std::to_string(depth_obj->end_frame)+" | ";
		}
		infos.push_back(layer_info);
	}
	
	for (TimelineFrame * f : this->frames) 
	{
		framecnt++;
		infos.push_back("Frame nr "+std::to_string(framecnt)+" duration = "+std::to_string(f->get_frame_duration()));
		f->get_frame_info(infos);
	}
	return AWD::result::AWD_SUCCESS;
}

/** \brief Gets a pointer to a potentialChild.
* if all potential childs for this awd_block are already in use, a new one is created.
*/
PotentialTimelineChild* 
Timeline::get_child_for_block(BASE::AWDBlock* awd_block, ANIM::FrameCommandBase* newCmd)
{
	// for each encountered awd-block, we have a PotentialTimelineChildGroup
	// this groupd provides a list of PotentialTimelineChild

	for(PotentialTimelineChildGroup* childgroup: this->timeline_childs){
		if(childgroup->awd_block==awd_block){
			for(PotentialTimelineChild* child: childgroup->childs){
				if(!child->used){
					child->used=true;
					return child;
				}
			}
			PotentialTimelineChild* newChild = new PotentialTimelineChild();
			newChild->id=this->timeline_childs.size();
			newChild->used=true;
			newChild->awd_block=awd_block;
			newChild->command=newCmd;
			childgroup->childs.push_back(newChild);
			return newChild;
		}
	}
	PotentialTimelineChildGroup* new_Group = new PotentialTimelineChildGroup();
	PotentialTimelineChild* newChild = new PotentialTimelineChild();
	newChild->id=this->timeline_childs.size();
	newChild->used=true;
	if(newChild->command!=NULL)
		newCmd->prev_frame=newChild->command;
	newChild->command=newCmd;
	newChild->awd_block=awd_block;
	new_Group->awd_block=awd_block;
	new_Group->childs.push_back(newChild);
	this->timeline_childs.push_back(new_Group);
	return newChild;
}
result 
Timeline::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{

	// before collecting the dependencies, we need to finalize the commands.

	// first get rid of all frames that are empty (no commands / no label / no framecode)
	std::vector<TimelineFrame*> newFramesList;
	TimelineFrame* lastTimeLine=NULL;
	for (TimelineFrame * f : this->frames) 
	{
		if((f->get_commands().size()>0)||(f->get_frame_code()!="")||(f->get_labels().size()>0)){
			newFramesList.push_back(f);
			lastTimeLine=f;
		}
		else{
			if(lastTimeLine!=NULL)
				lastTimeLine->set_frame_duration(f->get_frame_duration()+lastTimeLine->get_frame_duration());
			delete f;
		}
	}
	this->frames.clear();
	for(int frameCnt=0; frameCnt<newFramesList.size(); frameCnt++)
		this->frames.push_back(newFramesList[frameCnt]);
	
	
	// convert mask-ids
	this->calc_mask_ids();
	
	// create a list of PotentialTimelineChild that are used by this timeline.
	// each PotentialTimelineChild stores a reference to a awd-block
	this->create_timeline_childs();

	// finalizes the commands, so that we only store needed properties
	// also takes care of resetting object-props when neccessary (when resuing objects)
	//for (TimelineFrame * f : this->frames) 
	//	f->finalize_object_states();
	
	// now we have a list of dependencies that needs to be collected before this awd-block
	for(PotentialTimelineChildGroup* child: this->timeline_childs)
		child->awd_block->add_with_dependencies(target_file, instance_type);

	return result::AWD_SUCCESS;
}
result
Timeline::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings *curBlockSettings, AWDFile* awd_file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);// name	
	
	fileWriter->writeUINT8(this->is_scene);// is scene
	fileWriter->writeUINT8(this->scene_id);// sceneID //TODO
	fileWriter->writeFLOAT32(this->fps);// fps
	
	fileWriter->writeUINT16(TYPES::UINT16(this->timeline_childs_one_instance.size()));
	for(PotentialTimelineChildGroup* child: this->timeline_childs_one_instance){
		TYPES::UINT32 block_addr=0;
		if (child->awd_block != NULL)
			child->awd_block->get_block_addr(awd_file, block_addr);
		fileWriter->writeUINT32(block_addr);
	}
	
	fileWriter->writeUINT16(TYPES::UINT16(this->timeline_childs_multiple_instances.size()));
	for(PotentialTimelineChildGroup* child: this->timeline_childs_multiple_instances){
		TYPES::UINT32 block_addr=0;
		if (child->awd_block != NULL)
			child->awd_block->get_block_addr(awd_file, block_addr);
		fileWriter->writeUINT32(block_addr);
		fileWriter->writeUINT16(child->childs.size());
	}

	fileWriter->writeUINT16(TYPES::UINT16(this->timeline_sounds.size()));
	for(PotentialTimelineChild* sound: this->timeline_sounds){
		TYPES::UINT32 block_addr=0;
		if (sound->awd_block != NULL)
			sound->awd_block->get_block_addr(awd_file, block_addr);
		fileWriter->writeUINT32(block_addr);
	}

	fileWriter->writeUINT16(TYPES::UINT16(this->frames.size()));// num of frames	
	for (TimelineFrame * f : this->frames) 
	{
		f->write_frame(fileWriter, curBlockSettings, awd_file);
	}
	
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
	return result::AWD_SUCCESS;
}

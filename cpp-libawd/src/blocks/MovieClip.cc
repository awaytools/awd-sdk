#include "blocks/movieclip.h"
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


MovieClip::MovieClip(const std::string& name):
	BASE::AWDBlock(BLOCK::block_type::MOVIECLIP, name),
    BASE::AttrElementBase()
{
	depth_manager = new TimelineDepthManager();
	this->is_collected=false;
	this->isButton=false;
	this->is_scene=false;
	this->fps=0.0;
	this->is_finalized=false;
	this->instance_cnt=0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	this->has_grafic_instances=false;
	this->is_grafic_instance=true;
	this->current_frame=1;
	this->all_scripts="";
	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=25.0;
	this->properties->add(1,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);

	default_union.v=malloc(sizeof(TYPES::UINT16));
	*default_union.ui16=0;
	this->properties->add(2,	default_union, 2,   data_types::UINT16, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::UINT8));
	*default_union.ui8=0;
	this->properties->add(3,	default_union, 1,   data_types::UINT16, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
}
MovieClip::MovieClip():
	BASE::AWDBlock(BLOCK::block_type::MOVIECLIP),
    BASE::AttrElementBase()
{
	depth_manager = new TimelineDepthManager();
	this->is_collected=false;
	this->isButton=false;
	this->instance_cnt=0;
	this->is_scene=false;
	this->is_finalized=false;
	this->fps=0.0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	this->has_grafic_instances=false;
	this->is_grafic_instance=true;
	this->current_frame=1;
	this->all_scripts="";
	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=25.0;
	this->properties->add(1,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);

	default_union.v=malloc(sizeof(TYPES::UINT16));
	*default_union.ui16=0;
	this->properties->add(2,	default_union, 2,   data_types::UINT16, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::UINT8));
	*default_union.ui8=0;
	this->properties->add(3,	default_union, 1,   data_types::UINT8, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	
}
MovieClip::~MovieClip()
{
	for (TimelineFrame * f : this->frames) 
		delete f;
	this->frames.clear();
	for (TimelineFrame * f : this->remove_frames) 
		delete f;
	this->remove_frames.clear();
	
	for(PotentialTimelineChildGroup* child_group: this->timeline_childs){
		for(PotentialTimelineChild* child: child_group->childs)
			delete child;
		child_group->childs.clear();
		delete child_group;
	}
	this->timeline_childs.clear();

	for(Graphic_instance* graphic: this->graphic_clips)
		delete graphic;
	this->graphic_clips.clear();
	
	for(iterate_child_map iterator = timeline_childs_to_obj_id.begin(); iterator != timeline_childs_to_obj_id.end(); iterator++) 
		delete iterator->second;
	timeline_childs_to_obj_id.clear();
	
	delete depth_manager;
}

TYPES::state MovieClip::validate_block_state()
{
	return TYPES::state::VALID;
}

////////////////////////

std::string&
MovieClip::get_symbol_name()
{
	return this->symbol_name;
}
void
MovieClip::set_symbol_name(const std::string& symbol_name)
{
	this->symbol_name = symbol_name;
}

ANIM::TimelineFrame*
MovieClip:: get_frame()
{
	return this->frames.back();
}

std::vector<TimelineFrame*>&
MovieClip::get_frames()
{
	return this->frames;
}

bool
MovieClip::get_is_scene()
{
	return this->is_scene;
}

int
MovieClip::get_scene_id()
{
    return this->scene_id;
}
void
MovieClip::set_scene_id(int scene_id)
{
	this->is_scene=true;
    this->scene_id = scene_id;
}


TYPES::F64
MovieClip::get_fps()
{
	return this->fps;
}
void
MovieClip::set_fps(TYPES::F64 fps)
{
	this->fps=fps;
}


void
MovieClip::add_adobe_frame(TimelineFrame* newFrame)
{
	newFrame->startframe=this->current_frame;
	this->current_frame++;
	this->frames.push_back(newFrame);
}

FrameCommandDisplayObject*
MovieClip::get_update_command_by_id(TYPES::UINT32 objectID)
{
	return this->frames.back()->get_update_command_by_id(objectID);
}

void
MovieClip::remove_object_by_id(TYPES::UINT32 obj_id)
{
	this->frames.back()->remove_object_by_id(obj_id);
}

FrameCommandDisplayObject*
MovieClip::add_display_object_by_id(TYPES::UINT32 objectID, TYPES::UINT32 add_after_id)
{
	
	AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)this->frames.back()->add_display_object_by_id(objectID, add_after_id);
	frameCommand->set_depth(add_after_id);
	return frameCommand;
}

/*
bool
MovieClip::test_depth_ids(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID)
{
	return this->frames.back()->test_depth_ids(objectID, add_after_ID);
}
*/

// used while finalizing the commands:

/** \brief Add a new frame
*/
void
MovieClip::add_frame(TimelineFrame* newFrame)
{
	this->frames.push_back(newFrame);
}



/** \brief Gets a pointer to a PotentialTimelineChild for a AWDBlock.
* If all existing PotentialTimelineChild for this AWDBlock are already in use, a new one is created.
* Per AWDBlock, we store 1 PotentialTimelineChildGroups.
*/
PotentialTimelineChild* 
MovieClip::get_child_for_block(BASE::AWDBlock* awd_block)
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
			childgroup->childs.push_back(newChild);
			return newChild;
		}
	}
	PotentialTimelineChildGroup* new_Group = new PotentialTimelineChildGroup();
	PotentialTimelineChild* newChild = new PotentialTimelineChild();
	newChild->id=this->timeline_childs.size();
	newChild->used=true;
	newChild->awd_block=awd_block;
	new_Group->awd_block=awd_block;
	new_Group->childs.push_back(newChild);
	this->timeline_childs.push_back(new_Group);
	return newChild;
}

void 
MovieClip::create_timeline_streams()
{
	this->frame_durations.clear();

	
	this->remove_child_stream.clear();
	this->add_child_stream.clear();
	this->frame_scripts_stream.clear();
	
	this->update_child_stream.clear();

	
	
	this->property_type_stream.clear();
	this->property_index_stream.clear();

	this->properties_stream_int.clear();
	this->properties_stream_strings.clear();

	

	this->labels.clear();
	this->label_indices.clear();
	
	std::map<std::string, TYPES::UINT32> map_matrix_idx;
	std::map<std::string, TYPES::UINT32> map_colortransform_idx;
	std::map<std::string, TYPES::UINT32> map_mask_ids;
	std::map<std::string, TYPES::UINT32> map_instancenames;
	std::vector<bool> new_vec;
	TYPES::INT32 session_cnt = 0;
	TYPES::UINT32 keyframe_cnt = 0;
	TYPES::UINT32 frame_cnt = 1;
	for (TimelineFrame * f : this->frames) 
	{
		this->frame_durations.push_back(f->get_frame_duration());
		this->frame_command_index.push_back(this->command_index_stream.size());
		
		for(std::string one_label:f->get_labels()){
			this->labels.push_back(one_label);
			this->label_indices.push_back(keyframe_cnt);
		}
			
		if (f->get_frame_code()!=""){
			
			this->frame_scripts_stream.push_back(f->get_frame_code());
			this->frame_scripts_mc_names.push_back(this->get_name());
			this->frame_scripts_frameindex.push_back(frame_cnt);
			this->frame_scripts_indices.push_back(keyframe_cnt);
		}
		frame_cnt+=f->get_frame_duration();
		
		TYPES::UINT32 command_cnt = 0;
		TYPES::UINT8 command_recipe_flag=0;
		TYPES::UINT32 start_index = this->remove_child_stream.size();
		
		if(f->isFullConstruct){
			command_recipe_flag |= 0x01;
			command_cnt=0;
		}
		else{
			for(FrameCommandRemoveObject* removecmd : f->remove_commands)
			{
				this->remove_child_stream.push_back(removecmd->child->depth);
				command_cnt++;
			}
		}
		//command_cnt
		if(command_cnt>0){
			command_recipe_flag |= 0x02;
			this->command_length_stream.push_back(command_cnt);// just to keep in sync
			this->command_index_stream.push_back(start_index);// just to keep in sync
			
		}
		
		command_cnt = 0;
		start_index = this->add_child_stream.size()/2;	
		for(FrameCommandDisplayObject* displaycmd : f->display_commands){
			if(displaycmd->get_command_type()!=frame_command_type::FRAME_COMMAND_UPDATE)
			{
				this->add_child_stream.push_back(displaycmd->child->child->id);
				this->add_child_stream.push_back(displaycmd->child->depth);
				displaycmd->child->sessionID=session_cnt++;

				// if this is a textelement, we use the awd-block name as instance name
				if(displaycmd->child->child->awd_block->get_type()==block_type::TEXT_ELEMENT){
					if(displaycmd->child->child->awd_block->get_name()!=""){
						displaycmd->set_instanceName(displaycmd->child->child->awd_block->get_name());
					}
				}
				command_cnt++;
			}
		}
		if(command_cnt>0){
			command_recipe_flag |= 0x04;
			this->command_length_stream.push_back(command_cnt);// just to keep in sync
			this->command_index_stream.push_back(start_index);// just to keep in sync
		}
		
		command_cnt = 0;
		start_index = this->update_child_stream.size();	
		for(FrameCommandDisplayObject* displaycmd : f->display_commands){
			if(displaycmd->has_update_properties()){
				
				this->update_child_stream.push_back(displaycmd->child->sessionID);//displaycmd->child->child->id);////
				this->update_child_props_indices_stream.push_back(this->property_type_stream.size());
				
				TYPES::INT16 num_updated_props = 0;

				if(displaycmd->get_hasDisplayMatrix()){
					int this_save_type=displaycmd->get_display_matrix()->get_save_type();
					if(this_save_type!=0){
						num_updated_props++;
                        this->property_type_stream.push_back(this_save_type);
						std::string matrix_str;
						displaycmd->get_display_matrix()->toString(matrix_str);
						if(map_matrix_idx.find(matrix_str)!=map_matrix_idx.end()){
							this->property_index_stream.push_back(map_matrix_idx[matrix_str]);
						}
						else{
							if(this_save_type==1){
								this->property_index_stream.push_back(this->properties_stream_f32_mtx.size()/6);
								map_matrix_idx[matrix_str]=this->properties_stream_f32_mtx.size()/6;
								displaycmd->get_display_matrix()->fill_into_list(this->properties_stream_f32_mtx);
							}
							else if(this_save_type==11){
								this->property_index_stream.push_back(this->properties_stream_f32_mtx_scale.size()/4);
								map_matrix_idx[matrix_str]=this->properties_stream_f32_mtx_scale.size()/4;
								displaycmd->get_display_matrix()->fill_into_list(this->properties_stream_f32_mtx_scale);
							}
							else if(this_save_type==12){
								this->property_index_stream.push_back(this->properties_stream_f32_mtx_pos.size()/2);
								map_matrix_idx[matrix_str]=this->properties_stream_f32_mtx_pos.size()/2;
								displaycmd->get_display_matrix()->fill_into_list(this->properties_stream_f32_mtx_pos);
							}
						}
					}
				}
				if(displaycmd->get_hasColorMatrix()){
					num_updated_props++;
					this->property_type_stream.push_back(2);
					std::string color_str;
					displaycmd->get_color_matrix()->toString(color_str);
					if(map_colortransform_idx.find(color_str)!=map_colortransform_idx.end()){
						this->property_index_stream.push_back(map_colortransform_idx[color_str]);
					}
					else{
						this->property_index_stream.push_back(this->properties_stream_f32_ct.size()/8);
						map_colortransform_idx[color_str]=this->properties_stream_f32_ct.size()/8;
						displaycmd->get_color_matrix()->fill_into_list(this->properties_stream_f32_ct);
					}
				}
				if(displaycmd->reset_masks){
					num_updated_props++;
					this->property_type_stream.push_back(201);
					this->property_index_stream.push_back(0);
				}
				if(displaycmd->get_hasTargetMaskIDs()){
					if(displaycmd->mask==ANIM::mask_type::MASK){
						num_updated_props++;
						this->property_type_stream.push_back(200);
						this->property_index_stream.push_back(0);
					}

					if(displaycmd->mask==ANIM::mask_type::MASKED){
						if(displaycmd->mask_child_levels.size()>0){
							for(std::vector<TimelineChild_instance* > mask_group:displaycmd->mask_child_levels){
								num_updated_props++;
								this->property_type_stream.push_back(3);
								std::string mask_str = "";
								for(TimelineChild_instance* mask_child:mask_group){
									mask_str+=std::to_string(mask_child->sessionID)+"#";
								}
								if(map_mask_ids.find(mask_str)!=map_mask_ids.end()){
									this->property_index_stream.push_back(map_mask_ids[mask_str]);
								}
								else{
									this->property_index_stream.push_back(this->properties_stream_int.size());
									map_mask_ids[mask_str]=this->properties_stream_int.size();
									this->properties_stream_int.push_back(mask_group.size());
									for(TimelineChild_instance* mask_child:mask_group){
										this->properties_stream_int.push_back((mask_child->sessionID));
									}
								}
							}
						}
					}
				}

				if(displaycmd->get_instanceName()!=""){
					num_updated_props++;
					// we have 2 objects that can have instance names: movieclip instances, and button-instances
					if(displaycmd->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_BUTTON_CHILD){
						this->property_type_stream.push_back(5);
					}
					else{
						this->property_type_stream.push_back(4);
					}
					if(map_instancenames.find(displaycmd->get_instanceName())!=map_instancenames.end()){
						this->property_index_stream.push_back(map_instancenames[displaycmd->get_instanceName()]);
					}
					else{
						this->property_index_stream.push_back(this->properties_stream_strings.size());
						map_instancenames[displaycmd->get_instanceName()]=this->properties_stream_strings.size();
						this->properties_stream_strings.push_back(displaycmd->get_instanceName());
					}
				}

				if(displaycmd->get_hasVisiblitiyChange()){
					num_updated_props++;
					this->property_type_stream.push_back(6);
					if(displaycmd->get_visible())
						this->property_index_stream.push_back(1);
					else
						this->property_index_stream.push_back(0);
				}
				if(displaycmd->get_hasBlendModeChange()){
					// hack to export blendmode:erase as cacheing
					if(false){
						num_updated_props++;
						this->property_type_stream.push_back(8);
						if(displaycmd->get_blendmode()==10){//todo: get correct int for erase (10 is multiply)
							this->property_index_stream.push_back(1);
						}
						else{
							this->property_index_stream.push_back(0);
						}
					}
				}
				command_cnt++;
				this->update_child_props_length_array.push_back(num_updated_props);
			}
		}
		if(command_cnt>0){
			command_recipe_flag |= 0x08;
			this->command_length_stream.push_back(command_cnt);// just to keep in sync
			this->command_index_stream.push_back(start_index);// just to keep in sync
		}
		

		this->frame_recipe.push_back(command_recipe_flag);
		keyframe_cnt++;

	}
	// there must exists at least 1 keyframe, so keyframestreams are not optional
	this->all_int_vecs.push_back(this->frame_durations);//0
	this->all_int_vecs.push_back(this->frame_command_index);//1
	this->all_int_vecs.push_back(this->frame_recipe);//2
	
	// there must exists at least 1 command, so command_streams are not optional
	this->all_int_vecs.push_back(this->command_length_stream);//3
	this->all_int_vecs.push_back(this->command_index_stream);//4
	
	// there must exists at least 1 addchildcommand, so add_child_stream are not optional
	this->all_int_vecs.push_back(this->add_child_stream);//5
	
	// the rest is optional
	this->all_optional_int_vecs.push_back(this->remove_child_stream);//6
	this->all_optional_int_vecs.push_back(this->update_child_stream);//7
	this->all_optional_int_vecs.push_back(this->update_child_props_indices_stream);//8
	this->all_optional_int_vecs.push_back(this->update_child_props_length_array);//9
	this->all_optional_int_vecs.push_back(this->property_type_stream);//10
	this->all_optional_int_vecs.push_back(this->property_index_stream);//11
	this->all_optional_int_vecs.push_back(this->properties_stream_int);//12
	
	this->all_optional_f32_vecs.push_back(this->properties_stream_f32_mtx_scale);//13
	this->all_optional_f32_vecs.push_back(this->properties_stream_f32_mtx_pos);//14
	this->all_optional_f32_vecs.push_back(this->properties_stream_f32_mtx);//15
	this->all_optional_f32_vecs.push_back(this->properties_stream_f32_ct);//16
	
}

/** \brief Advance the playhead to the next frame.
* After we recieved all Adobe-frameCommands, we play through the timeline, to set depthmanager, and merge graphic clips
*/
void 
MovieClip::advance_frame(TimelineFrame* frame)
{
	//	check if any of the alive graphic-instances provides a keyframe for the current frame.
	//	set all visible graphic-instances to dirty = true.
	bool has_graphic_keyframes=false;
	for(ANIM::Graphic_instance* graphic_clip : graphic_clips){
		MovieClip* timeline = reinterpret_cast<MovieClip*>(graphic_clip->graphic_timeline);
		if(timeline->has_frame_at(this->current_frame-graphic_clip->offset_frames)){
			has_graphic_keyframes=true;
		}
		graphic_clip->dirty_masks=false;
		graphic_clip->current_command=NULL;
	}

	//	no matter what, the depthmanager must advance 1 frame.
	//	this will set the last child on each layer of the depth_manager to end_frame = this->current_frame
	this->depth_manager->advance_frame(this->current_frame);

	// no keyframe is available for this frame (neither for timeline nor for any of the graphic instances that are alive)
	if((!has_graphic_keyframes)&&(frame==NULL)){
		// no need to add a frame, but we need to extent the last duration of last frame.
		this->frames.back()->set_frame_duration(this->frames.back()->get_frame_duration()+1);
		return;
	}

	// if we got here, we definitv have to create a keyframe for this frame
	
	// if we have a input frame, we will resuse it (extract the commands, and clear its command lists) 
	// if we have no 
	TimelineFrame* final_frame;
	std::vector<FrameCommandDisplayObject*> input_display_commands;
	std::vector<FrameCommandRemoveObject*> input_remove_commands;
	if(frame){
		final_frame=frame;
		for(FrameCommandRemoveObject* removeCmd: frame->remove_commands)
			input_remove_commands.push_back(removeCmd);
		frame->remove_commands.clear();
		for(FrameCommandDisplayObject* dispalyCmd: frame->display_commands)
			input_display_commands.push_back(dispalyCmd);
		frame->display_commands.clear();
	}
	else		
		final_frame=new TimelineFrame();
	
	final_frame->set_frame_duration(1);				//	duration will be extendet if empty frames follow
	final_frame->startframe=this->current_frame;	//	keep track of startframe for debug purpose

	// add the new frame to the timeline. this will create empty update commands for all objects that exists on previous frame
	this->add_frame(final_frame);
	
	TimelineChild_instance* current_child=NULL;
	TimelineFrame* clip_frame=NULL;
	TimelineChild_instance* parent_child = NULL;
	AWDBlock* current_block = NULL;
	BLOCKS::MovieClip* child_timeline = NULL;
	// EVAL REMOVE COMMANDS FOR FRAME:
	for(FrameCommandRemoveObject* removeCmd: input_remove_commands){
		
		current_child = timeline_childs_to_obj_id[removeCmd->objID];
		current_child->child->used=false;
		removeCmd->child=current_child;
		
		if(current_child->graphic!=NULL){
			// Add Remove_commands for all the childs that are present on the stage, and was added for this graphic clip
			for(TimelineChild_instance* grafic_child:current_child->graphic->graphic_childs){
				FrameCommandRemoveObject* new_gc_remove_command=new FrameCommandRemoveObject();
				new_gc_remove_command->child=grafic_child;
				grafic_child->child->used=false;
				final_frame->apply_remove_command(new_gc_remove_command);
				this->depth_manager->apply_remove_command(new_gc_remove_command);
			}
			// remove the graphic clip from the graphic-clip-list
			if(graphic_clips.size()>1){
				std::vector<Graphic_instance* > new_graphic_clips;
				for(Graphic_instance* old_graphic_clip : this->graphic_clips){
					if(old_graphic_clip!=current_child->graphic)
						new_graphic_clips.push_back(old_graphic_clip);
					else
						delete old_graphic_clip;
					
				}
				this->graphic_clips.clear();
				for(Graphic_instance* old_graphic_clip : new_graphic_clips)
					this->graphic_clips.push_back(old_graphic_clip);
				new_graphic_clips.clear();
			}
			else{
				graphic_clips.clear();
			}
		}
		else{
			final_frame->apply_remove_command(removeCmd);
			this->depth_manager->apply_remove_command(removeCmd);
		}
	}
	
	// EVAL REMOVE COMMANDS FOR ACTIV GRAPHIC CLIPS:
	if(has_graphic_keyframes){
		for(Graphic_instance* graphic_clip : graphic_clips){
			if(graphic_clip->graphic_timeline->has_frame_at(this->current_frame-graphic_clip->offset_frames)){
				clip_frame = graphic_clip->graphic_timeline->get_frame_at(this->current_frame-graphic_clip->offset_frames);

				// copy stuff over from the frame
				final_frame->set_frame_code(clip_frame->get_frame_code());
				for(FrameCommandRemoveObject* removeCmd2: clip_frame->remove_commands){
					removeCmd2->child->child->used=false;
					final_frame->apply_remove_command(removeCmd2);
					//this->depth_manager->apply_remove_command(removeCmd2);
					graphic_clip->remove_child(removeCmd2->child);
				}
				clip_frame->remove_commands.clear();
			}
		}
	}
	// take care of update commands for childs inserted for graphic clips
	for(Graphic_instance* graphic_clip : graphic_clips){
		
		for(TimelineChild_instance* gc_child:graphic_clip->graphic_childs){
			gc_child->end_frame=this->current_frame;
		}
	}
	final_frame->isFullConstruct=false;
	std::vector<TimelineChild_instance*> active_childs;
	this->depth_manager->get_children_at_frame(this->current_frame, active_childs);
	if(active_childs.size()==0){
		final_frame->isFullConstruct=true;
	}
	
	// EVAL COMMANDS FOR FRAME:
	for(FrameCommandDisplayObject* display_cmd: input_display_commands){
		if(display_cmd->get_command_type()!=ANIM::frame_command_type::FRAME_COMMAND_UPDATE){
			current_block = display_cmd->get_object_block();// create a new child-instance for this object
			current_child = new TimelineChild_instance();
			current_child->mask_state=display_cmd->mask;
			current_child->start_frame=this->current_frame;
			current_child->end_frame=this->current_frame;
			current_child->obj_id=display_cmd->get_objID();
			if(display_cmd->mask==ANIM::mask_type::MASK){
				current_child->mask_until_objID=display_cmd->get_clipDepth();
			}

			current_child->child=this->get_child_for_block(current_block);
					
			timeline_childs_to_obj_id[display_cmd->get_objID()]=current_child;
				
			display_cmd->child=current_child;
			
			parent_child = NULL;
			if(display_cmd->get_depth()!=0)
				parent_child=timeline_childs_to_obj_id[display_cmd->get_depth()];

			bool isgrafic=false;
			// check if the object is a timeline, and if so, make sure it is finalized (recursion), and check if it is a graphic instance 
			if(current_block->get_type()==BLOCK::block_type::MOVIECLIP){
				child_timeline=reinterpret_cast<BLOCKS::MovieClip*>(current_block);
				// make sure that the timeline has been finalized itself
				child_timeline->finalize();
				
				if(child_timeline->is_grafic_instance){
					isgrafic=true;
					// this is a graphic instance.
					Graphic_instance* new_graphic_clip = new Graphic_instance();
					new_graphic_clip->graphic_timeline=child_timeline;
					new_graphic_clip->graphic_command=display_cmd;
					new_graphic_clip->offset_frames=current_frame-1;
					new_graphic_clip->graphic_command=display_cmd;
					new_graphic_clip->current_command=display_cmd;
					new_graphic_clip->graphic_child=current_child;
					current_child->graphic=new_graphic_clip;
					new_graphic_clip->insert_layer=this->depth_manager->merge_graphic_timeline(child_timeline->depth_manager, this->current_frame, parent_child); 
					this->graphic_clips.push_back(new_graphic_clip);
					
				}
			}
			if(!isgrafic){
				// get the parent object, using the obj-id
				if(parent_child!=NULL){
					if(parent_child->graphic!=NULL){
						this->depth_manager->add_child_after_layer(current_child, parent_child->graphic->insert_layer);
					}
					else{
						this->depth_manager->add_child_after_child(current_child, parent_child);
					}
				}
				else{
					this->depth_manager->add_child_after_child(current_child, parent_child);
					
				}
				final_frame->apply_add_command(display_cmd, parent_child);
			}
		}
		else if(display_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE){

			current_child = timeline_childs_to_obj_id[display_cmd->get_objID()];
			display_cmd->child=current_child;

			final_frame->apply_update_command(display_cmd);
			
			if(display_cmd->mask==ANIM::mask_type::MASK){
				current_child->mask_until_objID=display_cmd->get_clipDepth();
			}

			if(current_child->graphic!=NULL){
				current_child->graphic->current_command=display_cmd;
				if(display_cmd->get_hasDisplayMatrix())
					current_child->graphic->graphic_command->set_display_matrix(display_cmd->get_display_matrix()->get());
				if(display_cmd->get_hasColorMatrix())
					current_child->graphic->graphic_command->set_color_matrix(display_cmd->get_color_matrix()->get());
				//if(display_cmd->get_hasTargetMaskIDs())
					//current_child->graphic->graphic_command->mask_ids=display_cmd->mask_ids;
			}
		}
	}
	

	
	// EVAL COMMANDS FOR ACTIV GRAPHIC CLIPS:
	for(Graphic_instance* graphic_clip : graphic_clips){
		if(graphic_clip->graphic_timeline->has_frame_at(this->current_frame - graphic_clip->offset_frames)){
			TimelineFrame* clip_frame = graphic_clip->graphic_timeline->get_frame_at(this->current_frame - graphic_clip->offset_frames);
			for(FrameCommandDisplayObject* dp_cmd: clip_frame->display_commands){
				if(dp_cmd->get_command_type()!=frame_command_type::FRAME_COMMAND_UPDATE){
					dp_cmd->child->child=this->get_child_for_block(dp_cmd->get_object_block());
					dp_cmd->child->parent_grafic=graphic_clip->graphic_child;
					dp_cmd->child->start_frame=this->current_frame;

					final_frame->apply_add_command(dp_cmd, parent_child);

					graphic_clip->graphic_childs.push_back(dp_cmd->child);
				}
				else
					final_frame->apply_update_command(dp_cmd);
			}
			clip_frame->display_commands.clear();
		}
	}
	Graphic_instance* graphic_clip=NULL;
	FrameCommandDisplayObject* parent_command=NULL;
	
	// makeing sure all existing object have correct update commands available:
	
	// masking
	std::vector<TimelineChild_instance*> existing_childs;
	std::vector<TimelineChild_instance*> collected_mask_objects;
	depth_manager->get_children_at_frame(this->current_frame, existing_childs);
	TYPES::INT32 mask_up_to_id=-1;
	TYPES::UINT8 mask_collect_state=0;// 3 states:	nothing - collecting masks - applying masks - 

	// loop over all children that exists on this frame. they are ordered front to back
	int i=0;
	int k=0;
	for (i=0; i<existing_childs.size();i++){
		TimelineChild_instance* existing_child = existing_childs[i];
		// if this object is coming from a merged clip, we need to check the merged clip for its mask props
		if(existing_child->parent_grafic!=NULL){
			if(existing_child->parent_grafic->mask_state==ANIM::mask_type::MASK){
				int start=i;
				for (k=start; k<existing_childs.size();k++){
					TimelineChild_instance* existing_graphic_child = existing_childs[k];
					if(existing_graphic_child->parent_grafic==existing_child->parent_grafic){
						// todo: get the display command for this child on this frame (or create if it doesnt exists)
						// set the mask id -1 on it
						// check if its already used as mask. in this case, this would be error
						// becasue we do not support mask as child of mask
						FrameCommandDisplayObject* child_command = final_frame->get_update_command_by_child(existing_graphic_child);
						if(child_command==NULL){
							child_command=new FrameCommandDisplayObject();
							child_command->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
							child_command->child = existing_graphic_child;
							final_frame->add_command(child_command);
						}
						child_command->mask=ANIM::mask_type::MASK;

						collected_mask_objects.push_back(existing_graphic_child);
						i++;// skip this child in the main loop
					}
					else{
						k=existing_childs.size();
					}
				}
				if(i!=start)
					i--;
				// this is a mask. add it to the queued masks. set the mask_up_to_id if it is not the same objID as the child itself
				if(existing_child->parent_grafic->mask_until_objID!=existing_child->parent_grafic->obj_id){
					mask_up_to_id=existing_child->parent_grafic->mask_until_objID;
				}
			}
			else{
				// this is not a mask. 
				if(mask_up_to_id>=0){	// we have a active mask queue
					
					int start=i;
					for (k=start; k<existing_childs.size();k++){
						TimelineChild_instance* existing_graphic_child = existing_childs[k];
						if(existing_graphic_child->parent_grafic==existing_child->parent_grafic){
							
							FrameCommandDisplayObject* child_command = final_frame->get_update_command_by_child(existing_graphic_child);
							if(child_command==NULL){
								child_command=new FrameCommandDisplayObject();
								child_command->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
								child_command->child = existing_graphic_child;
								final_frame->add_command(child_command);
							}
							if(child_command->mask!=ANIM::mask_type::MASK){
								child_command->mask=ANIM::mask_type::MASKED;
								child_command->set_hasTargetMaskIDs(true);
								child_command->add_mask_childs(collected_mask_objects);
							}
							i++;	// skip this child in the main loop
						}
						else{
							k=existing_childs.size();
						}
					}
					if(i!=start)
						i--;
					if(mask_up_to_id==existing_child->parent_grafic->obj_id){// this is the objectID where the masking should stop.
						collected_mask_objects.clear();
						mask_up_to_id=-1;
					}
				}
				else{// no queue is set. clear list in case it was added for a mask layers that has no maskee on this frame
					collected_mask_objects.clear();
				}
			}
		}
		else{
			if(existing_child->mask_state==ANIM::mask_type::MASK){
				// this is a mask. add it to the queued masks. set the mask_up_to_id if it is not the same objID as the child itself
				collected_mask_objects.push_back(existing_child);
				// todo: we need to get the display command for this frame (or create if it doesnt exists)
				// set the mask id -1 on it

				if(existing_child->mask_until_objID!=existing_child->obj_id){
					mask_up_to_id=existing_child->mask_until_objID;
				}
			}
			else{
				if(mask_up_to_id>=0){// we have a active mask queue
					// get a updatecommand for child, or create if not exists
					// apply masking
					FrameCommandDisplayObject* child_command = final_frame->get_update_command_by_child(existing_child);
					if(child_command==NULL){
						child_command=new FrameCommandDisplayObject();
						child_command->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
						child_command->child = existing_child;
						final_frame->add_command(child_command);
					}
					// this is not a mask. 
					child_command->mask=ANIM::mask_type::MASKED;
					child_command->set_hasTargetMaskIDs(true);
					child_command->add_mask_childs(collected_mask_objects);

					if(mask_up_to_id==existing_child->obj_id){// this is the objectID where the masking should stop.
						collected_mask_objects.clear();
						mask_up_to_id=-1;
					}
				}
				else{// no queue is set. clear list in case it was added for a mask layers that has no maskee on this frame
					collected_mask_objects.clear();
				}
			}
		}
	}
	
	// take care of update commands for childs inserted for graphic clips
	for(Graphic_instance* graphic_clip : graphic_clips){
		
		for(TimelineChild_instance* gc_child:graphic_clip->graphic_childs){
			
			FrameCommandDisplayObject* child_command = final_frame->get_update_command_by_child(gc_child);
			if((child_command==NULL)&&(graphic_clip->current_command==NULL))
				continue;// no need for any update
			bool new_command=false;
			bool has_any_effect=false;
			if(child_command==NULL){
				new_command=true;
				child_command=new FrameCommandDisplayObject();
				child_command->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
				child_command->child = gc_child;
				child_command->mask=gc_child->mask_state;
			}
			if((child_command->get_hasDisplayMatrix())||(graphic_clip->graphic_command->get_hasDisplayMatrix())||((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasDisplayMatrix()))){
				if(!child_command->get_hasDisplayMatrix()){
					FrameCommandDisplayObject* matrix_command = get_matrix_command_by_child(gc_child);
					if(matrix_command!=NULL){
						child_command->set_display_matrix(matrix_command->get_display_matrix()->get());
						child_command->matrix_parents.clear();
						for (GEOM::MATRIX2x3* parent_mtx:matrix_command->matrix_parents){
							if(parent_mtx!=matrix_command->matrix_parents.back()){
								GEOM::MATRIX2x3* new_mtx = new GEOM::MATRIX2x3(parent_mtx->get());
								child_command->matrix_parents.push_back(new_mtx);
							}
						}
					}else{
						child_command->set_display_matrix(child_command->get_display_matrix()->get());
					}
				}
				GEOM::MATRIX2x3* parent_mtx = graphic_clip->graphic_command->get_display_matrix();
				if((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasDisplayMatrix()))
					parent_mtx=graphic_clip->current_command->get_display_matrix();
				child_command->matrix_parents.push_back(new GEOM::MATRIX2x3(parent_mtx->get()));
				has_any_effect=true;
			}
			if(((child_command!=NULL)&&(child_command->get_hasColorMatrix()))||(graphic_clip->graphic_command->get_hasColorMatrix())||((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasColorMatrix()))){
				if(!child_command->get_hasColorMatrix()){
					FrameCommandDisplayObject* color_command = get_color_transform_command_by_child(gc_child);
					if(color_command!=NULL){
						child_command->set_color_matrix(color_command->get_color_matrix()->get());
						child_command->color_transform_parents.clear();
						for (GEOM::ColorTransform* parent_mtx:color_command->color_transform_parents){
							if(parent_mtx!=color_command->color_transform_parents.back()){
								GEOM::ColorTransform* new_mtx = new GEOM::ColorTransform(parent_mtx->get());
								child_command->color_transform_parents.push_back(new_mtx);
							}
						}
					}else{
						child_command->set_color_matrix(child_command->get_color_matrix()->get());
					}
				}
				GEOM::ColorTransform* parent_color_trans = graphic_clip->graphic_command->get_color_matrix();
				if((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasColorMatrix()))
					parent_color_trans=graphic_clip->current_command->get_color_matrix();
				child_command->color_transform_parents.push_back(new GEOM::ColorTransform(parent_color_trans->get()));
				has_any_effect=true;
			}	
			if((graphic_clip->graphic_command->get_hasBlendModeChange())||((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasBlendModeChange()))){
				if((graphic_clip->graphic_command->get_hasBlendModeChange())){
					child_command->set_blendmode(graphic_clip->graphic_command->get_blendmode());
				}
				else if((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasBlendModeChange())){
					child_command->set_blendmode(graphic_clip->current_command->get_blendmode());
				}
				has_any_effect=true;
			}
			if((graphic_clip->graphic_command->get_hasVisiblitiyChange())||((graphic_clip->current_command!=NULL)&&(graphic_clip->current_command->get_hasVisiblitiyChange()))){
			
				has_any_effect=true;
			}

			if((has_any_effect)&&(new_command))
				final_frame->add_command(child_command);
			else if ((!has_any_effect)&&(new_command))
				delete child_command;
		}		
	}
	for(FrameCommandDisplayObject* display_cmd:final_frame->display_commands){
		if(display_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE){
			display_cmd->prev_obj = this->get_prev_cmd(display_cmd->child);
		}
		
	}
	
}

FrameCommandDisplayObject*
MovieClip::get_prev_cmd(TimelineChild_instance* child)
{
	if(this->frames.size()==1)
		return NULL;
	int keyframe_cnt=this->frames.size()-1;
	FrameCommandDisplayObject* fcm=NULL;
	while(keyframe_cnt--){
		fcm = this->frames[keyframe_cnt]->get_update_command_by_child(child);
		if(fcm!=NULL)
			return fcm;
	}
	return NULL;
}
	
FrameCommandDisplayObject*
MovieClip::get_matrix_command_by_child(TimelineChild_instance* child)
{
	int keyframe_cnt=this->frames.size();
	FrameCommandDisplayObject* fcm=NULL;
	while(keyframe_cnt--){
		fcm = this->frames[keyframe_cnt]->get_update_command_by_child(child);
		if((fcm!=NULL)&&(fcm->get_hasDisplayMatrix()))
			return fcm;
	}
	return NULL;
}
FrameCommandDisplayObject*
MovieClip::get_color_transform_command_by_child(TimelineChild_instance* child)
{
	int keyframe_cnt=this->frames.size();
	FrameCommandDisplayObject* fcm=NULL;
	while(keyframe_cnt--){
		fcm = this->frames[keyframe_cnt]->get_update_command_by_child(child);
		if((fcm!=NULL)&&(fcm->get_hasColorMatrix()))
			return fcm;
	}
	return NULL;
}

/** \brief Finalizes the timeline. This will recurivly call finalize on all timeline-childs
* After we recieved all Adobe-frameCommands, we play 1 time through the timeline, to set depthmanager, and merge graphic clips
*/
result 
MovieClip::finalize()
{
	if(this->is_finalized){
		return result::AWD_SUCCESS;
	}
	this->is_finalized=true;

	

	/*
	if(this->isButton){
		if(this->frames.size()<4){
			//error no 4th frame exists)
			int test=0;
		}
		else{
			TimelineFrame * hittestFrame = this->frames[3];
			std::vector<ANIM::FrameCommandDisplayObject*> new_frame_1_commands;
			TYPES::UINT32 deepest_child=0;
			// loop over first frame, and find the max depth for a children
			for(int i=0; i<3; i++){
				TimelineFrame * oneFrame = this->frames[i];
				for(ANIM::FrameCommandDisplayObject* one_cmd:oneFrame->display_commands){
					if(one_cmd->get_depth()>deepest_child){
						deepest_child=one_cmd->get_depth();
					}
				}
			}
			TimelineFrame * firstframe = this->frames[0];
			for(ANIM::FrameCommandDisplayObject* one_cmd:firstframe->display_commands){
				new_frame_1_commands.push_back(one_cmd);
			}
			deepest_child++;
			for(ANIM::FrameCommandDisplayObject* one_cmd:hittestFrame->display_commands){
				one_cmd->set_depth(deepest_child++);
				new_frame_1_commands.push_back(one_cmd);
			}
			firstframe->display_commands.clear();
			for(ANIM::FrameCommandDisplayObject* one_cmd:new_frame_1_commands){
				firstframe->display_commands.push_back(one_cmd);
			}
			hittestFrame->display_commands.clear();
		}
	}
	
	*/

	//	step1:
	//	first, remove all empty frames (no commands / no label / no framecode / not first frame (first frame can be empty))
	//	expand duration of previous frames, if a frame is removed 
	//	store valid frames into a new list, and clear the original timeline frames after iterating them

	std::vector<TimelineFrame*> newFramesList;
	TimelineFrame* lastTimeLineFrame=NULL;
	for (TimelineFrame * f : this->frames) 
	{
		// a frame is not empty, if it has commands, script, labels, or if it is the first frame
		if((!f->is_empty())||(lastTimeLineFrame==NULL)){
			newFramesList.push_back(f);
			lastTimeLineFrame=f;
		}
		else{
			lastTimeLineFrame->set_frame_duration(f->get_frame_duration()+lastTimeLineFrame->get_frame_duration());
			delete f;
		}
	}
	this->frames.clear();

	//	step2:
	//	We have a valid list of frames, but the considered now empty.
	//	we now loop over every frame (not keyframe), 
	//	and evaluate the commands to fill the depthmanager, and merge graphiclips
	//	at this step we connect commands to depth-objects on depth-layers inside the depth-manager.
	//	the depth-manager is used to convert to as2 depth.
	this->current_frame=1;
	graphic_clips.clear();
	for (TimelineFrame * f : newFramesList){
		
		int start_frame=this->current_frame;
		int frame_duration=f->get_frame_duration();
		advance_frame(f);
		this->current_frame++;
		while (this->current_frame<(start_frame+frame_duration))
		{
			advance_frame(NULL);
			this->current_frame++;
		}
	}
	
	// only needed for as2 depth. todo: implement the option
	depth_manager->apply_depth();
	
	//	this removes the graphic instance-commands. 
	//	for as2 it also orders the commands by depth.
	//	for as3 commands must not be reordered, otherwise they will no longer execute correctly
	for (TimelineFrame * f : this->frames) 
		f->build_final_commands();

	// remove the graphic instances from the potential child list
	std::vector<ANIM::PotentialTimelineChildGroup* > new_timeline_childs;
	for(ANIM::PotentialTimelineChildGroup* child_group: this->timeline_childs){
		bool export_this=true;
		if(child_group->awd_block->get_type()==BLOCK::block_type::MOVIECLIP){
			MovieClip* this_timeline=reinterpret_cast<MovieClip*>(child_group->awd_block);
			if(this_timeline->is_grafic_instance)
				export_this=false;
		}
		if(export_this){
			new_timeline_childs.push_back(child_group);
		}
	}
	this->timeline_childs.clear();
	for(ANIM::PotentialTimelineChildGroup* child_group: new_timeline_childs)
		this->timeline_childs.push_back(child_group);
	new_timeline_childs.clear();

	// create the final potential child lists
	// we have one list of potential-childs that will always be used by only 1 instance at the same time.
	// and another list of potential-childs that will be used by multiple instances at the same time.
	int child_id_cnt=0;
	for(ANIM::PotentialTimelineChildGroup* child_group: this->timeline_childs){
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
	
	/*
	// masking - get id for mask-childs should be done when streams are created
	for (TimelineFrame * f : this->frames) 
	{
		for(FrameCommandDisplayObject* fd: f->display_commands){
			if(fd->mask_child_levels.size()>0){
				if(fd->mask_child_levels.size()>1){
					fd->mask_child_levels[1000]=std::vector<TimelineChild_instance*> (); // make a error to check if this case will ever be in icycle
				}
				else{
					fd->mask_ids.clear();
					std::vector<TYPES::INT32> new_mask_ids;
					for(TimelineChild_instance* mask_child:fd->mask_childs){
						new_mask_ids.push_back(mask_child->child->id);
					}
					for(TYPES::INT32 m_id:new_mask_ids)
						fd->mask_ids.push_back(mask_child->child->id);
					new_mask_ids.clear();
				}
			}
		}
	}
	*/
	
	return result::AWD_SUCCESS;
}

/** \brief get the TimelineFrame for the given frame
*/
TimelineFrame* 
MovieClip::get_frame_at(TYPES::UINT32 current_frame)
{
	int start_frame=1;
	for (TimelineFrame * f : this->frames) 
	{
		if((current_frame>=start_frame)&&(current_frame<=(start_frame+f->get_frame_duration()-1)))
			return f;
		start_frame+=f->get_frame_duration();
	}
	return this->frames.back();
}

/** \brief Check if TimelineFrame exists that has startframe = given frame
*/
bool 
MovieClip::has_frame_at(TYPES::UINT32 frame)
{
	int f_cnt=1;
	for (TimelineFrame * f : this->frames) 
	{
		if(f_cnt==frame)
			return true;
		f_cnt++;
		f_cnt+=f->get_frame_duration()-1;
	}
	return false;
}




TYPES::UINT32
MovieClip::calc_body_length(AWDFile* awd_file, BlockSettings *curBlockSettings)
{
	
	TYPES::union_ptr properties_union;
	properties_union.v=malloc(sizeof(TYPES::F64));
	*properties_union.F64=this->fps;
	this->properties->set(1, properties_union, TYPES::UINT32(sizeof(TYPES::F32)));
	
	properties_union.v=malloc(sizeof(TYPES::UINT16));
	*properties_union.ui16=this->scene_id;
	this->properties->set(2, properties_union, TYPES::UINT32(sizeof(TYPES::UINT16)));

	properties_union.v=malloc(sizeof(TYPES::UINT8));
	*properties_union.ui8=1;
	this->properties->set(3, properties_union, TYPES::UINT32(sizeof(TYPES::UINT8)));


    TYPES::UINT32 len;

    len = sizeof(TYPES::UINT16) +  TYPES::UINT16(this->get_name().size());//name
	
   // len += sizeof(TYPES::UINT8);
    //len += sizeof(TYPES::UINT8);
    //len += sizeof(TYPES::F32);
	
    len += sizeof(TYPES::UINT16);// num of potential childs with 1 instance
    len += this->timeline_childs_one_instance.size()*sizeof(TYPES::UINT32);// num of potential childs
    len += sizeof(TYPES::UINT16);// num of potential childs with multiple instance
    len += this->timeline_childs_multiple_instances.size()*(sizeof(TYPES::UINT32)+sizeof(TYPES::UINT16));// num of potential childs
	
    len += sizeof(TYPES::UINT16);// num of potential sounds
    //len += 0*sizeof(TYPES::UINT32);// num of potential sounds
	
	
	len += sizeof(TYPES::UINT8);// stream_count
	for(std::vector<TYPES::UINT32> one_int_vec : all_int_vecs){
		len += sizeof(TYPES::UINT8);// stream data type			
		len += sizeof(TYPES::UINT32);
		TYPES::UINT8 storage_precision=1;
		for(TYPES::UINT32 one_int: one_int_vec){
			if(one_int > std::numeric_limits<unsigned short int>::max()){
				storage_precision=4;
				break;
			}
			if((storage_precision==1)&&(one_int > std::numeric_limits<unsigned char>::max())){
				storage_precision=2;
			}
		}
		len += one_int_vec.size() * storage_precision;
	}
	for(std::vector<TYPES::UINT32> one_int_vec : all_optional_int_vecs){
		if(one_int_vec.size()>0){
			len += sizeof(TYPES::UINT8);// stream_type 
			len += sizeof(TYPES::UINT8);// stream data type			
			len += sizeof(TYPES::UINT32);
			TYPES::UINT8 storage_precision=1;
			for(TYPES::UINT32 one_int: one_int_vec){
				if(one_int >= std::numeric_limits<unsigned short int>::max()){
					storage_precision=4;
					break;
				}
				if((storage_precision==1)&&(one_int >= std::numeric_limits<unsigned char>::max())){
					storage_precision=2;
				}
			}
			 len += one_int_vec.size() * storage_precision;
		}
	}
	
	len += sizeof(TYPES::UINT8);// stream_count
	for(std::vector<TYPES::F32> one_int_vec : all_optional_f32_vecs){
		if(one_int_vec.size()>0){
			len += sizeof(TYPES::UINT8);// stream_type 
			len += sizeof(TYPES::UINT32);
			len += one_int_vec.size() * sizeof(TYPES::F32);
		}
	}
	
	if(this->labels.size()>0){
		len += sizeof(TYPES::UINT8);// stream_type 
		len += sizeof(TYPES::UINT32);
		len += sizeof(TYPES::UINT16);
		for(std::string onestring : this->labels){
			len += sizeof(TYPES::UINT16) +  TYPES::UINT16(onestring.size()) + sizeof(TYPES::UINT16);//name
		}
	}
	
	if(this->properties_stream_strings.size()>0){
		len += sizeof(TYPES::UINT8);// stream_type 
		len += sizeof(TYPES::UINT32);
		len += sizeof(TYPES::UINT16);
		for(std::string onestring : this->properties_stream_strings){
			len += sizeof(TYPES::UINT16) +  TYPES::UINT16(onestring.size());//name
		}
	}
	
	if(curBlockSettings->get_bool(SETTINGS::bool_settings::ExportFrameScript)){
		if(this->frame_scripts_stream.size()>0){
			len += sizeof(TYPES::UINT8);// stream_type
			len += sizeof(TYPES::UINT32);
			len += sizeof(TYPES::UINT16);
			for(std::string onestring : this->frame_scripts_stream){
				len += sizeof(TYPES::UINT32) + TYPES::UINT32(onestring.size()) + sizeof(TYPES::UINT16);//name
			}
		}
	}
		
	
	
    len += this->calc_attr_length(true,true, curBlockSettings);
	
    return len;
}


result
MovieClip::get_frames_info(std::vector<std::string>& infos)
{
	
	int cnt = 0;
	for(std::vector<TYPES::UINT32> one_int_vec : all_int_vecs){
		std::string layer_info="	"+std::to_string(cnt)+"	'"+std::to_string(one_int_vec.size()); 
		cnt++;
		//infos.push_back(layer_info);
	}
	for(std::vector<TYPES::UINT32> one_int_vec : all_optional_int_vecs){
		if(one_int_vec.size()>0){
			std::string layer_info="	"+std::to_string(cnt)+"	'"+std::to_string(one_int_vec.size()); 
			//infos.push_back(layer_info);
		}
		cnt++;
	}
	
	for(std::vector<TYPES::F32> one_int_vec : all_optional_f32_vecs){
		if(one_int_vec.size()>0){
			std::string layer_info="	"+std::to_string(cnt)+"	'"+std::to_string(one_int_vec.size()); 
			//infos.push_back(layer_info);
		}
		cnt++;
	}
	
	if(this->labels.size()>0){
		std::string layer_info="	labels: "+std::to_string(this->labels.size()); 
		infos.push_back(layer_info);
	}
	
	if(this->properties_stream_strings.size()>0){
		std::string layer_info="	strings (names): "+std::to_string(this->properties_stream_strings.size()); 
		infos.push_back(layer_info);
	}
	
	if(this->frame_scripts_stream.size()>0){
		std::string layer_info="	scripts: "+std::to_string(this->frame_scripts_stream.size()); 
		infos.push_back(layer_info);
	}
		
	
	
	
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
	infos.push_back("Depth layers: "+std::to_string(this->depth_manager->depth_layers.size()));
	for(TimelineDepthLayer* depth_layer: this->depth_manager->depth_layers){
		std::string layer_info="	depth = "+std::to_string(depth_layer->depth); 
		for(TimelineChild_instance* depth_obj: depth_layer->depth_objs){
			std::string childID=" - ";
			if(depth_obj->child!=NULL)
				childID=std::to_string(depth_obj->child->id);
			layer_info+=" child_id: "+childID+" start: "+std::to_string(depth_obj->start_frame)+" end: "+std::to_string(depth_obj->end_frame)+" | ";
		}
		infos.push_back(layer_info);
	}	
	
	int keyframe_cnt=0;
	int fcnt1=0;
	bool keepgoing=true;
	while(keepgoing){
		keyframe_cnt++;
		if(this->frames[fcnt1]->startframe==keyframe_cnt){
			infos.push_back("Frame "+std::to_string(this->frames[fcnt1]->startframe)+" full construct = "+std::to_string(this->frames[fcnt1]->isFullConstruct));
			infos.push_back("Frame "+std::to_string(this->frames[fcnt1]->startframe)+" duration = "+std::to_string(this->frames[fcnt1]->get_frame_duration()));

			this->frames[fcnt1]->get_frame_info(infos);
			fcnt1++;
		}
		if(fcnt1>=this->frames.size())
			keepgoing=false;
		
	}
	
	return AWD::result::AWD_SUCCESS;
}

result 
MovieClip::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	// all timelines need to be finalized before collecting dependencies
	// we do not automaticcally call the finalize here, because it need to be called before collecting dependencies, 
	// reason for that is, that finalizing timelines will merge some timelines (graphic instances), so its easier to keep this in 2 sepperate steps 
	if(!this->is_finalized)
		return result::AWD_ERROR;
	
	std::vector<TimelineFrame*> final_frames;
	std::vector<TimelineFrame*> remove_frames;
	TimelineFrame* prev_frame=NULL;
	for (TimelineFrame * f : this->frames) 
	{
		f->finalize_commands();
		if((f->is_empty())&&(prev_frame!=NULL)){
			prev_frame->set_frame_duration(prev_frame->get_frame_duration()+f->get_frame_duration());
			this->remove_frames.push_back(f);
			continue;
		}
		prev_frame=f;
		final_frames.push_back(f);
	}
	this->frames.clear();
	for (TimelineFrame * f : final_frames)
		this->frames.push_back(f);
	final_frames.clear();
	
	// we have a list of dependencies that needs to appear in awd-file before this timeline
	for(PotentialTimelineChildGroup* child: this->timeline_childs)
		child->awd_block->add_with_dependencies(target_file, instance_type);

	this->create_timeline_streams();

	return result::AWD_SUCCESS;
}

result
MovieClip::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings *curBlockSettings, AWDFile* awd_file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);// name	
	
	// todo: export this as properties
	/*
	fileWriter->writeUINT8(this->is_scene);// is scene
	fileWriter->writeUINT8(this->scene_id);// sceneID //TODO
	fileWriter->writeFLOAT32(this->fps);// fps
	*/
	
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

	fileWriter->writeUINT16(TYPES::UINT16(0));
	/*
	for(PotentialTimelineChild* sound: this->timeline_sounds){
		TYPES::UINT32 block_addr=0;
		if (sound->awd_block != NULL)
			sound->awd_block->get_block_addr(awd_file, block_addr);
		fileWriter->writeUINT32(block_addr);
	}
	*/
	
	// new way (using streams)

	TYPES::UINT8 stream_cnt=0;
	for(std::vector<TYPES::UINT32> one_int_vec : all_int_vecs){
		stream_cnt++;		
	}
	for(std::vector<TYPES::UINT32> one_int_vec : all_optional_int_vecs){
		if(one_int_vec.size()>0){
			stream_cnt++;
		}
	}
	fileWriter->writeUINT8(stream_cnt);
	stream_cnt=0;
	for(std::vector<TYPES::UINT32> one_int_vec : all_int_vecs){
		fileWriter->writeUINTSasSmallestData(one_int_vec);
		stream_cnt++;
	}
	for(std::vector<TYPES::UINT32> one_int_vec : all_optional_int_vecs){
		if(one_int_vec.size()>0){
			fileWriter->writeUINT8(stream_cnt);
			fileWriter->writeUINTSasSmallestData(one_int_vec);
		}
		stream_cnt++;
	}

	stream_cnt=0;
	for(std::vector<TYPES::F32> one_int_vec : all_optional_f32_vecs){
		if(one_int_vec.size()>0){
			stream_cnt++;
		}
	}
	if(this->labels.size()>0){
		stream_cnt++;
	}
	if(this->properties_stream_strings.size()>0){
		stream_cnt++;
	}
	if(this->frame_scripts_stream.size()>0){
		stream_cnt++;
	}
	fileWriter->writeUINT8(stream_cnt);
	stream_cnt=0;
	for(std::vector<TYPES::F32> one_int_vec : all_optional_f32_vecs){
		if(one_int_vec.size()>0){
			fileWriter->writeUINT8(stream_cnt);
			fileWriter->writeUINT32(one_int_vec.size()*sizeof(TYPES::F32));
			fileWriter->writeFLOAT32multi(&one_int_vec[0], one_int_vec.size());
		}
		stream_cnt++;
	}
		
	
	if(this->labels.size()>0){
		fileWriter->writeUINT8(stream_cnt);
		TYPES::UINT32 labels_stream_length=sizeof(TYPES::UINT16);
		for(std::string onestring : this->labels){
			labels_stream_length+= sizeof(TYPES::UINT16) +  TYPES::UINT16(onestring.size()) + sizeof(TYPES::UINT16);
		}
		fileWriter->writeUINT32(labels_stream_length);
		fileWriter->writeUINT16(this->labels.size());
		int counter = 0;
		for(std::string onestring : this->labels){
			fileWriter->writeSTRING(onestring, FILES::write_string_with::LENGTH_AS_UINT16);
			fileWriter->writeUINT16(this->label_indices[counter++]);
		}
	}	
	stream_cnt++;
	
	if(this->properties_stream_strings.size()>0){
		fileWriter->writeUINT8(stream_cnt);
		TYPES::UINT32 propsstring_stream_length=sizeof(TYPES::UINT16);
		for(std::string onestring : this->properties_stream_strings){
			propsstring_stream_length+=sizeof(TYPES::UINT16) +  TYPES::UINT16(onestring.size());
		}
		fileWriter->writeUINT32(propsstring_stream_length);
		fileWriter->writeUINT16(this->properties_stream_strings.size());
		for(std::string onestring : this->properties_stream_strings){
			fileWriter->writeSTRING(onestring, FILES::write_string_with::LENGTH_AS_UINT16);
		}	
	}
	stream_cnt++;
	
	if(curBlockSettings->get_bool(SETTINGS::bool_settings::ExportFrameScript)){
		if(this->frame_scripts_stream.size()>0){
			fileWriter->writeUINT8(stream_cnt);
			TYPES::UINT32 script_stream_length=sizeof(TYPES::UINT16);
			for(std::string onestring : this->properties_stream_strings){
				script_stream_length+=sizeof(TYPES::UINT32) + TYPES::UINT32(onestring.size()) + sizeof(TYPES::UINT16);
			}
			fileWriter->writeUINT32(script_stream_length);
			fileWriter->writeUINT16(this->frame_scripts_stream.size());
			int counter = 0;
			for(std::string onestring : this->frame_scripts_stream){
				fileWriter->writeUINT16(this->frame_scripts_indices[counter++]);
				fileWriter->writeSTRING(onestring, FILES::write_string_with::LENGTH_AS_UINT32);
			}
		}
	}
	
	//	fileWriter->writeUINT32(0);
	//	fileWriter->writeUINT32(0);
	
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
	return result::AWD_SUCCESS;
}

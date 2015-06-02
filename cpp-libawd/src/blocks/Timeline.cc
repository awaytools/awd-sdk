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
	depth_manager = new TimelineDepthManager();
	this->is_collected=false;
	this->is_scene=false;
	this->fps=0.0;
	this->is_finalized=false;
	this->instance_cnt=0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	this->has_grafic_instances=false;
	this->is_grafic_instance=true;
	this->current_frame=1;
}
Timeline::Timeline():
	BASE::AWDBlock(BLOCK::block_type::TIMELINE),
    BASE::AttrElementBase()
{
	depth_manager = new TimelineDepthManager();
	this->is_collected=false;
	this->instance_cnt=0;
	this->is_scene=false;
	this->is_finalized=false;
	this->fps=0.0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	this->has_grafic_instances=false;
	this->is_grafic_instance=true;
	this->current_frame=1;
	
}
Timeline::~Timeline()
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

TYPES::state Timeline::validate_block_state()
{
	return TYPES::state::VALID;
}

////////////////////////

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

// used while recieving adobe frame commands:

void
Timeline::add_adobe_frame(TimelineFrame* newFrame)
{
	newFrame->startframe=this->current_frame;
	this->current_frame++;
	this->frames.push_back(newFrame);
}

FrameCommandDisplayObject*
Timeline::get_update_command_by_id(TYPES::UINT32 objectID)
{
	return this->frames.back()->get_update_command_by_id(objectID);
}

void
Timeline::remove_object_by_id(TYPES::UINT32 obj_id)
{
	this->frames.back()->remove_object_by_id(obj_id);
}

FrameCommandDisplayObject*
Timeline::add_display_object_by_id(TYPES::UINT32 objectID, TYPES::UINT32 add_after_id)
{
	
	AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)this->frames.back()->add_display_object_by_id(objectID, add_after_id);
	frameCommand->set_depth(add_after_id);
	return frameCommand;
}

/*
bool
Timeline::test_depth_ids(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID)
{
	return this->frames.back()->test_depth_ids(objectID, add_after_ID);
}
*/

// used while finalizing the commands:

/** \brief Add a new frame
*/
void
Timeline::add_frame(TimelineFrame* newFrame)
{
	this->frames.push_back(newFrame);
}

/** \brief Converts mask ids from adobe id type to Awayjs id type. 
* i think we should change the way masks are stored.
*/
void
Timeline::calc_mask_ids()
{
	for (TimelineFrame * f : this->frames) 
		f->calc_mask_ids();
}

/** \brief Gets a pointer to a PotentialTimelineChild for a AWDBlock.
* If all existing PotentialTimelineChild for this AWDBlock are already in use, a new one is created.
* Per AWDBlock, we store 1 PotentialTimelineChildGroups.
*/
PotentialTimelineChild* 
Timeline::get_child_for_block(BASE::AWDBlock* awd_block)
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


/** \brief Advance the playhead to the next frame.
* After we recieved all Adobe-frameCommands, we play through the timeline, to set depthmanager, and merge graphic clips
*/
void 
Timeline::advance_frame(TimelineFrame* frame)
{
	std::vector<int> error_vec;// assert doesnt work for flashpro in release mode. so i just try to access a element in this vec whenever a error should trow
	//	check if any of the alive graphic-instances provides a keyframe for the current frame.
	//	set all visible graphic-instances to dirty = true.
	bool has_graphic_keyframes=false;
	for(ANIM::Graphic_instance* graphic_clip : graphic_clips){
		Timeline* timeline = reinterpret_cast<Timeline*>(graphic_clip->graphic_timeline);
		if(timeline->has_frame_at(this->current_frame-graphic_clip->offset_frames)){
			has_graphic_keyframes=true;
		}
		graphic_clip->dirty_masks=false;
		graphic_clip->current_command=NULL;
	}

	//	no matter what, the depthmanager must advance 1 frame., this will set endFrame for all active childs to this->current_frame
	this->depth_manager->advance_frame(this->current_frame);

	// no keyframe is available for this frame (neither for timeline nor for any of the graphic instances that are alive)
	if((!has_graphic_keyframes)&&(frame==NULL)){
		// no need to add a frame, but we need to extent the last duration of last frame.
		this->frames.back()->set_frame_duration(this->frames.back()->get_frame_duration()+1);
		return;
	}
	TimelineFrame* final_frame;
	std::vector<FrameCommandDisplayObject*> input_display_commands;
	std::vector<FrameCommandRemoveObject*> input_remove_commands;
	// if we have a input frame, we will resuse it.
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
	BLOCKS::Timeline* child_timeline = NULL;
	// EVAL REMOVE COMMANDS FOR FRAME:
	for(FrameCommandRemoveObject* removeCmd: input_remove_commands){
		if(timeline_childs_to_obj_id.find(removeCmd->objID)==timeline_childs_to_obj_id.end())
			error_vec[0]=1;// ERROR BECAUSE OBJECT NOT FOUND.
		
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
		
		final_frame->apply_remove_command(removeCmd);
		this->depth_manager->apply_remove_command(removeCmd);
	}
	
	// EVAL REMOVE COMMANDS FOR ACTIV GRAPHIC CLIPS:
	if(has_graphic_keyframes){
		for(Graphic_instance* graphic_clip : graphic_clips){
			if(graphic_clip->graphic_timeline->has_frame_at(this->current_frame-graphic_clip->offset_frames)){
				clip_frame = graphic_clip->graphic_timeline->get_frame_at(this->current_frame-graphic_clip->offset_frames);
				if(clip_frame==NULL)
					error_vec[0]=1;// ERROR NO FRAME RETRIEVED
				// copy stuff over from the frame
				final_frame->set_frame_code(clip_frame->get_frame_code());
				for(FrameCommandRemoveObject* removeCmd2: clip_frame->remove_commands){
					removeCmd2->child->child->used=false;
					final_frame->apply_remove_command(removeCmd2);
					this->depth_manager->apply_remove_command(removeCmd2);
					graphic_clip->remove_child(removeCmd2->child);
				}
				clip_frame->remove_commands.clear();
			}
		}
	}
	
	// EVAL COMMANDS FOR FRAME:
	for(FrameCommandDisplayObject* display_cmd: input_display_commands){
		if(display_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
			current_block = display_cmd->get_object_block();// create a new child-instance for this object
			current_child = new TimelineChild_instance();
			current_child->start_frame=this->current_frame;
			current_child->end_frame=this->current_frame;
			if(display_cmd->get_hasTargetMaskIDs()){
				if(!((display_cmd->mask_ids.size()==1)&&(display_cmd->mask_ids[0]==-1)))
					current_child->is_masked=true;
				else
					current_child->is_mask=true;
			}

			current_child->child=this->get_child_for_block(current_block);
					
			timeline_childs_to_obj_id[display_cmd->get_objID()]=current_child;
				
			display_cmd->child=current_child;

			// check if the object is a timeline, and if so, make sure it is finalized (recursion), and check if it is a graphic instance 
			if(current_block->get_type()==BLOCK::block_type::TIMELINE){
				child_timeline=reinterpret_cast<BLOCKS::Timeline*>(current_block);
				// make sure that the timeline has been finalized itself
				child_timeline->finalize();
				
				if(child_timeline->is_grafic_instance){
					
					// this is a graphic instance.
					Graphic_instance* new_graphic_clip = new Graphic_instance();
					new_graphic_clip->graphic_timeline=child_timeline;
					new_graphic_clip->graphic_command=display_cmd;
					new_graphic_clip->offset_frames=current_frame-1;
					new_graphic_clip->graphic_command=display_cmd;
					new_graphic_clip->current_command=display_cmd;
					new_graphic_clip->graphic_child=current_child;
					current_child->graphic=new_graphic_clip;
					this->graphic_clips.push_back(new_graphic_clip);
					
				}
			}
			// get the parent object, using the obj-id
			parent_child = NULL;
			if(display_cmd->get_depth()!=0)
				parent_child=timeline_childs_to_obj_id[display_cmd->get_depth()];
			if(parent_child!=NULL){
				if(parent_child->graphic!=NULL)
					parent_child=this->depth_manager->get_parent_for_graphic_clip(parent_child);
			}
			// insert the object
			final_frame->apply_add_command(display_cmd, parent_child);
			this->depth_manager->add_child_after_child(current_child, parent_child);
		}
		else if(display_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE){
			if(timeline_childs_to_obj_id.find(display_cmd->get_objID())==timeline_childs_to_obj_id.end())
				error_vec[0]=1;// ERROR NO OBJECT RETIREVED
			current_child = timeline_childs_to_obj_id[display_cmd->get_objID()];
			display_cmd->child=current_child;
			final_frame->apply_update_command(display_cmd);
			
			if(current_child->graphic!=NULL){
				current_child->graphic->current_command=display_cmd;
				if(display_cmd->get_hasDisplayMatrix())
					current_child->graphic->graphic_command->set_display_matrix(display_cmd->get_display_matrix()->get());
				if(display_cmd->get_hasColorMatrix())
					current_child->graphic->graphic_command->set_color_matrix(display_cmd->get_color_matrix()->get());
				if(display_cmd->get_hasTargetMaskIDs())
					current_child->graphic->graphic_command->mask_ids=display_cmd->mask_ids;
			}
		}
	}


	
	// EVAL COMMANDS FOR ACTIV GRAPHIC CLIPS:
	for(Graphic_instance* graphic_clip : graphic_clips){
		if(graphic_clip->graphic_timeline->has_frame_at(this->current_frame - graphic_clip->offset_frames)){
			TimelineFrame* clip_frame = graphic_clip->graphic_timeline->get_frame_at(this->current_frame - graphic_clip->offset_frames);
			if(clip_frame==NULL)
				error_vec[0]=1;// ERROR NO FRAME
			for(FrameCommandDisplayObject* dp_cmd: clip_frame->display_commands){
				if(dp_cmd->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD){
					dp_cmd->child->child=this->get_child_for_block(dp_cmd->get_object_block());
					dp_cmd->child->parent_grafic=graphic_clip->graphic_child;
					dp_cmd->child->start_frame=this->current_frame;
					dp_cmd->child->end_frame=this->current_frame;
					
					if(graphic_clip->graphic_child->is_mask){
						dp_cmd->hasTargetMaskIDs=true;
						dp_cmd->mask_ids.clear();
						dp_cmd->mask_ids.push_back(-1);
					}
					graphic_clip->dirty_masks=true;
					parent_child=graphic_clip->graphic_child;
					if(dp_cmd->child->parent_child!=NULL)
						parent_child=dp_cmd->child->parent_child;
					final_frame->apply_add_command(dp_cmd, parent_child);
					this->depth_manager->add_child_after_child(dp_cmd->child, parent_child);
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

	std::vector<TimelineChild_instance*> existing_childs;
	depth_manager->get_children_at_frame(this->current_frame, existing_childs);
	for(TimelineChild_instance* existing_child:existing_childs){
		if(existing_child->graphic!=NULL){
			// this object is a graphic instance. we only need to take care of its childs.
			continue;
		}
		if(existing_child->parent_grafic==NULL){
			// this is a normal object. we only need to take care of masking
			// if the object is a mask, it will already have the "-1" assigned.
			if(existing_child->is_masked){
				bool mask_dirty=false;
				// get the last active command for this child, that is affecting the masks
				FrameCommandDisplayObject* mask_command = get_mask_command_for_child(existing_child);
				// we need to get all childs for the mask-id. if the child is a graphic instance, we get all its childs instead.
				std::vector<TimelineChild_instance*> new_mask_childs;
				for(int m_id : mask_command->mask_ids){
					if(timeline_childs_to_obj_id.find(m_id)==timeline_childs_to_obj_id.end())
						_ASSERT(0);
					TimelineChild_instance* mask_child = timeline_childs_to_obj_id[m_id];
					if(mask_child->graphic!=NULL){
						if(mask_child->graphic->dirty_masks)
							mask_dirty=true;
						for(TimelineChild_instance* gc_mask_child:mask_child->graphic->graphic_childs)
							new_mask_childs.push_back(gc_mask_child);
					}
					else
						new_mask_childs.push_back(mask_child);
				}
				FrameCommandDisplayObject* child_command = final_frame->get_update_command_by_child(existing_child);
				if(((child_command!=NULL)&&(child_command->get_hasTargetMaskIDs()))||(mask_dirty)){
					if(child_command==NULL){
						child_command=new FrameCommandDisplayObject();
						child_command->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
						child_command->child = existing_child;
						final_frame->add_command(child_command);
					}
					child_command->hasTargetMaskIDs=true;
					child_command->mask_childs.clear();
					for(TimelineChild_instance* m_child : new_mask_childs)
						child_command->mask_childs.push_back(m_child);
				}
				new_mask_childs.clear();
			}
			continue;
		}
		// if we make it here, child is a child of a graphic clip. skip it (will be processed in next step).
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
				// prepend the parent transform. seem to work
				child_command->color_transform_parents.push_back(new GEOM::ColorTransform(parent_color_trans->get()));
				has_any_effect=true;
			}	
			if((has_any_effect)&&(new_command))
				final_frame->add_command(child_command);
			else if ((!has_any_effect)&&(new_command))
				delete child_command;
			//TODO: take care of blendmodes and visiblility here
		}

		
		// check the masking for masked graphic clips.
		if(graphic_clip->graphic_child->is_masked){
			bool mask_dirty=false;
			// we need to get all childs for the mask-id. if the child is a graphic instance, we get all its childs instead.
			std::vector<TimelineChild_instance*> new_mask_childs;
			for(int m_id : graphic_clip->graphic_command->mask_ids){
				if(timeline_childs_to_obj_id.find(m_id)==timeline_childs_to_obj_id.end())
					_ASSERT(0);
				TimelineChild_instance* mask_child = timeline_childs_to_obj_id[m_id];
				if(mask_child->graphic!=NULL){
					if(mask_child->graphic->dirty_masks)
						mask_dirty=true;
					for(TimelineChild_instance* gc_mask_child:mask_child->graphic->graphic_childs)
						new_mask_childs.push_back(gc_mask_child);
				}
				else
					new_mask_childs.push_back(mask_child);
			}
			if((graphic_clip->dirty_masks)||(mask_dirty)){
				for(TimelineChild_instance* gc_child:graphic_clip->graphic_childs){
					FrameCommandDisplayObject* child_command = final_frame->get_update_command_by_child(gc_child);
					bool apply_masks=true;
					bool new_command=false;
					if(child_command==NULL){
						new_command=true;
						child_command=new FrameCommandDisplayObject();
						child_command->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
						child_command->child = gc_child;
					}
					if(!(child_command->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)){
						apply_masks=false;
						FrameCommandDisplayObject* mask_command = get_mask_command_for_child(gc_child);
						if(mask_command==NULL)
							apply_masks=true;
						else{
							// todo: compare the 2 lists of mask-childs to check if the masks need update
							apply_masks=true;
						}
					}
					if(apply_masks){
						if(new_command)
							final_frame->add_command(child_command);
						child_command->hasTargetMaskIDs=true;
						child_command->mask_childs.clear();
						for(TimelineChild_instance* m_child : new_mask_childs)
							child_command->mask_childs.push_back(m_child);
					}
					else{
						if(new_command)
							delete child_command;
					}
				}
			}
			new_mask_childs.clear();
		}
		;
		for(FrameCommandDisplayObject* display_cmd:final_frame->display_commands){
			if(display_cmd->child->graphic==NULL)
				display_cmd->prev_obj = this->get_prev_cmd(display_cmd->child);
		}
	}
	
}

FrameCommandDisplayObject*
Timeline::get_prev_cmd(TimelineChild_instance* child)
{
	if(this->frames.size()==1)
		return NULL;
	int frame_cnt=this->frames.size()-1;
	FrameCommandDisplayObject* fcm=NULL;
	while(frame_cnt--){
		fcm = this->frames[frame_cnt]->get_update_command_by_child(child);
		if(fcm!=NULL)
			return fcm;
	}
	return NULL;
}
	
FrameCommandDisplayObject*
Timeline::get_matrix_command_by_child(TimelineChild_instance* child)
{
	int frame_cnt=this->frames.size();
	FrameCommandDisplayObject* fcm=NULL;
	while(frame_cnt--){
		fcm = this->frames[frame_cnt]->get_update_command_by_child(child);
		if((fcm!=NULL)&&(fcm->get_hasDisplayMatrix()))
			return fcm;
	}
	return NULL;
}
FrameCommandDisplayObject*
Timeline::get_color_transform_command_by_child(TimelineChild_instance* child)
{
	int frame_cnt=this->frames.size();
	FrameCommandDisplayObject* fcm=NULL;
	while(frame_cnt--){
		fcm = this->frames[frame_cnt]->get_update_command_by_child(child);
		if((fcm!=NULL)&&(fcm->get_hasColorMatrix()))
			return fcm;
	}
	return NULL;
}
FrameCommandDisplayObject*
Timeline::get_mask_command_for_child(TimelineChild_instance* child)
{
	int frame_cnt=this->frames.size();
	FrameCommandDisplayObject* fcm=NULL;
	while(frame_cnt--){
		fcm = this->frames[frame_cnt]->get_update_command_by_child(child);
		if((fcm!=NULL)&&(fcm->get_hasTargetMaskIDs()))
			return fcm;
	}
	return NULL;
}
/** \brief Finalizes the timeline. This will recurivly call finalize on all timeline-childs
* After we recieved all Adobe-frameCommands, we play 1 time through the timeline, to set depthmanager, and merge graphic clips
*/
result 
Timeline::finalize()
{
	if(this->is_finalized){
		return result::AWD_SUCCESS;
	}
	this->is_finalized=true;

	// first, remove all empty frames (no commands / no label / no framecode)
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
	//clear out the frames, the valid frames are now stored in "newFramesList"
	this->frames.clear();

	// convert mask-property, so that every masked object points to the objects it should be masked with.	
	for (TimelineFrame * f : newFramesList) 
		f->calc_mask_ids();
	
	// rebuild the timeline.
	// while doing so, we merge timelines from graphic instances into the timeline.
	// also, at this step we connect commands to depth-objects on depth-layers inside the depth-manager.
	// the depth-manager is used to convert to as2 depth.
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
		bool export=true;
		if(child_group->awd_block->get_type()==BLOCK::block_type::TIMELINE){
			Timeline* this_timeline=reinterpret_cast<Timeline*>(child_group->awd_block);
			if(this_timeline->is_grafic_instance)
				export=false;
		}
		if(export){
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
	
	// masking - get id for mask-childs
	for (TimelineFrame * f : this->frames) 
	{
		for(FrameCommandDisplayObject* fd: f->display_commands){
			if(fd->get_hasTargetMaskIDs()){
				if((fd->mask_ids.size()==1)&&(fd->mask_ids[0]==-1)){
				}
				else{
					std::vector<TYPES::INT32> new_mask_ids;
					for(TimelineChild_instance* mask_child:fd->mask_childs){
						new_mask_ids.push_back(mask_child->child->id);
					}
					fd->mask_ids.clear();
					for(TYPES::INT32 m_id:new_mask_ids)
						fd->mask_ids.push_back(m_id);
					new_mask_ids.clear();
				}
			}
		}
	}
	
	return result::AWD_SUCCESS;
}

/** \brief get the TimelineFrame for the given frame
*/
TimelineFrame* 
Timeline::get_frame_at(TYPES::UINT32 current_frame)
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
Timeline::has_frame_at(TYPES::UINT32 frame)
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
    len += 0*sizeof(TYPES::UINT32);// num of potential sounds

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
	
	int frame_cnt=0;
	int fcnt1=0;
	bool keepgoing=true;
	while(keepgoing){
		frame_cnt++;
		if(this->frames[fcnt1]->startframe==frame_cnt){
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
Timeline::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
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

	fileWriter->writeUINT16(TYPES::UINT16(0));
	/*
	for(PotentialTimelineChild* sound: this->timeline_sounds){
		TYPES::UINT32 block_addr=0;
		if (sound->awd_block != NULL)
			sound->awd_block->get_block_addr(awd_file, block_addr);
		fileWriter->writeUINT32(block_addr);
	}
	*/
	fileWriter->writeUINT16(TYPES::UINT16(this->frames.size()));// num of frames	
	for (TimelineFrame * f : this->frames) 
	{
		f->write_frame(fileWriter, curBlockSettings, awd_file);
	}
	
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
	return result::AWD_SUCCESS;
}

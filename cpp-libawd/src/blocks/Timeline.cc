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
	{
		delete f;
	}
	
	for(PotentialTimelineChildGroup* child_group: this->timeline_childs){		
		for(PotentialTimelineChild* child: child_group->childs)
			delete child;
		delete child_group;
	}
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
	if(this->adobe_frames.size()>0){
		for(FrameCommandBase* existingCMD : this->adobe_frames.back()->commands){
			FrameCommandDisplayObject* newFrameCommand = new FrameCommandDisplayObject();
			newFrameCommand->set_objID(existingCMD->get_objID());
			newFrameCommand->set_depth(existingCMD->get_depth());
			newFrameCommand->child=existingCMD->child;
			newFrameCommand->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
			newFrame->add_command(newFrameCommand);
		}
	}
	newFrame->startframe=this->current_frame;
	this->adobe_frames.push_back(newFrame);
	this->current_frame++;
}

FrameCommandBase*
Timeline::get_update_command_by_id(TYPES::UINT32 objectID)
{
	return this->adobe_frames.back()->get_update_command_by_id(objectID);
}

void
Timeline::remove_object_by_id(TYPES::UINT32 obj_id)
{
	this->adobe_frames.back()->remove_object_by_id(obj_id);
}

FrameCommandDisplayObject*
Timeline::add_display_object_by_id(TYPES::UINT32 objectID, TYPES::UINT32 add_after_id)
{
	
	AWD::ANIM::FrameCommandDisplayObject* frameCommand=(AWD::ANIM::FrameCommandDisplayObject*)this->adobe_frames.back()->add_display_object_by_id(objectID, add_after_id);
	frameCommand->set_depth(add_after_id);
	frameCommand->does_something;
	return frameCommand;
}

bool
Timeline::test_depth_ids(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID)
{
	return this->adobe_frames.back()->test_depth_ids(objectID, add_after_ID);
}


// used while finalizing the commands:

/** \brief Add a new frame
*/
void
Timeline::add_frame(TimelineFrame* newFrame)
{
	
	if(this->frames.size()>0){
		for(FrameCommandBase* existingCMD : this->frames.back()->commands){
			FrameCommandDisplayObject* existingCMDDP = reinterpret_cast<FrameCommandDisplayObject*> (existingCMD);
			FrameCommandDisplayObject* newFrameCommand = new FrameCommandDisplayObject();
			newFrameCommand->set_objID(existingCMD->get_objID());
			newFrameCommand->set_depth(existingCMD->get_depth());
			newFrameCommand->child=existingCMD->child;
			newFrameCommand->bkp_matrix->set(existingCMDDP->bkp_matrix->get());
			newFrameCommand->bkp_color_matrix->set(existingCMDDP->bkp_color_matrix->get());
			newFrameCommand->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
			newFrame->add_command(newFrameCommand);
		}
	}
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
	for(TimelineChild_instance* graphic_clip : graphic_clips){
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
	// if we havnt returned, we need to create a keyframe for this frame
	// create a new keyframe
	TimelineFrame* final_frame = new TimelineFrame();
	
	final_frame->set_frame_duration(1);				//	duration will be extendet if empty frames follow
	final_frame->startframe=this->current_frame;	//	keep track of startframe for debug purpose

	// add the new frame to the timeline. this will create empty update commands for all objects that exists on previous frame
	this->add_frame(final_frame);
	
	TimelineChild_instance* current_child=NULL;
	TimelineFrame* clip_frame=NULL;
	std::vector<FrameCommandRemoveObject*> new_remove_cmds;
	std::vector<TimelineChild_instance* > new_graphic_clips;
	TimelineChild_instance* parent_child = NULL;
	AWDBlock* current_block = NULL;
	BLOCKS::Timeline* child_timeline = NULL;
	// EVAL REMOVE COMMANDS FOR FRAME:
	if(frame){
		// copy stuff over from the frame
		final_frame->set_frame_code(frame->get_frame_code());
		for(std::string onelabel:frame->get_labels())
			final_frame->add_label(ANIM::frame_label_type::AWD_FRAME_LABEL_NAME, onelabel);

		// apply remove commands
		for(FrameCommandRemoveObject* removeCmd: frame->remove_commands){
			if(timeline_childs_to_obj_id.find(removeCmd->get_objID())==timeline_childs_to_obj_id.end())
				error_vec[0]=1;// ERROR BECAUSE OBJECT NOT FOUND.
			
			current_child = timeline_childs_to_obj_id[removeCmd->get_objID()];
			current_child->child->used=false;
			removeCmd->child=current_child;
			if(current_child->graphic_instance){
				// Add Remove_commands for all the childs that are present on the stage, and was added for this graphic clip
				new_remove_cmds.clear();
				for(FrameCommandDisplayObject* gc_cmd:final_frame->commands){
					if(gc_cmd->child->graphic_clip_origin==current_child){
						FrameCommandRemoveObject* new_gc_remove_command=new FrameCommandRemoveObject();
						new_gc_remove_command->child=gc_cmd->child;
						gc_cmd->child->child->used=false;
						new_remove_cmds.push_back(new_gc_remove_command);
					}
				}
				for(FrameCommandRemoveObject* gc_cmd:new_remove_cmds){
					final_frame->apply_remove_command(gc_cmd);
					this->depth_manager->remove_child(gc_cmd->child);
				}
				new_remove_cmds.clear();

				// remove the graphic clip from the graphic-clip-list
				if(graphic_clips.size()>1){
					new_graphic_clips.clear();
					for(TimelineChild_instance* old_graphic_clip : this->graphic_clips){
						if(old_graphic_clip!=current_child)
							new_graphic_clips.push_back(old_graphic_clip);
					}
					this->graphic_clips.clear();
					for(TimelineChild_instance* old_graphic_clip : new_graphic_clips)
						this->graphic_clips.push_back(old_graphic_clip);
					new_graphic_clips.clear();
				}
				else{
					graphic_clips.clear();
				}

			}
			final_frame->apply_remove_command(removeCmd);
			this->depth_manager->remove_child(current_child);
		}
	}
	// EVAL REMOVE COMMANDS FOR ACTIV GRAPHIC CLIPS:
	if(has_graphic_keyframes){
		for(TimelineChild_instance* graphic_clip : graphic_clips){
			if(graphic_clip->graphic_timeline->has_frame_at(this->current_frame-graphic_clip->offset_frames)){
				clip_frame = graphic_clip->graphic_timeline->get_frame_at(this->current_frame-graphic_clip->offset_frames);
				if(clip_frame==NULL)
					error_vec[0]=1;// ERROR NO FRAME RETRIEVED
				// copy stuff over from the frame
				final_frame->set_frame_code(clip_frame->get_frame_code());
				for(FrameCommandRemoveObject* removeCmd2: clip_frame->remove_commands){
					removeCmd2->child->child->used=false;
					final_frame->apply_remove_command(removeCmd2);
					this->depth_manager->remove_child(removeCmd2->child);
				}
			}
		}
	}
	// EVAL COMMANDS FOR FRAME:
	if(frame){
		// apply the add commands to the new frame.
		for(FrameCommandDisplayObject* display_cmd: frame->commands){
			if(display_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
				current_block = display_cmd->get_object_block();// create a new child-instance for this object
				current_child = new TimelineChild_instance();
				current_child->obj_id=display_cmd->get_objID();
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
						current_child->graphic_timeline=child_timeline;
						current_child->graphic_instance=true;
						current_child->graphic_command=display_cmd;
						current_child->offset_frames=current_frame-1;
						current_child->graphic_command=display_cmd;
						current_child->current_command=display_cmd;
						this->graphic_clips.push_back(current_child);
					}
				}
				// get the parent object, using the obj-id
				parent_child = NULL;
				if(display_cmd->get_depth()!=0)
					parent_child=timeline_childs_to_obj_id[display_cmd->get_depth()];
				if(parent_child!=NULL){
					if(parent_child->graphic_instance)
						parent_child=this->depth_manager->get_parent_for_graphic_clip(parent_child);
				}

				// insert the object
				final_frame->apply_add_command(display_cmd, parent_child);
				this->depth_manager->add_child_after_child(current_child, parent_child);

			}
			else if((display_cmd->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE)&&(display_cmd->does_something)){
				if(timeline_childs_to_obj_id.find(display_cmd->get_objID())==timeline_childs_to_obj_id.end())
					error_vec[0]=1;// ERROR NO OBJECT RETIREVED
				current_child = timeline_childs_to_obj_id[display_cmd->get_objID()];
				display_cmd->child=current_child;
				final_frame->apply_update_command(display_cmd);
				if(current_child->graphic_instance){
					current_child->current_command=display_cmd;
					if(display_cmd->get_hasDisplayMatrix())
						current_child->graphic_command->set_display_matrix(display_cmd->get_display_matrix()->get());
					if(display_cmd->get_hasColorMatrix())
						current_child->graphic_command->set_color_matrix(display_cmd->get_color_matrix()->get());
					if(display_cmd->get_hasTargetMaskIDs())
						current_child->graphic_command->mask_ids=display_cmd->mask_ids;
				}
			}
		}
	}

	// EVAL COMMANDS FOR ACTIV GRAPHIC CLIPS:
	for(TimelineChild_instance* graphic_clip : graphic_clips){
		if(graphic_clip->graphic_timeline->has_frame_at(this->current_frame - graphic_clip->offset_frames)){
			TimelineFrame* clip_frame = graphic_clip->graphic_timeline->get_frame_at(this->current_frame - graphic_clip->offset_frames);
			if(clip_frame==NULL)
				error_vec[0]=1;// ERROR NO FRAME
			for(FrameCommandBase* base_cmd: clip_frame->final_commands){
				if(base_cmd->get_command_type()==frame_command_type::FRAME_COMMAND_REMOVE)
					continue;
				FrameCommandDisplayObject* dp_cmd=reinterpret_cast<FrameCommandDisplayObject*>(base_cmd);
				if(dp_cmd->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD){
					dp_cmd->child->child=this->get_child_for_block(dp_cmd->get_object_block());
					dp_cmd->child->graphic_clip_origin=graphic_clip;
					dp_cmd->child->start_frame=this->current_frame;
					dp_cmd->child->end_frame=this->current_frame;
					
					if(graphic_clip->is_mask){
						dp_cmd->hasTargetMaskIDs=true;
						dp_cmd->mask_ids.clear();
						dp_cmd->mask_ids.push_back(-1);
					}
					graphic_clip->dirty_masks=true;
					parent_child=graphic_clip;
					if(dp_cmd->child->parent_child!=NULL)
						parent_child=dp_cmd->child->parent_child;
					final_frame->apply_add_command(dp_cmd, parent_child);
					this->depth_manager->add_child_after_child(dp_cmd->child, parent_child);
				}
				else
					final_frame->apply_update_command(dp_cmd);
			}
		}
		// collect all childs of this graphic instance. needed for masking
		graphic_clip->graphic_childs.clear();
		for(FrameCommandDisplayObject* display_cmd:final_frame->commands){
			if(display_cmd->child->graphic_clip_origin==graphic_clip)
				graphic_clip->graphic_childs.push_back(display_cmd->child);
		}

	}

	TimelineChild_instance* graphic_clip=NULL;
	FrameCommandDisplayObject* parent_command=NULL;

	// we need to apply the updated properties onto all objects that are present for this frame.

	for(FrameCommandDisplayObject* display_cmd:final_frame->commands){

		if(display_cmd->child->graphic_instance){
			// this command will be removed later. no need to dio anything
			continue;
		}
		if(display_cmd->child->graphic_clip_origin==NULL){
			// this is a normal object. we just need to take care of masking
			// if the object is a mask, it will already have the "-1" assigned.			
			if(display_cmd->child->is_masked){
				
				bool mask_dirty=false;
				// we need to get all childs for the mask-id. if the child is a graphic instance, we get all its childs instead.
				std::vector<TimelineChild_instance*> new_mask_childs;
				for(int m_id : display_cmd->mask_ids){
					if(timeline_childs_to_obj_id.find(m_id)==timeline_childs_to_obj_id.end())
						_ASSERT(0);
					TimelineChild_instance* mask_child = timeline_childs_to_obj_id[m_id];
					if(mask_child->graphic_instance){
						if(mask_child->dirty_masks)
							mask_dirty=true;
						for(TimelineChild_instance* gc_mask_child:mask_child->graphic_childs)
							new_mask_childs.push_back(gc_mask_child);
					}
					else
						new_mask_childs.push_back(mask_child);
				}
				if((display_cmd->get_hasTargetMaskIDs())||(mask_dirty)){
					display_cmd->hasTargetMaskIDs=true;
					display_cmd->mask_childs.clear();
					for(TimelineChild_instance* m_child : new_mask_childs)
						display_cmd->mask_childs.push_back(m_child);
				}
				new_mask_childs.clear();
				
			}
			continue;
		}
		graphic_clip = display_cmd->child->graphic_clip_origin;
		parent_command = graphic_clip->current_command;

		// if this command does nothing, and we have no parent command for this frame, we dont need to do anything.
		if((!display_cmd->does_something)&&(parent_command==NULL))
			continue;

		if((display_cmd->get_hasDisplayMatrix())||(graphic_clip->graphic_command->get_hasDisplayMatrix())||((parent_command!=NULL)&&(parent_command->get_hasDisplayMatrix()))){
			GEOM::MATRIX2x3* parent_mtx = graphic_clip->graphic_command->get_display_matrix();
			if((parent_command!=NULL)&&(parent_command->get_hasDisplayMatrix()))
				parent_mtx=parent_command->get_display_matrix();
			// reset the display matrix to its original state
			display_cmd->set_display_matrix(display_cmd->bkp_matrix->get());
			// multiply the matrix. append works for 99%, prepend for 5% ...
			display_cmd->get_display_matrix()->append(parent_mtx);
		}
		if((display_cmd->get_hasColorMatrix())||(graphic_clip->graphic_command->get_hasColorMatrix())||((parent_command!=NULL)&&(parent_command->get_hasColorMatrix()))){
			GEOM::ColorTransform* parent_color_trans = graphic_clip->graphic_command->get_color_matrix();
			if((parent_command!=NULL)&&(parent_command->get_hasColorMatrix()))
				parent_color_trans=parent_command->get_color_matrix();
			// reset the color transform to its original state
			display_cmd->set_color_matrix(display_cmd->bkp_color_matrix->get());
			// prepend the parent transform. seem to work
			display_cmd->get_color_matrix()->prepend(parent_color_trans);
		}
	}
	
	// take care of graphic clip masking
	for(TimelineChild_instance* graphic_clip : graphic_clips){
		// we only need to do this for graphic clips that had any childs added.
		if(graphic_clip->is_masked){
			bool mask_dirty=false;
			// we need to get all childs for the mask-id. if the child is a graphic instance, we get all its childs instead.
			std::vector<TimelineChild_instance*> new_mask_childs;
			for(int m_id : graphic_clip->graphic_command->mask_ids){
				if(timeline_childs_to_obj_id.find(m_id)==timeline_childs_to_obj_id.end())
					_ASSERT(0);
				TimelineChild_instance* mask_child = timeline_childs_to_obj_id[m_id];
				if(mask_child->graphic_instance){
					if(mask_child->dirty_masks)
						mask_dirty=true;
					for(TimelineChild_instance* gc_mask_child:mask_child->graphic_childs)
						new_mask_childs.push_back(gc_mask_child);
				}
				else
					new_mask_childs.push_back(mask_child);
			}
			if((graphic_clip->dirty_masks)||(mask_dirty)){
				for(FrameCommandDisplayObject* display_cmd:final_frame->commands){
					// todo: check if this update is needed (are same masks already assigned to objects)
					if(display_cmd->child->graphic_clip_origin==graphic_clip){
						display_cmd->hasTargetMaskIDs=true;
						display_cmd->mask_childs.clear();
						for(TimelineChild_instance* m_child : new_mask_childs)
							display_cmd->mask_childs.push_back(m_child);
					}
				}
			}
			new_mask_childs.clear();
		}
	}
	
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
	for (TimelineFrame * f : this->adobe_frames) 
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
	this->adobe_frames.clear();
	for (TimelineFrame * f : newFramesList) 
		this->adobe_frames.push_back(f);

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
		advance_frame(f);
		this->current_frame++;
		while (this->current_frame<(start_frame+f->get_frame_duration()))
		{
			advance_frame(NULL);
			this->current_frame++;
		}
	}
	
	depth_manager->apply_depth();

	for (TimelineFrame * f : this->frames) 
		f->build_final_commands();
		/*
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
		*/
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

	// after we have set the ids on the child-insatnces, we now can convert mask-ids from adobe-obj-id to child-id
	for (TimelineFrame * f : this->frames) 
	{
		for(FrameCommandDisplayObject* fd: f->commands){
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
	// create a list of PotentialTimelineChild that are used by this timeline.
	// each PotentialTimelineChild stores a reference to a awd-block
	//	this->create_timeline_childs();

	// finalizes the commands, so that we only store needed properties
	// also takes care of resetting object-props when neccessary (when resuing objects)
	//for (TimelineFrame * f : this->frames) 
	//	f->finalize_object_states();
	
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
	int fcnt2=0;
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
		/*
		if(this->adobe_frames[fcnt2]->startframe==frame_cnt){
			infos.push_back("Adobe Frame "+std::to_string(this->adobe_frames[fcnt2]->startframe)+" duration = "+std::to_string(this->adobe_frames[fcnt2]->get_frame_duration()));
			this->adobe_frames[fcnt2]->get_frame_info(infos);
			infos.push_back("	remove:");
			for (FrameCommandBase * f : this->adobe_frames[fcnt2]->remove_commands) 
			{
				std::string this_string;
				f->get_command_info(this_string);
				infos.push_back("		"+this_string);
			}
			infos.push_back("	commands:");
			for (FrameCommandBase * f : this->adobe_frames[fcnt2]->commands) 
			{
				FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(f);
				if(fd->does_something){
					std::string this_string;
					f->get_command_info(this_string);
					infos.push_back("		"+this_string);
				}
			}
			fcnt2++;
		}
		if(fcnt2>=this->adobe_frames.size())
			keepgoing=false;
			*/
	}
	return AWD::result::AWD_SUCCESS;
}

result 
Timeline::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	// all timelines need to be finalized before collecting dependencies
	// we do not automaticcally call the finalize here, because it need to be called before collecting dependencies, 
	// reason for that is, that finalizing timelines will merge some timelines (graphic instances), so its easier to keep this in 2 sepperate steps 
	if(!this->is_finalized){
		return result::AWD_ERROR;
	}
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

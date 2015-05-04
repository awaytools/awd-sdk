#include "elements/timeline_frame.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
 

TimelineFrame::TimelineFrame() :
    AttrElementBase()
{
	this->max_depth=0;
	this->frame_duration=0;
    this->command_properties=new ATTR::NumAttrList();
}

TimelineFrame::~TimelineFrame()
{
    delete this->command_properties;
	for (FrameCommandBase* fc :  this->commands){
		//delete fc;
	}
}


TYPES::UINT32
TimelineFrame::get_frame_duration()
{
	return this->frame_duration;
}

void
TimelineFrame::set_frame_duration(TYPES::UINT32 frame_duration)
{
	this->frame_duration=frame_duration;
}

std::vector<std::string>&
TimelineFrame::get_labels()
{
	return this->labels;
}
void
TimelineFrame::add_label(frame_label_type label_type, const std::string& label)
{
	this->label_types.push_back(label_type);
	this->labels.push_back(label);
}

std::vector<FrameCommandBase*>
TimelineFrame::get_inActivecommands()
{
	return this->inActivecommands;
}
std::vector<FrameCommandBase*>
TimelineFrame::get_commands()
{
	return this->commands;
}
void
TimelineFrame::set_frame_code(const std::string& frame_code)
{
	this->frame_code=this->frame_code+frame_code;
}

std::string&
TimelineFrame::get_frame_code()
{
    return this->frame_code;
}
void
TimelineFrame::add_command(FrameCommandBase* newFrame)
{
	this->commands.push_back(newFrame);
}
void
TimelineFrame::add_inActivecommand(FrameCommandBase* newFrame)
{
	this->inActivecommands.push_back(newFrame);
}

FrameCommandBase*
TimelineFrame::get_command(TYPES::UINT32 objectID, frame_command_type command_type)
{
	for (FrameCommandBase* fc :  this->commands){
		if(fc->get_objID()==objectID){
			if((command_type==frame_command_type::FRAME_COMMAND_REMOVE)&&(fc->get_command_type()==frame_command_type::FRAME_COMMAND_REMOVE))
				return fc;
			if((command_type==frame_command_type::FRAME_COMMAND_UPDATE)&&((fc->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)||(fc->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)))
				return fc;
			fc->set_command_type(command_type);
			return fc;
		}
	}
	if((command_type==frame_command_type::FRAME_COMMAND_UPDATE)||(command_type==frame_command_type::FRAME_COMMAND_ADD_CHILD)){
		FrameCommandDisplayObject* newFrameCommand = new FrameCommandDisplayObject();
		newFrameCommand->set_objID(objectID);
		newFrameCommand->set_command_type(command_type);
		this->commands.push_back(newFrameCommand);
		return newFrameCommand;
	}
	else if(command_type==frame_command_type::FRAME_COMMAND_REMOVE){
		FrameCommandRemoveObject* newFrameCommand = new FrameCommandRemoveObject();
		newFrameCommand->set_objID(objectID);
		newFrameCommand->set_command_type(command_type);
		this->commands.push_back(newFrameCommand);
		return newFrameCommand;
	}
	return NULL;
}

TimelineChild_instance*
TimelineFrame::get_display_object_by_id(TYPES::UINT32 obj_id)
{
	for(ANIM::TimelineChild_instance* tlchild_inst : this->display_objects){
		if(tlchild_inst->obj_id==obj_id){
			return tlchild_inst;
		}
	}
	return NULL;
}
TimelineChild_instance* 
TimelineFrame::remove_display_object_by_id(TYPES::UINT32 obj_id)
{
	std::vector<ANIM::TimelineChild_instance*> new_display_objects;
	this->max_depth = 0;
	TimelineChild_instance* child_id = NULL;
	for(ANIM::TimelineChild_instance* tlchild_inst : this->display_objects){
		if(tlchild_inst->obj_id!=obj_id){
			new_display_objects.push_back(tlchild_inst);
			tlchild_inst->depth=this->max_depth;
			this->max_depth+=3;
		}
		else{
			child_id=tlchild_inst;
			tlchild_inst->child->used=false;
			if(tlchild_inst->depth_obj!=NULL){
				tlchild_inst->depth_obj->in_use=false;
			}
		}
	}
	if(this->max_depth>=3)
		this->max_depth-=3;
	this->display_objects.clear();
	this->display_objects=new_display_objects;
	return child_id;
}
void
TimelineFrame::add_display_object_after_id(TYPES::UINT32 obj_id, ANIM::TimelineChild_instance* new_display_object)
{
	std::vector<ANIM::TimelineChild_instance*> objects_before_id;
	std::vector<ANIM::TimelineChild_instance*> objects_after_id;
	if(obj_id==0){
		for(ANIM::TimelineChild_instance* tlchild_inst:this->display_objects){
			objects_after_id.push_back(tlchild_inst);
		}
	}
	else{
		bool found=false;
		for(ANIM::TimelineChild_instance* tlchild_inst:this->display_objects){
			if(!found)
				objects_before_id.push_back(tlchild_inst);
			else{
				objects_after_id.push_back(tlchild_inst);
			}
			if(tlchild_inst->obj_id==obj_id)
				found=true;
		}
		if(!found){
			int error=0; // this is a error. obj_id was not found on frame
		}
	}
	this->display_objects.clear();
	this->max_depth = 0;
	for(ANIM::TimelineChild_instance* tlchild_inst:objects_before_id){
		this->display_objects.push_back(tlchild_inst);
	}
	this->display_objects.push_back(new_display_object);
	for(ANIM::TimelineChild_instance* tlchild_inst:objects_after_id){
		this->display_objects.push_back(tlchild_inst);
	}
}
void 
TimelineFrame::copy_display_objects(std::vector<ANIM::TimelineChild_instance*>& objects_to_copy)
{
	this->display_objects.clear();
	for(ANIM::TimelineChild_instance* tlchild_inst:objects_to_copy){
		TimelineChild_instance* timelinechild_inst = new TimelineChild_instance();
		timelinechild_inst->child=tlchild_inst->child;
		timelinechild_inst->depth=tlchild_inst->depth;
		timelinechild_inst->depth_obj=tlchild_inst->depth_obj;
		timelinechild_inst->obj_id=tlchild_inst->obj_id;
		timelinechild_inst->new_obj=false;
		this->display_objects.push_back(timelinechild_inst);
	}
}
	
result
TimelineFrame::get_frame_info(std::vector<std::string>& infos)
{	
	//infos.push_back("input  commands:");
	/*for (FrameCommandBase * f : this->commands) 
	{
		std::string this_string;
		f->get_command_info(this_string);
		infos.push_back("		"+this_string);
	}*/
	infos.push_back("	commands:");
	for (FrameCommandBase * f : this->final_commands) 
	{
		std::string this_string;
		f->get_command_info(this_string);
		infos.push_back("		"+this_string);
	}
	return AWD::result::AWD_SUCCESS;
}

void
TimelineFrame::calculate_command_depths()
{
	//
}
/*
used only when generating frames from adobe frame generator. prepares the commands, so they are exactly same as when generating own commands
*/
void
TimelineFrame::calc_mask_ids()
{

	// todo: this works, but its a bit confusing and not simple to read. better redo this function.

	for (int i=0;i< this->commands.size();i++){
		if((this->commands[i]->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD)||(this->commands[i]->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE)){
			std::vector<FrameCommandDisplayObject*> fcommands;
			std::vector<TYPES::INT32> mask_ids;
			FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(this->commands[i]);
			if(fd!=NULL){
				// each command only needs to be processed once.
				if(!fd->calculated_mask){
					if(fd->get_hasTargetMaskIDs()){
						int this_id=fd->get_objID();
						int mask_id=fd->get_clipDepth();
						fd->calculated_mask=true;
						FrameCommandDisplayObject* fd2 = fd;
						while ((this_id==mask_id)&&(i<this->commands.size()-1)){
							i++;
							fcommands.push_back(fd2);
							mask_ids.push_back(this_id);
							if((this->commands[i]->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD)||(this->commands[i]->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE)){
								fd2 = reinterpret_cast<FrameCommandDisplayObject*>(this->commands[i]);
								if(fd2!=NULL){
									if(fd2->get_hasTargetMaskIDs()){
										this_id=fd2->get_objID();
										mask_id=fd2->get_clipDepth();
										fd2->calculated_mask=true;
									}
								}
							}
						}
						if(this_id!=mask_id){
							mask_ids.push_back(this_id);
							fcommands.push_back(fd2);
						}
						for (FrameCommandDisplayObject * fd3 : fcommands) 
						{
							fd3->mask_ids.clear();
							fd3->mask_ids.push_back(TYPES::INT32(-1));
							fd3->set_hasTargetMaskIDs(true);
						}

						for (FrameCommandBase * f2 : this->commands) 
						{
							if((f2->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD)||(f2->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE)){
								
								FrameCommandDisplayObject* fd2 = reinterpret_cast<FrameCommandDisplayObject*>(f2);
								if(fd2!=NULL){
									int this_id2=fd2->get_objID();
									if((this_id2>this_id)&&(this_id2<=mask_id)){
										fd2->mask_ids=mask_ids;
										fd2->set_hasTargetMaskIDs(true);
										fd2->calculated_mask=true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

FrameCommandBase*
TimelineFrame::get_command_for_obj_id(TYPES::UINT32 obj_id)
{
	for (FrameCommandBase * f : this->commands) 
	{
		if(f->get_objID()==obj_id){
			return f;
		}
	}
	return NULL;

}

void
TimelineFrame::finalize_object_states()
{
	std::vector<FrameCommandBase*> final_commands;
	for (FrameCommandBase * f : this->final_commands) 
	{
		if((f->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD)||(f->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE)){
			
			FrameCommandDisplayObject* fd = reinterpret_cast<FrameCommandDisplayObject*>(f);
			if(fd->update_from_prev()>0)
				final_commands.push_back(f);
		}
		else{
			final_commands.push_back(f);
		}
	}

}

TYPES::UINT32
TimelineFrame::calc_frame_length(BlockSettings * blockSettings)
{
	/* reset mask ids for adobe commands
	*/
    int len;
    len = sizeof(TYPES::UINT32); // frame duration
	len += sizeof(TYPES::UINT8);//labels-cnt
	for (std::string s : this->labels) 
	{
		len += sizeof(TYPES::UINT16) + s.size();
	}
	
	len += sizeof(TYPES::UINT16);
	for (FrameCommandBase* f : this->final_commands){
		len+=f->calc_command_length(blockSettings);
	}

	
	if(!blockSettings->get_export_framescripts())
		this->frame_code="";
	
    len += sizeof(TYPES::UINT32) +  TYPES::UINT16(this->frame_code.size()); 

    len += this->calc_attr_length(false,true, blockSettings); 
    
	return len;
}

void
TimelineFrame::clear_commands()
{
	this->commands.clear();
}
void
TimelineFrame::write_frame(FILES::FileWriter * fileWriter, BlockSettings * blockSettings, AWDFile* awd_file)
{
	fileWriter->writeUINT32(this->frame_duration);// frameduration (whole frames / default = 1)
	fileWriter->writeUINT8(TYPES::UINT8(this->labels.size()));
	for (std::string s : this->labels) 
	{
		fileWriter->writeSTRING(s,  FILES::write_string_with::LENGTH_AS_UINT16);
	}
	fileWriter->writeUINT16(TYPES::UINT16(this->final_commands.size()));// num of commands	
	
	for (FrameCommandBase* f : this->final_commands) 
	{
		f->write_command(fileWriter, blockSettings, awd_file);
	}

	fileWriter->writeSTRING(this->get_frame_code(),  FILES::write_string_with::LENGTH_AS_UINT32);// frame code	
	this->user_attributes->write_attributes(fileWriter, blockSettings);
}

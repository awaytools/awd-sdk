#include "elements/timeline_frame.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"

#include "Stdafx.h"


using namespace IceCore;
#define SORTER	RadixSort

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
 

TimelineFrame::TimelineFrame() 
{
	this->startframe=0;
	this->frame_duration=1;
}

TimelineFrame::~TimelineFrame()
{
	for (FrameCommandDisplayObject* fc :  this->display_commands)
		delete fc;
	for (FrameCommandDisplayObject* fc_empty :  this->empty_commands)
		delete fc_empty;
	for (FrameCommandRemoveObject* fc_remove :  this->remove_commands)
		delete fc_remove;
}


bool
TimelineFrame::is_empty()
{
	if(this->remove_commands.size()>0)
		return false;
	if(this->frame_code!="")
		return false;
	if(this->labels.size()>0)
		return false;
	if(this->display_commands.size()>0)
		return false;
	return true;
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

/*
// used while recieving adobe frame commands:
bool
TimelineFrame::test_depth_ids(TYPES::UINT32 objectID, TYPES::UINT32 add_after_ID)
{
	FrameCommandDisplayObject* prev_fc=NULL;
	for (FrameCommandDisplayObject* fc :  this->commands){
		if(fc->get_objID()==objectID){
			if((prev_fc==NULL)&&(add_after_ID==0))
				return true;
			else if((prev_fc==NULL)&&(add_after_ID!=0))
				return false;
			else if((prev_fc!=NULL)&&(prev_fc->get_objID()==add_after_ID))
				return true;
			else if((prev_fc!=NULL)&&(prev_fc->get_objID()!=add_after_ID))
				return false;
		}
		prev_fc=fc;
	}
	_ASSERT(0);
	return false;
}
*/

void
TimelineFrame::add_command(FrameCommandDisplayObject* newFrame)
{
	this->display_commands.push_back(newFrame);
}
void
TimelineFrame::remove_object_by_id(TYPES::UINT32 objectID)
{
	/*
	int removedIdx = -1;
	int idx_cnt = 0;
	std::vector<FrameCommandDisplayObject*> old_commands;
	bool removed_object=false;
	for (FrameCommandDisplayObject* fc :  this->commands){
		if(fc->get_objID()==objectID){
			removedIdx=idx_cnt;
			delete fc;
		}
		else{
			old_commands.push_back(fc);
		}
		idx_cnt++;
	}	
	this->commands=old_commands;
	if(removedIdx<0){
		// no object was found that could be removed. must be a sound
		idx_cnt=0;
		std::vector<FrameCommandBase*> old_sound_commands;
		for (FrameCommandBase* fc_sound :  this->sound_commands){
			if(fc_sound->get_objID()==objectID){
			removedIdx=idx_cnt;
				delete fc_sound;
			}
			else{
				old_sound_commands.push_back(fc_sound);
			}
			idx_cnt++;
		}
		this->sound_commands=old_sound_commands;
		if(removedIdx<0){
			// no sound was found neither
			_ASSERT(0);//this is a error !
		}
		else{
			FrameCommandRemoveObject* newFrameCommand = new FrameCommandRemoveObject();
			newFrameCommand->set_objID(objectID);
			newFrameCommand->remove_at_index=removedIdx;
			newFrameCommand->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_REMOVE);
			this->remove_sounds_commands.push_back(newFrameCommand);
		}
	}
	else{
	*/
		FrameCommandRemoveObject* newFrameCommand = new FrameCommandRemoveObject();
		newFrameCommand->objID=objectID;
		//newFrameCommand->remove_at_index=removedIdx;
		this->remove_commands.push_back(newFrameCommand);
	//}
}

FrameCommandDisplayObject*
TimelineFrame::add_display_object_by_id(TYPES::UINT32 objectID, TYPES::UINT32 add_after_id)
{
	/*
	for (FrameCommandBase* fc :  this->commands){
		if(fc->get_objID()==objectID){
			_ASSERT(0);
		}
	}
	*/
	FrameCommandDisplayObject* newFrameCommand = new FrameCommandDisplayObject();
	newFrameCommand->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD);
	newFrameCommand->set_objID(objectID);
	newFrameCommand->set_depth(add_after_id);
	/*
	std::vector<FrameCommandDisplayObject*> new_display_commands;
	if(add_after_id==0){
		newFrameCommand->add_at_index=0;
		new_display_commands.push_back(newFrameCommand);
		for (FrameCommandDisplayObject* fc :  this->commands){
			new_display_commands.push_back(fc);
		}
	}
	else{
		int index_cnt=0;
		for (FrameCommandDisplayObject* fc :  this->commands){
			new_display_commands.push_back(fc);
			if(fc->get_objID()==add_after_id){
				newFrameCommand->add_at_index=index_cnt;
				new_display_commands.push_back(newFrameCommand);
			}
			index_cnt++;
		}
		if(new_display_commands.size()==this->commands.size()){
			_ASSERT(0);
		}
	}
	this->commands=new_display_commands;
	*/
	this->display_commands.push_back(newFrameCommand);
	return newFrameCommand;
}
FrameCommandDisplayObject*
TimelineFrame::get_update_command_by_id(TYPES::UINT32 objectID)
{
	for (FrameCommandDisplayObject* fc :  this->display_commands){
		if(fc->get_objID()==objectID){
			return fc;
		}
	}
	FrameCommandDisplayObject* new_cmd=new FrameCommandDisplayObject();
	new_cmd->set_command_type(frame_command_type::FRAME_COMMAND_UPDATE);
	new_cmd->set_objID(objectID);
	this->display_commands.push_back(new_cmd);
	return new_cmd;
}



// used while finalizing the commands:
void
TimelineFrame::apply_add_command(FrameCommandDisplayObject* add_cmd, TimelineChild_instance* parent_child)
{
	/*
	for (FrameCommandBase* fc :  this->display_commands){
		if(fc->child==add_cmd->child){
			_ASSERT(0);
		}
	}
	
	std::vector<FrameCommandDisplayObject*> new_display_commands;
	if(parent_child==NULL){
		add_cmd->add_at_index=0;
		new_display_commands.push_back(add_cmd);
		for (FrameCommandDisplayObject* fc :  this->display_commands){
			new_display_commands.push_back(fc);
		}
	}
	else{
		int index_cnt=0;
		for (FrameCommandDisplayObject* fc :  this->display_commands){
			new_display_commands.push_back(fc);
			if(fc->child==parent_child){
				add_cmd->add_at_index=index_cnt;
				new_display_commands.push_back(add_cmd);
			}
			index_cnt++;
		}
		if(new_display_commands.size()==this->display_commands.size()){
			_ASSERT(0);//new_display_commands.push_back(add_cmd);
		}
	}
	this->display_commands.clear();
	for (FrameCommandDisplayObject* fc : new_display_commands)
		this->display_commands.push_back(fc);
	new_display_commands.clear();
	*/
	this->display_commands.push_back(add_cmd);
		
}
FrameCommandDisplayObject*
TimelineFrame::get_update_command_by_child(TimelineChild_instance* child)
{
	for (FrameCommandDisplayObject* fc :  this->display_commands){
		if(fc->child==child){
			return fc;
		}
	}
	return NULL;
}
void
TimelineFrame::apply_update_command(FrameCommandDisplayObject* update_cmd)
{
	/*
	int cnt_cmd=0;
	for (FrameCommandBase* fc :  this->display_commands){
		if(fc->child==update_cmd->child){
			this->display_commands[cnt_cmd]=update_cmd;
			return;
		}
		cnt_cmd++;
	}
	*/
	this->display_commands.push_back(update_cmd);
	//_ASSERT(0); //error because no command was not found
	
}
void
TimelineFrame::apply_remove_command(FrameCommandRemoveObject* remove_cmd)
{
	/*
	int idx_cnt = 0;
	std::vector<FrameCommandDisplayObject*> old_commands;
	bool removed_object=false;
	for (FrameCommandDisplayObject* fc :  this->display_commands){
		if(fc->child==remove_cmd->child){
			remove_cmd->depth=idx_cnt;
			delete fc;
		}
		else{
			old_commands.push_back(fc);
		}
		idx_cnt++;
	}
	if(old_commands.size()==this->display_commands.size())
		_ASSERT(0); // error object not found
	this->display_commands.clear();
	for (FrameCommandDisplayObject* fc :  old_commands)
		this->display_commands.push_back(fc);
		*/
	this->remove_commands.push_back(remove_cmd);
}

void
TimelineFrame::finalize_commands()
{
	std::vector<FrameCommandDisplayObject*> new_commands;
	for (FrameCommandDisplayObject * f : this->display_commands) 
	{
		f->resolve_parenting();
		f->finalize_command();
		if(f->has_active_properties())
			new_commands.push_back(f);
		else
			this->empty_commands.push_back(f);
	}
	this->display_commands.clear();
	for (FrameCommandDisplayObject * f : new_commands) 
		this->display_commands.push_back(f);
	new_commands.clear();

}
void
TimelineFrame::build_final_commands()
{
	// filter the graphic instance commands out of the remove-commands list
	std::vector<FrameCommandRemoveObject*> all_remove;
	for(FrameCommandRemoveObject* removeCmd: this->remove_commands){
		if(removeCmd->child->graphic==NULL){
			all_remove.push_back(removeCmd);
		}
	}
	this->remove_commands.clear();
	for(FrameCommandRemoveObject* removeCmd: all_remove)
		this->remove_commands.push_back(removeCmd);
	all_remove.clear();	
	
	//	filter the graphic instance commands out of the commands list
	std::vector<FrameCommandDisplayObject*> new_cmds;
	for (FrameCommandDisplayObject * f : this->display_commands) 
	{
		if(f->child->graphic==NULL){
			new_cmds.push_back(f);
		}
		else{
			delete f;
		}
	}
	// fill the commands into the final commands, sorted by depth
	int cmd_cnt=0;
	udword* InputValues_depth = new udword[ new_cmds.size()];
	for(ANIM::FrameCommandDisplayObject* f_cmd:new_cmds){
		InputValues_depth[cmd_cnt++]=(f_cmd->child->depth);
	}
	SORTER RS;
	const udword* Sorted = RS.Sort(InputValues_depth, new_cmds.size(), RADIX_SIGNED).GetRanks();
	int sorted_cnt=new_cmds.size();
	this->display_commands.clear();
	TimelineChild_instance* parent=NULL;
	while(sorted_cnt--){
		ANIM::FrameCommandDisplayObject* f = new_cmds[Sorted[sorted_cnt]];
		if(f->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
			f->child->parent_child=parent;
		}
		parent=f->child;
		this->display_commands.push_back(f);
	}
	DELETEARRAY(InputValues_depth);
	//DELETEARRAY(Sorted);
}
void
TimelineFrame::calc_mask_ids()
{
	for (int i=0;i< this->display_commands.size();i++){
		std::vector<TYPES::INT32> mask_ids;
		FrameCommandDisplayObject* fd = this->display_commands[i];
		if(!fd->calculated_mask){
			fd->calculated_mask=true;
			if(fd->get_hasTargetMaskIDs()){
				// this is a mask object
				int this_id=fd->get_objID();
				int mask_id=fd->get_clipDepth();
				mask_ids.push_back(this_id);
				fd->mask_ids.clear();
				fd->mask_ids.push_back(TYPES::INT32(-1));
				fd->set_hasTargetMaskIDs(true);
				// while loop only enters if next object will be a mask too
				while ((this_id==mask_id)&&(i<this->display_commands.size()-1)){
					i++;
					FrameCommandDisplayObject* fd2 = this->display_commands[i];
					if(fd2->get_hasTargetMaskIDs()){
						this_id=fd2->get_objID();
						mask_id=fd2->get_clipDepth();
						mask_ids.push_back(this_id);
						fd2->calculated_mask=true;
						fd2->mask_ids.clear();
						fd2->mask_ids.push_back(TYPES::INT32(-1));
						fd2->set_hasTargetMaskIDs(true);
					}
				}
				for (FrameCommandDisplayObject * f2 : this->display_commands) 
				{
					int this_id2=f2->get_objID();
					if((this_id2>this_id)&&(this_id2<=mask_id)){
						f2->mask_ids=mask_ids;
						f2->set_hasTargetMaskIDs(true);
						f2->calculated_mask=true;
					}
				}
			}
		}
	}
}




	
result
TimelineFrame::get_frame_info(std::vector<std::string>& infos)
{	
	infos.push_back("	commands:");
	if(this->remove_commands.size()>0){
		std::string remove_infos = "		remove objects at depth: ";
		for (FrameCommandRemoveObject * f_remove : this->remove_commands) 
			remove_infos+=std::to_string(f_remove->child->depth)+"|";
		infos.push_back(remove_infos);
	}
	for (FrameCommandDisplayObject * f : this->display_commands) 
	{
		std::string this_string;
		f->get_command_info(this_string);
		infos.push_back("		"+this_string);
	}
	return AWD::result::AWD_SUCCESS;
}


TYPES::UINT32
TimelineFrame::calc_frame_length(BlockSettings * blockSettings)
{
	/* reset mask ids for adobe commands
	*/
    int len=0;
    len = sizeof(TYPES::UINT32); // frame duration
	len += sizeof(TYPES::UINT8);//labels-cnt
	for (std::string s : this->labels) 
	{
		len += sizeof(TYPES::UINT16) + s.size();
	}
	
	len += sizeof(TYPES::UINT16);
	if(this->remove_commands.size()>0){
		len+=sizeof(TYPES::UINT8);//cmdID;
		len+=sizeof(TYPES::UINT16);//number remove objs;
		len+=this->remove_commands.size()*sizeof(TYPES::UINT16);//number remove objs;
	}
	for (FrameCommandDisplayObject* f : this->display_commands){
		len+=f->calc_command_length(blockSettings);
	}

	
	if(!blockSettings->get_export_framescripts())
		this->frame_code="";
	
    len += sizeof(TYPES::UINT32) +  TYPES::UINT32(this->frame_code.size()); 

    len += sizeof(TYPES::UINT32); //properties
    
	return len;
}

void
TimelineFrame::clear_commands()
{
	this->display_commands.clear();
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
	TYPES::UINT16 num_cmd = TYPES::UINT16(this->display_commands.size());
	if(this->remove_commands.size()>0)
		num_cmd++;
	fileWriter->writeUINT16(TYPES::UINT16(num_cmd));// num of commands	
	
	if(this->remove_commands.size()>0){
		fileWriter->writeUINT8(TYPES::UINT8(frame_command_type::FRAME_COMMAND_REMOVE));// num of commands	
		fileWriter->writeUINT16(TYPES::UINT16(this->remove_commands.size()));
		for (FrameCommandRemoveObject * f_remove : this->remove_commands) 
			fileWriter->writeINT16(TYPES::UINT16(f_remove->child->depth));
	}
	for (FrameCommandDisplayObject* f : this->display_commands) 
	{
		f->write_command(fileWriter, blockSettings, awd_file);
	}

	fileWriter->writeSTRING(this->get_frame_code(),  FILES::write_string_with::LENGTH_AS_UINT32);// frame code	
	fileWriter->writeUINT32(TYPES::UINT32(0));
}

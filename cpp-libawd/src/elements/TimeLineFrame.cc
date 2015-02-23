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

void
TimelineFrame::add_dirty_layer(int newDirtyLayer)
{
	for(int i : this->dirty_layer_idx){
		if(i==newDirtyLayer){
			return;
		}
	}
	this->dirty_layer_idx.push_back(newDirtyLayer);
}

std::vector<int>
TimelineFrame::get_dirty_layers()
{
	return this->dirty_layer_idx;
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
			if((command_type==frame_command_type::AWD_FRAME_COMMAND_UPDATE)&&((fc->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE)||(fc->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)))
				return fc;
			fc->set_command_type(command_type);
			return fc;
		}
	}
	
	FrameCommandDisplayObject* newFrameCommand=  new FrameCommandDisplayObject();
	newFrameCommand->set_objID(objectID);
	newFrameCommand->set_command_type(command_type);
	this->commands.push_back(newFrameCommand);
	return newFrameCommand;
}

result 
TimelineFrame::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	// reorder the commands, so they appear in this order: remove / add / update / sound
	std::vector<FrameCommandBase *> remove_commands;
	std::vector<FrameCommandBase *> add_commands;
	std::vector<FrameCommandBase *> update_commands;
	std::vector<FrameCommandBase *> sound_commands;
	for (FrameCommandBase * f : this->commands) 
	{
		if(f->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_REMOVE_OBJECT)
			remove_commands.push_back(f);
		else if((f->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)||(f->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE))
			add_commands.push_back(f);
		else if(f->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_UPDATE)
			update_commands.push_back(f);
		else if(f->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_SOUND)
			sound_commands.push_back(f);
	}
	this->commands.clear();
	for (FrameCommandBase * f : remove_commands) 
		this->commands.push_back(f);
	for (FrameCommandBase * f : add_commands) 
		this->commands.push_back(f);
	for (FrameCommandBase * f : update_commands) 
		this->commands.push_back(f);
	for (FrameCommandBase * f : sound_commands) 
		this->commands.push_back(f);

	for (FrameCommandBase * f : this->commands) 
	{
		f->collect_dependencies(target_file, instance_type);
	}
	return result::AWD_SUCCESS;
	
}
	
result
TimelineFrame::get_frame_info(std::vector<std::string>& infos)
{	
	for (FrameCommandBase * f : this->commands) 
	{
		std::string this_string;
		f->get_command_info(this_string);
		infos.push_back("	"+this_string);
	}
	return AWD::result::AWD_SUCCESS;
}

TYPES::UINT32
TimelineFrame::calc_frame_length(BlockSettings * blockSettings)
{
    int len;
    len = sizeof(TYPES::UINT32); // frame duration
	len += sizeof(TYPES::UINT16);//labels-cnt
	for (std::string s : this->labels) 
	{
		len += sizeof(TYPES::UINT8);
		len += sizeof(TYPES::UINT16) + s.size();
	}

	len += sizeof(TYPES::UINT16);// number of frame commands

	for (FrameCommandBase * f : this->commands) 
	{
		len+=f->calc_command_length(blockSettings);
	}
	
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
	fileWriter->writeUINT16(TYPES::UINT16(this->labels.size()));
	int labelCnt=0;
	for (std::string s : this->labels) 
	{
		TYPES::UINT8 labelType=TYPES::UINT8(this->label_types[labelCnt]);
		fileWriter->writeUINT8(labelType);
		labelCnt++;
		fileWriter->writeSTRING(s,  FILES::write_string_with::LENGTH_AS_UINT16);
	}
	fileWriter->writeUINT16(TYPES::UINT16(this->commands.size()));// num of commands	

	// reverse the order in which commands are written to file
	// at the moment this is done, so the as3 runtime works correctly.
	// for js runtime, this should not be needed
	bool reverseCommandOrder=false;

	if(reverseCommandOrder){
		int cmdCnt = int(this->commands.size());
		while(cmdCnt>0){
			cmdCnt--;
			FrameCommandBase * f = this->commands[cmdCnt]; 
			f->write_command(fileWriter, blockSettings, awd_file);
		}
	}
	else{
		for(int cmdCnt=0; cmdCnt<this->commands.size(); cmdCnt++){
			FrameCommandBase * f = this->commands[cmdCnt]; 
			f->write_command(fileWriter, blockSettings, awd_file);
		}
	}
	fileWriter->writeSTRING(this->get_frame_code(),  FILES::write_string_with::LENGTH_AS_UINT32);// frame code	
	this->user_attributes->write_attributes(fileWriter, blockSettings);
}

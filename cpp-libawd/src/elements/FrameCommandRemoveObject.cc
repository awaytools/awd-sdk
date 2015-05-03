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
 
FrameCommandRemoveObject::FrameCommandRemoveObject() :
FrameCommandBase()
{ 
	this->set_command_type(frame_command_type::FRAME_COMMAND_REMOVE);
    
}

FrameCommandRemoveObject::~FrameCommandRemoveObject()
{
}
result
FrameCommandRemoveObject::get_command_info_specific(std::string& info)
{
	info = "	Remove objects at depth : ";
	info += std::to_string(this->cur_depth)+" | ";//+" name: "+this->child->awd_block->get_name()+"\n";
	if(this->child_commands.size()>0){
		for(FrameCommandBase* child_cmd:this->child_commands){
			info += std::to_string(child_cmd->cur_depth)+" | ";//+" name: "+child_cmd->child->awd_block->get_name()+"\n";
		}
	}
	return AWD::result::AWD_SUCCESS;
}




double
FrameCommandRemoveObject::compare_to_command_specific(FrameCommandBase* frameCommand)
{
	FrameCommandRemoveObject* thisFC=(FrameCommandRemoveObject*)frameCommand;
	if(thisFC==NULL){
		return 0;
	}
	if(this->get_objectType()!=thisFC->get_objectType()){
		return 0;
	}
	if(this->get_command_type()!=thisFC->get_command_type()){
		return 0;
	}
	if(this->get_object_block()!=thisFC->get_object_block()){
		return 0;
	}
	// the commands
	int equal=1;
	int equalCnt=1;
	return double(equal)/double(equalCnt);
}
void
FrameCommandRemoveObject::update_by_command_specific(FrameCommandBase* frameCommand)
{
	//int equal=0;
}

TYPES::UINT32
FrameCommandRemoveObject::calc_command_length_specific(SETTINGS::BlockSettings * blockSettings)
{
	 
    int len = sizeof(TYPES::UINT8); // command_type
	len += sizeof(TYPES::UINT16); // count of commands
	len += this->child_commands.size() * sizeof(TYPES::UINT16); // childs ids for child commands
	len += sizeof(TYPES::UINT16); // childs ids 
	
    return len;
}

void
FrameCommandRemoveObject::write_command_specific(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * blockSettings, AWDFile* awd_file)
{
	
	fileWriter->writeUINT8(TYPES::UINT8(ANIM::frame_command_type::FRAME_COMMAND_REMOVE));// command type
	TYPES::UINT16 num_commands=TYPES::UINT16(this->child_commands.size()+1);
	fileWriter->writeUINT16(num_commands);	// number of commands
	fileWriter->writeINT16(this->cur_depth);	// first target depth
	for(FrameCommandBase* child_cmd:this->child_commands){
		fileWriter->writeINT16(child_cmd->cur_depth);// target_depth
	}
	
    //this->command_properties->write_attributes(fileWriter, blockSettings);
}

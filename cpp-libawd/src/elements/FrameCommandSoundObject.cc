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
 
FrameCommandSoundObject::FrameCommandSoundObject() :
FrameCommandBase()
{
    this->limit_in=0;
    this->limit_out=0;
    this->repeats=0;    
    
}

FrameCommandSoundObject::~FrameCommandSoundObject()
{
}
result
FrameCommandSoundObject::get_command_info_specific(std::string& info)
{
	std::string cmdtype="";
	if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)
		cmdtype="AddChild";
	else if(this->get_command_type()==frame_command_type::FRAME_COMMAND_REMOVE)
		cmdtype="Remove";
	else if(this->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)
		cmdtype="Update";
    
	info = "	cmd-type: "+cmdtype+" | obj-id: "+std::to_string(this->get_objID());
    
	return AWD::result::AWD_SUCCESS;
}


void
FrameCommandSoundObject::set_loop_mode(TYPES::UINT32 repeats){
    // should only be called if sound mode is finitve.
    this->repeats=repeats;
    
}

void
FrameCommandSoundObject::set_sound_limit(TYPES::UINT32 limit_in, TYPES::UINT32 limit_out){
    
    this->limit_in=limit_in;
    
    this->limit_out=limit_out;
}

double
FrameCommandSoundObject::compare_to_command_specific(FrameCommandBase* frameCommand)
{
	FrameCommandSoundObject* thisFC=(FrameCommandSoundObject*)frameCommand;
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
FrameCommandSoundObject::update_by_command_specific(FrameCommandBase* frameCommand)
{
	//int equal=0;
}

TYPES::UINT32
FrameCommandSoundObject::calc_command_length_specific(SETTINGS::BlockSettings * blockSettings)
{
	
	if(this->get_command_type()!=frame_command_type::FRAME_COMMAND_REMOVE){
		
	}    
    int len;
    len = sizeof(TYPES::UINT16); // command_type
	
    return len;
}

void
FrameCommandSoundObject::write_command_specific(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * blockSettings, AWDFile* awd_file)
{
	
	fileWriter->writeUINT16(TYPES::UINT16(this->get_command_type()));// command type
	
    //this->command_properties->write_attributes(fileWriter, blockSettings);
}

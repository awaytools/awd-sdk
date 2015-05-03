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
 
FrameCommandBase::FrameCommandBase() 
{
    this->command_properties=new ATTR::NumAttrList();
	this->command_type = frame_command_type::FRAME_COMMAND_ADD_CHILD;
	this->hasRessource=false;// if true than the command will be "add" not "update"
	this->objID = -1;
	this->object_block = NULL;
	this->isUsed=false;
	this->hasRessource=false;// if true than the command will be "add" not "update"
	this->prev_frame=NULL;
	this->is_child=false;
	this->child_instance=NULL;
	this->child=NULL;
}

FrameCommandBase::~FrameCommandBase()
{
    delete this->command_properties;
	for(FrameCommandBase* fc:this->child_commands)
		delete fc;
	this->object_block = NULL;
}

result
FrameCommandBase::get_command_info(std::string& info)
{
	return get_command_info_specific(info);
}

double
FrameCommandBase::compare_to_command(FrameCommandBase * frame_command)
{
	return compare_to_command_specific(frame_command);
}

void
FrameCommandBase::set_depth(TYPES::UINT32 depth)
{
    this->depth = depth;
}
TYPES::UINT32
FrameCommandBase::get_depth()
{
	return this->depth;
}
void
FrameCommandBase::update_by_command(FrameCommandBase * frame_command)
{
	update_by_command_specific(frame_command);
}
void
FrameCommandBase::write_command(FILES::FileWriter * fileWriter,  SETTINGS::BlockSettings * settings, AWDFile* awd_file)
{
	write_command_specific(fileWriter, settings, awd_file);
}
TYPES::UINT32
FrameCommandBase::calc_command_length( SETTINGS::BlockSettings * settings)
{
	return calc_command_length_specific(settings);
}
std::string&
FrameCommandBase::get_objectType()
{
	return this->objectType;
}
void
FrameCommandBase::set_objectType(const std::string& objectType)
{
    this->objectType = objectType;
}
std::string&
FrameCommandBase::get_resID()
{
	return this->resID;
}
void
FrameCommandBase::set_resID(const std::string& new_res)
{
	this->hasRessource=true;
    this->resID = new_res;
}
TYPES::UINT32
FrameCommandBase::get_objID()
{
	return this->objID;
}
void
FrameCommandBase::set_objID(TYPES::UINT32 objID)
{
    this->objID = objID;
}
void
FrameCommandBase::set_object_block(BASE::AWDBlock * object_block)
{
    this->object_block = object_block;
}
BASE::AWDBlock*
FrameCommandBase::get_object_block()
{
    return this->object_block;
}
ATTR::NumAttrList *
FrameCommandBase::get_command_properties()
{
    return this->command_properties;
}
frame_command_type
FrameCommandBase::get_command_type()
{
    return this->command_type;
}
void
FrameCommandBase::set_layerID(TYPES::UINT32 layerID)
{
	this->layerID = layerID;
}
TYPES::UINT32
FrameCommandBase::get_layerID()
{
	return this->layerID;
}
void
FrameCommandBase::set_command_type(frame_command_type command_type)
{
    this->command_type=command_type;
}
bool
FrameCommandBase::get_hasRessource()
{
    return this->hasRessource;
}
void
FrameCommandBase::set_hasRessource(bool hasRessource)
{
    this->hasRessource=hasRessource;
}

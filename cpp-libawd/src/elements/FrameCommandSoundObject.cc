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
 
FrameCommandSoundObject::FrameCommandSoundObject()
{
    this->limit_in=0;
    this->limit_out=0;
    this->repeats=0;    
    
}

FrameCommandSoundObject::~FrameCommandSoundObject()
{
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



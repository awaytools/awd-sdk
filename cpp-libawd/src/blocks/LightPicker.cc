#include "blocks/light_picker.h"
#include <string>
#include <vector>
#include "utils/awd_properties.h"
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"

#include "files/file_writer.h"
#include "blocks/shadows.h"
#include "blocks/light.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

LightPicker::LightPicker(std::string& name) :
	AWDBlock(block_type::LIGHTPICKER, name),
	
	AttrElementBase()
{
}
LightPicker::LightPicker():
	AWDBlock(block_type::LIGHTPICKER),
	AttrElementBase()
{
}
LightPicker::~LightPicker(){
}
TYPES::state LightPicker::validate_block_state()
{
	return TYPES::state::VALID;
}
void
LightPicker::set_lights(std::vector<Light*> newLights)
{
	this->lights = newLights;
}
std::vector<Light*>
LightPicker::get_lights()
{
		return this->lights;
}
AWDBlock *
LightPicker::get_shadowMethod()
{
	for (Light * awdLight:this->lights){
		if (awdLight->get_shadowMethod()!=NULL) {
			ShadowMethod * checkMethod=(ShadowMethod *)awdLight->get_shadowMethod();
			if(checkMethod->get_autoApply()){
				return checkMethod;
			}
		}
	}
	return NULL;
}
bool
LightPicker::check_shadowMethod(ShadowMethod * checkMethod)
{
	Light * shadowLight=(Light*)checkMethod->get_awdLight();
	Light * awdLight=NULL;
	for (Light * awdLight:this->lights){
		if (awdLight==shadowLight){
			return true;
		}
	}
	return false;
}

result
LightPicker::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	for (Light * awdLight:this->lights)
		awdLight->add_with_dependencies(target_file, instance_type);
	return result::AWD_SUCCESS;
}

TYPES::UINT32
LightPicker::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;
	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size());
	len += sizeof(TYPES::UINT16); //num lights
	len += TYPES::UINT32(this->lights.size() * sizeof(BADDR)); //light IDs
	len += this->calc_attr_length(false,true, settings);
	return len;
}

result
LightPicker::write_body(FileWriter * fileWriter, BlockSettings * settings, FILES::AWDFile* file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	fileWriter->writeUINT16(TYPES::UINT16(this->lights.size()));
	// Write animSet id - TODO-> ERROR IF NULL
	/*for (AWD * awdLight:this->lights)
		fileWriter->writeUINT32(awdLight->get_addr());*/
    
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

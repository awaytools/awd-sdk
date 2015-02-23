#include "blocks/shadows.h"
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::ATTR;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;
using namespace AWD::LIGHTS;

ShadowMethod::ShadowMethod(std::string& name, shadow_method_type shadow_type) :
	AWDBlock(block_type::SHADOW_METHOD, name),
	
	AttrElementBase()
{
	this->shadow_props=new NumAttrList();
	this->base_method=NULL;
	this->shadow_type = shadow_type;
	this->awdLight = NULL;
	this->autoApply=false;
}

ShadowMethod::ShadowMethod():
	AWDBlock(block_type::SHADOW_METHOD),
	AttrElementBase()
{
}
ShadowMethod::~ShadowMethod()
{
	delete this->shadow_props;
}

TYPES::state ShadowMethod::validate_block_state()
{
	return TYPES::state::VALID;
}
NumAttrList *
ShadowMethod::get_shadow_props()
{
	return this->shadow_props;
}

shadow_method_type
ShadowMethod::get_shadow_type()
{
	return this->shadow_type;
}
void
ShadowMethod::set_shadow_type(shadow_method_type shadow_type)
{
	this->shadow_type=shadow_type;
}
AWDBlock *
ShadowMethod::get_base_method()
{
	return this->base_method;
}
void
ShadowMethod::set_base_method(AWDBlock * base_method)
{
	this->base_method=base_method;
}
void
ShadowMethod::set_awdLight(AWDBlock * awdLight)
{
	this->awdLight=awdLight;
}
AWDBlock *
ShadowMethod::get_awdLight()
{
	return this->awdLight;
}
bool
ShadowMethod::get_autoApply()
{
	return this->autoApply;
}
void
ShadowMethod::set_autoApply(bool autoApply)
{
	this->autoApply=autoApply;
}

void
ShadowMethod::add_color_shadow_prop(int targetID, TYPES::UINT32 targetValue, TYPES::UINT32 defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT32));
		*newVal.col = targetValue;
		//this->shadow_props->set(targetID, newVal, sizeof(TYPES::UINT32), TYPES::data_types::COLOR);
	}
}
void
ShadowMethod::add_number_shadow_prop(int targetID, float targetValue, float defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::F64));
		*newVal.F64 = targetValue;
		//this->shadow_props->set(targetID, newVal, sizeof(TYPES::F64), TYPES::data_types::FLOAT64);
	}
}
void
ShadowMethod::add_int16_shadow_prop(int targetID, int targetValue, int defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT16));
		*newVal.ui16 = targetValue;
		//this->shadow_props->set(targetID, newVal, sizeof(TYPES::UINT16), TYPES::data_types::UINT16);
	}
}
void
ShadowMethod::add_int_shadow_prop(int targetID, int targetValue, int defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT32));
		*newVal.ui32 = targetValue;
		//this->shadow_props->set(targetID, newVal, sizeof(TYPES::UINT32), TYPES::data_types::UINT32);
	}
}
void
ShadowMethod::add_bool_shadow_prop(int targetID, bool targetValue, bool defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(bool));
		*newVal.b = targetValue;
		//this->shadow_props->set(targetID, newVal, sizeof(bool), TYPES::data_types::BOOL);
	}
}
result
ShadowMethod::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	if (this->base_method != NULL) {
		this->base_method->add_with_dependencies(target_file, instance_type);
		//this->add_int_shadow_prop(1, this->base_method->get_addr(), 0);
	}
	if (this->awdLight != NULL) {
		this->awdLight->add_with_dependencies(target_file, instance_type);
	}
	return result::AWD_SUCCESS;
}

TYPES::UINT32
ShadowMethod::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;
	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	len += sizeof(TYPES::UINT32); //light_id
	len += sizeof(TYPES::UINT16); //type
	len += this->shadow_props->calc_length(settings);
	len += this->calc_attr_length(true, true, settings);
	return len;
}

result
ShadowMethod::write_body(FileWriter * fileWriter, BlockSettings *settings, AWDFile* file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	int light_addr = 0;
	//if (this->awdLight != NULL)
	//	light_addr = this->awdLight->get_addr();
	fileWriter->writeUINT32(light_addr);

	fileWriter->writeUINT16(TYPES::UINT16(this->shadow_type));

	this->shadow_props->write_attributes(fileWriter, settings);
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

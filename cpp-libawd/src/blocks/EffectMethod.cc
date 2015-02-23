#include "blocks/effect_method.h"
#include <string>
#include <vector>
#include "utils/awd_types.h"
#include "utils/awd_properties.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::ATTR;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

EffectMethod::EffectMethod(std::string& name, effect_method_type type) :
AWDBlock(EFFECT_METHOD, name),
	
	AttrElementBase()
{
	this->effect_props=new NumAttrList();
	this->type = type;
	this->awdBlock1 = NULL;
	this->awdBlock2 = NULL;
	this->awdBlock3 = NULL;
	this->awdBlock4 = NULL;
}

EffectMethod::EffectMethod():
	AWDBlock(block_type::EFFECT_METHOD),
	AttrElementBase()
{
}
EffectMethod::~EffectMethod()
{
	delete this->effect_props;
	this->awdBlock1 = NULL;
	this->awdBlock2 = NULL;
	this->awdBlock3 = NULL;
	this->awdBlock4 = NULL;
}
TYPES::state EffectMethod::validate_block_state()
{
	return TYPES::state::VALID;
}
void
EffectMethod::set_awdBlock1(AWDBlock * newawdBlock)
{
	this->awdBlock1 = newawdBlock;
}
void
EffectMethod::set_awdBlock2(AWDBlock * newawdBlock)
{
	this->awdBlock2 = newawdBlock;
}
void
EffectMethod::set_awdBlock3(AWDBlock * newawdBlock)
{
	this->awdBlock3 = newawdBlock;
}
void
EffectMethod::set_awdBlock4(AWDBlock * newawdBlock)
{
	this->awdBlock4 = newawdBlock;
}
NumAttrList *
EffectMethod::get_effect_props()
{
	return this->effect_props;
}

void
EffectMethod::add_color_method_prop(int targetID, TYPES::UINT32 targetValue, TYPES::UINT32 defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT32));
		*newVal.col = targetValue;
		//this->effect_props->set(targetID, newVal, sizeof(TYPES::UINT32), TYPES::data_types::COLOR);
	}
}
void
EffectMethod::add_number_method_prop(int targetID, float targetValue, float defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::F64));
		*newVal.F64 = targetValue;
		//this->effect_props->set(targetID, newVal, sizeof(TYPES::F64), TYPES::data_types::FLOAT64);
	}
}
void
EffectMethod::add_int_method_prop(int targetID, int targetValue, int defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT16));
		*newVal.ui16 = targetValue;
		//this->effect_props->set(targetID, newVal, sizeof(TYPES::UINT16), TYPES::data_types::UINT16);
	}
}
void
EffectMethod::add_bool_method_prop(int targetID, bool targetValue, bool defaultValue)
{
	if (targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(bool));
		*newVal.b = targetValue;
		//this->effect_props->set(targetID, newVal, sizeof(bool), TYPES::data_types::BOOL);
	}
}
result
EffectMethod::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	if (this->awdBlock1 != NULL) {

		this->awdBlock1->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = malloc(sizeof(BADDR));
		//*tex_val.addr = this->awdBlock1->get_addr();
		//this->effect_props->set(PROPS_BADDR1, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	if (this->awdBlock2 != NULL) {
		this->awdBlock2->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = malloc(sizeof(BADDR));
		//*tex_val.addr = this->awdBlock2->get_addr();
		//this->effect_props->set(PROPS_BADDR2, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	if (this->awdBlock3 != NULL) {
		this->awdBlock3->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = malloc(sizeof(BADDR));
		//*tex_val.addr = this->awdBlock3->get_addr();
		//this->effect_props->set(PROPS_BADDR3, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	if (this->awdBlock4 != NULL) {
		this->awdBlock4->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = malloc(sizeof(BADDR));
		//*tex_val.addr = this->awdBlock4->get_addr();
		//this->effect_props->set(PROPS_BADDR4, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	return result::AWD_SUCCESS;
}

TYPES::UINT32
EffectMethod::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;
	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	len += sizeof(TYPES::UINT16); //type
	len += this->effect_props->calc_length(settings);
	len += this->calc_attr_length(true, true, settings);
	return len;
}

result
EffectMethod::write_body(FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
	
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	fileWriter->writeUINT16(TYPES::UINT16(this->type));
	this->effect_props->write_attributes(fileWriter, settings);
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
	
}

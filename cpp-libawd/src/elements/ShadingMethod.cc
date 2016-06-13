#include "elements/shading.h"
#include "utils/awd_properties.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::MATERIAL;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::ATTR;
using namespace AWD::SETTINGS;



ShadingMethod::ShadingMethod(shading_type type) :
	AttrElementBase()
{
	this->shading_props=new NumAttrList();
	this->type = type;
	this->awdBlock1 = NULL;
	this->awdBlock2 = NULL;
	this->awdBlock3 = NULL;
	this->awdBlock4 = NULL;
}

ShadingMethod::~ShadingMethod()
{
	delete this->shading_props;
	this->awdBlock1 = NULL;
	this->awdBlock2 = NULL;
	this->awdBlock3 = NULL;
	this->awdBlock4 = NULL;
}
void
ShadingMethod::set_awdBlock1(AWDBlock * newawdBlock)
{
	this->awdBlock1 = newawdBlock;
}
void
ShadingMethod::set_awdBlock2(AWDBlock * newawdBlock)
{
	this->awdBlock2 = newawdBlock;
}
void
ShadingMethod::set_awdBlock3(AWDBlock * newawdBlock)
{
	this->awdBlock3 = newawdBlock;
}
void
ShadingMethod::set_awdBlock4(AWDBlock * newawdBlock)
{
	this->awdBlock4 = newawdBlock;
}
NumAttrList *
ShadingMethod::get_shading_props()
{
	return this->shading_props;
}
shading_type
ShadingMethod::get_shading_type()
{
	return this->type;
}

void
ShadingMethod::add_shading_color_property(int targetID, TYPES::UINT32 targetValue, TYPES::UINT32 defaultValue)
{
}
void
ShadingMethod::add_shading_number_property(int targetID, float targetValue, float defaultValue)
{
}
void
ShadingMethod::add_shading_int_property(int targetID, int targetValue, int defaultValue)
{
}
void
ShadingMethod::add_shading_bool_property(int targetID, bool targetValue, bool defaultValue)
{
}
void
ShadingMethod::add_shading_int8_property(int targetID, int targetValue, int defaultValue)
{
}

result
ShadingMethod::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	/*
	if (this->awdBlock1 != NULL) {
		this->awdBlock1->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = (BADDR *)malloc(sizeof(BADDR));
		// *tex_val.addr = this->awdBlock1->get_addr();
		this->shading_props->set(PROPS_BADDR1, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	if (this->awdBlock2 != NULL) {
		this->awdBlock2->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = (BADDR *)malloc(sizeof(BADDR));
		// *tex_val.addr = this->awdBlock2->get_addr();
		this->shading_props->set(PROPS_BADDR2, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	if (this->awdBlock3 != NULL) {
		this->awdBlock3->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = (BADDR *)malloc(sizeof(BADDR));
		// *tex_val.addr = this->awdBlock3->get_addr();
		this->shading_props->set(PROPS_BADDR3, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	if (this->awdBlock4 != NULL) {
		this->awdBlock4->add_with_dependencies(target_file, instance_type);
		union_ptr tex_val;
		tex_val.v = (BADDR *)malloc(sizeof(BADDR));
		// *tex_val.addr = this->awdBlock4->get_addr();
		this->shading_props->set(PROPS_BADDR4, tex_val, sizeof(BADDR), TYPES::data_types::BADDR);
	}
	*/
    return result::AWD_SUCCESS;
}

TYPES::UINT32
ShadingMethod::calc_method_length(BlockSettings * blockSettings)
{
	int len;
	len = sizeof(TYPES::UINT16); // shading type
	len += this->shading_props->calc_length(blockSettings); // shading props
	len += this->calc_attr_length(false,true, blockSettings); // shading attributes
	return len;
}

result
ShadingMethod::write_method(FileWriter* fileWriter, BlockSettings * blockSettings)
{
	fileWriter->writeUINT16(TYPES::UINT16(this->type));
	this->shading_props->write_attributes(fileWriter, blockSettings);
	this->user_attributes->write_attributes(fileWriter, blockSettings);
	return result::AWD_SUCCESS;
}

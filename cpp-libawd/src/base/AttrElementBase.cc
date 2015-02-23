#include "base/attr.h"
#include "utils/util.h"
using namespace AWD;
using namespace AWD::BASE;
using namespace AWD::TYPES;
using namespace AWD::ATTR;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::SETTINGS;

AttrElementBase::AttrElementBase()
{
	this->properties = new NumAttrList();
	this->user_attributes = new UserAttrList();
}

AttrElementBase::~AttrElementBase()
{
	delete this->properties;
	delete this->user_attributes;
}

/*
void
AttrElementBase::add_with_dependencies(AWDProject *awd)
{
	this->user_attributes->add_namespaces(awd);
}
*/

TYPES::UINT32
AttrElementBase::calc_attr_length(bool with_props, bool with_user_attr, BlockSettings * blockSettings )
{
	TYPES::UINT32 len;

	len = 0;
	if (with_props) len += this->properties->calc_length(blockSettings);
	if (with_user_attr) len += this->user_attributes->calc_length(blockSettings);

	return len;
}

result
AttrElementBase::get_attr(Namespace *ns, std::string& key,
	union_ptr *val, TYPES::UINT32 *val_len, data_types *val_type)
{
	return this->user_attributes->get(ns, key, val, val_len, val_type);
}

result
AttrElementBase::set_attr(Namespace *ns, std::string& key,
	union_ptr val, TYPES::UINT32 val_len, data_types val_type)
{
	this->user_attributes->set(ns, key, val, val_len, val_type);
	return result::AWD_SUCCESS;
}

result
AttrElementBase::add_color_property(TYPES::UINT32 targetID, TYPES::UINT32 targetValue)
{
	/*TYPES::INT32 default_idx = this->find_property_description(targetID, data_types::COLOR);
	if(default_idx<0)
		return result::AWD_ERROR;
	if (targetValue!=*this->property_descriptions[default_idx]->get_val(NULL, NULL).col){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT32));
		*newVal.col = targetValue;
		this->properties->set(targetID, newVal);
	}*/
	return result::AWD_SUCCESS;
}
result
AttrElementBase::add_number_property(TYPES::UINT32 targetID, TYPES::F64 targetValue)
{
	/*
	TYPES::INT32 default_idx = this->find_property_description(targetID, data_types::FLOAT64);
	if(default_idx<0)
		return result::AWD_ERROR;
	if (targetValue!=*this->property_descriptions[default_idx]->get_val(NULL, NULL).F64){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::F64));
		*newVal.F64 = targetValue;
		this->properties->set(targetID, newVal);
	}
	*/
	return result::AWD_SUCCESS;
}
result
AttrElementBase::add_uint32_property(TYPES::UINT32 targetID, TYPES::UINT32 targetValue)
{
	/*
	TYPES::INT32 default_idx = this->find_property_description(targetID, data_types::UINT32);
	if(default_idx<0)
		return result::AWD_ERROR;
	if (targetValue!=*this->property_descriptions[default_idx]->get_val(NULL, NULL).ui32){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT16));
		*newVal.ui32 = targetValue;
		this->properties->set(targetID, newVal);
	}
	*/
	return result::AWD_SUCCESS;
}
result
AttrElementBase::add_uint16_property(TYPES::UINT32 targetID, TYPES::UINT16 targetValue)
{
	/*
	TYPES::INT32 default_idx = this->find_property_description(targetID, data_types::UINT16);
	if(default_idx<0)
		return result::AWD_ERROR;
	if (targetValue!=*this->property_descriptions[default_idx]->get_val(NULL, NULL).ui16){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT16));
		*newVal.ui16 = targetValue;
		this->properties->set(targetID, newVal);
	}
	*/
	return result::AWD_SUCCESS;
}
result
AttrElementBase::add_uint8_property(TYPES::UINT32 targetID, TYPES::UINT8 targetValue)
{
	/*
	TYPES::INT32 default_idx = this->find_property_description(targetID, TYPES::data_types::UINT8);
	if(default_idx<0)
		return result::AWD_ERROR;
	if (targetValue!=*this->property_descriptions[default_idx]->get_val(NULL, NULL).ui8){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT8));
		*newVal.ui8 = targetValue;
		this->properties->set(targetID, newVal);
	}
	*/
	return result::AWD_SUCCESS;
}

result
AttrElementBase::add_bool_property(TYPES::UINT32 targetID, bool targetValue)
{
	/*
	TYPES::INT32 default_idx = this->find_property_description(targetID, TYPES::data_types::BOOL);
	if(default_idx<0)
		return result::AWD_ERROR;
	if (targetValue!=*this->property_descriptions[default_idx]->get_val(NULL, NULL).b){
		union_ptr newVal;
		newVal.v = malloc(sizeof(bool));
		*newVal.b = targetValue;
		this->properties->set(targetID, newVal);
	}
	*/
	return result::AWD_SUCCESS;
}


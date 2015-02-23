#include "base/attr.h"
#include "utils/awd_types.h"
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::ATTR;

NumAttr::NumAttr():
	AttrBase()
{

}

NumAttr::~NumAttr()
{
}
#include "utils/util.h"
/**
	\brief Compares the value against the mdefault value and returns the result.
	If the value is the same as the default value, we will not write this property.
*/
result
NumAttr::compare_against_default()
{
	if(this->value.v==NULL)
		return result::AWD_ERROR;
	if(this->default_value.v==NULL)
		return result::DIFFERENT_PROPERTY_VALUE;
	
	if(TYPES::is_data_type_floats(this->type)){
		if(this->default_value.F64!=this->value.F64)
			return result::DIFFERENT_PROPERTY_VALUE;
	}
	std::string thisString(this->value.str);
	std::string thisStringDefault(this->default_value.str);
	if(thisString!=thisStringDefault)
		return result::DIFFERENT_PROPERTY_VALUE;
	return result::SAME_PROPERTY_VALUE;
}


/**
	\brief Checks if the property should be written when exporting.\n
	This calls the NumAttr::compare_against_default() to check if the value is default or not.\n
	@return True, if the property should be written to file.
*/
bool
NumAttr::use_property(){
	if(this->compare_against_default()==result::DIFFERENT_PROPERTY_VALUE)
		return true;
	return false;
}

/**
	\brief Sets the inital properties for this NumAttr. The value is not set here.\n
	Use NumAttr::set() to set the value.
	@return result
*/
result
NumAttr::set(union_ptr default_val, TYPES::UINT32 val_len, data_types val_type, TYPES::storage_precision_category storage_cat, TYPES::property_storage_type storage_type)
{
	this->default_value = default_val;
	this->value_len = val_len;
	this->type = val_type;
	this->storage_precision_category = storage_cat;
	this->storage_type = storage_type;
	return result::AWD_SUCCESS;
}

/**
	\brief Updates the value of this property
	@return result
*/
result
NumAttr::update_value(union_ptr new_value)
{
	this->value = new_value;
	return result::AWD_SUCCESS;
}

/**
	\brief Updates the value of this property
	@return result
*/
result
NumAttr::update_value(union_ptr new_value, TYPES::UINT32 new_length)
{
	this->value = new_value;
	this->value_len = new_length;
	return result::AWD_SUCCESS;
}
/**
	\brief Checks if the property has a data-type that is a float number.
	@return True, if the property data-type uses float numbers
*/
bool 
NumAttr::is_floating_value()
{
	if((this->type!=data_types::FLOAT64) && (this->type!=data_types::VECTOR2x1) && (this->type!=data_types::VECTOR3x1)  && (this->type!=data_types::VECTOR4x1)
		&& (this->type!=data_types::MTX3x2)  && (this->type!=data_types::MTX3x3)  && (this->type!=data_types::MTX4x3)  && (this->type!=data_types::MTX4x4))
	{
		return false;
	}
	return true;
}

/**
	\brief Should only be called if data_type is float-numbers.\n
	@return True, if the property should be written with float64 - False, if it should be written with float32.
*/
bool 
NumAttr::get_storage_precision(SETTINGS::BlockSettings* settings)
{
	bool storage_precision=false;
	if(this->storage_precision_category==TYPES::storage_precision_category::GEOMETRY_VALUES)
		storage_precision=settings->get_wide_geom();
	else if(this->storage_precision_category==TYPES::storage_precision_category::FORCE_PRECISION)
		storage_precision=true;
	else if(this->storage_precision_category==TYPES::storage_precision_category::MATRIX_VALUES)
		storage_precision=settings->get_wide_matrix();
	else if(this->storage_precision_category==TYPES::storage_precision_category::PROPERIES_VALUES)
		storage_precision=settings->get_wide_props();
	else if(this->storage_precision_category==TYPES::storage_precision_category::ATTRIBUTES_VALUES)
		storage_precision=settings->get_wide_attributes();
	return storage_precision;

}

TYPES::UINT32
NumAttr::get_num_attr_length(SETTINGS::BlockSettings* settings)
{
	if(!this->is_floating_value())
		return this->value_len;
	if(this->get_storage_precision(settings))
		return this->value_len;

	return this->value_len/2;
}

/** 
	\brief	Overwrites write_metadata of AttrBase.\n
	NumAttr Writes following meta-data:
	-	key-id (TYPES::UINT16)
	-	data-length (TYPES::UINT32)
*/
result
NumAttr::write_num_attr(FileWriter* fileWriter, BlockSettings* settings)
{
	result res = result::AWD_SUCCESS;
	res = fileWriter->writeUINT16(this->key);
	if(res!=result::AWD_SUCCESS)
		return res;
	res = fileWriter->writeUINT32(this->value_len);
	if(res!=result::AWD_SUCCESS)
		return res;

	bool storage_prec=false;
	bool use_scale = false;
	if(this->storage_type==property_storage_type::SCALED_PROPERTY)
		use_scale=true;	
	if(!this->is_floating_value())
		storage_prec = this->get_storage_precision(settings);

	return fileWriter->writeUnion(this->value, this->type, this->value_len, storage_prec, use_scale, settings->get_scale());
}


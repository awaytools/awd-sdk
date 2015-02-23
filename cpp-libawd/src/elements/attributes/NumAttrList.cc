#include "base/attr.h"

#include "utils/awd_types.h"
#include "files/file_writer.h"
#include "utils/settings.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::ATTR;

NumAttrList::NumAttrList()
{
}

NumAttrList::~NumAttrList()
{
	for(NumAttr* attr : this->attributes){
		delete attr;
	}
}

TYPES::UINT32
NumAttrList::calc_length(BlockSettings * blockSettings)
{
	TYPES::UINT32 len = sizeof(TYPES::UINT32); // length of attribute-list always included

	for(NumAttr* attr : this->attributes){
		if(attr->use_property())
			len += (6 + attr->get_val_len(blockSettings));
	}

	return len;
}

result 
NumAttrList::write_attributes(FileWriter* fileWriter, BlockSettings * blockSettings)
{
	TYPES::UINT32 len = this->calc_length(blockSettings) - sizeof(TYPES::UINT32);
	result res = fileWriter->writeUINT32(len);
	if(res!=result::AWD_SUCCESS)
		return res;
	
	for(NumAttr* attr : this->attributes){
		if(attr->use_property()){
			res = attr->write_num_attr(fileWriter, blockSettings);
			if(res!=result::AWD_SUCCESS)
				return res;
		}
	}
	return result::AWD_SUCCESS;
}

NumAttr *
NumAttrList::find(awd_propkey key)
{
	for(NumAttr* attr : this->attributes){
		if (attr->key == key)
			return attr;
	}
	return NULL;
}

bool
NumAttrList::get(awd_propkey key, union_ptr *val, TYPES::UINT32 *val_len, data_types *val_type)
{
	NumAttr *attr;

	attr = this->find(key);
	if (attr) {
		*val = attr->get_val(val_len, val_type);
		return true;
	}
	return false;
}

result
NumAttrList::set(awd_propkey key, union_ptr default_value)
{
	NumAttr *attr=NULL;
	attr = this->find(key);
	if (!attr) 
		return result::AWD_ERROR;	
	attr->update_value(default_value);
	return result::AWD_SUCCESS;
}
result
NumAttrList::set(awd_propkey key, union_ptr default_value, TYPES::UINT32 data_length)
{
	NumAttr *attr=NULL;
	attr = this->find(key);
	if (!attr) 
		return result::AWD_ERROR;	
	attr->update_value(default_value, data_length);
	return result::AWD_SUCCESS;
}

result
NumAttrList::add(awd_propkey key, union_ptr default_value, TYPES::UINT32 value_length, data_types type, TYPES::storage_precision_category storage_cat, TYPES::property_storage_type storage_type)
{
	NumAttr *attr;

	attr = this->find(key);
	if (!attr) {
		attr = new NumAttr();
		attr->key = key;
		this->attributes.push_back(attr);
	}
	attr->set(default_value, value_length, type, storage_cat, storage_type);
	return result::AWD_SUCCESS;
}


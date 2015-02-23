#include "base/attr.h"

#include <string>
#include "utils/awd_types.h"
#include "files/file_writer.h"
#include "blocks/namespace.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::ATTR;

UserAttrList::UserAttrList()
{
}

UserAttrList::~UserAttrList()
{
	for(UserAttr* attr : this->attributes){
		delete attr;
	}
}

TYPES::UINT32
UserAttrList::calc_length(BlockSettings * blockSettings)
{
	TYPES::UINT32 len = sizeof(TYPES::UINT32); // length of attribute-list always included
	
	for(UserAttr* attr : this->attributes){
		len += (8 + (TYPES::UINT32)attr->get_key().size() + (TYPES::UINT32)attr->get_val_len(blockSettings));
	}

	return len;
}

result
UserAttrList::write_attributes(FileWriter* fileWriter, BlockSettings * blockSettings)
{
	fileWriter->writeUINT32(this->calc_length(blockSettings) - sizeof(TYPES::UINT32));
	
	for(UserAttr* attr : this->attributes){
		attr->write_user_attr(fileWriter);
	}
	return result::AWD_SUCCESS;
}

UserAttr *
UserAttrList::find(Namespace *ns, std::string& key)
{
	for(UserAttr* attr : this->attributes){
		if (attr->get_ns() != ns)
			continue;
		if (attr->get_key() == key)
			return attr;
	}
	return NULL;
}

result
UserAttrList::get(Namespace *ns, std::string& key,
	union_ptr *val, TYPES::UINT32 *val_len, data_types *val_type)
{
	UserAttr *attr;

	attr = this->find(ns, key);
	if (attr) {
		*val = attr->get_val(val_len, val_type);
		return result::AWD_ERROR;
	}
	
	return result::AWD_SUCCESS;
}

result
UserAttrList::set(Namespace *ns, std::string& key,	union_ptr value, TYPES::UINT32 value_length, data_types type)
{
	UserAttr *attr;

	attr = this->find(ns, key);
	if (attr == NULL) {
		attr = new UserAttr(ns, key);
		this->attributes.push_back(attr);
	}

	attr->set(value, value_length, type);
	return result::AWD_SUCCESS;

}

#include "base/attr.h"

#include <string>

#include "utils/awd_types.h"
#include "files/file_writer.h"
#include "blocks/namespace.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::ATTR;


UserAttr::UserAttr(Namespace *ns, std::string& key):
	AttrBase()
{
	this->ns = ns;
	this->key = key;
}

UserAttr::~UserAttr()
{
}

result
UserAttr::set(union_ptr val, TYPES::UINT32 val_len, data_types val_type)
{
	this->value = val;
	this->value_len = val_len;
	this->type = val_type;
	return result::AWD_SUCCESS;
}


Namespace *
UserAttr::get_ns()
{
	return this->ns;
}

std::string&
UserAttr::get_key()
{
	return this->key;
}

/** 
	\brief	
	UserAttr Writes following meta-data:
	-	name_space handle (TYPES::UINT8)
	-	key-name (String + TYPES::UINT16)
	-	data-type (TYPES::UINT8)
	-	data-length (TYPES::UINT8)
*/
result
UserAttr::write_user_attr(FileWriter* fileWriter)
{
	TYPES::UINT8 type;
	TYPES::UINT32 len_be;
	NS_ID ns_handle;
	len_be = this->value_len;
	type = (TYPES::UINT8)this->type;
	ns_handle = 0;// todo:handle namepsace for properties;

	result res = result::AWD_SUCCESS;

	res = fileWriter->writeUINT8(ns_handle);
	if(res!=result::AWD_SUCCESS)
		return res;
	res = fileWriter->writeSTRING(this->key, FILES::write_string_with::LENGTH_AS_UINT16);
	if(res!=result::AWD_SUCCESS)
		return res;
	res = fileWriter->writeUINT8(type);
	if(res!=result::AWD_SUCCESS)
		return res;
	res = fileWriter->writeUINT32(len_be);
	if(res!=result::AWD_SUCCESS)
		return res;
	
	return fileWriter->writeUnion(this->value, this->type, this->value_len, false, false, 1.0);
}

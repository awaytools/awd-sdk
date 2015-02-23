#include "base/attr.h"
#include "utils/awd_types.h"
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::ATTR;

AttrBase::AttrBase()
{
	this->value.v=NULL;
	
}
AttrBase::~AttrBase()
{
	if (this->type!=TYPES::data_types::STRING){
		if(this->value.v!=NULL)
			free(this->value.v);
	}
	this->value.v=NULL;
	this->value_len=0;
	
}
result
AttrBase::write_data(FileWriter* fileWriter, bool storage_precision, bool use_scale)
{
	
	return result::AWD_ERROR;
}


TYPES::UINT32
AttrBase::get_val_len(BlockSettings * settings)
{
	// get bthe value length for this attribute, but divide by 2 if the attribute should be saved with storage.

	return this->value_len;
}


TYPES::union_ptr
AttrBase::get_val(TYPES::UINT32* val_len, TYPES::data_types* val_type)
{
    *val_len = this->value_len;
    *val_type = this->type;
    return this->value;
}
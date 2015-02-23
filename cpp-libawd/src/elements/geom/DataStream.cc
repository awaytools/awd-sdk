#include "elements/stream.h"
#include <vector>
#include "utils/awd_types.h"
#include "utils/util.h"
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;


DataStreamAttr::DataStreamAttr(GEOM::data_stream_attr_type type, TYPES::union_ptr data)
{
}
DataStreamAttr::DataStreamAttr()
{
}
DataStreamAttr::~DataStreamAttr()
{
}


GEOM::data_stream_attr_type 
DataStreamAttr::get_type()
{
	return this->type;
}

TYPES::union_ptr  
DataStreamAttr::get_data()
{
	return this->data;
}

void 
DataStreamAttr::set_data(TYPES::union_ptr data)
{
	this->data=data;
}

				
DataStreamAttrDesc::DataStreamAttrDesc(GEOM::data_stream_attr_type type, TYPES::data_types data_type, TYPES::UINT32 data_length, TYPES::storage_precision_category storage_cat, bool use_scale,  bool preserve_all_channels, bool force_unique, bool channel, const std::string& channel_id)
{
	this->type = type;
	this->data_type = data_type;
	this->storage_cat = storage_cat;
	this->use_scale = use_scale;
	this->preserve_all_channels = preserve_all_channels;
	this->force_unique = force_unique;
	this->channel = channel;
	this->channel_id = channel_id;
	this->data_length = data_length;
}
DataStreamAttrDesc::DataStreamAttrDesc()
{
}
DataStreamAttrDesc::~DataStreamAttrDesc()
{
}

TYPES::UINT32 
DataStreamAttrDesc::get_data_length()
{
	return this->data_length;
}

TYPES::storage_precision_category 
DataStreamAttrDesc::get_storage_cat()
{
	return this->storage_cat;
}		

GEOM::DataStreamAttr 
DataStreamAttrDesc::get_default_value()
{
	return this->default_value;
}		

GEOM::data_stream_attr_type 
DataStreamAttrDesc::get_type()
{
	return this->type;
}		

TYPES::data_types 
DataStreamAttrDesc::get_data_type()
{
	return this->data_type;
}		

bool 
DataStreamAttrDesc::has_channel()	
{
	return this->channel;
}		

std::string& 
DataStreamAttrDesc::get_channel_id()
{
	return this->channel_id;
}		

bool 
DataStreamAttrDesc::preserve_channels()
{
	return this->preserve_all_channels;
}		

	


DataStreamRecipe::DataStreamRecipe(GEOM::stream_type stream_type, GEOM::stream_target stream_target_type, std::vector<GEOM::DataStreamAttrDesc>& attr_descriptions)
{
	this->stream_type = stream_type;
	this->stream_target_type = stream_target_type;
	this->attr_descriptions = attr_descriptions;
	if(this->attr_descriptions.size()==0)
		return; //invalid stream !!
	
	// if we have multiple DataStreamAttrDesc, we want to check if they all have the same data_type.

	this->homogen=true;
	if(this->attr_descriptions.size()==1)
		return;

	bool this_type = TYPES::is_data_type_floats(this->attr_descriptions[0].get_data_type());
	TYPES::storage_precision_category this_storage_cat = this->attr_descriptions[0].get_storage_cat();
	for(GEOM::DataStreamAttrDesc attr_desc : this->attr_descriptions){
		if(this_type != TYPES::is_data_type_floats(attr_desc.get_data_type())){
			this->homogen=false;
			return;
		}
		if(this_storage_cat != attr_desc.get_storage_cat()){
			this->homogen=false;
			return;
		}
	}
}
DataStreamRecipe::~DataStreamRecipe()
{
}	

bool 
DataStreamRecipe::is_homogen()
{
	return this->homogen;
}		

std::vector<GEOM::DataStreamAttrDesc>& 
DataStreamRecipe::get_attr_descriptions()
{
	return this->attr_descriptions;
}		

GEOM::stream_type 
DataStreamRecipe::get_stream_type()
{
	return this->stream_type;
}		
	
GEOM::stream_target 
DataStreamRecipe::get_stream_target_type()
{
	return this->stream_target_type;
}		


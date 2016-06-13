#include "blocks/meta.h"

#include "utils/awd_types.h"
#include "utils/awd_properties.h"
#include "utils/awd_globals.h"
#include "utils/util.h"
#include "base/block.h"
#include "base/attr.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;

MetaData::MetaData(const std::string& generator_name, const std::string& generator_version) :
	AWDBlock(block_type::METADATA),
	AttrElementBase()
{
	this->generator_name=generator_name;
	this->generator_version=generator_version;
	this->encoder_name = "libawd";
    this->encoder_version = FILES::int_to_string(AWD::VERSION_MAJOR) + "." + FILES::int_to_string(AWD::VERSION_MINOR) + "." + FILES::int_to_string(AWD::VERSION_BUILD) +"." + AWD::VERSION_RELEASE;

	//	Create default-properties for every property that should be able to set on MetaData.
	//	String are getting set with a NULL pointer as default.

	TYPES::union_ptr default_union;
	default_union.v=NULL;
	this->properties->add(PROP_META_ENCODER_NAME,	default_union, 0,   data_types::STRING, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	this->properties->add(PROP_META_ENCODER_VER,	default_union, 0,   data_types::STRING, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	this->properties->add(PROP_META_GENERATOR_NAME, default_union, 0,   data_types::STRING, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	this->properties->add(PROP_META_GENERATOR_VER,	default_union, 0,   data_types::STRING, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);

	default_union.str = (char*)this->encoder_name.c_str();
	this->properties->set(PROP_META_ENCODER_NAME, default_union, TYPES::UINT32(this->encoder_name.size()));
	default_union.str = (char*)this->encoder_version.c_str();
	this->properties->set(PROP_META_ENCODER_VER, default_union,  TYPES::UINT32(this->encoder_version.size()));
	default_union.str = (char*)this->generator_name.c_str();
	this->properties->set(PROP_META_GENERATOR_NAME, default_union,  TYPES::UINT32(this->generator_name.size()));
	default_union.str = (char*)this->generator_version.c_str();
	this->properties->set(PROP_META_GENERATOR_VER, default_union,  TYPES::UINT32(this->generator_version.size()));
}

MetaData::~MetaData() 
{
}

TYPES::state MetaData::validate_block_state()
{
	return TYPES::state::VALID;
}
void
MetaData::override_encoder_metadata(std::string& name, std::string& version)
{
	this->encoder_name = name;
	this->encoder_version = version;
}
void
MetaData::set_generator_metadata(std::string& name, std::string& version)
{
	this->generator_name = name;
	this->generator_version = version;
}

result
MetaData::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
    
}

TYPES::UINT32
MetaData::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	return this->calc_attr_length(true, false, settings);
}

result
MetaData::write_body(FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
	result res = result::AWD_SUCCESS;
	res = this->properties->write_attributes(fileWriter, settings);
	return res;
}

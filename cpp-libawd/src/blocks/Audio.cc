#include "blocks/audio.h"
#include <string>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

Audio::Audio(std::string& name) :
	AWDBlock(block_type::SOUND_SOURCE, name),
	
	AttrElementBase(),
	DataBlockBase(name)
{
}
Audio::Audio():
	AWDBlock(BLOCK::block_type::SOUND_SOURCE),
	AttrElementBase(),
	DataBlockBase("")
{
}

Audio::~Audio()
{
}

TYPES::state Audio::validate_block_state()
{
	return TYPES::state::VALID;

}
TYPES::UINT32
Audio::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len=0;

	len = TYPES::UINT16(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	len += TYPES::UINT8(sizeof(TYPES::UINT8)); //save type (external/embbed)

	len+=this->calc_bytes_length(settings);

	len += this->calc_attr_length(true, true, settings);

	return len;
}


result
Audio::collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type)
{
	// Do nothing
	return result::AWD_SUCCESS;
}



result
Audio::write_body(FileWriter * fileWriter, BlockSettings * settings, FILES::AWDFile* file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	
	fileWriter->writeUINT8(TYPES::UINT8(this->get_storage_type()));
	
	this->write_bytes(fileWriter, settings, file);

	this->properties->write_attributes(fileWriter,  settings);
	this->user_attributes->write_attributes(fileWriter,  settings);
	return result::AWD_SUCCESS;
}

#include "blocks/ConcanetedStream.h"
#include "utils/awd_types.h"

#include "base/block.h"
#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"
#include "utils/util.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::ATTR;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;

ConcanetedStream::ConcanetedStream(std::string& name) :
	AWDBlock(block_type::CONCANETEDSTREAM, name),	
	AttrElementBase(),
	DataBlockBase(name)
{
}
ConcanetedStream::ConcanetedStream():
	AWDBlock(BLOCK::block_type::CONCANETEDSTREAM),
	AttrElementBase(),
	DataBlockBase("")
{
}
ConcanetedStream::~ConcanetedStream()
{
}

result
ConcanetedStream::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	return result::AWD_SUCCESS;
}

TYPES::state
ConcanetedStream::validate_block_state()
{
	return TYPES::state::VALID;
}


TYPES::UINT32
ConcanetedStream::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	int len;
	len = sizeof(TYPES::UINT8);//hasSceneInfos
	len += this->calc_attr_length(true,true, settings);
	return len;
}

result
ConcanetedStream::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}


#include "blocks/command.h"
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

CommandBlock::CommandBlock(std::string& name) :
	SceneBlockBase(block_type::COMMAND, name, NULL)
{
	this->commandProperties = new NumAttrList();
	this->hasSceneInfos=true;
}
CommandBlock::CommandBlock():
	SceneBlockBase(block_type::COMMAND, "", NULL)
{
}
CommandBlock::~CommandBlock()
{
	delete this->commandProperties;
}

result
CommandBlock::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	return result::AWD_SUCCESS;
}

TYPES::state
CommandBlock::validate_block_state()
{
	return TYPES::state::VALID;
}

void
CommandBlock::add_target_light(BADDR targetValue)
{
	union_ptr newVal;
	newVal.v = (BADDR *)malloc(sizeof(BADDR));
	*newVal.ui32 = targetValue;
	//property id for the target light is 1
	//this->commandProperties->set(1, newVal, sizeof(BADDR), data_types::BADDR);
}

TYPES::UINT32
CommandBlock::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	int len;
	len = sizeof(TYPES::UINT8);//hasSceneInfos
	len += this->calc_common_length(settings->get_wide_matrix());
	len += sizeof(TYPES::UINT16);//num commands
	len += sizeof(TYPES::UINT16);//command type (command1)
	len +=this->commandProperties->calc_length(settings);
	len += this->calc_attr_length(true,true, settings);
	return len;
}

result
CommandBlock::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{
	int hasSceneInfos=this->hasSceneInfos;
	fileWriter->writeUINT8(hasSceneInfos);
	this->write_scene_common(fileWriter, settings, file);
	int numCommands=1;
	fileWriter->writeUINT16(numCommands);
	int commandtype=1;
	fileWriter->writeUINT16(commandtype);
	this->commandProperties->write_attributes(fileWriter, settings);
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}


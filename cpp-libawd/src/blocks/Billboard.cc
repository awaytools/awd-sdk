#include "blocks/billboard.h"

#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"


using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

Billboard::Billboard(std::string& name) :
	AWDBlock(BLOCK::BILLBOARD, name),
	AttrElementBase()
{
	this->material = NULL;
}

Billboard::Billboard():
	AWDBlock(BLOCK::BILLBOARD, ""),
	AttrElementBase()
{
	this->material = NULL;
}
Billboard::Billboard(std::string& name, TYPES::F64* mtx) :
	AWDBlock(BLOCK::MESH_INSTANCE_2, name),
	AttrElementBase()
{
	this->material = NULL;
}

Billboard::~Billboard()
{
	this->material = NULL;
}


TYPES::state Billboard::validate_block_state()
{
	return TYPES::state::VALID;
}

AWDBlock *
Billboard::get_material()
{
	return this->material;
}
void
Billboard::set_material(AWDBlock *material)
{
	this->material=material;
}

TYPES::UINT32
Billboard::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	TYPES::UINT32 length = sizeof(TYPES::UINT16) + this->get_name().size();//name
	length += sizeof(BADDR);//mat address
	length += this->calc_attr_length(true, true, settings);
	return length;
}

result
Billboard::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	
	if (this->material != NULL)
		this->material->add_with_dependencies(target_file, instance_type);
	
	return result::AWD_SUCCESS;
}

result
Billboard::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{	
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);

	TYPES::UINT32 block_addr=0;
	if (this->material != NULL)
		this->material->get_block_addr(file, block_addr);
	fileWriter->writeUINT32(block_addr);
	
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	
	return result::AWD_SUCCESS;
}

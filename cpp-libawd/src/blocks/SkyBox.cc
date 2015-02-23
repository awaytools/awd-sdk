#include "blocks/skybox.h"
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
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

SkyBox::SkyBox(std::string& name) :
	SceneBlockBase(block_type::SKYBOX, name, NULL)
{
	this->cubeTex = NULL;
}
SkyBox::SkyBox() :
	SceneBlockBase(block_type::SKYBOX, "", NULL)
{
	this->cubeTex = NULL;
}
SkyBox::~SkyBox()
{
	this->cubeTex = NULL;
}
TYPES::state SkyBox::validate_block_state()
{
	return TYPES::state::VALID;
}
void
SkyBox::set_cube_tex(AWDBlock * newCubeTex)
{
	this->cubeTex = newCubeTex;
}
AWDBlock *
SkyBox::get_cube_tex()
{
	return this->cubeTex;
}

result
SkyBox::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	if (this->cubeTex!=NULL) {
		this->cubeTex->add_with_dependencies(target_file, instance_type);
	}
	return result::AWD_SUCCESS;
}
TYPES::UINT32
SkyBox::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	TYPES::UINT32 len;
	len = TYPES::UINT32(sizeof(TYPES::UINT16)+this->get_name().size());//name
	len += sizeof(BADDR);//cube Tex
	len += this->calc_attr_length(true,true, settings);
	return len;
}

result
SkyBox::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	int cubeTex=0;
	/*if (this->cubeTex!=NULL) {
		cubeTex=this->cubeTex->get_addr();
	}*/
	fileWriter->writeUINT32(cubeTex);
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

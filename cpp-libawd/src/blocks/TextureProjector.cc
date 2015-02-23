#include "blocks/texture_projector.h"
#include <string>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::CAMERA;
TextureProjector::TextureProjector(std::string& name, TYPES::F64 * mtx) :
	SceneBlockBase(block_type::TEXTURE_PROJECTOR, name, mtx)
{
	this->texture = NULL;
	this->field_of_view = 60.0;
	this->aspect_ratio = 20.0;
}
TextureProjector::TextureProjector() :
	SceneBlockBase(block_type::TEXTURE_PROJECTOR, "", NULL)
{
	this->texture = NULL;
	this->field_of_view = 60.0;
	this->aspect_ratio = 20.0;
}

TextureProjector::~TextureProjector()
{
}

TYPES::state TextureProjector::validate_block_state()
{
	return TYPES::state::VALID;
}
TYPES::F64
TextureProjector::get_field_of_view()
{
	return this->field_of_view;
}

void
TextureProjector::set_field_of_view(TYPES::F64 field_of_view)
{
	this->field_of_view = field_of_view;
}

TYPES::F64
TextureProjector::get_aspect_ratio()
{
	return this->aspect_ratio;
}

void
TextureProjector::set_aspect_ratio(TYPES::F64 aspect_ratio)
{
	this->aspect_ratio = aspect_ratio;
}
AWDBlock *
TextureProjector::get_texture()
{
	return this->texture;
}
void
TextureProjector::set_texture(AWDBlock * texture)
{
	this->texture = texture;
}

result
TextureProjector::collect_dependencies(AWDFile *file, BLOCK::instance_type instance_type)
{
	if (this->texture != NULL)
		this->texture->add_with_dependencies(file, instance_type);
	return result::AWD_SUCCESS;
}

TYPES::UINT32
TextureProjector::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	TYPES::UINT32 len;

	len = this->calc_common_length(settings->get_wide_matrix());
	len += sizeof(TYPES::UINT32) ;//texture
	len += sizeof(TYPES::F32) ;//aspect ratio
	len += sizeof(TYPES::F32) ;//field of view
	len += this->calc_attr_length(true, true, settings);

	return len;
}

result
TextureProjector::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
{
	/*this->write_scene_common(fileWriter, settings);
	int texture_addr = 0;
	if (this->texture != NULL)
		texture_addr = this->texture->get_addr();
	fileWriter->writeUINT32(texture_addr);

	TYPES::F32 aspectRatio=TYPES::F32(this->aspect_ratio);//unused in the parser atm
	fileWriter->writeFLOAT32(aspectRatio);
	TYPES::F32 field_of_view=TYPES::F32(this->field_of_view);//unused in the parser atm
	fileWriter->writeFLOAT32(field_of_view);

	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter,  settings);*/
	return result::AWD_SUCCESS;
}

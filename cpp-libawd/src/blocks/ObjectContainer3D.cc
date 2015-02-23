#include "blocks/objectcontainer.h"
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
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

ObjectContainer3D::ObjectContainer3D(const std::string& name) :
	SceneBlockBase(block_type::CONTAINER, name, NULL)
{
}
ObjectContainer3D::ObjectContainer3D(const std::string& name, TYPES::F64 *mtx) :
	SceneBlockBase(block_type::CONTAINER, name, mtx)
{
}
ObjectContainer3D::ObjectContainer3D():
	SceneBlockBase(block_type::CONTAINER, "", NULL)
{
}
TYPES::state ObjectContainer3D::validate_block_state()
{
	return TYPES::state::VALID;
}

result
ObjectContainer3D::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
}

ObjectContainer3D::~ObjectContainer3D()
{
}

TYPES::UINT32
ObjectContainer3D::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	return this->calc_common_length(settings->get_wide_matrix()) + this->calc_attr_length(true,true, settings);
}

result
ObjectContainer3D::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{
	result res = this->write_scene_common(fileWriter, settings, file);
	if (res != result::AWD_SUCCESS)
		return res;
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

#include "blocks/scene.h"
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

Scene::Scene(std::string& name) :
	SceneBlockBase(block_type::SCENE, name, NULL)
{
}

Scene::Scene():
	SceneBlockBase(block_type::SCENE, "", NULL)
{
}
Scene::~Scene()
{
}

TYPES::state Scene::validate_block_state()
{
	return TYPES::state::VALID;
}
TYPES::UINT32
Scene::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	return this->calc_common_length(settings->get_wide_matrix()) +
		this->calc_attr_length(true,true, settings);
}

result
Scene::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
{
	this->write_scene_common(fileWriter, settings, file);
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

#include "blocks/light.h"

#include <string>
#include <vector>
#include "utils/awd_properties.h"
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::ATTR;
using namespace AWD::LIGHTS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

Light::Light(std::string& name) :
	SceneBlockBase(block_type::LIGHT, name, NULL)
{
	this->type = LIGHTS::type::UNDEFINED;
	this->radius = 90000;
	this->falloff = 100000;
	this->color = 0;
	this->specular = 0;
	this->diffuse = 0;
	this->ambientColor = 0;
	this->ambient = 0;
	this->dirX = 0;
	this->dirY = 0;
	this->dirZ = 0;
	this->shadowMethod = NULL;
	this->shadowmapper_type = LIGHTS::shadow_mapper_type::UNDEFINED_SHADOW;
	this->shadowmapper_depth_size = 0;
	this->shadowmapper_coverage = 1.0;
	this->shadowmapper_cascades = 3;
}
Light::Light():
	SceneBlockBase(block_type::LIGHT, "", NULL)
{
	this->type = LIGHTS::type::UNDEFINED;
	this->radius = 90000;
	this->falloff = 100000;
	this->color = 0;
	this->specular = 0;
	this->diffuse = 0;
	this->ambientColor = 0;
	this->ambient = 0;
	this->dirX = 0;
	this->dirY = 0;
	this->dirZ = 0;
	this->shadowMethod = NULL;
	this->shadowmapper_type = LIGHTS::shadow_mapper_type::UNDEFINED_SHADOW;
	this->shadowmapper_depth_size = 0;
	this->shadowmapper_coverage = 1.0;
	this->shadowmapper_cascades = 3;
}
Light::~Light(){
}
TYPES::state Light::validate_block_state()
{
	return TYPES::state::VALID;
}
void
Light::set_shadowmapper_type(shadow_mapper_type mapper_type)
{
	this->shadowmapper_type=mapper_type;
}
shadow_mapper_type
Light::get_shadowmapper_type()
{
	return this->shadowmapper_type;
}
void
Light::set_shadowmapper_depth_size(int depth_size)
{
	this->shadowmapper_depth_size=depth_size;
}
int
Light::get_shadowmapper_depth_size()
{
	return this->shadowmapper_depth_size;
}
void
Light::set_shadowmapper_coverage(TYPES::F64 coverage)
{
	this->shadowmapper_coverage=coverage;
}
TYPES::F64
Light::get_shadowmapper_coverage()
{
	return this->shadowmapper_coverage;
}
void
Light::set_shadowmapper_cascades(int cascades)
{
	this->shadowmapper_cascades=cascades;
}
int
Light::get_shadowmapper_cascades()
{
	return this->shadowmapper_cascades;
}
result
Light::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	
	if (this->shadowmapper_type != shadow_mapper_type::UNDEFINED_SHADOW) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::UINT8));
		*col_val.ui8 = TYPES::UINT8(this->shadowmapper_type);
	//	this->properties->set(PROP_LIGHT_SHADOWMAPPER, col_val, sizeof(TYPES::UINT8), data_types::UINT8);
		if(this->shadowmapper_type == shadow_mapper_type::CUBE_SHADOW){
			if(this->shadowmapper_depth_size!=1){
				union_ptr newVal;
				newVal.v = malloc(sizeof(TYPES::UINT8));
				*newVal.ui8 = this->shadowmapper_depth_size;
				//this->properties->set(PROP_LIGHT_SHADOWMAPPER_DEPTHMAPSIZE, newVal, sizeof(TYPES::UINT8), data_types::UINT8);
			}
		}
		else{
			if(this->shadowmapper_depth_size!=2){
				union_ptr newVal;
				newVal.v = malloc(sizeof(TYPES::UINT8));
				*newVal.ui8 = this->shadowmapper_depth_size;
				//this->properties->set(PROP_LIGHT_SHADOWMAPPER_DEPTHMAPSIZE, newVal, sizeof(TYPES::UINT8), data_types::UINT8);
			}
			if(this->shadowmapper_type == shadow_mapper_type::CASCADE_SHADOW){
				if(this->shadowmapper_cascades!=3){
					union_ptr newVal;
					newVal.v = malloc(sizeof(TYPES::UINT16));
					*newVal.ui16 = this->shadowmapper_cascades;
					//this->properties->set(PROP_LIGHT_SHADOWMAPPER_NUMCASCADES, newVal, sizeof(TYPES::UINT16), data_types::UINT16);
				}
			}
			else if(this->shadowmapper_type == shadow_mapper_type::NEAR_SHADOW){
				if(this->shadowmapper_coverage!=1.0){
					union_ptr col_val;
					col_val.v = malloc(sizeof(TYPES::F64));
					*col_val.F64 = this->shadowmapper_coverage;
					//this->properties->set(PROP_LIGHT_SHADOWMAPPER_COVERAGE, col_val, sizeof(TYPES::F64), data_types::FLOAT64);
				}
			}
		}
	}
	if (this->radius != 90000) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->radius;
		//this->properties->set(PROP_LIGHT_RADIUS, col_val, sizeof(TYPES::F64), data_types::FLOAT64);
	}
	if (this->falloff != 100000) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->falloff;
		//this->properties->set(PROP_LIGHT_FALLOFF, col_val, sizeof(TYPES::F64), data_types::FLOAT64);
	}
	if (this->color > 0xffffff) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::UINT32));
		*col_val.col = this->color;
		//this->properties->set(PROP_LIGHT_COLOR, col_val, sizeof(COLOR), data_types::COLOR);
	}
	if (this->specular != 1.0) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->specular;
		//this->properties->set(PROP_LIGHT_SPECULAR, col_val, sizeof(TYPES::F64), data_types::FLOAT64);
	}
	if (this->diffuse != 1.0) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->diffuse;
		//this->properties->set(PROP_LIGHT_DIFFUSE, col_val, sizeof(TYPES::F64), data_types::FLOAT64);
	}
			//if (this->ambientColor != 0xffffff) {
	TYPES::union_ptr col_val;
	col_val.v = malloc(sizeof(TYPES::UINT32));
	*col_val.col = this->ambientColor;
	//this->properties->set(PROP_LIGHT_AMBIENT_COLOR, col_val, sizeof(COLOR), data_types::COLOR);
			//}
	if (this->ambient != 0) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->ambient;
		//this->properties->set(PROP_LIGHT_AMBIENT, col_val, sizeof(TYPES::F64), data_types::FLOAT64);
	}
	if (this->dirX != 0) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->dirX;
		//this->properties->set(PROP_LIGHT_DIRX, col_val, sizeof(TYPES::F64), data_types::FLOAT64, 1);
	}
	if (this->dirY != 0) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->dirY;
		//this->properties->set(PROP_LIGHT_DIRY, col_val, sizeof(TYPES::F64), data_types::FLOAT64, 1);
	}
	if (this->dirZ != 0) {
		union_ptr col_val;
		col_val.v = malloc(sizeof(TYPES::F64));
		*col_val.F64 = this->dirZ;
		//this->properties->set(PROP_LIGHT_DIRZ, col_val, sizeof(TYPES::F64), data_types::FLOAT64, 1);
	}
	// TODO: Set all properties
	return result::AWD_SUCCESS;
}
TYPES::UINT32
Light::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	// Length is a single TYPES::UINT8 (light type) and common fields
	return sizeof(TYPES::UINT8) + this->calc_common_length(settings->get_wide_matrix()) + this->calc_attr_length(true, true, settings);
}
void
Light::set_directionVec(TYPES::F64 dirX, TYPES::F64 dirY, TYPES::F64 dirZ)
{
	this->dirX = dirX;
	this->dirY = dirY;
	this->dirZ = dirZ;
}
void
Light::set_shadowMethod(AWDBlock * newShadowMethod)
{
	this->shadowMethod = newShadowMethod;
}
AWDBlock *
Light::get_shadowMethod()
{
	return this->shadowMethod;
}
void
Light::set_light_type(LIGHTS::type type)
{
	this->type = type;
}
LIGHTS::type
Light::get_light_type()
{
	return this->type;
}
void
Light::set_radius(TYPES::F64 radius)
{
	this->radius = radius;
}
TYPES::F64
Light::get_radius()
{
	return this->radius;
}
void
Light::set_falloff(TYPES::F64 falloff)
{
	this->falloff = falloff;
}
TYPES::F64
Light::get_falloff()
{
	return this->falloff;
}
void
Light::set_color(COLOR newColor)
{
	this->color = newColor;
}
COLOR
Light::get_color()
{
	return this->color;
}
void
Light::set_specular(TYPES::F64 specular)
{
	this->specular = specular;
}
TYPES::F64
Light::get_specular()
{
	return this->specular;
}
void
Light::set_diffuse(TYPES::F64 diffuse)
{
	this->diffuse = diffuse;
}
TYPES::F64
Light::get_diffuse()
{
	return this->diffuse;
}
void
Light::set_ambientColor(COLOR ambientColor)
{
	this->ambientColor = ambientColor;
}
COLOR
Light::get_ambientColor()
{
	return this->ambientColor;
}
void
Light::set_ambient(TYPES::F64 ambient)
{
	this->ambient = ambient;
}
TYPES::F64
Light::get_ambient()
{
	return this->ambient;
}
result
Light::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
{
	this->write_scene_common(fileWriter, settings, file);
	fileWriter->writeUINT8(TYPES::UINT8(this->type));
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}



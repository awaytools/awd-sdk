#include "platform.h"
#include "light.h"
#include "util.h"


AWDLight::AWDLight(const char *name, awd_uint16 name_len) :
    AWDSceneBlock(LIGHT, name, name_len, NULL)
{
	this->type = AWD_LIGHT_UNDEFINED;
    this->radius = 0;
    this->falloff = 0;
    this->color = 0;
    this->specular = 0;
    this->diffuse = 0;
    this->ambientColor = 0;
    this->ambient = 0;
    this->dirX = 0;
    this->dirY = 0;
    this->dirZ = 0;
    this->shadowMethod = NULL;
}
AWDLight::~AWDLight(){
}
void
AWDLight::prepare_and_add_dependencies(AWDBlockList *export_list)
{
		
	if (this->radius != 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->radius;
		this->properties->set(PROP_LIGHT_RADIUS, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
	if (this->falloff != 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->falloff;
		this->properties->set(PROP_LIGHT_FALLOFF, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
	if (this->color > 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_uint32));
        *col_val.col = this->color;
		this->properties->set(PROP_LIGHT_COLOR, col_val, sizeof(awd_color), AWD_FIELD_COLOR);
    }
	if (this->specular != 1.0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->specular;
		this->properties->set(PROP_LIGHT_SPECULAR, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
	if (this->diffuse != 1.0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->diffuse;
		this->properties->set(PROP_LIGHT_DIFFUSE, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
	if (this->ambientColor > 0.0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_uint32));
        *col_val.col = this->ambientColor;
		this->properties->set(PROP_LIGHT_AMBIENT_COLOR, col_val, sizeof(awd_color), AWD_FIELD_COLOR);
    }
	if (this->ambient != 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->ambient;
		this->properties->set(PROP_LIGHT_AMBIENT, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }




	if (this->dirX != 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->dirX;
		this->properties->set(PROP_LIGHT_DIRX, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
	if (this->dirY != 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->dirY;
		this->properties->set(PROP_LIGHT_DIRY, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
	if (this->dirZ != 0) {
		AWD_field_ptr col_val;
        col_val.v = malloc(sizeof(awd_float32));
        *col_val.f32 = this->dirZ;
		this->properties->set(PROP_LIGHT_DIRZ, col_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
    }
    // TODO: Set all properties
}


awd_uint32
AWDLight::calc_body_length(BlockSettings * curBlockSettings)
{
    // Length is a single awd_uint8 (light type) and common fields
    return sizeof(awd_uint8) + this->calc_common_length(curBlockSettings->get_wide_matrix()) + this->calc_attr_length(true, true, curBlockSettings->get_wide_matrix());
}

void
AWDLight::set_directionVec(awd_float32 dirX, awd_float32 dirY, awd_float32 dirZ)
{
    this->dirX = dirX;
    this->dirY = dirY;
    this->dirZ = dirZ;
}
void
AWDLight::set_shadowMethod(AWDBlock * newShadowMethod)
{
    this->shadowMethod = newShadowMethod;
}
AWDBlock *
AWDLight::get_shadowMethod()
{
    return this->shadowMethod;
}
void
AWDLight::set_light_type(AWD_light_type light_type)
{
    this->type = light_type;
}
AWD_light_type
AWDLight::get_light_type()
{
    return this->type;
}
void
AWDLight::set_radius(awd_float32 radius)
{
    this->radius = radius;
}
awd_float32
AWDLight::get_radius()
{
    return this->radius;
}
void
AWDLight::set_falloff(awd_float32 falloff)
{
    this->falloff = falloff;
}
awd_float32
AWDLight::get_falloff()
{
    return this->falloff;
}
void
AWDLight::set_color(awd_color newColor)
{
    this->color = newColor;
}
awd_color
AWDLight::get_color()
{
    return this->color;
}
void
AWDLight::set_specular(awd_float32 specular)
{
    this->specular = specular;
}
awd_float32
AWDLight::get_specular()
{
    return this->specular;
}
void
AWDLight::set_diffuse(awd_float32 diffuse)
{
    this->diffuse = diffuse;
}
awd_float32
AWDLight::get_diffuse()
{
    return this->diffuse;
}
void
AWDLight::set_ambientColor(awd_color ambientColor)
{
    this->ambientColor = ambientColor;
}
awd_color
AWDLight::get_ambientColor()
{
    return this->ambientColor;
}
void
AWDLight::set_ambient(awd_float32 ambient)
{
    this->ambient = ambient;
}
awd_float32
AWDLight::get_ambient()
{
    return this->ambient;
}

void
AWDLight::write_body(int fd, BlockSettings * curBlockSettings)
{
    this->write_scene_common(fd, curBlockSettings);
    write(fd, &this->type, sizeof(awd_uint8));
    this->properties->write_attributes(fd, curBlockSettings->get_wide_matrix());
    this->user_attributes->write_attributes(fd, curBlockSettings->get_wide_matrix());
}


AWDLightPicker::AWDLightPicker(const char *name, awd_uint16 name_len) :
    AWDBlock(LIGHTPICKER),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
	this->lights = new AWDBlockList();
}
AWDLightPicker::~AWDLightPicker(){
	if(this->lights!=NULL)
		delete this->lights;
}
void
AWDLightPicker::set_lights(AWDBlockList * newLights)
{
	if(this->lights!=NULL)
		delete this->lights;
	this->lights = newLights;
}
AWDBlockList *
AWDLightPicker::get_lights()
{
	 return this->lights;
}
AWDBlock *
AWDLightPicker::get_shadowLight()
{
	AWDLight * awdLight=NULL;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->lights);
    while ((awdLight = (AWDLight*)it->next()) != NULL) {
		if (awdLight->get_shadowMethod()!=NULL)			
			return awdLight;
	}
	delete it;
    return NULL;
}

void
AWDLightPicker::prepare_and_add_dependencies(AWDBlockList *export_list)
{
	
    AWDBlock *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->lights);
    while ((block = it->next()) != NULL) 
		block->prepare_and_add_with_dependencies(export_list);
	delete it;
    // TODO: Set all properties
}


awd_uint32
AWDLightPicker::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;
    len = sizeof(awd_uint16) + this->get_name_length();
    len += sizeof(awd_uint16); //num lights
	len += this->lights->get_num_blocks() * sizeof(awd_baddr); //light IDs
    len += this->calc_attr_length(false,true, curBlockSettings->get_wide_matrix());
	return len;
}


void
AWDLightPicker::write_body(int fd, BlockSettings * curBlockSettings)
{
	awd_uint16 lightLength=this->lights->get_num_blocks();
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &lightLength, sizeof(awd_uint16));
    // Write animSet id - TODO-> ERROR IF NULL
	AWDBlockIterator * it;
	AWDLight * block;
    it = new AWDBlockIterator(this->lights);
    while ((block = (AWDLight *)it->next()) != NULL) {
        awd_baddr addr = UI32(block->get_addr());
        write(fd, &addr, sizeof(awd_baddr));
    }
	delete it;
    this->user_attributes->write_attributes(fd, curBlockSettings->get_wide_matrix());
}
#include "blocks/material.h"

#include <vector>
#include "utils/awd_types.h"
#include "utils/awd_properties.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "blocks/bitmap_texture.h"
#include "elements/shading.h"
#include "blocks/light.h"
#include "blocks/light_picker.h"

#include "blocks/effect_method.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

Material::Material(std::string& name) :
	AWDBlock(block_type::SIMPLE_MATERIAL, name),
	
	AttrElementBase()
{
	this->material_type = MATERIAL::type::UNDEFINED;
	this->isCreated = false;//will be true, once the mtl is converted to awd
	this->isClone = false;
	this->isDefault = true;
	this->color = 0xffffff;


	
	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::COLOR));
	*default_union.col=0xcccccc;
	this->properties->add(PROP_MAT_COLOR,	default_union, 4,   data_types::COLOR, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::BADDR));
	*default_union.addr=0;
	this->properties->add(PROP_MAT_TEXTURE,	default_union, 4,   data_types::BADDR, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::UPDATED_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_MAT_ALPHA,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);

	this->texture = NULL;
	this->normalTexture = NULL;
	this->multiPass = false;
	this->repeat = true;
	this->smooth = true;
	this->mipmap = false;
	this->both_sides = false;
	this->premultiplied = false;
	this->alpha_blending = false;
	this->blendMode = 0;
	this->alpha = 1.0;
	this->alpha_threshold = 0.0f;
	this->ambientStrength = 1.0f;
	this->ambientColor = 0xffffff;
	this->ambientTexture = NULL;
	this->specularStrength = 1.0f;
	this->glossStrength = 50;
	this->specularColor = 0xffffff;
	this->specTexture = NULL;
	this->lightPicker = NULL;
	this->animator = NULL;
	this->shadowMethod = NULL;

	this->uv_transform_mtx = NULL;
}

Material::Material():
	AWDBlock(block_type::SIMPLE_MATERIAL),
	AttrElementBase()
{
	this->material_type = MATERIAL::type::UNDEFINED;
	this->isCreated = false;//will be true, once the mtl is converted to awd
	this->isClone = false;
	this->isDefault = true;

	this->color = 0xffffff;
	this->color_components = GEOM::VECTOR4D (0.5, 0.5, 0.5, 1.0);

	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::COLOR));
	*default_union.col=0xcccccc;
	this->properties->add(PROP_MAT_COLOR,	default_union, 4,   data_types::COLOR, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::BADDR));
	*default_union.addr=0;
	this->properties->add(PROP_MAT_TEXTURE,	default_union, 4,   data_types::BADDR, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::UPDATED_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_MAT_ALPHA,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);

	this->texture = NULL;
	this->normalTexture = NULL;
	this->multiPass = false;
	this->repeat = true;
	this->smooth = true;
	this->mipmap = false;
	this->both_sides = false;
	this->premultiplied = false;
	this->alpha_blending = false;
	this->blendMode = 0;
	this->alpha = 1.0;
	this->alpha_threshold = 0.0f;
	this->ambientStrength = 1.0f;
	this->ambientColor = 0xffffff;
	this->ambientTexture = NULL;
	this->specularStrength = 1.0f;
	this->glossStrength = 50;
	this->specularColor = 0xffffff;
	this->specTexture = NULL;
	this->lightPicker = NULL;
	this->animator = NULL;
	this->shadowMethod = NULL;

	this->uv_transform_mtx = NULL;
}
Material::~Material()
{
	/*
	if(!this->isClone){
		for(ShadingMethod * shadingMethod : this->shadingMethods){
			delete shadingMethod;
		}
	}
	this->texture = NULL;
	this->ambientTexture = NULL;
	this->specTexture = NULL;
	this->normalTexture = NULL;

	this->type = type::UNDEFINED;
	if (this->uv_transform_mtx!=NULL) {
		free(this->uv_transform_mtx);
		this->uv_transform_mtx = NULL;
	}
	*/
}

TYPES::state Material::validate_block_state()
{
	return TYPES::state::VALID;
}

SETTINGS::BlockSettings*
Material::get_sub_geom_settings()
{
	return this->sub_geom_settings;
}

void
Material::set_sub_geom_settings(SETTINGS::BlockSettings* sub_geom_settings)
{
	this->sub_geom_settings = sub_geom_settings;
}

void
Material::resetShadingMethods()
{
	for(ShadingMethod * shadingMethod : this->shadingMethods){
		delete shadingMethod;
	}
}

MATERIAL::type
Material::get_material_type()
{
	return this->material_type;
}

void
Material::set_material_type(MATERIAL::type material_type)
{
	this->material_type = material_type;
}
		
void 
Material::set_color_components(TYPES::F64 r,TYPES::F64 g, TYPES::F64 b, TYPES::F64 a)
{
	this->color_components = GEOM::VECTOR4D (r, g, b, a);
}

GEOM::VECTOR4D 
Material::get_color_components(){
	return this->color_components;
}
bool
Material::get_isDefault()
{
	return this->isDefault;
}
void
Material::set_isDefault(bool isDefault)
{
	this->isDefault = isDefault;
}
bool
Material::get_isClone()
{
	return this->isClone;
}

void
Material::set_isClone(bool isClone)
{
	this->isClone = isClone;
}

std::vector<Material*>&
Material::get_materialClones()
{
	return this->materialClones;
}
std::vector<EffectMethod*>
Material::get_effectMethods()
{
	return this->effectMethods;
}

void
Material::set_effectMethods(std::vector<EffectMethod*> effectMethods)
{
	this->effectMethods = effectMethods;
}

bool
Material::get_isCreated()
{
	return this->isCreated;
}
void
Material::set_isCreated(bool isCreated)
{
	this->isCreated = isCreated;
}
GEOM::MATRIX2x3*
Material::get_uv_transform_mtx()
{
	return this->uv_transform_mtx;
}
void
Material::set_uv_transform_mtx(TYPES::F64 * new_uv_transform_mtx)
{
	this->uv_transform_mtx->set(new_uv_transform_mtx);
	/*
	if (this->uv_transform_mtx!=NULL)
		free(this->uv_transform_mtx);
	this->uv_transform_mtx = (double *)malloc(6*sizeof(double));
	this->uv_transform_mtx[0] = new_uv_transform_mtx[0];
	this->uv_transform_mtx[1] = new_uv_transform_mtx[1];
	this->uv_transform_mtx[2] = new_uv_transform_mtx[2];
	this->uv_transform_mtx[3] = new_uv_transform_mtx[3];
	this->uv_transform_mtx[4] = new_uv_transform_mtx[4];
	this->uv_transform_mtx[5] = new_uv_transform_mtx[5];
	*/
}


void
Material::add_method(ShadingMethod *method)
{
	this->shadingMethods.push_back(method);
}

COLOR
Material::get_color()
{
	return this->color;
}
void
Material::set_color(COLOR color)
{
	this->color = color;
}

BitmapTexture *
Material::get_texture()
{
	return this->texture;
}
void
Material::set_texture(BitmapTexture *texture)
{
	this->texture = texture;
}

BitmapTexture *
Material::get_normalTexture()
{
	return this->normalTexture;
}
void
Material::set_normalTexture(BitmapTexture *normalTexture)
{
	this->normalTexture = normalTexture;
}

bool
Material::get_multiPass()
{
	return this->multiPass;
}
void
Material::set_multiPass(bool multiPass)
{
	this->multiPass = multiPass;
}

bool
Material::get_smooth()
{
	return this->smooth;
}
void
Material::set_smooth(bool smooth)
{
	this->smooth = smooth;
}

bool
Material::get_mipmap()
{
	return this->mipmap;
}
void
Material::set_mipmap(bool mipmap)
{
	this->mipmap = mipmap;
}

bool
Material::get_both_sides()
{
	return this->both_sides;
}
void
Material::set_both_sides(bool both_sides)
{
	this->both_sides = both_sides;
}

bool
Material::get_premultiplied()
{
	return this->premultiplied;
}
void
Material::set_premultiplied(bool premultiplied)
{
	this->premultiplied = premultiplied;
}

int
Material::get_blendMode()
{
	return this->blendMode;
}
void
Material::set_blendMode(int blendMode)
{
	this->blendMode = blendMode;
}

TYPES::F32
Material::get_alpha()
{
	return this->alpha;
}
void
Material::set_alpha(TYPES::F32 alpha)
{
	this->alpha = alpha;
}

bool
Material::get_alpha_blending()
{
	return this->alpha_blending;
}
void
Material::set_alpha_blending(bool alpha_blending)
{
	this->alpha_blending = alpha_blending;
}

TYPES::F32
Material::get_alpha_threshold()
{
	return this->alpha_threshold;
}
void
Material::set_alpha_threshold(TYPES::F32 alpha_threshold)
{
	this->alpha_threshold = alpha_threshold;
}

bool
Material::get_repeat()
{
	return this->repeat;
}
void
Material::set_repeat(bool repeat)
{
	this->repeat = repeat;
}

TYPES::F32
Material::get_ambientStrength()
{
	return this->ambientStrength;
}
void
Material::set_ambientStrength(TYPES::F32 ambientStrength)
{
	this->ambientStrength = ambientStrength;
}

COLOR
Material::get_ambientColor()
{
	return this->ambientColor;
}
void
Material::set_ambientColor(COLOR ambientColor)
{
	this->ambientColor = ambientColor;
}

BitmapTexture *
Material::get_ambientTexture()
{
	return this->ambientTexture;
}
void
Material::set_ambientTexture(BitmapTexture *ambientTexture)
{
	this->ambientTexture = ambientTexture;
}

TYPES::F32
Material::get_specularStrength()
{
	return this->specularStrength;
}
void
Material::set_specularStrength(TYPES::F32 specularStrength)
{
	this->specularStrength = specularStrength;
}

TYPES::UINT16
Material::get_glossStrength()
{
	return this->glossStrength;
}
void
Material::set_glossStrength(TYPES::UINT16 glossStrength)
{
	this->glossStrength = glossStrength;
}

COLOR
Material::get_specularColor()
{
	return this->specularColor;
}
void
Material::set_specularColor(COLOR specularColor)
{
	this->specularColor = specularColor;
}

BitmapTexture *
Material::get_specTexture()
{
	return this->specTexture;
}
void
Material::set_specTexture(BitmapTexture *specTexture)
{
	this->specTexture = specTexture;
}

LightPicker *
Material::get_lightPicker()
{
	return this->lightPicker;
}

void
Material::set_lightPicker(LightPicker *lightPicker_block)
{
	this->lightPicker = lightPicker_block;
}

AWDBlock *
Material::get_animator()
{
	return this->animator;
}

void
Material::set_animator(AWDBlock *animator)
{
	this->animator = animator;
}

AWDBlock *
Material::get_shadowMethod()
{
	return this->shadowMethod;
}

void
Material::set_shadowMethod(AWDBlock *shadowMethod)
{
	this->shadowMethod = shadowMethod;
}
result
Material::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	
	if (this->texture) {
		this->texture->add_with_dependencies(target_file, instance_type);
	}
	/*
	if (this->type == type::COLOR) {
		this->add_color_property(PROP_MAT_COLOR,this->color,0xffffff);
	}
	else {
		if (this->ambientTexture) {
			if (this->ambientTexture->get_storage_type()!=storage_type::UNDEFINEDTEXTYPE){
				this->ambientTexture->add_with_dependencies(target_file, instance_type);
				union_ptr ambientTex_val;
				ambientTex_val.v = malloc(sizeof(BADDR));
				// *ambientTex_val.addr = this->ambientTexture->get_addr();
				this->properties->set(PROP_MAT_AMBIENTTEXTURE, ambientTex_val, sizeof(BADDR), data_types::BADDR);
			}
		}
	}
	*/
	// create wrapper-methods for all effectMethods (wrapper methods only consist of awdID thats pointing to the EffectMethods-AWDBlock)
	/*for(EffectMethod * effectMethodBlock : this->effectMethods){
		effectMethodBlock->add_with_dependencies(target_file, instance_type);
		ShadingMethod * newWrapperMethod = new ShadingMethod(AWD_EFFECTMETHOD_WRAPPER);
		union_ptr effectBlock_val;
		effectBlock_val.v = malloc(sizeof(BADDR));
		*effectBlock_val.addr = effectMethodBlock->get_addr();
		newWrapperMethod->get_shading_props()->set(PROPS_BADDR1, effectBlock_val, sizeof(BADDR), BADDR);
		this->add_method(newWrapperMethod);
	}
	
	for(ShadingMethod * shadingMethod : this->shadingMethods){
		shadingMethod->collect_dependencies(export_list);
	}*/
	/*
	if (this->shadowMethod!=NULL) {
		this->shadowMethod->add_with_dependencies(target_file, instance_type);
		ShadingMethod * newWrapperMethod = new ShadingMethod(shading_type::AWD_SHADOWMETHOD_WRAPPER);
		union_ptr shadowBlock_val;
		shadowBlock_val.v = malloc(sizeof(BADDR));
		// *shadowBlock_val.addr = this->shadowMethod->get_addr();
		newWrapperMethod->get_shading_props()->set(PROPS_BADDR1, shadowBlock_val, sizeof(BADDR), data_types::BADDR);
		this->add_method(newWrapperMethod);
	}
	// this properties can be set for texture and for color materials
	if (this->lightPicker!=NULL) {
		this->lightPicker->add_with_dependencies(target_file, instance_type);
		union_ptr lightPicker_val;
		lightPicker_val.v = malloc(sizeof(BADDR));
		// *lightPicker_val.addr = this->lightPicker->get_addr();
		this->properties->set(PROP_MAT_LIGHTPICKER, lightPicker_val, sizeof(BADDR), data_types::BADDR);
	}
	if (this->normalTexture) {
		if (this->normalTexture->get_storage_type()!=storage_type::UNDEFINEDTEXTYPE){
			this->normalTexture->add_with_dependencies(target_file, instance_type);
			union_ptr normaltex_val;
			normaltex_val.v = malloc(sizeof(BADDR));
			// *normaltex_val.addr = this->normalTexture->get_addr();
			this->properties->set(PROP_MAT_NORMALTEXTURE, normaltex_val, sizeof(BADDR), data_types::BADDR);
		}
	}
	if (this->specTexture) {
		if (this->specTexture->get_storage_type()!=storage_type::UNDEFINEDTEXTYPE){
			this->specTexture->add_with_dependencies(target_file, instance_type);
			union_ptr specTex_val;
			specTex_val.v = malloc(sizeof(BADDR));
			// *specTex_val.addr = this->specTexture->get_addr();
				this->properties->set(PROP_MAT_SPECULARTEXTURE, specTex_val, sizeof(BADDR), data_types::BADDR);
		}
	}
	this->add_bool_property(PROP_MAT_SPECIAL_ID,this->multiPass,false);
	this->add_bool_property(PROP_MAT_REPEAT,this->repeat,false);
	this->add_bool_property(PROP_MAT_SMOOTH,this->smooth,true);
	this->add_bool_property(PROP_MAT_MIPMAP,this->mipmap,true);
	this->add_bool_property(PROP_MAT_BOTHSIDES,this->both_sides,false);
	this->add_number_property(PROP_MAT_ALPHA,this->alpha,1.0f);
	this->add_bool_property(PROP_MAT_ALPHA_BLENDING,this->alpha_blending,false);
	this->add_number_property(PROP_MAT_ALPHA_THRESHOLD,this->alpha_threshold,0.0f);
	this->add_bool_property(PROP_MAT_APLHA_PREMULTIPLIED,this->premultiplied,false);
	this->add_int8_property(PROP_MAT_BLENDMODE,this->blendMode,0);
	this->add_color_property(PROP_MAT_SPECULARCOLOR,this->specularColor,0xffffff);
	this->add_color_property(PROP_MAT_AMBIENTCOLOR,this->ambientColor,0xffffff);
	this->add_number_property(PROP_MAT_SPECULARLEVEL,this->specularStrength,1.0f);
	this->add_number_property(PROP_MAT_AMBIENT_LEVEL,this->ambientStrength,1.0f);
	this->add_number_property(PROP_MAT_GLOSS,this->glossStrength,50);
	*/
	return result::AWD_SUCCESS;
}

Material*
Material::get_unique_material(LightPicker * lightPicker, AWDBlock * animator, std::vector<AWDBlock*>&  fx_blocks)
{
	if ((this->lightPicker==NULL)&&(this->animator==NULL)){//&&(this->effectMethods==NULL)){
		this->lightPicker=lightPicker;
		this->animator=animator;
		//this->effectMethods=fx_blocks;
		return this;
	}
	if ((this->lightPicker==lightPicker)&&(this->animator==animator)){
		return this;
	}
	for(Material * block : this->materialClones){
		if ((block->lightPicker==lightPicker)&&(block->animator==animator)){
			return block;
		}
	}
	Material * clonedMat = new Material(this->get_name());
	clonedMat->set_isClone(true);
	clonedMat->set_type(this->type);
	clonedMat->set_ambientTexture(this->ambientTexture);
	clonedMat->set_texture(this->texture);
	clonedMat->set_specTexture(this->specTexture);
	clonedMat->set_normalTexture(this->normalTexture);
	clonedMat->color=this->color;
	clonedMat->alpha_threshold=this->alpha_threshold;
	clonedMat->alpha_blending=this->alpha_blending;
	clonedMat->repeat=this->repeat;
	clonedMat->ambientColor=this->ambientColor;
	clonedMat->specularColor=this->specularColor;
	clonedMat->specularStrength=this->specularStrength;
	clonedMat->ambientStrength=this->ambientStrength;
	clonedMat->glossStrength=this->glossStrength;
	clonedMat->specularStrength=this->specularStrength;
	//clonedMat->effectMethods=this->effectMethods;
	for(ShadingMethod * shadingMethod : this->shadingMethods){
		//clonedMat->get_shadingMethods().push_back(shadingMethod);
	}
	clonedMat->set_multiPass(this->multiPass);
	clonedMat->set_shadowMethod(this->shadowMethod);
	if(animator!=NULL)
		clonedMat->set_animator(animator);
	if(lightPicker!=NULL)
		clonedMat->set_lightPicker(lightPicker);

	this->materialClones.push_back(clonedMat);
	return clonedMat;
}

TYPES::UINT32
Material::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	//at this point, we need to update the properties
	
	TYPES::union_ptr properties_union;
	properties_union.v=malloc(sizeof(TYPES::COLOR));
	*properties_union.col=this->color;
	this->properties->set(PROP_MAT_COLOR, properties_union, TYPES::UINT32(sizeof(TYPES::UINT32)));
	
	properties_union.v=malloc(sizeof(TYPES::F64));
	*properties_union.F64=this->alpha;
	this->properties->set(PROP_MAT_ALPHA, properties_union, TYPES::UINT32(sizeof(TYPES::F32)));

	if (this->texture) {
		if (this->texture->get_storage_type()!=storage_type::UNDEFINEDTEXTYPE){
			TYPES::UINT32 block_addr;
			this->texture->get_block_addr(awd_file, block_addr);
			union_ptr tex_val;
			tex_val.v = malloc(sizeof(TYPES::BADDR));
			*tex_val.addr = block_addr;
			this->properties->set(PROP_MAT_TEXTURE, tex_val, sizeof(BADDR));
		}
	}

	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	len += sizeof(TYPES::UINT8); // type
	len += sizeof(TYPES::UINT8); // method count

	len += this->calc_attr_length(true, true, settings);

	//at this point, all effectmethods should already be included as wrappermethods in the materials method-list
	for(ShadingMethod * shadingMethod : this->shadingMethods){
		len += shadingMethod->calc_method_length(settings);
	}
	return len;
}

result
Material::write_body(FileWriter * fileWriter, BlockSettings * settings, FILES::AWDFile* file)
{
	
	if(fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16)!=result::AWD_SUCCESS)
		return result::AWD_ERROR;
	if(fileWriter->writeUINT8(TYPES::UINT8(this->material_type))!=result::AWD_SUCCESS)
		return result::AWD_ERROR;
	if(fileWriter->writeUINT8(TYPES::UINT8(this->shadingMethods.size()))!=result::AWD_SUCCESS)
		return result::AWD_ERROR;
	
	this->properties->write_attributes(fileWriter, settings);
	
	for(ShadingMethod * shadingMethod : this->shadingMethods){
		shadingMethod->write_method(fileWriter, settings);
	}

	this->user_attributes->write_attributes(fileWriter, settings);
	
	return result::AWD_SUCCESS;
}

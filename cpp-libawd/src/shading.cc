#include "shading.h"
#include "util.h"

#include "platform.h"



inline void __set_tex_prop(AWDNumAttrList *properties, awd_uint16 prop, AWDBlock *tex)
{
    if (tex) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_baddr));
        *val.addr = tex->get_addr();
        properties->set(prop, val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
}



awd_uint32
AWDShadingMethod::calc_method_length(bool wide_mtx)
{
    return sizeof(awd_uint16) + this->calc_attr_length(true,true, wide_mtx);
}


void
AWDShadingMethod::write_method(int fd, bool wide_mtx)
{
    awd_uint16 type_be;

    type_be = UI16(this->type);
    write(fd, &type_be, sizeof(awd_uint16));
    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}




AWDEnvMapAmbientMethod::AWDEnvMapAmbientMethod()
{
    this->type = AWD_SHADETYPE_ENVMAP_AMB;
    this->cube_texture = NULL;
}

void
AWDEnvMapAmbientMethod::prepare_write()
{
    __set_tex_prop(properties, PROP_SHAD_ENVMAP_CUBETEX, this->cube_texture);
}




AWDColorMatrixMethod::AWDColorMatrixMethod()
{
    this->type = AWD_SHADETYPE_COLOR_MATRIX;
    this->mtx = NULL;
}

void
AWDColorMatrixMethod::prepare_write()
{
    if (this->mtx != NULL) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_float64) * 20);
        val.f64 = this->mtx;
        this->properties->set(PROP_SHAD_COLOR_MATRIX, val, sizeof(awd_float64)*20, AWD_FIELD_FLOAT64);
    }
}



AWDColorTransformMethod::AWDColorTransformMethod()
{
    this->type = AWD_SHADETYPE_COLOR_TRANSFORM;
    this->alpha_multiplier = 1.0;
    this->red_multiplier = 1.0;
    this->green_multiplier = 1.0;
    this->blue_multiplier = 1.0;
    this->color_offset = 0;
}

void
AWDColorTransformMethod::prepare_write()
{
    if (this->alpha_multiplier != 1.0) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_float64));
        *val.f64 = this->alpha_multiplier;
        this->properties->set(PROP_SHAD_ALPHA_MULT, val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }

    if (this->red_multiplier != 1.0) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_float64));
        *val.f64 = this->red_multiplier;
        this->properties->set(PROP_SHAD_RED_MULT, val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    
    if (this->green_multiplier != 1.0) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_float64));
        *val.f64 = this->green_multiplier;
        this->properties->set(PROP_SHAD_GREEN_MULT, val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }

    if (this->blue_multiplier != 1.0) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_float64));
        *val.f64 = this->blue_multiplier;
        this->properties->set(PROP_SHAD_BLUE_MULT, val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }

    if (this->color_offset != 0) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_color));
        *val.col = this->color_offset;
        this->properties->set(PROP_SHAD_COLOR_OFFS, val, sizeof(awd_float64), AWD_FIELD_COLOR);
    }
}




AWDEnvMapMethod::AWDEnvMapMethod()
{
    this->type = AWD_SHADETYPE_ENVMAP;
    this->cube_texture = NULL;
    this->alpha = 1.0;
}

void
AWDEnvMapMethod::prepare_write()
{
    if (this->alpha != 1.0) {
        AWD_field_ptr val;
        val.v = malloc(sizeof(awd_float64));
        *val.f64 = this->alpha;
        this->properties->set(PROP_SHAD_ALPHA, val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }

    __set_tex_prop(properties, PROP_SHAD_ENVMAP_CUBETEX, this->cube_texture);
}



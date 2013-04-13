#ifndef _LIBAWD_SHADING_H
#define _LIBAWD_SHADING_H

#include "texture.h"
#include "block.h"
#include "attr.h"

#define PROP_SHAD_STRENGTH 1
#define PROP_SHAD_COLOR 2
#define PROP_SHAD_ALPHA 3
#define PROP_SHAD_MAP 4

#define PROP_SHAD_ENVMAP_CUBETEX 101

#define PROP_SHAD_CEL_LEVELS 201
#define PROP_SHAD_CEL_SMOOTHNESS 202
#define PROP_SHAD_CEL_CUTOFF 203

#define PROP_SHAD_SCATTERING 301
#define PROP_SHAD_TRANSLUCENCY 302

#define PROP_SHAD_GLOSS 21
#define PROP_SHAD_GLOSS_MAP 22

#define PROP_SHAD_NORMAL_REFLECTANCE 401

#define PROP_SHAD_EPSILON 501
#define PROP_SHAD_STEP_SIZE 502

#define PROP_SHAD_COLOR_MATRIX 1001
#define PROP_SHAD_ALPHA_MULT 1101
#define PROP_SHAD_RED_MULT 1102
#define PROP_SHAD_GREEN_MULT 1103
#define PROP_SHAD_BLUE_MULT 1104
#define PROP_SHAD_COLOR_OFFS 1105

#define PROP_SHAD_FOG_DIST 1201

#define PROP_SHAD_PROJ_MODE 1301
#define PROP_SHAD_PROJ_TEXTURE 1302

#define PROP_SHAD_POWER 1401


typedef enum {
    AWD_SHADETYPE_ENVMAP_AMB=1,
    AWD_SHADETYPE_COLOR_MATRIX=401,
    AWD_SHADETYPE_COLOR_TRANSFORM,
    AWD_SHADETYPE_ENVMAP
} AWD_shade_type;

class AWDShadingMethod :
    public AWDAttrElement
{
    protected:
        AWD_shade_type type;
        virtual void prepare_write()=0;

    public:
        awd_uint32 calc_method_length(bool);
        void write_method(int, bool);
};


class AWDEnvMapAmbientMethod :
    public AWDShadingMethod
{
    protected:
        void prepare_write();

    public:
        AWDEnvMapAmbientMethod();

        AWDCubeTexture *cube_texture;
};



class AWDColorMatrixMethod :
    public AWDShadingMethod
{
    protected:
        void prepare_write();

    public:
        AWDColorMatrixMethod();

        awd_float64 *mtx;
};


class AWDColorTransformMethod :
    public AWDShadingMethod
{
    protected:
        void prepare_write();

    public:
        AWDColorTransformMethod();

        awd_float64 alpha_multiplier;
        awd_float64 red_multiplier;
        awd_float64 green_multiplier;
        awd_float64 blue_multiplier;
        awd_color color_offset;
};


class AWDEnvMapMethod :
    public AWDShadingMethod
{
    protected:
        void prepare_write();

    public:
        AWDEnvMapMethod();

        AWDCubeTexture *cube_texture;
        awd_float64 alpha;
};


#endif

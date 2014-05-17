#ifndef _LIBAWD_SHADING_H
#define _LIBAWD_SHADING_H

#include "texture.h"
#include "block.h"
#include "attr.h"

typedef enum {
    AWD_SHADEMETHOD_ENV_AMBIENT=1,
    AWD_SHADEMETHOD_DIFFUSE_DEPTH=51,
    AWD_SHADEMETHOD_DIFFUSE_GRADIENT=52,
    AWD_SHADEMETHOD_DIFFUSE_WRAP=53,
    AWD_SHADEMETHOD_DIFFUSE_LIGHTMAP=54,
    AWD_SHADEMETHOD_DIFFUSE_CELL=55,
    AWD_SHADEMETHOD_DIFFUSE_SUBSURFACESCATTERING=56,
    AWD_SHADEMETHOD_SPECULAR_ANISOTROPIC=101,
    AWD_SHADEMETHOD_SPECULAR_PHONG=102,
    AWD_SHADEMETHOD_SPECULAR_CELL=103,
    AWD_SHADEMETHOD_SPECULAR_FRESNEL=104,
    AWD_SHADEMETHOD_NORMAL_SIMPLE_WATER=152,
    AWD_SHADOWMETHOD_WRAPPER=998,
    AWD_EFFECTMETHOD_WRAPPER=999,
} AWD_shade_type;

class AWDShadingMethod :
    public AWDAttrElement
{
    protected:
        AWD_shade_type type;
        AWDNumAttrList * shading_props;
        AWDBlock * awdBlock1;
        AWDBlock * awdBlock2;
        AWDBlock * awdBlock3;
        AWDBlock * awdBlock4;

    public:
        AWDShadingMethod(AWD_shade_type);
        ~AWDShadingMethod();
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        void set_awdBlock1(AWDBlock *);
        void set_awdBlock2(AWDBlock *);
        void set_awdBlock3(AWDBlock *);
        void set_awdBlock4(AWDBlock *);
        AWD_shade_type get_shading_type();
        AWDNumAttrList * get_shading_props();
        awd_uint32 calc_method_length(BlockSettings *);
        void write_method(int, BlockSettings *);
        void add_color_property(int, awd_uint32, awd_uint32);
        void add_number_property(int, float, float);
        void add_int_property(int, int, int);
        void add_int8_property(int, int, int);
        void add_bool_property(int, bool, bool);
};

#endif

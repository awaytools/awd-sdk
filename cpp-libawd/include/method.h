#ifndef _LIBAWD_METHOD_H
#define _LIBAWD_METHOD_H

#include "scene.h"

#define PROPS_BADDR1 1
#define PROPS_BADDR2 2
#define PROPS_BADDR3 3
#define PROPS_BADDR4 4

#define PROPS_NUMBER1 101
#define PROPS_NUMBER2 102
#define PROPS_NUMBER3 103
#define PROPS_NUMBER4 104
#define PROPS_NUMBER5 105
#define PROPS_NUMBER6 106

#define PROPS_SCALEU 110
#define PROPS_SCALEV 111

#define PROPS_INT1 301
#define PROPS_INT2 302
#define PROPS_INT3 303
#define PROPS_INT4 304
#define PROPS_INT5 305
#define PROPS_INT6 306

#define PROPS_INT8_1 401

#define PROPS_COLOR1 601
#define PROPS_COLOR2 602
#define PROPS_COLOR3 603

#define PROPS_BOOL1 701
#define PROPS_BOOL2 702
#define PROPS_BOOL3 703
#define PROPS_BOOL4 704
#define PROPS_BOOL5 705
#define PROPS_BOOL6 706

typedef enum {
    AWD_FXMETHOD_COLORMATRIX=401,
    AWD_FXMETHOD_COLORTRANSFORM=402,
    AWD_FXMETHOD_ENVMAP=403,
    AWD_FXMETHOD_LIGHTMAP=404,
    AWD_FXMETHOD_PROJECTIVE_TEXTURE=405,
    AWD_FXMETHOD_RIMLIGHT=406,
    AWD_FXMETHOD_ALPHA_MASK=407,
    AWD_FXMETHOD_REFRACTION_ENVMAP=408,
    AWD_FXMETHOD_OUTLINE=409,
    AWD_FXMETHOD_FRESNEL_ENVMAP=410,
    AWD_FXMETHOD_FOG=411,
} AWD_effect_method_type;

class AWDEffectMethod :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_effect_method_type type;
        AWDNumAttrList * effect_props;
        AWDBlock * awdBlock1;
        AWDBlock * awdBlock2;
        AWDBlock * awdBlock3;
        AWDBlock * awdBlock4;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *curBlockSettings);

    public:
        AWDEffectMethod(const char *, awd_uint16, AWD_effect_method_type);
        ~AWDEffectMethod();
        void set_awdBlock1(AWDBlock *);
        void set_awdBlock2(AWDBlock *);
        void set_awdBlock3(AWDBlock *);
        void set_awdBlock4(AWDBlock *);
        AWDNumAttrList * get_effect_props();
        void add_color_method_prop(int, awd_uint32, awd_uint32);
        void add_number_method_prop(int, float, float);
        void add_int_method_prop(int, int, int);
        void add_bool_method_prop(int, bool, bool);
};

#endif

#ifndef _LIBAWD_SHADOWS_H
#define _LIBAWD_SHADOWS_H

#include "scene.h"

#define PROP_LIGHT_SHADOWMAPPER 9
#define PROP_LIGHT_SHADOWMAPPER_DEPTHMAPSIZE 10
#define PROP_LIGHT_SHADOWMAPPER_COVERAGE 11
#define PROP_LIGHT_SHADOWMAPPER_NUMCASCADES 12

typedef enum {
    AWD_SHADOW_UNDEFINED=0,
    AWD_FILTERED_SHADOW=1101,
    AWD_DITHERED_SHADOW=1102,
    AWD_SOFT_SHADOW=1103,
    AWD_HARD_SHADOW=1104,
    AWD_CASCADE_SHADOW=1001,
    AWD_NEAR_SHADOW=1002,
} AWD_shadow_method_type;

class AWDShadowMethod :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_shadow_method_type shadow_type;
        AWDNumAttrList * shadow_props;
        AWDBlock * awdLight;
        AWDBlock * base_method;
        bool autoApply;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *curBlockSettings);

    public:
        AWDShadowMethod(const char *, awd_uint16, AWD_shadow_method_type);
        ~AWDShadowMethod();
        void set_awdLight(AWDBlock *);;
        AWDBlock * get_awdLight();
        AWDNumAttrList * get_shadow_props();
        void set_base_method(AWDBlock *);
        AWDBlock * get_base_method();
        void set_autoApply(bool);
        bool get_autoApply();
        void set_shadow_type(AWD_shadow_method_type);
        AWD_shadow_method_type get_shadow_type();
        void add_color_shadow_prop(int, awd_uint32, awd_uint32);
        void add_number_shadow_prop(int, float, float);
        void add_int_shadow_prop(int, int, int);
        void add_int16_shadow_prop(int, int, int);
        void add_bool_shadow_prop(int, bool, bool);
};

#endif

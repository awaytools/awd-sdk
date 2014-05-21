#ifndef _LIBAWD_MATERIAL_H
#define _LIBAWD_MATERIAL_H

#include "name.h"
#include "texture.h"
#include "block.h"
#include "attr.h"
#include "shading.h"
#include "light.h"
//#include "anim.h"

#define PROP_MAT_COLOR 1
#define PROP_MAT_TEXTURE 2
#define PROP_MAT_NORMALTEXTURE 3
#define PROP_MAT_SPECIAL_ID 4
#define PROP_MAT_SMOOTH 5
#define PROP_MAT_MIPMAP 6
#define PROP_MAT_BOTHSIDES 7
#define PROP_MAT_APLHA_PREMULTIPLIED 8
#define PROP_MAT_BLENDMODE 9
#define PROP_MAT_ALPHA 10
#define PROP_MAT_ALPHA_BLENDING 11
#define PROP_MAT_ALPHA_THRESHOLD 12
#define PROP_MAT_REPEAT 13
//#define PROP_MAT_CURRENTLY_NOT_USED 14
#define PROP_MAT_AMBIENT_LEVEL 15
#define PROP_MAT_AMBIENTCOLOR 16
#define PROP_MAT_AMBIENTTEXTURE 17
#define PROP_MAT_SPECULARLEVEL 18
#define PROP_MAT_GLOSS 19
#define PROP_MAT_SPECULARCOLOR 20
#define PROP_MAT_SPECULARTEXTURE 21
#define PROP_MAT_LIGHTPICKER 22

typedef enum {
    AWD_MATTYPE_UNDEFINED=0,
    AWD_MATTYPE_COLOR=1,
    AWD_MATTYPE_TEXTURE=2
} AWD_mat_type;

typedef struct _AWD_mat_method {
    AWDShadingMethod *method;
    struct _AWD_mat_method *next;
} AWD_mat_method;

class AWDMaterial :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_mat_type type;
        int mappingChannel;
        int secondMappingChannel;
        bool is_faceted;
        bool isCreated;//will be true, once the mtl is converted to awd
        bool isClone;

        AWD_mat_method *first_method;
        AWD_mat_method *last_method;

        awd_uint8 num_methods;

        AWDBlockList * effectMethods;

        AWDBlockList * materialClones;

        // vars for material properties
        awd_color color;
        AWDBitmapTexture *texture;
        AWDBitmapTexture *normalTexture;
        bool multiPass;//specialID
        bool smooth;
        bool mipmap;
        bool both_sides;
        bool premultiplied;
        int blendMode;
        awd_float32 alpha;
        bool alpha_blending;
        awd_float32 alpha_threshold;
        bool repeat;
        awd_float32 ambientStrength;
        awd_color ambientColor;
        AWDBitmapTexture *ambientTexture;
        awd_float32 specularStrength;
        awd_uint16 glossStrength;
        awd_color specularColor;
        AWDBitmapTexture *specTexture;
        AWDLightPicker *lightPicker;
        AWDBlock *animator;
        AWDBlock *shadowMethod;
        awd_float64 * uv_transform_mtx;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        void write_body(int, BlockSettings *);

    public:
        AWDMaterial(const char *, awd_uint16);
        ~AWDMaterial();

        void set_type(AWD_mat_type);
        AWD_mat_type get_type();
        void set_isClone(bool);
        bool get_isClone();

        void set_mappingChannel(int);
        int get_mappingChannel();
        void set_secondMappingChannel(int);
        int get_secondMappingChannel();
        void set_is_faceted(bool);
        bool get_is_faceted();
        void set_isCreated(bool);
        bool get_isCreated();

        void set_color(awd_color);
        awd_color get_color();
        void set_texture(AWDBitmapTexture *);
        AWDBitmapTexture *get_texture();
        void set_normalTexture(AWDBitmapTexture *);
        AWDBitmapTexture *get_normalTexture();
        void set_multiPass(bool);
        bool get_multiPass();
        void set_smooth(bool);
        bool get_smooth();
        void set_mipmap(bool);
        bool get_mipmap();
        void set_both_sides(bool);
        bool get_both_sides();
        void set_premultiplied(bool);
        bool get_premultiplied();
        void set_blendMode(int);
        int get_blendMode();
        void set_alpha(awd_float32);
        awd_float32 get_alpha();
        void set_alpha_blending(bool);
        bool get_alpha_blending();
        void set_alpha_threshold(awd_float32);
        awd_float32 get_alpha_threshold();
        void set_repeat(bool);
        bool get_repeat();
        void set_ambientStrength(awd_float32);
        awd_float32 get_ambientStrength();
        void set_ambientColor(awd_color);
        awd_color get_ambientColor();
        void set_ambientTexture(AWDBitmapTexture *);
        AWDBitmapTexture *get_ambientTexture();
        void set_specularStrength(awd_float32);
        awd_float32 get_specularStrength();
        void set_glossStrength(awd_uint16);
        awd_uint16 get_glossStrength();
        void set_specularColor(awd_color);
        awd_color get_specularColor();
        void set_specTexture(AWDBitmapTexture *);
        AWDBitmapTexture *get_specTexture();
        void set_lightPicker(AWDLightPicker *);
        AWDLightPicker *get_lightPicker();
        void set_animator(AWDBlock *);
        AWDBlock *get_animator();
        void set_shadowMethod(AWDBlock *);
        AWDBlock *get_shadowMethod();

        AWDBlockList *get_materialClones();

        void set_effectMethods(AWDBlockList *);
        AWDBlockList *get_effectMethods();

        void set_uv_transform_mtx(awd_float64 *);
        awd_float64 *get_uv_transform_mtx();

        AWDMaterial* get_unique_material(AWDLightPicker *, AWDBlock *, AWDBlockList*);
        void add_method(AWDShadingMethod *);
        void resetShadingMethods();
};

#endif

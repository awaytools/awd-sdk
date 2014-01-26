#ifndef _LIBAWD_MATERIAL_H
#define _LIBAWD_MATERIAL_H

#include "name.h"
#include "texture.h"
#include "block.h"
#include "attr.h"
#include "shading.h"

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
#define PROP_MAT_CURRENTLY_NOT_USED 14
#define PROP_MAT_AMBIENT_LEVEL 15
#define PROP_MAT_AMBIENTCOLOR 16
#define PROP_MAT_AMBIENTTEXTURE 17  
#define PROP_MAT_SPECULARLEVEL 18
#define PROP_MAT_GLOSS 19
#define PROP_MAT_SPECULARCOLOR 20
#define PROP_MAT_SPECULARTEXTURE 21
#define PROP_MAT_LIGHTPICKER 22


typedef enum {
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
        AWDBitmapTexture *texture;
        AWDBitmapTexture *ambientTexture;
        AWDBitmapTexture *specTexture;
        AWDBitmapTexture *normalTexture;
        AWD_mat_method *first_method;
        AWD_mat_method *last_method;
        awd_uint8 num_methods;

    protected:
        awd_uint32 calc_body_length(bool);
        void prepare_and_add_dependencies(AWDBlockList *);
        void write_body(int, bool);

    public:
        AWDMaterial(AWD_mat_type, const char *, awd_uint16);
        ~AWDMaterial();
		bool isCreated;//will be true, once the mtl is converted to awd

        bool repeat;
        bool alpha_blending;
        awd_color color;
        awd_color ambientColor;
        awd_color specularColor;
        double specularStrength;
        double abmientStrength;
        double glossStrength;
        awd_float32 alpha_threshold;

        void set_type(AWD_mat_type);
        AWD_mat_type get_type();

        void set_texture(AWDBitmapTexture *);
        AWDBitmapTexture *get_texture();
        void set_ambientTexture(AWDBitmapTexture *);
        AWDBitmapTexture *get_ambientTexture();
        void set_specTexture(AWDBitmapTexture *);
        AWDBitmapTexture *get_specTexture();
        void set_normalTexture(AWDBitmapTexture *);
        AWDBitmapTexture *get_normalTexture();

        void add_method(AWDShadingMethod *);
};

#endif

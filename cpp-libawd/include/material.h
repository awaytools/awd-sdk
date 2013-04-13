#ifndef _LIBAWD_MATERIAL_H
#define _LIBAWD_MATERIAL_H

#include "name.h"
#include "texture.h"
#include "block.h"
#include "attr.h"
#include "shading.h"

#define PROP_MAT_COLOR 1
#define PROP_MAT_TEXTURE 2
#define PROP_MAT_ALPHA_BLENDING 11
#define PROP_MAT_ALPHA_THRESHOLD 12
#define PROP_MAT_REPEAT 13


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
        AWD_mat_method *first_method;
        AWD_mat_method *last_method;
        awd_uint8 num_methods;

    protected:
        awd_uint32 calc_body_length(bool);
        void prepare_write();
        void write_body(int, bool);

    public:
        AWDMaterial(AWD_mat_type, const char *, awd_uint16);
        ~AWDMaterial();

        bool repeat;
        bool alpha_blending;
        awd_color color;
        awd_float32 alpha_threshold;

        void set_type(AWD_mat_type);
        AWD_mat_type get_type();

        void set_texture(AWDBitmapTexture *);
        AWDBitmapTexture *get_texture();

        void add_method(AWDShadingMethod *);
};

#endif

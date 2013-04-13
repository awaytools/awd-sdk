#include "material.h"
#include "util.h"

#include "platform.h"

AWDMaterial::AWDMaterial(AWD_mat_type type, const char *name, awd_uint16 name_len) : 
    AWDBlock(SIMPLE_MATERIAL),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->type = type;
    this->texture = NULL;
    this->color = 0;
    this->alpha_threshold = 0.0f;
    this->alpha_blending = false;
    this->repeat = false;
    this->first_method = NULL;
    this->last_method = NULL;
    this->num_methods = 0;
}

AWDMaterial::~AWDMaterial()
{
    // Texture will be deleted 
    // by the block list it's in
    this->texture = NULL;
}


AWD_mat_type
AWDMaterial::get_type()
{
    return this->type;
}

void
AWDMaterial::set_type(AWD_mat_type type)
{
    this->type = type;
}



AWDBitmapTexture *
AWDMaterial::get_texture()
{
    return this->texture;
}

void
AWDMaterial::set_texture(AWDBitmapTexture *texture)
{
    this->texture = texture;
}


void
AWDMaterial::add_method(AWDShadingMethod *method)
{
    AWD_mat_method *item;

    item = (AWD_mat_method *)malloc(sizeof(AWD_mat_method));
    item->method = method;
    item->next = NULL;

    if (!this->first_method)
        this->first_method = item;
    else
        this->last_method->next = item;

    this->last_method = item;
    this->last_method->next = NULL;

    this->num_methods++;
}


awd_uint32
AWDMaterial::calc_body_length(bool wide_mtx)
{
    AWD_mat_method *cur;
    awd_uint32 len;

    len = 2; // type + method count 8-bit ints

    len += sizeof(awd_uint16) + this->get_name_length();
    len += this->calc_attr_length(true, true, wide_mtx);

    cur = this->first_method;
    while (cur) {
        len += cur->method->calc_method_length(wide_mtx);
        cur = cur->next;
    }

    return len;
}


void
AWDMaterial::prepare_write()
{
    if (this->type == AWD_MATTYPE_COLOR) {
        if (this->color > 0) {
            AWD_field_ptr col_val;
            col_val.col = (awd_color *)malloc(sizeof(awd_uint32));
            *col_val.col = this->color;
            this->properties->set(PROP_MAT_COLOR, col_val, sizeof(awd_color), AWD_FIELD_COLOR);
        }
    }
    else {
        if (this->texture) {
            AWD_field_ptr tex_val;
            tex_val.addr = (awd_baddr *)malloc(sizeof(awd_baddr));
            *tex_val.addr = this->texture->get_addr();
            this->properties->set(PROP_MAT_TEXTURE, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
        }

        if (this->repeat) {
            AWD_field_ptr rep_val;
            rep_val.b = (awd_bool *)malloc(sizeof(awd_bool));
            *rep_val.b = AWD_TRUE;
            this->properties->set(PROP_MAT_REPEAT, rep_val, sizeof(awd_bool), AWD_FIELD_BOOL);
        }

        if (this->alpha_blending) {
            AWD_field_ptr trans_val;
            trans_val.b = (awd_bool *)malloc(sizeof(awd_bool));
            *trans_val.b = AWD_TRUE;
            this->properties->set(PROP_MAT_ALPHA_BLENDING, trans_val, sizeof(awd_bool), AWD_FIELD_BOOL);
        }

        if (this->alpha_threshold != 0.0f) {
            AWD_field_ptr th_val;
            th_val.f32 = (awd_float32 *)malloc(sizeof(awd_float32));
            *th_val.f32 = this->alpha_threshold;
            this->properties->set(PROP_MAT_ALPHA_THRESHOLD, th_val, sizeof(awd_float32), AWD_FIELD_FLOAT32);
        }
    }
}


void
AWDMaterial::write_body(int fd, bool wide_mtx)
{
    AWD_mat_method *cur;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &this->type, sizeof(awd_uint8));
    write(fd, &this->num_methods, sizeof(awd_uint8));

    this->properties->write_attributes(fd, wide_mtx);

    cur = this->first_method;
    while (cur) {
        cur->method->write_method(fd, wide_mtx);
        cur = cur->next;
    }

    this->user_attributes->write_attributes(fd, wide_mtx);
}

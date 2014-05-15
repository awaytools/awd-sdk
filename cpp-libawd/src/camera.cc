#include "camera.h"
#include "platform.h"
#include "util.h"
#include "block.h"

AWDCamera::AWDCamera(const char *name, awd_uint16 name_len,  AWD_lens_type lens, awd_float64 * mtx) :
    AWDSceneBlock(CAMERA, name, name_len, mtx)
{
    this->lens_type = lens;

    this->lens_fov = 60.0;
    this->lens_near = 20.0;
    this->lens_far = 3000.0;
    this->lens_proj_height = 600;
    this->lens_properties=new AWDNumAttrList();
}

AWDCamera::~AWDCamera()
{
    delete this->lens_properties;
}

awd_float64
AWDCamera::get_lens_near()
{
    return this->lens_near;
}

void
AWDCamera::set_lens_near(awd_float64 lens_near)
{
    this->lens_near = lens_near;
}

awd_float64
AWDCamera::get_lens_far()
{
    return this->lens_far;
}

void
AWDCamera::set_lens_far(awd_float64 lens_far)
{
    this->lens_far = lens_far;
}
awd_float64
AWDCamera::get_lens_proj_height()
{
    return this->lens_proj_height;
}

void
AWDCamera::set_lens_proj_height(awd_float64 lens_proj_height)
{
    this->lens_proj_height = lens_proj_height;
}
awd_float64
AWDCamera::get_lens_fov()
{
    return this->lens_fov;
}
void
AWDCamera::set_lens_fov(awd_float64 fov)
{
    this->lens_fov = fov;
}
AWD_cam_type
AWDCamera::get_cam_type()
{
    return this->cam_type;
}
void
AWDCamera::set_cam_type(AWD_cam_type cam_type)
{
    this->cam_type = cam_type;
}
AWD_lens_type
AWDCamera::get_lens_type()
{
    return this->lens_type;
}
void
AWDCamera::set_lens_type(AWD_lens_type lens_type)
{
    this->lens_type = lens_type;
}

void
AWDCamera::set_lens_offset(awd_float64 x_pos,awd_float64 x_neg,awd_float64 y_pos,awd_float64 y_neg)
{
    this->lens_offX_pos = x_pos;
    this->lens_offX_neg = x_neg;
    this->lens_offY_pos = y_pos;
    this->lens_offY_neg = y_neg;
}

void
AWDCamera::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    if(this->lens_type==AWD_LENS_PERSPECTIVE){
        if (this->lens_fov!=60.0){
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64));
            *newVal.f64 = this->lens_fov;
            this->lens_properties->set(101, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
        }
    }
    else if(this->lens_type==AWD_LENS_ORTHO){
        if (this->lens_proj_height!=600.0){
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64));
            *newVal.f64 = this->lens_proj_height;
            this->lens_properties->set(101, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
        }
    }
    else if(this->lens_type==AWD_LENS_ORTHOOFFCENTER){
        if (this->lens_offX_pos!=400.0){
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64));
            *newVal.f64 = this->lens_offX_pos;
            this->lens_properties->set(102, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
        }
        if (this->lens_offX_neg!=-400.0){
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64));
            *newVal.f64 = this->lens_offX_neg;
            this->lens_properties->set(101, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
        }
        if (this->lens_offY_pos!=300.0){
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64));
            *newVal.f64 = this->lens_offY_pos;
            this->lens_properties->set(104, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
        }
        if (this->lens_offY_neg!=-300.0){
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64));
            *newVal.f64 = this->lens_offY_neg;
            this->lens_properties->set(103, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
        }
    }
    if (this->lens_near!=20.0){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_float64));
        *newVal.f64 = this->lens_near;
        this->properties->set(101, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    if (this->lens_far!=3000.0){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_float64));
        *newVal.f64 = this->lens_far;
        this->properties->set(102, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
    //AWD_field_ptr fov_val;

    //fov_val.f64 = &this->lens_fov;
    //this->lens_properties->set(PROP_CAM_FOV, fov_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
}

awd_uint32
AWDCamera::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = this->calc_common_length(curBlockSettings->get_wide_matrix());
    len += sizeof(awd_uint8) ;//set as active cam
    len += sizeof(awd_uint16) ;//num - prop lists (for now allways 1)
    len += sizeof(awd_uint16) ;//prop lists type (lens-type)
    len += this->lens_properties->calc_length(curBlockSettings);
    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}

void
AWDCamera::write_body(int fd,BlockSettings *curBlockSettings)
{
    this->write_scene_common(fd, curBlockSettings);
    awd_uint8 useasActive=1;//unused in the parser atm
    write(fd, &useasActive, sizeof(awd_uint8));
    awd_uint16 lengthOfPropsLists=1;//allways 1 for now
    write(fd, &lengthOfPropsLists, sizeof(awd_uint16));
    awd_uint16 lens=this->lens_type;
    write(fd, &lens, sizeof(awd_uint16));
    this->lens_properties->write_attributes(fd, curBlockSettings);

    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd,  curBlockSettings);
}

AWDTextureProjector::AWDTextureProjector(const char *name, awd_uint16 name_len, awd_float64 * mtx) :
    AWDSceneBlock(TEXTURE_PROJECTOR, name, name_len, mtx)
{
    this->texture = NULL;
    this->field_of_view = 60.0;
    this->aspect_ratio = 20.0;
}

AWDTextureProjector::~AWDTextureProjector()
{
}

awd_float64
AWDTextureProjector::get_field_of_view()
{
    return this->field_of_view;
}

void
AWDTextureProjector::set_field_of_view(awd_float64 field_of_view)
{
    this->field_of_view = field_of_view;
}

awd_float64
AWDTextureProjector::get_aspect_ratio()
{
    return this->aspect_ratio;
}

void
AWDTextureProjector::set_aspect_ratio(awd_float64 aspect_ratio)
{
    this->aspect_ratio = aspect_ratio;
}
AWDBlock *
AWDTextureProjector::get_texture()
{
    return this->texture;
}
void
AWDTextureProjector::set_texture(AWDBlock * texture)
{
    this->texture = texture;
}

void
AWDTextureProjector::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    if (this->texture != NULL)
        this->texture->prepare_and_add_with_dependencies(targetList);
}

awd_uint32
AWDTextureProjector::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;

    len = this->calc_common_length(curBlockSettings->get_wide_matrix());
    len += sizeof(awd_uint32) ;//texture
    len += sizeof(awd_float32) ;//aspect ratio
    len += sizeof(awd_float32) ;//field of view
    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}

void
AWDTextureProjector::write_body(int fd,BlockSettings *curBlockSettings)
{
    this->write_scene_common(fd, curBlockSettings);
    int texture_addr = 0;
    if (this->texture != NULL)
        texture_addr = UI32(this->texture->get_addr());
    write(fd, &texture_addr, sizeof(awd_uint32));

    awd_float32 aspectRatio=awd_float32(this->aspect_ratio);//unused in the parser atm
    write(fd, &aspectRatio, sizeof(awd_float32));
    awd_float32 field_of_view=awd_float32(this->field_of_view);//unused in the parser atm
    write(fd, &field_of_view, sizeof(awd_float32));

    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd,  curBlockSettings);
}
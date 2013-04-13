#include "camera.h"
#include "platform.h"

AWDCamera::AWDCamera(const char *name, awd_uint16 name_len, AWD_cam_type type, AWD_lens_type lens) :
    AWDSceneBlock(CAMERA, name, name_len, NULL)
{
    this->type = type;
    this->lens = lens;

    this->lens_fov = 35.0;
}


AWDCamera::~AWDCamera()
{
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


void
AWDCamera::prepare_write()
{
    AWD_field_ptr fov_val;

    fov_val.f64 = &this->lens_fov;
    this->properties->set(PROP_CAM_FOV, fov_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
}


awd_uint32
AWDCamera::calc_body_length(bool wide_mtx)
{
    awd_uint32 len;

    len = 2; // Two single-byte static fields
    len += this->calc_common_length(wide_mtx);
    len += this->calc_attr_length(true, true, wide_mtx);

    return len;
}


void
AWDCamera::write_body(int fd, bool wide_mtx)
{
    this->write_scene_common(fd, wide_mtx);

    write(fd, &this->type, sizeof(awd_uint8));
    write(fd, &this->lens, sizeof(awd_uint8));

    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}


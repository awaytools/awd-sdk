#include "camera.h"
#include "platform.h"
#include "util.h"

AWDCamera::AWDCamera(const char *name, awd_uint16 name_len,  AWD_lens_type lens, awd_float64 * mtx) :
    AWDSceneBlock(CAMERA, name, name_len, mtx)
{
    this->lens = lens;

    this->lens_fov = 35.0;

	this->lens_properties=new AWDNumAttrList();
}


AWDCamera::~AWDCamera()
{
	delete this->lens_properties;
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
AWDCamera::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    //AWD_field_ptr fov_val;

    //fov_val.f64 = &this->lens_fov;
    //this->lens_properties->set(PROP_CAM_FOV, fov_val, sizeof(awd_float64), AWD_FIELD_FLOAT64);
}

awd_uint32
AWDCamera::calc_body_length(BlockSettings * curBlockSettings)
{
    awd_uint32 len;

    len = this->calc_common_length(curBlockSettings->get_wide_matrix());
	//len += sizeof(awd_uint16) + this->get_name_length();//name
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
    //awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
	awd_uint8 useasActive=1;//unused in the parser atm
    write(fd, &useasActive, sizeof(awd_uint8));
	awd_uint16 lengthOfPropsLists=1;//allways 1 for now
    write(fd, &lengthOfPropsLists, sizeof(awd_uint16));
	awd_uint16 lens=this->lens;
    write(fd, &lens, sizeof(awd_uint16));	
	this->lens_properties->write_attributes(fd, curBlockSettings);

	this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd,  curBlockSettings);
}


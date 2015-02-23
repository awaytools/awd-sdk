#include "blocks/camera.h"
#include <string>
#include "utils/awd_properties.h"
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::ATTR;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::CAMERA;

Camera::Camera(std::string& name,  CAMERA::lens_type lens, TYPES::F64 * mtx) :
	SceneBlockBase(block_type::CAMERA, name, mtx)
{
	this->lens_type = lens;

	this->lens_fov = 60.0;
	this->lens_near = 20.0;
	this->lens_far = 3000.0;
	this->lens_proj_height = 600;
	this->lens_properties=new NumAttrList();
}
Camera::Camera():
	SceneBlockBase(block_type::CAMERA, "", NULL)
{
}

Camera::~Camera()
{
	delete this->lens_properties;
}

TYPES::state Camera::validate_block_state()
{
	return TYPES::state::VALID;
}
TYPES::F64
Camera::get_lens_near()
{
	return this->lens_near;
}

void
Camera::set_lens_near(TYPES::F64 lens_near)
{
	this->lens_near = lens_near;
}

TYPES::F64
Camera::get_lens_far()
{
	return this->lens_far;
}

void
Camera::set_lens_far(TYPES::F64 lens_far)
{
	this->lens_far = lens_far;
}
TYPES::F64
Camera::get_lens_proj_height()
{
	return this->lens_proj_height;
}

void
Camera::set_lens_proj_height(TYPES::F64 lens_proj_height)
{
	this->lens_proj_height = lens_proj_height;
}
TYPES::F64
Camera::get_lens_fov()
{
	return this->lens_fov;
}
void
Camera::set_lens_fov(TYPES::F64 fov)
{
	this->lens_fov = fov;
}
cam_type
Camera::get_cam_type()
{
	return this->cam_type;
}
void
Camera::set_cam_type(CAMERA::cam_type cam_type)
{
	this->cam_type = cam_type;
}
lens_type
Camera::get_lens_type()
{
	return this->lens_type;
}
void
Camera::set_lens_type(CAMERA::lens_type lens_type)
{
	this->lens_type = lens_type;
}

void
Camera::set_lens_offset(TYPES::F64 x_pos,TYPES::F64 x_neg,TYPES::F64 y_pos,TYPES::F64 y_neg)
{
	this->lens_offX_pos = x_pos;
	this->lens_offX_neg = x_neg;
	this->lens_offY_pos = y_pos;
	this->lens_offY_neg = y_neg;
}

result
Camera::collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type)
{
	if(this->lens_type==lens_type::AWD_LENS_PERSPECTIVE){
		if (this->lens_fov!=60.0){
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64));
			*newVal.F64 = this->lens_fov;
			//this->lens_properties->set(101, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
		}
	}
	else if(this->lens_type==lens_type::AWD_LENS_ORTHO){
		if (this->lens_proj_height!=600.0){
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64));
			*newVal.F64 = this->lens_proj_height;
			//this->lens_properties->set(101, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
		}
	}
	else if(this->lens_type==lens_type::AWD_LENS_ORTHOOFFCENTER){
		if (this->lens_offX_pos!=400.0){
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64));
			*newVal.F64 = this->lens_offX_pos;
		//	this->lens_properties->set(102, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
		}
		if (this->lens_offX_neg!=-400.0){
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64));
			*newVal.F64 = this->lens_offX_neg;
			//this->lens_properties->set(101, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
		}
		if (this->lens_offY_pos!=300.0){
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64));
			*newVal.F64 = this->lens_offY_pos;
			//this->lens_properties->set(104, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
		}
		if (this->lens_offY_neg!=-300.0){
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64));
			*newVal.F64 = this->lens_offY_neg;
			//this->lens_properties->set(103, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
		}
	}
	if (this->lens_near!=20.0){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::F64));
		*newVal.F64 = this->lens_near;
		//this->properties->set(101, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
	}
	if (this->lens_far!=3000.0){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::F64));
		*newVal.F64 = this->lens_far;
		//this->properties->set(102, newVal, sizeof(TYPES::F64), data_types::FLOAT64);
	}
	//union_ptr fov_val;

	//fov_val.F64 = &this->lens_fov;
	//this->lens_properties->set(PROP_CAM_FOV, fov_val, sizeof(TYPES::F64), FLOAT64);
	return result::AWD_SUCCESS;
}

TYPES::UINT32
Camera::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 len;

	len = this->calc_common_length(settings->get_wide_matrix());
	len += sizeof(TYPES::UINT8) ;//set as active cam
	len += sizeof(TYPES::UINT16) ;//num - prop lists (for now allways 1)
	len += sizeof(TYPES::UINT16) ;//prop lists type (lens-type)
	len += this->lens_properties->calc_length(settings);
	len += this->calc_attr_length(true, true, settings);

	return len;
}

result
Camera::write_body(FileWriter * fileWriter, BlockSettings * settings, AWDFile* file)
{
	this->write_scene_common(fileWriter, settings, file);
	TYPES::UINT8 useasActive=1;//unused in the parser atm
	fileWriter->writeUINT8(useasActive);
	TYPES::UINT16 lengthOfPropsLists=1;//allways 1 for now
	fileWriter->writeUINT16(lengthOfPropsLists);
	TYPES::UINT16 lens=TYPES::UINT16(this->lens_type);
	fileWriter->writeUINT16(lens);
	this->lens_properties->write_attributes(fileWriter, settings);

	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter,  settings);
	return result::AWD_SUCCESS;
}

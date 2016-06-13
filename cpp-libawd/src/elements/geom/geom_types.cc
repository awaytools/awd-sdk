#include "elements/geom_elements.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "utils/util.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;


MATRIX2x3::MATRIX2x3(TYPES::F64 *mtx)
{
	this->matrix_data = (TYPES::F64*)malloc(6 * sizeof(TYPES::F64));
	set(mtx);
	this->hasOther=true;
	this->hasPosition=true;
}
MATRIX2x3::MATRIX2x3()
{
	this->matrix_data = (TYPES::F64*)malloc(6 * sizeof(TYPES::F64));
	set(NULL);
	this->hasOther=true;
	this->hasPosition=true;
}
MATRIX2x3::~MATRIX2x3()
{
	free(this->matrix_data);
}
TYPES::UINT8 
MATRIX2x3::get_save_type(){
	if((this->hasPosition)&&(this->hasOther))
		return 1;
	if((!this->hasPosition)&&(this->hasOther))
		return 11;
	if((this->hasPosition)&&(!this->hasOther))
		return 12;
	return 0;
}
TYPES::F64* 
MATRIX2x3::get(){
	return this->matrix_data;
}

result
MATRIX2x3::set(TYPES::F64 *matrix_data)
{
	// no values given. set from 
	if (matrix_data == NULL) {
		this->matrix_data[0]  = 1.0;		this->matrix_data[1]  = 0.0;
		this->matrix_data[2]  = 0.0; 		this->matrix_data[3]  = 1.0;
		this->matrix_data[4]  = 0.0;		this->matrix_data[5]  = 0.0; 
	}
	else{
		for(int cnt=0; cnt<6; cnt++){
			this->matrix_data[cnt]=matrix_data[cnt];
		}
	}

	return result::AWD_SUCCESS;
}

result
MATRIX2x3::read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *)
{
	return result::AWD_SUCCESS;
}

void 
MATRIX2x3::fill_into_list(std::vector<TYPES::F32>& input_list){
	if(this->hasOther){
		for(int cnt=0; cnt<4; cnt++)
			input_list.push_back(this->matrix_data[cnt]);
	}
	if(this->hasPosition){
		for(int cnt=4; cnt<6; cnt++)
			input_list.push_back(this->matrix_data[cnt]);
	}
}
void 
MATRIX2x3::toString(std::string& output_str){
	if(this->hasOther){
		for(int cnt=0; cnt<4; cnt++)
			output_str+="#"+FILES::num_to_string(this->matrix_data[cnt]);
	}
	if(this->hasPosition){
		for(int cnt=4; cnt<6; cnt++)
			output_str+="#"+FILES::num_to_string(this->matrix_data[cnt]);
	}
}
result
MATRIX2x3::append(MATRIX2x3* mtx)
{
	TYPES::F64 a1 =  this->matrix_data[0];
	TYPES::F64 b1 =  this->matrix_data[1];
	TYPES::F64 c1 =  this->matrix_data[2];
	TYPES::F64 d1 =  this->matrix_data[3];
	TYPES::F64 tx1 =  this->matrix_data[4];
	TYPES::F64 ty1 =  this->matrix_data[5];
	
	TYPES::F64* input_mtx=mtx->get();

	this->matrix_data[0]= a1 * input_mtx[0] + b1 * input_mtx[2];
	this->matrix_data[1]= a1 * input_mtx[1] + b1 * input_mtx[3];

	this->matrix_data[2]= c1 * input_mtx[0] + d1 * input_mtx[2];
	this->matrix_data[3]= c1 * input_mtx[1] + d1 * input_mtx[3];

	this->matrix_data[4]= tx1 * input_mtx[0] + ty1 * input_mtx[2] + input_mtx[4];
	this->matrix_data[5]= tx1 * input_mtx[1] + ty1 * input_mtx[3] + input_mtx[5];

	return result::AWD_SUCCESS;
}

bool
MATRIX2x3::compare(MATRIX2x3* comp)
{	
	TYPES::F64* comp_mtx=comp->get();
	if(this->matrix_data[0]!=comp_mtx[0])
		return false;
	if(this->matrix_data[1]!=comp_mtx[1])
		return false;
	if(this->matrix_data[2]!=comp_mtx[2])
		return false;
	if(this->matrix_data[3]!=comp_mtx[3])
		return false;
	if(this->matrix_data[4]!=comp_mtx[4])
		return false;
	if(this->matrix_data[5]!=comp_mtx[5])
		return false;
	return true;
}
bool
MATRIX2x3::is_identity()
{	
	if(this->matrix_data[0]!=1)
		return false;
	if(this->matrix_data[1]!=0)
		return false;
	if(this->matrix_data[2]!=0)
		return false;
	if(this->matrix_data[3]!=1)
		return false;
	if(this->matrix_data[4]!=0)
		return false;
	if(this->matrix_data[5]!=0)
		return false;
	return true;
}
result
MATRIX2x3::prepend(MATRIX2x3* mtx)
{	
	TYPES::F64* input_mtx=mtx->get();

	TYPES::F64 a2 =  this->matrix_data[0];
	TYPES::F64 b2 =  this->matrix_data[1];
	TYPES::F64 c2 =  this->matrix_data[2];
	TYPES::F64 d2 =  this->matrix_data[3];
	TYPES::F64 tx2 = this->matrix_data[4];
	TYPES::F64 ty2 = this->matrix_data[5];

	this->matrix_data[0]= input_mtx[0] * a2 + input_mtx[1] * c2;
	this->matrix_data[1]= input_mtx[0] * b2 + input_mtx[1] * d2;

	this->matrix_data[2]= input_mtx[2] * a2 + input_mtx[3] * c2;
	this->matrix_data[3]= input_mtx[2] * b2 + input_mtx[3] * d2;

	this->matrix_data[4]= input_mtx[4] * a2 + input_mtx[5] * c2 + tx2;
	this->matrix_data[5]= input_mtx[4] * b2 + input_mtx[5] * d2 + ty2;

	return result::AWD_SUCCESS;
}

result
MATRIX2x3::write_to_file(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings * settings)
{
	result res = result::AWD_SUCCESS;
	// if scale is 1, we can just write the number depending on storage precision
	if(settings->get_scale()==1)
		return fileWriter->writeNumbers(this->matrix_data, 6, settings->get_wide_matrix());
		
	// otherwise we only write the first 9 values, and than write the position with scale.
	res = fileWriter->writeNumbers(this->matrix_data, 4, settings->get_wide_matrix());
	if(res!=result::AWD_SUCCESS)
		return result::AWDFILE_ERROR;
	
	if (settings->get_wide_matrix()){
		TYPES::F64 offX=TYPES::F64(this->matrix_data[4] * settings->get_scale());
		TYPES::F64 offY=TYPES::F64(this->matrix_data[5] * settings->get_scale());
		if(fileWriter->writeFLOAT64(offX)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
		if(fileWriter->writeFLOAT64(offY)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
	}
	else{
		TYPES::F32 offX=TYPES::F32(this->matrix_data[4] * settings->get_scale());
		TYPES::F32 offY=TYPES::F32(this->matrix_data[5] * settings->get_scale());
		if(fileWriter->writeFLOAT32(offX)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
		if(fileWriter->writeFLOAT32(offY)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
	}

	return result::AWD_SUCCESS;
}



MATRIX4x3::MATRIX4x3(TYPES::F64 *mtx)
{
	this->matrix_data = (TYPES::F64*)malloc(12 * sizeof(TYPES::F64));
	set(mtx);
}
MATRIX4x3::MATRIX4x3()
{
	this->matrix_data = (TYPES::F64*)malloc(12 * sizeof(TYPES::F64));
	set(NULL);
}

MATRIX4x3::~MATRIX4x3()
{
	free(this->matrix_data);
}

TYPES::F64* 
MATRIX4x3::get(){
	return this->matrix_data;
}
result
MATRIX4x3::set(TYPES::F64 *matrix_data)
{
	// no values given. set from 
	if (matrix_data == NULL) {
		this->matrix_data[0]  = 1.0;		this->matrix_data[1]  = 0.0;		this->matrix_data[2]  = 0.0; 
		this->matrix_data[3]  = 0.0;		this->matrix_data[4]  = 1.0;		this->matrix_data[5]  = 0.0; 
		this->matrix_data[6]  = 0.0;		this->matrix_data[7]  = 0.0;		this->matrix_data[8]  = 1.0;	
		this->matrix_data[9]  = 2.50; 		this->matrix_data[10] = 1.5;		this->matrix_data[11] = 0.8;
	}
	else{
		for(int cnt=0; cnt<12; cnt++)
			this->matrix_data[cnt]=matrix_data[cnt];
	}

	return result::AWD_SUCCESS;
}

result
MATRIX4x3::read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *)
{
	return result::AWD_SUCCESS;
}

result
MATRIX4x3::write_to_file(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings * settings)
{
	result res = result::AWD_SUCCESS;
	// if scale is 1, we can just write the number depending on storage precision
	if(settings->get_scale()==1.0)
		return fileWriter->writeNumbers(this->matrix_data, 12, settings->get_wide_matrix());
		
	// otherwise we only write the first 9 values, and than write the position with scale.
	res = fileWriter->writeNumbers(this->matrix_data, 9, settings->get_wide_matrix());
	if(res!=result::AWD_SUCCESS)
		return result::AWDFILE_ERROR;

	if (settings->get_wide_matrix()){
		TYPES::F64 offX=TYPES::F64(this->matrix_data[9] * settings->get_scale());
		TYPES::F64 offY=TYPES::F64(this->matrix_data[10] * settings->get_scale());
		TYPES::F64 offZ=TYPES::F64(this->matrix_data[11] * settings->get_scale());
		if(fileWriter->writeFLOAT64(offX)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
		if(fileWriter->writeFLOAT64(offY)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
		if(fileWriter->writeFLOAT64(offZ)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
	}
	else{
		TYPES::F32 offX=TYPES::F32(this->matrix_data[9] * settings->get_scale());
		TYPES::F32 offY=TYPES::F32(this->matrix_data[10] * settings->get_scale());
		TYPES::F32 offZ=TYPES::F32(this->matrix_data[11] * settings->get_scale());
		if(fileWriter->writeFLOAT32(offX)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
		if(fileWriter->writeFLOAT32(offY)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
		if(fileWriter->writeFLOAT32(offZ)!=result::AWD_SUCCESS)
			return result::AWDFILE_ERROR;
	}

	return result::AWD_SUCCESS;
}

ColorTransform::ColorTransform(TYPES::F64 *mtx)
{
	this->matrix_data = (TYPES::F64*)malloc(8 * sizeof(TYPES::F64));
	set(mtx);
}
ColorTransform::ColorTransform()
{
	this->matrix_data = (TYPES::F64*)malloc(8 * sizeof(TYPES::F64));
	set(NULL);
}

ColorTransform::~ColorTransform()
{
	free(this->matrix_data);
}


void 
ColorTransform::fill_into_list(std::vector<TYPES::F32>& input_list){
	input_list.push_back(TYPES::F32(this->matrix_data[0]));
	input_list.push_back(TYPES::F32(this->matrix_data[2]));
	input_list.push_back(TYPES::F32(this->matrix_data[4]));
	input_list.push_back(TYPES::F32(this->matrix_data[6]));
	input_list.push_back(TYPES::INT16(this->matrix_data[1]));
	input_list.push_back(TYPES::INT16(this->matrix_data[3]));
	input_list.push_back(TYPES::INT16(this->matrix_data[5]));
	input_list.push_back(TYPES::INT16(this->matrix_data[7]));
}
void 
ColorTransform::toString(std::string& output_str){
	for(int cnt=0; cnt<8; cnt++)
		output_str+=FILES::num_to_string(this->matrix_data[cnt]);
}

TYPES::F64* 
ColorTransform::get(){
	return this->matrix_data;
}
result
ColorTransform::set(TYPES::F64 *matrix_data)
{
	// no values given. set from 
	if (matrix_data == NULL) {
		this->matrix_data[0]  = 1.0;//r
		this->matrix_data[1]  = 0.0;
		this->matrix_data[2]  = 1.0;//g
		this->matrix_data[3]  = 0.0;
		this->matrix_data[4]  = 1.0;//b
		this->matrix_data[5]  = 0.0;
		this->matrix_data[6]  = 1.0;//a
		this->matrix_data[7]  = 0.0;
	}
	else{
		for(int cnt=0; cnt<8; cnt++)
			this->matrix_data[cnt]=matrix_data[cnt];
	}

	return result::AWD_SUCCESS;
}

bool
ColorTransform::is_identity()
{	
	if(this->matrix_data[0]!=1)
		return false;
	if(this->matrix_data[1]!=0)
		return false;
	if(this->matrix_data[2]!=1)
		return false;
	if(this->matrix_data[3]!=0)
		return false;
	if(this->matrix_data[4]!=1)
		return false;
	if(this->matrix_data[5]!=0)
		return false;
	if(this->matrix_data[6]!=1)
		return false;
	if(this->matrix_data[7]!=0)
		return false;
	return true;
}
result
ColorTransform:: prepend(ColorTransform* mtx)
{
	TYPES::F64* mtx2=mtx->get();
	
	TYPES::F64 m_r =  mtx2[0];
	TYPES::F64 o_r =  mtx2[1];
	TYPES::F64 m_g =  mtx2[2];
	TYPES::F64 o_g =  mtx2[3];
	TYPES::F64 m_b =  mtx2[4];
	TYPES::F64 o_b =  mtx2[5];
	TYPES::F64 m_a =  mtx2[6];
	TYPES::F64 o_a =  mtx2[7];
	
	this->matrix_data[0]= m_r * this->matrix_data[0];
	this->matrix_data[1]= o_r +  (this->matrix_data[1] * m_r);
	this->matrix_data[2]= m_g * this->matrix_data[2];
	this->matrix_data[3]= o_g +  (this->matrix_data[3] * m_g);
	this->matrix_data[4]= m_b * this->matrix_data[4];
	this->matrix_data[5]= o_b +  (this->matrix_data[5] * m_b);
	this->matrix_data[6]= m_a * this->matrix_data[6];
	this->matrix_data[7]= o_a +  (this->matrix_data[7] * m_a);

	return result::AWD_SUCCESS;
}
result
ColorTransform::read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *)
{
	return result::AWD_SUCCESS;
}

result
ColorTransform::write_to_file(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings * settings)
{
	


	fileWriter->writeFLOAT32(TYPES::F32(this->matrix_data[0]));
	fileWriter->writeFLOAT32(TYPES::F32(this->matrix_data[2]));
	fileWriter->writeFLOAT32(TYPES::F32(this->matrix_data[4]));
	fileWriter->writeFLOAT32(TYPES::F32(this->matrix_data[6]));
	fileWriter->writeINT16(TYPES::INT16(this->matrix_data[1]));
	fileWriter->writeINT16(TYPES::INT16(this->matrix_data[3]));
	fileWriter->writeINT16(TYPES::INT16(this->matrix_data[5]));
	fileWriter->writeINT16(TYPES::INT16(this->matrix_data[7]));

	return result::AWD_SUCCESS;
}


#include "elements/geom_elements.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"

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
}
MATRIX2x3::MATRIX2x3()
{
	this->matrix_data = (TYPES::F64*)malloc(6 * sizeof(TYPES::F64));
	set(NULL);
}

MATRIX2x3::~MATRIX2x3()
{
	free(this->matrix_data);
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
		this->matrix_data[0]  = 1.0;		this->matrix_data[1]  = 0.0;		this->matrix_data[2]  = 0.0; 
		this->matrix_data[3]  = 0.0;		this->matrix_data[4]  = 1.0;		this->matrix_data[5]  = 0.0; 
	}
	else{
		for(int cnt=0; cnt<6; cnt++)
			this->matrix_data[cnt]=matrix_data[cnt];
	}

	return result::AWD_SUCCESS;
}

result
MATRIX2x3::read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *)
{
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

MATRIX4x5::MATRIX4x5(TYPES::F64 *mtx)
{
	this->matrix_data = (TYPES::F64*)malloc(20 * sizeof(TYPES::F64));
	set(mtx);
}
MATRIX4x5::MATRIX4x5()
{
	this->matrix_data = (TYPES::F64*)malloc(20 * sizeof(TYPES::F64));
	set(NULL);
}

MATRIX4x5::~MATRIX4x5()
{
	free(this->matrix_data);
}

TYPES::F64* 
MATRIX4x5::get(){
	return this->matrix_data;
}
result
MATRIX4x5::set(TYPES::F64 *matrix_data)
{
	// no values given. set from 
	if (matrix_data == NULL) {
		this->matrix_data[0]  = 1.0;		this->matrix_data[1]  = 0.0;		this->matrix_data[2]  = 0.0;    this->matrix_data[3]  = 0.0;		
		this->matrix_data[4]  = 1.0;		this->matrix_data[5]  = 0.0; 		this->matrix_data[6]  = 0.0;	this->matrix_data[7]  = 0.0;	
		this->matrix_data[8]  = 1.0;		this->matrix_data[9]  = 2.50; 		this->matrix_data[10] = 1.5;	this->matrix_data[11] = 0.8;
		this->matrix_data[12]  = 1.0;		this->matrix_data[13]  = 2.50; 		this->matrix_data[14] = 1.5;	this->matrix_data[15] = 0.8;
		this->matrix_data[16]  = 1.0;		this->matrix_data[17]  = 2.50; 		this->matrix_data[18] = 1.5;	this->matrix_data[19] = 0.8;
	}
	else{
		for(int cnt=0; cnt<20; cnt++)
			this->matrix_data[cnt]=matrix_data[cnt];
	}

	return result::AWD_SUCCESS;
}

result
MATRIX4x5::read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *)
{
	return result::AWD_SUCCESS;
}

result
MATRIX4x5::write_to_file(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings * settings)
{
	result res = result::AWD_SUCCESS;
	// matrix4x5 is not scale-able, so we can write it to file in one go.
	if(settings->get_scale()==1)
		return fileWriter->writeNumbers(this->matrix_data, 20, settings->get_wide_matrix());		

	return result::AWD_SUCCESS;
}


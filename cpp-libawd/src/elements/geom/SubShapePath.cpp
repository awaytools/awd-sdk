#include "blocks/geometry.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "utils/util.h"
#include "utils/awd_globals.h"

#include <math.h>       
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
 


SubShapePath::SubShapePath()
{
	this->color=0;
	this->thickness=0;
	this->capStyle=0;
	this->jointStlye=0;
	this->mitter=0;
}

SubShapePath::~SubShapePath()
{
}
TYPES::UINT32
SubShapePath::get_bytesize(){
	TYPES::UINT32 byteSize=0;
	byteSize += 4; // length element
	byteSize += 8; // properties + attr

	byteSize += 1;// stream_type
	byteSize += 1;// stream_data_type
	byteSize += 4;// stream_length

	byteSize+=4;//color
	byteSize+=4;//thickness
	byteSize+=1;//capStyle
	byteSize+=1;//jointStlye
	byteSize+=4;//mitter
	//byteSize+=4;//count pathdata
	byteSize+=this->path_data.size()*4;

	return byteSize;

}
result
SubShapePath::writeToFile(FileWriter* fileWriter){
	
	result res= result::AWD_SUCCESS;
	int len = 4 + 6 + 14  + this->path_data.size()*4;
	fileWriter->writeUINT32(len); // 6stream header + 12 streamdata + 4 props
	fileWriter->writeUINT32(0); // properties
		
	res = fileWriter->writeUINT8(24);
	res = fileWriter->writeUINT8(0);
	res = fileWriter->writeUINT32(14 + this->path_data.size() * 4);

	fileWriter->writeUINT32(this->color);
	fileWriter->writeFLOAT32(this->thickness);
	fileWriter->writeUINT8(this->capStyle);
	fileWriter->writeUINT8(this->jointStlye);
	fileWriter->writeFLOAT32(this->mitter);
	for(TYPES::F32 elem:this->path_data){

		res = fileWriter->writeFLOAT32(elem);
	}

	fileWriter->writeUINT32(0); // user attr
	return res;
}
#include "base/datablock.h"
#include "utils/util.h"

#include <sstream>
#include <fstream>
#include <istream>
#include <stdio.h>
using namespace AWD;
using namespace AWD::BASE;
using namespace AWD::TYPES;

DataBlockBase::DataBlockBase(const std::string& url)
{
	this->url = url;
	this->storage = BLOCK::storage_type::EMBEDDED;
	this->embed_data = NULL;
	this->embed_data_len = 0;
}

DataBlockBase::~DataBlockBase()
{
	if (this->embed_data_len>0) free(this->embed_data);
	this->embed_data = NULL;
	this->embed_data_len = 0;
}

BLOCK::storage_type
DataBlockBase::get_storage_type()
{
	return this->storage;
}
void
DataBlockBase::set_storage_type(BLOCK::storage_type storage)
{
	this->storage = storage;
}

std::string&
DataBlockBase::get_input_url()
{
	return this->input_url;
}
void
DataBlockBase::set_input_url(std::string& input_url)
{
	this->input_url = input_url;
}
std::string&
DataBlockBase::get_url()
{
	return this->url;
}
void
DataBlockBase::set_url(std::string& url)
{
	this->url = url;
}
void
DataBlockBase::set_embed_data(char *buf, TYPES::UINT32 buf_len)
{
	this->embed_data = buf;
	this->embed_data_len = buf_len;
}

TYPES::UINT32 
DataBlockBase::calc_bytes_length(SETTINGS::BlockSettings * settings)
{
	TYPES::UINT32 len;
	len = TYPES::UINT32(sizeof(TYPES::UINT32));
	if (storage==BLOCK::storage_type::EXTERNAL){
		len += TYPES::UINT32(this->url.size());   
	}
	else 
	{
		//copy the external file into this->embed_data and get the length 
		if(this->input_url.size()>0){
			std::string extracted;
			if(FILES::extract_directory_from_path(this->input_url, extracted)==result::AWD_SUCCESS){
				std::ifstream inFile(this->input_url.c_str(), std::ifstream::in | std::ifstream::binary);
				if(inFile){
					inFile.seekg (0, inFile.end);
					long length = (long)inFile.tellg();
        
					inFile.seekg (0, inFile.beg);
					this->embed_data = new char [length + 1];
					inFile.read (this->embed_data, length);
					this->embed_data[length] = 0;
					inFile.close();
					this->embed_data_len=length;
				}
			}
		}
		len += this->embed_data_len;    
	}

	return len;
}

void
DataBlockBase::write_bytes(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file)
{
	if (storage == BLOCK::storage_type::EXTERNAL) {
		fileWriter->writeSTRING(this->url, FILES::write_string_with::LENGTH_AS_UINT32);
	}
	else if (storage == BLOCK::storage_type::EMBEDDED) {
		TYPES::UINT32 data_len;
		data_len = this->embed_data_len;
		fileWriter->writeUINT32(data_len);
		if(this->embed_data_len>0){
			fileWriter->writeBytes((TYPES::UINT8*)this->embed_data,  this->embed_data_len);
		}
	}
}
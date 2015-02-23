#include "files/file_reader.h"
#include "utils/util.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;

FileReader::FileReader()
{
	this->file=NULL;

	//	test if system works with BigEndian or LittleEndian.
	//	if it is littleEndian, we will swap bytes before writing to file
	this->swapBytes=false;
	int num = 1;
	if(*(char *)&num == 1)
	{
		this->swapBytes=false;
	}
	else
	{
		this->swapBytes=true;
	}
}
FileReader::~FileReader()
{
}

result 
FileReader::get_pos(TYPES::UINT32 pos)
{
	TYPES::INT32 pos_int = ftell(this->file);
    if(pos_int<0)
		return result::FILEREADER_ERROR_GETTING_POS;
    pos=TYPES::UINT32(pos_int);
	return result::AWD_SUCCESS;
}
result 
FileReader::set_pos(TYPES::UINT32 pos)
{
    if(!fseek(this->file, pos, SEEK_SET))
		return result::FILEREADER_ERROR_SETTING_POS;
	return result::AWD_SUCCESS;
}
result
FileReader::open_file(std::string& file)
{

	if(this->file!=NULL){
		if(!fclose(this->file))
			return result::FILEREADER_ERROR_CLOSING_FILE;
	}
	this->file = fopen (file.c_str(), "rb");
	if(this->file==NULL){
		return result::FILEREADER_ERROR_OPENING_FILE;
	}
	return result::AWD_SUCCESS;
}
result
FileReader::set_file(FILE* file)
{
	if(this->file!=NULL){
		if(!fclose(this->file))
			return result::FILEREADER_ERROR_CLOSING_FILE;
	}
	this->file = file;
	if(this->file==NULL){
		return result::FILEREADER_ERROR_OPENING_FILE;
	}
	return result::AWD_SUCCESS;
}
result
FileReader::readBOOL(bool* outputBool)
{
    if(!(fread(outputBool, sizeof(bool), 1, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	return result::AWD_SUCCESS;
}
result
FileReader::readUINT8(TYPES::UINT8* outputBool)
{
    if(!(fread(outputBool, sizeof(TYPES::UINT8), 1, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	return result::AWD_SUCCESS;
}
result
FileReader::readBytes(TYPES::UINT8* outputBool, int length)
{
    if(!(fread(outputBool, sizeof(TYPES::UINT8), length, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	return result::AWD_SUCCESS;
}
result
FileReader::readUINT16(TYPES::UINT16* outputBool)
{
    if(!(fread(outputBool, sizeof(TYPES::UINT16), 1, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	if(this->swapBytes)
		*outputBool=FILES::swapui16(*outputBool);
	return result::AWD_SUCCESS;
}
result
FileReader::readUINT16multi(TYPES::UINT16* outputBool, int length)
{
    if(!(fread(outputBool, sizeof(TYPES::UINT16), length, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	//TODO: swap bytes
	return result::AWD_SUCCESS;
}
result
FileReader::readUINT32(TYPES::UINT32* outputBool)
{
    if(!(fread(outputBool, sizeof(TYPES::UINT32), 1, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	if(this->swapBytes)
		*outputBool=FILES::swapui32(*outputBool);
	return result::AWD_SUCCESS;
}
result
FileReader::readFLOAT32(TYPES::F32* outputBool)
{
    if(!(fread(outputBool, sizeof(TYPES::F32), 1, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	if(this->swapBytes)
		*outputBool= FILES::swapf32(*outputBool);
	return result::AWD_SUCCESS;
}
result
FileReader::readFLOAT32multi(TYPES::F32* outputBool, int length)
{
    if(!(fread(outputBool, sizeof(TYPES::F32), length, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	//TODO: swap bytes
	return result::AWD_SUCCESS;
}
result
FileReader::readFLOAT64(TYPES::F64* outputBool)
{
    if(!(fread(outputBool, sizeof(TYPES::F64), 1, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	if(this->swapBytes)
		*outputBool= FILES::swapf64(*outputBool);
	return result::AWD_SUCCESS;
}
result
FileReader::readFLOAT64multi(TYPES::F64* outputBool, int length)
{
    if(!(fread(outputBool, sizeof(TYPES::F64), length, this->file)))
		return result::FILEREADER_ERROR_READING_BYTES;
	//todo: swap bytes
	return result::AWD_SUCCESS;
}
result
FileReader::readSTRING_FIXED(std::string& stringtofill, TYPES::UINT32 char_count)
{
	//todo: swap bytes(?)
    char * thisString = (char *)malloc(sizeof(char)*char_count+1);
    if(!(fread(thisString, char_count, 1, this->file))){
		thisString[char_count] = 0;
		free(thisString);
        return result::FILEREADER_ERROR_READING_BYTES;
	}
    thisString[char_count] = 0;
	stringtofill=thisString;
    return result::AWD_SUCCESS;
}
result
FileReader::readSTRING(std::string& stringtofill, FILES::write_string_with readLength)
{
	TYPES::UINT32 char_count;
	TYPES::UINT32 bytesize=sizeof(TYPES::UINT16);
	if(readLength==FILES::write_string_with::LENGTH_AS_UINT32){
		bytesize=sizeof(TYPES::UINT32);
	}
    if(!(fread(&char_count, bytesize, 1, this->file)))
        return result::FILEREADER_ERROR_READING_BYTES;
	//todo: swap bytes(?)
    char * thisString = (char *)malloc(sizeof(char)*char_count+1);
    if(!(fread(thisString, char_count, 1, this->file))){
		thisString[char_count] = 0;
		free(thisString);
        return result::FILEREADER_ERROR_READING_BYTES;
	}

    thisString[char_count] = 0;
	
    return result::AWD_SUCCESS;
}
result
FileReader::get_uncompress_data(TYPES::UINT32 compressed_length, compression compress)
{
	result res;
    int curPos = ftell(this->file);
    fseek(this->file,0,SEEK_END);
    int size = ftell(this->file);
    fseek(this->file, curPos, SEEK_SET);

    if (compressed_length!=size){
        // file is corrupt, but we still can try to read it
		return result::FILEREADER_ERROR_READING_BYTES;
    }

	if (compress==compression::UNCOMPRESSED){
		return result::AWD_SUCCESS;
	}
	if (compress==compression::ZLIB){
		TYPES::UINT8* buffer = (TYPES::UINT8*)malloc(compressed_length);
		if(!(fread(buffer, compressed_length, 1, this->file))){
			return result::FILEREADER_ERROR_READING_BYTES;
		}
		// to do: uncompress zlib
		res = this->set_file(tmpfile());
		if(res!=result::AWD_SUCCESS){
		}
		if(!fwrite(reinterpret_cast<const char*>(buffer), compressed_length, 1, this->file)){
			return result::WRITE_ERROR;
		}
		fflush (this->file);


		//TODO:		read the uncompressed data.
		//			uncompress the data
		//			put it into a temp file
		//			set the 
		return result::AWD_SUCCESS;
	}
	else if (compress==compression::UNCOMPRESSED){
		return result::AWD_SUCCESS;
	}

	
	return result::AWD_SUCCESS;

}

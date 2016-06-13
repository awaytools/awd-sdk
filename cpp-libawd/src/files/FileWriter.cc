#include "files/file_writer.h"
#include "utils/util.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;

FileWriter::FileWriter()
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

FileWriter::~FileWriter()
{
}

result
FileWriter::open_file(std::string& file)
{

	if(this->file!=NULL){
		if(fclose(this->file)!=0)
			return result::WRITE_ERROR;
	}
	this->file = fopen (file.c_str(), "wb");
	if(this->file==NULL)
		return result::WRITE_ERROR;
	return result::AWD_SUCCESS;
}

result
FileWriter::set_file(FILE* file)
{
	if(this->file!=NULL){
		if(!fclose(this->file))
			return result::WRITE_ERROR;
	}
	this->file = file;
	if(this->file==NULL)
		return result::WRITE_ERROR;
	return result::AWD_SUCCESS;
}

result
FileWriter::writeBOOL(bool val)
{
	if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(bool), 1, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeUINTSasSmallestData(std::vector<TYPES::UINT32> value_32)
{
	TYPES::UINT8 storage_precision = 1; //1: UINT8 2:UINT16 3:UINT32
	std::vector<TYPES::UINT16> value_16;
	std::vector<TYPES::UINT8> value_8;
	for(TYPES::UINT32 one_int: value_32){
		if(one_int >= std::numeric_limits<unsigned short int>::max()){
			storage_precision=4;
			break;
		}
		if((storage_precision==1)&&(one_int >= std::numeric_limits<unsigned char>::max())){
			storage_precision=2;
		}
		value_16.push_back(TYPES::UINT16(one_int));
		value_8.push_back(TYPES::UINT8(one_int));
	}
	this->writeUINT8(storage_precision);
	if(storage_precision==1){
		this->writeUINT32(value_8.size());
		this->writeUINT8multi(&value_8[0], value_8.size());
	}
	else if(storage_precision==2){
		this->writeUINT32(value_16.size() * 2);
		this->writeUINT16multi(&value_16[0], value_16.size());
	}
	else{
		this->writeUINT32(value_32.size() * 4);
		this->writeUINT32multi(&value_32[0], value_32.size());
	}


	return result::AWD_SUCCESS;
}
result
FileWriter::writeINT8(TYPES::INT8 val)
{
	if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::INT8), 1, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeINT8multi(TYPES::INT8* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::INT8), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}
result
FileWriter::writeUINT8(TYPES::UINT8 val)
{
	if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::UINT8), 1, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeUINT8multi(TYPES::UINT8* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::UINT8), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}
result
FileWriter::writeINT16(TYPES::INT16 val)
{
	if(this->swapBytes){
		TYPES::INT16 swaptVal=swapui16(val);
		if(!fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(TYPES::INT16), 1, this->file))
			return result::WRITE_ERROR;
	}
	else{
		if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::INT16), 1, this->file))
			return result::WRITE_ERROR;
	}
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeINT16multi(TYPES::INT16* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::INT16), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}
result
FileWriter::writeUINT16(TYPES::UINT16 val)
{
	if(this->swapBytes){
		TYPES::UINT16 swaptVal=swapui16(val);
		if(!fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(TYPES::UINT16), 1, this->file))
			return result::WRITE_ERROR;
	}
	else{
		if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::UINT16), 1, this->file))
			return result::WRITE_ERROR;
	}
	fflush (this->file);
	return result::AWD_SUCCESS;
}
result
FileWriter::writeUINT16multi(TYPES::UINT16* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::UINT16), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeINT32(TYPES::INT32 val)
{
	if(this->swapBytes){
		TYPES::INT32 swaptVal=swapui32(val);
		if(!fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(TYPES::INT32), 1, this->file))
			return result::WRITE_ERROR;
	}
	else{
		if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::INT32), 1, this->file))
			return result::WRITE_ERROR;
	}
	fflush (this->file);
	return result::AWD_SUCCESS;
}
result
FileWriter::writeINT32multi(TYPES::INT32* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::INT32), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeUINT32(TYPES::UINT32 val)
{
	if(this->swapBytes){
		TYPES::UINT32 swaptVal=swapui32(val);
		if(!fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(TYPES::UINT32), 1, this->file))
			return result::WRITE_ERROR;
	}
	else{
		if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::UINT32), 1, this->file))
			return result::WRITE_ERROR;
	}
	fflush (this->file);
	return result::AWD_SUCCESS;
}
result
FileWriter::writeUINT32multi(TYPES::UINT32* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::UINT32), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeBytes(TYPES::UINT8* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), length, 1, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}


result
FileWriter::writeFLOAT32(TYPES::F32 val)
{
	if(this->swapBytes){
		TYPES::F32 swaptVal=swapf32(val);
		if(!fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(TYPES::F32), 1, this->file))
			return result::WRITE_ERROR;
	}
	else{
		if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::F32), 1, this->file))
			return result::WRITE_ERROR;
	}
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeFLOAT32multi(TYPES::F32* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::F32), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeFLOAT64(TYPES::F64 val)
{
	if(this->swapBytes){
		TYPES::F64 swaptVal=swapf64(val);
		if(!fwrite(reinterpret_cast<const char*>(&swaptVal), sizeof(TYPES::F64), 1, this->file))
			return result::WRITE_ERROR;
	}
	else{
		if(!fwrite(reinterpret_cast<const char*>(&val), sizeof(TYPES::F64), 1, this->file))
			return result::WRITE_ERROR;
	}
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeFLOAT64multi(TYPES::F64* val, int length)
{
	if(!fwrite(reinterpret_cast<const char*>(val), sizeof(TYPES::F64), length, this->file))
		return result::WRITE_ERROR;
	fflush (this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::writeNumbers(TYPES::F64* val, int length, bool savePrecision)
{
	result res = result::AWD_SUCCESS;
	if(savePrecision){
		res=this->writeFLOAT64multi(val, length);
		if(res!=result::AWD_SUCCESS){
			return res;
		}
	}
	else{		
		TYPES::F32 *pf = (TYPES::F32*)malloc(sizeof(TYPES::F32)*length);				
		for (int e=0; e<length; e++) {
			TYPES::F64 *p = (val + e);
			TYPES::F32 elem = ((TYPES::F32)*p);
			pf[e]=elem;
		}
		res=this->writeFLOAT32multi((TYPES::F32*)pf, length);
		free(pf);
		if(res!=result::AWD_SUCCESS){
			return res;
		}
	}
	return result::AWD_SUCCESS;
}

result
FileWriter::writeSTRING(std::string& s, write_string_with writeLength)
{
	unsigned int N((unsigned int)s.size());
	if(writeLength==write_string_with::LENGTH_AS_UINT8){
		TYPES::UINT8 awduint8=TYPES::UINT8(N);
		if(!fwrite(&awduint8,sizeof(TYPES::UINT8), 1 ,this->file))
			return result::WRITE_ERROR;
	}
	if(writeLength==write_string_with::LENGTH_AS_UINT16){
		TYPES::UINT16 awduint16=TYPES::UINT16(N);
		if(!fwrite(&awduint16,sizeof(TYPES::UINT16), 1 ,this->file))
			return result::WRITE_ERROR;
	}
	if(writeLength==write_string_with::LENGTH_AS_UINT32){
		TYPES::UINT32 awduint32=TYPES::UINT32(N);
		if(!fwrite(&awduint32,sizeof(TYPES::UINT32), 1 ,this->file))
			return result::WRITE_ERROR;
	}
	if(!fwrite(s.c_str(),1, N ,this->file)){
		return result::WRITE_ERROR;
	}

	fflush(this->file);
	return result::AWD_SUCCESS;
}

result
FileWriter::get_compressed_bytes(TYPES::UINT8** buffer_to_compress, SETTINGS::compression compress, TYPES::UINT32 final_body_length)
{
	long body_length = 0;
	rewind(this->file);
	if (fseek(this->file,0,SEEK_END)==0){
		body_length = ftell(this->file);
	}
	rewind(this->file);
	// read the tmp file into a tmp-buffer and close it. it is no longer needed
	TYPES::UINT8 *tmp_buf = (TYPES::UINT8 *) malloc(body_length);
	if(!fread(tmp_buf, 1, body_length, this->file)){
		return result::WRITE_ERROR;
	}	
	if(!buffer_to_compress)
		return result::WRITE_ERROR;
	// create a new buffer that contains the compressed body
	*buffer_to_compress = (TYPES::UINT8 *) malloc(body_length);
	
	if (compress == compression::UNCOMPRESSED) {
		// Uncompressed, so output should be the exact
		// same data that was in the temporary file
		final_body_length = body_length;
		*buffer_to_compress = tmp_buf;
		return result::AWD_SUCCESS;
	}
	
#ifndef USECOMPRESSION	
	final_body_length = body_length;
	*buffer_to_compress = tmp_buf;
	return result::AWD_SUCCESS;
#else
	if (compress == compression::ZLIB) {
		// compress using deflate option of zlib - library
		int zlib_len = body_length;
		TYPES::UINT8 * zlib_buf = (TYPES::UINT8*)malloc(zlib_len);
		bool done = false;		

		z_streamp zstrm = (z_streamp)malloc(sizeof(z_stream_s));
		zstrm->zalloc = awd_zalloc;
		zstrm->zfree = awd_zfree;
		zstrm->opaque = NULL;
		zstrm->next_in = tmp_buf;
		zstrm->avail_in = body_length;
		zstrm->next_out = zlib_buf;
		zstrm->avail_out = zlib_len;
		int stat = deflateInit(zstrm, 9);
		while (!done) {
			stat = deflate(zstrm, Z_NO_FLUSH);
			switch (stat) {
				case Z_STREAM_END:
				case Z_BUF_ERROR:
					done = true;
					break;
			}
		}
		deflate(zstrm, Z_FINISH);
		deflateEnd(zstrm);

		final_body_length = zstrm->total_out;
		*buffer_to_compress = (TYPES::UINT8*)malloc(final_body_length);
		memcpy(buffer_to_compress, zlib_buf, final_body_length);

		free(zlib_buf);
		free(zstrm);
		free(tmp_buf);
		return result::AWD_SUCCESS;
	}
	else if (compress == SETTINGS::compression::LZMA) {
		Byte *lzma_buf;
		SizeT lzma_len, props_len;
		CLzmaEncProps props;
		ISzAlloc alloc;
		Byte *props_buf;
		long tmp_len_bo;

		// Create allocation structure. LZMA library uses
		// these functions for memory management. They are
		// defined in awdlzma.c as simple wrappers for
		// malloc() and free().
		alloc.Alloc = &awd_SzAlloc;
		alloc.Free = &awd_SzFree;

		lzma_len = body_length;
		lzma_buf = (Byte *)malloc(body_length);
		props_len = sizeof(CLzmaEncProps);
		props_buf = (Byte *)malloc(props_len);

		LzmaEncProps_Init(&props);
		props.algo = 1;
		props.level = 9;

		int result=LzmaEncode(lzma_buf, &lzma_len, tmp_buf, body_length, &props, props_buf, &props_len, 0, NULL, &alloc, &alloc);
		if (result!=SZ_OK)
			int error=0;

		// Body length is the length of the actual
		// compressed body + size of props and an integer
		// definining the uncompressed length (see below)
		final_body_length = (TYPES::UINT32)lzma_len + (TYPES::UINT32)props_len + sizeof(TYPES::UINT32);

		// Create new buffer containing LZMA props, length
		// of uncompressed body and the actual body data
		// concatenated together.
		tmp_len_bo = body_length;
		*buffer_to_compress = (TYPES::UINT8*)malloc(final_body_length);
		memcpy(buffer_to_compress, props_buf, props_len);
		memcpy(buffer_to_compress, &tmp_len_bo, sizeof(TYPES::UINT32));
		memcpy(buffer_to_compress, lzma_buf, lzma_len);
		free(tmp_buf);
		return result::AWD_SUCCESS;
	}
	return result::AWD_SUCCESS;
#endif
		
}

/**
\brief Writes a union according to the given  TYPES::data_types and settings.
*/
result 
FileWriter::writeUnion(union_ptr val, TYPES::data_types data_type, TYPES::UINT32 data_length, bool use_precise, bool use_scale, TYPES::F64 scale)
{

	TYPES::UINT32 bytes_written=0;
	TYPES::F64 this_value64;
	TYPES::F32 this_value32;	
	if(use_precise)
		data_length = data_length/2;
	while (bytes_written < data_length) {
		// Check type, and write data accordingly
		switch (data_type) {
			case TYPES::data_types::INT8:
				this->writeINT8(*val.i8);
				bytes_written += sizeof(TYPES::INT8);
				val.i8++;
				break;
			case TYPES::data_types::UINT8:
				this->writeUINT8(*val.ui8);
				bytes_written += sizeof(TYPES::UINT8);
				val.ui8++;
				break;
			case TYPES::data_types::INT16:
				this->writeINT16(*val.i16);
				bytes_written += sizeof(TYPES::INT16);
				val.i16++;
				break;
			case TYPES::data_types::UINT16:
				this->writeUINT16(*val.ui16);
				bytes_written += sizeof(TYPES::UINT16);
				val.ui16++;
				break;
			case TYPES::data_types::INT32:
				this->writeINT32(*val.i32);
				bytes_written += sizeof(TYPES::INT32);
				val.i32++;
				break;
			case TYPES::data_types::UINT32:
			case TYPES::data_types::BADDR:
			case TYPES::data_types::COLOR:
				this->writeUINT32(*val.ui32);
				bytes_written += sizeof(TYPES::UINT32);
				val.ui32++;
				break;
			case TYPES::data_types::FLOAT32:
				// \todo
				break;
			case TYPES::data_types::FLOAT64:
			case TYPES::data_types::VECTOR2x1:
			case TYPES::data_types::VECTOR3x1:
			case TYPES::data_types::VECTOR4x1:
				{
				if(use_precise){
					this_value64 = *val.F64;
					if(use_scale)
						 this_value64 *= scale;
					this->writeFLOAT64(this_value64);
					bytes_written += sizeof(TYPES::F64);
					val.F64++;
				}
				else{
					this_value32 = TYPES::F32(*val.F64);
					if(use_scale)
						 this_value32 = TYPES::F32(this_value32 * scale);
					this->writeFLOAT32(this_value32);
					bytes_written += sizeof(TYPES::F32);
					val.F64++;
					break;
				}
				break;
				}

			case TYPES::data_types::STRING:
				{
				// Write entire string in one go
				std::string thisString(val.str);
				this->writeSTRING(thisString, write_string_with::NO_LENGTH_VALUE);
				bytes_written += data_length;
				break;
				}

			case TYPES::data_types::BOOL:
				this->writeBOOL(*val.b);
				bytes_written +=  sizeof(bool);
				val.b++;
				break;
				
			case TYPES::data_types::MTX3x2:
			case TYPES::data_types::MTX3x3:
			case TYPES::data_types::MTX4x3:
			case TYPES::data_types::MTX4x4:
			case TYPES::data_types::MTX5x4:
				{
				if(use_precise){
					this_value64 = *val.mtx;
					if(use_scale)
						 this_value64 *= scale;
					this->writeFLOAT64(this_value64);
					bytes_written += sizeof(TYPES::F64);
					val.F64++;
				}
				else{
					this_value32 = TYPES::F32(*val.mtx);
					if(use_scale)
						 this_value32 = TYPES::F32(this_value32 * scale);
					this->writeFLOAT32(this_value32);
					bytes_written += sizeof(TYPES::F32);
					val.F64++;
					break;
				}
				break;
				}

			default:
				return result::AWD_ERROR;
		}
	}
	return result::AWD_SUCCESS;
}
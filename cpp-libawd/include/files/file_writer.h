#ifndef _LIBAWD_FILEWRITER_H
#define _LIBAWD_FILEWRITER_H

#include <stdlib.h>
#include <stdio.h>

#include <string>
#ifdef USECOMPRESSION
#include <zlib.h>
#include "LzmaEnc.h"
#include "files/awd_lzma.h"
#include "files/awd_zlib.h"
#endif
#include "utils/awd_types.h"
#include "utils/settings.h"

namespace AWD
{
	namespace FILES
	{
		//#include <crtdbg.h>
		class FileWriter
		{
			private:
				// File header fields
				FILE * file;
				bool swapBytes;

			public:
				FileWriter();
				~FileWriter();
		
				result open_file(std::string& file);
				result set_file(FILE* file);

				result writeBOOL(bool);

				FILE* get_file();
				
				result writeUINTSasSmallestData(std::vector<TYPES::UINT32>);
				result writeINT8(TYPES::INT8);
				result writeINT8multi(TYPES::INT8* val, int length);
				result writeUINT8(TYPES::UINT8);
				result writeUINT8multi(TYPES::UINT8* val, int length);
				result writeINT16(TYPES::INT16);
				result writeINT16multi(TYPES::INT16* val, int length);
				result writeUINT16(TYPES::UINT16);
				result writeUINT16multi(TYPES::UINT16* val, int length);
				result writeINT32(TYPES::INT32);
				result writeINT32multi(TYPES::INT32* val, int length);
				result writeUINT32(TYPES::UINT32);
				result writeUINT32multi(TYPES::UINT32* val, int length);
				result writeFLOAT32(TYPES::F32);
				result writeFLOAT32multi(TYPES::F32* val, int length);
				result writeFLOAT64(TYPES::F64);
				result writeFLOAT64multi(TYPES::F64* val, int length);
				result writeBytes(TYPES::UINT8* val, int length);
				result writeSTRING(std::string& s, FILES::write_string_with output_style);
				result writeNumbers(TYPES::F64* val, int length,bool savePrecision);

				/**
				* \brief Write a unsion to the file.
				*@param[in] val The union to write to file
				*@param[in] data_type The type of data to write 
				*@param[in] data_length The length of the data. For Numbers, this should be the length of the data as F64
				*@param[in] use_precise If true, Numbers are writen as F64 otherwise F32
				*@param[in] use_scale If true, the values writing to file will be scaled.
				*@param[in] scale The scale to use.
				*@return result
				*\todo The LZMA compression is not working.
				*/
				result writeUnion(TYPES::union_ptr val, TYPES::data_types data_type, TYPES::UINT32 data_length, bool use_precise, bool use_scale, TYPES::F64 scale);

				/**
				*\briefReads the current file from disc, and fills the given buffer with optionally compressed data
				*@param[out] buffer A Pointer to the buffer that should be filled.
				*@param[in] compress The compression to use.
				*@return TYPES::UINT32 Length of compressed buffer
				*\todo The LZMA compression is not working.
				*/
				result get_compressed_bytes(TYPES::UINT8** buffer_to_compress, SETTINGS::compression compress, TYPES::UINT32 final_body_length);
		};
	}
}
#endif

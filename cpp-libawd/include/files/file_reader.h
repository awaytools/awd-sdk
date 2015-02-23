#ifndef _LIBAWD_FILEREADER_H
#define _LIBAWD_FILEREADER_H

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

namespace AWD
{
	namespace FILES
	{
		//#include <crtdbg.h>
		class FileReader
		{
			private:
				// File header fields
				FILE * file;
				bool swapBytes;
				bool all_parsed;

			public:
				FileReader();
				~FileReader();
		
				bool keep_parsing();
				
				/**
				*\brief Get a new FILE for binary reading and sets it on the FileReader.
				*@param[out] filepath The url to open
				*@return result
				*/
				result get_pos(TYPES::UINT32);
				/**
				*\brief Get a new FILE for binary reading and sets it on the FileReader.
				*@param[out] filepath The url to open
				*@return result
				*/
				result set_pos(TYPES::UINT32);
				/**
				*\brief Opens a new FILE for binary reading and sets it on the FileReader.
				*@param[out] filepath The url to open
				*@return result
				*/
				result open_file(std::string& filepath);

				/**
				*\brief Set a new File to use for the FileReader. If a File was set already, it will be closed.
				*@param[out] file Pointer to the FILE that should be set.
				*@return result
				*/
				result set_file(FILE* file);

				/**
				*\brief Read a single boolean byte from the file
				*@param[out] val A Pointer to the bool that will be filled.
				*@return result
				*/
				result readBOOL(bool*);

				/**
				*\brief Read a single TYPES::UINT8 from the file
				*@param[out] val A Pointer to the TYPES::UINT8 that will be filled.
				*@return result
				*/
				result readUINT8(TYPES::UINT8*);

				/**
				*\brief Read a single TYPES::UINT16 from the file
				*@param[out] val A Pointer to the TYPES::UINT16 that will be filled.
				*@return result
				*/
				result readUINT16(TYPES::UINT16*);

				/**
				*\brief Read a single TYPES::UINT32 from the file
				*@param[out] val A Pointer to the TYPES::UINT32 that will be filled.
				*@return result
				*/
				result readUINT32(TYPES::UINT32*);

				/**
				*\brief Read a single TYPES::F32 from the file
				*@param[out] val A Pointer to the TYPES::F32 that will be filled.
				*@return result
				*/
				result readFLOAT32(TYPES::F32*);

				/**
				*\brief Read a single TYPES::F64 from the file
				*@param[out] val A Pointer to the TYPES::F64 that will be filled.
				*@return result
				*/
				result readFLOAT64(TYPES::F64*);

				/**
				*\brief Read a number of bytes from the file
				*@param[out] val A Pointer to the list of TYPES::UINT8 that will be filled.
				*@param[in] length The length of the list.
				*@return result
				*/
				result readBytes(TYPES::UINT8* val, int length);

				/**
				*\brief Read a std::string from the file. the number of bytes to read is retrieved from the file before reading the string.
				*@param[out] output_string A Pointer to the std::string that will be filled.
				*@param[in] output_style FILES::write_string_with defines type of interger that should be read for the string-length
				*@return result
				*/
				result readSTRING(std::string& output_string, FILES::write_string_with output_style);

				/**
				*\brief Read a std::string from the file. The number of bytes to read must be given.
				*@param[out] output_string A Pointer to the std::string that will be filled.
				*@param[in] string_length FILES::TYPES::UINT32 The number of bytes to read
				*@return result
				*/
				result readSTRING_FIXED(std::string& output_string, TYPES::UINT32 string_length);

				/**
				*\brief Read a list of TYPE::TYPES::F32 from the file
				*@param[out] val A Pointer to the list that will be filled.
				*@param[in] length The length of the list.
				*@return result
				*/
				result readFLOAT32multi(TYPES::F32* val, int length);

				/**
				*\brief Read a list of TYPE::TYPES::F64 from the file
				*@param[out] val A Pointer to the list that will be filled.
				*@param[in] length The length of the list.
				*@return result
				*/
				result readFLOAT64multi(TYPES::F64* val, int length);

				/**
				*\brief Read a list of TYPE::TYPES::UINT16 from the file
				*@param[out] val A Pointer to the list that will be filled.
				*@param[in] length The length of the list.
				*@return result
				*/
				result readUINT16multi(TYPES::UINT16* val, int length);

				/**
				*\brief Reads the current file from disc, and fills the given buffer with optionally uncompressed data
				*@param[out] buffer A Pointer to the buffer that should be filled.
				*@param[in] compress The compression to use.
				*@return TYPES::UINT32 Length of compressed buffer
				*\todo The LZMA compression is not working.
				*/
				result get_uncompress_data(TYPES::UINT32 uncompressed_length, SETTINGS::compression compress);
		};
	}
}
#endif

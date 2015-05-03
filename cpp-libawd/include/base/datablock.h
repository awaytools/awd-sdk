#ifndef LIBAWD_DATA_BLOCK_H
#define LIBAWD_DATA_BLOCK_H
#include <string>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"

#include "files/file_writer.h"
#include "files/file_reader.h"

namespace AWD
{
	namespace BASE
	{
		/** \class DataBlockBase
		*	\brief Allows to save data from a file into a block. Should never be instantiated directly.
		*	The data can be saved external (only the path to the file is saved) or embbed (byteArray from file is copied into the block)
		*/
		class DataBlockBase
		{
			private:
				BLOCK::storage_type storage;
				std::string url;
				std::string input_url;
				char *embed_data;
				

			public:
				DataBlockBase(const std::string&);
				~DataBlockBase();
				
				TYPES::UINT32 embed_data_len;
				void set_embed_data(char *, TYPES::UINT32);

				std::string& get_url();
				void set_url(std::string&);
				std::string& get_input_url();
				void set_input_url(std::string&);
			
				TYPES::UINT32 calc_bytes_length(SETTINGS::BlockSettings *);
				void write_bytes(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);

				BLOCK::storage_type get_storage_type();
				void set_storage_type(BLOCK::storage_type);
		};
	}
}
#endif

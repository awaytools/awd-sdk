#ifndef LIBAWD_AUDIO_H
#define LIBAWD_AUDIO_H
#include <string>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/datablock.h"

#include "files/file_writer.h"
#include "base/state_element_base.h"

namespace AWD
{
	namespace BLOCKS
	{
		/** \class Audio
		*	\brief Descripes a AWDBlock for [Away3D BitmapTexture](http://www.away3d.com) 
		*
		*/
		class Audio :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase,
			public BASE::DataBlockBase
		{
			private:


			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);

			public:
				Audio(std::string&);
				Audio();
				~Audio();
			
		};

	}
}
#endif

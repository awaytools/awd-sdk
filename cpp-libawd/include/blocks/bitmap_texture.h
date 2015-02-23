#ifndef LIBAWD_BITMAP_TEXTURE_H
#define LIBAWD_BITMAP_TEXTURE_H
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
		/** \class BitmapTexture
		*	\brief Descripes a AWDBlock for [Away3D BitmapTexture](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_13_2
		*
		*/
		class BitmapTexture :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase,
			public BASE::DataBlockBase
		{
			private:
				int height;
				int width;
				std::string uvAnimSourceId;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);

			public:
				BitmapTexture(std::string&);
				BitmapTexture();
				~BitmapTexture();

			
				std::string& get_uvAnimSourceId();
				void set_uvAnimSourceId(std::string&);

				int get_height();
				void set_height(int);
				int get_width();
				void set_width(int);
		};

	}
}
#endif

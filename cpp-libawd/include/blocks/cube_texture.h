#ifndef LIBAWD_CUBE_TEXTURE_H
#define LIBAWD_CUBE_TEXTURE_H
#include <string>
#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"

#include "files/file_writer.h"
#include "blocks/bitmap_texture.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

namespace AWD
{
	namespace BLOCKS
	{
		
		/** \class CubeTexture
		*	\brief Descripes a AWDBlock for [Away3D CubeTexture](http://www.away3d.com)  as defined by AWDSpec: \ref awd_pt2_13_3
		*
		*/
		class CubeTexture :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				BLOCK::storage_type saveType;
				std::vector<BitmapTexture *> texture_blocks;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);

			public:
				CubeTexture(std::string&);
				CubeTexture();
				~CubeTexture();

				std::vector<BitmapTexture *> get_texture_blocks();
				void set_texture_blocks(std::vector<BitmapTexture *>);

				BLOCK::storage_type get_tex_type();
				void set_tex_type(BLOCK::storage_type);
		};
	}
}
#endif

#ifndef LIBAWD_FONT_H
#define LIBAWD_FONT_H

#include "utils/awd_types.h"
#include "elements/subgeometry.h"
#include "elements/fonts.h"
#include "base/block.h"

#include "base/attr.h"
#include <map>
namespace AWD
{
	namespace BLOCKS
	{
		
		class Font :
			public AWDBlock,
			public AttrElementBase
		{
			private:
				std::vector<FONT::FontStyle*> font_styles;
				bool from_library;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				Font(const std::string& name);
				Font();
				~Font();

				FONT::FontStyle* get_font_style(const std::string&);
				std::vector<FONT::FontStyle*> get_font_styles();
		};
	}
}
#endif

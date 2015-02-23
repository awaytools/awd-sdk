#ifndef LIBAWD_TEXT_ELEMENT_H
#define LIBAWD_TEXT_ELEMENT_H

#include "utils/awd_types.h"
#include "elements/subgeometry.h"
#include "elements/fonts.h"
#include "base/block.h"

#include "base/attr.h"
#include "elements/fonts.h"
#include <map>
namespace AWD
{
	namespace BLOCKS
	{
		
		class TextElement :
			public AWDBlock,
			public AttrElementBase
		{
			private:
				std::vector<FONT::Paragraph*> textParagraphs;
				std::string text;
				bool isLocalized;
		
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				
			public:
				TextElement(const std::string& name);
				TextElement();
				~TextElement();
		
				std::string& get_text();
				bool get_isLocalized();
				void set_isLocalized(bool);
				void set_text(const std::string&);
				void add_paragraph(FONT::Paragraph*);

		};
	}
}
#endif

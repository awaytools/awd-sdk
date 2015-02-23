#ifndef LIBAWD_TEXT_FORMAT_H
#define LIBAWD_TEXT_FORMAT_H

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
		
		class TextFormat:
			public AWDBlock,
			public AttrElementBase
		{
			private:
				std::string fontName;
				std::string fontStyle;
				int fontSize;
				int letterSpacing;
				AWDBlock* fill;
				FONT::baselineshift_type baseLineShift;
		
			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile* ,SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);
			public:
				TextFormat(const std::string& name);
				TextFormat();
				~TextFormat();
				AWDBlock* get_fill();
				void set_fill(AWDBlock*);
				FONT::baselineshift_type get_baseLineShift();
				void set_baseLineShift(FONT::baselineshift_type);
				std::string& get_fontStyle();
				void set_fontStyle(const std::string&);
				std::string& get_fontName();
				void set_fontName(const std::string&);
				int get_fontSize();
				void set_fontSize(int);
				int get_letterSpacing();
				void set_letterSpacing(int);
		};
	}
}
#endif

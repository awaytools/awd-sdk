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
				std::vector<double> uv_values;
				std::string fontName;
				std::string fontStyle;
				int fontSize;
				double letterSpacing;
				bool is_rotated;
				bool autokerning;
				AWDBlock* fill_material;
				AWDBlock* font;
				FONT::baselineshift_type baseLineShift;
				void init();
				
			
				FONT::AlignMode alignment;
				double indent;
				double line_spacing;
				double left_margin;
				double right_margin;
		
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
				AWDBlock* get_fill_material();
				void set_fill_material(AWDBlock*);

				void set_alignment(FONT::AlignMode alignment);
				void set_indent(double);
				void set_linespacing(double);
				void set_leftmargin(double);
				void set_rightmargin(double);


				FONT::baselineshift_type get_baseLineShift();
				void set_baseLineShift(FONT::baselineshift_type);
				std::string& get_fontStyle();
				void set_fontStyle(const std::string&);
				std::string& get_fontName();
				void set_fontName(const std::string&);
				AWDBlock* get_font();
				void set_font(AWDBlock*);
				int get_fontSize();
				void set_fontSize(int);
				double get_letterSpacing();
				void set_letterSpacing(double);
				bool get_is_rotated();
				void set_is_rotated(bool);
				bool get_autokerning();
				void set_autokerning(bool);
				result merge_for_textureatlas(BASE::AWDBlock* );
		};
	}
}
#endif

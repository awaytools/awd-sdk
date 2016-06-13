#ifndef LIBAWD_FONTS_H
#define LIBAWD_FONTS_H

#include "utils/awd_types.h"
#include "elements/subgeometry.h"
#include "base/block.h"

#include "base/attr.h"
#include <map>
namespace AWD
{
	namespace FONT
	{
		class TextRun:
			public AttrElementBase
		{
			private:
				std::string text;
				AWDBlock* textFormat;
		
			public:
				TextRun();
				~TextRun();
				AWDBlock* get_format();
				void set_format(AWDBlock*);
				std::string& get_text();
				void set_text(const std::string&);
				TYPES::UINT32 calc_body_length(SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter*, SETTINGS::BlockSettings *, FILES::AWDFile* awd_file);
				result add_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
		};
		class FontShape
		{
			private:
				int charCode;
				bool shape_data;
				GEOM::SubGeom* subShape;

			public:
				FontShape();
				~FontShape();
				double advanceValue;
				int get_charCode();
				void set_charCode(int);
				bool has_shape_data();
				GEOM::SubGeom* get_subShape();
				void set_subShape(GEOM::SubGeom*);
				TYPES::UINT32 calc_body_length(SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter*, SETTINGS::BlockSettings *, double font_size);
		};
		class FontStyle
		{
			private:
				std::string style_name;
				double style_size;
				double ascent;
				double descent;
				double whitespace_size;
				std::vector<FontShape*> shapes;

			public:
				FontStyle(const std::string&);
				~FontStyle();
				std::map<int, FontShape*> shapesmap;
				std::vector<FontShape*> get_ungenerated_chars();
				void set_style_size(int);
				void set_ascent(double);
				void set_descent(double);
				void set_whitespace_size(int);
				void delete_fontShape(int char_code);
				std::string& get_style_name();
				FontShape* get_fontShape(int);
				void get_fontshapes_from(FontStyle* font_shape);
				TYPES::UINT32 calc_body_length(SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter*, SETTINGS::BlockSettings *);
		};
		class Paragraph:
			public AttrElementBase
		{
			private:
				std::vector<TextRun*> textRuns;

			public:
				Paragraph();
				~Paragraph();
				void add_textrun(TextRun*);
				TYPES::UINT32 calc_body_length(SETTINGS::BlockSettings *);
				result write_body(FILES::FileWriter*, SETTINGS::BlockSettings *, FILES::AWDFile* awd_file);
				result add_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
		};
		
	}
}
#endif

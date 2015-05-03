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
				FONT::Textfield_type tf_type;
				bool is_selectable;
				bool render_as_html;
				bool is_border_drawn;
				bool is_scrollable;
				FONT::TextFlow textflow;
				FONT::OrientationMode orient_mode;
				FONT::LineMode line_mode;
				GEOM::BOUNDS2D bounds;
		
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

				void init();

				
				double text_width;
				double text_height;
				void set_bounds(GEOM::BOUNDS2D bounds);
				GEOM::BOUNDS2D get_bounds();
				void set_tf_type(FONT::Textfield_type tf_type);
				FONT::Textfield_type get_tf_type();

				void set_is_selectable(bool is_selectable);
				bool get_is_selectable();
				
				void set_render_as_html(bool render_as_html);
				bool get_render_as_html();
				
				void set_is_border_drawn(bool is_border_drawn);
				bool get_is_border_drawn();

				void set_is_scrollable(bool is_scrollable);
				bool get_is_scrollable();

								
				void set_textflow(FONT::TextFlow textflow);
				FONT::TextFlow get_textflow();
				
				void set_orientationMode(FONT::OrientationMode orient_mode);
				FONT::OrientationMode get_orientationMode();

				void set_line_mode(FONT::LineMode line_mode);
				FONT::LineMode get_line_mode();

				bool get_isLocalized();
				void set_isLocalized(bool);
				void set_text(const std::string&);
				std::string& get_text();
				void add_paragraph(FONT::Paragraph*);

		};
	}
}
#endif

#include "blocks/font.h"
#include "elements/fonts.h"
#include <sys/stat.h>

#include <sys/stat.h>

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::ATTR;
using namespace AWD::BLOCKS;
using namespace AWD::FONT;
using namespace AWD::BLOCK;

Font::Font(const std::string& name):
	AWDBlock(BLOCK::block_type::FONT, name),    
    AttrElementBase()
{
	this->delegate_to_font=NULL;
}

Font::Font():
	AWDBlock(block_type::FONT),
	AttrElementBase()
{
	this->delegate_to_font=NULL;
}
Font::~Font()
{
}

TYPES::state Font::validate_block_state()
{
	return TYPES::state::VALID;
}

FontStyle* Font::get_font_style(const std::string& fontstyleName)
{
	for(FontStyle* font_style: font_styles){
		if(font_style->get_style_name()==fontstyleName){
			return font_style;
		}
	}
	font_styles.push_back(new FontStyle(fontstyleName));
	return font_styles.back();
}

void Font::addFontCharsFromFont(Font* font){
	
	FontStyle* thisFontStlye=NULL;
	for(FontStyle* font_style: font_styles){
		thisFontStlye=font->get_font_style(font_style->get_style_name());
		if(thisFontStlye!=NULL){
			font_style->addFontCharsFromFont(thisFontStlye);
		}
		else{
			//todo: throw error
		}
	}
}
std::vector<FontStyle*> Font::get_font_styles()
{
	return font_styles;
}
TYPES::UINT32
Font::calc_body_length(AWDFile* awd_file, SETTINGS::BlockSettings * settings)
{
    TYPES::UINT32 len=0;
	
	len += sizeof(TYPES::UINT16) + this->get_name().size(); //name
    len += sizeof(TYPES::UINT32); //number of font-styles;
	for(FontStyle* font_style: font_styles)
		len+=font_style->calc_body_length(settings);	

    len += this->calc_attr_length(true, true, settings);

    return len;
}

result 
Font::collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
}




result
Font::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, AWDFile* awd_file)
{

	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	
	fileWriter->writeUINT32(this->font_styles.size());
	for(FontStyle* font_style: font_styles)
		font_style->write_body(fileWriter, curBlockSettings);	

    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}

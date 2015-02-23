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
}

Font::Font():
	AWDBlock(block_type::FONT),
	AttrElementBase()
{
}
Font::~Font()
{
	//free the url ?
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


std::vector<FontStyle*> Font::get_font_styles()
{
	return font_styles;
}
TYPES::UINT32
Font::calc_body_length(AWDFile* awd_file, SETTINGS::BlockSettings * settings)
{
    TYPES::UINT32 len;

    len = 0;//sizeof(TYPES::UINT32); //datalength;
	//len += sizeof(TYPES::UINT16) + this->get_name().size(); //name
    //len += sizeof(TYPES::UINT8); //save type (external/embbed)


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

	//awdutil_write_varstr(fd, this->get_name().c_str(), this->get_name().size());


    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}

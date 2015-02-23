#include "blocks/text_format.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::ATTR;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;
using namespace AWD::FONT;

TextFormat::TextFormat(const std::string& name) :
	BASE::AWDBlock(BLOCK::block_type::FORMAT, name),
    BASE::AttrElementBase()
{
	this->baseLineShift=baselineshift_type::BASELINE_NORMAL;
	this->fontSize=0;
	this->letterSpacing=0;
	this->fill=NULL;
}
TextFormat::TextFormat() :
	AWDBlock(BLOCK::block_type::FORMAT),
    AttrElementBase()
{
	this->baseLineShift=baselineshift_type::BASELINE_NORMAL;
	this->fontSize=0;
	this->letterSpacing=0;
	this->fill=NULL;
}

TextFormat::~TextFormat()
{
	//free the url ?
}

TYPES::state TextFormat::validate_block_state()
{
	return TYPES::state::VALID;
}

baselineshift_type TextFormat::get_baseLineShift()
{
	return this->baseLineShift;
}
void TextFormat::set_baseLineShift(baselineshift_type baseLineShift)
{
	this->baseLineShift=baseLineShift;
}
std::string& TextFormat::get_fontStyle()
{
	return this->fontStyle;
}
void TextFormat::set_fontStyle(const std::string& fontStyle)
{
	this->fontStyle=fontStyle;
}

std::string& TextFormat::get_fontName()
{
	return this->fontName;
}
void TextFormat::set_fontName(const std::string& fontName)
{
	this->fontName=fontName;
}

int TextFormat::get_fontSize()
{
	return this->fontSize;
}
void TextFormat::set_fontSize(int fontSize)
{
	this->fontSize=fontSize;
}
int TextFormat::get_letterSpacing()
{
	return this->letterSpacing;
}
void TextFormat::set_letterSpacing(int letterSpacing)
{
	this->letterSpacing=letterSpacing;
}

AWDBlock* TextFormat::get_fill()
{
	return this->fill;
}
void TextFormat::set_fill(AWDBlock* fill)
{
	this->fill=fill;
}


TYPES::UINT32
TextFormat::calc_body_length(AWDFile* awd_file, SETTINGS::BlockSettings * settings)
{
    TYPES::UINT32 len;

    len = 0;//sizeof(TYPES::UINT32); //datalength;
    len += this->calc_attr_length(true, true, settings);

    return len;
}

result 
TextFormat::collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
}


result
TextFormat::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, AWDFile* awd_file)
{
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}

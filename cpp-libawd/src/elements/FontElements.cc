#include "elements/fonts.h"
#include <sys/stat.h>

#include <sys/stat.h>

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::ATTR;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;
using namespace AWD::FONT;

TextRun::TextRun() :
    AttrElementBase()
{
}

TextRun::~TextRun()
{
	//free the url ?
}
AWDBlock* TextRun::get_format()
{
	return this->textFormat;
}
void TextRun::set_format(AWDBlock* textFormat)
{
	this->textFormat=textFormat;
}
std::string& TextRun::get_text()
{
	return this->text;
}
void TextRun::set_text(const std::string& text)
{
	this->text=text;
}

TYPES::UINT32
TextRun::calc_body_length(SETTINGS::BlockSettings * settings)
{
    TYPES::UINT32 len;

    len = 0;//sizeof(TYPES::UINT32); //datalength;
    len += this->calc_attr_length(true, true, settings);

    return len;
}

result
TextRun::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings)
{
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}

Paragraph::Paragraph() :
    AttrElementBase()
{
}

Paragraph::~Paragraph()
{
	//free the url ?
}
void Paragraph::add_textrun(TextRun* textRun)
{
	this->textRuns.push_back(textRun);
}


TYPES::UINT32
Paragraph::calc_body_length(SETTINGS::BlockSettings * settings)
{
    TYPES::UINT32 len;

    len = 0;//sizeof(TYPES::UINT32); //datalength;
	len+=sizeof(TYPES::UINT32);
	for(TextRun* tr : this->textRuns){
		len+=tr->calc_body_length(settings);
	}
    len += this->calc_attr_length(true, true, settings);

    return len;
}



result
Paragraph::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings)
{
	fileWriter->writeUINT32(TYPES::UINT32(this->textRuns.size()));
	for(TextRun* tr : this->textRuns){
		tr->write_body(fileWriter, curBlockSettings);
	}
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}


FontShape::FontShape() 
{
	shape_data=false;
}

FontShape::~FontShape()
{
	//free the url ?
}
    
	
int FontShape::get_charCode() 
{
	return this->charCode;
}
void FontShape::set_charCode(int charCode) 
{
	this->charCode=charCode;
}
bool FontShape::has_shape_data() 
{
	return this->shape_data;
}
GEOM::FilledRegion* FontShape::get_subShape() 
{
	return this->subShape;
}
void FontShape::set_subShape(GEOM::FilledRegion* subShape) 
{
	if(subShape!=NULL){
		this->subShape=subShape;
		this->shape_data=true;
	}
}
		
TYPES::UINT32 FontShape::calc_body_length(SETTINGS::BlockSettings*) 
{
	return 0;
}
result FontShape::write_body(FILES::FileWriter*, SETTINGS::BlockSettings*) 
{
	return result::AWD_SUCCESS;
}



FontStyle::FontStyle(const std::string& name) 
{
	this->style_size=0.0;
	this->style_name=name;
}

FontStyle::~FontStyle()
{
	//free the url ?
}
std::string& FontStyle::get_style_name() 
{
	return this->style_name;
}

void FontStyle::set_style_size(int style_size) 
{
	this->style_size=style_size;
}
std::vector<FontShape*> FontStyle::get_ungenerated_chars()
{
	std::vector<FontShape*> returner;
	typedef std::map<int, FontShape*>::iterator it_type;
	for(it_type iterator = shapesmap.begin(); iterator != shapesmap.end(); iterator++) {
		// iterator->first = key
		if(!iterator->second->has_shape_data()){	
			returner.push_back(iterator->second);
		}
	}
	return returner;
}
void FontStyle::delete_fontShape(int char_code){
	if(shapesmap.find(char_code) == shapesmap.end())
	{
		return;
	}
	FontShape* newFS=shapesmap[char_code];
	shapesmap.erase(char_code);
	delete newFS;
}
FontShape* FontStyle::get_fontShape(int char_code) 
{
	if(char_code==32){
		return NULL;
	}
	if(shapesmap.find(char_code) == shapesmap.end())
	{
		FontShape* newFS = new FontShape();
		newFS->set_charCode(char_code);
		shapesmap[char_code]=newFS;
		return newFS;
	}
	return shapesmap[char_code];
}
TYPES::UINT32 FontStyle::calc_body_length(SETTINGS::BlockSettings*) 
{
	return 0;
}
result FontStyle::write_body(FILES::FileWriter*, SETTINGS::BlockSettings*) 
{
	return result::AWD_SUCCESS;
}



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
    TYPES::UINT32 len = sizeof(TYPES::BADDR);// textformat id
	len += TYPES::UINT32(sizeof(TYPES::UINT32) + this->text.size()); //text
    return len;
}

result
TextRun::add_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	if(this->textFormat!=NULL)
		this->textFormat->add_with_dependencies(target_file, instance_type);
	return result::AWD_SUCCESS;
}
result
TextRun::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, FILES::AWDFile* awd_file)
{	
	TYPES::UINT32 block_addr=0;
	if (this->textFormat != NULL)
		this->textFormat->get_block_addr(awd_file, block_addr);
	fileWriter->writeUINT32(block_addr);
	fileWriter->writeSTRING(this->text, FILES::write_string_with::LENGTH_AS_UINT32);
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
    TYPES::UINT32 len = sizeof(TYPES::UINT32);
	for(TextRun* tr : this->textRuns)
		len+=tr->calc_body_length(settings);
    return len;
}


result
Paragraph::add_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	for(TextRun* tr : this->textRuns)
		tr->add_dependencies(target_file, instance_type);
	return result::AWD_SUCCESS;
}

result
Paragraph::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, FILES::AWDFile* awd_file)
{
	fileWriter->writeUINT32(TYPES::UINT32(this->textRuns.size()));
	for(TextRun* tr : this->textRuns)
		tr->write_body(fileWriter, curBlockSettings, awd_file);
	return result::AWD_SUCCESS;
}


FontShape::FontShape() 
{
	shape_data=false;
	subShape=NULL;
}

FontShape::~FontShape()
{
	if(shape_data)
		delete this->subShape;
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
GEOM::SubGeom* FontShape::get_subShape() 
{
	return this->subShape;
}
void FontShape::set_subShape(GEOM::SubGeom* subGeom) 
{
	this->subShape=subGeom;
	this->shape_data=true;

}
		
TYPES::UINT32 FontShape::calc_body_length(SETTINGS::BlockSettings* settings) 
{
	TYPES::UINT32 len =0;
	len += 4; // charcode
	len += 4; // subgeom_length
	if(subShape!=NULL){
		if(subShape->get_sub_geoms().size()>0)
			len += subShape->calc_subgeom_streams_length(subShape->get_sub_geoms()[0]);
	}
	return len;
}
result FontShape::write_body(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings* settings, double font_size) 
{
	fileWriter->writeUINT32(this->charCode);
	if(subShape!=NULL){
		if(subShape->get_sub_geoms().size()>0){
			
			TYPES::UINT32 sub_len = subShape->calc_subgeom_streams_length(subShape->get_sub_geoms()[0]);
			// Write sub-mesh header
			fileWriter->writeUINT32(sub_len);
			subShape->modify_font_char(font_size);
			subShape->write_subgeom_streams(fileWriter, subShape->get_sub_geoms()[0]);
		}
	}
	else{
		fileWriter->writeUINT32(0);
	}
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
typedef std::map<int, FontShape*>::iterator it_type;
std::vector<FontShape*> FontStyle::get_ungenerated_chars()
{
	std::vector<FontShape*> returner;
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
TYPES::UINT32 FontStyle::calc_body_length(SETTINGS::BlockSettings* settings) 
{
	
    TYPES::UINT32 len=0;
	
	len += sizeof(TYPES::UINT16) + this->get_style_name().size(); //name
    len += sizeof(TYPES::UINT32); //size;
    len += sizeof(TYPES::UINT32); //char_count;
	
	for(it_type iterator = shapesmap.begin(); iterator != shapesmap.end(); iterator++) {
		// iterator->first = key
		if(iterator->second->has_shape_data()){	
			len+=iterator->second->calc_body_length(settings);
		}
	}
	return len;
}
result FontStyle::write_body(FILES::FileWriter* fileWriter, SETTINGS::BlockSettings* settings) 
{
	fileWriter->writeSTRING(this->get_style_name(), FILES::write_string_with::LENGTH_AS_UINT16);	
	fileWriter->writeUINT32(this->style_size);
	fileWriter->writeUINT32(this->shapesmap.size());//charcount
	
	for(it_type iterator = shapesmap.begin(); iterator != shapesmap.end(); iterator++) {
		if(iterator->second->has_shape_data()){	
			iterator->second->write_body(fileWriter, settings, this->style_size);
		}
	}
	return result::AWD_SUCCESS;
}



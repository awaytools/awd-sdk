#include "blocks/text_element.h"
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


TextElement::TextElement(const std::string& name) :
	AWDBlock(BLOCK::block_type::TEXT_ELEMENT, name),
    
    AttrElementBase()
{
	this->isLocalized=false;
}
TextElement::TextElement() :
	AWDBlock(BLOCK::block_type::TEXT_ELEMENT),
    AttrElementBase()
{
	this->isLocalized=false;
}

TextElement::~TextElement()
{
}

TYPES::state TextElement::validate_block_state()
{
	return TYPES::state::VALID;
}

void
TextElement::set_isLocalized(bool isLocalized)
{
	this->isLocalized=isLocalized;
}
bool
TextElement::get_isLocalized()
{
	return this->isLocalized;
}
void
TextElement::set_text(const std::string& text)
{
	this->text=text;
}
std::string&
TextElement::get_text()
{
	return this->text;
}


void TextElement::add_paragraph(Paragraph* paragraph)
{
	this->textParagraphs.push_back(paragraph);
}



result 
TextElement::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
}


TYPES::UINT32
TextElement::calc_body_length(AWDFile* awd_file, SETTINGS::BlockSettings * settings)
{
    TYPES::UINT32 len;

    len = 0;//sizeof(TYPES::UINT32); //datalength;
	//len += sizeof(TYPES::UINT16) + this->get_name().size(); //name
   // len += sizeof(TYPES::UINT8); //save type (external/embbed)
	len += 1; //isLocalized
	len += TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	if(!this->isLocalized){
		len+=sizeof(TYPES::UINT32);
		for(Paragraph* p : this->textParagraphs){
			len+=p->calc_body_length(settings);
		}
	}
 
    len += this->calc_attr_length(true, true, settings);

    return len;
}


result
TextElement::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, FILES::AWDFile* awd_file)
{
	
	fileWriter->writeBOOL(this->isLocalized);
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);// name	
	if(!this->isLocalized){
		fileWriter->writeUINT32(TYPES::UINT32(this->textParagraphs.size()));
		for(Paragraph* p : this->textParagraphs){
			p->write_body(fileWriter, curBlockSettings);
		}
	}
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}
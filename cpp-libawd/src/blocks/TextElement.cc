#include "blocks/text_element.h"
#include "utils/awd_properties.h"
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
	init();	
}
TextElement::TextElement() :
	AWDBlock(BLOCK::block_type::TEXT_ELEMENT),
    AttrElementBase()
{
	init();
}



void TextElement::init()
{
	this->tf_type=FONT::Textfield_type::STATIC;
	this->isLocalized=false;	
	this->is_selectable=false;	
	this->render_as_html=false;	
	this->is_border_drawn=false;	
	this->is_scrollable=false;	
	this->textflow=TextFlow::TEXT_FLOW_LEFT_TO_RIGHT;	
	this->orient_mode=OrientationMode::ORIENTATION_MODE_HORIZONTAL;	
	this->line_mode=LineMode::LINE_MODE_SINGLE;	
	
	this->text_width=0;
	this->text_height=0;

	TYPES::union_ptr default_union;
	default_union.v=malloc(1);
	*default_union.b=false;
	this->properties->add(PROP_TEXTFIELD_IS_SELECTABLE,	default_union, 1,   data_types::BOOL, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		
	default_union.v=malloc(1);
	*default_union.b=false;
	this->properties->add(PROP_TEXTFIELD_BORDER,	default_union, 1,   data_types::BOOL, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		
	default_union.v=malloc(1);
	*default_union.b=false;
	this->properties->add(PROP_TEXTFIELD_RENDER_HTML,	default_union, 1,   data_types::BOOL, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		
	default_union.v=malloc(1);
	*default_union.b=false;
	this->properties->add(PROP_TEXTFIELD_IS_SCROLLABLE,	default_union, 1,   data_types::BOOL, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		
	default_union.v=malloc(1);
	*default_union.ui8=(TYPES::UINT8)FONT::TextFlow::TEXT_FLOW_LEFT_TO_RIGHT;
	this->properties->add(PROP_TEXTFIELD_TEXTFLOW,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
			
	default_union.v=malloc(1);
	*default_union.ui8=(TYPES::UINT8)FONT::OrientationMode::ORIENTATION_MODE_HORIZONTAL;
	this->properties->add(PROP_TEXTFIELD_ORIENTATIONMODE,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
			
	default_union.v=malloc(1);
	*default_union.ui8=(TYPES::UINT8)FONT::LineMode::LINE_MODE_SINGLE;
	this->properties->add(PROP_TEXTFIELD_LINEMODE,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		
}	


TextElement::~TextElement()
{
}

void 
TextElement::set_bounds(GEOM::BOUNDS2D bounds)
{
	this->bounds=bounds;
}
GEOM::BOUNDS2D 
TextElement::get_bounds()
{
	return this->bounds;
}

void 
TextElement::set_tf_type(FONT::Textfield_type tf_type)
{
	this->tf_type=tf_type;
}
FONT::Textfield_type 
TextElement::get_tf_type()
{
	return this->tf_type;
}

void 
TextElement::set_is_selectable(bool is_selectable)
{
	this->is_selectable=is_selectable;
}
bool 
TextElement::get_is_selectable()
{
	return this->is_selectable;
}

void 
TextElement::set_render_as_html(bool render_as_html)
{
	this->render_as_html=render_as_html;
}
bool 
TextElement::get_render_as_html()
{
	return this->render_as_html;
}

void 
TextElement::set_is_border_drawn(bool is_border_drawn)
{
	this->is_border_drawn=is_border_drawn;
}
bool 
TextElement::get_is_border_drawn()
{
	return this->is_border_drawn;
}

void 
TextElement::set_is_scrollable(bool is_scrollable)
{
	this->is_scrollable=is_scrollable;
}
bool 
TextElement::get_is_scrollable()
{
	return this->is_scrollable;
}



void 
TextElement::set_textflow(FONT::TextFlow textflow)
{
	this->textflow=textflow;
}
FONT::TextFlow 
TextElement::get_textflow()
{
	return this->textflow;
}

void 
TextElement::set_orientationMode(FONT::OrientationMode orient_mode)
{
	this->orient_mode=orient_mode;
}

FONT::OrientationMode 
TextElement::get_orientationMode()
{
	return this->orient_mode;
}

void 
TextElement::set_line_mode(FONT::LineMode line_mode)
{
	this->line_mode=line_mode;
}

FONT::LineMode 
TextElement::get_line_mode()
{
	return this->line_mode;
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
	for(FONT::Paragraph* pgraph : this->textParagraphs)
		pgraph->add_dependencies(target_file, instance_type);
	return result::AWD_SUCCESS;
}


TYPES::UINT32
TextElement::calc_body_length(AWDFile* awd_file, SETTINGS::BlockSettings * settings)
{

	if(this->is_selectable){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.b=true;
		this->properties->set(PROP_TEXTFIELD_IS_SELECTABLE, properties_union, 1);
	}
	if(this->is_border_drawn){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.b=true;
		this->properties->set(PROP_TEXTFIELD_BORDER, properties_union, 1);
	}
	if(this->render_as_html){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.b=true;
		this->properties->set(PROP_TEXTFIELD_RENDER_HTML, properties_union, 1);
	}
	if(this->is_scrollable){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.b=true;
		this->properties->set(PROP_TEXTFIELD_IS_SCROLLABLE, properties_union, 1);
	}
	if(this->textflow!=FONT::TextFlow::TEXT_FLOW_LEFT_TO_RIGHT){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.ui8=(TYPES::UINT8)this->textflow;
		this->properties->set(PROP_TEXTFIELD_TEXTFLOW, properties_union, 1);
	}
	if(this->orient_mode!=FONT::OrientationMode::ORIENTATION_MODE_HORIZONTAL){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.ui8=(TYPES::UINT8)this->orient_mode;
		this->properties->set(PROP_TEXTFIELD_ORIENTATIONMODE, properties_union, 1);
	}
	if(this->line_mode!=FONT::LineMode::LINE_MODE_SINGLE){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(1);
		*properties_union.ui8=(TYPES::UINT8)this->line_mode;
		this->properties->set(PROP_TEXTFIELD_LINEMODE, properties_union, 1);
	}

    TYPES::UINT32 len;

    len = 0;//sizeof(TYPES::UINT32); //datalength;
	len += TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size()); //name
	len+= sizeof(TYPES::UINT8);//tf_type
	len += 2 * sizeof(TYPES::F32);
	len += sizeof(TYPES::UINT32);
	for(Paragraph* p : this->textParagraphs){
		len+=p->calc_body_length(settings);
	} 
    len += this->calc_attr_length(true, true, settings);
    return len;
}


result
TextElement::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, FILES::AWDFile* awd_file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);// name	
	fileWriter->writeUINT8(TYPES::UINT8(this->tf_type));
	fileWriter->writeFLOAT32(this->text_width);
	fileWriter->writeFLOAT32(this->text_height);
	fileWriter->writeUINT32(this->textParagraphs.size());
	for(Paragraph* p : this->textParagraphs){
		p->write_body(fileWriter, curBlockSettings, awd_file);	
	} 

    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}
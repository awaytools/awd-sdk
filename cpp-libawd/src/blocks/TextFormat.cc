#include "blocks/text_format.h"
#include "blocks/material.h"
#include "utils/awd_properties.h"

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
	this->init();
}
TextFormat::TextFormat() :
	AWDBlock(BLOCK::block_type::FORMAT),
    AttrElementBase()
{
	this->init();
}


void TextFormat::init()
{
	this->baseLineShift=baselineshift_type::BASELINE_NORMAL;
	this->fontSize=0;
	this->letterSpacing=0;
	this->fill_material=NULL;
	this->font=NULL;
	this->is_rotated=false;
	this->autokerning=true;
	TYPES::union_ptr default_union;
	default_union.v=malloc(2);
	*default_union.ui16=12;
	this->properties->add(PROP_TEXTFORMAT_FONTSIZE,	default_union, 2,   data_types::UINT16, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
			
	default_union.v=malloc(2);
	*default_union.ui16=0;
	this->properties->add(PROP_TEXTFORMAT_LETTER_SPACING,	default_union, 2,   data_types::UINT16, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(1);
	*default_union.ui8=0;
	this->properties->add(PROP_TEXTFORMAT_IS_ROTATED,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(1);
	*default_union.ui8=1;
	this->properties->add(PROP_TEXTFORMAT_AUTO_KERNING,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(1);
	*default_union.ui8=1;
	this->properties->add(PROP_TEXTFORMAT_BASELINESHIFT,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(2);
	*default_union.ui8=0;
	this->properties->add(PROP_TEXTFORMAT_ALIGNMENT,	default_union, 1,   data_types::UINT8, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(2);
	*default_union.ui16=0;
	this->properties->add(PROP_TEXTFORMAT_INDENT,	default_union, 2,   data_types::UINT16, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(2);
	*default_union.ui16=0;
	this->properties->add(PROP_TEXTFORMAT_LEFT_MARGIN,	default_union, 2,   data_types::UINT16, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(2);
	*default_union.ui16=0;
	this->properties->add(PROP_TEXTFORMAT_RIGHT_MARGIN,	default_union, 2,   data_types::UINT16, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(2);
	*default_union.ui16=0;
	this->properties->add(PROP_TEXTFORMAT_LINE_SPACING,	default_union, 2,   data_types::UINT16, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
}	
TextFormat::~TextFormat()
{
	//free the url ?
}


TYPES::state TextFormat::validate_block_state()
{
	return TYPES::state::VALID;
}
			
void 
TextFormat::set_alignment(FONT::AlignMode alignment)
{
	this->alignment=alignment;
}	
			
void 
TextFormat::set_indent(double indent)
{
	this->indent=indent;
}
			
void 
TextFormat::set_linespacing(double line_spacing)
{
	this->line_spacing=line_spacing;
}
			
void 
TextFormat::set_leftmargin(double left_margin)
{
	this->left_margin=left_margin;
}
			
void 
TextFormat::set_rightmargin(double right_margin)
{
	this->right_margin=right_margin;
}

baselineshift_type 
TextFormat::get_baseLineShift()
{
	return this->baseLineShift;
}

void 
TextFormat::set_baseLineShift(baselineshift_type baseLineShift)
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

BASE::AWDBlock* TextFormat::get_font()
{
	return this->font;
}
void TextFormat::set_font(BASE::AWDBlock* font)
{
	this->font=font;
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

AWDBlock* TextFormat::get_fill_material()
{
	return this->fill_material;
}
void TextFormat::set_fill_material(AWDBlock* fill_material)
{
	this->fill_material=fill_material;
}


TYPES::UINT32
TextFormat::calc_body_length(AWDFile* awd_file, SETTINGS::BlockSettings * settings)
{
			
	TYPES::union_ptr properties_union_size;
	properties_union_size.v=malloc(2);
	*properties_union_size.ui16=this->fontSize;
	this->properties->set(PROP_TEXTFORMAT_FONTSIZE, properties_union_size, 2);

	TYPES::union_ptr properties_union_letter_space;
	properties_union_letter_space.v=malloc(2);
	*properties_union_letter_space.ui16=this->letterSpacing;
	this->properties->set(PROP_TEXTFORMAT_LETTER_SPACING, properties_union_letter_space, 2);
	
	TYPES::union_ptr properties_union_is_rotated;
	properties_union_is_rotated.v=malloc(1);
	*properties_union_is_rotated.ui8=this->is_rotated;
	this->properties->set(PROP_TEXTFORMAT_IS_ROTATED, properties_union_is_rotated, 1);

	TYPES::union_ptr properties_union_autokerning;
	properties_union_autokerning.v=malloc(1);
	*properties_union_autokerning.ui8=this->autokerning;
	this->properties->set(PROP_TEXTFORMAT_AUTO_KERNING, properties_union_autokerning, 1);
	
	TYPES::union_ptr properties_union_baselineshift;
	properties_union_baselineshift.v=malloc(1);
	*properties_union_baselineshift.ui8=(TYPES::UINT8)this->baseLineShift;
	this->properties->set(PROP_TEXTFORMAT_BASELINESHIFT, properties_union_baselineshift, 1);
	
	TYPES::union_ptr properties_union_align;
	properties_union_align.v=malloc(1);
	*properties_union_align.ui8=(TYPES::UINT8)this->alignment;
	this->properties->set(PROP_TEXTFORMAT_ALIGNMENT, properties_union_align, 1);
	
	TYPES::union_ptr properties_union_indent;
	properties_union_indent.v=malloc(2);
	*properties_union_indent.ui16=(TYPES::UINT16)this->indent;
	this->properties->set(PROP_TEXTFORMAT_INDENT, properties_union_indent, 2);

	TYPES::union_ptr properties_left_margin;
	properties_left_margin.v=malloc(2);
	*properties_left_margin.ui16=(TYPES::UINT16)this->left_margin;
	this->properties->set(PROP_TEXTFORMAT_LEFT_MARGIN, properties_left_margin, 2);

	TYPES::union_ptr properties_right_margin;
	properties_right_margin.v=malloc(2);
	*properties_right_margin.ui16=(TYPES::UINT16)this->right_margin;
	this->properties->set(PROP_TEXTFORMAT_RIGHT_MARGIN, properties_right_margin, 2);
	
	TYPES::union_ptr properties_line_spacing;
	properties_line_spacing.v=malloc(2);
	*properties_line_spacing.ui16=(TYPES::UINT16)this->line_spacing;
	this->properties->set(PROP_TEXTFORMAT_LINE_SPACING, properties_line_spacing, 2);


	TYPES::UINT32 length = sizeof(TYPES::UINT16) + this->get_name().size();//name
	length += sizeof(BADDR);//geom address
	length += sizeof(TYPES::UINT16) + this->get_fontStyle().size();//fontstyle_name
	length += sizeof(BADDR);//mat address
	length += sizeof(TYPES::UINT8)+(this->uv_values.size()*(4));//length uv
    length += this->calc_attr_length(true, true, settings);

    return length;
}

result 
TextFormat::merge_for_textureatlas(BASE::AWDBlock* this_material)
{
	if(this->fill_material==NULL){
		return result::AWD_ERROR;
	}
	
	BLOCKS::Material* mat = reinterpret_cast<BLOCKS::Material*> (this->fill_material);
	BLOCKS::Material* mat2 = reinterpret_cast<BLOCKS::Material*> (this_material);
	this->uv_values.clear();

	// if texture_u is set (is positiove) texture_v must also been set. same for max values
	if(mat->texture_u>=0)
		this->uv_values.push_back(mat->texture_u);
	if(mat->texture_v>=0)
		this->uv_values.push_back(mat->texture_v);

	if(mat->texture_u_max>=0)
		this->uv_values.push_back(mat->texture_u_max);
	if(mat->texture_v_max>=0)
		this->uv_values.push_back(mat->texture_v_max);
	
	if(mat2->get_texture()==mat->get_texture()){
		this->fill_material=this_material;
	}
	return result::AWD_SUCCESS;
}

bool 
TextFormat::get_is_rotated()
{
	return this->is_rotated;
}
void 
TextFormat::set_is_rotated(bool is_rotated)
{
	this->is_rotated=is_rotated;
}
bool 
TextFormat::get_autokerning()
{
	return this->autokerning;
}
void 
TextFormat::set_autokerning(bool autokerning)
{
	this->autokerning=autokerning;
}
result 
TextFormat::collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type)
{
	if(this->font!=NULL)
		this->font->add_with_dependencies(target_file, instance_type);
	if(this->fill_material!=NULL)
		this->fill_material->add_with_dependencies(target_file, instance_type);
	return result::AWD_SUCCESS;
}


result
TextFormat::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * curBlockSettings, AWDFile* awd_file)
{
	if(fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16)!=result::AWD_SUCCESS)
		return result::AWD_ERROR;

	TYPES::UINT32 block_addr=0;
	if (this->font != NULL)
		this->font->get_block_addr(awd_file, block_addr);
	fileWriter->writeUINT32(block_addr);

	if(fileWriter->writeSTRING(this->get_fontStyle(), FILES::write_string_with::LENGTH_AS_UINT16)!=result::AWD_SUCCESS)
		return result::AWD_ERROR;

	block_addr=0;
	if (this->fill_material != NULL)
		this->fill_material->get_block_addr(awd_file, block_addr);
	fileWriter->writeUINT32(block_addr);

	fileWriter->writeUINT8(this->uv_values.size());
	for(double uv_value : this->uv_values)
		fileWriter->writeFLOAT32(uv_value);
		
    this->properties->write_attributes(fileWriter,  curBlockSettings);
    this->user_attributes->write_attributes(fileWriter,  curBlockSettings);
	return result::AWD_SUCCESS;
}

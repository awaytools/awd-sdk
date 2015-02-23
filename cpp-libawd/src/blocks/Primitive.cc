#include "blocks/primitive.h"

#include "blocks/scene.h"
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "base/state_element_base.h"
#include "utils/settings.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

Primitive::Primitive(std::string& name, GEOM::primitive_type type) :
	AWDBlock(block_type::PRIM_GEOM, name),
	
	AttrElementBase()
{
	this->prim_type = type;
	this->yoffset = 0;
}
Primitive::Primitive():
	AWDBlock(block_type::PRIM_GEOM),
	AttrElementBase()
{
}

Primitive::~Primitive()
{
	this->prim_type=GEOM::primitive_type::UNDEFINED;
	this->yoffset = NULL;
}

TYPES::state Primitive::validate_block_state()
{
	return TYPES::state::VALID;
}



void set_Yoffset(double);
double
Primitive::get_Yoffset()
{
	return this->yoffset;
}
void
Primitive::set_Yoffset(double yoffset)
{
	this->yoffset = yoffset;
}
void
Primitive::add_number_property(int targetID, float targetValue, float defaultValue)
{
	if(targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::F64));
		*newVal.F64 = targetValue;
		//this->properties->set(targetID, newVal, sizeof(TYPES::F64), TYPES::data_types::FLOAT64, 2);
	}
}
void
Primitive::add_int_property(int targetID, int targetValue, int defaultValue)
{
	if(targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(TYPES::UINT16));
		*newVal.ui16 = targetValue;
	//	this->properties->set(targetID, newVal, sizeof(TYPES::UINT16),  TYPES::data_types::UINT16);
	}
}
void
Primitive::add_bool_property(int targetID, bool targetValue, bool defaultValue)
{
	if(targetValue!=defaultValue){
		union_ptr newVal;
		newVal.v = malloc(sizeof(bool));
		*newVal.b = targetValue;
	//	this->properties->set(targetID, newVal, sizeof(bool),  TYPES::data_types::BOOL);
	}
}
TYPES::UINT32
Primitive::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return TYPES::UINT32(0);*/
	return TYPES::UINT32(1 + sizeof(TYPES::UINT16) + this->get_name().size()+this->calc_attr_length(true, true, settings));
}
result 
Primitive::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	return result::AWD_SUCCESS;
}
result
Primitive::write_body(FileWriter * fileWriter, BlockSettings * settings,AWDFile* file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	fileWriter->writeUINT8( TYPES::UINT8(this->prim_type));
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	return result::AWD_SUCCESS;
}

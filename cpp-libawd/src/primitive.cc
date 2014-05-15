#include "platform.h"
#include "primitive.h"
#include "util.h"

AWDPrimitive::AWDPrimitive(const char *name, awd_uint16 name_len, AWD_primitive_type type) :
    AWDBlock(PRIM_GEOM),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->prim_type = type;
    this->yoffset = 0;
}

AWDPrimitive::~AWDPrimitive()
{
    this->prim_type=AWD_PRIMITIVE_UNDEFINED;
    this->yoffset = NULL;
}
void set_Yoffset(double);
double
AWDPrimitive::get_Yoffset()
{
    return this->yoffset;
}
void
AWDPrimitive::set_Yoffset(double yoffset)
{
    this->yoffset = yoffset;
}
void
AWDPrimitive::add_number_property(int targetID, float targetValue, float defaultValue)
{
    if(targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_float64));
        *newVal.f64 = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64, 2);
    }
}
void
AWDPrimitive::add_int_property(int targetID, int targetValue, int defaultValue)
{
    if(targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint16));
        *newVal.ui16 = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_uint16), AWD_FIELD_UINT16);
    }
}
void
AWDPrimitive::add_bool_property(int targetID, bool targetValue, bool defaultValue)
{
    if(targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_bool));
        *newVal.b = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
}
void
AWDPrimitive::add_dependencies(AWDBlockList *)
{
}

awd_uint32
AWDPrimitive::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    return 1 + sizeof(awd_uint16) + this->get_name_length()+this->calc_attr_length(true, true, curBlockSettings);
}

void
AWDPrimitive::write_body(int fd, BlockSettings * curBlockSettings)
{
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &this->prim_type, sizeof(awd_uint8));
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
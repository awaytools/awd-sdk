#include "shading.h"
#include "util.h"

#include "platform.h"

AWDShadingMethod::AWDShadingMethod(AWD_shade_type type) :
    AWDAttrElement()
{
    this->shading_props=new AWDNumAttrList();
    this->type = type;
    this->awdBlock1 = NULL;
    this->awdBlock2 = NULL;
    this->awdBlock3 = NULL;
    this->awdBlock4 = NULL;
}

AWDShadingMethod::~AWDShadingMethod()
{
    delete this->shading_props;
    this->awdBlock1 = NULL;
    this->awdBlock2 = NULL;
    this->awdBlock3 = NULL;
    this->awdBlock4 = NULL;
}
void
AWDShadingMethod::set_awdBlock1(AWDBlock * newawdBlock)
{
    this->awdBlock1 = newawdBlock;
}
void
AWDShadingMethod::set_awdBlock2(AWDBlock * newawdBlock)
{
    this->awdBlock2 = newawdBlock;
}
void
AWDShadingMethod::set_awdBlock3(AWDBlock * newawdBlock)
{
    this->awdBlock3 = newawdBlock;
}
void
AWDShadingMethod::set_awdBlock4(AWDBlock * newawdBlock)
{
    this->awdBlock4 = newawdBlock;
}
AWDNumAttrList *
AWDShadingMethod::get_shading_props()
{
    return this->shading_props;
}
AWD_shade_type
AWDShadingMethod::get_shading_type()
{
    return this->type;
}

void
AWDShadingMethod::add_color_property(int targetID, awd_uint32 targetValue, awd_uint32 defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = (awd_uint32 *)malloc(sizeof(awd_uint32));
        *newVal.col = targetValue;
        this->shading_props->set(targetID, newVal, sizeof(awd_uint32), AWD_FIELD_COLOR);
    }
}
void
AWDShadingMethod::add_number_property(int targetID, float targetValue, float defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = (awd_float64 *)malloc(sizeof(awd_float64));
        *newVal.f64 = targetValue;
        this->shading_props->set(targetID, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
}
void
AWDShadingMethod::add_int_property(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = (awd_uint16 *)malloc(sizeof(awd_uint16));
        *newVal.ui16 = targetValue;
        this->shading_props->set(targetID, newVal, sizeof(awd_uint16), AWD_FIELD_UINT16);
    }
}
void
AWDShadingMethod::add_bool_property(int targetID, bool targetValue, bool defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = (awd_bool *)malloc(sizeof(awd_bool));
        *newVal.b = targetValue;
        this->shading_props->set(targetID, newVal, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
}
void
AWDShadingMethod::add_int8_property(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = (awd_int8 *)malloc(sizeof(awd_int8));
        *newVal.i8 = targetValue;
        this->shading_props->set(targetID, newVal, sizeof(awd_int8), AWD_FIELD_INT8);
    }
}

void
AWDShadingMethod::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->awdBlock1 != NULL) {
        this->awdBlock1->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = (awd_baddr *)malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock1->get_addr();
        this->shading_props->set(PROPS_BADDR1, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
    if (this->awdBlock2 != NULL) {
        this->awdBlock2->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = (awd_baddr *)malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock2->get_addr();
        this->shading_props->set(PROPS_BADDR2, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
    if (this->awdBlock3 != NULL) {
        this->awdBlock3->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = (awd_baddr *)malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock3->get_addr();
        this->shading_props->set(PROPS_BADDR3, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
    if (this->awdBlock4 != NULL) {
        this->awdBlock4->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = (awd_baddr *)malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock4->get_addr();
        this->shading_props->set(PROPS_BADDR4, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
}

awd_uint32
AWDShadingMethod::calc_method_length(BlockSettings * blockSettings)
{
    int len;
    len = sizeof(awd_uint16); // shading type
    len += this->shading_props->calc_length(blockSettings); // shading props
    len += this->calc_attr_length(false,true, blockSettings); // shading attributes
    return len;
}

void
AWDShadingMethod::write_method(int fd, BlockSettings * blockSettings)
{
    awd_uint16 type_be;
    type_be = UI16(this->type);
    write(fd, &type_be, sizeof(awd_uint16));
    this->shading_props->write_attributes(fd, blockSettings);
    this->user_attributes->write_attributes(fd, blockSettings);
}
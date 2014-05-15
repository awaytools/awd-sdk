#include "platform.h"
#include "method.h"
#include "util.h"

AWDEffectMethod::AWDEffectMethod(const char *name, awd_uint16 name_len, AWD_effect_method_type type) :
    AWDBlock(EFFECT_METHOD),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->effect_props=new AWDNumAttrList();
    this->type = type;
    this->awdBlock1 = NULL;
    this->awdBlock2 = NULL;
    this->awdBlock3 = NULL;
    this->awdBlock4 = NULL;
}

AWDEffectMethod::~AWDEffectMethod()
{
    delete this->effect_props;
    this->awdBlock1 = NULL;
    this->awdBlock2 = NULL;
    this->awdBlock3 = NULL;
    this->awdBlock4 = NULL;
}
void
AWDEffectMethod::set_awdBlock1(AWDBlock * newawdBlock)
{
    this->awdBlock1 = newawdBlock;
}
void
AWDEffectMethod::set_awdBlock2(AWDBlock * newawdBlock)
{
    this->awdBlock2 = newawdBlock;
}
void
AWDEffectMethod::set_awdBlock3(AWDBlock * newawdBlock)
{
    this->awdBlock3 = newawdBlock;
}
void
AWDEffectMethod::set_awdBlock4(AWDBlock * newawdBlock)
{
    this->awdBlock4 = newawdBlock;
}
AWDNumAttrList *
AWDEffectMethod::get_effect_props()
{
    return this->effect_props;
}

void
AWDEffectMethod::add_color_method_prop(int targetID, awd_uint32 targetValue, awd_uint32 defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint32));
        *newVal.col = targetValue;
        this->effect_props->set(targetID, newVal, sizeof(awd_uint32), AWD_FIELD_COLOR);
    }
}
void
AWDEffectMethod::add_number_method_prop(int targetID, float targetValue, float defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_float64));
        *newVal.f64 = targetValue;
        this->effect_props->set(targetID, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
}
void
AWDEffectMethod::add_int_method_prop(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint16));
        *newVal.ui16 = targetValue;
        this->effect_props->set(targetID, newVal, sizeof(awd_uint16), AWD_FIELD_UINT16);
    }
}
void
AWDEffectMethod::add_bool_method_prop(int targetID, bool targetValue, bool defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_bool));
        *newVal.b = targetValue;
        this->effect_props->set(targetID, newVal, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
}
void
AWDEffectMethod::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->awdBlock1 != NULL) {
        this->awdBlock1->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock1->get_addr();
        this->effect_props->set(PROPS_BADDR1, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
    if (this->awdBlock2 != NULL) {
        this->awdBlock2->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock2->get_addr();
        this->effect_props->set(PROPS_BADDR2, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
    if (this->awdBlock3 != NULL) {
        this->awdBlock3->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock3->get_addr();
        this->effect_props->set(PROPS_BADDR3, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
    if (this->awdBlock4 != NULL) {
        this->awdBlock4->prepare_and_add_with_dependencies(export_list);
        AWD_field_ptr tex_val;
        tex_val.v = malloc(sizeof(awd_baddr));
        *tex_val.addr = this->awdBlock4->get_addr();
        this->effect_props->set(PROPS_BADDR4, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
    }
}

awd_uint32
AWDEffectMethod::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;
    len = sizeof(awd_uint16) + this->get_name_length(); //name
    len += sizeof(awd_uint16); //type
    len += this->effect_props->calc_length(curBlockSettings);
    len += this->calc_attr_length(true, true, curBlockSettings);
    return len;
}

void
AWDEffectMethod::write_body(int fd, BlockSettings *curBlockSettings)
{
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &this->type, sizeof(awd_uint16));
    this->effect_props->write_attributes(fd, curBlockSettings);
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
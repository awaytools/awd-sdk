#include "platform.h"
#include "shadows.h"
#include "util.h"

AWDShadowMethod::AWDShadowMethod(const char *name, awd_uint16 name_len, AWD_shadow_method_type shadow_type) :
    AWDBlock(SHADOW_METHOD),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->shadow_props=new AWDNumAttrList();
    this->base_method=NULL;
    this->shadow_type = shadow_type;
    this->awdLight = NULL;
    this->autoApply=false;
}

AWDShadowMethod::~AWDShadowMethod()
{
    delete this->shadow_props;
}

AWDNumAttrList *
AWDShadowMethod::get_shadow_props()
{
    return this->shadow_props;
}

AWD_shadow_method_type
AWDShadowMethod::get_shadow_type()
{
    return this->shadow_type;
}
void
AWDShadowMethod::set_shadow_type(AWD_shadow_method_type shadow_type)
{
    this->shadow_type=shadow_type;
}
AWDBlock *
AWDShadowMethod::get_base_method()
{
    return this->base_method;
}
void
AWDShadowMethod::set_base_method(AWDBlock * base_method)
{
    this->base_method=base_method;
}
void
AWDShadowMethod::set_awdLight(AWDBlock * awdLight)
{
    this->awdLight=awdLight;
}
AWDBlock *
AWDShadowMethod::get_awdLight()
{
    return this->awdLight;
}
bool
AWDShadowMethod::get_autoApply()
{
    return this->autoApply;
}
void
AWDShadowMethod::set_autoApply(bool autoApply)
{
    this->autoApply=autoApply;
}

void
AWDShadowMethod::add_color_shadow_prop(int targetID, awd_uint32 targetValue, awd_uint32 defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint32));
        *newVal.col = targetValue;
        this->shadow_props->set(targetID, newVal, sizeof(awd_uint32), AWD_FIELD_COLOR);
    }
}
void
AWDShadowMethod::add_number_shadow_prop(int targetID, float targetValue, float defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_float64));
        *newVal.f64 = targetValue;
        this->shadow_props->set(targetID, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
}
void
AWDShadowMethod::add_int16_shadow_prop(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint16));
        *newVal.ui16 = targetValue;
        this->shadow_props->set(targetID, newVal, sizeof(awd_uint16), AWD_FIELD_UINT16);
    }
}
void
AWDShadowMethod::add_int_shadow_prop(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint32));
        *newVal.ui32 = targetValue;
        this->shadow_props->set(targetID, newVal, sizeof(awd_uint32), AWD_FIELD_UINT32);
    }
}
void
AWDShadowMethod::add_bool_shadow_prop(int targetID, bool targetValue, bool defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_bool));
        *newVal.b = targetValue;
        this->shadow_props->set(targetID, newVal, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
}
void
AWDShadowMethod::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->base_method != NULL) {
        this->base_method->prepare_and_add_with_dependencies(export_list);
        this->add_int_shadow_prop(1, this->base_method->get_addr(), 0);
    }
    if (this->awdLight != NULL) {
        this->awdLight->prepare_and_add_with_dependencies(export_list);
    }
}

awd_uint32
AWDShadowMethod::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;
    len = sizeof(awd_uint16) + this->get_name_length(); //name
    len += sizeof(awd_uint32); //light_id
    len += sizeof(awd_uint16); //type
    len += this->shadow_props->calc_length(curBlockSettings);
    len += this->calc_attr_length(true, true, curBlockSettings);
    return len;
}

void
AWDShadowMethod::write_body(int fd, BlockSettings *curBlockSettings)
{
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    int light_addr = 0;
    if (this->awdLight != NULL)
        light_addr = UI32(this->awdLight->get_addr());
    write(fd, &light_addr, sizeof(awd_uint32));

    write(fd, &this->shadow_type, sizeof(awd_uint16));
    this->shadow_props->write_attributes(fd, curBlockSettings);
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
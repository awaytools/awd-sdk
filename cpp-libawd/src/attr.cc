#include <cstdio>
#include <stdlib.h>
#include <cstring>

#include "attr.h"
#include "awd_types.h"
#include "util.h"

#include "platform.h"

void
AWDAttr::write_attr(int fd, BlockSettings *curBlockSettings)
{
    bool thisStoragePrec=false;
    if (this->type==AWD_FIELD_FLOAT64){
        bool thisStoragePrec=curBlockSettings->get_wide_props();
        if (this->storage_type==1)
            thisStoragePrec=curBlockSettings->get_wide_matrix();
        else if (this->storage_type==2)
            thisStoragePrec=curBlockSettings->get_wide_geom();
        else if (this->storage_type==3)
            thisStoragePrec=curBlockSettings->get_wide_attributes();
        if (!thisStoragePrec){
            this->type=AWD_FIELD_FLOAT32;
            this->value_len/=2;
           // *this->value.f32=(awd_float32 *)this->value.f64;
        }
    }
    AWD_field_ptr val;
    awd_uint32 bytes_written;
    awd_int16 i16_be;
    awd_uint8 i8_be;
    awd_int32 i32_be;
    awd_float32 f32_be;
    awd_float64 f64_be;

    this->write_metadata(fd);

    bytes_written = 0;
    val.v = this->value.v;

    while (bytes_written < this->value_len) {
        // Check type, and write data accordingly
        switch (this->type) {
            case AWD_FIELD_INT8:
            case AWD_FIELD_UINT8:
                i8_be = (*val.ui8);
                write(fd, &i8_be, sizeof(awd_uint8));
                bytes_written += sizeof(awd_uint8);
                val.ui8++;
                break;
            case AWD_FIELD_INT16:
            case AWD_FIELD_UINT16:
                i16_be = UI16(*val.i16);
                write(fd, &i16_be, sizeof(awd_int16));
                bytes_written += sizeof(awd_int16);
                val.i16++;
                break;

            case AWD_FIELD_INT32:
            case AWD_FIELD_UINT32:
            case AWD_FIELD_BADDR:
            case AWD_FIELD_COLOR:
                i32_be = UI32(*val.i32);
                write(fd, &i32_be, sizeof(awd_int32));
                bytes_written += sizeof(awd_int32);
                val.i32++;
                break;

            case AWD_FIELD_FLOAT32:
                f32_be = awd_float32(F32(*val.f64));
                write(fd, &f32_be, sizeof(awd_float32));
                bytes_written += sizeof(awd_float32);
                val.f64++;
                break;

            case AWD_FIELD_FLOAT64:
                f64_be = F64(*val.f64);
                write(fd, &f64_be, sizeof(awd_float64));
                bytes_written += sizeof(awd_float64);
                val.f64++;
                break;

            case AWD_FIELD_STRING:
                // Write entire string in one go
                write(fd, val.str, this->value_len);
                bytes_written += this->value_len;
                break;

            case AWD_FIELD_BOOL:
                write(fd, val.b, this->value_len);
                bytes_written += this->value_len;
                break;

            case AWD_FIELD_VECTOR2x1:
            case AWD_FIELD_VECTOR3x1:
            case AWD_FIELD_VECTOR4x1:
            case AWD_FIELD_MTX3x2:
            case AWD_FIELD_MTX3x3:
            case AWD_FIELD_MTX4x3:
            case AWD_FIELD_MTX4x4:
                bytes_written += awdutil_write_floats(fd, val.f64, this->value_len, thisStoragePrec);
                break;

            default:
                printf("unknown type: %d\n", this->type);
                return;
        }
    }
}

void
AWDAttr::set_val(AWD_field_ptr val, awd_uint32 val_len, AWD_field_type val_type, int storage_type=0)
{
    this->value = val;
    this->value_len = val_len;
    this->type = val_type;
    this->storage_type = storage_type;
}

AWD_field_ptr
AWDAttr::get_val(awd_uint32 *val_len, AWD_field_type *val_type)
{
    *val_len = this->value_len;
    *val_type = this->type;
    return this->value;
}

awd_uint32
AWDAttr::get_val_len(BlockSettings * curBlockSettings)
{
    if (this->type==AWD_FIELD_FLOAT64){
        bool thisStoragePrec=curBlockSettings->get_wide_props();
        if (this->storage_type==1)
            thisStoragePrec=curBlockSettings->get_wide_matrix();
        else if (this->storage_type==2)
            thisStoragePrec=curBlockSettings->get_wide_geom();
        else if (this->storage_type==3)
            thisStoragePrec=curBlockSettings->get_wide_attributes();
        if (!thisStoragePrec){
            return this->value_len/2;
        }
    }
    return this->value_len;
}

AWDUserAttr::AWDUserAttr(AWDNamespace *ns, const char *key, awd_uint16 key_len)
{
    this->ns = ns;
    this->key_len = key_len;
    if (key != NULL) {
        this->key = (char*)malloc(this->key_len+1);
        strncpy_s(this->key, this->key_len, key, _TRUNCATE);
        this->key[this->key_len] = 0;
    }
    this->next = NULL;
}

AWDUserAttr::~AWDUserAttr()
{
    if (this->key) {
        free(this->key);
        this->key = NULL;
    }
    if (this->type!=AWD_FIELD_STRING){
        if(this->value.v!=NULL)
            free(this->value.v);
    }
}

AWDNamespace *
AWDUserAttr::get_ns()
{
    return this->ns;
}

const char *
AWDUserAttr::get_key()
{
    return this->key;
}

awd_uint16
AWDUserAttr::get_key_len()
{
    return this->key_len;
}

void
AWDUserAttr::write_metadata(int fd)
{
    awd_uint8 type;
    awd_uint32 len_be;
    awd_nsid ns_handle;

    len_be = UI32(this->value_len);
    type = (awd_uint8)this->type;
    ns_handle = this->ns->get_handle();

    write(fd, &ns_handle, sizeof(awd_uint8));
    awdutil_write_varstr(fd, this->key, this->key_len);
    write(fd, &type, sizeof(awd_uint8));
    write(fd, &len_be, sizeof(awd_uint32));
}

AWDUserAttrList::AWDUserAttrList()
{
    this->first_attr = NULL;
    this->last_attr = NULL;
}

AWDUserAttrList::~AWDUserAttrList()
{
    AWDUserAttr *cur;

    cur = this->first_attr;
    while (cur) {
        AWDUserAttr *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }

    // These will have been destroyed as
    // part of the above loop.
    this->first_attr = NULL;
    this->last_attr = NULL;
}

awd_uint32
AWDUserAttrList::calc_length(BlockSettings * blockSettings)
{
    awd_uint32 len;
    AWDUserAttr *cur;

    // List length field always included
    len = sizeof(awd_uint32);

    // Accumulate size of individual attributes
    cur = this->first_attr;
    while (cur) {
        // Meta-data takes up 8 bytes
        len += (8 + (awd_uint32)cur->get_key_len() + (awd_uint32)cur->get_val_len(blockSettings));
        cur = cur->next;
    }

    return len;
}

void
AWDUserAttrList::write_attributes(int fd, BlockSettings * blockSettings)
{
    awd_uint32 len_be;
    AWDUserAttr *cur;

    len_be = UI32(this->calc_length(blockSettings) - sizeof(awd_uint32));
    write(fd, &len_be, sizeof(awd_uint32));

    cur = this->first_attr;
    while (cur) {
        cur->write_attr(fd, blockSettings);
        cur = cur->next;
    }
}

AWDUserAttr *
AWDUserAttrList::find(AWDNamespace *ns, const char *key, awd_uint16 key_len)
{
    if (this->first_attr) {
        AWDUserAttr *cur;
        cur = this->first_attr;
        while (cur) {
            // Move on if not in the same namespace
            if (cur->get_ns() != ns)
                continue;

            if (key_len == cur->get_key_len()) {
                if (strncmp(key, cur->get_key(), key_len)==0)
                    return cur;
            }

            cur = cur->next;
        }
    }

    return NULL;
}

bool
AWDUserAttrList::get(AWDNamespace *ns, const char *key, awd_uint16 key_len,
    AWD_field_ptr *val, awd_uint32 *val_len, AWD_field_type *val_type)
{
    AWDUserAttr *attr;

    attr = this->find(ns, key, key_len);
    if (attr) {
        *val = attr->get_val(val_len, val_type);
        return true;
    }

    return false;
}

void
AWDUserAttrList::set(AWDNamespace *ns, const char *key, awd_uint16 key_len,
    AWD_field_ptr value, awd_uint32 value_length, AWD_field_type type)
{
    bool created=false;
    AWDUserAttr *attr;

    attr = this->find(ns, key, key_len);
    if (attr == NULL) {
        attr = new AWDUserAttr(ns, key, key_len);
        created = true;
    }

    attr->set_val(value, value_length, type, 3);

    // Add to internal list if the attribute wasn't
    // originally found there.
    if (created) {
        if (!this->first_attr) {
            this->first_attr = attr;
        }
        else {
            this->last_attr->next = attr;
        }

        this->last_attr = attr;
        this->last_attr->next = NULL;
    }
}

/*
void
AWDUserAttrList::add_namespaces(AWD *awd)
{
    AWDUserAttr *cur = this->first_attr;
    while (cur) {
        awd->add_namespace(cur->get_ns());
        cur = cur->next;
    }
}
*/

AWDNumAttr::AWDNumAttr()
{
    this->next = NULL;
}

AWDNumAttr::~AWDNumAttr()
{
    this->next = NULL;
    if (this->type!=AWD_FIELD_STRING){
        if(this->value.v!=NULL)
            free(this->value.v);
    }
}

void
AWDNumAttr::write_metadata(int fd)
{
    awd_uint16 key_be;
    awd_uint32 len_be;

    key_be = UI16(this->key);
    len_be = UI32(this->value_len);

    write(fd, &key_be, sizeof(awd_uint16));
    write(fd, &len_be, sizeof(awd_uint32));
}

AWDNumAttrList::AWDNumAttrList()
{
    this->first_attr = NULL;
    this->last_attr = NULL;
}

AWDNumAttrList::~AWDNumAttrList()
{
    AWDNumAttr *cur;

    cur = this->first_attr;
    while (cur) {
        AWDNumAttr *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }

    // Already deleted as part of
    // the above loop.
    this->first_attr = NULL;
    this->last_attr = NULL;
}

awd_uint32
AWDNumAttrList::calc_length(BlockSettings * blockSettings)
{
    awd_uint32 len;
    AWDNumAttr *cur;

    // Attr list length always included
    len = sizeof(awd_uint32);

    cur = this->first_attr;
    while (cur) {
        // Meta-data is always six bytes
        len += (6 + cur->get_val_len(blockSettings));
        cur = cur->next;
    }

    return len;
}

void
AWDNumAttrList::write_attributes(int fd, BlockSettings * blockSettings)
{
    awd_uint32 len_be;
    AWDNumAttr *cur;

    len_be = UI32(this->calc_length(blockSettings) - sizeof(awd_uint32));
    write(fd, &len_be, sizeof(awd_uint32));

    cur = this->first_attr;
    while (cur) {
        cur->write_attr(fd, blockSettings);
        cur = cur->next;
    }
}

AWDNumAttr *
AWDNumAttrList::find(awd_propkey key)
{
    if (this->first_attr) {
        AWDNumAttr *cur;

        cur = this->first_attr;
        while (cur) {
            if (cur->key == key)
                return cur;
            cur = cur->next;
        }
    }

    return NULL;
}

bool
AWDNumAttrList::get(awd_propkey key, AWD_field_ptr *val, awd_uint32 *val_len, AWD_field_type *val_type)
{
    AWDNumAttr *attr;

    attr = this->find(key);
    if (attr) {
        *val = attr->get_val(val_len, val_type);
        return true;
    }

    // Return null if reached
    return false;
}

void
AWDNumAttrList::set(awd_propkey key, AWD_field_ptr value, awd_uint32 value_length, AWD_field_type type, int store_type)
{
    bool created;
    AWDNumAttr *attr;

    created = false;
    attr = this->find(key);
    if (!attr) {
        attr = new AWDNumAttr();
        attr->key = key;

        created = true;
    }

    attr->set_val(value, value_length, type, store_type);

    // Add to internal list if the attribute wasn't
    // originally found there.
    if (created) {
        if (!this->first_attr) {
            this->first_attr = attr;
        }
        else {
            this->last_attr->next = attr;
        }

        this->last_attr = attr;
        this->last_attr->next = NULL;
    }
}

AWDAttrElement::AWDAttrElement()
{
    this->properties = new AWDNumAttrList();
    this->user_attributes = new AWDUserAttrList();
}

AWDAttrElement::~AWDAttrElement()
{
    delete this->properties;
    delete this->user_attributes;
}

/*
void
AWDAttrElement::add_dependencies(AWD *awd)
{
    this->user_attributes->add_namespaces(awd);
}
*/

awd_uint32
AWDAttrElement::calc_attr_length(bool with_props, bool with_user_attr, BlockSettings * blockSettings )
{
    awd_uint32 len;

    len = 0;
    if (with_props) len += this->properties->calc_length(blockSettings);
    if (with_user_attr) len += this->user_attributes->calc_length(blockSettings);

    return len;
}

bool
AWDAttrElement::get_attr(AWDNamespace *ns, const char *key, awd_uint16 key_len,
    AWD_field_ptr *val, awd_uint32 *val_len, AWD_field_type *val_type)
{
    return this->user_attributes->get(ns, key, key_len, val, val_len, val_type);
}

void
AWDAttrElement::set_attr(AWDNamespace *ns, const char *key, awd_uint16 key_len,
    AWD_field_ptr val, awd_uint32 val_len, AWD_field_type val_type)
{
    this->user_attributes->set(ns, key, key_len, val, val_len, val_type);
}

void
AWDAttrElement::add_color_property(int targetID, awd_uint32 targetValue, awd_uint32 defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint32));
        *newVal.col = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_uint32), AWD_FIELD_COLOR);
    }
}
void
AWDAttrElement::add_number_property(int targetID, float targetValue, float defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_float64));
        *newVal.f64 = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_float64), AWD_FIELD_FLOAT64);
    }
}
void
AWDAttrElement::add_int_property(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint16));
        *newVal.ui16 = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_uint16), AWD_FIELD_UINT16);
    }
}
void
AWDAttrElement::add_int8_property(int targetID, int targetValue, int defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_uint8));
        *newVal.ui8 = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_uint8), AWD_FIELD_UINT8);
    }
}
void
AWDAttrElement::add_bool_property(int targetID, bool targetValue, bool defaultValue)
{
    if (targetValue!=defaultValue){
        AWD_field_ptr newVal;
        newVal.v = malloc(sizeof(awd_bool));
        *newVal.b = targetValue;
        this->properties->set(targetID, newVal, sizeof(awd_bool), AWD_FIELD_BOOL);
    }
}
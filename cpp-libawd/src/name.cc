#include <cstring>
#include <stdlib.h>

#include "name.h"


AWDNamedElement::AWDNamedElement(const char *name, awd_uint16 name_len)
{
    this->name = NULL;
    this->name_len = 0;
    this->set_name(name, name_len);
}

AWDNamedElement::~AWDNamedElement()
{
    if (this->name) {
        free(this->name);
        this->name = NULL;
    }
}


char *
AWDNamedElement::get_name()
{
    return this->name;
}


void
AWDNamedElement::set_name(const char *name, awd_uint16 name_len)
{
    this->name_len = name_len;
    if (name != NULL) {
        this->name = (char*)malloc(this->name_len+1);
        strncpy(this->name, name, this->name_len);
        this->name[this->name_len] = 0;
    }
}


awd_uint16
AWDNamedElement::get_name_length()
{
    return this->name_len;
}


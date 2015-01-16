#include <cstring>
#include <ctime>
#include <iostream>
#include "name.h"
#include "platform.h"

AWDNamedElement::AWDNamedElement(const char *name, awd_uint16 name_len)
{
    this->name = NULL;
    this->name_len = 0;
    this->awdID = NULL;
    this->awdID_len = 0;
    this->targetAWD = NULL;
    this->targetAWD_len = 0;
    this->set_name(name, name_len);
}

AWDNamedElement::~AWDNamedElement()
{
    if (this->name_len>0) {
        free(this->name);
        this->name_len=NULL;
        this->name = NULL;
    }
    if (this->awdID_len>0) {
        free(this->awdID);
        this->awdID_len=NULL;
        this->awdID = NULL;
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
    if ((name != NULL)&&(name_len>0)) {
        if (this->name_len>0){
            free(this->name);
            this->name=NULL;
            this->name_len=0;
        }
        this->name_len = name_len+1;
        this->name = (char*)malloc(this->name_len);
        strncpy_s(this->name, this->name_len, name, _TRUNCATE);
       }
}
char *
AWDNamedElement::get_targetAWD()
{
    return this->targetAWD;
}
void
AWDNamedElement::set_targetAWD(const char *targetAWD, awd_uint16 targetAWD_len)
{
    if ((targetAWD != NULL)&&(targetAWD_len>0)) {
        if (this->targetAWD_len>0){
            free(this->targetAWD);
            this->targetAWD=NULL;
            this->targetAWD_len=0;
        }
        this->targetAWD_len = targetAWD_len+1;
        this->targetAWD = (char*)malloc(this->targetAWD_len);
        strncpy_s(this->targetAWD, this->targetAWD_len, targetAWD, _TRUNCATE);
       }
}
char *
AWDNamedElement::get_awdID()
{
    if (this->awdID_len==0)
        this->generate_awdID();
    return this->awdID;
}

char *
AWDNamedElement::generate_awdID()
{
    time_t timev=time(0);
    char buffer [1024];
    char * timeStr=(char *)itoa ((int)timev,buffer,10);
    int r = int((rand() % 89999 + 10000));
    char buffer2 [20];
    char * valueStr=(char *)itoa (int(r),buffer2,10);

    if (this->awdID_len>0){
        free(this->awdID);
        this->awdID=NULL;
        this->awdID_len=0;
    }
    this->awdID_len = strlen(valueStr)+strlen(timeStr)+4+3;
    this->awdID = (char*)malloc(this->awdID_len);
    strncpy_s(this->awdID, this->awdID_len, "awd_", _TRUNCATE);
    strcat_s(this->awdID, this->awdID_len, timeStr);
    strcat_s(this->awdID, this->awdID_len, valueStr);
    return this->awdID;
}
void
AWDNamedElement::set_awdID(const char *awdID, awd_uint16 awdID_len)
{
    if ((awdID != NULL)&&(awdID_len>0)) {
        if (this->awdID_len>0){
            free(this->awdID);
            this->awdID=NULL;
            this->awdID_len=0;
        }
        this->awdID_len = awdID_len+1;
        this->awdID = (char*)malloc(this->awdID_len);
        strncpy_s(this->awdID, this->awdID_len, awdID, _TRUNCATE);
        //this->name[this->name_len] = 0;
   }
}

awd_uint16
AWDNamedElement::get_name_length()
{
    return this->name_len;
}

awd_uint16
AWDNamedElement::get_awdID_length()
{
    return this->awdID_len;
}
awd_uint16
AWDNamedElement::get_targetAWD_length()
{
    return this->targetAWD_len;
}
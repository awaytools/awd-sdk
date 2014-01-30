#include <cstdio>
#include <string.h>

#include "platform.h"
#include "meta.h"
#include "awd.h"

AWDMetaData::AWDMetaData() :
    AWDBlock(METADATA),
    AWDAttrElement()
{
	this->generator_name=NULL;
    this->generator_name_len = 0;
	this->generator_version=NULL;
    this->generator_version_len = 0;
    this->encoder_name = (char *)malloc(7);
    memcpy(this->encoder_name, "libawd", 7);
    this->encoder_name_len = 7;

    this->encoder_version = (char *)malloc(255);
    this->encoder_version_len = 255;
    snprintf(this->encoder_version, 255, "%d.%d.%d%c",
        AWD::VERSION_MAJOR, AWD::VERSION_MINOR, AWD::VERSION_BUILD, AWD::VERSION_RELEASE);
}

AWDMetaData::~AWDMetaData() {

 if(this->generator_name_len>0) {
    free(this->generator_name);
    this->generator_name = NULL;
 }
 if(this->generator_version_len>0) {
    free(this->generator_version);
    this->generator_version = NULL;
 }

 if(this->encoder_name_len>0) {
    free(this->encoder_name);
    this->encoder_name = NULL;
 }
 if(this->encoder_version_len>0) {
    free(this->encoder_version);
    this->encoder_version = NULL;
 }
 this->encoder_version_len=0;
 this->encoder_name_len=0;
 this->generator_version_len=0;
 this->generator_name_len=0;
}
void
AWDMetaData::override_encoder_metadata(const char *name, const char *version)
{
	if(this->encoder_name_len>0)
		free(this->encoder_name);
	if(this->encoder_version_len>0)
		free(this->encoder_version);
    if (name != NULL) {
		this->encoder_name_len=strlen(name);
        this->encoder_name = (char*)malloc(strlen(name)+1);
        strncpy(this->encoder_name, name, this->encoder_name_len);
        this->encoder_name[this->encoder_name_len] = 0;
    }
    if (version != NULL) {
		this->encoder_version_len=strlen(version);
        this->encoder_version = (char*)malloc(strlen(version)+1);
        strncpy(this->encoder_version, version, this->encoder_version_len);
        this->encoder_version[this->encoder_version_len] = 0;
    }
}
void
AWDMetaData::set_generator_metadata(const char *name, const char *version)
{
    if (name != NULL) {
		this->generator_name_len=strlen(name);
        this->generator_name = (char*)malloc(strlen(name)+1);
        strncpy(this->generator_name, name, this->generator_name_len);
        this->generator_name[this->generator_name_len] = 0;
    }
    if (version != NULL) {
		this->generator_version_len=strlen(version);
        this->generator_version = (char*)malloc(strlen(version)+1);
        strncpy(this->generator_version, version, this->generator_version_len);
        this->generator_version[this->generator_version_len] = 0;
    }
}

void
AWDMetaData::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    AWD_field_ptr val;
	
    //val.v= malloc(strlen(this->encoder_name)+1);
    //strncpy(val.str, this->encoder_name, this->encoder_name_len);
    val.str = this->encoder_name;
	//val.str[this->encoder_name_len]=0;
    this->properties->set(PROP_META_ENCODER_NAME, val, (awd_uint32)strlen(this->encoder_name), AWD_FIELD_STRING);
    
    AWD_field_ptr val2;
    //val2.v= malloc(strlen(this->encoder_version)+1);
    //strncpy(val2.v, this->encoder_version, this->encoder_version_len);
    val2.str = this->encoder_version;
    //val.str = (char *)val.v;
	//val.str[this->encoder_version_len]=0;
    this->properties->set(PROP_META_ENCODER_VER, val2, (awd_uint32)strlen(this->encoder_version), AWD_FIELD_STRING);
	
	if (this->generator_name_len>0){
		AWD_field_ptr val3;
		//val3.v= malloc(strlen(this->generator_name)+1);
		//strncpy(val3.str, this->generator_name, this->generator_name_len);
		val3.str = this->generator_name;
		//val.str = (char *)val.v;
		//val.str[this->generator_name_len]=0;
		this->properties->set(PROP_META_GENERATOR_NAME, val3, (awd_uint32)strlen(this->generator_name), AWD_FIELD_STRING);
	}
	if (this->generator_version_len>0){
		AWD_field_ptr val4;
		val4.str = this->generator_version;
		//val4.v= malloc(strlen(this->generator_version)+1);
		//strncpy(val4.str, this->generator_version, this->generator_version_len);
		//val.str = (char *)val.v;
		//val.str[this->generator_version_len]=0;
		this->properties->set(PROP_META_GENERATOR_VER, val4, (awd_uint32)strlen(this->generator_version), AWD_FIELD_STRING);
	}
	
}


awd_uint32
AWDMetaData::calc_body_length(BlockSettings * curBlockSettings)
{
    return this->calc_attr_length(true, false, curBlockSettings->get_wide_matrix());
}


void
AWDMetaData::write_body(int fd, BlockSettings *curBlockSettings)
{
    this->properties->write_attributes(fd, curBlockSettings->get_wide_matrix());
}


#include "util.h"
#include "texture.h"
#include <sys/stat.h>

#include "platform.h"

AWDBitmapTexture::AWDBitmapTexture(const char *name, awd_uint16 name_len) :
    AWDBlock(BITMAP_TEXTURE),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->saveType = UNDEFINEDTEXTYPE;
    this->url = NULL;
    this->url_len = 0;
    this->embed_data = NULL;
    this->embed_data_len = 0;
    this->height = 0;
    this->width = 0;
    this->uvAnimSourceId = NULL;
    this->uvAnimSourceId_len = 0;
}

AWDBitmapTexture::~AWDBitmapTexture()
{
    if (this->embed_data_len>0) free(this->embed_data);
    if (this->url_len>0) free(this->url);
    this->url = NULL;
    this->url_len = 0;
    this->embed_data = NULL;
    this->embed_data_len = 0;
    if (this->uvAnimSourceId_len>0) {
        free(this->uvAnimSourceId);
        this->uvAnimSourceId_len=0;
        this->uvAnimSourceId = NULL;
    }
}

char *
AWDBitmapTexture::get_uvAnimSourceId()
{
    return this->uvAnimSourceId;
}
void
AWDBitmapTexture::set_uvAnimSourceId(const char * uvAnimSourceId, int uvAnimSourceId_len)
{
    if ((uvAnimSourceId != NULL)&&(uvAnimSourceId_len>0)) {
        if (this->uvAnimSourceId_len>0){
            free(this->uvAnimSourceId);
            this->uvAnimSourceId=NULL;
            this->uvAnimSourceId_len=0;
        }
        this->uvAnimSourceId_len = uvAnimSourceId_len+1;//the extra 1 was needed after replacer strncpy with strncpy_s
        this->uvAnimSourceId = (char*)malloc(this->uvAnimSourceId_len);
        strncpy_s(this->uvAnimSourceId, this->uvAnimSourceId_len, uvAnimSourceId, _TRUNCATE);
       }
}
int
AWDBitmapTexture::get_uvAnimSourceId_len()
{
    return this->uvAnimSourceId_len;
}
int
AWDBitmapTexture::get_width()
{
    return this->width;
}

void
AWDBitmapTexture::set_width(int width)
{
    this->width=width;
}
int
AWDBitmapTexture::get_height()
{
    return this->height;
}
void
AWDBitmapTexture::set_height(int height)
{
    this->height=height;
}
const char *
AWDBitmapTexture::get_url()
{
    return this->url;
}

awd_uint16
AWDBitmapTexture::get_url_length()
{
    return this->url_len;
}

void
AWDBitmapTexture::set_url(const char *url, awd_uint16 url_len)
{
    this->url = (char *) url;
    this->url_len = url_len;
}

AWD_tex_type
AWDBitmapTexture::get_tex_type()
{
    return this->saveType;
}
void
AWDBitmapTexture::set_tex_type(AWD_tex_type texType)
{
    this->saveType = texType;
}

awd_uint32
AWDBitmapTexture::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = sizeof(awd_uint32); //datalength;
    len += sizeof(awd_uint16) + this->get_name_length(); //name
    len += sizeof(awd_uint8); //save type (external/embbed)

    if (this->saveType == EXTERNAL) {
        len += this->url_len;
    }
    else {
        len += this->embed_data_len;
    }

    len += this->calc_attr_length(true, true, curBlockSettings);

    return len;
}

void
AWDBitmapTexture::set_embed_data(awd_uint8 *buf, awd_uint32 buf_len)
{
    this->embed_data = buf;
    this->embed_data_len = buf_len;
}

void
    AWDBitmapTexture::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}

awd_uint32
AWDBitmapTexture::calc_body_length_for_CubeTex(AWD_tex_type textype)
{
    awd_uint32 len;
    len = sizeof(awd_uint32); //datalength;
    if (textype==EXTERNAL){
        len += this->url_len;   }
    else {
        len += this->embed_data_len;    }
    if (len==sizeof(awd_uint32)){
        int error=0;
    }
    return len;
}
void
    AWDBitmapTexture::write_for_CubeTex(int fd, AWD_tex_type textype)
{
    awd_uint32 data_len;
    this->isExported=true;
    if (textype == EXTERNAL) {
        data_len = UI32(this->url_len);
        if (data_len==0){
            int error=0;
        }
        write(fd, &data_len, sizeof(awd_uint32));
        write(fd, this->url, this->url_len);
    }
    else {
        data_len = UI32(this->embed_data_len);
        if (data_len==0){
            int error=0;
        }
        write(fd, &data_len, sizeof(awd_uint32));
        write(fd, this->embed_data, this->embed_data_len);
    }
}

void
AWDBitmapTexture::write_body(int fd, BlockSettings * curBlockSettings)
{
    awd_uint32 data_len;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    write(fd, &this->saveType, sizeof(awd_uint8));
    if (this->saveType == EXTERNAL) {
        data_len = UI32(this->url_len);
        write(fd, &data_len, sizeof(awd_uint32));
        write(fd, this->url, this->url_len);
    }
    else {
        data_len = UI32(this->embed_data_len);
        write(fd, &data_len, sizeof(awd_uint32));
        write(fd, this->embed_data, this->embed_data_len);
    }

    this->properties->write_attributes(fd,  curBlockSettings);
    this->user_attributes->write_attributes(fd,  curBlockSettings);
}

AWDCubeTexture::AWDCubeTexture(const char *name, awd_uint16 name_len) :
    AWDBlock(CUBE_TEXTURE),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->saveType = UNDEFINEDTEXTYPE;
    this->texture_blocks=new AWDBlockList();
}

AWDCubeTexture::~AWDCubeTexture()
{
    delete this->texture_blocks;
    this->texture_blocks=NULL;
}

AWDBlockList *
AWDCubeTexture::get_texture_blocks()
{
    return this->texture_blocks;
}
void
AWDCubeTexture::set_texture_blocks(AWDBlockList * texture_blocks)
{
    this->texture_blocks = texture_blocks;
}

AWD_tex_type
AWDCubeTexture::get_tex_type()
{
    return this->saveType;
}
void
AWDCubeTexture::set_tex_type(AWD_tex_type texType)
{
    this->saveType = texType;
}

awd_uint32
AWDCubeTexture::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    int len;

    len = sizeof(awd_uint16) + this->get_name_length(); //name
    len += sizeof(awd_uint8); //save type (external/embbed)
    if (this->texture_blocks->get_num_blocks()==1){
        this->type=CUBE_TEXTURE_ATF;
    }
    else if (this->texture_blocks->get_num_blocks()==6){
        this->type=CUBE_TEXTURE;
    }
    else {
        //TODO: HANDLE ERROR HERE; OR DO IT BEFORE THIS
    }
    AWDBitmapTexture *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->texture_blocks);
    while ((block = (AWDBitmapTexture *)it->next()) != NULL) {
        len += block->calc_body_length_for_CubeTex(this->saveType);
    }
    delete it;
    len += this->calc_attr_length(true, true, curBlockSettings);
    return len;
}

void
AWDCubeTexture::prepare_and_add_dependencies(AWDBlockList * targetList)
{
    // Do nothing
}

void
AWDCubeTexture::write_body(int fd, BlockSettings *curBlockSettings)
{
    awd_uint8 saveType = (awd_uint8)this->saveType;
    write(fd, &saveType, sizeof(awd_uint8));
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    AWDBitmapTexture *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->texture_blocks);
    while ((block = (AWDBitmapTexture *)it->next()) != NULL)
        block->write_for_CubeTex(fd,this->saveType);
    delete it;
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
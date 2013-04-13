#include "util.h"
#include "texture.h"
#include <sys/stat.h>

#include "platform.h"

AWDBitmapTexture::AWDBitmapTexture(AWD_tex_type type, const char *name, awd_uint16 name_len) :
    AWDBlock(BITMAP_TEXTURE),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->type = type;
    this->url = NULL;
    this->url_len = 0;
    this->embed_data = NULL;
    this->embed_data_len = 0;
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
    this->url = url;
    this->url_len = url_len;
}


awd_uint32
AWDBitmapTexture::calc_body_length(bool wide_mtx)
{
    awd_uint32 len;

    len = 7;
    len += this->get_name_length();

    if (this->type == EXTERNAL) {
        len += this->url_len;
    }
    else {
        len += this->embed_data_len;
    }

    len += this->calc_attr_length(true, true, wide_mtx);

    return len;
}


void
AWDBitmapTexture::set_embed_data(awd_uint8 *buf, awd_uint32 buf_len)
{
    this->embed_data = buf;
    this->embed_data_len = buf_len;
}


void
AWDBitmapTexture::set_embed_file_data(int fd)
{
    struct stat *s;

    s = (struct stat *)malloc(sizeof(struct stat));
    fstat(fd, s);
    this->embed_data_len = s->st_size;
    this->embed_data = (awd_uint8 *)malloc(this->embed_data_len);

    lseek(fd, 0, SEEK_SET);
    read(fd, this->embed_data, this->embed_data_len);
}


void 
AWDBitmapTexture::prepare_write()
{
    // Do nothing
}


void
AWDBitmapTexture::write_body(int fd, bool wide_mtx)
{
    awd_uint32 data_len;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    write(fd, &this->type, sizeof(awd_uint8));
    if (this->type == EXTERNAL) {
        data_len = UI32(this->url_len);
        write(fd, &data_len, sizeof(awd_uint32));
        write(fd, this->url, this->url_len);
    }
    else {
        data_len = UI32(this->embed_data_len);
        write(fd, &data_len, sizeof(awd_uint32));
        write(fd, this->embed_data, this->embed_data_len);
    }

    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}



AWDCubeTexture::AWDCubeTexture(const char *name, awd_uint16 name_len) :
    AWDBlock(CUBE_TEXTURE),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->sides = (AWDBlock **)malloc(6*sizeof(AWDBlock *));
    this->sides[0] = NULL;
    this->sides[1] = NULL;
    this->sides[2] = NULL;
    this->sides[3] = NULL;
    this->sides[4] = NULL;
    this->sides[5] = NULL;
}

AWDCubeTexture::~AWDCubeTexture()
{
    free(this->sides);
}


void
AWDCubeTexture::set_dir_tex(AWD_cube_dir dir, AWDBlock *texture)
{
    this->sides[(int)dir] = texture;
}

AWDBlock *
AWDCubeTexture::get_dir_tex(AWD_cube_dir dir)
{
    return this->sides[(int)dir];
}


awd_uint32
AWDCubeTexture::calc_body_length(bool wide_mtx)
{
    return (2 + this->get_name_length()) + 
        6 * sizeof(awd_baddr) + 
        this->calc_attr_length(true, true, wide_mtx);
}


void
AWDCubeTexture::prepare_write()
{
    // Do nothing
}


void
AWDCubeTexture::write_dir_tex(int fd, AWD_cube_dir dir)
{
    AWDBlock *side;

    side = this->sides[(int)dir];
    if (side != NULL) {
        awd_baddr addr = UI32(side->get_addr());
        write(fd, &addr, sizeof(awd_baddr));
    }
}


void
AWDCubeTexture::write_body(int fd, bool wide_mtx)
{
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    this->write_dir_tex(fd, POS_X);
    this->write_dir_tex(fd, NEG_X);
    this->write_dir_tex(fd, POS_Y);
    this->write_dir_tex(fd, NEG_Y);
    this->write_dir_tex(fd, POS_Z);
    this->write_dir_tex(fd, NEG_Z);

    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}


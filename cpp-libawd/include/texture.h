#ifndef LIBAWD_TEXTURE_H
#define LIBAWD_TEXTURE_H

#include "awd_types.h"
#include "block.h"
#include "name.h"
#include "attr.h"


typedef enum {
    EXTERNAL=0,
    EMBEDDED
} AWD_tex_type;

class AWDBitmapTexture : 
    public AWDBlock, 
    public AWDNamedElement, 
    public AWDAttrElement
{
    private:
        AWD_tex_type type;
        const char *url;
        awd_uint16 url_len;
        awd_uint8 *embed_data;
        awd_uint32 embed_data_len;
    
    protected:
        awd_uint32 calc_body_length(bool);
        void prepare_write();
        void write_body(int, bool);

    public:
        AWDBitmapTexture(AWD_tex_type, const char *, awd_uint16);

        void set_embed_data(awd_uint8 *, awd_uint32);
        void set_embed_file_data(int);

        const char *get_url();
        awd_uint16 get_url_length();
        void set_url(const char *, awd_uint16);
};


typedef enum {
    POS_X=0,
    NEG_X,
    POS_Y,
    NEG_Y,
    POS_Z,
    NEG_Z
} AWD_cube_dir;


class AWDCubeTexture :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWDBlock **sides; // Array of textures

        void write_dir_tex(int, AWD_cube_dir);

    protected:
        awd_uint32 calc_body_length(bool);
        void prepare_write();
        void write_body(int, bool);

    public:
        AWDCubeTexture(const char *, awd_uint16);
        ~AWDCubeTexture();

        void set_dir_tex(AWD_cube_dir, AWDBlock *);
        AWDBlock *get_dir_tex(AWD_cube_dir);
};

#endif

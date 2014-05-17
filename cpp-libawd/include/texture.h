#ifndef LIBAWD_TEXTURE_H
#define LIBAWD_TEXTURE_H

#include "awd_types.h"
#include "block.h"
#include "name.h"
#include "attr.h"

typedef enum {
    EXTERNAL=0,
    EMBEDDED,
    UNDEFINEDTEXTYPE
} AWD_tex_type;
typedef enum {
    MATDIFFUSE=0,
    MATSPECULAR,
    MATAMBIENT,
    MATNORMAL,
    SHADINGLIGHTMAP,
    SHADINGGRADIENTDIFFUSE,
    SHADINGSIMPLEWATERNORMAL,
    FXENVMAPMASK,
    FXLIGHTMAP,
    FXMASK,
    FORCUBETEXTURE,
    FORTEXTUREPROJECTOR
} AWDTexPurpose_type;

class AWDBitmapTexture :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_tex_type saveType;
        char *url;
        awd_uint16 url_len;
        awd_uint8 *embed_data;
        awd_uint32 embed_data_len;
        int height;
        int width;
        char * uvAnimSourceId;
        int uvAnimSourceId_len;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        void write_body(int, BlockSettings *);

    public:
        AWDBitmapTexture(const char *, awd_uint16);
        ~AWDBitmapTexture();

        void set_embed_data(awd_uint8 *, awd_uint32);

        const char *get_url();
        awd_uint16 get_url_length();
        void set_url(const char *, awd_uint16);
        void set_uvAnimSourceId(const char *, int);
        char * get_uvAnimSourceId();
        int get_uvAnimSourceId_len();

        AWD_tex_type get_tex_type();
        void set_tex_type(AWD_tex_type);
        int get_height();
        void set_height(int);
        int get_width();
        void set_width(int);
        void write_for_CubeTex(int, AWD_tex_type);
        awd_uint32 calc_body_length_for_CubeTex(AWD_tex_type);
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
        AWD_tex_type saveType;
        AWDBlockList * texture_blocks;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        void write_body(int, BlockSettings *);

    public:
        AWDCubeTexture(const char *, awd_uint16);
        ~AWDCubeTexture();

        AWDBlockList * get_texture_blocks();
        void set_texture_blocks(AWDBlockList *);

        AWD_tex_type get_tex_type();
        void set_tex_type(AWD_tex_type);
};

#endif

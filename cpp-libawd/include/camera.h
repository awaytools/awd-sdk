#ifndef _LIBAWD_CAMERA_H
#define _LIBAWD_CAMERA_H

#include "scene.h"
#include "block.h"

typedef enum {
    AWD_CAM_FREE,
    AWD_CAM_TARGET,
    AWD_CAM_HOVER,
} AWD_cam_type;

typedef enum {
    AWD_LENS_PERSPECTIVE = 5001,
    AWD_LENS_ORTHO = 5002,
    AWD_LENS_ORTHOOFFCENTER = 5003,
} AWD_lens_type;

#define PROP_CAM_FOV 101

class AWDCamera :
    public AWDSceneBlock
{
    private:
        AWD_cam_type cam_type;
        AWD_lens_type lens_type;
        AWDNumAttrList * lens_properties;
        awd_float64 lens_fov;
        awd_float64 lens_near;
        awd_float64 lens_far;
        awd_float64 lens_proj_height;
        awd_float64 lens_offX_pos;
        awd_float64 lens_offX_neg;
        awd_float64 lens_offY_pos;
        awd_float64 lens_offY_neg;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDCamera(const char *, awd_uint16, AWD_lens_type, awd_float64 *);
        ~AWDCamera();

        awd_float64 get_lens_fov();
        void set_lens_fov(awd_float64);
        AWD_cam_type get_cam_type();
        void set_cam_type(AWD_cam_type);
        AWD_lens_type get_lens_type();
        void set_lens_type(AWD_lens_type);
        awd_float64 get_lens_near();
        void set_lens_near(awd_float64);
        awd_float64 get_lens_far();
        void set_lens_far(awd_float64);
        awd_float64 get_lens_proj_height();
        void set_lens_proj_height(awd_float64);
        void set_lens_offset(awd_float64, awd_float64, awd_float64, awd_float64);
};

class AWDTextureProjector :
    public AWDSceneBlock
{
    private:
        AWDBlock * texture;
        awd_float64 field_of_view;
        awd_float64 aspect_ratio;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *export_list);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDTextureProjector(const char *, awd_uint16,  awd_float64 *);
        ~AWDTextureProjector();

        awd_float64 get_field_of_view();
        void set_field_of_view(awd_float64);
        awd_float64 get_aspect_ratio();
        void set_aspect_ratio(awd_float64);
        AWDBlock * get_texture();
        void set_texture(AWDBlock *);
};
#endif

#ifndef _LIBAWD_CAMERA_H
#define _LIBAWD_CAMERA_H

#include "scene.h"

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
        AWD_cam_type type;
        AWD_lens_type lens;
		AWDNumAttrList * lens_properties;
        awd_float64 lens_fov;

    protected:
        void prepare_and_add_dependencies(AWDBlockList *);
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);

    public:
        AWDCamera(const char *, awd_uint16, AWD_lens_type, awd_float64 *);
        ~AWDCamera();

        awd_float64 get_lens_fov();
        void set_lens_fov(awd_float64 fov);
};

#endif

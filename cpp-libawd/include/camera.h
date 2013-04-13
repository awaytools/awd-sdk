#ifndef _LIBAWD_CAMERA_H
#define _LIBAWD_CAMERA_H

#include "scene.h"

typedef enum {
    AWD_CAM_FREE,
    AWD_CAM_TARGET,
    AWD_CAM_HOVER,
} AWD_cam_type;

typedef enum {
    AWD_LENS_ORTHO,
    AWD_LENS_PERSPECTIVE,
} AWD_lens_type;


#define PROP_CAM_FOV 1


class AWDCamera :
    public AWDSceneBlock
{
    private:
        AWD_cam_type type;
        AWD_lens_type lens;

        awd_float64 lens_fov;

    protected:
        void prepare_write();
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDCamera(const char *, awd_uint16, AWD_cam_type, AWD_lens_type);
        ~AWDCamera();

        awd_float64 get_lens_fov();
        void set_lens_fov(awd_float64 fov);
};

#endif

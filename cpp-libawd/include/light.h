#ifndef _LIBAWD_LIGHT_H
#define _LIBAWD_LIGHT_H

#include "scene.h"


typedef enum {
    AWD_LIGHT_POINT = 1,
    AWD_LIGHT_DIR,
} AWD_light_type;

class AWDLight :
    public AWDSceneBlock
{
    private:
        AWD_light_type type;

    protected:
        void prepare_write();
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDLight(const char *, awd_uint16, AWD_light_type);
        ~AWDLight();
};

#endif

#ifndef _LIBAWD_PRIMITIVE_H
#define _LIBAWD_PRIMITIVE_H

#include "scene.h"


#define PROP_PRIM_WIDTH 1
#define PROP_PRIM_HEIGHT 2
#define PROP_PRIM_DEPTH 3
#define PROP_PRIM_RADIUS 4
#define PROP_PRIM_SEGMENTS_X 5
#define PROP_PRIM_SEGMENTS_Y 6
#define PROP_PRIM_SEGMENTS_Z 7


typedef enum {
    AWD_PRIMITIVE_PLANE=1,
    AWD_PRIMITIVE_CUBE,
    AWD_PRIMITIVE_SPHERE,
    AWD_PRIMITIVE_CYLINDER,
    AWD_PRIMITIVE_CONE
} AWD_primitive_type;


class AWDPrimitive :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        AWD_primitive_type type;

    protected:
        void prepare_write();
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDPrimitive(const char *, awd_uint16, AWD_primitive_type);
        ~AWDPrimitive();
};


#endif

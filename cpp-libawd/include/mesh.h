#ifndef _LIBAWD_MESH_H
#define _LIBAWD_MESH_H

#include "awd_types.h"
#include "material.h"
#include "scene.h"
#include "stream.h"
#include "block.h"
#include "name.h"
#include "attr.h"



/**
 * Data stream type
*/
typedef enum {
    VERTICES=1,
    TRIANGLES,
    UVS,
    VERTEX_NORMALS,
    VERTEX_TANGENTS,
    JOINT_INDICES,
    VERTEX_WEIGHTS,
} AWD_mesh_str_type;



class AWDSubGeom :
    public AWDAttrElement
{
    private:
        unsigned int num_streams;
        AWDDataStream * first_stream;
        AWDDataStream * last_stream;
        awd_uint32 calc_streams_length();

    public:
        AWDSubGeom();
        ~AWDSubGeom();

        AWDSubGeom * next;

        unsigned int get_num_streams();
        AWDDataStream *get_stream_at(unsigned int);
        void add_stream(AWD_mesh_str_type, AWD_field_type, AWD_str_ptr, awd_uint32);

        awd_uint32 calc_sub_length(bool);
        void write_sub(int, bool);
};


class AWDTriGeom : 
    public AWDBlock, 
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        unsigned int num_subs;
        AWDSubGeom * first_sub;
        AWDSubGeom * last_sub;

        awd_float64 * bind_mtx;

    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDTriGeom(const char *, awd_uint16);
        ~AWDTriGeom();

        unsigned int get_num_subs();
        AWDSubGeom *get_sub_at(unsigned int);
        void add_sub_mesh(AWDSubGeom *);

        awd_float64 *get_bind_mtx();
        void set_bind_mtx(awd_float64 *bind_mtx);
};



class AWDMeshInst : 
    public AWDSceneBlock
{
    private:
        AWDBlock * geom;
        AWDBlockList * materials;

        void init();

    protected:
        awd_uint32 calc_body_length(bool);
        void write_body(int, bool);

    public:
        AWDMeshInst(const char *, awd_uint16, AWDTriGeom *);
        AWDMeshInst(const char *, awd_uint16, AWDTriGeom *, awd_float64 *);
        ~AWDMeshInst();

        void add_material(AWDMaterial *);

        AWDBlock * get_geom();
        void set_geom(AWDBlock *);
};

#endif

#ifndef _LIBAWD_MESH_H
#define _LIBAWD_MESH_H

#include "awd_types.h"
#include "material.h"
#include "scene.h"
#include "stream.h"
#include "block.h"
#include "name.h"
#include "attr.h"
//#include "anim.h"

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
    SUVS,
    ALLVERTDATA,
    ORIGINALIDX//usedIntern
} AWD_mesh_str_type;

class AWDSubGeom :
    public AWDAttrElement
{
    private:
        unsigned int num_streams;
        AWDDataStream * first_stream;
        AWDDataStream * last_stream;
        awd_uint32 calc_streams_length();
        AWDBlockList * materials;
        AWD_str_ptr originalIdx;
        awd_uint32 originalIdx_num;

    public:
        AWDSubGeom(AWDBlockList *);
        ~AWDSubGeom();

        AWDSubGeom * next;

        unsigned int get_num_streams();
        AWDDataStream *get_stream_at(unsigned int);
        void add_stream(AWD_mesh_str_type, AWD_field_type, AWD_str_ptr, awd_uint32);

        void add_original_idx_data(AWD_str_ptr, awd_uint32);
        AWD_str_ptr get_original_idx_data();
        awd_uint32 get_original_idx_data_len();
        awd_uint32 calc_animations_streams_length();
        awd_uint32 calc_sub_length(BlockSettings *);
        void write_sub(int, BlockSettings *, double);
        void write_anim_sub(int, bool, double);
        AWDBlockList* get_materials();
        void set_materials(AWDBlockList *);
};

class AWDTriGeom :
    public AWDBlock,
    public AWDNamedElement,
    public AWDAttrElement
{
    private:
        bool is_created;
        bool split_faces;
        int originalPointCnt;
        unsigned int num_subs;
        AWDSubGeom * first_sub;
        AWDSubGeom * last_sub;
        AWDBlockList * meshInstanceList;

        awd_float64 * bind_mtx;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *curBlockSettings);

    public:
        AWDTriGeom(const char *, awd_uint16);
        ~AWDTriGeom();

        unsigned int get_num_subs();
        AWDSubGeom *get_sub_at(unsigned int);
        void add_sub_mesh(AWDSubGeom *);

        awd_float64 *get_bind_mtx();
        void set_bind_mtx(awd_float64 *bind_mtx);
        AWDBlockList * get_mesh_instance_list();
        void set_mesh_instance_list(AWDBlockList *);
        bool get_is_created();
        void set_is_created(bool);
        bool get_split_faces();
        void set_split_faces(bool);
        int get_originalPointCnt();
        void set_originalPointCnt(int);
        AWDSubGeom *  get_first_sub();
};

class AWDMeshInst:
    public AWDSceneBlock
{
    private:
        AWDBlock * geom;
        AWDBlockList * materials;
        AWDBlockList * pre_materials;
        AWDBlock * lightPicker;
        AWDBlock * animator;
        AWDBlock * defaultMat;
        void init();

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *curBlockSettings);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDMeshInst(const char *name, awd_uint16, AWDBlock *);
        AWDMeshInst(const char *name, awd_uint16, AWDBlock *, awd_float64 *);
        ~AWDMeshInst();

        void add_material(AWDMaterial *);
        AWDBlockList * get_pre_materials();
        void set_pre_materials(AWDBlockList *);

        AWDBlock * get_defaultMat();
        void set_defaultMat(AWDBlock *);
        AWDBlock * get_geom();
        void set_geom(AWDBlock *);
        AWDBlock* get_lightPicker();
        void set_lightPicker(AWDBlock *);
        void set_animator(AWDBlock *);
        AWDBlock *get_animator();
};

#endif

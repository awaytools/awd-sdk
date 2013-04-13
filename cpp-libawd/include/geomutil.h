#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

#include "mesh.h"

typedef struct _ninfluence {
    double nx;
    double ny;
    double nz;
    struct _ninfluence *next;
} ninfluence;


typedef struct _vdata {
    unsigned int orig_idx;
    int out_idx;

    // Position
    awd_float64 x;
    awd_float64 y;
    awd_float64 z;

    // UV
    awd_float64 u;
    awd_float64 v;

    // Normal
    awd_float64 nx;
    awd_float64 ny;
    awd_float64 nz;

    // Skinning
    int num_bindings;
    awd_float64 *weights;
    awd_uint32 *joints;

    int mtlid;

    bool force_hard;

    ninfluence *first_normal_influence;
    ninfluence *last_normal_influence;
} vdata;

typedef struct _vdata_list_item {
	vdata *vd;
	struct _vdata_list_item *next;
} vdata_list_item;

class VertexDataList
{
private:
	int num_items;
	vdata_list_item *cur;
	vdata_list_item *first;
	vdata_list_item *last;

public:
	VertexDataList();
	~VertexDataList();

	void append_vdata(vdata *);
	void clear();
	int get_num_items();

	void iter_reset();
	vdata *iter_next();
};

class AWDGeomUtil
{
private:
	VertexDataList *expanded;
	VertexDataList *collapsed;
	
	int max_orig_idx;
	int num_idx_lists;
	VertexDataList **per_idx_lists;

	void prepare_build();
    int has_vert(vdata *);

public:
    AWDGeomUtil();
    ~AWDGeomUtil();

    int joints_per_vertex;
    double normal_threshold;
    bool include_uv;
    bool include_normals;

    void append_vert_data(unsigned int,  double, double, double, double, double, double, double, double, bool);
    void append_vdata_struct(vdata *);
    int build_geom(AWDTriGeom *);
};


#endif

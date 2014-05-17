#ifndef _GEOMUTILSHELPER_H
#define _GEOMUTILSHELPER_H

#include "mesh.h"

// Config attribute macros
#define ATTR_BUF_LEN 1024
#define ATTREQ(str0,str1) strncmp(str0,str1,ATTR_BUF_LEN)==0
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
    // UV
    awd_float64 su;
    awd_float64 sv;

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
    bool weakReference;

public:
    VertexDataList(bool weakReference=true);
    ~VertexDataList();

    void append_vdata(vdata *);
    void clear();
    int get_num_items();

    void iter_reset();
    vdata *iter_next();
};

typedef struct GeomStringCacheItem_struct {
    char *key;
    int keyLen;
    void *val;
    GeomStringCacheItem_struct *next;
} GeomStringCacheItem;

class GeomStringCache
{
private:
    GeomStringCacheItem *firstItem;
    GeomStringCacheItem *lastItem;

public:
    GeomStringCache(void);
    ~GeomStringCache(void);

    void Set(char *key, void *val);
    void *Get(char *key);
    char *GetKeyByVal(void *val);
};

/**
 * BlockCache
 * Cache that is used generically to store any type of block or
 * sub-element in a look-up table by MAX object.
*/
typedef struct SubGeoGroupCacheItem_struct {
    char *key;
    int val;
    int keyLen;
    int matIdx;
    SubGeoGroupCacheItem_struct *next;
} SubGeoGroupCacheItem;

class SubGeoGroupCache
{
private:
    SubGeoGroupCacheItem *firstItem;
    SubGeoGroupCacheItem *lastItem;

public:
    SubGeoGroupCache(void);
    ~SubGeoGroupCache(void);

    void Set(char *key, int val, int matIdx);
    int Get(char *key);
    int GetIdxByMatIdx(int val);
};

class GUSubGeo
{
    private:
        int num_idx_lists;
        bool deleteMaterialList;
    public:
        AWDBlockList * materials;
        int vertCnt;
        int triCnt;
        int max_orig_idx;
        int joints_per_vertex;
        double normal_threshold;
        bool include_uv;
        bool include_suv;
        bool include_normals;
        bool split_by_mat;
        bool explode;
        VertexDataList **per_idx_lists;
        VertexDataList *expanded;
        VertexDataList *collapsed;
        bool isExported;
        GUSubGeo(AWDBlockList * ,bool, bool, bool, double, int, bool);
        ~GUSubGeo();

        void updatematerials(AWDBlockList * newMaterials);
        int has_vert(vdata *);
        void prepare_build();
        void checkLimits();
};

typedef struct _gusubgeo_list_item {
    GUSubGeo *subGeo;
    struct _gusubgeo_list_item *next;
} gusubgeo_list_item;
class AWDGeomUtil;//predefine, so we can use it inside GUSubGeoList / will be defined later in this file
class GUSubGeoList
{
    private:
        int num_blocks;

    public:
        int max_orig_idx;
        gusubgeo_list_item *first_block;
        gusubgeo_list_item *last_block;

        GUSubGeoList();
        ~GUSubGeoList();

        bool append(GUSubGeo *);
        void force_append(GUSubGeo *);
        bool contains(GUSubGeo *);
        GUSubGeo * get_subgeo_for_matID(int);
        int get_num_blocks();
};

class GUSubGeoIterator
{
    private:
        GUSubGeoList * list;
        gusubgeo_list_item * cur_block;

    public:
        GUSubGeoIterator(GUSubGeoList *);
        GUSubGeo * next();
        void reset();
};

class GUSubGeoGroup
{
    private:
        int num_sub_geos;
        bool is_converted;
    public:
        GUSubGeo *curSubGeo;
        AWDBlockList * materials;
        GUSubGeoList * subGeosList;
        int joints_per_vertex;
        double normal_threshold;
        bool include_uv;
        bool include_suv;
        bool include_normals;
        bool split_by_mat;
        bool explode;
        GUSubGeoGroup(AWDBlockList *,bool, bool, bool, double, int, bool);
        ~GUSubGeoGroup();
        int mtdIdx;
        void append_vdata(vdata *);
        void check_limits();
        bool get_is_converted();
        void set_is_converted(bool);
};

typedef struct _gusubgeogroup_list_item {
    GUSubGeoGroup *subGeoGroup;
    struct _gusubgeogroup_list_item *next;
} gusubgeogroup_list_item;
class AWDGeomUtil;//predefine, so we can use it inside GUSubGeoList / will be defined later in this file
class GUSubGeoGroupList
{
    private:
        int num_blocks;

    public:
        int max_orig_idx;
        gusubgeogroup_list_item *first_block;
        gusubgeogroup_list_item *last_block;

        GUSubGeoGroupList();
        ~GUSubGeoGroupList();

        bool append(GUSubGeoGroup *);
        void force_append(GUSubGeoGroup *);
        bool contains(GUSubGeoGroup *);
        GUSubGeoGroup * get_by_idx(int);
        int get_num_blocks();
};

class GUSubGeoGroupIterator
{
    private:
        GUSubGeoGroupList * list;
        gusubgeogroup_list_item * cur_block;

    public:
        GUSubGeoGroupIterator(GUSubGeoGroupList *);
        GUSubGeoGroup * next();
        void reset();
};

class GUGeo
{
    private:
        GUSubGeoGroupList *subGeoGroupList;
        AWDBlockList * meshBlocks;
    public:
        GUGeo(AWDMeshInst *);
        ~GUGeo();
        GUSubGeoGroupList * get_subGeomList();
        void add_subGeom(GUSubGeoGroup *);
        AWDBlockList * get_meshBlocks();
        void add_meshBlock(AWDMeshInst *);
        void set_meshBlocks(AWDBlockList *);
};

typedef struct _gugeo_list_item {
    GUGeo *geo;
    struct _gugeo_list_item *next;
} gugeo_list_item;
class GUGeoList
{
    private:
        int num_blocks;
        bool weakReference;
    public:
        int max_orig_idx;
        gugeo_list_item *first_geo;
        gugeo_list_item *last_geo;

        GUGeoList(bool weakReference=true);
        ~GUGeoList();

        bool append(GUGeo *);
        void force_append(GUGeo *);
        bool contains(GUGeo *);
        GUGeo * get_by_idx(int);
        int get_num_blocks();
};

class GUGeoIterator
{
    private:
        GUGeoList * list;
        gugeo_list_item * cur_geo;

    public:
        GUGeoIterator(GUGeoList *);
        GUGeo * next();
        void reset();
};
#endif
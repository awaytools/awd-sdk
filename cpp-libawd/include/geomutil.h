#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

#include "mesh.h"
#include "geomutilhelper.h"

class AWDGeomUtil
{
    private:
        GUGeoList *preGeoList;
        GUGeoList *geoList;
        AWD_field_type geoStreamType;
        GeomStringCache *geoCache;
        SubGeoGroupCache *subGeoGroupCache;

    public:
        AWDGeomUtil(bool split_by_mat, bool force_split, bool useUV, bool useSecUVs, bool useNormals, double normalThreshold, int joints_per_vertex, AWD_field_type geoStreamType);
        ~AWDGeomUtil();

        GUGeoList * get_geoList();

        void createPreGeometries(AWDBlockList *);
        void createGeometries();
        void add_new_sub_geo_to_preGUgeoms(AWDBlockList *, int);

        // to delete after refactor:
        int joints_per_vertex;
        double normal_threshold;
        bool include_uv;
        bool include_suv;
        bool include_normals;
        bool split_by_mat;
        bool force_split;

        int getSubGeoIdxForMatIdx(int);
        AWDBlockList * build_geom(AWDTriGeom *);
};

#endif

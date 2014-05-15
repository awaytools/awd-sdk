#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "geomutil.h"
#include "awd_types.h"
#include "geomutilhelper.h"

AWDGeomUtil::AWDGeomUtil(bool split_by_mat, bool force_split, bool useUV, bool useSecUVs, bool useNormals, double normalThreshold, int joints_per_vertex, AWD_field_type geoStreamType)
{
    this->geoStreamType=geoStreamType;
    this->preGeoList = new GUGeoList(false);
    this->geoList = new GUGeoList();
    this->subGeoGroupCache = new SubGeoGroupCache();
    this->geoCache = new GeomStringCache();

    this->force_split = force_split;
    this->include_uv = useUV;
    this->split_by_mat = split_by_mat;
    this->include_suv = useSecUVs;
    this->include_normals = useNormals;
    this->normal_threshold = normalThreshold;
    this->joints_per_vertex = joints_per_vertex;
}

AWDGeomUtil::~AWDGeomUtil()
{
    delete preGeoList;
    delete geoList;
    this->preGeoList=NULL;
    this->geoList=NULL; // we dont need to delete these, as they are allready contained in the geoList
    delete this->subGeoGroupCache;
    delete this->geoCache;
}

GUGeoList *
AWDGeomUtil::get_geoList()
{
    return this->geoList;
}
void
AWDGeomUtil::createPreGeometries(AWDBlockList * meshInstanceList)
{
    // create a pregeom for each mesh instance given...
    int t;
    if (this->preGeoList!=NULL)
        delete this->preGeoList;
    this->preGeoList = new GUGeoList(false);
    for (t=0; t<meshInstanceList->get_num_blocks(); t++)
        this->preGeoList->append(new GUGeo((AWDMeshInst *)meshInstanceList->getByIndex(t)));
}

void
AWDGeomUtil::add_new_sub_geo_to_preGUgeoms (AWDBlockList * materialList, int matIdx)
{
    int numGeos=this->preGeoList->get_num_blocks();
    if (materialList->get_num_blocks()!=numGeos){
        int error = 0;// this should not happen
    }
    else{
        int thisSubIdx=0;
        if (!this->force_split){
            // if we want to merge subgeometries that are sharing materials but have different mat-ids:
            char *spacer = "#";
            AWDBlockIterator * it;
            AWDMaterial * block;
            it = new AWDBlockIterator(materialList);
            int newNameLength=0;
            while ((block = (AWDMaterial*)it->next()) != NULL) {
                newNameLength+=(int)(strlen(block->get_awdID())+strlen(spacer)+2);
            }
            if(newNameLength>0){
                newNameLength+=strlen(spacer)+1;
                char * newName = (char*)malloc(newNameLength);
                strncpy_s(newName, newNameLength, spacer, _TRUNCATE);
                it->reset();
                while ((block = (AWDMaterial*)it->next()) != NULL) {
                    strcat_s(newName, newNameLength, spacer);
                    strcat_s(newName, newNameLength, block->get_awdID());
                }
                //newName[newNameLength]=0;
                int thisSubIdx=this->subGeoGroupCache->Get(newName);

                // if the cache returns a int thats <0, we create a new SubGeoGroup for each mesh
                if (thisSubIdx<0){
                    int meshInstCnt=0;
                    for (meshInstCnt=0; meshInstCnt<numGeos; meshInstCnt++){
                        GUGeo * thisGeo = this->preGeoList->get_by_idx(meshInstCnt);
                        thisSubIdx=thisGeo->get_subGeomList()->get_num_blocks();
                        AWDBlockList * thisAWDMatList=new AWDBlockList();
                        AWDMaterial * thisMat=(AWDMaterial *)materialList->getByIndex(meshInstCnt);
                        thisAWDMatList->append(thisMat);
                        bool useSecUV=false;
                        if (this->include_suv){
                            if (thisMat->get_secondMappingChannel()>0){
                                useSecUV=true;
                            }
                        }
                        GUSubGeoGroup * newSubGeoGroup=new GUSubGeoGroup (thisAWDMatList, this->include_uv, useSecUV, this->include_normals, this->normal_threshold, this->joints_per_vertex, thisMat->get_is_faceted());
                        thisGeo->add_subGeom(newSubGeoGroup);
                    }
                }
                this->subGeoGroupCache->Set(newName, thisSubIdx, matIdx);
                free(newName);
            }
            else{
                // error - this should not happen
            }
            delete it;
            //free(spacer);
        }
        //if we do not want to merge any subgeometries (have subgeos for all mat-ids)
        else{
            int meshInstCnt=0;
            for (meshInstCnt=0; meshInstCnt<numGeos; meshInstCnt++){
                GUGeo * thisGeo = this->preGeoList->get_by_idx(meshInstCnt);
                thisSubIdx=thisGeo->get_subGeomList()->get_num_blocks();
                AWDBlockList * thisAWDMatList=new AWDBlockList();
                AWDMaterial * thisMat=(AWDMaterial *)materialList->getByIndex(meshInstCnt);
                thisAWDMatList->append(thisMat);
                bool useSecUV=false;
                if (this->include_suv){
                    if (thisMat->get_secondMappingChannel()>0){
                        useSecUV=true;
                    }
                }
                GUSubGeoGroup * newSubGeoGroup=new GUSubGeoGroup(thisAWDMatList, this->include_uv, useSecUV, this->include_normals, this->normal_threshold, this->joints_per_vertex, thisMat->get_is_faceted());
                thisGeo->add_subGeom(newSubGeoGroup);
            }
            this->subGeoGroupCache->Set("#", thisSubIdx, matIdx);
        }
    }
}
void
AWDGeomUtil::createGeometries()
{
    // if only one PreGeom exists, we can just output this as geom
    if (this->preGeoList->get_num_blocks()==1){
        this->geoList->force_append(this->preGeoList->get_by_idx(0));
    }
    // if multiple PreGeoms exists, we need to check if they can be merged (considering 'explode', 'uv-main', 'uv-second')
    else{
        int geoCnt=0;
        for (geoCnt=0;geoCnt<this->preGeoList->get_num_blocks();geoCnt++){
            // for the pre_geom, only one material per submesh should have been set
            // we collect a string made up by the relevant subMesh/material properties,
            // this string is used as lookup into a cache, as a way to merge 'mergable' geometry-instances
            GUGeo * thisPreGeo = this->preGeoList->get_by_idx(geoCnt);
            char *spacer = "#";
            int subeGeoCnt=0;
            int subeGeoMax=thisPreGeo->get_subGeomList()->get_num_blocks();
            int newNameLength=0;
            // for each subgeoGroup, get the first material (there should only be one material)
            for (subeGeoCnt=0;subeGeoCnt<subeGeoMax;subeGeoCnt++){
                GUSubGeoGroup * subGeoGroup=thisPreGeo->get_subGeomList()->get_by_idx(subeGeoCnt);
                AWDMaterial * matBlock= (AWDMaterial *) subGeoGroup->materials->getByIndex(0);
                bool explode=matBlock->get_is_faceted();
                char bufferEXPL [4];
                char * explodeOutStr= (char *)itoa (bool(explode),bufferEXPL, 4);
                int uvChannel=matBlock->get_mappingChannel();
                char bufferUV [4];
                char * uvMapOutStr= (char *)itoa (int(uvChannel),bufferUV, 4);
                int suvChannel=matBlock->get_secondMappingChannel();
                char bufferSUV [4];
                char * suvMapOutStr= (char *)itoa (int(suvChannel),bufferSUV, 4);
                newNameLength+=(int)(strlen(explodeOutStr)+strlen(spacer)+strlen(uvMapOutStr)+strlen(spacer)+strlen(suvMapOutStr)+strlen(spacer)+6);
            }
            newNameLength+=strlen(spacer)+1;
            char * newLookUpStr = (char*)malloc(newNameLength);
            strncpy_s(newLookUpStr, newNameLength, spacer, _TRUNCATE);
            for (subeGeoCnt=0;subeGeoCnt<subeGeoMax;subeGeoCnt++){
                GUSubGeoGroup * subGeoGroup=thisPreGeo->get_subGeomList()->get_by_idx(subeGeoCnt);
                AWDMaterial * matBlock= (AWDMaterial *)subGeoGroup->materials->getByIndex(0);
                bool explode=matBlock->get_is_faceted();
                char bufferEXPL [4];
                char * explodeOutStr= (char *)itoa (bool(explode), bufferEXPL, 4);
                int uvChannel=matBlock->get_mappingChannel();
                char bufferUV [4];
                char * uvMapOutStr= (char *)itoa (int(uvChannel), bufferUV, 4);
                int suvChannel=matBlock->get_secondMappingChannel();
                char bufferSUV [4];
                char * suvMapOutStr= (char *)itoa (int(suvChannel), bufferSUV, 4);
                strcat_s(newLookUpStr, newNameLength, explodeOutStr);
                strcat_s(newLookUpStr, newNameLength, spacer);
                strcat_s(newLookUpStr, newNameLength, uvMapOutStr);
                strcat_s(newLookUpStr, newNameLength, spacer);
                strcat_s(newLookUpStr, newNameLength, suvMapOutStr);
                strcat_s(newLookUpStr, newNameLength, spacer);
            }
            GUGeo * cachedGeo = (GUGeo *)this->geoCache->Get(newLookUpStr);
            if (cachedGeo!=NULL){
                cachedGeo->add_meshBlock((AWDMeshInst *)thisPreGeo->get_meshBlocks()->getByIndex(0));
                int numSubGeos=0;
                for (numSubGeos=0; numSubGeos< thisPreGeo->get_subGeomList()->get_num_blocks();numSubGeos++){
                    GUSubGeoGroup * thisSubgeoGroup=thisPreGeo->get_subGeomList()->get_by_idx(numSubGeos);
                    AWDMaterial * thisSubGeoMat=(AWDMaterial *)thisSubgeoGroup->materials->getByIndex(0);
                    GUSubGeoGroup * targetSubgeoGroup=cachedGeo->get_subGeomList()->get_by_idx(numSubGeos);
                    targetSubgeoGroup->materials->force_append(thisSubGeoMat);
                }
                thisPreGeo->set_meshBlocks(NULL);
            }
            else{
                this->geoCache->Set(newLookUpStr, thisPreGeo);
                this->geoList->force_append(thisPreGeo);
            }
            free(newLookUpStr);
        }
    }
}

int
AWDGeomUtil::getSubGeoIdxForMatIdx (int matIdx)
{
    return this->subGeoGroupCache->GetIdxByMatIdx(matIdx);
}

AWDBlockList *
AWDGeomUtil::build_geom(AWDTriGeom *md)
{
    AWDBlockList * returnAWDBlocks = new AWDBlockList();
    int geoCnt=0;
    for (geoCnt=0; geoCnt<this->geoList->get_num_blocks(); geoCnt++){
        GUGeo * thisGUGeom = this->geoList->get_by_idx(geoCnt);
        AWDTriGeom *thisAWDGeom;
        // if this is the first geom, we can use the original AWDTriblock that was allready created,
        // otherwise we need to create a new one (TODO: add a incremental to the created geoms...)
        if (geoCnt==0)
            thisAWDGeom = md;
        else
            thisAWDGeom = new AWDTriGeom(md->get_name(), md->get_name_length());
        thisAWDGeom->set_mesh_instance_list(thisGUGeom->get_meshBlocks());
        returnAWDBlocks->force_append(thisAWDGeom);
        //iterate over all SubGeoGroups, and build all the SubGeos. each subgeo contains a list of materials (one material per mesh-instance).
        GUSubGeoGroup *subGeogroup;
        GUSubGeoGroupIterator it(thisGUGeom->get_subGeomList());
        while ((subGeogroup = it.next()) != NULL){
            if (!subGeogroup->get_is_converted()){
                subGeogroup->set_is_converted(true);
                GUSubGeo *subGeo;
                GUSubGeoIterator itsub(subGeogroup->subGeosList);
                while ((subGeo = itsub.next()) != NULL){
                    if (subGeo->triCnt>0){
                        vdata *vd;
                        AWDSubGeom *sub;
                        //AWD_field_type tri_str_type;
                        int v_idx, i_idx;
                        AWD_str_ptr v_str;
                        AWD_str_ptr i_str;
                        AWD_str_ptr all_str;
                        AWD_str_ptr n_str;
                        AWD_str_ptr u_str;
                        AWD_str_ptr su_str;
                        AWD_str_ptr w_str;
                        AWD_str_ptr j_str;
                        AWD_str_ptr orig_idx_str;
                        bool exportCompact=false;
                        subGeo->joints_per_vertex=this->joints_per_vertex;
                        subGeo->normal_threshold=this->normal_threshold;
                        subGeo->include_uv=this->include_uv;
                        subGeo->include_suv=subGeogroup->include_suv;// all other are set global per mesh, but secUV is set per subMesh
                        subGeo->include_normals=this->include_normals;

                        int num_exp = subGeo->expanded->get_num_items();

                        i_str.ui32 = (awd_uint32*) malloc(sizeof(awd_uint32) * num_exp);
                        orig_idx_str.ui32 = (awd_uint32*) malloc(sizeof(awd_uint32) * num_exp);
                        if (!exportCompact){
                            v_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 3 * num_exp);
                            if (this->include_normals)
                                n_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 3 * num_exp);
                            if (subGeogroup->include_uv)
                                u_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 2 * num_exp);
                            if (subGeogroup->include_suv)
                                su_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 2 * num_exp);
                        }
                        else{
                            all_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * 13 * num_exp);
                        }
                        if (this->joints_per_vertex > 0) {
                            int max_num_vals = num_exp * this->joints_per_vertex;
                            w_str.f64 = (awd_float64*) malloc(sizeof(awd_float64) * max_num_vals);
                            j_str.ui32 = (awd_uint32*) malloc(sizeof(awd_uint32) * max_num_vals);

                            memset(w_str.v, 0, max_num_vals * sizeof(awd_float64));
                            memset(j_str.v, 0, max_num_vals * sizeof(awd_uint32));
                        }

                        // Prebuild a lookup list of vertices by their original index, so that
                        // has_vert() can check only those vertices that used to be the same
                        // client vertex, instead of looping over them all.
                        subGeo->prepare_build();

                        v_idx = i_idx = 0;

                        subGeo->expanded->iter_reset();
                        vd = subGeo->expanded->iter_next();

                        if (!exportCompact){
                            while (vd) {
                                int idx = subGeo->has_vert(vd);
                                if (idx >= 0) {
                                    i_str.ui32[i_idx++] = idx;
                                }
                                else {
                                    v_str.f64[v_idx*3+0] = vd->x;
                                    v_str.f64[v_idx*3+1] = vd->y;
                                    v_str.f64[v_idx*3+2] = vd->z;

                                    orig_idx_str.ui32[v_idx] = vd->orig_idx;

                                    if (this->include_normals) {
                                        n_str.f64[v_idx*3+0] = vd->nx;
                                        n_str.f64[v_idx*3+1] = vd->ny;
                                        n_str.f64[v_idx*3+2] = vd->nz;
                                    }

                                    if (subGeogroup->include_uv) {
                                        u_str.f64[v_idx*2+0] = vd->u;
                                        u_str.f64[v_idx*2+1] = vd->v;
                                    }
                                    if (subGeogroup->include_suv) {
                                        su_str.f64[v_idx*2+0] = vd->su;
                                        su_str.f64[v_idx*2+1] = vd->sv;
                                    }
                                    // If there are bindings, transfer them from
                                    // array in vdata struct to output streams.
                                    if (vd->num_bindings>0) {
                                        int w_idx;
                                        int jpv = vd->num_bindings;
                                        for (w_idx=0; w_idx<jpv; w_idx++) {
                                            w_str.f64[v_idx*jpv + w_idx] = vd->weights[w_idx];
                                            j_str.ui32[v_idx*jpv + w_idx] = vd->joints[w_idx];
                                        }
                                    }

                                    vd->out_idx = v_idx;
                                    i_str.ui32[i_idx++] = v_idx++;

                                    subGeo->collapsed->append_vdata(vd);
                                    subGeo->per_idx_lists[vd->orig_idx]->append_vdata(vd);
                                }

                                vd = subGeo->expanded->iter_next();
                            }

                            // Smoothing (averaging of normals) required?
                            if (this->normal_threshold > 0) {
                                subGeo->collapsed->iter_reset();
                                vd = subGeo->collapsed->iter_next();
                                while (vd) {
                                    int num_influences;
                                    double nx, ny, nz;
                                    ninfluence *inf;

                                    nx = ny = nz = 0.0;
                                    num_influences = 0;
                                    inf = vd->first_normal_influence;
                                    while (inf) {
                                        nx += inf->nx;
                                        ny += inf->ny;
                                        nz += inf->nz;
                                        inf = inf->next;
                                        num_influences++;
                                    }

                                    n_str.f64[vd->out_idx*3+0] = nx / num_influences;
                                    n_str.f64[vd->out_idx*3+1] = ny / num_influences;
                                    n_str.f64[vd->out_idx*3+2] = nz / num_influences;

                                    vd = subGeo->collapsed->iter_next();
                                }
                            }

                            // Reallocate the vertex buffer using final length after vertices were
                            // joined. There's no need to reallocate the index buffer since the
                            // triangle count will not have changed.
                            v_str.v = realloc(v_str.v, sizeof(awd_float64) * 3 * v_idx);
                            orig_idx_str.v = realloc(orig_idx_str.v, sizeof(awd_uint32) * v_idx);

                            // Choose stream type for the triangle stream depending on whether
                            // all vertex indices can be represented by an uint16 or not.
                            //tri_str_type = (v_idx > 0xffff)? AWD_FIELD_UINT32 : AWD_FIELD_UINT16;

                            sub = new AWDSubGeom(subGeogroup->materials);
                            sub->add_stream(VERTICES, this->geoStreamType, v_str, v_idx*3);
                            sub->add_stream(TRIANGLES, AWD_FIELD_UINT16, i_str, i_idx);

                            sub->add_original_idx_data(orig_idx_str, v_idx);

                            if (this->include_normals) {
                                // Reallocate buffer using actual length and add to sub-geom
                                n_str.v = realloc(n_str.v, sizeof(awd_float64) * 3 * v_idx);
                                sub->add_stream(VERTEX_NORMALS, this->geoStreamType, n_str, v_idx*3);
                            }

                            if (subGeogroup->include_uv) {
                                // Reallocate buffer using actual length and add to sub-geom
                                u_str.v = realloc(u_str.v, sizeof(awd_float64) * 2 * v_idx);
                                sub->add_stream(UVS, this->geoStreamType, u_str, v_idx*2);
                            }

                            if (subGeogroup->include_suv) {
                                // Reallocate buffer using actual length and add to sub-geom
                                su_str.v = realloc(su_str.v, sizeof(awd_float64) * 2 * v_idx);
                                sub->add_stream(SUVS, this->geoStreamType, su_str, v_idx*2);
                            }
                        }
                        else{
                            while (vd) {
                                int idx = subGeo->has_vert(vd);
                                if (idx >= 0) {
                                    i_str.ui32[i_idx++] = idx;
                                }
                                else {
                                    all_str.f64[v_idx*13+0] = vd->x;
                                    all_str.f64[v_idx*13+1] = vd->y;
                                    all_str.f64[v_idx*13+2] = vd->z;

                                    orig_idx_str.ui32[v_idx] = vd->orig_idx;

                                    if (this->include_normals) {
                                        all_str.f64[v_idx*13+3] = vd->nx;
                                        all_str.f64[v_idx*13+4] = vd->ny;
                                        all_str.f64[v_idx*13+5] = vd->nz;
                                    }
                                    else{
                                        all_str.f64[v_idx*13+3] = 0;
                                        all_str.f64[v_idx*13+4] = 0;
                                        all_str.f64[v_idx*13+5] = 0;
                                    }

                                    all_str.f64[v_idx*13+6] = 0;
                                    all_str.f64[v_idx*13+7] = 0;
                                    all_str.f64[v_idx*13+8] = 0;

                                    if (subGeogroup->include_uv) {
                                        all_str.f64[v_idx*13+9] = vd->u;
                                        all_str.f64[v_idx*13+10] = vd->v;
                                    }
                                    else{
                                        all_str.f64[v_idx*13+9] = 0;
                                        all_str.f64[v_idx*13+10] = 0;
                                    }
                                    if (subGeogroup->include_suv) {
                                        all_str.f64[v_idx*13+11] = vd->nx;
                                        all_str.f64[v_idx*13+12] = vd->ny;
                                    }
                                    else{
                                        all_str.f64[v_idx*13+11] = 0;
                                        all_str.f64[v_idx*13+12] = 0;
                                    }
                                    // If there are bindings, transfer them from
                                    // array in vdata struct to output streams.
                                    if (vd->num_bindings>0) {
                                        int w_idx;
                                        int jpv = vd->num_bindings;
                                        for (w_idx=0; w_idx<jpv; w_idx++) {
                                            w_str.f64[v_idx*jpv + w_idx] = vd->weights[w_idx];
                                            j_str.ui32[v_idx*jpv + w_idx] = vd->joints[w_idx];
                                        }
                                    }

                                    vd->out_idx = v_idx;
                                    i_str.ui32[i_idx++] = v_idx++;

                                    subGeo->collapsed->append_vdata(vd);
                                    subGeo->per_idx_lists[vd->orig_idx]->append_vdata(vd);
                                }
                                vd = subGeo->expanded->iter_next();
                            }

                            // Smoothing (averaging of normals) required?
                            if (this->normal_threshold > 0) {
                                subGeo->collapsed->iter_reset();
                                vd = subGeo->collapsed->iter_next();
                                while (vd) {
                                    int num_influences;
                                    double nx, ny, nz;
                                    ninfluence *inf;

                                    nx = ny = nz = 0.0;
                                    num_influences = 0;
                                    inf = vd->first_normal_influence;
                                    while (inf) {
                                        nx += inf->nx;
                                        ny += inf->ny;
                                        nz += inf->nz;
                                        inf = inf->next;
                                        num_influences++;
                                    }

                                    all_str.f64[vd->out_idx*13+3] = nx / num_influences;
                                    all_str.f64[vd->out_idx*13+4] = ny / num_influences;
                                    all_str.f64[vd->out_idx*13+5] = nz / num_influences;

                                    vd = subGeo->collapsed->iter_next();
                                }
                            }
                            all_str.v = realloc(all_str.v, sizeof(awd_float64) * 13 * v_idx);

                            orig_idx_str.v = realloc(orig_idx_str.v, sizeof(awd_uint32) * v_idx);

                            sub = new AWDSubGeom(subGeogroup->materials);
                            sub->add_stream(ALLVERTDATA, AWD_FIELD_FLOAT32, all_str, v_idx*13);
                            sub->add_stream(TRIANGLES, AWD_FIELD_UINT16, i_str, i_idx);

                            sub->add_original_idx_data(orig_idx_str, v_idx);
                        }
                        if (this->joints_per_vertex > 0) {
                            // Reallocate buffers using actual length and add to sub-geom
                            w_str.v = realloc(w_str.v, sizeof(awd_float64) * v_idx * this->joints_per_vertex);
                            j_str.v = realloc(j_str.v, sizeof(awd_uint32) * v_idx * this->joints_per_vertex);
                            sub->add_stream(VERTEX_WEIGHTS, this->geoStreamType, w_str, v_idx*this->joints_per_vertex);
                            sub->add_stream(JOINT_INDICES, AWD_FIELD_UINT16, j_str, v_idx*this->joints_per_vertex);
                        }
                        thisAWDGeom->add_sub_mesh(sub);
                    }
                }
            }
        }
    }
    return returnAWDBlocks;
}
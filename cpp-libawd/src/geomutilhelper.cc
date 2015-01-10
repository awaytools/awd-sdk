#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "geomutil.h"
#include "awd_types.h"

VertexDataList::VertexDataList(bool weakReference)
{
    this->num_items = 0;
    this->cur = NULL;
    this->first = NULL;
    this->last = NULL;
    this->weakReference = weakReference;
}

VertexDataList::~VertexDataList()
{
    vdata_list_item *cur_item = this->first;
    while (cur_item) {
        ninfluence *cur_n;
        vdata_list_item *next_item = cur_item->next;

        vdata *cur_v = cur_item->vd;
        if(!this->weakReference){
            if (cur_v!=NULL){
                // Free skinning data if any
                if (cur_v->num_bindings>0) {
                    if (cur_v->weights!=NULL)
                        free(cur_v->weights);
                    cur_v->weights=NULL;
                    if (cur_v->joints!=NULL)
                        free(cur_v->joints);
                    cur_v->joints=NULL;
                }

                cur_n = cur_v->first_normal_influence;
                while (cur_n) {
                    ninfluence *next_n = cur_n->next;
                    free(cur_n);
                    cur_n = next_n;
                }
                free(cur_v);
            }
        }
        free(cur_item);
        cur_item = next_item;
    }

    this->clear();
}

void
VertexDataList::append_vdata(vdata *vd)
{
    vdata_list_item *item = (vdata_list_item*)malloc(sizeof(vdata_list_item));
    item->vd = vd;

    if (!this->first) {
        this->first = item;
    }
    else {
        this->last->next = item;
    }

    this->last = item;
    this->last->next = NULL;
    this->num_items++;
}

void
VertexDataList::clear()
{
    this->num_items = 0;
    this->first = NULL;
    this->last = NULL;
}

int
VertexDataList::get_num_items()
{
    return this->num_items;
}

void
VertexDataList::iter_reset()
{
    this->cur = this->first;
}

vdata *
VertexDataList::iter_next()
{
    if (this->cur) {
        vdata *vd = this->cur->vd;
        this->cur = this->cur->next;
        return vd;
    }
    else {
        return NULL;
    }
}

SubGeoGroupCache::SubGeoGroupCache(void)
{
    firstItem = NULL;
    lastItem = NULL;
}

SubGeoGroupCache::~SubGeoGroupCache(void)
{
    SubGeoGroupCacheItem *cur = firstItem;
    while (cur) {
        SubGeoGroupCacheItem *next = cur->next;
        if (cur->keyLen>0)
            free(cur->key);
        cur->keyLen=0;
        free(cur);
        cur = next;
    }

    firstItem = lastItem = NULL;
}

void SubGeoGroupCache::Set(char *key, int val, int matIdx)
{
    SubGeoGroupCacheItem *item;

    item = (SubGeoGroupCacheItem *)malloc(sizeof(SubGeoGroupCacheItem));
    item->key = (char * )malloc(strlen(key)+1);
    strncpy_s(item->key, strlen(key)+1, key, _TRUNCATE);
    item->keyLen=awd_uint16(strlen(key)+1);
    item->val = val;
    item->matIdx = matIdx;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}

int SubGeoGroupCache::Get(char *key)
{
    SubGeoGroupCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (strcmp(cur->key,key) == 0)
            return cur->val;

        cur = cur->next;
    }

    return -1;
}
int SubGeoGroupCache::GetIdxByMatIdx(int matIdx)
{
    SubGeoGroupCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (cur->matIdx == matIdx)
            return cur->val;

        cur = cur->next;
    }

    return NULL;
}

GeomStringCache::GeomStringCache(void)
{
    firstItem = NULL;
    lastItem = NULL;
}

GeomStringCache::~GeomStringCache(void)
{
    GeomStringCacheItem *cur = firstItem;
    while (cur) {
        GeomStringCacheItem *next = cur->next;
        if (cur->keyLen>0)
            free(cur->key);
        cur->keyLen=0;
        free(cur);
        cur = next;
    }

    firstItem = lastItem = NULL;
}

void GeomStringCache::Set(char *key, void *val)
{
    GeomStringCacheItem *item;

    item = (GeomStringCacheItem *)malloc(sizeof(GeomStringCacheItem));
    item->key = (char * )malloc(strlen(key)+1);
    strncpy_s(item->key, strlen(key)+1, key, _TRUNCATE);
    item->keyLen=awd_uint16(strlen(key)+1);
    item->val = val;
    item->next = NULL;

    if (!firstItem) {
        firstItem = item;
    }
    else {
        lastItem->next = item;
    }

    lastItem = item;
}

void *GeomStringCache::Get(char *key)
{
    GeomStringCacheItem *cur;

    cur = firstItem;
    while (cur) {
        if (ATTREQ(cur->key,key))
            return cur->val;

        cur = cur->next;
    }

    return NULL;
}

static inline void
add_unique_influence(vdata *vd, double nx, double ny, double nz)
{
    if (!vd->first_normal_influence) {
        ninfluence *inf = (ninfluence *)malloc(sizeof(ninfluence));
        inf->nx = nx;
        inf->ny = ny;
        inf->nz = nz;
        inf->next = NULL;
        vd->first_normal_influence = inf;
        vd->last_normal_influence = inf;
    }
    else {
        ninfluence *cur;
        ninfluence *inf;

        cur = vd->first_normal_influence;
        while (cur) {
            if (cur->nx==nx && cur->ny==ny && cur->nz==nz) {
                return;}

            cur = cur->next;
        }

        // If reached, no duplicates exist
        inf = (ninfluence *)malloc(sizeof(ninfluence));
        inf->nx = nx;
        inf->ny = ny;
        inf->nz = nz;
        inf->next = NULL;
        vd->last_normal_influence->next = inf;
        vd->last_normal_influence = inf;
    }
}

GUSubGeo::GUSubGeo(AWDBlockList * materials, bool include_uv, bool include_suv, bool include_normals, double normal_threshold, int joints_per_vertex, bool explode)
{
    // TODO: Allow setting flags
    this->expanded = new VertexDataList(false);
    this->collapsed = new VertexDataList();
    this->per_idx_lists = NULL;
    this->num_idx_lists = 0;
    this->max_orig_idx = 0;
    this->vertCnt = 0;
    this->triCnt = 0;
    this->explode = explode;
    this->materials = NULL;
    this->updatematerials(materials);
    this->include_uv = include_uv;
    this->include_suv = include_suv;
    this->include_normals = include_normals;
    this->normal_threshold = normal_threshold;
    this->joints_per_vertex = joints_per_vertex;
}

GUSubGeo::~GUSubGeo()
{
    if(this->materials!=NULL)
        delete this->materials;// materials allready deleted on GUSubGeoList
    this->materials=NULL;
    this->include_uv = NULL;
    this->include_suv = NULL;
    this->include_normals = NULL;
    this->normal_threshold = NULL;
    this->joints_per_vertex = NULL;
    if (this->collapsed!=NULL)
        delete this->collapsed;
    if (this->expanded!=NULL)
        delete this->expanded;
    int i;
    for (i=0; i<num_idx_lists; i++) {
        // Empty list to not delete items, which are
        // duplicated in the expanded list.
        delete per_idx_lists[i];
    }
    free(per_idx_lists);
    this->expanded=NULL;
    this->per_idx_lists = NULL;
    this->num_idx_lists =  NULL;
    this->max_orig_idx =  NULL;
    this->vertCnt =  NULL;
    this->explode=NULL;
    this->triCnt =  NULL;
}

int
GUSubGeo::has_vert(vdata *vd)
{
    int idx;
    vdata *cur;
    VertexDataList *list;

    if (!this->explode){
        // The list that is used for look-up is a list that only contains
        // vertex data structs that originate from the same client vertex.
        // Vertices that weren't the same in the client app should never
        // be joined together, and this optimization stems from that fact.
        list = this->per_idx_lists[vd->orig_idx];
        idx = 0;
        list->iter_reset();
        cur = list->iter_next();
        while (cur) {
            // If any of vertices have force_hard set, their normals must
            // not be averaged. Hence, they must be returned by this
            // function as separate verts.
            if (cur->force_hard || vd->force_hard)
                goto next;

            // If position doesn't match, move on to next vertex
            if (cur->x != vd->x || cur->y != vd->y || cur->z != vd->z)
                goto next;

            // If UV coordinates do not match, move on to next vertex
            if (this->include_uv) {
                if (cur->u != vd->u || cur->v != vd->v)
                    goto next;
            }

            // If secondary UV coordinates do not match, move on to next vertex
            if (this->include_suv) {
                if (cur->su != vd->su || cur->sv != vd->sv)
                    goto next;
            }

            // Check if normals match (possibly with fuzzy matching using a
            // threshold) and if they don't, move on to the next vertex.
            if (this->include_normals) {
                if (this->normal_threshold > 0) {
                    if (vd->nx==cur->nx && vd->ny==cur->ny && vd->nz==cur->nz) {
                        // The exact same normals; avoid angle calculation
                        add_unique_influence(cur, vd->nx, vd->ny, vd->nz);
                    }
                    else {
                        double angle;
                        double l0, l1;

                        // Calculate lenghts (usually 1.0)
                        l0 = sqrt(cur->nx*cur->nx + cur->ny*cur->ny + cur->nz*cur->nz);
                        l1 = sqrt(vd->nx*vd->nx + vd->ny*vd->ny + vd->nz*vd->nz);

                        // Calculate angle and compare to threshold
                        angle = acos((cur->nx*vd->nx + cur->ny*vd->ny + cur->nz*vd->nz) / (l0*l1));
                        if (angle <= this->normal_threshold) {
                            add_unique_influence(cur, vd->nx, vd->ny, vd->nz);
                        }
                        else {
                            goto next;
                        }
                    }
                }
                else if (cur->nx != vd->nx || cur->ny != vd->ny || cur->nz != vd->nz)
                    goto next;
            }

            // Important: Don't put any more tests here after the normal test, which
            // needs to come last since it will add normal influences, which should
            // not be included unless the vertex matches.

            // Made it here? Then vertices match!
            return cur->out_idx;

next:
            idx++;
            cur = list->iter_next();
        }
    }

    // This is the first time that this vertex is encountered,
    // so it's own influence needs to be added.
    if (this->include_normals)
        add_unique_influence(vd, vd->nx, vd->ny, vd->nz);

    return -1;
}

void
GUSubGeo::updatematerials(AWDBlockList * newMaterials)
{
    if(this->materials!=NULL)
        delete this->materials;// materials allready deleted on GUSubGeoList
    AWDBlock * block;
    AWDBlockIterator *it = new AWDBlockIterator(newMaterials);
    this->materials=new AWDBlockList();
    while ((block = it->next()) != NULL) {
        this->materials->force_append(block);
    }
    delete it;
}
void
GUSubGeo::checkLimits()
{
}
void
GUSubGeo::prepare_build()
{
    int i;
    num_idx_lists = this->max_orig_idx + 1;
    this->per_idx_lists = (VertexDataList**)malloc(num_idx_lists * sizeof(VertexDataList *));
    for (i=0; i<num_idx_lists; i++) {
        this->per_idx_lists[i] = new VertexDataList();
    }
}

GUSubGeoList::GUSubGeoList()
{
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = 0;
}

GUSubGeoList::~GUSubGeoList()
{
    gusubgeo_list_item *cur;

    cur = this->first_block;
    while(cur) {
        gusubgeo_list_item *next = cur->next;
        cur->next = NULL;
        if (cur->subGeo!=NULL)
            delete cur->subGeo;
        cur->subGeo=NULL;
        if (cur!=NULL)
            delete cur;
        cur = next;
    }

    this->first_block = NULL;
    this->last_block = NULL;
}

bool
GUSubGeoList::append(GUSubGeo *subGeo)
{
    if (!this->contains(subGeo)) {
        gusubgeo_list_item *ctr = (gusubgeo_list_item *)malloc(sizeof(gusubgeo_list_item));
        ctr->subGeo = subGeo;
        if (this->first_block == NULL) {
            this->first_block = ctr;
        }
        else {
            this->last_block->next = ctr;
        }

        this->last_block = ctr;
        this->last_block->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}

void
GUSubGeoList::force_append(GUSubGeo *subGeo)
{
    gusubgeo_list_item *ctr = (gusubgeo_list_item *)malloc(sizeof(gusubgeo_list_item));
    ctr->subGeo = subGeo;
    if (this->first_block == NULL) {
        this->first_block = ctr;
    }
    else {
        this->last_block->next = ctr;
    }

    this->last_block = ctr;
    this->last_block->next = NULL;
    this->num_blocks++;
}

bool
GUSubGeoList::contains(GUSubGeo *subGeo)
{
    gusubgeo_list_item *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->subGeo == subGeo)
            return true;

        cur = cur->next;
    }

    return false;
}

int
GUSubGeoList::get_num_blocks()
{
    return this->num_blocks;
}

GUSubGeoIterator::GUSubGeoIterator(GUSubGeoList *list)
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}

void
GUSubGeoIterator::reset()
{
    if (this->list != NULL)
        this->cur_block = this->list->first_block;
}

GUSubGeo *
GUSubGeoIterator::next()
{
    gusubgeo_list_item *tmp;

    tmp = this->cur_block;
    if (this->cur_block != NULL)
        this->cur_block = this->cur_block->next;

    if (tmp)
        return tmp->subGeo;
    else return NULL;
}

GUSubGeoGroup::GUSubGeoGroup(AWDBlockList * materials, bool include_uv, bool include_suv, bool include_normals, double normal_threshold, int joints_per_vertex, bool explode)
{
    // TODO: Allow setting flags
    this->num_sub_geos = 0;
    this->materials = materials;
    this->include_uv = include_uv;
    this->include_suv = include_suv;
    this->include_normals = include_normals;
    this->normal_threshold = normal_threshold;
    this->joints_per_vertex = joints_per_vertex;
    this->is_converted=false;
    this->subGeosList = new GUSubGeoList();
    this->explode=explode;
    this->curSubGeo = new GUSubGeo(materials, include_uv, include_suv, include_normals, normal_threshold, joints_per_vertex, explode);
    this->subGeosList->append(this->curSubGeo);
}

GUSubGeoGroup::~GUSubGeoGroup()
{
    this->num_sub_geos = NULL;
    delete this->materials;// dont delete materials on GUSubGeo, because they are the same as this!
    this->include_uv = NULL;
    this->include_suv = NULL;
    this->include_normals = NULL;
    this->normal_threshold = NULL;
    this->joints_per_vertex = NULL;
    this->is_converted = NULL;
    delete this->subGeosList;
    this->curSubGeo = NULL;//allready deleted in list
}
void
GUSubGeoGroup::append_vdata(vdata * vdata)
{
    vdata->first_normal_influence = NULL;
    vdata->last_normal_influence = NULL;
    vdata->out_idx = 0;
    if ((int*)vdata->orig_idx > (int*)this->curSubGeo->max_orig_idx)
        this->curSubGeo->max_orig_idx = vdata->orig_idx;
    this->curSubGeo->expanded->append_vdata(vdata);
    this->curSubGeo->vertCnt++;
}

void
GUSubGeoGroup::check_limits()
{
    if ((this->curSubGeo->triCnt>=(15*0xffff))||((this->curSubGeo->vertCnt+3)>=(3*0xffff))){
        this->curSubGeo = new GUSubGeo(this->materials, this->include_uv, this->include_suv, this->include_normals, this->normal_threshold, this->joints_per_vertex, this->explode);
        this->subGeosList->force_append(this->curSubGeo);
    }
    this->curSubGeo->triCnt++;
}

bool
GUSubGeoGroup::get_is_converted()
{
    return this->is_converted;
}

void
GUSubGeoGroup::set_is_converted(bool is_converted)
{
    this->is_converted=is_converted;
}

GUSubGeoGroupList::GUSubGeoGroupList()
{
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = 0;
}

GUSubGeoGroupList::~GUSubGeoGroupList()
{
    gusubgeogroup_list_item *cur;

    cur = this->first_block;
    while(cur) {
        gusubgeogroup_list_item *next = cur->next;
        cur->next = NULL;
        if (cur->subGeoGroup!=NULL)
            delete cur->subGeoGroup;
        cur->subGeoGroup=NULL;
        delete cur;
        cur = next;
    }
    // Already deleted as part
    // of above loop
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = NULL;
}

bool
GUSubGeoGroupList::append(GUSubGeoGroup *subGeo)
{
    if (!this->contains(subGeo)) {
        gusubgeogroup_list_item *ctr = (gusubgeogroup_list_item *)malloc(sizeof(gusubgeogroup_list_item));
        ctr->subGeoGroup = subGeo;
        if (this->first_block == NULL) {
            this->first_block = ctr;
        }
        else {
            this->last_block->next = ctr;
        }

        this->last_block = ctr;
        this->last_block->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}

void
GUSubGeoGroupList::force_append(GUSubGeoGroup *subGeoGroup)
{
    gusubgeogroup_list_item *ctr = (gusubgeogroup_list_item *)malloc(sizeof(gusubgeogroup_list_item));
    ctr->subGeoGroup = subGeoGroup;
    if (this->first_block == NULL) {
        this->first_block = ctr;
    }
    else {
        this->last_block->next = ctr;
    }

    this->last_block = ctr;
    this->last_block->next = NULL;
    this->num_blocks++;
}

bool
GUSubGeoGroupList::contains(GUSubGeoGroup *subGeoGroup)
{
    gusubgeogroup_list_item *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->subGeoGroup == subGeoGroup)
            return true;

        cur = cur->next;
    }

    return false;
}

int
GUSubGeoGroupList::get_num_blocks()
{
    return this->num_blocks;
}

GUSubGeoGroup *
GUSubGeoGroupList::get_by_idx(int idx)
{
    if (idx>=this->num_blocks)
        return NULL;
    gusubgeogroup_list_item *cur;
    cur = this->first_block;
    int i;
    for (i=0;i<idx;i++){
        cur = cur->next;
    }
    return cur->subGeoGroup;
}

GUSubGeoGroupIterator::GUSubGeoGroupIterator(GUSubGeoGroupList *list)
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}

void
GUSubGeoGroupIterator::reset()
{
    if (this->list != NULL)
        this->cur_block = this->list->first_block;
}

GUSubGeoGroup *
GUSubGeoGroupIterator::next()
{
    gusubgeogroup_list_item *tmp;

    tmp = this->cur_block;
    if (this->cur_block != NULL)
        this->cur_block = this->cur_block->next;

    if (tmp)
        return tmp->subGeoGroup;
    else return NULL;
}

GUGeo::GUGeo(AWDMeshInst * meshBlock)
{
    this->subGeoGroupList = new GUSubGeoGroupList();
    this->meshBlocks = new AWDBlockList();
    this->meshBlocks->append(meshBlock);
}

GUGeo::~GUGeo()
{
    delete this->subGeoGroupList;
    if(this->meshBlocks!=NULL)
        delete this->meshBlocks;
    this->meshBlocks=NULL;
}

void
GUGeo::add_subGeom(GUSubGeoGroup *newSubGeoGrp)
{
    this->subGeoGroupList->force_append(newSubGeoGrp);
}

AWDBlockList *
GUGeo::get_meshBlocks()
{
    return this->meshBlocks;
}
void
GUGeo::add_meshBlock(AWDMeshInst * meshBlock)
{
    this->meshBlocks->force_append(meshBlock);
}
void
GUGeo::set_meshBlocks(AWDBlockList * meshBlocks)
{
    if(this->meshBlocks!=NULL)
        delete this->meshBlocks;
    this->meshBlocks=meshBlocks;
}
GUSubGeoGroupList *
GUGeo::get_subGeomList()
{
    return this->subGeoGroupList;
}

GUGeoList::GUGeoList(bool weakReference)
{
    this->first_geo = NULL;
    this->last_geo = NULL;
    this->num_blocks = 0;
    this->weakReference = weakReference;
}

GUGeoList::~GUGeoList()
{
    gugeo_list_item *cur;

    cur = this->first_geo;
    while(cur) {
        gugeo_list_item *next = cur->next;
        cur->next = NULL;
        if(!this->weakReference)
            delete cur->geo;
        delete cur;
        cur = next;
    }
    // Already deleted as part
    // of above loop
    this->first_geo = NULL;
    this->last_geo = NULL;
    this->num_blocks = NULL;
}

bool
GUGeoList::append(GUGeo *subGeo)
{
    if (!this->contains(subGeo)) {
        gugeo_list_item *ctr = (gugeo_list_item *)malloc(sizeof(gugeo_list_item));
        ctr->geo = subGeo;
        if (this->first_geo == NULL) {
            this->first_geo = ctr;
        }
        else {
            this->last_geo->next = ctr;
        }

        this->last_geo = ctr;
        this->last_geo->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}

void
GUGeoList::force_append(GUGeo *Geo)
{
    gugeo_list_item *ctr = (gugeo_list_item *)malloc(sizeof(gugeo_list_item));
    ctr->geo = Geo;
    if (this->first_geo == NULL) {
        this->first_geo = ctr;
    }
    else {
        this->last_geo->next = ctr;
    }

    this->last_geo = ctr;
    this->last_geo->next = NULL;
    this->num_blocks++;
}

bool
GUGeoList::contains(GUGeo *Geo)
{
    gugeo_list_item *cur;

    cur = this->first_geo;
    while (cur) {
        if (cur->geo == Geo)
            return true;

        cur = cur->next;
    }

    return false;
}

int
GUGeoList::get_num_blocks()
{
    return this->num_blocks;
}

GUGeo *
GUGeoList::get_by_idx(int idx)
{
    if (idx>=this->num_blocks)
        return NULL;
    gugeo_list_item *cur;
    cur = this->first_geo;
    int i;
    for (i=0;i<idx;i++){
        cur = cur->next;
    }
    return cur->geo;
}

GUGeoIterator::GUGeoIterator(GUGeoList *list)
{
    this->list = list;
    this->cur_geo = NULL;
    this->reset();
}

void
GUGeoIterator::reset()
{
    if (this->list != NULL)
        this->cur_geo = this->list->first_geo;
}

GUGeo *
GUGeoIterator::next()
{
    gugeo_list_item *tmp;

    tmp = this->cur_geo;
    if (this->cur_geo != NULL)
        this->cur_geo = this->cur_geo->next;

    if (tmp)
        return tmp->geo;
    else return NULL;
}
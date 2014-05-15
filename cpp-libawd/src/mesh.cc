#include <stdlib.h>
#include <cstdio>

#include "platform.h"

#include "mesh.h"
#include "util.h"

AWDSubGeom::AWDSubGeom(AWDBlockList * newMaterials) :
    AWDAttrElement()
{
    this->num_streams = 0;
    this->first_stream = NULL;
    this->last_stream = NULL;
    this->next = NULL;
    this->materials = newMaterials;
    this->originalIdx.v=NULL;
    this->originalIdx_num=0;
}

AWDSubGeom::~AWDSubGeom()
{
    AWDDataStream *cur;
    this->materials=NULL;
    cur = this->first_stream;
    while (cur) {
        AWDDataStream *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }
    if (this->originalIdx_num>0)
        free(this->originalIdx.v);
    this->originalIdx_num=NULL;
    this->first_stream = NULL;
    this->last_stream = NULL;
    this->first_stream = NULL;
}

unsigned int
AWDSubGeom::get_num_streams()
{
    return this->num_streams;
}

AWDBlockList*
AWDSubGeom::get_materials()
{
    return this->materials;
}
void
AWDSubGeom::set_materials(AWDBlockList * newMaterials)
{
    //if(this->materials!=NULL)
    //    delete this->materials;

    this->materials = newMaterials;
}

AWDDataStream *
AWDSubGeom::get_stream_at(unsigned int idx)
{
    if (idx < this->num_streams) {
        unsigned int cur_idx;
        AWDDataStream *cur;

        cur_idx = 0;
        cur = this->first_stream;
        while (cur) {
            if (cur_idx == idx)
                return cur;

            cur_idx++;
            cur = cur->next;
        }
    }

    return NULL;
}

void
AWDSubGeom::add_stream(AWD_mesh_str_type type, AWD_field_type data_type, AWD_str_ptr data, awd_uint32 num_elements)
{
    AWDDataStream *str;

    str = new AWDGeomDataStream((awd_uint8)type, data_type, data, num_elements);

    if (this->first_stream == NULL) {
        this->first_stream = str;
    }
    else {
        this->last_stream->next = str;
    }

    this->num_streams++;
    this->last_stream = str;
    this->last_stream->next = NULL;
}

void
AWDSubGeom::add_original_idx_data(AWD_str_ptr data, awd_uint32 num_elements)
{
    this->originalIdx = data;
    this->originalIdx_num = num_elements;
}
AWD_str_ptr
AWDSubGeom::get_original_idx_data()
{
    return this->originalIdx;
}
awd_uint32
AWDSubGeom::get_original_idx_data_len()
{
    return this->originalIdx_num;
}

awd_uint32
AWDSubGeom::calc_animations_streams_length()
{
    awd_uint32 len;
    AWDDataStream *str;

    len = 0;
    str = this->first_stream;
    while (str) {
        len += str->get_length();
        str = str->next;
    }

    return len;
}
awd_uint32
AWDSubGeom::calc_streams_length()
{
    awd_uint32 len;
    AWDDataStream *str;

    len = 0;
    str = this->first_stream;
    while (str) {
        len += 6 + str->get_length();
        str = str->next;
    }

    return len;
}

awd_uint32
AWDSubGeom::calc_sub_length(BlockSettings * blockSettings)
{
    awd_uint32 len;

    len = 4; // Sub-mesh header
    len += this->calc_streams_length();
    len += this->calc_attr_length(true,true, blockSettings);

    return len;
}

void
AWDSubGeom::write_anim_sub(int fd, bool wide_mtx, double scale)
{
    AWDDataStream *str;

    str = this->first_stream;
    while(str) {
        str->write_anim_stream(fd, scale);
        str = str->next;
    }
}

void
AWDSubGeom::write_sub(int fd, BlockSettings * blockSettings, double scale)
{
    AWDDataStream *str;
    awd_uint32 sub_len;

    // Verify byte-order
    sub_len = UI32(this->calc_streams_length());

    // Write sub-mesh header
    write(fd, &sub_len, sizeof(awd_uint32));

    this->properties->write_attributes(fd, blockSettings);

    str = this->first_stream;
    while(str) {
        str->write_stream(fd, scale);
        str = str->next;
    }

    this->user_attributes->write_attributes(fd, blockSettings);
}

AWDTriGeom::AWDTriGeom(const char *name, awd_uint16 name_len) :
    AWDBlock(TRI_GEOM),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->first_sub = NULL;
    this->last_sub = NULL;
    this->bind_mtx = NULL;
    this->num_subs = 0;
    this->is_created = false;
    this->split_faces = false;
    this->originalPointCnt = 0;
    this->meshInstanceList = new AWDBlockList();
}

AWDTriGeom::~AWDTriGeom()
{
    AWDSubGeom *cur;

    cur = this->first_sub;
    while (cur) {
        AWDSubGeom *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }

    if (this->bind_mtx) {
        free(this->bind_mtx);
        this->bind_mtx = NULL;
    }
    //if(this->meshInstanceList!=NULL)
    //    delete this->meshInstanceList;
    this->first_sub = NULL;
    this->last_sub = NULL;
}

AWDBlockList*
AWDTriGeom::get_mesh_instance_list()
{
    return this->meshInstanceList;
}
void
AWDTriGeom::set_mesh_instance_list(AWDBlockList *meshInstanceList)
{
    if(this->meshInstanceList!=NULL)
        delete this->meshInstanceList;
    this->meshInstanceList=meshInstanceList;
}

void
AWDTriGeom::add_sub_mesh(AWDSubGeom *sub)
{
    if (this->first_sub == NULL) {
        this->first_sub = sub;
    }
    else {
        this->last_sub->next = sub;
    }

    this->num_subs++;
    this->last_sub = sub;
}

unsigned int
AWDTriGeom::get_num_subs()
{
    return this->num_subs;
}

AWDSubGeom *
AWDTriGeom::get_sub_at(unsigned int idx)
{
    if (idx < this->num_subs) {
        unsigned int cur_idx;
        AWDSubGeom *cur;

        cur_idx = 0;
        cur = this->first_sub;
        while (cur) {
            if (cur_idx == idx)
                return cur;

            cur_idx++;
            cur = cur->next;
        }
    }

    return NULL;
}

awd_float64 *
AWDTriGeom::get_bind_mtx()
{
    return this->bind_mtx;
}

void
AWDTriGeom::set_bind_mtx(awd_float64 *bind_mtx)
{
    this->bind_mtx = bind_mtx;
}

bool
AWDTriGeom::get_is_created()
{
    return this->is_created;
}
void
AWDTriGeom::set_is_created(bool is_created)
{
    this->is_created=is_created;
}

bool
AWDTriGeom::get_split_faces()
{
    return this->split_faces;
}
void
AWDTriGeom::set_split_faces(bool split_faces)
{
    this->split_faces=split_faces;
}

int
AWDTriGeom::get_originalPointCnt()
{
    return this->originalPointCnt;
}
void
AWDTriGeom::set_originalPointCnt(int newPointCnt)
{
    this->originalPointCnt = newPointCnt;
}

AWDSubGeom *
AWDTriGeom::get_first_sub()
{
    return this->first_sub;
}

awd_uint32
AWDTriGeom::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    AWDSubGeom *sub;
    awd_uint32 mesh_len;

    // Calculate length of entire mesh
    // data (not block header)
    mesh_len = sizeof(awd_uint16); // Num subs
    mesh_len += sizeof(awd_uint16) + this->get_name_length();
    mesh_len += this->calc_attr_length(true,true,  curBlockSettings);
    sub = this->first_sub;
    while (sub) {
        mesh_len += sub->calc_sub_length( curBlockSettings);
        sub = sub->next;
    }

    return mesh_len;
}

void
AWDTriGeom::write_body(int fd, BlockSettings *curBlockSettings)
{
    awd_uint16 num_subs_be;
    AWDSubGeom *sub;

    // Write name and sub count
    num_subs_be = UI16(this->num_subs);
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &num_subs_be, sizeof(awd_uint16));

    // Write list of optional properties
    this->properties->write_attributes(fd, curBlockSettings);

    // Write all sub-meshes
    sub = this->first_sub;
    while (sub) {
        sub->write_sub(fd, curBlockSettings, curBlockSettings->get_scale());
        sub = sub->next;
    }

    // Write list of user attributes
    this->user_attributes->write_attributes(fd, curBlockSettings);
}

AWDMeshInst::AWDMeshInst(const char *name, awd_uint16 name_len, AWDBlock *geom) :
    AWDSceneBlock(MESH_INSTANCE, name, name_len, NULL)
{
    this->set_geom(geom);
    this->materials = new AWDBlockList();
    this->pre_materials = new AWDBlockList();
    this->defaultMat = NULL;
}

AWDMeshInst::AWDMeshInst(const char *name, awd_uint16 name_len, AWDBlock *geom, awd_float64 *mtx) :
    AWDSceneBlock(MESH_INSTANCE, name, name_len, mtx)
{
    this->set_geom(geom);
    this->materials = new AWDBlockList();
    this->pre_materials = new AWDBlockList();
    this->lightPicker = NULL;
    this->defaultMat = NULL;
    this->animator = NULL;
}

AWDMeshInst::~AWDMeshInst()
{
    delete this->materials;
    if(this->pre_materials!=NULL)
        delete this->pre_materials;
    this->lightPicker = NULL;
    this->defaultMat = NULL;
    this->geom = NULL;
}

AWDBlock *
AWDMeshInst::get_defaultMat()
{
    return this->defaultMat;
}
void
AWDMeshInst::set_defaultMat(AWDBlock *defaultMat)
{
    this->defaultMat=defaultMat;
}
void
AWDMeshInst::add_material(AWDMaterial *material)
{
    this->materials->force_append(material);
}
AWDBlockList *
AWDMeshInst::get_pre_materials()
{
    return this->pre_materials;
}
void
AWDMeshInst::set_pre_materials(AWDBlockList *materials)
{
    if(this->pre_materials!=NULL)
        delete this->pre_materials;
    this->pre_materials=materials;
}
AWDBlock *
AWDMeshInst::get_lightPicker()
{
    return this->lightPicker;
}
void
AWDMeshInst::set_lightPicker(AWDBlock *lightPicker)
{
    this->lightPicker=lightPicker;
}

AWDBlock *
AWDMeshInst::get_geom()
{
    return this->geom;
}

void
AWDMeshInst::set_geom(AWDBlock *geom)
{
    this->geom = geom;
}

AWDBlock *
AWDMeshInst::get_animator()
{
    return this->animator;
}
void
AWDMeshInst::set_animator(AWDBlock *animator)
{
    this->animator = animator;
}
awd_uint32
AWDMeshInst::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    return calc_common_length(curBlockSettings->get_wide_matrix()) + sizeof(awd_baddr) +
        sizeof(awd_uint16) + (this->materials->get_num_blocks() * sizeof(awd_baddr)) +
        this->calc_attr_length(true, true, curBlockSettings);
}

void
AWDMeshInst::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->geom != NULL)
        this->geom->prepare_and_add_with_dependencies(export_list);
    AWDMaterial *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->materials);
    // check if all materials(submehses) are using same uv-transform
    awd_float64 * uv_trans = NULL;
    bool hasSubMeshUVTransform = false;
    while ((block = (AWDMaterial *)it->next()) != NULL) {
        if (uv_trans==NULL){
            if (block->get_uv_transform_mtx()!=NULL){
                uv_trans=block->get_uv_transform_mtx();
            }
        }
        else {
            if(!hasSubMeshUVTransform){
                if (block->get_uv_transform_mtx()!=NULL){
                    awd_float64 * this_uv=block->get_uv_transform_mtx();
                    if ((this_uv[0]!=uv_trans[0])||(this_uv[1]!=uv_trans[1])||(this_uv[2]!=uv_trans[2])||(this_uv[3]!=uv_trans[3])||(this_uv[4]!=uv_trans[4])||(this_uv[5]!=uv_trans[5])){
                        hasSubMeshUVTransform=true;
                    }
                }
            }
        }
        block->prepare_and_add_with_dependencies(export_list);
    }
    // if all materials(submehses)use same uv)
    if (uv_trans!=NULL){
        if (!hasSubMeshUVTransform){
            if ((uv_trans[0]!=1.0)||(uv_trans[1]!=0.0)||(uv_trans[2]!=1.0)||(uv_trans[3]!=0.0)||(uv_trans[4]!=0.0)||(uv_trans[5]!=0.0)){
                AWD_field_ptr newVal;
                newVal.v = malloc(sizeof(awd_float64)*6);
                newVal.f64[0] = uv_trans[0];
                newVal.f64[1] = uv_trans[1];
                newVal.f64[2] = uv_trans[2];
                newVal.f64[3] = uv_trans[3];
                newVal.f64[4] = uv_trans[4];
                newVal.f64[5] = uv_trans[5];
                this->properties->set(6, newVal, sizeof(awd_float64)*6, AWD_FIELD_FLOAT64);
            }
        }
        else{
            AWD_field_ptr newVal;
            newVal.v = malloc(sizeof(awd_float64)*6*this->materials->get_num_blocks());
            it->reset();
            int uvTransCnt=0;
            while ((block = (AWDMaterial *)it->next()) != NULL) {
                awd_float64 * this_uv=block->get_uv_transform_mtx();
                if (this_uv!=NULL){
                    newVal.f64[uvTransCnt++] = uv_trans[0];
                    newVal.f64[uvTransCnt++] = uv_trans[1];
                    newVal.f64[uvTransCnt++] = uv_trans[2];
                    newVal.f64[uvTransCnt++] = uv_trans[3];
                    newVal.f64[uvTransCnt++] = uv_trans[4];
                    newVal.f64[uvTransCnt++] = uv_trans[5];
                }
                else {
                    newVal.f64[uvTransCnt++] = 1.0;
                    newVal.f64[uvTransCnt++] = 0.0;
                    newVal.f64[uvTransCnt++] = 0.0;
                    newVal.f64[uvTransCnt++] = 1.0;
                    newVal.f64[uvTransCnt++] = 0.0;
                    newVal.f64[uvTransCnt++] = 0.0;
                }
            }
            this->properties->set(7, newVal, sizeof(awd_float64)*6*this->materials->get_num_blocks(), AWD_FIELD_FLOAT64);
        }
    }
    delete it;
}

void
AWDMeshInst::write_body(int fd, BlockSettings *curBlockSettings)
{
    AWDBlock *block;
    AWDBlockIterator *it;
    awd_baddr geom_addr;
    awd_uint16 num_materials;

    this->write_scene_common(fd, curBlockSettings);

    // Write mesh geom address (can be NULL)
    geom_addr = 0;
    if (this->geom != NULL)
        geom_addr = UI32(this->geom->get_addr());
    write(fd, &geom_addr, sizeof(awd_uint32));

    // Write materials list. First write material count, and then
    // iterate over materials block list and write all addresses
    printf("material count: %d\n", this->materials->get_num_blocks());
    num_materials = UI16((awd_uint16)this->materials->get_num_blocks());
    write(fd, &num_materials, sizeof(awd_uint16));
    it = new AWDBlockIterator(this->materials);
    while ((block = it->next()) != NULL) {
        awd_baddr addr = UI32(block->get_addr());
        write(fd, &addr, sizeof(awd_baddr));
    }
    delete it;
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
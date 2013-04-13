#include <stdlib.h>
#include <cstdio>

#include "platform.h"

#include "mesh.h"
#include "util.h"




AWDSubGeom::AWDSubGeom() :
    AWDAttrElement()
{
    this->num_streams = 0;
    this->first_stream = NULL;
    this->last_stream = NULL;
    this->next = NULL;
}

AWDSubGeom::~AWDSubGeom()
{
    AWDDataStream *cur;

    cur = this->first_stream;
    while (cur) {
        AWDDataStream *next = cur->next;
        cur->next = NULL;
        delete cur;
        cur = next;
    }

    this->first_stream = NULL;
    this->last_stream = NULL;
}


unsigned int
AWDSubGeom::get_num_streams()
{
    return this->num_streams;
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
AWDSubGeom::calc_sub_length(bool wide_mtx)
{
    awd_uint32 len;

    len = 4; // Sub-mesh header
    len += this->calc_streams_length();
    len += this->calc_attr_length(true,true, wide_mtx);

    return len;
}


void
AWDSubGeom::write_sub(int fd, bool wide_mtx)
{
    AWDDataStream *str;
    awd_uint32 sub_len;

    // Verify byte-order
    sub_len = UI32(this->calc_streams_length());

    // Write sub-mesh header
    write(fd, &sub_len, sizeof(awd_uint32));

    this->properties->write_attributes(fd, wide_mtx);

    str = this->first_stream;
    while(str) {
        str->write_stream(fd);
        str = str->next;
    }

    this->user_attributes->write_attributes(fd, wide_mtx);
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

    this->first_sub = NULL;
    this->last_sub = NULL;
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


awd_uint32
AWDTriGeom::calc_body_length(bool wide_mtx)
{
    AWDSubGeom *sub;
    awd_uint32 mesh_len;

    // Calculate length of entire mesh 
    // data (not block header)
    mesh_len = sizeof(awd_uint16); // Num subs
    mesh_len += sizeof(awd_uint16) + this->get_name_length();
    mesh_len += this->calc_attr_length(true,true, wide_mtx);
    sub = this->first_sub;
    while (sub) {
        mesh_len += sub->calc_sub_length(wide_mtx);
        sub = sub->next;
    }

    return mesh_len;
}


void
AWDTriGeom::write_body(int fd, bool wide_mtx)
{
    awd_uint16 num_subs_be;
    AWDSubGeom *sub;

    // Write name and sub count
    num_subs_be = UI16(this->num_subs);
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length()); 
    write(fd, &num_subs_be, sizeof(awd_uint16));

    // Write list of optional properties
    this->properties->write_attributes(fd, wide_mtx);

    // Write all sub-meshes
    sub = this->first_sub;
    while (sub) {
        sub->write_sub(fd, wide_mtx);
        sub = sub->next;
    }
    
    // Write list of user attributes
    this->user_attributes->write_attributes(fd, wide_mtx);
}





AWDMeshInst::AWDMeshInst(const char *name, awd_uint16 name_len, AWDTriGeom *geom) :
    AWDSceneBlock(MESH_INSTANCE, name, name_len, NULL)
{
    this->set_geom(geom);
    this->materials = new AWDBlockList();
}


AWDMeshInst::AWDMeshInst(const char *name, awd_uint16 name_len, AWDTriGeom *geom, awd_float64 *mtx) :
    AWDSceneBlock(MESH_INSTANCE, name, name_len, mtx)
{
    this->set_geom(geom);
    this->materials = new AWDBlockList();
}


AWDMeshInst::~AWDMeshInst()
{
}


void
AWDMeshInst::add_material(AWDMaterial *material)
{
    this->materials->force_append(material);
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


awd_uint32
AWDMeshInst::calc_body_length(bool wide_mtx)
{
	return calc_common_length(wide_mtx) + sizeof(awd_baddr) + 
		sizeof(awd_uint16) + (this->materials->get_num_blocks() * sizeof(awd_baddr)) + 
		this->calc_attr_length(true, true, wide_mtx);
}



void
AWDMeshInst::write_body(int fd, bool wide_mtx)
{
    AWDBlock *block;
    AWDBlockIterator *it;
    awd_baddr geom_addr;
    awd_uint16 num_materials;

    this->write_scene_common(fd, wide_mtx);

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

    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}

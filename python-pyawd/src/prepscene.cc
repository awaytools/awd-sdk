/**
 * This file contains reparation functions for all blocks belonging in the pyawd.scene 
 * module. 
 * 
 * These are invoked by AWDWriter to convert a python object to it's C++/libawd
 * equivalence and add it to the AWD document.
*/


#include "AWDWriter.h"
#include "bcache.h"
#include "util.h"

static AWDSceneBlock *
__prepare_mesh_inst(PyObject *block, pyawd_bcache *bcache)
{
    int i;
    int num_materials;

    PyObject *md_attr;
    PyObject *mat_attr;

    AWDMeshInst *lawd_inst;
    AWDTriGeom *lawd_md;
    
    lawd_md = NULL;
    md_attr = PyObject_GetAttrString(block, "geometry");
    if (md_attr != Py_None) {
        lawd_md = (AWDTriGeom *)pyawd_bcache_get(bcache, md_attr);
    }
    
    lawd_inst = new AWDMeshInst(NULL, 0, lawd_md);

    mat_attr = PyObject_GetAttrString(block, "materials");
    num_materials = PyList_Size(mat_attr);
    for (i=0; i<num_materials; i++) {
        AWDMaterial *lawd_mat;

        lawd_mat = (AWDMaterial *)pyawd_bcache_get(bcache, PyList_GetItem(mat_attr, i));
        lawd_inst->add_material(lawd_mat);
    }

    return lawd_inst;
}


static AWDSceneBlock *
__prepare_container(PyObject *block)
{
    AWDContainer *lawd_ctr;
    lawd_ctr = new AWDContainer(NULL, 0);

    return lawd_ctr;
}


static AWDSceneBlock *
__prepare_scene(PyObject *block)
{
    AWDScene *lawd_scene;
    lawd_scene = new AWDScene(NULL, 0);

    return lawd_scene;
}


static AWDSceneBlock *
__prepare_camera(PyObject *block)
{
    PyObject *type_attr;
    PyObject *lens_attr;
    PyObject *fov_attr;

    awd_float64 fov;
    AWD_cam_type type;
    AWD_lens_type lens;
    AWDCamera *lawd_cam;

    type_attr = PyObject_GetAttrString(block, "type");
    type = (AWD_cam_type)PyLong_AsLong(type_attr);

    lens_attr = PyObject_GetAttrString(block, "lens");
    lens = (AWD_lens_type)PyLong_AsLong(lens_attr);

    fov_attr = PyObject_GetAttrString(block, "fov");
    fov = PyFloat_AsDouble(fov_attr);

    lawd_cam = new AWDCamera(NULL, 0, type, lens);
    lawd_cam->set_lens_fov(fov);
    
    return lawd_cam;
}


static AWDSceneBlock *
__prepare_light(PyObject *block)
{
    PyObject *type_attr;
    AWD_light_type type;
    AWDLight *lawd_light;

    type_attr = PyObject_GetAttrString(block, "type");
    type = (AWD_light_type)PyLong_AsLong(type_attr);

    lawd_light = new AWDLight(NULL, 0, type);

    return lawd_light;
}


void
__prepare_scene_block(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    const char *name;
    int name_len;
    awd_float64 *mtx;
    AWDSceneBlock *parent;

    PyObject *mtx_attr;
    PyObject *par_attr;
    PyObject *children_attr;

    AWDSceneBlock *scene_block;
    const char *type = block->ob_type->tp_name;

    if (strcmp(type, "AWDMeshInst")==0) {
        scene_block = __prepare_mesh_inst(block, bcache);
    }
    else if (strcmp(type, "AWDContainer")==0) {
        scene_block = __prepare_container(block);
    }
    else if (strcmp(type, "AWDScene")==0) {
        scene_block = __prepare_scene(block);
    }
    else if (strcmp(type, "AWDCamera")==0) {
        scene_block = __prepare_camera(block);
    }
    else if (strcmp(type, "AWDLight")==0) {
        scene_block = __prepare_light(block);
    }
    else {
		// Unknown type
		return;
    }

    mtx = NULL;
    mtx_attr = PyObject_GetAttrString(block, "transform");
    if (mtx_attr != Py_None) {
        PyObject *mtx_list;
        mtx_list = PyObject_GetAttrString(mtx_attr, "raw_data");
        mtx = pyawdutil_pylist_to_float64(mtx_list, NULL, 12);
    }

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    scene_block->set_name(name, name_len);
    scene_block->set_transform(mtx);

    // Prep any user attributes
    __prepare_attr_element(block, awd, scene_block);

    awd->add_scene_block(scene_block);
    pyawd_bcache_add(bcache, block, scene_block);


    // Add to parent if parent existed
    par_attr = PyObject_GetAttrString(block, "_AWDSceneBlockBase__parent");
    parent = (AWDSceneBlock *)pyawd_bcache_get(bcache, par_attr);
    if (parent != NULL) {
        parent->add_child(scene_block);
    }

    // Prepare all children
    children_attr = PyObject_GetAttrString(block, "_AWDSceneBlockBase__children");
    if (children_attr) {
        int i;
        int num_children;

        num_children = PyList_Size(children_attr);
        for (i=0; i<num_children; i++) {
            PyObject *child = PyList_GetItem(children_attr, i);
            __prepare_scene_block(child, awd, bcache);
        }
    }
}

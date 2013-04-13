#include <stdio.h>

#include <Python.h>
#include "structmember.h"

#include <awd/libawd.h>

#include "util.h"
#include "AWDWriter.h"



/**
 * free()
*/
void
cpyawd_AWDWriter_dealloc(cpyawd_AWDWriter *self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}


/**
 * AWDWriter();
*/
PyObject *
cpyawd_AWDWriter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    cpyawd_AWDWriter *self;

    self = (cpyawd_AWDWriter *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}


/**
 * AWDWriter.__init__();
*/
static int
cpyawd_AWDWriter_init(cpyawd_AWDWriter *self, PyObject *args, PyObject *kwds)
{
    return 0;
}



static void
__prepare_metadata(PyObject *pyawd_AWD, AWD *lawd_awd)
{
    PyObject *meta;

    meta = PyObject_GetAttrString(pyawd_AWD, "metadata");
    if (meta && meta != Py_None) {
        char *enc_name;
        char *enc_ver;
        AWDMetaData *lawd_meta;

        lawd_meta = new AWDMetaData();

        pyawdutil_get_strattr(meta, "generator", (const char**)&lawd_meta->generator_name, NULL);
        pyawdutil_get_strattr(meta, "generator_version", (const char**)&lawd_meta->generator_version, NULL);
        pyawdutil_get_strattr(meta, "encoder", (const char**)&enc_name, NULL);
        pyawdutil_get_strattr(meta, "encoder_version", (const char **)&enc_ver, NULL);

        lawd_meta->override_encoder_metadata(enc_name, enc_ver);
        lawd_awd->set_metadata(lawd_meta);
    }
}



/**
 * AWDWriter.write(awd, file)
*/
PyObject *
cpyawd_AWDWriter_write(cpyawd_AWDWriter *self, PyObject *args)
{
    AWD *lawd_awd;
    PyObject *awd_obj;
    PyObject *fobj;
    PyObject *flags_attr;
    PyObject *compression_attr;
    awd_uint16 flags;
    AWD_compression compression;
    int fd;

    printf("Writing using libawd.\n");

#if PYTHON_VERSION == 3
    extern PyTypeObject PyIOBase_Type;
    if (!PyArg_ParseTuple(args, "OO!", &awd_obj, &PyIOBase_Type, &fobj))
        return NULL;
#else
    extern PyTypeObject PyFile_Type;
    if (!PyArg_ParseTuple(args, "OO!", &awd_obj, &PyFile_Type, &fobj))
        return NULL;
#endif

    fd = PyObject_AsFileDescriptor(fobj);

    compression = UNCOMPRESSED; // Default
    compression_attr = PyObject_GetAttrString(awd_obj, "compression");
    if (compression_attr!=NULL) {
        compression = (AWD_compression)PyLong_AsLong(compression_attr);
    }

    flags = 0; // Default
    flags_attr = PyObject_GetAttrString(awd_obj, "flags");
    if (flags_attr!=NULL) {
        flags = (awd_uint16)PyLong_AsLong(flags_attr);
    }

    // Create AWD document
    lawd_awd = new AWD(compression,flags);

    if (fd >= 0) {
        pyawd_bcache *bcache;

        __prepare_metadata(awd_obj, lawd_awd);

        bcache = (pyawd_bcache *)malloc(sizeof(pyawd_bcache));
        pyawd_bcache_init(bcache);

        // Create libawd structures from all python blocks
        __prepare_blocks(awd_obj, "uvanim_blocks", lawd_awd, bcache, __prepare_uvanim);
        __prepare_blocks(awd_obj, "texture_blocks", lawd_awd, bcache, __prepare_texture);
        __prepare_blocks(awd_obj, "material_blocks", lawd_awd, bcache, __prepare_material);
        __prepare_blocks(awd_obj, "skeleton_blocks", lawd_awd, bcache, __prepare_skeleton);
        __prepare_blocks(awd_obj, "skelpose_blocks", lawd_awd, bcache, __prepare_skelpose);
        __prepare_blocks(awd_obj, "skelanim_blocks", lawd_awd, bcache, __prepare_skelanim);
        __prepare_blocks(awd_obj, "tri_geom_blocks", lawd_awd, bcache, __prepare_mesh_data);
        __prepare_blocks(awd_obj, "scene_blocks", lawd_awd, bcache, __prepare_scene_block);

        // Write buffer
        lawd_awd->flush(fd);
        Py_RETURN_NONE;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Unable to open file descriptor from Python file object");
        return NULL;
    }
}





/**
 * Method dictionary
*/
PyMethodDef cpyawd_AWDWriter_methods[] = {
    { "write", (PyCFunction)cpyawd_AWDWriter_write, METH_VARARGS,
        "Write everything in an AWD object to an output stream." },

    { NULL }
};




/**
 * Type object 
*/
PyTypeObject cpyawd_AWDWriterType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pyawd.cpyawd.AWDWriter",               // tp_name
    sizeof(cpyawd_AWDWriter),               // tp_basicsize
    0,                                      // tp_itemsize
    (destructor)cpyawd_AWDWriter_dealloc,   // tp_dealloc
    0,                                      // tp_print
    0,                                      // tp_getattr
    0,                                      // tp_setattr
    0,                                      // tp_reserved
    0,                                      // tp_repr
    0,                                      // tp_as_number
    0,                                      // tp_as_sequence
    0,                                      // tp_as_mapping
    0,                                      // tp_hash 
    0,                                      // tp_call
    0,                                      // tp_str
    0,                                      // tp_getattro
    0,                                      // tp_setattro
    0,                                      // tp_as_buffer
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                // tp_flags
    "AWD document object.",                 // tp_doc
    0,                                      // tp_traverse
    0,                                      // tp_clear
    0,                                      // tp_richcompare
    0,                                      // tp_weaklistoffset
    0,                                      // tp_iter
    0,                                      // tp_iternext
    cpyawd_AWDWriter_methods,               // tp_methods
    0,                                      // tp_members
    0,                                      // tp_getset
    0,                                      // tp_base
    0,                                      // tp_dict
    0,                                      // tp_descr_get
    0,                                      // tp_descr_set
    0,                                      // tp_dictoffset
    (initproc)cpyawd_AWDWriter_init,        // tp_init
    0,                                      // tp_alloc
    cpyawd_AWDWriter_new,                   // tp_new
};



void
__prepare_blocks(PyObject *pyawd_AWD, const char *list_attr, AWD *awd, pyawd_bcache *bcache, void (*prep_func)(PyObject*, AWD*, pyawd_bcache *))
{
    PyObject *list;

    list = PyObject_GetAttrString(pyawd_AWD, list_attr);
    if (list && PyObject_IsInstance(list, (PyObject *)&PyList_Type)) {
        int i;
        int len;

        len = PyList_Size(list);
        for (i=0; i<len; i++) {
            prep_func(PyList_GetItem(list, i), awd, bcache);
        }
    }
}


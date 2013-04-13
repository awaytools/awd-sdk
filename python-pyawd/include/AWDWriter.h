#ifndef _PYAWD_AWDWRITER_H
#define _PYAWD_AWDWRITER_H

#include <Python.h>

#include <awd/libawd.h>

#include "bcache.h"


extern PyTypeObject cpyawd_AWDWriterType;

typedef struct {
    PyObject_HEAD
} cpyawd_AWDWriter;


void __prepare_blocks(PyObject *, const char *, AWD *, pyawd_bcache *, void (*)(PyObject*,AWD*,pyawd_bcache*));
void __prepare_mesh_data(PyObject *, AWD *, pyawd_bcache *);
void __prepare_texture(PyObject *, AWD *, pyawd_bcache *);
void __prepare_material(PyObject *, AWD *, pyawd_bcache *);
void __prepare_skeleton(PyObject *, AWD *, pyawd_bcache *);
void __prepare_skelanim(PyObject *, AWD *, pyawd_bcache *);
void __prepare_skelpose(PyObject *, AWD *, pyawd_bcache *);
void __prepare_uvanim(PyObject *, AWD *, pyawd_bcache *);
void __prepare_scene_block(PyObject *, AWD *, pyawd_bcache *);

void __prepare_attr_element(PyObject *, AWD *, AWDAttrElement *);

#endif

#include <awd/mesh.h>
#include <awd/geomutil.h>
#include <cstdio>

#include "utils.h"
#include "util.h"

PyObject *
cpyawd_util_build_geom(PyObject *self, PyObject *args, PyObject *kwds)
{
    int i, len;
    int ret;
    double normal_threshold;
    PyObject *verts_list;
    PyObject *py_md;
    AWDTriGeom *lawd_md;
    AWDGeomUtil *lawd_util;
    AWDSubGeom *lawd_sub;
    PyObject *geom_mod;
    PyObject *sub_class;

    static const char *kwlist[] = {"vertices", "mesh_data", "normal_threshold", NULL};

    normal_threshold = 0.0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!O|d", (char**)kwlist, 
                &PyList_Type, &verts_list, &py_md, &normal_threshold))
        return NULL;

    lawd_util = new AWDGeomUtil();
    lawd_util->normal_threshold = normal_threshold;

    len = PyList_Size(verts_list);
    for (i=0; i<len; i++) {
        PyObject *vert_obj = PyList_GetItem(verts_list, i);
        
        if (PyTuple_Check(vert_obj)) {
            unsigned int idx;
            double x, y, z, u, v, nx, ny, nz;
            bool force_hard;

            idx = PyLong_AsLong(PyTuple_GetItem(vert_obj, 0));
            x = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 1));
            y = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 2));
            z = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 3));
            u = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 4));
            v = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 5));
            nx = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 6));
            ny = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 7));
            nz = PyFloat_AsDouble(PyTuple_GetItem(vert_obj, 8));
            force_hard = PyLong_AsLong(PyTuple_GetItem(vert_obj, 9));

            lawd_util->append_vert_data(idx, x, y, z, u, v, nx, ny, nz, force_hard);
        }
    }

    lawd_md = new AWDTriGeom("dummy", 5);
    ret = lawd_util->build_geom(lawd_md);

    // Import pyawd.geom module and get the AWDSubMesh classobj, which
    // will be used to instantiate new sub-meshes
    geom_mod = PyImport_ImportModule("pyawd.geom");
    sub_class = PyObject_GetAttrString(geom_mod, "AWDSubMesh");

    len = lawd_md->get_num_subs();
    for (i=0; i<len; i++) {
        int stream_idx;
        int num_streams;

        PyObject *py_sub;

        py_sub = PyObject_CallObject(sub_class, NULL);
        if (!py_sub)
            return NULL;
        PyObject_CallMethod(py_md, (char *)"add_sub_mesh", (char *)"O", py_sub);

        lawd_sub = lawd_md->get_sub_at(i);
        num_streams = lawd_sub->get_num_streams();
        for (stream_idx=0; stream_idx<num_streams; stream_idx++) {
            AWD_mesh_str_type str_type;
            AWDDataStream *stream;
            PyObject *py_list = NULL;

            stream = lawd_sub->get_stream_at(stream_idx);
            str_type = (AWD_mesh_str_type)stream->type;
            switch (str_type) {
                case VERTICES:
                case UVS:
                case VERTEX_NORMALS:
                case VERTEX_TANGENTS:
                case VERTEX_WEIGHTS:
                    py_list = pyawdutil_float64_to_pylist(stream->data.f64, stream->get_num_elements());
                    break;

                case TRIANGLES:
                case JOINT_INDICES:
                    py_list = pyawdutil_uint32_to_pylist(stream->data.ui32, stream->get_num_elements());
                    break;
            }

            PyObject_CallMethod(py_sub, (char *)"add_stream", (char *)"iO", (int)str_type, py_list);
        }
    }

    return PyLong_FromLong(ret);
}


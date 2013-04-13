#include <cstdio>
/**
 * This file contains reparation functions for all blocks belonging in the pyawd.geom
 * module. 
 *
 * These are invoked by AWDWriter to convert a python object to it's C++/libawd
 * equivalence and add it to the AWD document.
*/

#include "AWDWriter.h"
#include "bcache.h"
#include "util.h"

void
__prepare_mesh_data(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    int sub_i;
    int num_subs;
    AWDTriGeom *lawd_md;
    const char *name;
    int name_len;
    PyObject *subs_list;

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    lawd_md = new AWDTriGeom(name, name_len);

    // Get list of sub-meshes from python-space attribute __sub_meshes
    subs_list = PyObject_GetAttrString(block, "_AWDTriGeom__sub_geoms");
    num_subs = PyList_Size(subs_list);
    for (sub_i=0; sub_i<num_subs; sub_i++) {
        AWDSubGeom *lawd_sub;
        int str_i;
        int num_streams;
        PyObject *streams_list;
        PyObject *sub = PyList_GetItem(subs_list, sub_i);

        lawd_sub = new AWDSubGeom();

        // Get list of data streams from python-space attribute __data_streams
        streams_list = PyObject_GetAttrString(sub, "_AWDSubGeom__data_streams");
        num_streams = PyList_Size(streams_list);
        for (str_i=0; str_i<num_streams; str_i++) {
            int data_len;
			AWD_field_type elem_type;
            AWD_mesh_str_type str_type;
            AWD_str_ptr lawd_data;
            PyObject *type;
            PyObject *data;
            PyObject *str_tuple;
            
            // Data streams are simple tuples in python-space, where the first value 
            // is the stream type and the second is a list containing the stream data
            str_tuple = PyList_GetItem(streams_list, str_i);
            type = PyTuple_GetItem(str_tuple, 0);
            data = PyTuple_GetItem(str_tuple, 1);
            data_len = PyList_Size(data);

            // Read stream type and treat data differently depending on whether it
            // should be float or integer data.
            str_type = (AWD_mesh_str_type)PyLong_AsLong(type);
            if (str_type == TRIANGLES || str_type == JOINT_INDICES) {
                lawd_data.ui32 = pyawdutil_pylist_to_uint32(data, NULL, data_len);
				elem_type = AWD_FIELD_UINT16;
            }
            else {
                lawd_data.f64 = pyawdutil_pylist_to_float64(data, NULL, data_len);
				elem_type = AWD_FIELD_FLOAT32;
            }

			// TODO: Make elem_type configurable

            // Add stream to libawd sub-mesh
            lawd_sub->add_stream(str_type, elem_type, lawd_data, data_len);
        }

        // Add sub-mesh to libawd mesh data
        lawd_md->add_sub_mesh(lawd_sub);
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_md);

    awd->add_mesh_data(lawd_md);

    // Add to block cache so other blocks can reference it
    pyawd_bcache_add(bcache, block, lawd_md);
}

/**
 * This file contains reparation functions for all blocks belonging in the 
 * pyawd.material module. 
 *
 * These are invoked by AWDWriter to convert a python object to it's C++/libawd
 * equivalence and add it to the AWD document.
*/

#include "AWDWriter.h"
#include "bcache.h"
#include "util.h"


void
__prepare_material(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    const char *name;
    int name_len;
    AWD_mat_type type;
    PyObject *type_attr;

    AWDMaterial *lawd_mat;

    type_attr = PyObject_GetAttrString(block, "type");
    type = (AWD_mat_type)PyLong_AsLong(type_attr);

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    lawd_mat = new AWDMaterial(type, name, name_len);

    if (type == AWD_MATTYPE_COLOR) {
        PyObject *col_attr;

        col_attr = PyObject_GetAttrString(block, "color");
#if PYTHON_VERSION == 3
        if (PyLong_Check(col_attr)) {
            lawd_mat->color = PyLong_AsLong(col_attr);
        }
#else
        if (PyInt_Check(col_attr)) {
            lawd_mat->color = PyInt_AsLong(col_attr);
        }
#endif
    }
    else {
        PyObject *tex_attr;
        PyObject *ath_attr;

        tex_attr = PyObject_GetAttrString(block, "texture");
        if (tex_attr != Py_None) {
            AWDBitmapTexture *tex;
            tex = (AWDBitmapTexture *)pyawd_bcache_get(bcache, tex_attr);
            if (tex)
                lawd_mat->set_texture(tex);
        }

        lawd_mat->repeat = pyawdutil_has_true_attr(block, "repeat");
        lawd_mat->alpha_blending = pyawdutil_has_true_attr(block, "alpha_blending");

        ath_attr = PyObject_GetAttrString(block, "alpha_threshold");
        if (ath_attr && PyNumber_Check(ath_attr)) {
            lawd_mat->alpha_threshold = (awd_float32)PyFloat_AsDouble(PyNumber_Float(ath_attr));
        }
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_mat);

    awd->add_material(lawd_mat);
    pyawd_bcache_add(bcache, block, lawd_mat);
}

void
__prepare_texture(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    const char *name;
    int name_len;
    AWD_tex_type type;
    PyObject *type_attr;

    AWDBitmapTexture *lawd_tex;

    type_attr = PyObject_GetAttrString(block, "type");
    type = (AWD_tex_type)PyLong_AsLong(type_attr);

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    lawd_tex = new AWDBitmapTexture(type, name, name_len);

    if (type == EXTERNAL) {
        const char *url;
        int url_len;

        pyawdutil_get_strattr(block, "url", &url, &url_len);

        if (url) {
            lawd_tex->set_url(url, url_len);
        }
    }
    else {
        PyObject *tex_bytes;
        const char *buf;
        int buf_len;

        tex_bytes = PyObject_GetAttrString(block, "_AWDBitmapTexture__data");
        buf = PyBytes_AsString(tex_bytes);
        buf_len = PyBytes_Size(tex_bytes);

        if (buf_len>0) {
            lawd_tex->set_embed_data((awd_uint8 *)buf, (awd_uint32)buf_len);
        }
        else {
            // TODO: Implement way to set errors (return null)
        }
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_tex);

    awd->add_texture(lawd_tex);
    pyawd_bcache_add(bcache, block, lawd_tex);
}


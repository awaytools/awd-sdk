
#include <Python.h>
#include <awd/libawd.h>

#include "AWDWriter.h"
#include "utils.h"

static PyMethodDef cpyawd_methods[] = {
    {"util_build_geom", (PyCFunction)cpyawd_util_build_geom, METH_VARARGS | METH_KEYWORDS, 
        "Use the libawd AWDGeomUtil to build geometry." },
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PYTHON_VERSION == 3
static struct PyModuleDef cpyawd_mod = {
    PyModuleDef_HEAD_INIT,
    "cpyawd",
    "Back-end module for the PyAWD SDK. This module interfaces with the C++ libawd library.",
    -1,
    cpyawd_methods, 
    NULL, NULL, NULL, NULL
};
#endif


void
_add_mod_type(PyObject *m, const char *name, PyTypeObject *type)
{
    Py_INCREF(type);
    PyType_Ready(type);
    PyModule_AddObject(m, name, (PyObject *)type);
}


/*
 * Python version-agnostic method to initialize C io module.
 * Invoked in different ways depending on whether this module
 * is compiled for Python 2.6 or 3.x.
*/
PyObject *_init_cpyawd(PyObject *m)
{
    // Add classes to module
    _add_mod_type(m, "AWDWriter", &cpyawd_AWDWriterType);

    PyModule_AddIntConstant(m, "LIBAWD_VERSION_MAJOR", AWD::VERSION_MAJOR);
    PyModule_AddIntConstant(m, "LIBAWD_VERSION_MINOR", AWD::VERSION_MINOR);
    PyModule_AddIntConstant(m, "LIBAWD_VERSION_BUILD", AWD::VERSION_BUILD);
    PyModule_AddIntConstant(m, "LIBAWD_VERSION_RELEASE", AWD::VERSION_RELEASE);

    return m;
}



#if PYTHON_VERSION == 3
PyMODINIT_FUNC
PyInit_cpyawd(void)
{
    PyObject *m;

    m = PyModule_Create(&cpyawd_mod);
    if (m == NULL)
        return NULL;

    return _init_cpyawd(m);
}
#else // Python 2.6
PyMODINIT_FUNC
initcpyawd(void)
{
    PyObject *m;

    m = Py_InitModule("cpyawd", cpyawd_methods);

    _init_cpyawd(m);
}
#endif



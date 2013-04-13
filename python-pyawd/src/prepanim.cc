/**
 * This file contains preparation functions for all blocks belonging in the pyawd.anim
 * module. 
 *
 * These are invoked by AWDWriter to convert a python object to it's C++/libawd
 * equivalence and add it to the AWD document.
*/

#include "AWDWriter.h"
#include "bcache.h"
#include "util.h"


static AWDSkeletonJoint *
__prepare_skeljoint(PyObject *py_joint, AWD *awd, AWDSkeletonJoint *lawd_parent)
{
    int i;
    int num_children;
    const char *name;
    int name_len;
    awd_float64 *mtx;

    AWDSkeletonJoint *lawd_joint;

    PyObject *bind_mtx_attr;
    PyObject *children_attr;

    pyawdutil_get_strattr(py_joint, "name", &name, &name_len);

    bind_mtx_attr = PyObject_GetAttrString(py_joint, "inv_bind_mtx");
    if (bind_mtx_attr && bind_mtx_attr != Py_None) {
        PyObject *raw_data_attr;

        raw_data_attr = PyObject_GetAttrString(bind_mtx_attr, "raw_data");
        if (raw_data_attr)
            mtx = pyawdutil_pylist_to_float64(raw_data_attr, NULL, 12);
    }

    lawd_joint = new AWDSkeletonJoint(name, name_len, mtx);

    // Prep any user attributes
    __prepare_attr_element(py_joint, awd, lawd_joint);

    if (lawd_parent)
        lawd_parent->add_child_joint(lawd_joint);


    children_attr = PyObject_GetAttrString(py_joint, "_AWDSkeletonJoint__children");
    num_children = PyList_Size(children_attr);
    for (i=0; i<num_children; i++) {
        PyObject *py_child;

        py_child = PyList_GetItem(children_attr, i);
        __prepare_skeljoint(py_child, awd, lawd_joint);
    }

    return lawd_joint;
}


void
__prepare_skeleton(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    const char *name;
    int name_len;
    AWDSkeleton *lawd_skel;
    PyObject *root_attr;

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    lawd_skel = new AWDSkeleton(name, name_len);

    root_attr = PyObject_GetAttrString(block, "root_joint");
    if (root_attr != Py_None) {
        AWDSkeletonJoint *lawd_joint;

        lawd_joint = __prepare_skeljoint(root_attr, awd, NULL);
        lawd_skel->set_root_joint(lawd_joint);
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_skel);

    awd->add_skeleton(lawd_skel);
    pyawd_bcache_add(bcache, block, lawd_skel);
}

void
__prepare_skelanim(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    int i;
    const char *name;
    int name_len;
    int num_frames;
    PyObject *frames_attr;
    AWDSkeletonAnimation *lawd_anim;

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    frames_attr = PyObject_GetAttrString(block, "_AWDSkeletonAnimation__frames");
    num_frames = PyList_Size(frames_attr);
    
    lawd_anim = new AWDSkeletonAnimation(name, name_len);
    
    for (i=0; i<num_frames; i++) {
        AWDSkeletonPose *lawd_pose;
        PyObject *pose_attr;
        PyObject *dur_attr;
        PyObject *frame;
        awd_uint16 dur;

        frame = PyList_GetItem(frames_attr, i);
        pose_attr = PyObject_GetAttrString(frame, "data");
        dur_attr = PyObject_GetAttrString(frame, "duration");

        lawd_pose = (AWDSkeletonPose *)pyawd_bcache_get(bcache, pose_attr);
        dur = (awd_uint16)PyLong_AsLong(dur_attr);

        lawd_anim->set_next_frame_pose(lawd_pose, dur);
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_anim);

    awd->add_skeleton_anim(lawd_anim);
    pyawd_bcache_add(bcache, block, lawd_anim);
}

void
__prepare_skelpose(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    int i;
    const char *name;
    int name_len;
    int num_jtf;

    PyObject *tfs_attr;
    AWDSkeletonPose *lawd_pose;
    
    pyawdutil_get_strattr(block, "name", &name, &name_len);

    lawd_pose = new AWDSkeletonPose(name, name_len);
    
    tfs_attr = PyObject_GetAttrString(block, "transforms");
    num_jtf = PyList_Size(tfs_attr);
    for (i=0; i<num_jtf; i++) {
        PyObject *joint_tf;
    
        joint_tf = PyList_GetItem(tfs_attr, i);
        if (joint_tf != Py_None) {
            PyObject *raw_data_attr;
            awd_float64 *mtx;

            raw_data_attr = PyObject_GetAttrString(joint_tf, "raw_data");
            mtx = pyawdutil_pylist_to_float64(raw_data_attr, NULL, 12);
            lawd_pose->set_next_transform(mtx);
        }
        else {
            lawd_pose->set_next_transform(NULL);
        }
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_pose);

    awd->add_skeleton_pose(lawd_pose);
    pyawd_bcache_add(bcache, block, lawd_pose);
}



void
__prepare_uvanim(PyObject *block, AWD *awd, pyawd_bcache *bcache)
{
    int i;
    int len;
    const char *name;
    int name_len;
    PyObject *frames_attr;
    AWDUVAnimation *lawd_anim;

    pyawdutil_get_strattr(block, "name", &name, &name_len);

    lawd_anim = new AWDUVAnimation(name, name_len);

    frames_attr = PyObject_GetAttrString(block, "_AWDUVAnimation__frames");
    len = PyList_Size(frames_attr);
    for (i=0; i<len; i++) {
        PyObject *frame;
        PyObject *dur_attr;
        PyObject *data_attr;
        PyObject *raw_data_attr;
        awd_float64 *mtx;
        awd_uint16 dur;

        frame = PyList_GetItem(frames_attr, i);
        data_attr = PyObject_GetAttrString(frame, "data");
        dur_attr = PyObject_GetAttrString(frame, "duration");
        raw_data_attr = PyObject_GetAttrString(data_attr, "raw_data");

        mtx = pyawdutil_pylist_to_float64(raw_data_attr, NULL, 6);
        dur = (awd_uint16)PyLong_AsLong(dur_attr);

        lawd_anim->set_next_frame_tf(mtx, dur);
    }

    // Prep any user attributes
    __prepare_attr_element(block, awd, lawd_anim);

    awd->add_uv_anim(lawd_anim);
    pyawd_bcache_add(bcache, block, lawd_anim);
}


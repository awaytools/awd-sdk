#include <cstdio>
#include <cstring>

#include "util.h"
#include "skeleton.h"

#include "platform.h"

AWDSkeletonJoint::AWDSkeletonJoint(const char *name, awd_uint16 name_len, awd_float64 *bind_mtx) :
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->next = NULL;
    this->first_child = NULL;
    this->last_child = NULL;
    this->num_children = 0;
    this->bind_mtx = NULL;

    set_bind_mtx(bind_mtx);
}

AWDSkeletonJoint::~AWDSkeletonJoint()
{
    AWDSkeletonJoint *cur;

    cur = this->first_child;
    while (cur) {
        AWDSkeletonJoint *next = cur->next;
        cur->next = NULL;
        delete cur; // Will remove it's children too (recursively)
        cur = next;
    }

    if (this->bind_mtx) {
        free(this->bind_mtx);
        this->bind_mtx = NULL;
    }
    this->num_children = 0;
    this->first_child = NULL;
    this->last_child = NULL;
}

awd_uint32
AWDSkeletonJoint::get_id()
{
    return this->id;
}

void
AWDSkeletonJoint::set_parent(AWDSkeletonJoint *joint)
{
    this->parent = joint;
}

AWDSkeletonJoint *
AWDSkeletonJoint::get_parent()
{
    return this->parent;
}

void
AWDSkeletonJoint::set_bind_mtx(awd_float64 *bind_mtx)
{
    if (this->bind_mtx) {
        free(this->bind_mtx);
    }

    this->bind_mtx = bind_mtx;

    if (this->bind_mtx == NULL) {
        this->bind_mtx = awdutil_id_mtx4x4(NULL);
    }
}

awd_float64 *
AWDSkeletonJoint::get_bind_mtx()
{
    return this->bind_mtx;
}

AWDSkeletonJoint *
AWDSkeletonJoint::add_child_joint(AWDSkeletonJoint *joint)
{
    if (joint != NULL) {
        if (joint->get_parent() != NULL) {
            // TODO: Remove from old parent
        }

        // Find place in list of children
        if (this->first_child == NULL) {
            this->first_child = joint;
        }
        else {
            this->last_child->next = joint;
        }

        joint->set_parent(this);
        this->last_child = joint;
        this->last_child->next = NULL;
        this->num_children++;
    }

    return joint;
}

int
AWDSkeletonJoint::calc_length(BlockSettings * blockSettings)
{
    int len;
    AWDSkeletonJoint *child;

    // id + parent + name varstr + matrix
    len = sizeof(awd_uint16) + sizeof(awd_uint16) +
        sizeof(awd_uint16) + this->get_name_length() +
        MTX43_SIZE(blockSettings->get_wide_matrix());

    len += this->calc_attr_length(true,true, blockSettings);

    child = this->first_child;
    while (child) {
        len += child->calc_length(blockSettings);
        child = child->next;
    }

    return len;
}

int
AWDSkeletonJoint::calc_num_children()
{
    int num_children;
    AWDSkeletonJoint *child;

    num_children = this->num_children;
    child = this->first_child;
    while (child) {
        num_children += child->calc_num_children();
        child = child->next;
    }

    return num_children;
}

int
AWDSkeletonJoint::write_joint(int fd, awd_uint32 id, BlockSettings * curBlockSettings)
{
    int num_written;
    awd_uint32 child_id;
    AWDSkeletonJoint *child;
    awd_uint16 par_id_be;
    awd_uint16 id_be;

    this->id = id;

    // Convert numbers to big-endian
    id_be = UI16(this->id);
    if (this->parent)
        par_id_be = UI16(this->parent->id);
    else par_id_be = 0;

    // Write this joint
    write(fd, &id_be, sizeof(awd_uint16));
    write(fd, &par_id_be, sizeof(awd_uint16));
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    awdutil_write_floats(fd, this->bind_mtx, 9, curBlockSettings->get_wide_matrix());
    if (!curBlockSettings->get_wide_matrix()){
        awd_float32 offX=awd_float32(this->bind_mtx[9] * curBlockSettings->get_scale());
        awd_float32 offY=awd_float32(this->bind_mtx[10] *curBlockSettings->get_scale());
        awd_float32 offZ=awd_float32(this->bind_mtx[11]*curBlockSettings->get_scale());
        write(fd, &offX, sizeof(awd_float32));
        write(fd, &offY, sizeof(awd_float32));
        write(fd, &offZ, sizeof(awd_float32));
    }
    else{
        awd_float64 offX=awd_float64(this->bind_mtx[9] * curBlockSettings->get_scale());
        awd_float64 offY=awd_float64(this->bind_mtx[10] *curBlockSettings->get_scale());
        awd_float64 offZ=awd_float64(this->bind_mtx[11]*curBlockSettings->get_scale());
        write(fd, &offX, sizeof(awd_float64));
        write(fd, &offY, sizeof(awd_float64));
        write(fd, &offZ, sizeof(awd_float64));
    }

    //  TODO: Write attributes
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);

    // Write children
    child_id = id+1;
    num_written = 1;
    child = this->first_child;
    while (child) {
        int num_children_written;

        num_children_written = child->write_joint(fd, child_id, curBlockSettings);

        child_id += num_children_written;
        num_written += num_children_written;

        child = child->next;
    }

    return num_written;
}

AWDSkeleton::AWDSkeleton(const char *name, awd_uint16 name_len, int neutralTime) :
    AWDBlock(SKELETON),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->root_joint = NULL;
    this->clip_blocks = new AWDBlockList();
    this->joints_per_vert = 0;
    this->simpleMode=false;
    this->shareAutoAnimator=true;
    this->neutralPose=neutralTime;
}

AWDSkeleton::~AWDSkeleton()
{
    if (this->root_joint!=NULL) {
        delete this->root_joint;
        this->root_joint = NULL;
    }
    delete this->clip_blocks;
}

bool
AWDSkeleton::get_simpleMode(){
    return this->simpleMode;
}
void
AWDSkeleton::set_simpleMode(bool newSimpleMode){
    this->simpleMode=newSimpleMode;
}
bool
AWDSkeleton::get_shareAutoAnimator(){
    return this->shareAutoAnimator;
}
void
AWDSkeleton::set_shareAutoAnimator(bool shareAutoAnimator){
    this->shareAutoAnimator=shareAutoAnimator;
}
AWDBlockList *
AWDSkeleton::get_clip_blocks(){
    return this->clip_blocks;
}
void
AWDSkeleton::set_clip_blocks(AWDBlockList * newBlocklist){
    this->clip_blocks=newBlocklist;
}
int
AWDSkeleton::get_joints_per_vert(){
    return this->joints_per_vert;
}
void
AWDSkeleton::set_joints_per_vert(int jpv){
    this->joints_per_vert=jpv;
}
int
AWDSkeleton::get_neutralPose(){
    return this->neutralPose;
}
void
AWDSkeleton::set_neutralPose(int newNeutralpose){
    this->neutralPose=newNeutralpose;
}
awd_uint32
AWDSkeleton::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = sizeof(awd_uint16) + this->get_name_length() + sizeof(awd_uint16);
    len += this->calc_attr_length(true,true, curBlockSettings);

    if (this->root_joint != NULL)
        len += this->root_joint->calc_length(curBlockSettings);

    return len;
}

void
AWDSkeleton::write_body(int fd, BlockSettings * curBlockSettings)
{
    awd_uint16 num_joints_be;

    num_joints_be = 0;
    if (this->root_joint != NULL)
        num_joints_be = UI16(1 + this->root_joint->calc_num_children());

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &num_joints_be, sizeof(awd_uint16));

    // Write optional properties
    this->properties->write_attributes(fd,  curBlockSettings);

    // Write joints (if any)
    if (this->root_joint != NULL)
        this->root_joint->write_joint(fd, 1,  curBlockSettings);

    // Write user attributes
    this->user_attributes->write_attributes(fd,  curBlockSettings);
}

AWDSkeletonJoint *
AWDSkeleton::get_root_joint()
{
    return this->root_joint;
}

AWDSkeletonJoint *
AWDSkeleton::set_root_joint(AWDSkeletonJoint *joint)
{
    this->root_joint = joint;
    if (this->root_joint != NULL)
        this->root_joint->set_parent(NULL);

    return joint;
}
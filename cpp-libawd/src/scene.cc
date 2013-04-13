
#include "scene.h"
#include "util.h"

#include "platform.h"


AWDSceneBlock::AWDSceneBlock(AWD_block_type type, const char *name, awd_uint16 name_len, awd_float64 *mtx) :
    AWDBlock(type),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->parent = NULL;
    this->children = new AWDBlockList();

    if (mtx == NULL)
        mtx = awdutil_id_mtx4x4(NULL);
    this->set_transform(mtx);
}


AWDSceneBlock::~AWDSceneBlock()
{
    if (this->transform_mtx) {
        free(this->transform_mtx);
        this->transform_mtx = NULL;
    }
}



void
AWDSceneBlock::write_scene_common(int fd, bool wide_mtx)
{
    awd_baddr parent_addr;

    // Get IDs for references, verify byte-order
    parent_addr = 0;
    if (this->parent != NULL)
        parent_addr = UI32(this->parent->get_addr());

    // Write scene block common fields
    // TODO: Move this to separate base class
    write(fd, &parent_addr, sizeof(awd_baddr));
    awdutil_write_floats(fd, this->transform_mtx, 12, wide_mtx);
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
}


awd_uint32
AWDSceneBlock::calc_common_length(bool wide_mtx)
{
    return 4 + MTX43_SIZE(wide_mtx) + sizeof(awd_uint16) + this->get_name_length();
}


void
AWDSceneBlock::set_transform(awd_float64 *mtx)
{
    this->transform_mtx = mtx;
}


AWDBlock *
AWDSceneBlock::get_parent()
{
    return this->parent;
}

void
AWDSceneBlock::set_parent(AWDSceneBlock *parent)
{
    if (parent != this->parent) {
        this->parent = parent;
        if (this->parent) {
            this->parent->add_child(this);
        }
    }
}



void
AWDSceneBlock::add_child(AWDSceneBlock *child)
{
    if (child && !this->children->contains(child)) {
        this->children->append(child);
        child->set_parent(this);
    }
}


void
AWDSceneBlock::remove_child(AWDSceneBlock *child)
{
    // TODO: Implement remove() in BlockList
    //this->children->remove(child);
}



AWDBlockIterator *
AWDSceneBlock::child_iter()
{
    return new AWDBlockIterator(this->children);
}





AWDScene::AWDScene(const char *name, awd_uint16 name_len) :
    AWDSceneBlock(SCENE, name, name_len, NULL)
{
}

AWDScene::~AWDScene()
{
}

awd_uint32
AWDScene::calc_body_length(bool wide_mtx)
{
    return this->calc_common_length(wide_mtx) + 
        this->calc_attr_length(true,true, wide_mtx);
}


void
AWDScene::write_body(int fd, bool wide_mtx)
{
    this->write_scene_common(fd, wide_mtx);
    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}







AWDContainer::AWDContainer(const char *name, awd_uint16 name_len) :
    AWDSceneBlock(CONTAINER, name, name_len, NULL)
{
}


AWDContainer::~AWDContainer()
{
}


awd_uint32
AWDContainer::calc_body_length(bool wide_mtx)
{
    return this->calc_common_length(wide_mtx) + this->calc_attr_length(true,true, wide_mtx);
}


void
AWDContainer::write_body(int fd, bool wide_mtx)
{
    this->write_scene_common(fd, wide_mtx);
    this->properties->write_attributes(fd, wide_mtx);
    this->user_attributes->write_attributes(fd, wide_mtx);
}



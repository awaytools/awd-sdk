#include "scene.h"
#include "util.h"

#include "platform.h"

AWDSceneBlock::AWDSceneBlock(AWD_block_type type, const char *name, awd_uint16 name_len, awd_float64 *mtx) :
    AWDBlock(type),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->parent = NULL;
    this->children = new AWDBlockList(false); // no weak reference (childs should be deleted when parent is deleted)

    this->transform_mtx=NULL;
    if (mtx == NULL){
        mtx = awdutil_id_mtx4x4(NULL);
        this->set_transform(mtx);
        free(mtx);
    }
    else{
        this->set_transform(mtx);
    }
}

AWDSceneBlock::~AWDSceneBlock()
{
    if (this->transform_mtx!=NULL) {
        free(this->transform_mtx);
        this->transform_mtx = NULL;
    }
    delete this->children;
}

void
AWDSceneBlock::write_scene_common(int fd, BlockSettings *curBlockSettings)
{
    awd_baddr parent_addr;

    // Get IDs for references, verify byte-order
    parent_addr = 0;
    if (this->parent != NULL)
        parent_addr = UI32(this->parent->get_addr());

    // Write scene block common fields
    // TODO: Move this to separate base class
    write(fd, &parent_addr, sizeof(awd_baddr));
    awdutil_write_floats(fd, this->transform_mtx, 9, curBlockSettings->get_wide_matrix());
    if (curBlockSettings->get_wide_matrix()){
        awd_float64 offX=awd_float64(this->transform_mtx[9]*curBlockSettings->get_scale());
        awd_float64 offY=awd_float64(this->transform_mtx[10]*curBlockSettings->get_scale());
        awd_float64 offZ=awd_float64(this->transform_mtx[11]*curBlockSettings->get_scale());
        write(fd, &offX, sizeof(awd_float64));
        write(fd, &offY, sizeof(awd_float64));
        write(fd, &offZ, sizeof(awd_float64));
    }
    else{
        awd_float32 offX=awd_float32(this->transform_mtx[9]*curBlockSettings->get_scale());
        awd_float32 offY=awd_float32(this->transform_mtx[10]*curBlockSettings->get_scale());
        awd_float32 offZ=awd_float32(this->transform_mtx[11]*curBlockSettings->get_scale());
        write(fd, &offX, sizeof(awd_float32));
        write(fd, &offY, sizeof(awd_float32));
        write(fd, &offZ, sizeof(awd_float32));
    }
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
    if (this->transform_mtx!=NULL)
        free(this->transform_mtx);
    this->transform_mtx = (double *)malloc(12*sizeof(awd_float64));
    this->transform_mtx[0] = mtx[0];
    this->transform_mtx[1] = mtx[1];
    this->transform_mtx[2] = mtx[2];
    this->transform_mtx[3] = mtx[3];
    this->transform_mtx[4] = mtx[4];
    this->transform_mtx[5] = mtx[5];
    this->transform_mtx[6] = mtx[6];
    this->transform_mtx[7] = mtx[7];
    this->transform_mtx[8] = mtx[8];
    this->transform_mtx[9] = mtx[9];
    this->transform_mtx[10] = mtx[10];
    this->transform_mtx[11] = mtx[11];
}

awd_float64 *
AWDSceneBlock::get_transform()
{
    return this->transform_mtx;
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
void
AWDSceneBlock::make_children_invalide()
{
    // TODO: Implement remove() in BlockList
    //this->children->remove(child);    
    AWDSceneBlock * block;
    AWDBlockIterator * it = new AWDBlockIterator(this->children);
    while ((block = (AWDSceneBlock *)it->next()) != NULL) {
        block->make_invalide();
        block->make_children_invalide();
    }
}

bool
AWDSceneBlock::isEmpty()
{
    if (this->type!=CONTAINER)
        return false;
    AWDSceneBlock * block;
    AWDBlockIterator * it = new AWDBlockIterator(this->children);
    while ((block = (AWDSceneBlock *)it->next()) != NULL) {
        bool isemtpy=block->isEmpty();
        if (!isemtpy)
            return false;
    }
    delete it;
    return true;
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
AWDScene::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    return this->calc_common_length(curBlockSettings->get_wide_matrix()) +
        this->calc_attr_length(true,true, curBlockSettings);
}

void
AWDScene::write_body(int fd, BlockSettings * curBlockSettings)
{
    this->write_scene_common(fd, curBlockSettings);
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}

AWDContainer::AWDContainer(const char *name, awd_uint16 name_len) :
    AWDSceneBlock(CONTAINER, name, name_len, NULL)
{
}
AWDContainer::AWDContainer(const char *name, awd_uint16 name_len, awd_float64 *mtx) :
    AWDSceneBlock(CONTAINER, name, name_len, mtx)
{
}

AWDContainer::~AWDContainer()
{
}

awd_uint32
AWDContainer::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    return this->calc_common_length(curBlockSettings->get_wide_matrix()) + this->calc_attr_length(true,true, curBlockSettings);
}

void
AWDContainer::write_body(int fd, BlockSettings *curBlockSettings)
{
    this->write_scene_common(fd, curBlockSettings);
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}

AWDCommandBlock::AWDCommandBlock(const char * name, awd_uint16 name_length) :
    AWDSceneBlock(COMMAND, name, name_length, NULL)
{
    this->commandProperties = new AWDNumAttrList();
    this->hasSceneInfos=true;
}
AWDCommandBlock::~AWDCommandBlock()
{
    delete this->commandProperties;
}
void
AWDCommandBlock::add_target_light(awd_baddr targetValue)
{
    AWD_field_ptr newVal;
    newVal.v = (awd_baddr *)malloc(sizeof(awd_baddr));
    *newVal.ui32 = targetValue;
    //property id for the target light is 1
    this->commandProperties->set(1, newVal, sizeof(awd_baddr), AWD_FIELD_BADDR);
}

awd_uint32
AWDCommandBlock::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    int len;
    len = sizeof(awd_uint8);//hasSceneInfos
    len += this->calc_common_length(curBlockSettings->get_wide_matrix());
    len += sizeof(awd_uint16);//num commands
    len += sizeof(awd_uint16);//command type (command1)
    len +=this->commandProperties->calc_length(curBlockSettings);
    len += this->calc_attr_length(true,true, curBlockSettings);
    return len;
}

void
AWDCommandBlock::write_body(int fd, BlockSettings *curBlockSettings)
{
    int hasSceneInfos=this->hasSceneInfos;
    write(fd, &hasSceneInfos, sizeof(awd_uint8));//has scene header
    this->write_scene_common(fd, curBlockSettings);
    int numCommands=1;
    write(fd, &numCommands, sizeof(awd_uint16));//num commands
    int commandtype=1;
    write(fd, &commandtype, sizeof(awd_uint16));//command type: 1='PutintoSceneGraph'(used for lights)
    this->commandProperties->write_attributes(fd, curBlockSettings);
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}

AWDSkyBox::AWDSkyBox(const char * name, awd_uint16 name_length) :
    AWDSceneBlock(SKYBOX, name, name_length, NULL)
{
    this->cubeTex = NULL;
}
AWDSkyBox::~AWDSkyBox()
{
    this->cubeTex = NULL;
}
void
AWDSkyBox::set_cube_tex(AWDBlock * newCubeTex)
{
    this->cubeTex = newCubeTex;
}
AWDBlock *
AWDSkyBox::get_cube_tex()
{
    return this->cubeTex;
}

void
AWDSkyBox::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->cubeTex!=NULL) {
        this->cubeTex->prepare_and_add_with_dependencies(export_list);
    }
}
awd_uint32
AWDSkyBox::calc_body_length(BlockSettings * curBlockSettings)
{
    int len;
    len = sizeof(awd_uint16)+this->get_name_length();//name
    len += sizeof(awd_baddr);//cube Tex
    len += this->calc_attr_length(true,true, curBlockSettings);
    return len;
}

void
AWDSkyBox::write_body(int fd, BlockSettings *curBlockSettings)
{
    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    int cubeTex=0;
    if (this->cubeTex!=NULL) {
        cubeTex=this->cubeTex->get_addr();
    }
    write(fd, &cubeTex, sizeof(awd_baddr));//num commands
    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
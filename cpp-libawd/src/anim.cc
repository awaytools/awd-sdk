#include "util.h"
#include "anim.h"

#include "platform.h"
#include <stdlib.h>
#include <cstdio>

#include "mesh.h"

AWDAnimationSet::AWDAnimationSet(const char *name, awd_uint16 name_len, AWD_Anim_Type animtype, const char *sourcePreID, awd_uint16 sourcePreID_len, AWDBlockList * sourcClipList ) :
    AWDBlock(ANIMATION_SET),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->sourcePreID_len=0;
    this->sourcePreID=NULL;
    this->target_geom=NULL;
    this->set_sourcePreID(sourcePreID,sourcePreID_len);
    this->animSet_type = animtype;// 1: skeleton    2: vertex
    this->preAnimationClipNodes = NULL;
    if(sourcClipList!=NULL)
        this->preAnimationClipNodes=sourcClipList;
    this->animationClipNodes = new AWDBlockList();
    this->is_processed=false;
    this->simple_mode=true;
    this->originalPointCnt = 0;
    this->skeletonBlock =NULL;
}
AWDAnimationSet::~AWDAnimationSet()
{
    if(this->preAnimationClipNodes!=NULL)
        delete this->preAnimationClipNodes;
    if (this->sourcePreID_len>0) {
        free(this->sourcePreID);
        this->sourcePreID = NULL;
        this->sourcePreID_len = 0;
    }
    delete this->animationClipNodes;
}

AWDBlock*
AWDAnimationSet::get_target_geom()
{
    return this->target_geom;
}
void
AWDAnimationSet::set_target_geom(AWDBlock* target_geom)
{
    this->target_geom = target_geom;
}
int
AWDAnimationSet::get_originalPointCnt()
{
    return this->originalPointCnt;
}
void
AWDAnimationSet::set_originalPointCnt(int newPointCnt)
{
    this->originalPointCnt = newPointCnt;
}

bool
AWDAnimationSet::get_simple_mode()
{
    return this->simple_mode;
}

void
AWDAnimationSet::set_simple_mode(bool simple_mode)
{
    this->simple_mode = simple_mode;
}
bool
AWDAnimationSet::get_is_processed()
{
    return this->is_processed;
}

void
AWDAnimationSet::set_is_processed(bool isProcessed)
{
    this->is_processed = isProcessed;
}
AWDBlockList *
AWDAnimationSet::get_preAnimationClipNodes()
{
    if(this->preAnimationClipNodes==NULL)
        this->preAnimationClipNodes=new AWDBlockList();
    return this->preAnimationClipNodes;
}

void
AWDAnimationSet::set_preAnimationClipNodes(AWDBlockList * preAnimationClipNodes)
{
    if (this->preAnimationClipNodes!=NULL)
        delete this->preAnimationClipNodes;
    this->preAnimationClipNodes = preAnimationClipNodes;
}
AWDBlockList *
AWDAnimationSet::get_animationClipNodes()
{
    return this->animationClipNodes;
}

void
AWDAnimationSet::set_animationClipNodes(AWDBlockList * newanimClipNodes)
{
    if (this->animationClipNodes!=NULL)
        delete this->animationClipNodes;
    this->animationClipNodes = newanimClipNodes;
}

AWD_Anim_Type
AWDAnimationSet::get_anim_type()
{
    return this->animSet_type;
}

void
AWDAnimationSet::set_anim_type(AWD_Anim_Type animType)
{
    this->animSet_type = animType;
}

char *
AWDAnimationSet::get_sourcePreID()
{
    return this->sourcePreID;
}
void
AWDAnimationSet::set_sourcePreID(const char *sourcePreID, awd_uint16 sourcePreID_len)
{
    if ((sourcePreID != NULL)&&(sourcePreID_len>0)) {
        if (this->sourcePreID_len>0)
            free(this->sourcePreID);
        this->sourcePreID_len = sourcePreID_len+1;
        this->sourcePreID = (char*)malloc(this->sourcePreID_len);
        strncpy_s(this->sourcePreID, size_t(this->sourcePreID_len), sourcePreID, _TRUNCATE);
    }
}

AWDSkeleton *
AWDAnimationSet::get_skeleton()
{
    return this->skeletonBlock;
}

void
AWDAnimationSet::set_skeleton(AWDSkeleton *skeleton)
{
    this->skeletonBlock = skeleton;
}

awd_uint32
AWDAnimationSet::calc_body_length(BlockSettings *curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = sizeof(awd_uint16) + this->get_name_length();
    len += sizeof(awd_uint16);
    len += this->animationClipNodes->get_num_blocks() * sizeof(awd_baddr);
    len += this->calc_attr_length(true,true, curBlockSettings);
    return len;
}

void
AWDAnimationSet::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    AWDBlock *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->animationClipNodes);
    while ((block = it->next()) != NULL)
        block->prepare_and_add_with_dependencies(export_list);
    if (this->animSet_type==ANIMTYPESKELETON){
        if (this->skeletonBlock) {
            this->skeletonBlock->prepare_and_add_with_dependencies(export_list);
            AWD_field_ptr tex_val;
            tex_val.ui16 = (awd_uint16 *)malloc(sizeof(awd_uint16));
            *tex_val.ui16 = this->skeletonBlock->get_joints_per_vert();
            this->properties->set(1, tex_val, sizeof(awd_uint16), AWD_FIELD_UINT16);
        }
    }
    delete it;
}
void
AWDAnimationSet::write_body(int fd, BlockSettings *curBlockSettings)
{
    AWDBlock *block;
    AWDBlockIterator *it;
    awd_uint16 numClips;

    numClips = UI16((awd_uint16)this->animationClipNodes->get_num_blocks());

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());

    write(fd, &numClips, sizeof(awd_uint16));

    this->properties->write_attributes(fd, curBlockSettings);

    it = new AWDBlockIterator(this->animationClipNodes);
    while ((block = it->next()) != NULL) {
        awd_baddr addr = UI32(block->get_addr());
        write(fd, &addr, sizeof(awd_baddr));
    }
    delete it;
    this->user_attributes->write_attributes(fd, curBlockSettings);
}

AWDAnimator::AWDAnimator(const char *name, awd_uint16 name_len, AWDAnimationSet *animSet, AWD_Anim_Type animType ) :
    AWDBlock(ANIMATOR),
    AWDNamedElement(name, name_len),
    AWDAttrElement()
{
    this->animator_type = animType;// 1: skeleton    2: vertex
    this->animationSet=animSet;
    this->skeletonBlock = NULL;
    this->animator_targets = new AWDBlockList();
    this->animatorProperties = new AWDNumAttrList();
}
AWDAnimator::~AWDAnimator()
{
    delete this->animatorProperties;
    delete this->animator_targets;
}

AWDSkeleton *
AWDAnimator::get_skeleton()
{
    return this->skeletonBlock;
}

void
AWDAnimator::set_skeleton(AWDSkeleton *skeleton)
{
    this->skeletonBlock = skeleton;
}

AWDAnimationSet *
AWDAnimator::get_animationSet()
{
    return this->animationSet;
}

void
AWDAnimator::set_animationSet(AWDAnimationSet *animationSet)
{
    this->animationSet = animationSet;
}

AWD_Anim_Type
AWDAnimator::get_anim_type()
{
    return this->animator_type;
}

AWDBlockList *
AWDAnimator::get_targets()
{
    return this->animator_targets;
}
void
AWDAnimator::set_anim_type(AWD_Anim_Type animType)
{
    this->animator_type = animType;
}
void
AWDAnimator::add_target(AWDMeshInst *newTarget)
{
    this->animator_targets->append(newTarget);
}

void
AWDAnimator::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    if (this->animationSet != NULL){
        this->animator_type=this->animationSet->get_anim_type();
        this->animationSet->prepare_and_add_with_dependencies(export_list);
        if (this->animationSet->get_anim_type()==ANIMTYPESKELETON){
            if (this->skeletonBlock==NULL)
                this->skeletonBlock=this->animationSet->get_skeleton();
            if (this->skeletonBlock==NULL){
                //error
            }
            else{
                this->skeletonBlock->prepare_and_add_with_dependencies(export_list);
                AWD_field_ptr tex_val;
                tex_val.addr = (awd_baddr *)malloc(sizeof(awd_baddr));
                *tex_val.addr = this->skeletonBlock->get_addr();
                this->animatorProperties->set(1, tex_val, sizeof(awd_baddr), AWD_FIELD_BADDR);
            }
        }
    }
    AWDBlock *block;
    AWDBlockIterator *it;
    it = new AWDBlockIterator(this->animator_targets);
    while ((block = it->next()) != NULL)
        block->prepare_and_add_with_dependencies(export_list);
    delete it;
}
awd_uint32
AWDAnimator::calc_body_length(BlockSettings * curBlockSettings)
{
    if(!this->get_isValid())
        return 0;
    awd_uint32 len;

    len = sizeof(awd_uint16) + this->get_name_length();
    len += sizeof(awd_uint16); //animatortype
    len += this->calc_attr_length(true,true, curBlockSettings);
    len += this->animatorProperties->calc_length(curBlockSettings);
    len += sizeof(awd_uint32);//animSetID
    len += sizeof(awd_uint16);//num_targets
    len += sizeof(awd_uint16);//activeState
    len += sizeof(awd_uint8);//autoPlay
    //len += sizeof(awd_uint32);//autoPlay
    len += this->animator_targets->get_num_blocks()*sizeof(awd_uint32);//target IDs

    return len;
}
void
AWDAnimator::write_body(int fd, BlockSettings *curBlockSettings)
{
    awd_baddr animSet_addr;
    AWDBlock *block;
    AWDBlockIterator *it;
    awd_uint16 num_targets;
    awd_uint16 animatorType=this->animator_type;

    awdutil_write_varstr(fd, this->get_name(), this->get_name_length());
    write(fd, &animatorType, sizeof(awd_uint16));

    this->animatorProperties->write_attributes(fd, curBlockSettings);

    // Write animSet id - TODO-> ERROR IF NULL
    animSet_addr = 0;
    if (this->animationSet != NULL)
        animSet_addr = UI32(this->animationSet->get_addr());
    write(fd, &animSet_addr, sizeof(awd_uint32));

    num_targets = UI16((awd_uint16)this->animator_targets->get_num_blocks());
    write(fd, &num_targets, sizeof(awd_uint16));
    it = new AWDBlockIterator(this->animator_targets);
    while ((block = it->next()) != NULL) {
        awd_baddr addr = UI32(block->get_addr());
        write(fd, &addr, sizeof(awd_baddr));
    }
    delete it;
    int activeState=0;
    write(fd, &activeState, sizeof(awd_uint16));
    int autoplay=0;
    write(fd, &autoplay, sizeof(awd_uint8));

    this->properties->write_attributes(fd, curBlockSettings);
    this->user_attributes->write_attributes(fd, curBlockSettings);
}
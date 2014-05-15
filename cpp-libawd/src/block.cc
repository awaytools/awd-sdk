#include "awd_types.h"
#include "block.h"
#include "util.h"

#include "platform.h"

BlockSettings::BlockSettings(bool wideMatrix, bool wideGeom, bool wideProps, bool wideAttributes, double scale)
{
        this->wideMatrix=wideMatrix;
        this->wideGeom=wideGeom;
        this->wideProps=wideProps;
        this->wideAttributes=wideAttributes;
        this->scale=scale;
}

BlockSettings::~BlockSettings()
{
}

bool
BlockSettings::get_wide_matrix()
{
    return this->wideMatrix;
}
void
BlockSettings::set_wide_matrix(bool wideMatrix)
{
    this->wideMatrix=wideMatrix;
}
bool
BlockSettings::get_wide_geom()
{
    return this->wideGeom;
}
void
BlockSettings::set_wide_geom(bool wideGeom)
{
    this->wideGeom=wideGeom;
}
bool
BlockSettings::get_wide_props()
{
    return this->wideProps;
}
void
BlockSettings::set_wide_props(bool wideProps)
{
    this->wideProps=wideProps;
}
bool
BlockSettings::get_wide_attributes()
{
    return this->wideAttributes;
}
void
BlockSettings::set_wide_attributes(bool wideAttributes)
{
    this->wideAttributes=wideAttributes;
}
double
BlockSettings::get_scale()
{
    return this->scale;
}
void
BlockSettings::set_scale(double scale)
{
    this->scale=scale;
}

AWDBlock::AWDBlock(AWD_block_type type)
{
    this->type = type;

    // TODO: Allow setting flags
    this->flags = 0;
    this->addr = 0;
    this->isValid =true; //true as long as the block is valdi and should get exported
    this->isExported =false; //true if block was exported by the export process (for all files)
    this->isExportedToFile =false; //true if block was exported for one file (gets reset befor exporting one file)
}

AWDBlock::~AWDBlock()
{
}

bool
AWDBlock::get_isValid()
{
    return this->isValid;
}
void
AWDBlock::make_invalide()
{
    this->isValid = false;
}

void
AWDBlock::prepare_and_add_dependencies(AWDBlockList *export_list)
{
    // Does nothing by default. Can be optionally
    // overriden by sub-classes to take any actions
    // that need to happen before length is calculated
}

void
AWDBlock::prepare_and_add_with_dependencies( AWDBlockList *target_list)
{
    if(isValid){
        if (!this->isExportedToFile){
            this->isExportedToFile=true;
            this->isExported=true;
            this->prepare_and_add_dependencies(target_list);
            this->addr = target_list->get_num_blocks();
            target_list->append(this);
        }
    }
    else{
        this->addr = 0;
    }
}
size_t
AWDBlock::write_block(int fd, BlockSettings *curBlockSettings)
{
    if(!this->isValid)
        return 0;
    awd_uint8 ns_addr;
    awd_uint32 length;
    awd_uint32 length_be;
    awd_baddr block_addr_be;

    this->isExported=true;

    length = this->calc_body_length(curBlockSettings);

    //TODO: Get addr of actual namespace
    ns_addr = 0;

    // Convert to big-endian if necessary
    block_addr_be = UI32(this->addr);
    length_be = UI32(length);
    if (curBlockSettings->get_wide_matrix())//bit1 = storagePrecision Matrix
        this->flags |= 0x01;
    if (curBlockSettings->get_wide_geom())//bit2 = storagePrecision Geo
        this->flags |= 0x02;
    if (curBlockSettings->get_wide_props())//bit3 = storagePrecision Props
        this->flags |= 0x04;
    // Write header
    write(fd, &block_addr_be, sizeof(awd_baddr));
    write(fd, &ns_addr, sizeof(awd_uint8));
    write(fd, &this->type, sizeof(awd_uint8));
    write(fd, &this->flags, sizeof(awd_uint8));
    write(fd, &length_be, sizeof(awd_uint32));

    // Write body using concrete implementation
    // in block sub-classes
    this->write_body(fd, curBlockSettings);

    return (size_t)length + 11;
}
awd_baddr
AWDBlock::get_addr()
{
    return this->addr;
}

AWD_block_type
AWDBlock::get_type()
{
    return this->type;
}

AWDBlockList::AWDBlockList(bool weakReference)
{
    this->first_block = NULL;
    this->last_block = NULL;
    this->num_blocks = 0;
    this->weakReference=weakReference;
}

AWDBlockList::~AWDBlockList()
{
    list_block *cur;
    cur = this->first_block;
    while(cur) {
        list_block *next = cur->next;
        cur->next = NULL;
        if(!weakReference){
            if(cur->block->get_type()==TRI_GEOM){
                AWDTriGeom * thisTriGeom=(AWDTriGeom*)cur->block;
                if (thisTriGeom!=NULL)
                    delete thisTriGeom;
            }
            else if(cur->block->get_type()==PRIM_GEOM){
                AWDPrimitive * thisPrim=(AWDPrimitive*)cur->block;
                if (thisPrim!=NULL)
                    delete thisPrim;
            }
            else if(cur->block->get_type()==SCENE){    // should not be used yet
                AWDScene * thisScene=(AWDScene*)cur->block;
                if (thisScene!=NULL)
                    delete thisScene;
            }
            else if(cur->block->get_type()==CONTAINER){
                AWDContainer * thisContainer=(AWDContainer*)cur->block;
                if (thisContainer!=NULL)
                    delete thisContainer;
            }
            else if(cur->block->get_type()==MESH_INSTANCE){
                AWDMeshInst * thisMesh=(AWDMeshInst*)cur->block;
                if (thisMesh!=NULL)
                    delete thisMesh;
            }
            else if(cur->block->get_type()==SKYBOX){
                AWDSkyBox * thisSkyBox=(AWDSkyBox*)cur->block;
                if (thisSkyBox!=NULL)
                    delete thisSkyBox;
            }
            else if(cur->block->get_type()==LIGHT){
                AWDLight * thisLight=(AWDLight*)cur->block;
                if (thisLight!=NULL)
                    delete thisLight;
            }
            else if(cur->block->get_type()==CAMERA){
                AWDCamera * thisCam=(AWDCamera*)cur->block;
                if (thisCam!=NULL)
                    delete thisCam;
            }
            else if(cur->block->get_type()==TEXTURE_PROJECTOR){
                AWDTextureProjector * thisTextProject=(AWDTextureProjector*)cur->block;
                if (thisTextProject!=NULL)
                    delete thisTextProject;
            }
            else if(cur->block->get_type()==LIGHTPICKER){
                AWDLightPicker * thisLightPicker=(AWDLightPicker*)cur->block;
                if (thisLightPicker!=NULL)
                    delete thisLightPicker;
            }
            else if(cur->block->get_type()==SIMPLE_MATERIAL){
                AWDMaterial * thisMat=(AWDMaterial*)cur->block;
                if (thisMat!=NULL)
                    delete thisMat;
            }
            else if(cur->block->get_type()==BITMAP_TEXTURE){
                AWDBitmapTexture * thisTex=(AWDBitmapTexture*)cur->block;
                if (thisTex!=NULL)
                    delete thisTex;
            }
            else if(cur->block->get_type()==CUBE_TEXTURE){
                AWDCubeTexture * thisCubeTex=(AWDCubeTexture*)cur->block;
                if (thisCubeTex!=NULL)
                    delete thisCubeTex;
            }
            else if(cur->block->get_type()==CUBE_TEXTURE_ATF){    // should not be used yet
                AWDCubeTexture * thisCubeTex=(AWDCubeTexture*)cur->block;
                if (thisCubeTex!=NULL)
                    delete thisCubeTex;
            }
            else if(cur->block->get_type()==EFFECT_METHOD){
                AWDEffectMethod* thisFXMethod=(AWDEffectMethod*)cur->block;
                if (thisFXMethod!=NULL)
                    delete thisFXMethod;
            }
            else if(cur->block->get_type()==SHADOW_METHOD){
                AWDShadowMethod* thisShadowMethod=(AWDShadowMethod*)cur->block;
                if (thisShadowMethod!=NULL)
                    delete thisShadowMethod;
            }
            else if(cur->block->get_type()==SKELETON){
                AWDSkeleton* thisSkel=(AWDSkeleton*)cur->block;
                if (thisSkel!=NULL)
                    delete thisSkel;
            }
            else if(cur->block->get_type()==SKELETON_ANIM){
                AWDSkeletonAnimation* thisSkelanim=(AWDSkeletonAnimation*)cur->block;
                if (thisSkelanim!=NULL)
                    delete thisSkelanim;
            }
            else if(cur->block->get_type()==SKELETON_POSE){
                AWDSkeletonPose* thisSkelpose=(AWDSkeletonPose*)cur->block;
                if (thisSkelpose!=NULL)
                    delete thisSkelpose;
            }
            //VERTEX_POSE=111,
            else if(cur->block->get_type()==VERTEX_ANIM){
                AWDVertexAnimation* thisVertexAnim=(AWDVertexAnimation*)cur->block;
                if (thisVertexAnim!=NULL)
                    delete thisVertexAnim;
            }
            else if(cur->block->get_type()==ANIMATION_SET){
                AWDAnimationSet* thisAnimSet=(AWDAnimationSet*)cur->block;
                if (thisAnimSet!=NULL)
                    delete thisAnimSet;
            }
            else if(cur->block->get_type()==ANIMATOR){
                AWDAnimator* thisAnimator=(AWDAnimator*)cur->block;
                if (thisAnimator!=NULL)
                    delete thisAnimator;
            }
            //UV_ANIM=121,
            else if(cur->block->get_type()==COMMAND){
                AWDCommandBlock * thisCommand=(AWDCommandBlock*)cur->block;
                if (thisCommand!=NULL)
                    delete thisCommand;
            }
            else if(cur->block->get_type()==NAMESPACE){
                AWDNamespace * thisNS=(AWDNamespace*)cur->block;
                if (thisNS!=NULL)
                    delete thisNS;
            }
            else if(cur->block->get_type()==MESSAGE){
                AWDMessageBlock * thisMessage=(AWDMessageBlock*)cur->block;
                if (thisMessage!=NULL)
                    delete thisMessage;
            }
            // the Metadata-block never gets stored in a BlockList, so it doenst need to be deleted here
        }
        free(cur);
        cur = next;
    }

    // Already deleted as part
    // of above loop
    this->first_block = NULL;
    this->last_block = NULL;
}

bool
AWDBlockList::append(AWDBlock *block)
{
    if (!this->contains(block)) {
        list_block *ctr = (list_block *)malloc(sizeof(list_block));
        ctr->block = block;
        if (this->first_block == NULL) {
            this->first_block = ctr;
        }
        else {
            this->last_block->next = ctr;
        }

        this->last_block = ctr;
        this->last_block->next = NULL;
        this->num_blocks++;

        return true;
    }
    else {
        return false;
    }
}

bool
AWDBlockList::replace(AWDBlock *block, AWDBlock *oldBlock)
{
    list_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block){
            cur->block=oldBlock;
            return true;
        }

        cur = cur->next;
    }

    return false;
}

void
AWDBlockList::force_append(AWDBlock *block)
{
    list_block *ctr = (list_block *)malloc(sizeof(list_block));
    ctr->block = block;
    ctr->blockIdx = this->num_blocks;
    if (this->first_block == NULL) {
        this->first_block = ctr;
    }
    else {
        this->last_block->next = ctr;
    }

    this->last_block = ctr;
    this->last_block->next = NULL;
    this->num_blocks++;
}

AWDBlock *
AWDBlockList::getByIndex(int idx)
{
    if (idx>=this->num_blocks)
        return NULL;
    list_block *cur;
    cur = this->first_block;
    int i;
    for (i=0;i<idx;i++){
        cur = cur->next;
    }
    return cur->block;
}

bool
AWDBlockList::contains(AWDBlock *block)
{
    list_block *cur;

    cur = this->first_block;
    while (cur) {
        if (cur->block == block)
            return true;

        cur = cur->next;
    }

    return false;
}

int
AWDBlockList::get_num_blocks()
{
    return this->num_blocks;
}
int
AWDBlockList::get_num_blocks_valid()
{
    int num_blocks_valid=0;
    list_block *cur;
    cur = this->first_block;
    while (cur) {
        if (cur->block->get_isValid())
            num_blocks_valid++;
        cur = cur->next;
    }
    return num_blocks_valid;
}

AWDBlockIterator::AWDBlockIterator(AWDBlockList *list)
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}

AWDBlockIterator::~AWDBlockIterator()
{
    this->list = list;
    this->cur_block = NULL;
    this->reset();
}

void
AWDBlockIterator::reset()
{
    if (this->list != NULL)
        this->cur_block = this->list->first_block;
}

AWDBlock *
AWDBlockIterator::next()
{
    list_block *tmp;

    tmp = this->cur_block;
    if (this->cur_block != NULL)
        this->cur_block = this->cur_block->next;

    if (tmp)
        return tmp->block;
    else return NULL;
}

AWDAnimationClipNode::AWDAnimationClipNode(int start_frame, int end_frame, int skip_frame, bool stitch_final, const char * sourceID, bool loop, bool useTransforms)

{
    this->start_frame = start_frame;
    this->end_frame = end_frame;
    this->skip_frame = skip_frame;
    this->stitch_final = stitch_final;
    this->loop = loop;
    this->useTransforms = useTransforms;

    this->sourceID_len=0;
    this->set_sourceID(sourceID, awd_uint16(strlen(sourceID)));
    this->is_processed=false;
}

AWDAnimationClipNode::~AWDAnimationClipNode()
{
    if(this->sourceID_len>0){
        free(this->sourceID);
        this->sourceID_len=0;
    }
}

char *
AWDAnimationClipNode::get_sourceID()
{
    return this->sourceID;
}

void
AWDAnimationClipNode::set_sourceID(const char *name, awd_uint16 name_len)
{
    if(this->sourceID_len>0)
        free(this->sourceID);
    if ((name != NULL)&&(name_len>0)) {
        this->sourceID_len = name_len+1;
        this->sourceID = (char*)malloc(this->sourceID_len);
        strncpy_s(this->sourceID, this->sourceID_len, name, _TRUNCATE);
    }
}

bool
AWDAnimationClipNode::get_is_processed()
{
    return this->is_processed;
}

void
AWDAnimationClipNode::set_is_processed(bool isProcessed)
{
    this->is_processed = isProcessed;
}

int
AWDAnimationClipNode::get_start_frame()
{
    return this->start_frame;
}
int
AWDAnimationClipNode::get_end_frame()
{
    return this->end_frame;
}
int
AWDAnimationClipNode::get_skip_frame()
{
    return this->skip_frame;
}
bool
AWDAnimationClipNode::get_stitch_final()
{
    return this->stitch_final;
}
bool
AWDAnimationClipNode::get_loop()
{
    return this->loop;
}
bool
AWDAnimationClipNode::get_use_transforms()
{
    return this->useTransforms;
}
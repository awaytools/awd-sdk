#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "platform.h"

#include "awd.h"
#include "util.h"
#include "awdlzma.h"
#include "awdzlib.h"
#include "message.h"
#include "Types.h"
#include "LzmaEnc.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
   #ifndef DBG_NEW
      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
      #define new DBG_NEW
   #endif
#endif  // _DEBUG

const int AWD::VERSION_MAJOR = 2;
const int AWD::VERSION_MINOR = 1;
const int AWD::VERSION_BUILD = 0;
const char AWD::VERSION_RELEASE = 'a';

AWD::AWD(AWD_compression compression, awd_uint16 flags, char *outPathName, bool splitByRootObjs, BlockSettings * thisBlockSettings, bool exportEmtpyContainers)
{
    this->major_version = VERSION_MAJOR;
    this->minor_version = VERSION_MINOR;
    this->compression = compression;
    this->flags = flags;
    this->splitByRootObjs = splitByRootObjs;
    this->outPath=outPathName;
    this->exportEmtpyContainers=exportEmtpyContainers;
    this->thisBlockSettings=thisBlockSettings;
    // all this block-lists should have non-weak reference set, so they will delete the blocks
    // this means, that every awdblock should only occur one time in only one of this lists:
    this->texture_blocks = new AWDBlockList(false);
    this->cubetex_blocks = new AWDBlockList(false);
    this->material_blocks = new AWDBlockList(false);
    this->mesh_data_blocks = new AWDBlockList(false);
    this->skeleton_blocks = new AWDBlockList(false);
    this->skelanim_blocks = new AWDBlockList(false);
    this->skelpose_blocks = new AWDBlockList();//skel_pose-blocks are getting deleted within skelanim_blocks
    this->uvanim_blocks = new AWDBlockList(false);
    this->animator_blocks = new AWDBlockList(false);
    this->scene_blocks = new AWDBlockList(false);
    this->namespace_blocks = new AWDBlockList(false);
    this->prim_blocks = new AWDBlockList(false);
    this->amin_set_blocks = new AWDBlockList(false);
    this->vertex_anim_blocks = new AWDBlockList(false);
    this->effect_method_blocks = new AWDBlockList(false);
    this->command_blocks = new AWDBlockList(false);
    this->lightPicker_blocks = new AWDBlockList(false);
    this->shadow_blocks = new AWDBlockList(false);
    this->message_blocks = new AWDBlockList(false);
    this->darkLightPicker=NULL;
    this->metadata = NULL;
    this->last_used_nsid = 0;
    this->last_used_baddr = 0;
    this->header_written = AWD_FALSE;
}

AWD::~AWD()
{
    delete this->texture_blocks;
    delete this->cubetex_blocks;
    delete this->material_blocks;
    delete this->mesh_data_blocks;
    delete this->skeleton_blocks;
    delete this->skelanim_blocks;
    delete this->skelpose_blocks;
    delete this->uvanim_blocks;
    delete this->animator_blocks;
    delete this->scene_blocks;
    delete this->namespace_blocks;
    delete this->prim_blocks;
    delete this->amin_set_blocks;
    delete this->vertex_anim_blocks;
    delete this->effect_method_blocks;
    delete this->lightPicker_blocks;
    delete this->command_blocks;
    delete this->shadow_blocks;
    delete this->message_blocks;
    if (this->metadata){
        delete this->metadata;}
    this->metadata=NULL;

    if (this->darkLightPicker!=NULL)
        delete this->darkLightPicker;
    this->darkLightPicker=NULL;
    delete this->thisBlockSettings;
}
int
AWD::count_all_valid_blocks()
{
    int blockCnt=0;
    blockCnt+=this->texture_blocks->get_num_blocks_valid();
    blockCnt+=this->cubetex_blocks->get_num_blocks_valid();
    blockCnt+=this->material_blocks->get_num_blocks_valid();
    blockCnt+=this->mesh_data_blocks->get_num_blocks_valid();
    blockCnt+=this->skeleton_blocks->get_num_blocks_valid();
    blockCnt+=this->skelanim_blocks->get_num_blocks_valid();
    blockCnt+=this->skelpose_blocks->get_num_blocks_valid();
    blockCnt+=this->uvanim_blocks->get_num_blocks_valid();
    blockCnt+=this->animator_blocks->get_num_blocks_valid();
    blockCnt+=this->scene_blocks->get_num_blocks_valid();
    blockCnt+=this->namespace_blocks->get_num_blocks_valid();
    blockCnt+=this->prim_blocks->get_num_blocks_valid();
    blockCnt+=this->amin_set_blocks->get_num_blocks_valid();
    blockCnt+=this->vertex_anim_blocks->get_num_blocks_valid();
    blockCnt+=this->effect_method_blocks->get_num_blocks_valid();
    blockCnt+=this->lightPicker_blocks->get_num_blocks_valid();
    blockCnt+=this->command_blocks->get_num_blocks_valid();
    blockCnt+=this->shadow_blocks->get_num_blocks_valid();
    return blockCnt;
}
bool
AWD::has_flag(int flag)
{
    return ((this->flags & flag) > 0);
}

AWDLightPicker * AWD::CreateDarkLightPicker()
{
    if(this->darkLightPicker==NULL){
        if(this->lightPicker_blocks->get_num_blocks()>0){
            AWDLight * awdDarkLight = NULL;
            char * lightName_ptr="DarkLight";
            awdDarkLight=new AWDLight(lightName_ptr, awd_uint16(strlen(lightName_ptr)));
            awdDarkLight->set_light_type(AWD_LIGHT_POINT);
            awdDarkLight->set_diffuse(0);
            awdDarkLight->set_specular(0);
            awdDarkLight->set_ambient(0);
            AWDBlockList * lightBlocks=new AWDBlockList();
            lightBlocks->append(awdDarkLight);
            char *name = "DarkLightPicker";
            this->darkLightPicker=new AWDLightPicker(name, awd_uint16(strlen(name)));
            this->darkLightPicker->set_lights(lightBlocks);
            this->add_scene_block(awdDarkLight);
            return this->darkLightPicker;
        }
        return NULL;
    }
    else
        return this->darkLightPicker;
}

void
AWD::add_light_picker_block(AWDLightPicker *block)
{
    this->lightPicker_blocks->append(block);
}
void
AWD::set_metadata(AWDMetaData *block)
{
    this->metadata = block;
}
void
AWD::add_shadow(AWDShadowMethod *block)
{
    this->shadow_blocks->append(block);
}
void
AWD::add_effect_method_block(AWDEffectMethod *block)
{
    this->effect_method_blocks->append(block);
}
void
AWD::add_material(AWDMaterial *block)
{
    this->material_blocks->append(block);
}
void
AWD::add_texture(AWDBitmapTexture *block)
{
    this->texture_blocks->append(block);
}
void
AWD::add_cube_texture(AWDCubeTexture *block)
{
    this->cubetex_blocks->append(block);
}
void
AWD::add_mesh_data(AWDBlock *block)
{
    this->mesh_data_blocks->append(block);
}
void
AWD::add_prim_block(AWDPrimitive *block)
{
    this->prim_blocks->append(block);
}
void
AWD::add_amin_set_block(AWDAnimationSet *block)
{
    this->amin_set_blocks->append(block);
}
void
AWD::add_vertex_anim_block(AWDVertexAnimation *block)
{
    this->vertex_anim_blocks->append(block);
}
void
AWD::add_scene_block(AWDSceneBlock *block)
{
    this->scene_blocks->append(block);
}
void
AWD::add_skeleton(AWDSkeleton *block)
{
    this->skeleton_blocks->append(block);
}
void
AWD::add_skeleton_pose(AWDSkeletonPose *block)
{
    this->skelpose_blocks->append(block);
}
void
AWD::add_skeleton_anim(AWDSkeletonAnimation *block)
{
    this->skelanim_blocks->append(block);
}
void
AWD::add_uv_anim(AWDUVAnimation *block)
{
    this->uvanim_blocks->append(block);
}
void
AWD::add_animator(AWDAnimator *block)
{
    this->animator_blocks->append(block);
}
void
AWD::add_namespace(AWDNamespace *block)
{
    if (this->namespace_blocks->append(block)) {
        this->last_used_nsid++;
        block->set_handle(this->last_used_nsid);
    }
}

AWDNamespace *
AWD::get_namespace(const char *uri)
{
    AWDNamespace *ns;
    AWDBlockIterator it(this->namespace_blocks);

    while ((ns = (AWDNamespace *)it.next()) != NULL) {
        const char *ns_uri;
        int ns_uri_len;

        ns_uri = ns->get_uri(&ns_uri_len);

        if (strncmp(ns_uri, uri, ns_uri_len)==0) {
            return ns;
        }
    }

    return NULL;
}

void
AWD::write_header(int fd, awd_uint32 body_length)
{
    awd_uint16 flags_be;

    // Convert to big-endian if necessary
    flags_be = UI16(this->flags);
    body_length = UI32(body_length);

    write(fd, "AWD", 3);
    write(fd, &this->major_version, sizeof(awd_uint8));
    write(fd, &this->minor_version, sizeof(awd_uint8));
    write(fd, &flags_be, sizeof(awd_uint16));
    write(fd, (awd_uint8*)&this->compression, sizeof(awd_uint8));
    write(fd, &body_length, sizeof(awd_uint32));
}

void
AWD::check_exported_blocks(AWDBlockList * blocks)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);
    len = 0;
    while ((block = it.next()) != NULL) {
        if(block->get_isValid()){
            if(!block->isExported){
                AWDMessageBlock * newWarning = new AWDMessageBlock("n.a.", "Block not exported because not used in the AWDScene");
                this->message_blocks->append(newWarning);
            }
        }
    }
}

void
AWD::re_order_blocks(AWDBlockList *blocks, AWDBlockList *targetList)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);
    len = 0;
    while ((block = it.next()) != NULL) {
        if(block->get_isValid())
            block->prepare_and_add_with_dependencies(targetList);
    }
}
size_t
AWD::write_blocks(AWDBlockList *blocks, int fd)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    len = 0;
    while ((block = it.next()) != NULL) {
        len += block->write_block(fd, this->thisBlockSettings);
    }

    return len;
}

void
AWD::flatten_scene(AWDSceneBlock *cur, AWDBlockList *flat_list)
{
    if(cur->get_isValid()){
        if (((cur->get_type()==LIGHT)||(cur->get_type()==TEXTURE_PROJECTOR))&&(cur->isExported)){
            cur->isExported=true;
            AWDCommandBlock * awdCommand=new AWDCommandBlock(cur->get_name(),cur->get_name_length());
            awdCommand->add_target_light(cur->get_addr());
            awdCommand->set_transform(cur->get_transform());
            AWDSceneBlock * parentBlock = (AWDSceneBlock *)cur->get_parent();
            if (parentBlock!=NULL){
                awdCommand->set_parent(parentBlock);
                awdCommand->prepare_and_add_with_dependencies(flat_list);
            }
            else
                this->command_blocks->append(awdCommand);

            cur->set_parent(NULL);
        }
        else{
            bool exportThis=true;
            if (this->exportEmtpyContainers)
                exportThis=!cur->isEmpty();
            if(exportThis)
                cur->prepare_and_add_with_dependencies(flat_list);

            cur->isExported=true;
        }

        AWDBlock *child;
        AWDBlockIterator *children;
        children = cur->child_iter();
        while ((child = children->next()) != NULL) {
            this->flatten_scene((AWDSceneBlock*)child, flat_list);
        }

        delete children;
    }
}

void
AWD::reorder_scene(AWDBlockList *blocks, AWDBlockList *ordered)
{
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    while ((block = it.next()) != NULL) {
        this->flatten_scene((AWDSceneBlock*)block, ordered);
    }
}
void
AWD::reset_blocks(AWDBlockList *blocks)
{
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    while ((block = it.next()) != NULL) {
        block->isExportedToFile=false;
    }
}
void
AWD::reset_all_blocks()
{
    this->reset_blocks(this->scene_blocks);
    this->reset_blocks(this->namespace_blocks);
    this->reset_blocks(this->skeleton_blocks);
    this->reset_blocks(this->skelpose_blocks);
    this->reset_blocks(this->skelanim_blocks);
    this->reset_blocks(this->texture_blocks);
    this->reset_blocks(this->material_blocks);
    this->reset_blocks(this->mesh_data_blocks);
    this->reset_blocks(this->uvanim_blocks);
}
void
AWD::reset_blocks2(AWDBlockList *blocks)
{
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    while ((block = it.next()) != NULL) {
        block->isExportedToFile=block->isExported;
    }
}
void
AWD::reset_all_blocks2()
{
    this->reset_blocks2(this->namespace_blocks);
    this->reset_blocks2(this->skeleton_blocks);
    this->reset_blocks2(this->skelpose_blocks);
    this->reset_blocks2(this->skelanim_blocks);
    this->reset_blocks2(this->texture_blocks);
    this->reset_blocks2(this->material_blocks);
    this->reset_blocks2(this->mesh_data_blocks);
    this->reset_blocks2(this->uvanim_blocks);
}

awd_uint32
AWD::flush(int out_fd)
{
    //contain all blocks that are going to be written in the main AWD file.
    AWDBlockList * blocks_mainAWD = new AWDBlockList();

    //make shure the metadata block exists, because no other awdblock should have the id=0
    if (this->metadata==NULL)
        this->metadata=new AWDMetaData();
    this->metadata->prepare_and_add_with_dependencies(blocks_mainAWD);

    char * cur_filename=NULL;
    /*
    if (this->splitByRootObjs){
        AWDBlockList * blocks_splittedAWD;
        blocks_splittedAWD = new AWDBlockList();
        AWDBlock *block;
        AWDBlockIterator it(this->scene_blocks);
        int objCount=0;
        while ((block = it.next()) != NULL) {
            if  (((AWDSceneBlock*)block)->get_parent()==NULL){
                this->reset_all_blocks();
                blocks_splittedAWD = new AWDBlockList();
                this->metadata->isExportedToFile=false;
                this->metadata->isExported=false;
                this->metadata->prepare_and_add_with_dependencies(blocks_splittedAWD);
                this->re_order_blocks(this->namespace_blocks, blocks_splittedAWD);
                this->flatten_scene((AWDSceneBlock*)block, blocks_splittedAWD);
                // Open file and check for success
                char awdDrive[4];
                char awdPath[1024];
                char awdName[256];
                char outAWDPath[1024];
                _splitpath_s(this->outPath, awdDrive, 4, awdPath, 1024, awdName, 256, NULL, 0);
                _makepath_s(outAWDPath, 1024, awdDrive, awdPath, ((AWDSceneBlock*)block)->get_name(), "awd");
                int fd = open(outAWDPath, _O_TRUNC | _O_CREAT | _O_BINARY | _O_RDWR, _S_IWRITE);
                // to do: add error report for failing to export one of multiple files.
                // atm a error on creating a file is  skipped
                if (fd != -1){
                    cur_filename=outAWDPath;
                    this->write_blocks_to_file(fd, blocks_splittedAWD);
                }
            }
        }
    }
    */
    //else{
    this->re_order_blocks(this->namespace_blocks, blocks_mainAWD);
    this->reorder_scene(this->scene_blocks, blocks_mainAWD);
    //}

    this->reset_all_blocks2();
    this->re_order_blocks(this->animator_blocks, blocks_mainAWD);
    this->re_order_blocks(this->skeleton_blocks, blocks_mainAWD);
    this->re_order_blocks(this->skelpose_blocks, blocks_mainAWD);
    this->re_order_blocks(this->skelanim_blocks, blocks_mainAWD);
    //this->re_order_blocks(this->texture_blocks, blocks_mainAWD);
    this->re_order_blocks(this->material_blocks, blocks_mainAWD);
    this->re_order_blocks(this->mesh_data_blocks, blocks_mainAWD);
    this->re_order_blocks(this->uvanim_blocks, blocks_mainAWD);
    this->re_order_blocks(this->lightPicker_blocks, blocks_mainAWD);
    //this->re_order_blocks(this->vertex_anim_blocks, blocks_mainAWD);
    this->re_order_blocks(this->amin_set_blocks, blocks_mainAWD);
    this->re_order_blocks(this->effect_method_blocks, blocks_mainAWD);

    // write the main file
    if (blocks_mainAWD->get_num_blocks()>=1){
        cur_filename=this->outPath;
        this->write_blocks_to_file(out_fd, blocks_mainAWD);
    }

    this->check_exported_blocks(this->animator_blocks);
    this->check_exported_blocks(this->skeleton_blocks);
    this->check_exported_blocks(this->skelpose_blocks);
    this->check_exported_blocks(this->skelanim_blocks);
    this->check_exported_blocks(this->texture_blocks);
    this->check_exported_blocks(this->material_blocks);
    this->check_exported_blocks(this->mesh_data_blocks);
    this->check_exported_blocks(this->uvanim_blocks);
    this->check_exported_blocks(this->lightPicker_blocks);
    this->check_exported_blocks(this->vertex_anim_blocks);
    this->check_exported_blocks(this->amin_set_blocks);
    this->check_exported_blocks(this->effect_method_blocks);

    delete blocks_mainAWD;
    //open the last stored file with the default appliction (e.g. Awaybuilder)

    return AWD_TRUE;
}

AWDBlockList *
AWD::get_skeleton_blocks()
{
    return this->skeleton_blocks;
}
AWDBlockList *
AWD::get_amin_set_blocks()
{
    return this->amin_set_blocks;
}
AWDBlockList *
AWD::get_animator_blocks()
{
    return this->animator_blocks;
}
AWDBlockList *
AWD::get_mesh_data_blocks()
{
    return this->mesh_data_blocks;
}
AWDBlockList *
AWD::get_material_blocks()
{
    return this->material_blocks;
}
AWDBlockList *
AWD::get_message_blocks()
{
    return this->message_blocks;
}
AWDBlockList *
AWD::get_scene_blocks()
{
    return this->scene_blocks;
}

awd_uint32
AWD::write_blocks_to_file(int out_fd, AWDBlockList *blocks)
{
    int tmp_fd;
    char *tmp_path=NULL;

    off_t tmp_len;
    awd_uint8 *tmp_buf;

    awd_uint8 *body_buf;
    awd_uint32 body_len;

    tmp_len = 0;
    tmp_fd = awdutil_mktmp(&tmp_path);
    if (tmp_fd < 0) {
        extern int errno;
        printf("Could not open temporary file necessary for writing, errno=%d\n", errno);
        return AWD_FALSE;
    }
    int sds=(int)this->write_blocks(blocks, tmp_fd);
    tmp_len += sds;

    tmp_buf = (awd_uint8 *) malloc(tmp_len);
    lseek(tmp_fd, 0, SEEK_SET);
    read(tmp_fd, tmp_buf, tmp_len);

    // Temp file no longer needed
    close(tmp_fd);
    unlink((const char *)tmp_path);

    if (this->compression == UNCOMPRESSED) {
        // Uncompressed, so output should be the exact
        // same data that was in the temporary file
        body_len = tmp_len;
        body_buf = tmp_buf;
    }
    else if (this->compression == DEFLATE) {
        int stat;
        z_streamp zstrm;
        int zlib_len;
        awd_uint8 *zlib_buf;
        bool done;

        zlib_len = tmp_len;
        zlib_buf = (awd_uint8*)malloc(zlib_len);

        zstrm = (z_streamp)malloc(sizeof(z_stream_s));
        zstrm->zalloc = awd_zalloc;
        zstrm->zfree = awd_zfree;
        zstrm->opaque = NULL;
        zstrm->next_in = tmp_buf;
        zstrm->avail_in = tmp_len;
        zstrm->next_out = zlib_buf;
        zstrm->avail_out = zlib_len;

        stat = deflateInit(zstrm, 9);

        done = false;
        while (!done) {
            stat = deflate(zstrm, Z_NO_FLUSH);

            switch (stat) {
                case Z_STREAM_END:
                case Z_BUF_ERROR:
                    done = true;
                    break;
            }
        }

        deflate(zstrm, Z_FINISH);
        deflateEnd(zstrm);

        body_len = zstrm->total_out;
        body_buf = (awd_uint8*)malloc(body_len);
        memcpy(body_buf, zlib_buf, body_len);

        free(zlib_buf);
        free(zstrm);
    }
    else if (this->compression == LZMA) {
        Byte *lzma_buf;
        SizeT lzma_len, props_len;
        CLzmaEncProps props;
        ISzAlloc alloc;
        Byte *props_buf;
        long tmp_len_bo;

        // Create allocation structure. LZMA library uses
        // these functions for memory management. They are
        // defined in awdlzma.c as simple wrappers for
        // malloc() and free().
        alloc.Alloc = &awd_SzAlloc;
        alloc.Free = &awd_SzFree;

        lzma_len = tmp_len;
        lzma_buf = (Byte *)malloc(tmp_len);
        props_len = sizeof(CLzmaEncProps);
        props_buf = (Byte *)malloc(props_len);

        LzmaEncProps_Init(&props);
        props.algo = 1;
        props.level = 9;

        int result=LzmaEncode(lzma_buf, &lzma_len, tmp_buf, tmp_len, &props, props_buf, &props_len, 0, NULL, &alloc, &alloc);
        if (result!=SZ_OK)
            int error=0;

        // Body length is the length of the actual
        // compressed body + size of props and an integer
        // definining the uncompressed length (see below)
        body_len = (awd_uint32)lzma_len + (awd_uint32)props_len + sizeof(awd_uint32);

        // Create new buffer containing LZMA props, length
        // of uncompressed body and the actual body data
        // concatenated together.
        tmp_len_bo = UI32(tmp_len);
        body_buf = (awd_uint8*)malloc(body_len);
        memcpy(body_buf, props_buf, props_len);
        memcpy(body_buf, &tmp_len_bo, sizeof(awd_uint32));
        memcpy(body_buf, lzma_buf, lzma_len);
    }

    // Write header and then body from possibly
    // compressed buffer
    this->write_header(out_fd, body_len);

    write(out_fd, body_buf, body_len);
    if (body_buf!=tmp_buf)
        free(tmp_buf);
    free (body_buf);
    if (tmp_path!=NULL)
        free(tmp_path);
    return AWD_TRUE;
}
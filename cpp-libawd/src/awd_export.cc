
#include "awd.h"

AWDLightPicker * 
AWD::CreateDarkLightPicker()
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
    if (blocks_mainAWD->get_num_blocks()>1){
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
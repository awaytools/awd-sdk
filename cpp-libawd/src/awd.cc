#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "platform.h"

#include "awd.h"
#include "util.h"
#include "awdlzma.h"
#include "awdzlib.h"

#include "Types.h"
#include "LzmaEnc.h"


const int AWD::VERSION_MAJOR = 2;
const int AWD::VERSION_MINOR = 0;
const int AWD::VERSION_BUILD = 0;
const char AWD::VERSION_RELEASE = 'a';



AWD::AWD(AWD_compression compression, awd_uint16 flags)
{
    this->major_version = VERSION_MAJOR;
    this->minor_version = VERSION_MINOR;
    this->compression = compression;
    this->flags = flags;
    this->texture_blocks = new AWDBlockList();
    this->cubetex_blocks = new AWDBlockList();
    this->material_blocks = new AWDBlockList();
    this->mesh_data_blocks = new AWDBlockList();
    this->skeleton_blocks = new AWDBlockList();
    this->skelanim_blocks = new AWDBlockList();
    this->skelpose_blocks = new AWDBlockList();
    this->uvanim_blocks = new AWDBlockList();
    this->scene_blocks = new AWDBlockList();

    this->namespace_blocks = new AWDBlockList();

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
    delete this->scene_blocks;
    delete this->namespace_blocks;
}



bool
AWD::has_flag(int flag)
{
    return ((this->flags & flag) > 0);
}


void
AWD::set_metadata(AWDMetaData *block)
{
    this->metadata = block;
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
AWD::add_mesh_data(AWDTriGeom *block)
{
    this->mesh_data_blocks->append(block);
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

size_t
AWD::write_blocks(AWDBlockList *blocks, int fd)
{
    size_t len;
    AWDBlock *block;
    AWDBlockIterator it(blocks);

    len = 0;
    while ((block = it.next()) != NULL) {
        len += block->write_block(fd, ++this->last_used_baddr);
    }

    return len;
}


void
AWD::flatten_scene(AWDSceneBlock *cur, AWDBlockList *flat_list)
{
    AWDBlock *child;
    AWDBlockIterator *children;

    flat_list->append(cur);

    children = cur->child_iter();
    while ((child = children->next()) != NULL) {
        this->flatten_scene((AWDSceneBlock*)child, flat_list);
    }
}

size_t
AWD::write_scene(AWDBlockList *blocks, int fd)
{
    AWDBlock *block;
    AWDBlockList *ordered;
    AWDBlockIterator it(blocks);

    ordered = new AWDBlockList();

    while ((block = it.next()) != NULL) {
        this->flatten_scene((AWDSceneBlock*)block, ordered);
    }

    return this->write_blocks(ordered, fd);
}


awd_uint32
AWD::flush(int out_fd)
{
    int tmp_fd;
    char *tmp_path;

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

    if (this->metadata) {
        tmp_len += this->metadata->write_block(tmp_fd, ++this->last_used_baddr);
    }

    tmp_len += this->write_blocks(this->namespace_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->skeleton_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->skelpose_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->skelanim_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->texture_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->material_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->mesh_data_blocks, tmp_fd);
    tmp_len += this->write_blocks(this->uvanim_blocks, tmp_fd);
    tmp_len += this->write_scene(this->scene_blocks, tmp_fd);

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

        LzmaEncode(lzma_buf, &lzma_len, tmp_buf, tmp_len,
            &props, props_buf, &props_len, 0, NULL, &alloc, &alloc);

        // Body length is the length of the actual
        // compressed body + size of props and an integer
        // definining the uncompressed length (see below)
        body_len = (awd_uint32)lzma_len + (awd_uint32)props_len + sizeof(awd_uint32);

        // Create new buffer containing LZMA props, length
        // of uncompressed body and the actual body data
        // concatenated together.
        tmp_len_bo = UI32(tmp_len);
        body_buf = (awd_uint8*)malloc(body_len);
        memcpy(body_buf, &tmp_len_bo, sizeof(awd_uint32));
        memcpy(body_buf+sizeof(awd_uint32), props_buf, props_len);
        memcpy(body_buf+props_len+sizeof(awd_uint32), lzma_buf, lzma_len);
    }


    // Write header and then body from possibly
    // compressed buffer
    if (this->header_written == AWD_FALSE) {
        this->header_written = AWD_TRUE;
        this->write_header(out_fd, body_len);
    }

    write(out_fd, body_buf, body_len);

    return AWD_TRUE;
}





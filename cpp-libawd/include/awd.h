#ifndef _LIBAWD_AWD_H
#define _LIBAWD_AWD_H

#include "awd_types.h"
#include "ns.h"
#include "block.h"
#include "attr.h"
#include "material.h"
#include "mesh.h"
#include "skeleton.h"
#include "skelanim.h"
#include "texture.h"
#include "camera.h"
#include "uvanim.h"
#include "scene.h"
#include "meta.h"


#define AWD_STREAMING               0x1


class AWD
{
    private:
        // File header fields
        awd_uint8 major_version;
        awd_uint8 minor_version;
        awd_uint16 flags;
        AWD_compression compression;

        AWDMetaData *metadata;

        AWDBlockList * namespace_blocks;
        AWDBlockList * texture_blocks;
        AWDBlockList * cubetex_blocks;
        AWDBlockList * material_blocks;
        AWDBlockList * skelpose_blocks;
        AWDBlockList * skelanim_blocks;
        AWDBlockList * skeleton_blocks;
        AWDBlockList * mesh_data_blocks;
        AWDBlockList * uvanim_blocks;
        AWDBlockList * scene_blocks;

        // Flags and misc
        awd_baddr last_used_baddr;
        awd_nsid last_used_nsid;
        awd_bool header_written;

        void write_header(int, awd_uint32);
        void flatten_scene(AWDSceneBlock *, AWDBlockList *);
        size_t write_scene(AWDBlockList *, int);
        size_t write_blocks(AWDBlockList *, int);

    public:
        AWD(AWD_compression, awd_uint16);
        ~AWD();
        awd_uint32 flush(int);

        bool has_flag(int);

        static const int VERSION_MAJOR;
        static const int VERSION_MINOR;
        static const int VERSION_BUILD;
        static const char VERSION_RELEASE;

        void set_metadata(AWDMetaData *);

        void add_texture(AWDBitmapTexture *);
        void add_cube_texture(AWDCubeTexture *);
        void add_material(AWDMaterial *);
        void add_mesh_data(AWDTriGeom *);
        void add_skeleton(AWDSkeleton *);
        void add_skeleton_pose(AWDSkeletonPose *);
        void add_skeleton_anim(AWDSkeletonAnimation *);
        void add_uv_anim(AWDUVAnimation *);
        void add_scene_block(AWDSceneBlock *);

        void add_namespace(AWDNamespace *);
        AWDNamespace *get_namespace(const char *);
};


#endif

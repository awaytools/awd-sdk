#ifndef _LIBAWD_AWD_H
#define _LIBAWD_AWD_H

#include "awd_types.h"
#include "ns.h"
#include "block.h"
#include "attr.h"
#include "material.h"
#include "shadows.h"
#include "mesh.h"
#include "skeleton.h"
#include "skelanim.h"
#include "texture.h"
#include "camera.h"
#include "anim.h"
#include "uvanim.h"
#include "vertexanim.h"
#include "scene.h"
#include "meta.h"
#include "light.h"
#include "primitive.h"
#include "method.h"
#include "util.h"
#include "geomutil.h"
#include "message.h"

#define AWD_STREAMING               0x1

class AWD
{
    private:
        // File header fields
        awd_uint8 major_version;
        awd_uint8 minor_version;
        awd_uint16 flags;
        AWD_compression compression;
        bool splitByRootObjs;
        bool exportEmtpyContainers;
        char *outPath;
        BlockSettings * thisBlockSettings;
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
        AWDBlockList * lightPicker_blocks;
        AWDBlockList * animator_blocks;
        AWDBlockList * prim_blocks;
        AWDBlockList * amin_set_blocks;
        AWDBlockList * vertex_anim_blocks;
        AWDBlockList * effect_method_blocks;
        AWDBlockList * command_blocks;
        AWDBlockList * shadow_blocks;
        AWDBlockList * message_blocks;
        AWDLightPicker * darkLightPicker;

        // Flags and misc
        awd_baddr last_used_baddr;
        awd_nsid last_used_nsid;
        awd_bool header_written;

        void write_header(int, awd_uint32);
        void flatten_scene(AWDSceneBlock *, AWDBlockList *);
        void reorder_scene(AWDBlockList *, AWDBlockList *);
        size_t write_blocks(AWDBlockList *, int);
        void re_order_blocks(AWDBlockList *, AWDBlockList *);
        void reset_blocks(AWDBlockList *);
        void reset_all_blocks();
        void reset_blocks2(AWDBlockList *);
        void reset_all_blocks2();
        int get_root_objs_count(AWDBlockList *);
        void check_exported_blocks(AWDBlockList *);

    public:
        AWD(AWD_compression, awd_uint16, char *, bool, BlockSettings *, bool);
        ~AWD();
        awd_uint32 flush(int);
        awd_uint32 write_blocks_to_file(int, AWDBlockList *);

        bool has_flag(int);

        static const int VERSION_MAJOR;
        static const int VERSION_MINOR;
        static const int VERSION_BUILD;
        static const char VERSION_RELEASE;

        void set_metadata(AWDMetaData *);

        int count_all_valid_blocks();
        void add_texture(AWDBitmapTexture *);
        void set_out_path(char *);
        void add_shadow(AWDShadowMethod *);
        void add_cube_texture(AWDCubeTexture *);
        void add_material(AWDMaterial *);
        void add_mesh_data(AWDBlock *);
        void add_skeleton(AWDSkeleton *);
        void add_skeleton_pose(AWDSkeletonPose *);
        void add_skeleton_anim(AWDSkeletonAnimation *);
        void add_animator(AWDAnimator *);
        void add_uv_anim(AWDUVAnimation *);
        void add_scene_block(AWDSceneBlock *);
        void add_light_picker_block(AWDLightPicker *);
        void add_prim_block(AWDPrimitive *);
        void add_amin_set_block(AWDAnimationSet *);
        void add_vertex_anim_block(AWDVertexAnimation *);
        void add_effect_method_block(AWDEffectMethod *);
        AWDLightPicker * CreateDarkLightPicker();

        void add_namespace(AWDNamespace *);
        AWDNamespace *get_namespace(const char *);
        AWDBlockList * get_skeleton_blocks();
        AWDBlockList * get_animator_blocks();
        AWDBlockList * get_amin_set_blocks();
        AWDBlockList * get_mesh_data_blocks();
        AWDBlockList * get_material_blocks();
        AWDBlockList * get_message_blocks();
        AWDBlockList * get_scene_blocks();
        
};

#endif

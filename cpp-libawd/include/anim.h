#ifndef _LIBAWD_ANIM_H
#define _LIBAWD_ANIM_H

#include "awd_types.h"
#include "material.h"
#include "scene.h"
#include "stream.h"
#include "block.h"
#include "name.h"
#include "attr.h"
typedef enum {
    ANIMTYPESKELETON=1,
    ANIMTYPEVERTEX,
    ANIMTYPEUV,
} AWD_Anim_Type;

class AWDAnimationSet : public AWDNamedElement,
    public AWDAttrElement, public AWDBlock
{
    private:
        char * sourcePreID;
        int sourcePreID_len;
        AWDBlock * target_geom;
        AWD_Anim_Type animSet_type;
        AWDBlockList * preAnimationClipNodes;
        AWDBlockList * animationClipNodes;
        bool is_processed;
        bool simple_mode;
        int hirarchyString_len;
        int originalPointCnt;
        AWDSkeleton * skeletonBlock;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int,  BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDAnimationSet(const char *, awd_uint16, AWD_Anim_Type, const char *, awd_uint16, AWDBlockList *);
        ~AWDAnimationSet();

        void set_sourcePreID(const char *, awd_uint16);
        char * get_sourcePreID();
        AWD_Anim_Type get_anim_type();
        void set_anim_type(AWD_Anim_Type);
        bool get_is_processed();
        void set_is_processed(bool);
        AWDBlock * get_target_geom();
        void set_target_geom(AWDBlock *);
        bool get_simple_mode();
        void set_simple_mode(bool);
        int get_originalPointCnt();
        void set_originalPointCnt(int);
        AWDSkeleton * get_skeleton();
        void set_skeleton(AWDSkeleton *);
        AWDBlockList * get_preAnimationClipNodes();
        void set_preAnimationClipNodes(AWDBlockList *);
        AWDBlockList * get_animationClipNodes();
        void set_animationClipNodes(AWDBlockList *);
};

class AWDAnimator :
    public AWDNamedElement, public AWDAttrElement, public AWDBlock
{
    private:
        AWD_Anim_Type animator_type;
        AWDAnimationSet * animationSet;
        AWDBlockList * animator_targets;
        AWDNumAttrList *animatorProperties;
        AWDSkeleton * skeletonBlock;

    protected:
        awd_uint32 calc_body_length(BlockSettings *);
        void write_body(int, BlockSettings *);
        void prepare_and_add_dependencies(AWDBlockList *export_list);

    public:
        AWDAnimator(const char *, awd_uint16, AWDAnimationSet *, AWD_Anim_Type);
        ~AWDAnimator();

        void add_target(AWDMeshInst *);
        AWDBlockList * get_targets();
        AWDAnimationSet * get_animationSet();
        void set_animationSet(AWDAnimationSet *);
        AWDSkeleton * get_skeleton();
        void set_skeleton(AWDSkeleton *);
        AWD_Anim_Type get_anim_type();
        void set_anim_type(AWD_Anim_Type);
};
#endif
#ifndef _LIBAWD_ANIMATOR_H
#define _LIBAWD_ANIMATOR_H

#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "blocks/skeleton.h"
#include "blocks/anim_set.h"
#include "blocks/mesh_inst.h"
#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{
	
	
		/** \class Animator
		*	\brief Descripes a AWDBlock for [Away3D Animator](http://www.away3d.com)  as defined by AWDSpecs: \ref awd_pt2_14_6
		*
		*	Animator connects a AnimationSet to a list of meshes. 
		*
		*/
		class Animator :
			 
			public BASE::AttrElementBase, 
			public BASE::AWDBlock
		{
			private:
				ANIM::anim_type animator_type;
				AnimationSet * animationSet;
				std::vector<BASE::AWDBlock*> animator_targets;
				ATTR::NumAttrList *animatorProperties;
				Skeleton * skeletonBlock;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);				
				TYPES::state validate_block_state();
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);

			public:
				Animator(std::string&, AnimationSet *, ANIM::anim_type);
				Animator();
				~Animator();

				void add_target(Mesh *);
				std::vector<BASE::AWDBlock*>& get_targets();
				AnimationSet * get_animationSet();
				void set_animationSet(AnimationSet *);
				Skeleton * get_skeleton();
				void set_skeleton(Skeleton *);
				ANIM::anim_type get_anim_type();
				void set_anim_type(ANIM::anim_type);
		};
	}
}
#endif
#ifndef _LIBAWD_ANIMSET_H
#define _LIBAWD_ANIMSET_H

#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"
#include "utils/settings.h"
#include "blocks/skeleton.h"
#include "base/state_element_base.h"

namespace AWD
{
	namespace BLOCKS
	{
		/** \class AnimationSet
		*	\brief Descripes a AWDBlock for [Away3D AnimationSet](http://www.away3d.com) as defined by AWDSpecs: \ref awd_pt2_14_5
		*
		* A AnimationSet has a anim_type property, and all contained AWDAnimationClips must be of the correspoinding anim_type.
		*/
		class AnimationSet : 
			
			public BASE::AttrElementBase, 
			public BASE::AWDBlock

		{
			private:
				std::string sourcePreID;

				AWDBlock * target_geom;
				ANIM::anim_type animSet_type;
				std::vector<AWDBlock*> preAnimationClipNodes;
				std::vector<AWDBlock*> animationClipNodes;
				bool is_processed;
				bool simple_mode;
				int hirarchyString_len;
				int originalPointCnt;
				Skeleton * skeletonBlock;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);

			public:
				AnimationSet(std::string&, ANIM::anim_type, std::string&, std::vector<AWDBlock*>& );
				AnimationSet();
				~AnimationSet();

				void set_sourcePreID(std::string&);
				std::string& get_sourcePreID();
				ANIM::anim_type get_anim_type();
				void set_anim_type(ANIM::anim_type);
				bool get_is_processed();
				void set_is_processed(bool);
				BASE::AWDBlock * get_target_geom();
				void set_target_geom(BASE::AWDBlock *);
				bool get_simple_mode();
				void set_simple_mode(bool);
				int get_originalPointCnt();
				void set_originalPointCnt(int);
				Skeleton * get_skeleton();
				void set_skeleton(Skeleton *);
				std::vector<AWDBlock*>& get_preAnimationClipNodes();
				void set_preAnimationClipNodes(std::vector<AWDBlock*>&);
				std::vector<AWDBlock*>& get_animationClipNodes();
				void set_animationClipNodes(std::vector<AWDBlock*>&);
		};	
	}
}
#endif
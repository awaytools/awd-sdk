#ifndef _LIBAWD_SKELETON_JOINT_H
#define _LIBAWD_SKELETON_JOINT_H

#include <vector>
#include "utils/awd_types.h"
#include "base/attr.h"

#include "base/block.h"
#include "utils/settings.h"

#include "files/file_writer.h"


namespace AWD
{
	namespace ANIM
	{
		
		/**\brief SkeletonJoint represents one bone inside a Skeleton
		*
		* 
		*/
		class SkeletonJoint :
			
			public BASE::AttrElementBase
		{
			private:
				TYPES::UINT16 id;
				TYPES::F64 *bind_mtx;
				int num_children;

				SkeletonJoint *parent;
				std::vector<SkeletonJoint*> childs;

			public:
				SkeletonJoint(std::string&, TYPES::F64 *);
				~SkeletonJoint();

				int write_joint(FILES::FileWriter*, TYPES::UINT32, SETTINGS::BlockSettings * );
				int calc_length(SETTINGS::BlockSettings * );
				int calc_num_children();
				TYPES::UINT32 get_id();

				void set_bind_mtx(TYPES::F64 *);
				TYPES::F64 *get_bind_mtx();

				void set_parent(SkeletonJoint *);
				SkeletonJoint *get_parent();
				SkeletonJoint *add_child_joint(SkeletonJoint *);
		};
	}
}
#endif

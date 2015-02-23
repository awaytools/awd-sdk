#ifndef _LIBAWD_SCENE_BASE_H
#define _LIBAWD_SCENE_BASE_H

#include <vector>
#include "utils/awd_types.h"

#include "base/block.h"
#include "base/attr.h"
#include "files/awd_file.h"
#include "files/file_writer.h"
#include "files/file_reader.h"
#include "elements/geom_elements.h"

namespace AWD
{
	namespace BASE
	{	
		/** \class SceneBlockBase
		*	\brief Provide properties to define scenegraph-information for a block. Should never be instantiated directly.
		*/
		class SceneBlockBase :
			public AWDBlock,
			public AttrElementBase
		{
			private:
				GEOM::MATRIX4x3 * transform_mtx;
				BASE::SceneBlockBase *parent;
				std::vector<BASE::AWDBlock*> children;

			protected:
				result write_scene_common(FILES::FileWriter* , SETTINGS::BlockSettings *, FILES::AWDFile* awd_file);
				TYPES::UINT32 calc_common_length(bool);

			public:
				SceneBlockBase(BLOCK::block_type, const std::string&, TYPES::F64 *);
				~SceneBlockBase();
				
				result add_children_to_file(FILES::AWDFile* awd_file, BLOCK::instance_type instance_type);

				void set_transform(TYPES::F64 *);
				GEOM::MATRIX4x3 * get_transform();

				BASE::AWDBlock *get_parent();
				void set_parent(BASE::SceneBlockBase *);
				bool isEmpty();

				void add_child(BASE::SceneBlockBase *);
				std::vector<AWDBlock*>& get_children();
				/**
				*\todo is not implemented yet.
				*/
				void remove_child(BASE::SceneBlockBase *);

				/**
				*\todo is not implemented yet. (Do we need it ?)
				*/
				void make_children_invalide();

		};	
	}
}
#endif

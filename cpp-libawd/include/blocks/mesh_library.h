#ifndef _LIBAWD_MESHLIBRARY_H
#define _LIBAWD_MESHLIBRARY_H
#include <map>
#include <string>
#include "utils/awd_types.h"
#include "base/scene_base.h"
#include "base/attr.h"

#include "base/state_element_base.h"


namespace AWD
{
	namespace BLOCKS
	{	
	
		/** \class MeshLibrary
		*	\brief Descripes a AWDBlock for [Away3D MeshLibrary](http://www.away3d.com)  as defined by AWDSpecs: \ref awd_pt2_12_4
		*
		*/
		class MeshLibrary:
			public AWDBlock,
			public AttrElementBase
		{
			private:
				BASE::AWDBlock * geom;
				std::map<TYPES::SUBGEOM_ID_STRING, AWDBlock*> available_materials;
				BASE::AWDBlock * lightPicker;
				BASE::AWDBlock * animator;
				BASE::AWDBlock * defaultMat;
				void init();

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();				
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				MeshLibrary(std::string&, BASE::AWDBlock *);
				MeshLibrary(std::string&, BASE::AWDBlock *, TYPES::F64 *);
				MeshLibrary();
				~MeshLibrary();
				
				std::vector<AWDBlock*> materials;

				/**
				*\brief Clear the list of Material for this MeshLibrary. Should only be called from #GEOM::ProcessGeometry().
				*/
				void clear_final_materials();
				
				void createSlice9Scale(TYPES::F32 x, TYPES::F32 y, TYPES::F32 width, TYPES::F32 height);
				/**
				*\brief Add a Material to this MeshLibrary. Should only be called from #GEOM::ProcessGeometry().
				*@param[in] final_mat BASE::AWDBlock Pointer to the Material to add.
				*/
				void add_final_material(BASE::AWDBlock * final_mat);
				
				BASE::AWDBlock* get_material_by_face_id(const TYPES::SUBGEOM_ID_STRING&);	

				/**
				*\brief Get the id for a faceGroup, when the FaceGroups should be merged by material.
				*/
				void get_merged_face_id(const TYPES::SUBGEOM_ID_STRING&, TYPES::SUBGEOM_ID_STRING&);				
				
				/**
				*\brief Get the Material that should be used as default Material for this MeshLibrary.
				*
				*If no material is set as DefaultMaterial, the lib will look for the first material set in the material-map.
				*If no material is set to the material-map, the default-material set for the AWDFile will be used.
				*@return geom BASE::AWDBlock Pointer to the Material, that should be used as default Material for this MeshLibrary.
				*/
				BASE::AWDBlock * get_defaultMat();

				/**
				*\brief Set the Material that should be used as default Material for this MeshLibrary.
				*
				*If no material is set as DefaultMaterial, the lib will look for the first material set in the material-map.
				*If no material is set to the material-map, the default-material set for the AWDFile will be used.
				*@param[in] default_mat BASE::AWDBlock Pointer to the Material to use as default Material for this MeshLibrary.
				*/
				void set_defaultMat(BASE::AWDBlock * default_mat);

				
				/**
				*\brief Get a Geometry for this MeshLibrary.
				*@return geom BASE::AWDBlock Pointer to the Geometry to set.
				*/
				BASE::AWDBlock * get_geom();
				
				/**
				*\brief Set a Geometry for this MeshLibrary. Should only be called from #GEOM::ProcessGeometry().
				*@param[in] geom BASE::AWDBlock Pointer to the Geometry to set.
				*/
				void set_geom(BASE::AWDBlock * geom);
				BASE::AWDBlock* get_lightPicker();
				void set_lightPicker(BASE::AWDBlock *);
				void set_animator(BASE::AWDBlock *);
				BASE::AWDBlock *get_animator();
		};
	}
}
#endif

#ifndef _LIBAWD_GEOMETRY_H
#define _LIBAWD_GEOMETRY_H

#include "utils/awd_types.h"
#include "base/block.h"

#include "base/attr.h"

#include "files/file_writer.h"
#include "utils/settings.h"


#include "elements/subgeometry.h"
#include "base/state_element_base.h"
#include "elements/stream_attributes.h"
#include "blocks/material.h"

namespace AWD
{
	typedef std::map<std::string, TYPES::UINT32 >::iterator it_share_vert;
	namespace BLOCKS
	{	
		/** \class Geometry
		*	\brief Descripes a AWDBlock for [Away3D Geometry](http://www.away3d.com)  as defined by AWDSpecs: \ref awd_pt2_11_1   
		*
		* 
		*/
		class Geometry :
			public BASE::AWDBlock,
			
			public BASE::AttrElementBase
		{
			private:
				bool is_created;
				bool split_faces;
				int originalPointCnt;
				unsigned int num_subs;
				std::vector<GEOM::SubGeom *> subGeometries;
				std::vector<BASE::AWDBlock *> meshInstances;
				std::vector<GEOM::Triangle*> awdTriangles;
				std::vector<GEOM::Vertex3D*> awdVertices;
				std::vector<GEOM::FilledRegion*> all_path_shapes;



				std::map<std::string, TYPES::UINT32> shared_verts_idx_map;
				std::vector<GEOM::SharedVertex3D*> shared_verts;

				SETTINGS::BlockSettings* subGeomSettings;
				result add_vertex3D(GEOM::Vertex3D* vert);

				TYPES::F64 * bind_mtx;

			protected:
				TYPES::UINT32 calc_body_length(FILES::AWDFile*, SETTINGS::BlockSettings *);
				TYPES::state validate_block_state();								
				result write_body(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				result read_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* file);
				result collect_dependencies(FILES::AWDFile * target_file, BLOCK::instance_type instance_type);

			public:
				/** 
				*	\brief Descripes a AWDBlock for [Away3D Geometry](http://www.away3d.com) 
				*	@param[in] name std::string& the name of the Geometry.
				* 
				*/
				Geometry(std::string& name);
				Geometry();
				~Geometry();

				result add_path_shape(GEOM::FilledRegion* path_shape);
				
				
				result merge_for_textureatlas(BLOCKS::Material* );
				void clear_mesh_inst_list();

				TYPES::UINT32 get_num_subs();

				GEOM::SubGeom *get_sub_at(TYPES::UINT32);

				void get_material_blocks(std::vector<BASE::AWDBlock*>&);
				
				SETTINGS::BlockSettings* get_sub_geom_settings();
			
				void add_subgeo(GEOM::SubGeom *);
			
				/**
				* \brief Adds a new Triangle to the Geometry
				*	@param[in] triangle The triangle to add
				* If there already exists a Poly or Triangle that contains the three Vertex, the Triangle will not be added.
				*/
				result add_triangle(GEOM::Triangle* triangle);
			

				std::vector<BASE::AWDBlock*>& get_mesh_instances();
				void set_mesh_instances(std::vector<BASE::AWDBlock*>&);
				
				std::vector<GEOM::FilledRegion*>& get_filled_regions();

				std::vector<GEOM::Triangle*>& get_all_triangles();
				void set_all_triangles(std::vector<GEOM::Triangle*>&);

				std::vector<GEOM::Vertex3D*>& get_all_verts();
				void set_all_verts(std::vector<GEOM::Vertex3D*>&);

				std::vector<GEOM::SharedVertex3D*>& get_shared_verts();

				Geometry* get_cloned_geometry();
				void add_mesh_inst(AWDBlock* mesh_inst);
				AWDBlock* get_first_mesh();
				
				TYPES::F64 *get_bind_mtx();
				void set_bind_mtx(TYPES::F64 *bind_mtx);
				bool get_is_created();
				void set_is_created(bool);
				bool get_split_faces();
				void set_split_faces(bool);
				int get_originalPointCnt();
				void set_originalPointCnt(int);
		};	
	}
}
#endif

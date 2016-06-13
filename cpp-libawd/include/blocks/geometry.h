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
				std::vector<BASE::AWDBlock *> meshInstances;
				std::vector<GEOM::Triangle*> awdTriangles;
				std::vector<GEOM::Vertex3D*> awdVertices;
				std::vector<GEOM::FilledRegion*> all_filled_regions;



				std::map<std::string, TYPES::UINT32> shared_verts_idx_map;
				std::vector<GEOM::SharedVertex3D*> shared_verts;

				SETTINGS::BlockSettings* subGeomSettings;
				result add_vertex3D(GEOM::Vertex3D* vert);
				bool delete_subs;
				TYPES::F64 * bind_mtx;
				BASE::AWDBlock* mesh_instance;
				std::vector<GEOM::MATRIX2x3*> frame_cmd_inst_matrices;

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
				Geometry(bool);
				Geometry();
				~Geometry();
				static int subgeomcnt;
				std::vector<GEOM::SubGeom *> subGeometries;
				void add_res_id_geom(const std::string&, GEOM::MATRIX2x3*);
				GEOM::MATRIX2x3* has_res_id_geom(const std::string&);
				void clear_res_ids_geom();
				result add_filled_region(GEOM::FilledRegion* filled_regions);
				
				result merge_stream(GEOM::SubGeom* project);

				result write_uvs(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile*);
				TYPES::UINT32 get_uv_bytesize(FILES::AWDFile*, SETTINGS::BlockSettings *);

				result merge_subgeos();
				result merge_for_textureatlas(BLOCKS::Material*, BLOCKS::Material*, BLOCKS::Material*, BLOCKS::Material* );
				void clear_mesh_inst_list();

				TYPES::UINT32 get_num_subs();

				void set_addr_on_subgeom(FILES::AWDFile* file);

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
				void add_mesh_inst(BASE::AWDBlock* mesh_inst);
				BASE::AWDBlock* get_first_mesh();
				
				void set_mesh_instance(BASE::AWDBlock* mesh_inst);
				BASE::AWDBlock* get_mesh_instance();

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

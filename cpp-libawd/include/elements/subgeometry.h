#ifndef _LIBAWD_SUBGEOMETRY_H
#define _LIBAWD_SUBGEOMETRY_H

#include <vector>
#include <map>
#include <string>
#include "utils/awd_types.h"
#include "base/attr.h"
#include "base/block.h"

#include "elements/stream.h"
#include "elements/stream_attributes.h"
#include "elements/geom_elements.h"

using namespace AWD::BASE;

namespace AWD
{	
	namespace GEOM
	{	
		/** \class SubGeomInternal
		*	\brief A SubGeomInternal contains a list of DataStream.
		* 
		*/
		class SubGeomInternal 
		{
			private:
				std::vector<GEOM::GeomStreamElementBase*> vertices;
				std::vector<GEOM::Triangle*> triangles;
				std::map<std::string, TYPES::UINT32> vertex_cache;

			public:
				/**\brief SubGeomInternal only holds a list of verts and a list of triangles. One Subgeometry can have multiple SubGeom
				*
				*/
				SubGeomInternal();
				~SubGeomInternal();

				TYPES::UINT32 get_tri_cnt();		
				TYPES::UINT32 get_vert_cnt();			
				TYPES::UINT32 add_vertex_with_cache(GEOM::GeomStreamElementBase*);
				TYPES::UINT32 add_vertex(GEOM::GeomStreamElementBase*);
				TYPES::UINT32 add_triangle(GEOM::Triangle*);
				std::vector<GEOM::GeomStreamElementBase*>& get_vertices();	
				void set_vertices(std::vector<GEOM::GeomStreamElementBase*>&);				
				std::vector<GEOM::Triangle*>& get_triangles();
				void set_triangles(std::vector<GEOM::Triangle*>&);	
		};
	

		/** \class SubGeom
		*	\brief Holds a collection of SubGeomInternal.
		* This class acts as single SubGeometry, while it can consist of multiple SubGeomInternal, in case the ressource limits are reached for a SubGeomInternal.
		* 
		*/
		class SubGeom:
			public BASE::AttrElementBase,
			BASE::StateElementBase
		{
			private:
				std::vector<GEOM::SubGeomInternal*> sub_geoms;
				SETTINGS::BlockSettings* settings;
				TYPES::UINT32 tri_cnt;
				TYPES::F64 max_x;
				TYPES::F64 max_y;
				TYPES::F64 min_x;
				TYPES::F64 min_y;
				
				
			protected:
				TYPES::state validate_state();

			public:
				SubGeom(SETTINGS::BlockSettings *);
				~SubGeom();
				
				BASE::AWDBlock* material_block;
				TYPES::UINT32 allowed_tris;
				result merge_subgeo(SubGeom*);
				result set_uvs();
				TYPES::UINT32 get_tri_cnt();
				void set_material(BASE::AWDBlock*);
				void get_material_blocks(std::vector<BASE::AWDBlock*>& material_blocks);

				void get_internal_id(std::string&);

				TYPES::INT32 add_triangle(GEOM::Triangle*);
				bool isClockWiseXY(double point1x, double point1y, double point2x, double point2y, double point3x, double point3y);
				result create_triangle(GEOM::edge_type edge_type, GEOM::VECTOR2D v1, GEOM::VECTOR2D v2, GEOM::VECTOR2D v3);
				
				TYPES::INT32 add_vertex2D(GEOM::Vertex2D* vertex);
				TYPES::INT32 add_vertex(GEOM::Vertex3D*);
				
				void set_settings(SETTINGS::BlockSettings * settings);

				SETTINGS::BlockSettings * get_settings();
				
				TYPES::UINT32 calc_subgeom_streams_length(GEOM::SubGeomInternal* subGeo);
				
				TYPES::UINT16 get_num_subs();

				result write_subgeom_streams(FILES::FileWriter* filewWriter, GEOM::SubGeomInternal* subGeo);

				result write_vertex_stream(FILES::FileWriter* filewWriter, DataStreamRecipe* datastream, GEOM::SubGeomInternal* subGeo);

				result write_triangle_stream(FILES::FileWriter* filewWriter, DataStreamRecipe* datastream, GEOM::SubGeomInternal* subGeo);

				TYPES::UINT32 calc_stream_length(DataStreamRecipe* datastream, GEOM::SubGeomInternal* subGeom);

				TYPES::UINT32 get_max_stream_attributes_length(GEOM::stream_target stream_target_type);
				
				TYPES::UINT32 calc_sub_length();

				result write_sub(FILES::FileWriter*);				

				std::vector<GEOM::SubGeomInternal*>& get_sub_geoms();
				void set_sub_geoms(std::vector<GEOM::SubGeomInternal*>&);
		};
	}	
}
#endif

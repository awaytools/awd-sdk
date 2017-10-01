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
		class SubShapePath
		{


			public:
				SubShapePath();
				~SubShapePath();

				std::vector<TYPES::F32> path_data;

				int color;
				float thickness;
				int jointStlye;
				int capStyle;
				float mitter;
				TYPES::UINT32 get_bytesize();
				result writeToFile(FILES::FileWriter*);


		};
	
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
				bool isMerged;
				TYPES::UINT32 startIDX;
				TYPES::UINT32 vertCnt;
				
				void createSlice9Scale(TYPES::F32 x, TYPES::F32 y, TYPES::F32 width, TYPES::F32 height, std::vector<GEOM::SubGeomInternal*>& result);

				result modify_font_char(double size);
				std::vector<GEOM::Triangle*> exterior_triangles;
				std::vector<GEOM::Triangle*> interior_triangles;
				std::vector<GEOM::Triangle*> concave_triangles;
				std::vector<GEOM::Triangle*> convex_triangles;
				TYPES::UINT32 get_tri_cnt_for_type(GEOM::edge_type edge_type);
				TYPES::UINT32 get_tri_cnt();
				TYPES::UINT32 get_vert_cnt();
				TYPES::UINT32 add_vertex_with_cache(GEOM::GeomStreamElementBase*);
				TYPES::UINT32 add_vertex(GEOM::GeomStreamElementBase*);
				TYPES::UINT32 add_triangle_by_type(Triangle* triangle, GEOM::edge_type edge_type);
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
				std::string name;
				
				
			protected:
				TYPES::state validate_state();

			public:
				SubGeom(SETTINGS::BlockSettings *);
				~SubGeom();
				
				std::vector<TYPES::INT32> slice9ScaleStream;
				std::vector<TYPES::F32> slice9ScalePositionsStream;
				GEOM::SubGeom* target_subgeom;
				bool isMerged;
				bool isScaled9;
				bool isMerged_refactor;
				TYPES::UINT32 merged_address;
				TYPES::UINT32 startIDX;
				TYPES::UINT32 vertCnt;
				TYPES::UINT32 tri_cnt;
				
				TYPES::F64 max_x;
				TYPES::F64 max_y;
				TYPES::F64 min_x;
				TYPES::F64 min_y;
				TYPES::F64 uint16_offsetX;
				TYPES::F64 uint16_offsetY;
				TYPES::F64 uint16_scaleX;
				TYPES::F64 uint16_scaleY;
				MATERIAL::type mat_type;
				GEOM::MATRIX2x3* uv_transform;
				GEOM::MATRIX2x3* registration_uv_transform;
				/**
				*\brief Get the name.
				*/
				std::string& get_name();

				GEOM::VECTOR2D* uv_tl;
				GEOM::VECTOR2D* uv_br;

				/**
				*\brief Set the name.
				*/
				void set_name(const std::string& name);
				
				result modify_font_char(double size);
				BASE::AWDBlock* material_block;
				TYPES::UINT32 allowed_tris;
				result merge_subgeo(SubGeom*);
				result merge_stream_refactor(GEOM::SubGeom* project);
				result merge_stream(GEOM::SubGeom* project);
				result add_merged_stream_tri(GEOM::edge_type edge_type, GEOM::Vertex2D v1, GEOM::Vertex2D v2, GEOM::Vertex2D v3);
				result set_uvs();
				TYPES::F32 get_y_for_x_between_verts(GEOM::Vertex2D* v1, GEOM::Vertex2D* v2, TYPES::F32 x);
				TYPES::F32 get_x_for_y_between_verts(GEOM::Vertex2D* v1, GEOM::Vertex2D* v2, TYPES::F32 x);
				
				void split_tris_into_horizontal_groups(std::vector<GEOM::Triangle*>& input, std::vector<GEOM::Triangle*>& result1, std::vector<GEOM::Triangle*>& result2, std::vector<GEOM::Triangle*>& result3, TYPES::F32 x, TYPES::F32 width);
				void split_tris_into_vertical_groups(std::vector<GEOM::Triangle*>& input, std::vector<GEOM::Triangle*>& result1, std::vector<GEOM::Triangle*>& result2, std::vector<GEOM::Triangle*>& result3, TYPES::F32 y, TYPES::F32 height);
				void split_tris_into_vertical_groups(TYPES::F32 y, GEOM::Triangle* tri1, GEOM::Triangle* tri2);
				bool split_tri_horizontal(TYPES::F32 x, GEOM::Triangle* tri1, std::vector<GEOM::Triangle*>& tris1, std::vector<GEOM::Triangle*>& tris2);
				bool split_tri_vertical(TYPES::F32 y, GEOM::Triangle* tri1,  std::vector<GEOM::Triangle*>& tris1, std::vector<GEOM::Triangle*>& tris2);

				void createSlice9Scale(TYPES::F32 left, TYPES::F32 top, TYPES::F32 right, TYPES::F32 bottom, TYPES::F32 g_left, TYPES::F32 g_top, TYPES::F32 g_right, TYPES::F32 g_bottom);

				result set_uint16_positions(double tx, double ty, double scaleX, double scaleY, double width, double height);
				result calculate_bounds();
				
				TYPES::UINT32 get_tri_cnt_for_type(GEOM::edge_type edge_type);
				TYPES::UINT32 get_tri_cnt();
				void set_material(BASE::AWDBlock*);
				void get_material_blocks(std::vector<BASE::AWDBlock*>& material_blocks);

				void get_internal_id(std::string&);

				TYPES::INT32 add_triangle(GEOM::Triangle*);
				bool isClockWiseXY(double point1x, double point1y, double point2x, double point2y, double point3x, double point3y);
				result create_triangle(GEOM::edge_type edge_type, GEOM::VECTOR2D v1, GEOM::VECTOR2D v2, GEOM::VECTOR2D v3);
				
				TYPES::INT32 add_vertex2D(GEOM::Vertex2D* vertex, bool use_cache);
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

#ifndef _LIBAWD_STREAM_ATTR_H
#define _LIBAWD_STREAM_ATTR_H
#include <vector>
#include <string>
#include <map>
#include "utils/awd_types.h"
#include "files/file_writer.h"
#include "elements/geom_elements.h"
#include "elements/stream.h"


namespace AWD
{

	namespace GEOM
	{
	
		class GeomStreamElementBase
		{
			private:
			
				TYPES::UINT32 original_index;
				TYPES::UINT32 internal_index;
				GeomStreamElementBase* target_vertex;
				std::string output_matface_id;
				std::string output_subgeom_id;
				std::map<TYPES::SUBGEOM_ID_STRING, TYPES::UINT32> output_indices;
				GEOM::VECTOR3D  face_normal;
				SETTINGS::BlockSettings* v_n_settings;
				GEOM::VECTOR4D  color;	
				bool calculated_normal;	
				std::map< GEOM::data_stream_attr_type, GEOM::DataStreamAttr* > custom_attributes;
				std::map< GEOM::data_stream_attr_type, std::map< std::string, GEOM::DataStreamAttr* > > custom_attributes_channels;	
			protected:
				std::string lookup_string;
				virtual std::string& intern_get_vertex_string()=0;
			public:
				GeomStreamElementBase();
				~GeomStreamElementBase();
				
				std::string& get_vertex_string();
				int get_idx_for_sub_geo_id(TYPES::SUBGEOM_ID_STRING);
				int clear_output_indices(TYPES::SUBGEOM_ID_STRING);
				void set_v_n_settings(SETTINGS::BlockSettings* );
				SETTINGS::BlockSettings*  get_v_n_settings();
				void set_attribute(GEOM::DataStreamAttr* vertexAttribute);
				result get_attribute(GEOM::data_stream_attr_type stream_type, GEOM::DataStreamAttr** );
				void set_attribute_channel(const std::string&, GEOM::DataStreamAttr* vertexAttribute);
				result get_attribute_channel(const std::string&, GEOM::data_stream_attr_type stream_type , GEOM::DataStreamAttr**);
				
				void set_color(GEOM::VECTOR4D );
				GEOM::VECTOR4D  get_color();
				void set_face_normal(GEOM::VECTOR3D );
				GEOM::VECTOR3D  get_face_normal();
				
				GeomStreamElementBase* get_target_vertex();
				void set_target_vertex(GeomStreamElementBase*);
				TYPES::UINT32 get_internal_idx();
				void set_internal_idx(TYPES::UINT32);
				TYPES::UINT32 get_original_idx();
				void set_original_idx(TYPES::UINT32);


				std::string& get_face_group_id();
				void set_face_group_id(const std::string&);
				std::string& get_subgeom_id();
				void set_subgeom_id(const std::string&);
		};
		/** \class Vertex3D
		*	\brief A Vertex3D contains all information about a vertex in 3D space.
		* 
		*/
		class Vertex3D:
			public GeomStreamElementBase
		{
			private:
				GEOM::VECTOR3D  position;		
				GEOM::VECTOR3D  normal;
				GEOM::VECTOR3D  tangent;		
				std::map< std::string, GEOM::VECTOR2D > uv_channels;
			
			protected:
				virtual std::string& intern_get_vertex_string();
			public:
				Vertex3D();
				~Vertex3D();

				bool compare(Vertex3D*);

				void set_position(TYPES::F64 x, TYPES::F64 y, TYPES::F64 z);
				GEOM::VECTOR3D  get_position();
				void set_normal(GEOM::VECTOR3D );
				GEOM::VECTOR3D  get_normal();
				void set_tangent(GEOM::VECTOR3D );
				GEOM::VECTOR3D  get_tangent();
				void set_uv_channel(const std::string&, TYPES::F64 x, TYPES::F64 y);
				GEOM::VECTOR2D get_uv_channel(std::string&);

		};
		/** \class SharedVertex3D
		*	\brief A SharedVertex3D contains a list of Vertex3D that share the same position
		* 
		*/
		class SharedVertex3D
		{
			private:
				std::vector<Vertex3D*> verts;
			
			public:
				SharedVertex3D();
				~SharedVertex3D();
				
				void add_vert(Vertex3D*);
				std::vector<Vertex3D*>& get_verts();
				void calculate_normals();
				void connect_duplicates();
				TYPES::INT32 get_in_subgeom(const TYPES::SUBGEOM_ID_STRING&);
		};
		/** \class Vertex2D
		*	\brief A Vertex2D contains all information about a vertex in 2D space.
		* 
		*/
		class Vertex2D:
			public GeomStreamElementBase
		{
			private:
				GEOM::VECTOR2D  position;
				GEOM::VECTOR3D  curve_attributes;
				GEOM::VECTOR2D  uv;
				


			
			protected:
				virtual std::string& intern_get_vertex_string();

			public:
				Vertex2D();
				~Vertex2D();
				void set_position(TYPES::F64 x, TYPES::F64 y);
				void set_curve_attributes(TYPES::F64 type, TYPES::F64 curve_1, TYPES::F64 curve_2);
				void set_uv(TYPES::F64 u, TYPES::F64 v);
				GEOM::VECTOR2D  get_position();
				GEOM::VECTOR2D  get_uv();
				GEOM::VECTOR3D  get_curve_attributes();
		};
		/** \class Triangle
		*	\brief A Triangle contains information about a triangle.
		* 
		*/
		class Triangle:
			public GeomStreamElementBase
		{
			private:
				std::string face_id;
				GEOM::Vertex3D* v1;
				GEOM::Vertex3D* v2;
				GEOM::Vertex3D* v3;
				TYPES::UINT32 original_vert_index_1;
				TYPES::UINT32 original_vert_index_2;
				TYPES::UINT32 original_vert_index_3;
				TYPES::UINT16 output_idx_1;
				TYPES::UINT16 output_idx_2;
				TYPES::UINT16 output_idx_3;
				std::vector<TYPES::UINT32> tri_out_indices;
				
			protected:
				virtual std::string& intern_get_vertex_string();
			public:
				/**	\brief Create a new Triangle.
				* 
				*/
				Triangle(TYPES::UINT32 v1_index, GEOM::Vertex3D* v1, TYPES::UINT32 v2_index, GEOM::Vertex3D* v2, TYPES::UINT32 v3_index, GEOM::Vertex3D* v3);
				Triangle(TYPES::UINT32 v1_index, TYPES::UINT32 v2_index, TYPES::UINT32 v3_index);
				Triangle();
				~Triangle();
						
				void get_verts(std::vector<Vertex3D*>& all_verts);
				void reset_out_indices();
				void add_out_index(TYPES::UINT32);
				std::vector<TYPES::UINT32>& get_indices();

				void set_v1(GEOM::Vertex3D* );
				GEOM::Vertex3D*  get_v1();
				void set_v2(GEOM::Vertex3D* );
				GEOM::Vertex3D*  get_v2();
				void set_v3(GEOM::Vertex3D* );
				GEOM::Vertex3D*  get_v3();

				void set_original_vert_index_1(TYPES::UINT32 );
				TYPES::UINT32  get_original_vert_index_1();
				void set_original_vert_index_2(TYPES::UINT32 );
				TYPES::UINT32  get_original_vert_index_2();
				void set_original_vert_index_3(TYPES::UINT32 );
				TYPES::UINT32  get_original_vert_index_3();

				void set_output_idx_1(TYPES::UINT16 );
				TYPES::UINT16 get_output_idx_1();
				void set_output_idx_2(TYPES::UINT16 );
				TYPES::UINT16 get_output_idx_2();
				void set_output_idx_3(TYPES::UINT16 );
				TYPES::UINT16 get_output_idx_3();
		};
		
	}
}

#endif

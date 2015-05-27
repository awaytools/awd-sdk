#ifndef _LIBAWD_GEOM_ELEMENTS_H
#define _LIBAWD_GEOM_ELEMENTS_H

#include <vector>
#include <map>
#include <string>
#include "utils/awd_types.h"
#include "base/attr.h"


using namespace AWD::BASE;

namespace AWD
{	
	namespace GEOM
	{	
		class Vertex2D;
		class Vertex3D;
		class Triangle;
		
		/** \struct BOUNDS2D
		* 
		*/
		struct BOUNDS2D
		{
			TYPES::F64 minX;
			TYPES::F64 maxX;
			TYPES::F64 minY;
			TYPES::F64 maxY;
			BOUNDS2D()
			{
			}
			BOUNDS2D(TYPES::F64 minX, TYPES::F64 maxX, TYPES::F64  minY, TYPES::F64 maxY)
			{
				minX = minX;
				maxX = maxX;
				minY = minY;
				maxY = maxY;
			}
		};
		/** \struct BOUNDS3D
		* 
		*/
		struct BOUNDS3D
		{
			TYPES::F64 minX;
			TYPES::F64 maxX;
			TYPES::F64 minY;
			TYPES::F64 maxY;
			TYPES::F64 minZ;
			TYPES::F64 maxZ;
			BOUNDS3D()
			{
			}
			BOUNDS3D(TYPES::F64 minX, TYPES::F64 maxX, TYPES::F64  minY, TYPES::F64 maxY, TYPES::F64 minZ, TYPES::F64 maxZ)
			{
				minX = minX;
				maxX = maxX;
				minY = minY;
				maxY = maxY;
				minZ = minZ;
				maxZ = maxZ;
			}
		};
		/** \struct VECTOR3D
		*	\brief A VECTOR3D contains three values of type TYPES::F64
		* 
		*/
		struct VECTOR4D
		{
			TYPES::F64 x;
			TYPES::F64 y;
			TYPES::F64 z;
			TYPES::F64 w;
			VECTOR4D()
			{
			}
			VECTOR4D(TYPES::F64 newx, TYPES::F64 newy, TYPES::F64  newz, TYPES::F64 neww)
			{
				x = newx;
				y = newy;
				z = newz;
				w = neww;
			}
		};
		/** \struct VECTOR2D
		*	\brief A VECTOR2D contains two values of type TYPES::F64 
		* 
		*/
		/** \struct VECTOR3D
		*	\brief A VECTOR3D contains three values of type TYPES::F64
		* 
		*/
		struct VECTOR3D
		{
			TYPES::F64 x;
			TYPES::F64 y;
			TYPES::F64 z;
			VECTOR3D()
			{
			}
			VECTOR3D(TYPES::F64 newx, TYPES::F64 newy, TYPES::F64 newz)
			{
				x = newx;
				y = newy;
				z = newz;
			}
		};
		/** \struct VECTOR2D
		*	\brief A VECTOR2D contains two values of type TYPES::F64 
		* 
		*/
		struct VECTOR2D 
		{
			TYPES::F64 x;
			TYPES::F64 y;
			VECTOR2D()
			{
				x = 0;
				y = 0;
			}
			VECTOR2D(TYPES::F64 newx, TYPES::F64 newy)
			{
				x = newx;
				y = newy;
			}
		};
		
		/**\brief A Matrix2x3 class, used for 2d scene-transformations or uv-transformation etc \n 
		* 
		*/
		class MATRIX2x3
		{
			private:
				TYPES::F64* matrix_data;
				
			public:
				MATRIX2x3(TYPES::F64* matrix_data);
				MATRIX2x3();
				~MATRIX2x3();
				result read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *);
				result write_to_file(FILES::FileWriter*, SETTINGS::BlockSettings *);
				result prepend(MATRIX2x3*);
				result append(MATRIX2x3*);
				result set(TYPES::F64* matrix_data);
				TYPES::F64* get();

		};
		/**\brief A Matrix4x4 class, used for 3d - scene-transformations etc \n 
		* 
		*/
		class MATRIX4x3
		{
			private:
				TYPES::F64* matrix_data;
				
			public:
				MATRIX4x3(TYPES::F64* matrix_data);
				MATRIX4x3();
				~MATRIX4x3();
				result read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *);
				result write_to_file(FILES::FileWriter*, SETTINGS::BlockSettings *);
				result set(TYPES::F64* matrix_data);
				TYPES::F64* get();

		};
		
		/**\brief A ColorTransform class, used for color-transform etc \n 
		* 
		*/
		class ColorTransform
		{
			private:
				TYPES::F64* matrix_data;
				
			public:
				ColorTransform(TYPES::F64* matrix_data);
				ColorTransform();
				~ColorTransform();
				result read_from_file(FILES::FileReader*, SETTINGS::BlockSettings *);
				result write_to_file(FILES::FileWriter*, SETTINGS::BlockSettings *);
				result set(TYPES::F64* matrix_data);
				result prepend(ColorTransform* matrix_data);
				TYPES::F64* get();

		};

		class PathSegment;
		class AWDPathIntersection
		{
			private:
			GEOM::PathSegment* pathSeg1;
			GEOM::PathSegment* pathSeg2;
			GEOM::intersect_state state;
			public:
				AWDPathIntersection(PathSegment*, PathSegment*);     
				~AWDPathIntersection();
				GEOM::intersect_state get_state(); 
				GEOM::PathSegment* get_pathSeg1();		
				GEOM::PathSegment* get_pathSeg2();
				void set_state(GEOM::intersect_state);
				bool compare(GEOM::PathSegment*, GEOM::PathSegment*);       
		};
		
		
		/** \class PathSegment
		*	\brief A PathSegment defines one segment of a path. it can either be LINEAR, or CONVEX or CONCAVE.
		* 
		*/
		class Path;
		class PathSegment
		{
			private:
				GEOM::edge_state state;
				bool subdivide;
				bool intersects;
				bool deleteIt;
				bool no_intersecting;
				bool test_point_dirty;
				bool test_point_dirty2;
				bool test_point_dirty3;
				bool has_control_point;
				bool export_this_linear;
				bool contains_origin;
				GEOM::VECTOR2D startPoint;
				GEOM::VECTOR2D endPoint;
				GEOM::VECTOR2D controlPoint;
				GEOM::VECTOR2D test_point;
				GEOM::VECTOR2D test_point2;
				GEOM::VECTOR2D test_point3;

				GEOM::PathSegment* next;
				GEOM::PathSegment* last;

				int originalSegment;
				std::string id;
				double size;
				double length;
				double curviness;

				GEOM::edge_type edgeType;
				std::vector< GEOM::PathSegment*> subdividedPath;
		 
			public:
				PathSegment();
				~PathSegment();
				int subdivion_cnt;
				int max_x;
				int max_y;
				int min_x;
				int min_y;	
				
				TYPES::UINT16 bit_id_x;
				TYPES::UINT32 bit_id;
				TYPES::UINT16 bit_id_y;				

				GEOM::PathSegment* get_next();
				void set_next(GEOM::PathSegment*);

				GEOM::PathSegment* get_last();
				void set_last(GEOM::PathSegment*);
				
				GEOM::edge_state get_state();
				void set_state(GEOM::edge_state);

				GEOM::VECTOR2D get_startPoint();
				void set_startPoint(GEOM::VECTOR2D);

				GEOM::VECTOR2D get_endPoint();
				void set_endPoint(GEOM::VECTOR2D);

				GEOM::VECTOR2D get_controlPoint();
				void set_controlPoint(GEOM::VECTOR2D);
				
				GEOM::edge_type get_edgeType();
				void set_edgeType(GEOM::edge_type);
				/* test_point is a point that is used to test if a curve-segment is a Concave or convex.
				*/
				GEOM::VECTOR2D get_test_point();

				void sub_divide_outside_edge(std::vector<GEOM::VECTOR2D>& new_tri1,std::vector<GEOM::VECTOR2D>& new_tri2, double threshold);

		
				void add_path_intersections(GEOM::AWDPathIntersection*);
				GEOM::AWDPathIntersection* find_path_intersections(GEOM::PathSegment* path1, GEOM::PathSegment* path2);
				std::vector<GEOM::AWDPathIntersection*> get_path_intersections();
				int get_this_hole_idx();
				double get_size();
				double get_length();
				void calculate_bounds();
				void set_export_this_linear(bool);
				bool get_export_this_linear(); 
				void set_deleteIt(bool);
				void set_dirty(); 
				bool get_deleteIt(); 
				bool get_has_control_point();
				void set_hole_idx(int);
				bool get_intersects();       
				void set_intersects(bool);
				bool get_subdivide();       
				void set_subdivide(bool);
				bool get_no_intersecting();       
				void set_no_intersecting(bool);
				int get_originalSegment();       
				void set_originalSegment(int); 

				void clear_subdivison();

				double get_curviness();

				std::string& get_id();
				void set_id(std::string& );
				

				std::vector<GEOM::PathSegment*>& get_subdivided_path();

				result subdividePath(SETTINGS::Settings*);



		};
		
		class Path
		{
			private:
				std::vector< GEOM::PathSegment*> path_segments;
				std::string id;
				bool is_hole;

				bool is_dirty;

				float* polyX;
				float* polyY;
				int polyCorners;
				float* constant;
				float* multiple;
				bool has_curves;
				bool delete_segs;
				int* path_seg_indicies;
				int* path_seg_indicies_inner;
		 
			public:
				Path(bool);
				Path();
				~Path();

				int max_x;
				int max_y;
				int min_x;
				int min_y;	
				
				std::string& get_id();
				void set_id(std::string& );
				void prepare(GEOM::edge_type);
				bool get_has_curves();
				int point_inside_path(float, float);
				bool point_inside_path_bool(float, float);
				bool point_inside_path_get_intersections(float, float, std::vector<PathSegment*>& output_segs);
				bool get_is_hole();
				void set_is_hole(bool);
				bool check_boundingbox(double max_x, double max_y,double min_x, double min_y);
				void make_dirty();
				result add_segment_fonts(GEOM::PathSegment*);
				result add_segment(GEOM::PathSegment*);
				result validate_path();
				void make_linear();
				result get_curve_types(SETTINGS::Settings*);
				result resolve_curve_intersections(SETTINGS::Settings*, std::string&);
				result resolve_curve_intersections_contour_vs_holes(SETTINGS::Settings*, std::string&, std::vector<GEOM::Path*>);
				result resolve_curve_intersections_hole_vs_holes(SETTINGS::Settings*, std::string&, std::vector<GEOM::Path*>);
				TYPES::UINT32 get_point_count(GEOM::edge_type edgetype);
				std::vector<GEOM::PathSegment*>& get_segments();
				
				int IsPointInBoundingBox(float x1, float y1, float x2, float y2, float px, float py);
				int LineSegmentIntersection(float l1x1, float l1y1, float l1x2, float l1y2, float l2x1, float l2y1, float l2x2, float l2y2);


		};

		/** \class FilledRegion
		*	\brief A FilledRegion contains information about a filled Path.
		* 
		*/
		class FilledRegion 
		{
		private:
				BASE::AWDBlock* fill_material;
				TYPES::filled_region_type type;
				std::vector<GEOM::Path* > pathes;

			public:

				FilledRegion(TYPES::filled_region_type type);
				~FilledRegion();

				GEOM::MATRIX2x3* uv_transform;
				void add_path();
				void add_existing_path(GEOM::Path*);
				
				void set_type(TYPES::filled_region_type);
				TYPES::filled_region_type get_type();

				void set_material(BASE::AWDBlock*);
				BASE::AWDBlock* get_material();
				
				result add_segment(GEOM::PathSegment*);

				std::vector<GEOM::Path*>& get_pathes();
		};
		/** \class FilledRegion
		*	\brief A FilledRegion contains information about a filled Path.
		* 
		*/
		class FilledRegionGroup
		{
			public:

				FilledRegionGroup();
				~FilledRegionGroup();
		
				void merge_group_into(FilledRegionGroup* filled_reg_group);
				void add_filled_region(FilledRegion* filled_region);
				FilledRegionGroup* redirect;
				std::vector<FilledRegion* > filled_regions;
				int max_x;
				int max_y;
				int min_x;
				int min_y;	
				int group_id;	
				TYPES::UINT16 bit_id_x;
				TYPES::UINT16 bit_id_y;
		};
	}
}
#endif

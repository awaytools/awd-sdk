#ifndef _LIBAWD_AWDUTIL_H
#define _LIBAWD_AWDUTIL_H

#ifdef _WIN32
#else
#include <sys/time.h>
#include <ctime>
#endif
#include <math.h>
#include <string>
#include "utils/awd_types.h"
#include "utils/util.h"
#include "blocks/geometry.h"
#include "elements/geom_elements.h"
#include "awd_project.h"

// Macros to calculate matrix size depending on width (optimized for size or accuracy)
#define VEC2_SIZE(wide) (wide? (2*sizeof(TYPES::F64)):(2*sizeof(TYPES::F32)))
#define VEC3_SIZE(wide) (wide? (3*sizeof(TYPES::F64)):(3*sizeof(TYPES::F32)))
#define VEC4_SIZE(wide) (wide? (4*sizeof(TYPES::F64)):(4*sizeof(TYPES::F32)))
#define MTX32_SIZE(wide) (wide? (6*sizeof(TYPES::F64)):(6*sizeof(TYPES::F32)))
#define MTX33_SIZE(wide) (wide? (9*sizeof(TYPES::F64)):(9*sizeof(TYPES::F32)))
#define MTX43_SIZE(wide) (wide? (12*sizeof(TYPES::F64)):(12*sizeof(TYPES::F32)))
#define MTX44_SIZE(wide) (wide? (16*sizeof(TYPES::F64)):(16*sizeof(TYPES::F32)))
#define MTX54_SIZE(wide) (wide? (20*sizeof(TYPES::F64)):(20*sizeof(TYPES::F32)))



namespace AWD
{
	    
	result create_TextureAtlasfor_timelines(AWDProject* awd_project, const std::string&);
	result create_TextureAtlasfor_timelines_refactor(AWDProject* awd_project, const std::string&);
	
	inline TYPES::UINT16 get_bit16(int start, int end){	
		TYPES::UINT16 int_min = 1 << start;
		TYPES::UINT16 int_max = 1 << end;
		TYPES::UINT16 result = (int_max | int_max - int_min);
		return result;
		//return ((((result & 0xff) * 0x0202020202ULL & 0x010884422010ULL) % 1023) << 8) | ((((result & 0xff00) >> 8) * 0x0202020202ULL & 0x010884422010ULL) % 1023);
	}
	/**
	*\brief Gets a string that corresponts to a AWD::result. 
	*	@param[in] result The result to get a message for.
	*	@param[out] outString The std::string to fill
	* @return AWD::result::AWD_SUCCESS or AWD::result::NOT_FOUND.
	*/
	AWD::result get_string_for_result(AWD::result result, std::string& outString);
	
    
    namespace FILES
    {
        std::string num_to_string(double input);
        std::string int_to_string(int input);
    }
    
	namespace BLOCK
	{
			/**
			* \brief Gets the block-addr inside a AWDFile. This is a util, because AWDBLock cannot acces BlockInstance or AWDFile, because both are onl predeclared.
			*	@param[in] awdBlock Pointer to the AWDBlock that should be added to the AWDFile
			*	@param[in] path The path of the AWDFile
			*	@param[out] output_address The output address
			* @return The index of the block in the file, or 0 if the block doesnt exist
			*/
			result get_block_address_for_file(BASE::AWDBlock* awdBlock, FILES::AWDFile* file, TYPES::UINT32& output_address);
			
			result get_asset_type_as_string(BLOCK::block_type blocktype, std::string& type_str);
			result create_block_for_block_type(BASE::AWDBlock** awdBlock, BLOCK::block_type blocktype, FILES::AWDFile* awd_file);
			
	}
	namespace GEOM
	{
		
		inline void subdivideCurve(double startx, double starty, double cx, double cy, double endx, double endy, std::vector<double>& resultVec)
		  {
		   double c1x = startx + (cx - startx) * 0.5;
		   double c1y = starty + (cy - starty) * 0.5;
   
		   double c2x = cx + (endx - cx) * 0.5;
		   double c2y = cy + (endy - cy) * 0.5;
   
		   double ax = c1x + (c2x - c1x) * 0.5;
		   double ay = c1y + (c2y - c1y) * 0.5;
		   resultVec.push_back(startx);
		   resultVec.push_back(starty);
		   resultVec.push_back(c1x);
		   resultVec.push_back(c1y);
		   resultVec.push_back(ax);
		   resultVec.push_back(ay);
		   resultVec.push_back(c2x);
		   resultVec.push_back(c2y);
		   resultVec.push_back(endx);
		   resultVec.push_back(endy);
		}
		
		inline double maximum(double x, double y, double z) {
			double max_val = x; 
			if (y > max_val) 
				max_val = y;
			if (z > max_val) 
				max_val = z;
			return max_val; 
		} 
		inline double minimum(double x, double y, double z) {
			double min_val = x; 
			if (y < min_val)
				min_val = y;
			if (z < min_val) 
				min_val = z;
			return min_val; 
		} 

		inline bool bounds_intersect(double minx_a, double maxx_a, double miny_a, double maxy_a, double minx_b, double maxx_b, double miny_b, double maxy_b )
		{
			if((minx_a==maxx_b)&&(maxx_a==maxx_b)&&(miny_a==miny_b)&&(maxy_a==maxy_b))
				return true;
			if(minx_a>=maxx_b)
				return false;
			if(maxx_a<=minx_b)
				return false;
			if(miny_a>=maxy_b)
				return false;
			if(maxy_a<=miny_b)
				return false;
			return true;
		}
		
		inline TYPES::UINT32 PointInTri(double x, double y, double t1x, double t1y, double t2x, double t2y, double t3x, double t3y)
		{
				
			TYPES::UINT32 interext_cnt = 0;
	
			if (((t1y < y) && (t3y >= y)) || ((t3y < y) && (t1y >= y))){
				if ((t1x + (y - t1y)/(t3y - t1y)*(t3x - t1x)) < x)
					interext_cnt++;
			}
			if (((t2y < y) && (t1y >= y)) || ((t1y < y) && (t2y >= y))){
				if ((t2x + (y - t2y)/(t1y - t2y)*(t1x - t2x)) < x)
					interext_cnt++;
			}
			if (((t3y < y) && (t2y >= y)) || ((t2y < y) && (t3y >= y))){
				if ((t3x + (y - t3y)/(t2y - t3y)*(t2x - t3x)) < x)
					interext_cnt++;
			}
			return interext_cnt;
		}

		inline TYPES::UINT32 PointInPath(std::vector<double>& simplyifed_path, double x, double y)
		{
				
			TYPES::UINT32 interext_cnt = 0;
			int j = int(simplyifed_path.size()) - 2;
			for (int i = 0; i < simplyifed_path.size(); i+=2)
			{
				//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]);
				if (((simplyifed_path[i+1] < y) && (simplyifed_path[j+1] >= y)) || ((simplyifed_path[j+1] < y) && (simplyifed_path[i+1] >= y)))
				{
					if ((simplyifed_path[i] + (y - simplyifed_path[i+1])/(simplyifed_path[j+1] - simplyifed_path[i+1])*(simplyifed_path[j] - simplyifed_path[i])) < x)
						interext_cnt++;
				}
				j = i;
			}
			return interext_cnt;
		}

		inline result get_edge_string(std::string& string_out, double x1, double x2, double y1, double y2)
		{
			string_out="";
			if(x1>x2)
                string_out+=FILES::num_to_string(x1)+"#"+FILES::num_to_string(x2)+"#";
			else
				string_out+=FILES::num_to_string(x2)+"#"+FILES::num_to_string(x1)+"#";
			if(y1>y2)
				string_out+=FILES::num_to_string(y1)+"#"+FILES::num_to_string(y2)+"#";
			else
				string_out+=FILES::num_to_string(y2)+"#"+FILES::num_to_string(y1)+"#";
			return result::AWD_SUCCESS;
		}

	
		result get_edge_string(std::string& string_out, double x1, double x2, double y1, double y2);

		void subdivideCurve(double startx, double starty, double cx, double cy, double endx, double endy, std::vector<double>& resultVec);

		void resolve_line_curve_intersection(PathSegment* curve, PathSegment* line, SETTINGS::Settings*);
		void resolve_curve_curve_intersection(PathSegment* curve_1, PathSegment* curve_2, SETTINGS::Settings*);

		result update_simplified_path(std::vector<double>& simplyifed_path, std::vector<PathSegment*>& path);

		result write_stream_attr_to_file(FILES::FileWriter*, GeomStreamElementBase*, DataStreamAttrDesc*);
		result write_vertex_attr_to_file(FILES::FileWriter*, Vertex2D*, DataStreamAttrDesc*);
		result write_triangle_attr_to_file(FILES::FileWriter*, Triangle*, DataStreamAttrDesc*);
		
		TYPES::UINT32  append_vertex_attr_to_stream(TYPES::union_ptr,  Vertex2D*, DataStreamAttrDesc*, TYPES::UINT32);
		TYPES::UINT32  append_triangle_attr_to_stream(TYPES::union_ptr,  Triangle*, DataStreamAttrDesc*, TYPES::UINT32);
		result append_stream_attr_to_stream(TYPES::union_ptr,  GeomStreamElementBase*, DataStreamAttrDesc*, TYPES::UINT32);
		/**
		*\brief Processes a Geometry, so it contains a list of valid SubGeom that can be processed into datastreams to write to file.
		*@param[in] geom Pointer to the Geometry to process.
		*@param[in] assetlib [optional] Pointer to the AWDProject. The function might need to duplicate the given Geometry. Pass in the AWDProject, so the newly created geometry can be added to the AWDProject, or ommit it to ´prevent the function from creating any new Geometry.
		* \todo add error return type (?)
		*/
		result ProcessMeshGeometry(BLOCKS::Geometry* geom, AWDProject* assetlib=NULL);
		
		double distance_point_to_line(float startx, float starty, float testx, float testy, float endx, float endy);

		bool line_intersect(float x1, float y1, float x2, float y2,float x3, float y3,float x4, float y4);
		
		/**
		*\brief Processes a Geometry, so it contains a list of valid SubGeom that can be written into files.
		*@param[in] geom Pointer to the Geometry to process.
		*@param[in] assetlib [optional] Pointer to the AWDProject. The function might need to duplicate the given Geometry. Pass in the AWDProject, so the newly created geometry can be added to the AWDProject, or ommit it to ´prevent the function from creating any new Geometry.
		* \todo add error return type (?)

		*/
		result ProcessShapeGeometry(BLOCKS::Geometry* geom, AWDProject* assetlib, SETTINGS::Settings* subgeom_settings);
		
		bool bounds_intersect(double minx_a, double maxx_a, double miny_a, double maxy_a, double minx_b, double maxx_b, double miny_b, double maxy_b );

		result ResolvePathIntersections(SETTINGS::Settings* settings,  std::string& message, GEOM::Path* hole, GEOM::Path* contour, bool);
		/**
		*\brief Processes a SubGeom into datastreams that can be written to file.
		*@param[in] sub_geom Pointer to the SubGeomInternal to process.* \todo add error return type (?)

		*/
		result ProcessSubGeometry(GEOM::SubGeom* geom);
		
		/*
		bool tris_intersecting(GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *);
        bool point_in_tr(GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *);
        bool line_intersect(GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *);
        ShapePoint * line_intersection_point(GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *);
		bool resolve_segment_intersection(GEOM::AWDPathIntersection* thisIntersection);
		bool test_bounding_box_lines(GEOM::Vertex2D * a1, GEOM::Vertex2D* a2, GEOM::Vertex2D * b1, GEOM::Vertex2D * b2);
		bool test_bounding_box(GEOM::Vertex2D * a1, GEOM::Vertex2D* a2, GEOM::Vertex2D * a3, GEOM::Vertex2D * b1, GEOM::Vertex2D * b2, GEOM::Vertex2D * b3);
		double Sign(GEOM::Vertex2D * p1, GEOM::Vertex2D * p2, GEOM::Vertex2D * p3);
		
		double maximum(double x, double y, double z);
		double minimum(double x, double y, double z);

        void add_straight_edge(GEOM::Vertex2D *, GEOM::Vertex2D*);
        void add_curve(GEOM::Vertex2D *, GEOM::Vertex2D*, GEOM::Vertex2D*);
        string& get_edge_string(GEOM::Vertex2D *, GEOM::Vertex2D*);
		double side_of_line(GEOM::Vertex2D *, GEOM::Vertex2D *, GEOM::Vertex2D *);
		double edge_length(GEOM::Vertex2D *, GEOM::Vertex2D*);
		SimplePoint * calc_fill_point(GEOM::Vertex2D *, GEOM::Vertex2D*, GEOM::Vertex2D*, double);
		*/
	}
	
	namespace TYPES
	{
		
		/**
		*\brief Get the size of bytes of for a specific TYPES::data_types using a storage-precision.
		*@param[in] data_type The data-type to get the size for
		*@param[in] storage_precision The storageprecision
		*/
		TYPES::UINT32 get_data_type_size(TYPES::data_types data_type, bool storage_precision);
		/**
		*\brief Get the size of bytes of for a specific TYPES::data_types using a storage-precision calculated by a storage_precision type and the settings.
		*@param[in] data_type The data-type to get the size for
		*@param[in] storage_precision The storageprecision
		*/
		TYPES::UINT32 get_data_type_size_for_precision_category(data_types type, SETTINGS::BlockSettings* settings, TYPES::storage_precision_category storage_cat);
		
		TYPES::UINT32 get_single_val_data_type_size_for_precision_category(data_types type, SETTINGS::BlockSettings* settings, TYPES::storage_precision_category storage_cat);
		TYPES::UINT32 get_single_val_data_type_size(data_types type, bool storage_precision);
		/**
		*\brief Get the size of bytes of for a specific TYPES::data_types using a storage-precision calculated by a storage_precision type and the settings.
		*@param[in] data_type The data-type to get the size for
		*@param[in] storage_precision The storageprecision
		*/
		bool is_data_type_floats(data_types type);
		
		
		/**
		*\brief Create a TYPES::COLOR from 4 TYPES::F64;
		*/
		TYPES::COLOR create_color_from_floats(TYPES::F64 red, TYPES::F64 green, TYPES::F64 blue, TYPES::F64 alpha);

		/**
		*\brief Create a TYPES::COLOR from 4 TYPES::UINT32;
		*/
		TYPES::COLOR create_color_from_ints(TYPES::UINT32 red, TYPES::UINT32 green, TYPES::UINT32 blue, TYPES::UINT32 alpha);
	}
	namespace FILES
    {
		
		/**
		*\brief Makes sure the preview-files are modified and copied;
		*/
		result open_preview(FILES::AWDFile* output_file, std::string& preview_file, std::vector<std::string> change_ids, std::vector<std::string> change_values, std::string& open_path, bool append_name);
		
		result copy_files_from_directory(std::string& source_dir, std::string& target_dir, const std::string& exclude_filename);
		
		std::string RemoveSpecialCharacters(std::string str);

		result string_find_and_replace(std::string& str, const std::string& oldStr, const std::string& newStr);
		/**
		*\brief Create a TYPES::COLOR from 4 x TYPES::UINT32;
		*/
		result copy_file(const std::string& initialFilePath, const std::string& outputFilePath);
		
        
        
		/**
		*\brief Create a TYPES::COLOR from 4 x TYPES::UINT32;
		*/
		result extract_file_extension(const std::string& path, std::string& extension);
		
		/**
		*\brief Validate path.
		*param[in][out] path The path that should be modifed.
		*param[in] extension The std::string that should be filled.
		*/
		result validate_file_path(std::string& path, const std::string& extension);
				
		/**
		*\brief Extract the directory from a path, and creates it if it doesnt exists
		*param[in] path The path.
		*param[out] base_path The std::string that should be filled.
		*/
		result extract_directory_from_path(const std::string& path, std::string& base_path);
		
		result validate_directory_path(const std::string& path);
		
		/**
		*\brief Extract a file-name from a path.
		*param[in] path The path.
		*param[out] fileName The std::string that should be filled.
		*/
		result extract_name_from_path(const std::string& path, std::string& fileName);
		
		/**
		*\brief Get a copy of a path that contains no file-extension.
		*param[in] path The path.
		*param[out] path_no_ext The std::string that should be filled.
		*/
		result extract_path_without_file_extension(const std::string& path, std::string& path_no_ext);
				
		bool file_exists(const std::string& path);
		/**
		*\brief Swap the bytes of a TYPES::UINT16
		*/
		TYPES::UINT16   swapui16(TYPES::UINT16);

		/**
		*\brief Swap the bytes of a TYPES::UINT32
		*/
		TYPES::UINT32   swapui32(TYPES::UINT32);
		
		/**
		*\brief Swap the bytes of a TYPES::F32
		*/
		TYPES::F32		swapf32(TYPES::F32);
		
		/**
		*\brief Swap the bytes of a TYPES::F64
		*/
		TYPES::F64		swapf64(TYPES::F64);
	}
}
#endif

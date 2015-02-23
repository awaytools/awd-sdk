#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

#include "utils/awd_types.h"

namespace AWD
{
	
	namespace GEOM
	{
		//predeclare
		class DataStreamRecipe;
	}
	namespace SETTINGS
	{
	
		class VertexNormalsSettings
		{
			private:
				bool use_smoothing_groups;
				bool use_phong_breaks;
				GEOM::vertex_normals_mode vertex_normal_mode;
				TYPES::F64 normal_threshold;

			public:
				VertexNormalsSettings();
				~VertexNormalsSettings();
				bool is_compatible_vertex_normal_mode(VertexNormalsSettings* subGeom);
				bool get_use_smoothing_groups();
				bool get_use_phong_breaks();
				
				void set_vertex_normals_mode(GEOM::vertex_normals_mode );
				GEOM::vertex_normals_mode get_vertex_normals_mode();
				TYPES::F64 get_normal_threshold();

		};
		
		/** \class BlockSettings
		*	\brief Contains the settings for one AWD::BLOCK::BlockInstance (BLOCKS).
		* A AWD::BLOCK::BlockInstance can either have its own settings, or it can share settings with other blocks.
		* 
		*/
		class BlockSettings:
			public VertexNormalsSettings
		{
			private:
				std::string uv1_channel_id;
				std::string uv2_channel_id;
				std::vector<GEOM::DataStreamRecipe*> stream_recipes;
	
				// File header fields
				compression compression;
				bool use_compression_per_block;
				bool wideMatrix;
				bool wideGeom;
				bool wideProps;
				bool wideAttributes;
				TYPES::UINT8 block_header_flag;
				TYPES::F64 fps;
				bool resolve_intersections;
				TYPES::F64 curve_threshold;
				TYPES::F64 exterior_threshold;
				TYPES::F64 exterior_threshold_for_strokes;
				TYPES::F64 exterior_threshold_for_fonts;
				bool flipXaxis;
				bool flipYaxis;
				int max_iterations;
				TYPES::F64 scale;

			public:
				BlockSettings(bool create_default);
				~BlockSettings();
				void add_stream_recipe(GEOM::DataStreamRecipe* );
				GEOM::DataStreamRecipe* get_stream_by_type(GEOM::stream_type);
				void remove_stream_by_type(GEOM::stream_type, bool);

				/**	\brief Get the channel-id that can be used to retrieve the correct value for the UV-stream of a SubGeomInternal. 
				*/
				std::string& get_uv1_channel_id();
				/**	\brief Get the channel-id that can be used to retrieve the correct value for the SecondaryUV-stream of a SubGeomInternal. 
				*/
				std::string& get_uv2_channel_id();
            
				/**	\brief Get the stream recipes for this BlockSettings
				*/
				std::vector<GEOM::DataStreamRecipe*>& get_stream_recipes();

				bool is_compatible(BlockSettings*);
		
		
				bool get_use_compression_per_block();
				bool get_wide_matrix();
				void set_wide_matrix(bool);
				bool get_wide_geom();
				void set_wide_geom(bool);
				bool get_wide_props();
				void set_wide_props(bool);
				bool get_wide_attributes();
				void set_wide_attributes(bool);
				TYPES::F64 get_scale();
				void set_scale(TYPES::F64);
				bool get_flipXaxis();
				void set_flipXaxis(bool);
				bool get_flipYaxis();
				void set_flipYaxis(bool);
				bool get_resolve_intersections();
				void set_resolve_intersections(bool);
				TYPES::F64 get_exterior_threshold(TYPES::filled_region_type );
				void set_exterior_threshold(TYPES::F64, TYPES::filled_region_type );
				TYPES::F64 get_curve_threshold();
				void set_curve_threshold(TYPES::F64);
				int get_max_iterations();
				void set_max_iterations(int);
				TYPES::F64 get_fps();
				void set_fps(TYPES::F64);
				SETTINGS::compression get_compression();
				void set_compression(SETTINGS::compression);
				TYPES::UINT8 get_block_header_flag();
				void set_block_header_flag(TYPES::UINT8);
				BlockSettings* clone_block_settings();
		};

		class Settings:
			public BlockSettings
		{
			private:		
				TYPES::UINT16 file_header_flag;
				std::string root_directory;	
				std::string texture_directory;	
				std::string texture_directory_name;	
				std::string audio_directory;	
				std::string audio_directory_name;	
				std::string generator_version;	
				std::string generator_name;	
				TYPES::UINT8 default_ns_handle;	
				bool export_curves;
				bool distinglish_interior_exterior_triangles_2d;
				bool export_shapes_in_debug_mode;
				std::vector<BLOCK::category> export_block_categries_order;	///< this defines the order of exported block-categories. By default the scenegraph will be exported first (incl dependencies).
				bool export_all;	
				bool keep_verticles_in_path_order;
				bool export_materials;

			public:
				Settings(const std::string& root_directory, const std::string& generator_name, const std::string& generator_version);
				~Settings();
			
				
				TYPES::UINT16 get_file_header_flag();
				void set_file_header_flag(TYPES::UINT16);
				
				std::string& get_root_directory();
				void set_root_directory(const std::string& root_directory);

				std::string& get_texture_directory();
				std::string& get_texture_directory_name();
				void set_texture_directory_name(const std::string& root_directory);

				std::string& get_audio_directory();
				std::string& get_audio_directory_name();
				void set_audio_directory_name(const std::string& root_directory);

				std::string& get_generator_version();
				void set_generator_version(const std::string& generator_version);

				std::string& get_generator_name();
				void set_generator_name(const std::string& generator_name);
				
				result get_export_block_catergories_order(std::vector<BLOCK::category>& get_categories);
				result add_export_block_category(BLOCK::category new_cat);
				void clear_categories();	
				bool get_export_shapes_in_debug_mode();
				void set_export_shapes_in_debug_mode(bool );
				bool get_export_curves();
				void set_export_curves(bool );
				bool get_distinglish_interior_exterior_triangles_2d();
				void set_distinglish_interior_exterior_triangles_2d(bool );
				bool get_keep_verticles_in_path_order();
				void set_keep_verticles_in_path_order(bool );
		
				void set_export_materials(bool );
				bool get_export_materials();
				bool get_export_all();

				Settings* clone_settings();
		};
	}
}
#endif

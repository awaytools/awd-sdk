#include "utils/settings.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;

#include "utils/settings.h"
#include "utils/util.h"
#include "elements/geom_elements.h"
#include "elements/stream.h"


using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::GEOM;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;


VertexNormalsSettings::VertexNormalsSettings()
{
}

VertexNormalsSettings::~VertexNormalsSettings()
{
}

bool
VertexNormalsSettings::is_compatible_vertex_normal_mode(VertexNormalsSettings* vert_norm_settings)
{	
	// if the settings are the same, they are of course compatible.
	if(this==vert_norm_settings)
		return true;

	// if the vertex_normal_mode is different , they are not compatible.
	if(this->vertex_normal_mode!=vert_norm_settings->get_vertex_normals_mode())
		return false;

	// if the get_use_phong_breaks is different , they are not compatible.
	if(this->get_use_phong_breaks()!=vert_norm_settings->get_use_phong_breaks())
		return false;

	// if the get_use_smoothing_groups is different , they are not compatible.
	if(this->get_use_smoothing_groups()!=vert_norm_settings->get_use_smoothing_groups())
		return false;

	// even if all previous is same, it might be that it is set to use Normal Threshold for calculation
	if((this->vertex_normal_mode==GEOM::vertex_normals_mode::PHONG_FACE_NORMALS)||(this->vertex_normal_mode==GEOM::vertex_normals_mode::PHONG_VERTEX_NORMALS)){
		if(this->normal_threshold!=vert_norm_settings->get_normal_threshold())
			return false;return false;
	}
	return true;

}

bool 
VertexNormalsSettings::get_use_smoothing_groups()
{
	return this->use_smoothing_groups;
}

bool 
VertexNormalsSettings::get_use_phong_breaks()
{
	return this->use_phong_breaks;
}

void 
VertexNormalsSettings::set_vertex_normals_mode(GEOM::vertex_normals_mode vertex_normal_mode)
{
	this->vertex_normal_mode = vertex_normal_mode;
}

GEOM::vertex_normals_mode 
VertexNormalsSettings::get_vertex_normals_mode()
{
	return this->vertex_normal_mode;
}

TYPES::F64 
VertexNormalsSettings::get_normal_threshold()
{
	return this->normal_threshold;
}


BlockSettings::BlockSettings(bool create_default_streams):
	VertexNormalsSettings()
{
	this->radial_gradient_size=512;
	this->sound_file_extension="keep";
	this->compression = compression::UNCOMPRESSED;
	this->wideMatrix=false;
	this->wideGeom=false;
	this->wideProps=false;
	this->wideAttributes=false;
	this->scale=1.0;
	this->fps=25;
	this->block_header_flag=0;
	this->exterior_threshold=0.0;
	this->exterior_threshold_for_fonts=0.0;
	this->exterior_threshold_for_strokes=0.0;
	this->flipYaxis=false;
	this->flipXaxis=false;
	if(create_default_streams){
		create_streams(false, true);
	}
	this->all_bool_settings[bool_settings::OpenPreview]=true;
	this->all_bool_settings[bool_settings::CopyRuntime]=true;
	this->all_bool_settings[bool_settings::PrintExportLog]=false;
	this->all_bool_settings[bool_settings::PrintExportLogTimelines]=false;
}
bool
BlockSettings::get_bool(bool_settings id)
{
	if(this->all_bool_settings.find(id)==this->all_bool_settings.end()){
		//todo:raise alert
		return false;
	}
	return this->all_bool_settings[id];
}
void
BlockSettings::set_bool(bool_settings id, bool value)
{
	this->all_bool_settings[id]=value;
}
BlockSettings::~BlockSettings()
{
	this->all_bool_settings.clear();
}

void
BlockSettings::create_streams(bool tri_indices, bool uvs)
{
	this->stream_recipes.clear();
	// define streams for triangle. this is the same for 2d and 3d verts
	if(tri_indices){
		std::vector<GEOM::DataStreamAttrDesc> triangle_attrs;
		GEOM::DataStreamAttrDesc triangle_attr_indicies = DataStreamAttrDesc(data_stream_attr_type::VERTEX_INDICIES, data_types::UINT16, 3, storage_precision_category::UNDEFINED_STORAGE_PRECISION, true, true, true, false, "");
		triangle_attrs.push_back(triangle_attr_indicies);
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::TRIANGLES,	stream_target::TRIANGLE_STREAM, triangle_attrs));
		
	}
	// define streams for combined vertex stuff 2d:
	std::vector<GEOM::DataStreamAttrDesc> vertex2D_attributes;

	GEOM::DataStreamAttrDesc attribute_desc_position2d = DataStreamAttrDesc(data_stream_attr_type::POSITION2D, data_types::VECTOR2x1, 1, storage_precision_category::FORCE_FILESIZE, true, true, true, false, "");
	vertex2D_attributes.push_back(attribute_desc_position2d);

	GEOM::DataStreamAttrDesc attribute_desc_curve_data = DataStreamAttrDesc(data_stream_attr_type::CURVE_DATA_2D_INT, data_types::VECTORINT3x1, 1, storage_precision_category::FORCE_FILESIZE, true, true, true, false, "");
	//GEOM::DataStreamAttrDesc attribute_desc_curve_data = DataStreamAttrDesc(data_stream_attr_type::CURVE_DATA_2D, data_types::VECTOR3x1, 1, storage_precision_category::FORCE_FILESIZE, true, true, true, false, "");
	vertex2D_attributes.push_back(attribute_desc_curve_data);
		
	if(uvs){
		//GEOM::DataStreamAttrDesc attribute_desc_uv2d_data = DataStreamAttrDesc(data_stream_attr_type::UV_2D, data_types::VECTOR2x1, 1, storage_precision_category::FORCE_FILESIZE, true, true, true, false, "");
		//vertex2D_attributes.push_back(attribute_desc_uv2d_data);
			//GEOM::DataStreamAttrDesc attribute_desc_color = DataStreamAttrDesc(data_stream_attr_type::COLOR, data_types::VECTOR4x1, 1, storage_precision_category::FORCE_FILESIZE, true, true, true, false, "");
			//vertex2D_attributes.push_back(attribute_desc_color);
	}

	this->stream_recipes.push_back(new DataStreamRecipe(stream_type::ALLVERTDATA2D__2F3B,	stream_target::VERTEX_STREAM, vertex2D_attributes));
		
		/*

		std::vector<GEOM::DataStreamAttrDesc> verte3D_attributes;

		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::VERTICES,stream_target::VERTEX, data_types::FLOAT64));
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::VERTEX_NORMALS,stream_target::VERTEX, data_types::FLOAT64));
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::VERTEX_TANGENTS,stream_target::VERTEX, data_types::FLOAT64));
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::UVS,stream_target::VERTEX, data_types::FLOAT64));
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::SUVS,stream_target::VERTEX, data_types::FLOAT64));
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::VERTEX_WEIGHTS,stream_target::VERTEX, data_types::FLOAT64));
		this->stream_recipes.push_back(new DataStreamRecipe(stream_type::JOINT_INDICES,stream_target::VERTEX, data_types::UINT16));

		*/
}

bool
BlockSettings::get_use_compression_per_block()
{
	return false;
}

compression
BlockSettings::get_compression()
{
	return this->compression;
}
			
void
BlockSettings::set_compression(SETTINGS::compression compression)
{
	this->compression=compression;
}

bool
BlockSettings::get_resolve_intersections()
{
	return this->resolve_intersections;
}
void
BlockSettings::set_resolve_intersections(bool resolve_intersections)
{
	this->resolve_intersections=resolve_intersections;
}
void
BlockSettings::set_flipYaxis(bool flipYaxis)
{
	this->flipYaxis=flipYaxis;
}
bool
BlockSettings::get_flipYaxis()
{
	return this->flipYaxis;
}
void
BlockSettings::set_flipXaxis(bool flipXaxis)
{
	this->flipXaxis=flipXaxis;
}
bool
BlockSettings::get_flipXaxis()
{
	return this->flipXaxis;
}
TYPES::F64
BlockSettings::get_exterior_threshold(filled_region_type filled_region_type)
{
	if(filled_region_type==filled_region_type::FILL_CONVERTED_FROM_STROKE)
		return this->exterior_threshold_for_strokes;
	if(filled_region_type==filled_region_type::GENERATED_FONT_OUTLINES)
		return this->exterior_threshold_for_fonts;
	return exterior_threshold;

}
void
BlockSettings::set_exterior_threshold(TYPES::F64 exterior_threshold, filled_region_type filled_region_type)
{
	this->exterior_threshold=exterior_threshold;
	if(filled_region_type==filled_region_type::FILL_CONVERTED_FROM_STROKE)
		this->exterior_threshold_for_strokes=exterior_threshold;
	if(filled_region_type==filled_region_type::GENERATED_FONT_OUTLINES)
		this->exterior_threshold_for_fonts=exterior_threshold;

}
TYPES::F64
BlockSettings::get_curve_threshold()
{
	if(this->curve_threshold>1){
		this->curve_threshold=1;
	}
	if(this->curve_threshold<=0){
		this->curve_threshold=0.005;
	}
	return this->curve_threshold;
}
void
BlockSettings::set_curve_threshold(TYPES::F64 curve_threshold)
{
	if(this->curve_threshold>10){
		this->curve_threshold=10;
	}
	if(this->curve_threshold<=0){
		this->curve_threshold=0;
	}
	this->curve_threshold=curve_threshold;
}
TYPES::F64
BlockSettings::get_fps()
{
	return this->fps;
}
void
BlockSettings::set_fps(TYPES::F64 fps)
{
	this->fps=fps;
}
TYPES::UINT32
BlockSettings::get_radial_gradient_size()
{
	return this->radial_gradient_size;
}
void
BlockSettings::set_radial_gradient_size(TYPES::UINT32 radial_gradient_size)
{
	this->radial_gradient_size=radial_gradient_size;
}


int
BlockSettings::get_max_iterations()
{
	return this->max_iterations;
}
void
BlockSettings::set_max_iterations(int max_iterations)
{
	this->max_iterations=max_iterations;
}
bool
BlockSettings::get_wide_matrix()
{
	return this->wideMatrix;
}
void
BlockSettings::set_wide_matrix(bool wideMatrix)
{
	this->wideMatrix=wideMatrix;
}

std::string&
BlockSettings::get_sound_file_extension()
{
	return this->sound_file_extension;
}
void
BlockSettings::set_sound_file_extension(const std::string& sound_file_extension)
{
	this->sound_file_extension = sound_file_extension;
}
bool
BlockSettings::get_wide_geom()
{
	return this->wideGeom;
}
void
BlockSettings::set_wide_geom(bool wideGeom)
{
	this->wideGeom=wideGeom;
}
bool
BlockSettings::get_wide_props()
{
	return this->wideProps;
}
void
BlockSettings::set_wide_props(bool wideProps)
{
	this->wideProps=wideProps;
}
bool
BlockSettings::get_wide_attributes()
{
	return this->wideAttributes;
}
void
BlockSettings::set_wide_attributes(bool wideAttributes)
{
	this->wideAttributes=wideAttributes;
}
TYPES::F64
BlockSettings::get_scale()
{
	return this->scale;
}
void
BlockSettings::set_scale(TYPES::F64 scale)
{
	this->scale=scale;
}

TYPES::UINT8
BlockSettings::get_block_header_flag()
{
	this->block_header_flag=0;
		
	if (this->get_wide_matrix())	//bit1 = storagePrecision Matrix
		this->block_header_flag |= 0x01;
	if (this->get_wide_geom())		//bit2 = storagePrecision Geo
		this->block_header_flag |= 0x02;
	if (this->get_wide_props())		//bit3 = storagePrecision Props
		this->block_header_flag |= 0x04;

	return this->block_header_flag;
}

void
BlockSettings::set_block_header_flag(TYPES::UINT8 flag)
{
	this->block_header_flag=flag;
}

BlockSettings*
BlockSettings::clone_block_settings()
{
	BlockSettings* new_blockSettings = new BlockSettings(true);
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportFrameScript, this->get_bool(SETTINGS::bool_settings::ExportFrameScript));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExternalScripts, this->get_bool(SETTINGS::bool_settings::ExternalScripts));
	new_blockSettings->set_bool(SETTINGS::bool_settings::EmbbedAllChars, this->get_bool(SETTINGS::bool_settings::EmbbedAllChars));
	new_blockSettings->set_bool(SETTINGS::bool_settings::OpenPreview, this->get_bool(SETTINGS::bool_settings::OpenPreview));
	new_blockSettings->set_bool(SETTINGS::bool_settings::CopyRuntime, this->get_bool(SETTINGS::bool_settings::CopyRuntime));
	new_blockSettings->set_bool(SETTINGS::bool_settings::PrintExportLog, this->get_bool(SETTINGS::bool_settings::PrintExportLog));
	new_blockSettings->set_bool(SETTINGS::bool_settings::PrintExportLogTimelines, this->get_bool(SETTINGS::bool_settings::PrintExportLogTimelines));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportTimelines, this->get_bool(SETTINGS::bool_settings::ExportTimelines));
	new_blockSettings->set_bool(SETTINGS::bool_settings::IncludeInvisibleTimelineLayer, this->get_bool(SETTINGS::bool_settings::IncludeInvisibleTimelineLayer));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportGeometries, this->get_bool(SETTINGS::bool_settings::ExportGeometries));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportShapesInDebugMode, this->get_bool(SETTINGS::bool_settings::ExportShapesInDebugMode));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportFonts, this->get_bool(SETTINGS::bool_settings::ExportFonts));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportLibFonts, this->get_bool(SETTINGS::bool_settings::ExportLibFonts));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportBitmaps, this->get_bool(SETTINGS::bool_settings::ExportBitmaps));
	
	new_blockSettings->set_bool(SETTINGS::bool_settings::ForceTextureOverwrite, this->get_bool(SETTINGS::bool_settings::ForceTextureOverwrite));
	new_blockSettings->set_bool(SETTINGS::bool_settings::EmbbedTextures, this->get_bool(SETTINGS::bool_settings::EmbbedTextures));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportLibBitmaps, this->get_bool(SETTINGS::bool_settings::ExportLibBitmaps));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportSounds, this->get_bool(SETTINGS::bool_settings::ExportSounds));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ForceSoundOverwrite, this->get_bool(SETTINGS::bool_settings::ForceSoundOverwrite));
	new_blockSettings->set_bool(SETTINGS::bool_settings::EmbbedSounds, this->get_bool(SETTINGS::bool_settings::EmbbedSounds));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportLibSounds, this->get_bool(SETTINGS::bool_settings::ExportLibSounds));
	new_blockSettings->set_bool(SETTINGS::bool_settings::ExportTimelineSounds, this->get_bool(SETTINGS::bool_settings::ExportTimelineSounds));
	new_blockSettings->set_bool(SETTINGS::bool_settings::CreateAudioMap, this->get_bool(SETTINGS::bool_settings::CreateAudioMap));

	// todo: fill the blocksettings from original
	return new_blockSettings;
}

void 
BlockSettings::add_stream_recipe(GEOM::DataStreamRecipe*  new_stream)
{
	this->stream_recipes.push_back(new_stream);
}

GEOM::DataStreamRecipe* 
BlockSettings::get_stream_by_type(GEOM::stream_type this_type)
{
	for(DataStreamRecipe* stream : this->stream_recipes)
	{
		if(stream->get_stream_type()== this_type)
			return stream;
	}
	return NULL;
}

void 
BlockSettings::remove_stream_by_type(GEOM::stream_type this_type, bool bool_in)
{ 
	std::vector<DataStreamRecipe*> new_streams;
	for(DataStreamRecipe* stream : this->stream_recipes)
	{
		if(stream->get_stream_type()== this_type)
			delete stream;
		else 
			new_streams.push_back(stream);
	}
	this->stream_recipes = new_streams;
	return;
}

std::string& 
BlockSettings::get_uv1_channel_id()
{
	return this->uv1_channel_id;
}
std::string& 
BlockSettings::get_uv2_channel_id()
{
	return this->uv2_channel_id;
}

std::vector<GEOM::DataStreamRecipe*>& 
BlockSettings::get_stream_recipes()
{
	return this->stream_recipes;
}

/**
\brief Check if the BlockSettings are compatible to each other. \n
Two BlockSettings are compatible if they can share the same set of DataStreamRecipes to write a triangle into a file
*/
bool
BlockSettings::is_compatible(BlockSettings* subGeom)
{
	std::string uvchannel1;
	std::string uvchannel2;

	// for each stream-recipe we check if it also exists on the compare subgeometry-settings.
	// if it does, we need to check if they can use the same data, or if this would  result in render error

	// this locks like a heavy iteration, but the default streams-recipes only contain 1 DataStreamAttrDesc

	for(DataStreamRecipe* stream_recipe : this->stream_recipes){
		for(DataStreamAttrDesc data_stream_attr : stream_recipe->get_attr_descriptions()){
			for(DataStreamRecipe* comp_stream_recipe : subGeom->get_stream_recipes()){
				for(DataStreamAttrDesc comp_data_stream_attr : comp_stream_recipe->get_attr_descriptions()){
					if(data_stream_attr.get_type()==comp_data_stream_attr.get_type()){
						if(data_stream_attr.get_type()==GEOM::data_stream_attr_type::NORMAL){
							// vertex-normals can clash because of different vertex_normals_mode
							if(data_stream_attr.get_type()==GEOM::data_stream_attr_type::NORMAL){
								if(!this->is_compatible_vertex_normal_mode(subGeom))
									return false;
								
								// even if vertex_normal_mode is the same, it can be different if it uses the threshold.
								if((this->get_vertex_normals_mode()==GEOM::vertex_normals_mode::PHONG_FACE_NORMALS)||(this->get_vertex_normals_mode()==GEOM::vertex_normals_mode::PHONG_VERTEX_NORMALS)){
									if(this->get_normal_threshold()!=subGeom->get_normal_threshold())
										return false;
								}
							}
						}
						// if the stream has a channel (like uv streams), we need to check if both streams are targeting same channel-id
						if(data_stream_attr.has_channel()){
							if(data_stream_attr.preserve_channels()){
								// if one of the both channel-ids is empty, they cannot clash.
								if((data_stream_attr.get_channel_id().empty())||(data_stream_attr.get_channel_id().empty()))
									continue;
								else{
									if((data_stream_attr.get_channel_id().empty())!=(data_stream_attr.get_channel_id().empty()))
										return false;
								}
							}
						}
					}
				}
			}
		}
	}		
	// if we got to here, both subGeometrySettings can use the same data
	return true;
}




Settings::Settings(const std::string& root_directory, const std::string& generator_name, const std::string& generator_version):
	BlockSettings(true)
{
	this->export_shapes_in_debug_mode=false;
	this->export_curves=false;
#ifdef _WINDOWS
    this->audio_directory_name = "sounds\\";
    this->texture_directory_name = "textures\\";
#endif
    
#ifdef __APPLE__
    this->audio_directory_name = "sounds/";
    this->texture_directory_name = "textures/";
#endif
	this->file_header_flag = 0;
	this->distinglish_interior_exterior_triangles_2d=false;
	this->keep_verticles_in_path_order=false;
	this->export_materials=false;
	this->export_all=true;
	this->root_directory = root_directory;	
	this->generator_name = generator_name;	
	this->generator_version = generator_version;	
	this->export_invisible_timeline_layer = false;
	this->export_block_categries_order.push_back(BLOCK::category::SCENE_OBJECT);
	this->export_block_categries_order.push_back(BLOCK::category::MATERIALS);
	this->export_block_categries_order.push_back(BLOCK::category::LIGHTS);
	this->export_block_categries_order.push_back(BLOCK::category::ANIMATION);
}

Settings::~Settings()
{
}


void 
Settings::set_export_invisible_timeline_layer(bool export_invisible_timeline_layer)
{
	this->export_invisible_timeline_layer=export_invisible_timeline_layer;
}
bool 
Settings::get_export_invisible_timeline_layer()
{
	return this->export_invisible_timeline_layer;
}

TYPES::UINT16
Settings::get_file_header_flag()
{
	this->file_header_flag=0;
		
	if (this->get_wide_matrix())	//bit1 = storagePrecision Matrix
		this->file_header_flag |= 0x01;
	if (this->get_wide_geom())		//bit2 = storagePrecision Geo
		this->file_header_flag |= 0x02;
	if (this->get_wide_props())		//bit3 = storagePrecision Props
		this->file_header_flag |= 0x04;
	return this->file_header_flag;
}

void 
Settings::set_export_materials(bool export_materials)
{
	this->export_materials=export_materials;
}
bool 
Settings::get_export_materials()
{
	return this->export_materials;
}
bool 
Settings::get_export_all()
{
	return this->export_all;
}
result 
Settings::get_export_block_catergories_order(std::vector<BLOCK::category>& get_categories)
{
	if(this->export_block_categries_order.size()==0)
		return result::AWD_ERROR;
	get_categories=this->export_block_categries_order;
	return result::AWD_SUCCESS;
}

result 
Settings::add_export_block_category(BLOCK::category new_cat)
{
	for(BLOCK::category cat : this->export_block_categries_order){
		if(cat == new_cat)
			return result::AWD_SUCCESS;
	}
	this->export_block_categries_order.push_back(new_cat);
	return result::AWD_SUCCESS;
}

void
Settings::clear_categories()
{
	this->export_block_categries_order.clear();
}

void
Settings::set_file_header_flag(TYPES::UINT16 file_header_flag)
{
	this->file_header_flag=file_header_flag;
}

std::string&
Settings::get_root_directory()
{
	return this->root_directory;
}
void
Settings::set_root_directory(const std::string& root_directory )
{
	this->root_directory=root_directory;
}

std::string&
Settings::get_audio_directory()
{
	this->audio_directory = this->root_directory + this->audio_directory_name;
	FILES::validate_directory_path(this->audio_directory);
	return this->audio_directory;
}

std::string&
Settings::get_audio_directory_name()
{
	return this->audio_directory_name;
}

void
Settings::set_audio_directory_name(const std::string& audio_directory )
{
	this->audio_directory_name=audio_directory;
}

std::string&
Settings::get_texture_directory()
{
	this->texture_directory = this->root_directory + this->texture_directory_name;
	FILES::validate_directory_path(this->texture_directory);
	return this->texture_directory;
}

void 
Settings::set_distinglish_interior_exterior_triangles_2d(bool distinglish_interior_exterior_triangles_2d)
{
	this->distinglish_interior_exterior_triangles_2d = distinglish_interior_exterior_triangles_2d;
}
void 
Settings::set_keep_verticles_in_path_order(bool keep_verticles_in_path_order)
{
	this->keep_verticles_in_path_order = keep_verticles_in_path_order;
}
bool 
Settings::get_distinglish_interior_exterior_triangles_2d()
{
	return this->distinglish_interior_exterior_triangles_2d;
}
bool 
Settings::get_keep_verticles_in_path_order()
{
	return this->keep_verticles_in_path_order;
}
void 
Settings::set_export_curves(bool export_curves)
{
	this->export_curves = export_curves;
}
bool 
Settings::get_export_curves()
{
	return this->export_curves;
}
bool 
Settings::get_export_shapes_in_debug_mode()
{
	return this->export_shapes_in_debug_mode;
}
void 
Settings::set_export_shapes_in_debug_mode(bool export_shapes_in_debug_mode)
{
	this->export_shapes_in_debug_mode=export_shapes_in_debug_mode;
}
std::string&
Settings::get_texture_directory_name()
{
	return this->texture_directory_name;
}

void
Settings::set_texture_directory_name(const std::string& texture_directory )
{
	this->texture_directory_name=texture_directory;
}

Settings*
Settings::clone_settings()
{
	Settings* newSettings = new Settings(this->root_directory, this->generator_name, this->generator_version);
	//todo: apply rest of settings!
	return newSettings;
}

std::string&
Settings::get_generator_version()
{
	return this->generator_version;
}

void
Settings::set_generator_version(const std::string& generator_version )
{
	this->generator_version=generator_version;
}


std::string&
Settings::get_generator_name()
{
	return this->generator_name;
}
void
Settings::set_generator_name(const std::string& generator_name )
{
	this->generator_name=generator_name;
}

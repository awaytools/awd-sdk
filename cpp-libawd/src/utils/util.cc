#include "utils/util.h"

#ifdef _WIN32
#include "Windows.h"
#include "direct.h"
#include "utils/dirent_win.h"
#else
#include "dirent.h"
#include <sys/time.h>
#include <ctime>
#endif
#include <sstream>
#include <fstream>
#include <istream>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sys/stat.h>


#include <string>
#include <cstring>
#include <stdlib.h>
#include "blocks/anim_set.h"
#include "blocks/animator.h"
#include "blocks/bitmap_texture.h"
#include "blocks/billboard.h"
#include "blocks/camera.h"
#include "blocks/command.h"
#include "blocks/cube_texture.h"
#include "blocks/effect_method.h"
#include "blocks/geometry.h"
#include "blocks/light.h"
#include "blocks/light_picker.h"
#include "blocks/material.h"
#include "blocks/mesh_inst.h"
#include "blocks/mesh_library.h"
#include "blocks/meta.h"
#include "blocks/namespace.h"
#include "blocks/objectcontainer.h"
#include "blocks/primitive.h"
#include "blocks/scene.h"
#include "blocks/shadows.h"
#include "blocks/skeleton.h"
#include "blocks/skeleton_anim_clip.h"
#include "blocks/skeleton_pose.h"
#include "blocks/skybox.h"
#include "blocks/texture_projector.h"
#include "blocks/uv_anim_clip.h"
#include "blocks/uv_pose.h"
#include "blocks/vertex_anim_clip.h"
#include "blocks/vertex_pose.h"


using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;


result
BLOCK::get_block_address_for_file(BASE::AWDBlock* awdBlock, FILES::AWDFile* file, TYPES::UINT32& output_address)
{
	BLOCK::BlockInstance* blockInstance_from_Block;
	result res = awdBlock->find_instance_for_file(file, &blockInstance_from_Block);
	if(res==result::AWD_SUCCESS){
		output_address=blockInstance_from_Block->get_addr();
		if(output_address==0){
			return result::AWD_ERROR;
		}
		return result::AWD_SUCCESS;
	}
	return result::AWD_ERROR;
}

result
BLOCK::get_asset_type_as_string(BLOCK::block_type blocktype, std::string& type_str)
{

	switch(blocktype){
		case BLOCK::block_type::ANIMATION_SET:
			type_str="ANIMATION_SET";
			break;
		case BLOCK::block_type::ANIMATOR:
			type_str="ANIMATOR";
			break;
		case BLOCK::block_type::BITMAP_TEXTURE:
			type_str="BITMAP_TEXTURE";
			break;
		case BLOCK::block_type::BILLBOARD:
			type_str="BILLBOARD";
			break;
		case BLOCK::block_type::CAMERA:
			type_str="CAMERA";
			break;
		case BLOCK::block_type::COMMAND:
			type_str="COMMAND";
			break;
		case BLOCK::block_type::CUBE_TEXTURE:
			type_str="CUBE_TEXTURE";
			break;
		case BLOCK::block_type::EFFECT_METHOD:
			type_str="EFFECT_METHOD";
			break;
		case BLOCK::block_type::FONT:
			type_str="FONT";
			break;
		case BLOCK::block_type::FORMAT:
			type_str="FORMAT";
			break;
		case BLOCK::block_type::TRI_GEOM:
			type_str="TRI_GEOM";
			break;
		case BLOCK::block_type::LIGHT:
			type_str="LIGHT";
			break;
		case BLOCK::block_type::LIGHTPICKER:
			type_str="LIGHTPICKER";
			break;
		case BLOCK::block_type::SIMPLE_MATERIAL:
			type_str="SIMPLE_MATERIAL";
			break;
		case BLOCK::block_type::MESH_INSTANCE:
			type_str="MESH_INSTANCE";
			break;
		case BLOCK::block_type::MESH_INSTANCE_2:
			type_str="MESH_INSTANCE_2";
			break;
		case BLOCK::block_type::METADATA:
			type_str="METADATA";
			break;
		case BLOCK::block_type::NAMESPACE:
			type_str="NAMESPACE";
			break;
		case BLOCK::block_type::CONTAINER:
			type_str="CONTAINER";
			break;
		case BLOCK::block_type::PRIM_GEOM:
			type_str="PRIM_GEOM";
			break;
		case BLOCK::block_type::SHADOW_METHOD:
			type_str="SHADOW_METHOD";
			break;
		case BLOCK::block_type::SKELETON:
			type_str="SKELETON";
			break;
		case BLOCK::block_type::SKELETON_ANIM:
			type_str="SKELETON_ANIM";
			break;
		case BLOCK::block_type::SKELETON_POSE:
			type_str="SKELETON_POSE";
			break;
		case BLOCK::block_type::SKYBOX:
			type_str="SKYBOX";
			break;
		case BLOCK::block_type::SOUND_SOURCE:
			type_str="SOUND_SOURCE";
			break;
		case BLOCK::block_type::TEXT_ELEMENT:
			type_str="TEXT_ELEMENT";
			break;
		case BLOCK::block_type::TEXTURE_PROJECTOR:
			type_str="TEXTURE_PROJECTOR";
			break;
		case BLOCK::block_type::TIMELINE:
			type_str="TIMELINE";
			break;
		case BLOCK::block_type::UV_ANIM:
			type_str="UV_ANIM";
			break;
		case BLOCK::block_type::VERTEX_POSE:
			type_str="VERTEX_POSE";
			break;
		case BLOCK::block_type::VERTEX_ANIM:
			type_str="VERTEX_ANIM";
			break;
		default:
			type_str="";
			return result::AWD_ERROR;
	}

	return result::AWD_SUCCESS;
}
result
BLOCK::create_block_for_block_type(BASE::AWDBlock** awdBlock, BLOCK::block_type blocktype, FILES::AWDFile* awd_file)
{
	switch(blocktype){
		case BLOCK::block_type::ANIMATION_SET:
			*awdBlock = new BLOCKS::AnimationSet();
			break;
		case BLOCK::block_type::ANIMATOR:
			*awdBlock = new BLOCKS::Animator();
			break;
		case BLOCK::block_type::BITMAP_TEXTURE:
			*awdBlock = new BLOCKS::BitmapTexture();
			break;
		case BLOCK::block_type::BILLBOARD:
			*awdBlock = new BLOCKS::Billboard();
			break;
		case BLOCK::block_type::CAMERA:
			*awdBlock = new BLOCKS::Camera();
			break;
		case BLOCK::block_type::COMMAND:
			*awdBlock = new BLOCKS::CommandBlock();
			break;
		case BLOCK::block_type::CUBE_TEXTURE:
			*awdBlock = new BLOCKS::CommandBlock();
			break;
		case BLOCK::block_type::EFFECT_METHOD:
			*awdBlock = new BLOCKS::EffectMethod();
			break;
		case BLOCK::block_type::FONT:
			*awdBlock = new BLOCKS::Font();
			break;
		case BLOCK::block_type::FORMAT:
			*awdBlock = new BLOCKS::TextFormat();
			break;
		case BLOCK::block_type::TRI_GEOM:
			*awdBlock = new BLOCKS::Geometry();
			break;
		case BLOCK::block_type::LIGHT:
			*awdBlock = new BLOCKS::Light();
			break;
		case BLOCK::block_type::LIGHTPICKER:
			*awdBlock = new BLOCKS::LightPicker();
			break;
		case BLOCK::block_type::SIMPLE_MATERIAL:
			*awdBlock = new BLOCKS::Material();
			break;
		case BLOCK::block_type::MESH_INSTANCE:
			*awdBlock = new BLOCKS::Mesh();
			break;
		case BLOCK::block_type::MESH_INSTANCE_2:
			*awdBlock = new BLOCKS::MeshLibrary();
			break;
		case BLOCK::block_type::METADATA:
			*awdBlock = new BLOCKS::MetaData(awd_file->get_settings()->get_generator_name(), awd_file->get_settings()->get_generator_version());
			break;
		case BLOCK::block_type::NAMESPACE:
			*awdBlock = new BLOCKS::Namespace();
			break;
		case BLOCK::block_type::CONTAINER:
			*awdBlock = new BLOCKS::ObjectContainer3D();
			break;
		case BLOCK::block_type::PRIM_GEOM:
			*awdBlock = new BLOCKS::Primitive();
			break;
		case BLOCK::block_type::SHADOW_METHOD:
			*awdBlock = new BLOCKS::ShadowMethod();
			break;
		case BLOCK::block_type::SKELETON:
			*awdBlock = new BLOCKS::Skeleton();
			break;
		case BLOCK::block_type::SKELETON_ANIM:
			*awdBlock = new BLOCKS::SkeletonAnimationClip();
			break;
		case BLOCK::block_type::SKELETON_POSE:
			*awdBlock = new BLOCKS::SkeletonPose();
			break;
		case BLOCK::block_type::SKYBOX:
			*awdBlock = new BLOCKS::SkyBox();
			break;
		case BLOCK::block_type::SOUND_SOURCE:
			*awdBlock = new BLOCKS::Audio();
			break;
		case BLOCK::block_type::TEXT_ELEMENT:
			*awdBlock = new BLOCKS::TextElement();
			break;
		case BLOCK::block_type::TEXTURE_PROJECTOR:
			*awdBlock = new BLOCKS::TextureProjector();
			break;
		case BLOCK::block_type::TIMELINE:
			*awdBlock = new BLOCKS::Timeline();
			break;
		case BLOCK::block_type::UV_ANIM:
			*awdBlock = new BLOCKS::UVAnimationClip();
			break;
		case BLOCK::block_type::VERTEX_POSE:
			*awdBlock = new BLOCKS::VertexPose();
			break;
		case BLOCK::block_type::VERTEX_ANIM:
			*awdBlock = new BLOCKS::VertexAnimationClip();
			break;
		default:
			return result::AWD_ERROR;
	}

	return result::AWD_SUCCESS;
}

result
AWD::get_string_for_result(AWD::result the_result, std::string& outString)
{
	switch (the_result) {
		case AWD::result::AWD_SUCCESS:
			outString="AWD opperation was successful";
			return AWD::result::AWD_SUCCESS;
			break;
		case AWD::result::FILE_ALREADY_EXISTS:
			outString="AWD file already exists";
			return AWD::result::AWD_SUCCESS;
			break;
		default:
			return AWD::result::RESULT_STRING_NOT_FOUND;
	}
}

result 
FILES::string_find_and_replace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
  return result::AWD_SUCCESS;
}



result 
FILES::copy_files_from_directory(std::string& source_dir, std::string& target_dir, const std::string& exclude_filename)
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (source_dir.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name!=NULL){
                std::string thisname=ent->d_name;
                if((thisname!=".")&&(thisname!="..")&&(thisname!=exclude_filename)){
                    std::string extension;
                    std::string complete_path_source = source_dir + "/"+ thisname;
                    std::string complete_path_target = target_dir + "/"+ thisname;
                    result res = FILES::extract_file_extension(ent->d_name, extension);
                    if(res==result::AWD_SUCCESS){
                        if(extension=="protected_js"){
                            std::string path_no_extension;
                            FILES::extract_path_without_file_extension(complete_path_target, path_no_extension);
                            complete_path_target=path_no_extension+".js";
                        }
                        res = copy_file(complete_path_source, complete_path_target);
                        if(res!=result::AWD_SUCCESS)
                            return res;
                    }
                    else{
                        res=copy_files_from_directory(complete_path_source, complete_path_target, "");
                        if(res!=result::AWD_SUCCESS)
                            return res;
                    }
                }
            }
		}
		closedir (dir);
		return result::AWD_SUCCESS;
	} 
	else
		return result::AWDFILE_ERROR;

	return result::AWD_SUCCESS;
}


/**
\brief Copies a given folder with preview files to the output directory of a file.
The specified preview_file will be copied and a its content will be searched for the strings specified by change_ids. If any of this strings is found, it will be replaced by the according change_values
*/
result 
FILES::open_preview(FILES::AWDFile* output_file, std::string& preview_file, std::vector<std::string> change_ids, std::vector<std::string> change_values, std::string& open_path, bool append_name)
{
	result res = result::AWD_SUCCESS;

	// validate input

	if(output_file==NULL)
		return result::AWDFILE_ERROR;
	if(preview_file.empty())
		return result::AWDFILE_ERROR;

	if(change_ids.size()!=change_values.size())
		return result::AWDFILE_ERROR;
	
	// check if source_directory and output_directory exists

	std::string out_root_directory;
	res = FILES::extract_directory_from_path(output_file->get_url(), out_root_directory);
	if(res!=result::AWD_SUCCESS)
		return res;

	std::string preview_root_directory;
	res = FILES::extract_directory_from_path(preview_file, preview_root_directory);
	if(res!=result::AWD_SUCCESS)
		return res;

	std::string preview_filename;
	res = FILES::extract_name_from_path(preview_file, preview_filename);
	if(res!=result::AWD_SUCCESS)
		return res;
	
	// recursivly copy all files from source-directory to output-directory, except for the preview-file (index.html)
	res = copy_files_from_directory(preview_root_directory, out_root_directory, preview_filename);

	
	// modify the preview_file, and save it with updated name. (leave extension untouched)
	
    std::ifstream inFile(preview_file.c_str());
    if(!inFile){
		return result::FILE_NOT_FOUND;
    }
	
	std::string all_content;
	inFile.seekg(0, std::ios::end);   
	all_content.reserve(inFile.tellg());
	inFile.seekg(0, std::ios::beg);

	all_content.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

	int cnt=0;
	for(std::string change_id : change_ids){
		string_find_and_replace(all_content, change_id, change_values[cnt]);
		cnt++;
	}  
    inFile.close();
	
	std::string output_path_no_extension;
	res=extract_path_without_file_extension(output_file->get_url(), output_path_no_extension);
	if(res!=result::AWD_SUCCESS)
		return res;

	std::string extension;
	res=extract_file_extension(preview_file, extension);
	if(res!=result::AWD_SUCCESS)
		return res;
	
	std::string preview_file_output = output_path_no_extension + "."+ extension;

    std::ofstream outFile(preview_file_output.c_str());
    if(!outFile){
		return result::FILE_NOT_FOUND;
    }
    outFile << all_content;
    outFile.close();
	
	if(open_path.size()>0){
		preview_file_output=open_path;
		if(append_name)
			preview_file_output = open_path + "/" + preview_filename;
	}

	// open the preview_file

#ifdef _WIN32
		
#ifdef _UNICODE
	std::wstring tail;
	tail.assign(preview_file_output.begin(), preview_file_output.end());
	ShellExecute(NULL, L"open", tail.c_str(), NULL, NULL, SW_SHOWNORMAL);

#else
	std::string tail;
	tail.assign(preview_file_output.begin(), preview_file_output.end());
	ShellExecute(NULL, "open", tail.c_str(), NULL, NULL, SW_SHOWNORMAL);

#endif
#else
    std::string output = preview_file_output;
    std::string str = "/usr/bin/open " + output;
    system(str.c_str());

#endif



	return res; 
}

result 
FILES::copy_file(const std::string& initialFilePath, const std::string& outputFilePath)
{
	//Make sure the output directory exist:
	std::string extracted;
	result res = extract_directory_from_path(outputFilePath, extracted);
	if(res!=result::AWD_SUCCESS)
		return res;
	
	//copy the file:
    std::ifstream inFile(initialFilePath.c_str(), std::ifstream::in | std::ifstream::binary);
    std::ofstream outFile(outputFilePath.c_str(), std::ofstream::out | std::ofstream::binary);
    
    char *buffer = NULL;
    long length = 0;
    if(!inFile){
		return result::FILE_NOT_FOUND;
    }
    if(!outFile){
		return result::FILE_NOT_FOUND;
    }
    if ((inFile)&&(outFile))
    {
        inFile.seekg (0, inFile.end);
        length = (long)inFile.tellg();
        
        inFile.seekg (0, inFile.beg);
        buffer = new char [length + 1];
        inFile.read (buffer, length);
        buffer[length] = 0;
		outFile.write((char*)buffer, length);
    }
    inFile.close();
    outFile.close();

	return result::AWD_SUCCESS;
}

result 
FILES::validate_file_path(std::string& path, const std::string& extension)
{
	std::string no_extension;
	result res=extract_path_without_file_extension(path, no_extension);
	if(res!=result::AWD_SUCCESS)
		return res;

	std::string fileName;
	res=extract_name_from_path(no_extension, fileName);
	if(res!=result::AWD_SUCCESS)
		return res;

	std::string only_path;
	res=extract_directory_from_path(path, only_path);
	if(res!=result::AWD_SUCCESS)
		return res;

	path = only_path + "/" + fileName + "." + extension;
	return result::AWD_SUCCESS;
}

result 
FILES::extract_file_extension(const std::string& path, std::string& extension)
{
	size_t index = path.find_last_of(".");
	extension = "";
	if (index != std::string::npos)
	{
		extension = path.substr(index + 1, path.length());
	}
	if(extension.size()==0)
		return result::AWDFILE_ERROR;

	return result::AWD_SUCCESS;
}

result 
FILES::extract_name_from_path(const std::string& path, std::string& fileName)
{
	size_t index = path.find_last_of("/\\");
	fileName = path.substr(index + 1, path.length() - index - 1);
	if(fileName.empty()){
		return result::AWDFILE_ERROR;
	}
	return result::AWD_SUCCESS;
}

result 
FILES::extract_path_without_file_extension(const std::string& path, std::string& path_no_ext)
{
	// Remove the extension (if any)
	size_t index = path.find_last_of(".");
	if (index != std::string::npos)
	{
		path_no_ext = path.substr(0, index);
	}
	if(path_no_ext.empty()){
		return result::AWDFILE_ERROR;
	}
	return result::AWD_SUCCESS;
}

result 
FILES::validate_directory_path(const std::string& path)
{
	if(path.empty())
		return result::AWD_ERROR;
	int success=0;
#ifdef _WIN32
		success=mkdir(path.c_str());
#else
        success=mkdir(path.c_str(), 0777);
#endif
	if(success!=0){
		if(errno==EEXIST)
			return result::AWD_SUCCESS;
		return result::AWDFILE_ERROR;
	}
	return result::AWD_SUCCESS;
}
result 
FILES::extract_directory_from_path(const std::string& path, std::string& base_path)
{
	size_t index = path.find_last_of("/\\");
    base_path = path.substr(0, index + 1);
	if(base_path.empty())
		return result::AWD_ERROR;
	return validate_directory_path(base_path);
}

TYPES::UINT32
TYPES::get_data_type_size_for_precision_category(data_types type, SETTINGS::BlockSettings* settings, TYPES::storage_precision_category storage_cat)
{
	bool storage_precision=false;
	if(storage_cat==storage_precision_category::GEOMETRY_VALUES)
		storage_precision=settings->get_wide_geom();
	else if(storage_cat==storage_precision_category::FORCE_PRECISION)
		storage_precision=true;
	else if(storage_cat==storage_precision_category::MATRIX_VALUES)
		storage_precision=settings->get_wide_matrix();
	else if(storage_cat==storage_precision_category::PROPERIES_VALUES)
		storage_precision=settings->get_wide_props();
	else if(storage_cat==storage_precision_category::ATTRIBUTES_VALUES)
		storage_precision=settings->get_wide_attributes();

	return get_data_type_size(type, storage_precision);
}

TYPES::UINT32 
TYPES::get_single_val_data_type_size_for_precision_category(data_types type, SETTINGS::BlockSettings* settings, TYPES::storage_precision_category storage_cat)
{
	bool storage_precision=false;
	if(storage_cat==storage_precision_category::GEOMETRY_VALUES)
		storage_precision=settings->get_wide_geom();
	else if(storage_cat==storage_precision_category::FORCE_PRECISION)
		storage_precision=true;
	else if(storage_cat==storage_precision_category::MATRIX_VALUES)
		storage_precision=settings->get_wide_matrix();
	else if(storage_cat==storage_precision_category::PROPERIES_VALUES)
		storage_precision=settings->get_wide_props();
	else if(storage_cat==storage_precision_category::ATTRIBUTES_VALUES)
		storage_precision=settings->get_wide_attributes();

	return get_single_val_data_type_size(type, storage_precision);
}

TYPES::UINT32
TYPES::get_data_type_size(data_types type, bool storage_precision)
{
	TYPES::UINT32 elem_size=0;

	switch (type) {
		case data_types::INT8:
		case data_types::BOOL:
		case data_types::UINT8:
			elem_size = sizeof(INT8);
			break;

		case data_types::INT16:
		case data_types::UINT16:
			elem_size = sizeof(TYPES::INT16);
			break;

		case data_types::INT32:
		case data_types::UINT32:
		case data_types::COLOR:
		case data_types::BADDR:
			elem_size = sizeof(TYPES::INT32);
			break;
			
		case data_types::FLOAT32:
		case data_types::FLOAT64:
			if(storage_precision)
				elem_size = sizeof(TYPES::F64);
			else
				elem_size = sizeof(TYPES::F32);
			break;

		case data_types::VECTOR2x1:
			elem_size = VEC2_SIZE(storage_precision);
			break;

		case data_types::VECTOR3x1:
			elem_size = VEC3_SIZE(storage_precision);
			break;

		case data_types::VECTOR4x1:
			elem_size = VEC4_SIZE(storage_precision);
			break;

		case data_types::MTX3x2:
			elem_size = MTX32_SIZE(storage_precision);
			break;

		case data_types::MTX3x3:
			elem_size = MTX33_SIZE(storage_precision);
			break;

		case data_types::MTX4x3:
			elem_size = MTX43_SIZE(storage_precision);
			break;

		case data_types::MTX4x4:
			elem_size = MTX44_SIZE(storage_precision);
			break;
            
		case data_types::MTX5x4:
			elem_size = MTX54_SIZE(storage_precision);
			break;

		case data_types::STRING:
		case data_types::BYTEARRAY:
			// Can't know
			elem_size = 0;
			break;
	}

	return elem_size;
}

TYPES::UINT32
TYPES::get_single_val_data_type_size(data_types type, bool storage_precision)
{
	TYPES::UINT32 elem_size=0;

	switch (type) {
		case data_types::INT8:
		case data_types::BOOL:
		case data_types::UINT8:
			elem_size = sizeof(INT8);
			break;

		case data_types::INT16:
		case data_types::UINT16:
			elem_size = sizeof(TYPES::INT16);
			break;

		case data_types::INT32:
		case data_types::UINT32:
		case data_types::COLOR:
		case data_types::BADDR:
			elem_size = sizeof(TYPES::INT32);
			break;
			
		case data_types::FLOAT32:
		case data_types::FLOAT64:
		case data_types::VECTOR2x1:
		case data_types::VECTOR3x1:
		case data_types::VECTOR4x1:
		case data_types::MTX3x2:
		case data_types::MTX3x3:
		case data_types::MTX4x3:
		case data_types::MTX4x4:
		case data_types::MTX5x4:
			if(storage_precision)
				elem_size = sizeof(TYPES::F64);
			else
				elem_size = sizeof(TYPES::F32);
			break;

		case data_types::STRING:
		case data_types::BYTEARRAY:
			// Can't know
			elem_size = 0;
			break;
	}

	return elem_size;
}

bool
TYPES::is_data_type_floats(data_types type)
{
	switch (type) {
		case data_types::FLOAT32:
		case data_types::FLOAT64:
		case data_types::VECTOR2x1:
		case data_types::VECTOR3x1:
		case data_types::VECTOR4x1:
		case data_types::MTX3x2:
		case data_types::MTX3x3:
		case data_types::MTX4x3:
		case data_types::MTX4x4:
			return true;
        default:
            return false;

	}	
	return false;
}


COLOR
TYPES::create_color_from_floats(TYPES::F64 r, TYPES::F64 g, TYPES::F64 b, TYPES::F64 a)
{
	return create_color_from_ints((TYPES::UINT32)(r*255), (TYPES::UINT32)(g*255), (TYPES::UINT32)(b*255), (TYPES::UINT32)(a*255));
}

COLOR
TYPES::create_color_from_ints(TYPES::UINT32 r, TYPES::UINT32 g, TYPES::UINT32 b, TYPES::UINT32 a)
{
	return ((r&0xff)<<24) | ((g&0xff)<<16) | ((b&0xff)<<8) | (a&0xff);
}


TYPES::UINT16
FILES::swapui16(TYPES::UINT16 n)
{
	union {
		TYPES::UINT16 i;
		TYPES::UINT8 b[2];
	} in, out;

	in.i = n;

	out.b[0] = in.b[1];
	out.b[1] = in.b[0];

	return out.i;
}

TYPES::UINT32
FILES::swapui32(TYPES::UINT32 n)
{
	union {
		TYPES::UINT32 i;
		TYPES::UINT8 b[4];
	} in, out;

	in.i = n;
	out.b[0] = in.b[3];
	out.b[1] = in.b[2];
	out.b[2] = in.b[1];
	out.b[3] = in.b[0];

	return out.i;
}

TYPES::F32
FILES::swapf32(TYPES::F32 n)
{
	union {
		TYPES::F32 f;
		TYPES::UINT8 b[4];
	} in, out;

	in.f = n;
	out.b[0] = in.b[3];
	out.b[1] = in.b[2];
	out.b[2] = in.b[1];
	out.b[3] = in.b[0];

	return out.f;
}

TYPES::F64
FILES::swapf64(TYPES::F64 n)
{
	union {
		TYPES::F64 f;
		TYPES::UINT8 b[8];
	} in, out;

	in.f = n;
	out.b[0] = in.b[7];
	out.b[1] = in.b[6];
	out.b[2] = in.b[5];
	out.b[3] = in.b[4];
	out.b[4] = in.b[3];
	out.b[5] = in.b[2];
	out.b[6] = in.b[1];
	out.b[7] = in.b[0];

	return out.f;
}

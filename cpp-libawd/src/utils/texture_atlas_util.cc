#include "utils/util.h"



#include <string>
#include <cstring>
#include <stdlib.h>
#include "blocks/anim_set.h"
#include "blocks/animator.h"
#include "blocks/bitmap_texture.h"
#include "blocks/camera.h"
#include "blocks/command.h"
#include "blocks/cube_texture.h"
#include "blocks/effect_method.h"
#include "blocks/geometry.h"
#include "blocks/light.h"
#include "blocks/light_picker.h"
#include "blocks/material.h"
#include "blocks/mesh_inst.h"
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

#include "lodepng.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

#include <math.h>

inline double get_max_val(double x, double y, double z) {
	double max_val = x; 
	if (y > max_val) 
		max_val = y;
	if (z > max_val) 
		max_val = z;
	return max_val; 
} 
inline double get_min_val(double x, double y, double z) {
	double min_val = x; 
	if (y < min_val)
		min_val = y;
	if (z < min_val) 
		min_val = z;
	return min_val; 
} 

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v )
{
	float min, max, delta;
	min = get_min_val( r, g, b );
	max = get_max_val( r, g, b );
	*v = max;				// v

	delta = max - min;

	if( max != 0 )
		*s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if( r == max )
		*h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		*h = 4 + ( r - g ) / delta;	// between magenta & cyan

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;

}
GEOM::VECTOR2D interceptOnCircle(GEOM::VECTOR2D p1,GEOM::VECTOR2D p2, GEOM::VECTOR2D CircleCentre, double r, bool positive){
    //p1 is the first line point
    //p2 is the second line point
    //c is the circle's center
    //r is the circle's radius
	GEOM::VECTOR2D LocalP1 = GEOM::VECTOR2D(p1.x - CircleCentre.x, p1.y - CircleCentre.y);
	GEOM::VECTOR2D LocalP2 = GEOM::VECTOR2D(p2.x - CircleCentre.x, p2.y - CircleCentre.y);
	GEOM::VECTOR2D P2MinusP1 = GEOM::VECTOR2D(LocalP2.x - LocalP1.x, LocalP2.y - LocalP1.y);

	double a = P2MinusP1.x * P2MinusP1.x + P2MinusP1.y * P2MinusP1.y;
	double b = 2 * ( ( P2MinusP1.x * LocalP1.x ) + ( P2MinusP1.y * LocalP1.y ) );
	double c = ( LocalP1.x * LocalP1.x ) + ( LocalP1.y * LocalP1.y ) - (r * r);
	double delta = (b * b ) - (4 * a * c);
	if (delta < 0) // No intersection
		return GEOM::VECTOR2D(-1, -1);
	else if (delta == 0){ // One intersection
		double u = -(b / double((2 * a)));
		GEOM::VECTOR2D output1 = GEOM::VECTOR2D(p1.x+ (u * P2MinusP1.x), p1.y+ (u * P2MinusP1.y) );
		return output1;
	}
	else if (delta > 0){ // Two intersections
		double SquareRootDelta = sqrt(delta);
		double u1 = (-b + SquareRootDelta) / double((2 * a));
		double u2 = (-b - SquareRootDelta) / double((2 * a));
		GEOM::VECTOR2D output1 = GEOM::VECTOR2D(p1.x+ (u1 * P2MinusP1.x), p1.y+ (u1 * P2MinusP1.y) );
		GEOM::VECTOR2D output2 = GEOM::VECTOR2D(p1.x+ (u2 * P2MinusP1.x), p1.y+ (u2 * P2MinusP1.y) );
		if(positive){
			if(output1.y>r){
				return output1;
			}
			else if(output2.y>r){
				return output2;
			}
		}
		else{
			if(output1.y<r){
				return output1;
			}
			else if(output2.y<r){
				return output2;
			}
		}
	}
	return GEOM::VECTOR2D(-1, -1);

		
}
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}

}
void GetColorsFromRgbInt ( int nRGBVal, float& r, float& g, float& b, float& a) {

    r = nRGBVal & 7;
    g = (nRGBVal >> 8) & 7;
    b = (nRGBVal >> 16) & 7;
    a = (nRGBVal >> 24) & 7;
}
result 
AWD::create_TextureAtlasfor_timelines(AWDProject* awd_project, const std::string& name)
{
	return result::AWD_SUCCESS;
}
result 
AWD::create_TextureAtlasfor_timelines_refactor(AWDProject* awd_project, const std::string& name)
{
	int maxTexSize=8192;
	int minSize=4;
	
	std::vector<AWDBlock*> all_materials;
	awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);

	int pixelCount = 0;
	int width=minSize;
	bool has_radial_gradients=false;
	bool has_linear_gradients=false;
	bool has_texture_atlas=false;
	bool has_alpha_texture_atlas=false;
	bool has_bitmaps_atlas=false;
	for(AWDBlock* one_block:all_materials){
		BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
		if(this_mat!=NULL){
			if(this_mat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL){
				this_mat->set_state(state::INVALID);
				pixelCount++;
				if(this_mat->needsAlphaTex)
					has_alpha_texture_atlas=true;
				else
					has_texture_atlas=true;
			}
			else if(this_mat->get_material_type()==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL){
				this_mat->set_state(state::INVALID);
				if(width<256)
					width=256;
				pixelCount+=256;
				has_linear_gradients=true;
				if(this_mat->needsAlphaTex)
					has_alpha_texture_atlas=true;
				else
					has_texture_atlas=true;
			}
			else if(this_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
				this_mat->set_state(state::INVALID);
				has_radial_gradients=true;
				has_linear_gradients=true;
				if(width<256)
					width=256;
				pixelCount+=256;
				if(this_mat->needsAlphaTex)
					has_alpha_texture_atlas=true;
				else
					has_texture_atlas=true;
			}
			else if(this_mat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
				has_bitmaps_atlas=true;
				//if(width<256)
				//	width=256;
				//pixelCount+=256;
				this_mat->texture_u = 0;
				this_mat->texture_v = 0;
				this_mat->texture_u_max = 1;
				this_mat->texture_v_max = 1;
			}
		}
	}
	
	/*
	
	if(has_radial_gradients){
		int colorposition = 0;
		std::vector<unsigned char> image;
		image.resize(width * width * 4);
		int radial_cnt = 0;
		int width_radial=awd_project->get_settings()->get_radial_gradient_size();
		int width_radial_half=width_radial/2;
		double offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if(this_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
					radial_cnt++;
					std::string url_final_radial=awd_project->get_settings()->get_texture_directory()+ name + "_radial_gradient_"+std::to_string(radial_cnt) + ".png";
					std::string rel_url_radial="./"+awd_project->get_settings()->get_texture_directory_name()+ name + "_radial_gradient_"+std::to_string(radial_cnt) + ".png";
					std::string name="radial_gradient_"+std::to_string(radial_cnt) + ".png";
					BLOCKS::BitmapTexture* newtexture_radial = reinterpret_cast<BLOCKS::BitmapTexture*>(awd_project->get_block_by_name_and_type(name, BLOCK::block_type::BITMAP_TEXTURE, true));
					newtexture_radial->add_scene_name("Textureatlas");
					if(awd_project->get_settings()->get_embbed_textures()){
						newtexture_radial->set_storage_type(BLOCK::storage_type::EMBEDDED);
					}
					else{
						newtexture_radial->set_storage_type(BLOCK::storage_type::EXTERNAL);
					}
					this_mat->set_name("mat_"+name);
					newtexture_radial->set_name(name);
					newtexture_radial->set_input_url(url_final_radial);
					newtexture_radial->set_url(rel_url_radial);
					this_mat->set_texture(newtexture_radial);
					int colorposition_radial = 0;
					//double offset_half_pixel = double(0.5)/double(width_radial); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders
					std::vector<unsigned char> image_radial;
					image_radial.resize(width_radial * width_radial * 4);
					
					this_mat->texture_u = 0;
					this_mat->texture_v = 0;
					this_mat->texture_u_max = 1;
					this_mat->texture_v_max = 1;
					// adobe focalpoint has range -255 to 255
					double focal1 = (double(this_mat->focalPoint + 255)/double(510))*width_radial;
					// our focalpoint now has range 0 - 256. 128 being the middle
					GEOM::VECTOR2D focalpoint = GEOM::VECTOR2D(focal1, width_radial_half);
					if(focal1 == width_radial_half){
						for(int i=0; i<width_radial; i++){
							for(int k=0; k<width_radial; k++){
								double distance_to_focal = (sqrt(((width_radial_half-(i+0.5))*(width_radial_half-(i+0.5)))+((width_radial_half-(k+0.5))*(width_radial_half-(k+0.5))))/double(width_radial_half))*255;
								GEOM::VECTOR4D new_color=this_mat->get_interpolated_color(distance_to_focal);
								image_radial[4 * ((i*width_radial)+k)] = new_color.x;
								image_radial[4 * ((i*width_radial)+k)+ 1] = new_color.y;
								image_radial[4 * ((i*width_radial)+k)+ 2] = new_color.z;
								image_radial[4 * ((i*width_radial)+k)+ 3] = new_color.w;
							}
						}
					}
					else{
						for(int i=0; i<width_radial; i++){
							for(int k=0; k<width_radial; k++){
								// check distance of current pixel to circle center 
								TYPES::UINT32 distance_to_middle= sqrt(((width_radial_half-i)*(width_radial_half-i))+((width_radial_half-k)*(width_radial_half-k)));
								GEOM::VECTOR4D new_color;
								//focalpoint;
								if(distance_to_middle<width_radial_half){
									GEOM::VECTOR2D intercept_point = GEOM::VECTOR2D();
									if(i!=width_radial_half)
										intercept_point = interceptOnCircle(focalpoint, GEOM::VECTOR2D(k+0.5, i+0.5), GEOM::VECTOR2D(width_radial_half, width_radial_half), width_radial_half, (i>width_radial_half));
									else{
										if(k<focalpoint.x)
											intercept_point = GEOM::VECTOR2D(0, width_radial_half);
										else
											intercept_point = GEOM::VECTOR2D(width_radial-1, width_radial_half);
									}

									double distance_intercept_to_focal= sqrt(((focalpoint.x-intercept_point.x)*(focalpoint.x-intercept_point.x))+((width_radial_half-intercept_point.y)*(width_radial_half-intercept_point.y)));
									double distance_to_focal= sqrt(((focalpoint.x-(k+0.5))*(focalpoint.x-(k+0.5)))+((width_radial_half-(i+0.5))*(width_radial_half-(i+0.5))));
									double color_pos = (double(distance_to_focal)/double(distance_intercept_to_focal))*255;
									new_color=this_mat->get_interpolated_color(color_pos);
								}
								else{
									new_color=this_mat->get_interpolated_color(255);
								}
								image_radial[4 * ((i*width_radial)+k)] = new_color.x;
								image_radial[4 * ((i*width_radial)+k)+ 1] = new_color.y;
								image_radial[4 * ((i*width_radial)+k)+ 2] = new_color.z;
								image_radial[4 * ((i*width_radial)+k)+ 3] = new_color.w;
							}
						}
					}
					lodepng::encode(url_final_radial, image_radial, width_radial, width_radial);
				}
			}
		}
	}
	*/
	BLOCKS::BitmapTexture* newtextureAlpha=NULL;
	if(has_alpha_texture_atlas){
		while ((width*width)<pixelCount)
			width=width*2;

		if(width>maxTexSize){
			// todo: we cannot create one texture_atlas for all materials. 
			// start with the biggest material (in pixel-size)
			// get the timelines thats using this.
			// collect all materials that are connected (e.g. used by same shape / child timelines)
			// if textureatlas is full, create a new one and continue.
			// take care that each geometry  uses only 1 texture (e.g. als subgeos of 1 geometry need to use same texture)
			return result::AWD_ERROR;
		}
	
		std::string url_final=awd_project->get_settings()->get_texture_directory()+ name + "_textureAtlasAlpha.png";
		std::string rel_url="./"+awd_project->get_settings()->get_texture_directory_name()+ name + "_textureAtlasAlpha.png";
		newtextureAlpha = reinterpret_cast<BLOCKS::BitmapTexture*>(awd_project->get_block_by_name_and_type(url_final, BLOCK::block_type::BITMAP_TEXTURE, true));
		newtextureAlpha->set_input_url(url_final);
		newtextureAlpha->set_url(rel_url);
		newtextureAlpha->add_scene_name("TextureatlasAlpha");
		newtextureAlpha->set_name(name + "_textureAtlasAlpha");
		if(awd_project->get_settings()->get_bool(SETTINGS::bool_settings::EmbbedTextures)){
			newtextureAlpha->set_storage_type(BLOCK::storage_type::EMBEDDED);
		}
		else{
			newtextureAlpha->set_storage_type(BLOCK::storage_type::EXTERNAL);
		}
		int colorposition = 0;
		std::vector<unsigned char> image;
		image.resize(width * width * 4);
		int radial_cnt = 0;
		int width_radial=256;
		double offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders	
		int gradients_per_row=floor(double(width)/double(256));
		int gradients_per_row_cnt=0;
		int row_cnt=0;
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if((this_mat->get_material_type()==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL)||(this_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL)){
					if(this_mat->needsAlphaTex){
						this_mat->set_texture(newtextureAlpha);
						if(this_mat->gradient_colors.size()<2){
							//this is a error;
							return result::AWD_SUCCESS;
						}
						double v_value = double(colorposition)/double(width);
						double v_value_int=0;
						this_mat->texture_u = modf(v_value, &v_value_int) + offset_half_pixel;
						this_mat->texture_v = double(v_value_int)/double(width) + offset_half_pixel;
						for(int i=0; i<256; i++){
							GEOM::VECTOR4D new_color=this_mat->get_interpolated_color(i);
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))] =  new_color.x;
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 1] = new_color.y;
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 2] = new_color.z;
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 3] = new_color.w;
							colorposition++;

						}
						double v_value2 = double(colorposition-1)/double(width);
						double v_value_int2=0;
						this_mat->texture_u_max = modf(v_value2, &v_value_int2) + offset_half_pixel;
						this_mat->texture_v_max = double(v_value_int2)/double(width) + offset_half_pixel;
					
						gradients_per_row_cnt++;
						if(gradients_per_row_cnt>=gradients_per_row){
							row_cnt++;
							gradients_per_row_cnt=0;
						}
					}
				}
			}
		}
		offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if(this_mat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL){
					if(this_mat->needsAlphaTex){
						this_mat->set_texture(newtextureAlpha);
						if(this_mat->get_color()==0xff000000){
							image[4 * colorposition] =		0;//(unsigned int)this_mat->get_color_components().x;
							image[4 * colorposition + 1] =  0;//(unsigned int)this_mat->get_color_components().y;
							image[4 * colorposition + 2] =  0;//(unsigned int)this_mat->get_color_components().z;
							image[4 * colorposition + 3] =  255;//(unsigned int)this_mat->get_color_components().w;
						}
						else{
							image[4 * colorposition] =		(unsigned int)this_mat->get_color_components().x;
							image[4 * colorposition + 1] =  (unsigned int)this_mat->get_color_components().y;
							image[4 * colorposition + 2] =  (unsigned int)this_mat->get_color_components().z;
							image[4 * colorposition + 3] =  (unsigned int)this_mat->get_color_components().w;
						}
						double v_value = double(colorposition)/double(width);
						double v_value_int=0;
						this_mat->texture_u = modf(v_value, &v_value_int) + offset_half_pixel;
						this_mat->texture_v = double(v_value_int)/double(width) + offset_half_pixel;
						colorposition++;
					}
				}
			}
		}
		unsigned error = lodepng::encode(url_final, image, width, width);
		if(error!=0){
			return result::AWD_ERROR;
		}
	}


	BLOCKS::BitmapTexture* newtexture=NULL;
	if(has_texture_atlas){
		while ((width*width)<pixelCount)
			width=width*2;

		if(width>maxTexSize){
			// todo: we cannot create one texture_atlas for all materials. 
			// start with the biggest material (in pixel-size)
			// get the timelines thats using this.
			// collect all materials that are connected (e.g. used by same shape / child timelines)
			// if textureatlas is full, create a new one and continue.
			// take care that each geometry  uses only 1 texture (e.g. als subgeos of 1 geometry need to use same texture)
			return result::AWD_ERROR;
		}
	
		std::string url_final=awd_project->get_settings()->get_texture_directory()+ name + "_textureAtlas.png";
		std::string rel_url="./"+awd_project->get_settings()->get_texture_directory_name()+ name + "_textureAtlas.png";
		newtexture = reinterpret_cast<BLOCKS::BitmapTexture*>(awd_project->get_block_by_name_and_type(url_final, BLOCK::block_type::BITMAP_TEXTURE, true));
		newtexture->set_input_url(url_final);
		newtexture->set_url(rel_url);
		newtexture->add_scene_name("Textureatlas");
		newtexture->set_name(name + "_textureAtlas");
		if(awd_project->get_settings()->get_bool(SETTINGS::bool_settings::EmbbedTextures)){
			newtexture->set_storage_type(BLOCK::storage_type::EMBEDDED);
		}
		else{
			newtexture->set_storage_type(BLOCK::storage_type::EXTERNAL);
		}
		int colorposition = 0;
		std::vector<unsigned char> image;
		image.resize(width * width * 4);
		int radial_cnt = 0;
		int width_radial=256;
		double offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders	
		int gradients_per_row=floor(double(width)/double(256));
		int gradients_per_row_cnt=0;
		int row_cnt=0;
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if((this_mat->get_material_type()==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL)||(this_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL)){
					if(!this_mat->needsAlphaTex){
						this_mat->set_texture(newtexture);
						if(this_mat->gradient_colors.size()<2){
							//this is a error;
							return result::AWD_SUCCESS;
						}
						double v_value = double(colorposition)/double(width);
						double v_value_int=0;
						this_mat->texture_u = modf(v_value, &v_value_int) + offset_half_pixel;
						this_mat->texture_v = double(v_value_int)/double(width) + offset_half_pixel;
						for(int i=0; i<256; i++){
							GEOM::VECTOR4D new_color=this_mat->get_interpolated_color(i);
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))] =  new_color.x;
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 1] = new_color.y;
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 2] = new_color.z;
							image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 3] = new_color.w;
							colorposition++;

						}
						double v_value2 = double(colorposition-1)/double(width);
						double v_value_int2=0;
						this_mat->texture_u_max = modf(v_value2, &v_value_int2) + offset_half_pixel;
						this_mat->texture_v_max = double(v_value_int2)/double(width) + offset_half_pixel;
					
						gradients_per_row_cnt++;
						if(gradients_per_row_cnt>=gradients_per_row){
							row_cnt++;
							gradients_per_row_cnt=0;
						}
					}
				}
			}
		}
		offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if(this_mat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL){					
					if(!this_mat->needsAlphaTex){
						this_mat->set_texture(newtexture);
						if(this_mat->get_color()==0xff000000){
							image[4 * colorposition] =		0;//(unsigned int)this_mat->get_color_components().x;
							image[4 * colorposition + 1] =  0;//(unsigned int)this_mat->get_color_components().y;
							image[4 * colorposition + 2] =  0;//(unsigned int)this_mat->get_color_components().z;
							image[4 * colorposition + 3] =  255;//(unsigned int)this_mat->get_color_components().w;
						}
						else{
							image[4 * colorposition] =		(unsigned int)this_mat->get_color_components().x;
							image[4 * colorposition + 1] =  (unsigned int)this_mat->get_color_components().y;
							image[4 * colorposition + 2] =  (unsigned int)this_mat->get_color_components().z;
							image[4 * colorposition + 3] =  (unsigned int)this_mat->get_color_components().w;
						}
						double v_value = double(colorposition)/double(width);
						double v_value_int=0;
						this_mat->texture_u = modf(v_value, &v_value_int) + offset_half_pixel;
						this_mat->texture_v = double(v_value_int)/double(width) + offset_half_pixel;
						colorposition++;
					}
				}
			}
		}
		unsigned error = lodepng::encode(url_final, image, width, width);
		if(error!=0){
			return result::AWD_ERROR;
		}
	}
	
	BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("TextureAtlas_colors_mat",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	new_fill_material->add_scene_name("Textureatlas");
	if(newtexture!=NULL)
		new_fill_material->set_texture(newtexture);
	else
		new_fill_material->set_state(TYPES::state::INVALID);
	new_fill_material->set_material_type(MATERIAL::type::SOLID_COLOR_MATERIAL);

	BLOCKS::Material* new_fill_material_linear=new_fill_material; //reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("TextureAtlas_linear_gradients_mat",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	
	BLOCKS::Material* new_fill_material_alpha=reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("TextureAtlas_colors_mat_alpha",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	new_fill_material_alpha->add_scene_name("TextureatlasAlpha");
	new_fill_material_alpha->needsAlphaTex=true;
	if(newtextureAlpha!=NULL)
		new_fill_material_alpha->set_texture(newtextureAlpha);
	else
		new_fill_material_alpha->set_state(TYPES::state::INVALID);
	new_fill_material_alpha->set_material_type(MATERIAL::type::SOLID_COLOR_MATERIAL);

	BLOCKS::Material* new_fill_material_radial=reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("TextureAtlas_radial_gradients_mat",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	new_fill_material_radial->add_scene_name("Textureatlas");
	if(newtexture!=NULL)
		new_fill_material_radial->set_texture(newtexture);
	else
		new_fill_material_radial->set_state(TYPES::state::INVALID);
	if(!has_radial_gradients){
		new_fill_material_radial->set_state(TYPES::state::INVALID);
	}
	new_fill_material_radial->set_material_type(MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL);
	
	BLOCKS::Material* new_fill_material_radial_alpha=reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("TextureAtlas_radial_gradients_mat_alpha",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	new_fill_material_radial_alpha->add_scene_name("TextureatlasAlpha");
	new_fill_material_radial_alpha->needsAlphaTex=true;
	if(newtextureAlpha!=NULL)
		new_fill_material_radial_alpha->set_texture(newtextureAlpha);
	else
		new_fill_material_radial_alpha->set_state(TYPES::state::INVALID);
	if(!has_radial_gradients){
		new_fill_material_radial_alpha->set_state(TYPES::state::INVALID);
	}
	new_fill_material_radial_alpha->set_material_type(MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL);

	std::vector<AWDBlock*> all_geometries;
	awd_project->get_blocks_by_type(all_geometries, BLOCK::block_type::TRI_GEOM);

	for(AWDBlock* one_block:all_geometries){
		BLOCKS::Geometry* this_geom = reinterpret_cast<BLOCKS::Geometry*>(one_block);
		if(this_geom!=NULL){
			this_geom->merge_for_textureatlas(new_fill_material, new_fill_material_alpha, new_fill_material_radial, new_fill_material_radial_alpha);
		}
	}
	
	BLOCKS::Material* new_fill_material_fonts=reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("FontMaterial",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	new_fill_material_fonts->add_scene_name("Textureatlas");
	if(newtextureAlpha!=NULL)
		new_fill_material_fonts->set_texture(newtextureAlpha);
	else
		new_fill_material_fonts->set_state(TYPES::state::INVALID);
	new_fill_material_fonts->set_material_type(MATERIAL::type::SOLID_COLOR_MATERIAL);

	if(awd_project->get_settings()->get_bool(AWD::SETTINGS::bool_settings::TesselateGlyphs)==awd_project->get_settings()->get_bool(AWD::SETTINGS::bool_settings::TesselateGraphics)){
		new_fill_material_fonts->set_state(TYPES::state::INVALID);		
		new_fill_material_fonts=new_fill_material_alpha;
	}

	std::vector<AWDBlock*> all_text_formats;
	awd_project->get_blocks_by_type(all_text_formats, BLOCK::block_type::TEXT_FORMAT);
	for(AWDBlock* one_block:all_text_formats){
		BLOCKS::TextFormat* this_format = reinterpret_cast<BLOCKS::TextFormat*>(one_block);
		if(this_format!=NULL){
			this_format->merge_for_textureatlas(new_fill_material_fonts);
		}
	}
	return result::AWD_SUCCESS;
}
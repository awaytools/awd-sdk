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
	int maxTexSize=8192;
	int minSize=256;
	
	std::vector<AWDBlock*> all_materials;
	awd_project->get_blocks_by_type(all_materials, BLOCK::block_type::SIMPLE_MATERIAL);

	int pixelCount = 0;
	int biggestMaterial = 0;
	int width=minSize;
	bool has_gradients=false;
	for(AWDBlock* one_block:all_materials){
		BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
		if(this_mat!=NULL){
			if(this_mat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL)
				pixelCount++;
			else if(this_mat->get_material_type()==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL){
				if(width<256)
					width=256;
				pixelCount+=256;
				has_gradients=true;
			}
			else if(this_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
				has_gradients=true;
			}
			else if(this_mat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
				//if(width<256)
				//	width=256;
				//pixelCount+=256;
			}
		}
	}
	
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
	

	std::string url_final=awd_project->get_settings()->get_texture_directory()+ name + ".png";
	std::string rel_url="./"+awd_project->get_settings()->get_texture_directory_name()+ name + ".png";
	BLOCKS::BitmapTexture* newtexture = reinterpret_cast<BLOCKS::BitmapTexture*>(awd_project->get_block_by_name_and_type(url_final, BLOCK::block_type::BITMAP_TEXTURE, true));
	newtexture->set_input_url(url_final);
	newtexture->set_url(rel_url);
	newtexture->set_storage_type(BLOCK::storage_type::EXTERNAL);
	
	int colorposition = 0;
	std::vector<unsigned char> image;
	image.resize(width * width * 4);
	double v_value;
	int radial_cnt = 0;
	int width_radial=256;
	
	double offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders
	if(has_gradients){
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if(this_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
					radial_cnt++;
					std::string url_final_radial=awd_project->get_settings()->get_texture_directory()+ "radial_gradient_"+std::to_string(radial_cnt) + ".png";
					std::string rel_url_radial="./"+awd_project->get_settings()->get_texture_directory_name()+ "radial_gradient_"+std::to_string(radial_cnt) + ".png";
					BLOCKS::BitmapTexture* newtexture_radial = reinterpret_cast<BLOCKS::BitmapTexture*>(awd_project->get_block_by_name_and_type(url_final_radial, BLOCK::block_type::BITMAP_TEXTURE, true));
					newtexture_radial->set_input_url(url_final_radial);
					newtexture_radial->set_url(rel_url_radial);
					newtexture_radial->set_storage_type(BLOCK::storage_type::EXTERNAL);
					this_mat->set_texture(newtexture_radial);
					int colorposition_radial = 0;
					double offset_half_pixel = double(0.5)/double(width_radial); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders
					std::vector<unsigned char> image_radial;
					image_radial.resize(width_radial * width_radial * 4);
					double v_value;
					int radial_cnt = 0;

					std::vector<GEOM::VECTOR4D> colors(128);

					int startpixel=this_mat->gradient_positions[0]/2;
					int maxpixel_delta=(this_mat->gradient_positions[1]/2)-startpixel;	

					TYPES::COLOR start_color=this_mat->gradient_colors[0];
					TYPES::COLOR end_color=this_mat->gradient_colors[1];

					unsigned char red_start=(start_color >> 16);
					unsigned char green_start=(start_color >> 8);
					unsigned char blue_start=(start_color >> 0);
					unsigned char alpha_start=(start_color >> 24);

					unsigned char red_end=(end_color >> 16);
					unsigned char green_end=(end_color >> 8);
					unsigned char blue_end=(end_color >> 0);
					unsigned char alpha_end=(end_color >> 24);
					
					float red_start2 = float(red_start)/float(255);
					float green_start2 = float(green_start)/float(255);
					float blue_start2 = float(blue_start)/float(255);
					float alpha_start2 = float(alpha_start)/float(255);
					float red_end2 = float(red_end)/float(255);
					float green_end2 =float(green_end)/float(255);
					float blue_end2 = float(blue_end)/float(255);
					float alpha_end2 = float(alpha_end)/float(255);
					float h=0;
					float s=0;
					float v=0;
					float r1=0;
					float g1=0;
					float b1=0;
					float h2=0;
					float s2=0;
					float v2=0;
					float h3=0;
					float s3=0;
					float v3=0;
					RGBtoHSV(red_start2, green_start2, blue_start2, &h, &s, &v);
					RGBtoHSV(red_end2, green_end2, blue_end2, &h2, &s2, &v2);

					double alpha_difference=alpha_end2-alpha_start2;

					double current_percent=0;
					double gradient_point_cnt=1;
					
					v_value = double(colorposition_radial)/double(width_radial);
					double v_value_int=0;
					this_mat->texture_u = 0;
					this_mat->texture_v = 0;
					this_mat->texture_u_max = 1;
					this_mat->texture_v_max = 1;
					for(int i=0; i<128; i++){

						current_percent=double(i-startpixel)/double(maxpixel_delta);
						if(current_percent>=1){
							current_percent=1;
							if(gradient_point_cnt<this_mat->gradient_positions.size()-1){
								startpixel=this_mat->gradient_positions[gradient_point_cnt]/2;
								maxpixel_delta=(this_mat->gradient_positions[gradient_point_cnt+1]/2)-startpixel;	
								start_color=this_mat->gradient_colors[gradient_point_cnt];
								end_color=this_mat->gradient_colors[gradient_point_cnt+1];	
								red_start=(start_color >> 16);
								green_start=(start_color >> 8);
								blue_start=(start_color >> 0);
								alpha_start=(start_color >> 24);

								red_end=(end_color >> 16);
								green_end=(end_color >> 8);
								blue_end=(end_color >> 0);
								alpha_end=(end_color >> 24);
					
								red_start2 = float(red_start)/float(255);
								green_start2 = float(green_start)/float(255);
								blue_start2 = float(blue_start)/float(255);
								alpha_start2 = float(alpha_start)/float(255);
								red_end2 = float(red_end)/float(255);
								green_end2 =float(green_end)/float(255);
								blue_end2 = float(blue_end)/float(255);
								alpha_end2 = float(alpha_end)/float(255);
								//RGBtoHSV(red_start2, green_start2, blue_start2, &h, &s, &v);
								//RGBtoHSV(red_end2, green_end2, blue_end2, &h2, &s2, &v2);	
								current_percent=0;
							}	
						}
						colors[colorposition_radial].x = ((red_start2*(1-current_percent)) + (red_end2*current_percent))*255;
						colors[colorposition_radial].y  = ((green_start2*(1-current_percent)) + (green_end2*current_percent))*255;
						colors[colorposition_radial].z  = ((blue_start2*(1-current_percent)) + (blue_end2*current_percent))*255;
						colors[colorposition_radial].w  = 255;//((alpha_start*(1-current_percent)) + (alpha_end*current_percent))*255;
						colorposition_radial++;
					}
					
					for(int i=0; i<256; i++){
						for(int k=0; k<256; k++){
							TYPES::UINT32 distance_to_middle = floor(sqrt(((128-i)*(128-i))+((128-k)*(128-k))));
							if(distance_to_middle>=128){
								distance_to_middle=127;
							}
							image_radial[4 * ((i*256)+k)] = colors[distance_to_middle].x;
							image_radial[4 * ((i*256)+k)+ 1] = colors[distance_to_middle].y;
							image_radial[4 * ((i*256)+k)+ 2] = colors[distance_to_middle].z;
							image_radial[4 * ((i*256)+k)+ 3] = colors[distance_to_middle].w;
						}
					}
					unsigned error = lodepng::encode(url_final_radial, image_radial, width_radial, width_radial);
				}
			}
		}
			
		double offset_half_pixel = double(0.5)/double(width); // used to move uv half a pixel to the right. otherwise we have end up on pixel-borders	
		int gradients_per_row=floor(double(width)/double(256));
		int gradients_per_row_cnt=0;
		int row_cnt=0;
		for(AWDBlock* one_block:all_materials){
			BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(one_block);
			if(this_mat!=NULL){
				if(this_mat->get_material_type()==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL){
					this_mat->set_texture(newtexture);
					if(this_mat->gradient_colors.size()<2){
						//this is a error;
						return result::AWD_SUCCESS;
					}
					int startpixel=this_mat->gradient_positions[0];
					int maxpixel_delta=this_mat->gradient_positions[1]-startpixel;					
					TYPES::COLOR start_color=this_mat->gradient_colors[0];
					TYPES::COLOR end_color=this_mat->gradient_colors[1];
					unsigned char red_start=(start_color >> 16);
					unsigned char green_start=(start_color >> 8);
					unsigned char blue_start=(start_color >> 0);
					unsigned char alpha_start=(start_color >> 24);

					unsigned char red_end=(end_color >> 16);
					unsigned char green_end=(end_color >> 8);
					unsigned char blue_end=(end_color >> 0);
					unsigned char alpha_end=(end_color >> 24);
					
					float red_start2 = float(red_start)/float(255);
					float green_start2 = float(green_start)/float(255);
					float blue_start2 = float(blue_start)/float(255);
					float alpha_start2 = float(alpha_start)/float(255);
					float red_end2 = float(red_end)/float(255);
					float green_end2 =float(green_end)/float(255);
					float blue_end2 = float(blue_end)/float(255);
					float alpha_end2 = float(alpha_end)/float(255);
					float h=0;
					float s=0;
					float v=0;
					float r1=0;
					float g1=0;
					float b1=0;
					float h2=0;
					float s2=0;
					float v2=0;
					float h3=0;
					float s3=0;
					float v3=0;
					RGBtoHSV(red_start2, green_start2, blue_start2, &h, &s, &v);
					RGBtoHSV(red_end2, green_end2, blue_end2, &h2, &s2, &v2);

					double alpha_difference=alpha_end2-alpha_start2;

					double current_percent=0;
					double gradient_point_cnt=1;
					
					double v_value = double(colorposition)/double(width);
					double v_value_int=0;
					this_mat->texture_u = modf(v_value, &v_value_int) + offset_half_pixel;
					this_mat->texture_v = double(v_value_int)/double(width) + offset_half_pixel;
					for(int i=0; i<256; i++){

						current_percent=double(i-startpixel)/double(maxpixel_delta);
						if(current_percent>=1){
							current_percent=1;
							if(gradient_point_cnt<this_mat->gradient_positions.size()-1){
								startpixel=this_mat->gradient_positions[gradient_point_cnt];
								maxpixel_delta=this_mat->gradient_positions[gradient_point_cnt+1]-startpixel;	
								start_color=this_mat->gradient_colors[gradient_point_cnt];
								end_color=this_mat->gradient_colors[gradient_point_cnt+1];	
								red_start=(start_color >> 16);
								green_start=(start_color >> 8);
								blue_start=(start_color >> 0);
								alpha_start=(start_color >> 24);

								red_end=(end_color >> 16);
								green_end=(end_color >> 8);
								blue_end=(end_color >> 0);
								alpha_end=(end_color >> 24);
					
								red_start2 = float(red_start)/float(255);
								green_start2 = float(green_start)/float(255);
								blue_start2 = float(blue_start)/float(255);
								alpha_start2 = float(alpha_start)/float(255);
								red_end2 = float(red_end)/float(255);
								green_end2 =float(green_end)/float(255);
								blue_end2 = float(blue_end)/float(255);
								alpha_end2 = float(alpha_end)/float(255);
								//RGBtoHSV(red_start2, green_start2, blue_start2, &h, &s, &v);
								//RGBtoHSV(red_end2, green_end2, blue_end2, &h2, &s2, &v2);	
								current_percent=0;
							}	
							gradient_point_cnt++;
						}
						colorposition++;
						
						/*h3 = (h*(1-current_percent)) + (h2*current_percent);
						s3 = (s*(1-current_percent)) + (s2*current_percent);
						v3 = (v*(1-current_percent)) + (v2*current_percent);
						HSVtoRGB(&r1, &g1, &b1, h3, s3, v3);*/
						
						r1 = (red_start2*(1-current_percent)) + (red_end2*current_percent);
						g1 = (green_start2*(1-current_percent)) + (green_end2*current_percent);
						b1 = (blue_start2*(1-current_percent)) + (blue_end2*current_percent);

						image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))] =  r1*255;
						image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 1] = g1*255;
						image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 2] = b1*255;
						image[4 * (i+(row_cnt*width)+(gradients_per_row_cnt*256))+ 3] = 255;//(alpha_start*(1-current_percent)) + (alpha_end*current_percent);

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
				this_mat->set_texture(newtexture);
				
				if(this_mat->get_color()==0xff000000){
					image[4 * colorposition] =		unsigned int (this_mat->get_color_components().x)+1;
					image[4 * colorposition + 1] =  unsigned int (this_mat->get_color_components().y)+1;
					image[4 * colorposition + 2] =  unsigned int (this_mat->get_color_components().z)+1;
					image[4 * colorposition + 3] =  unsigned int (this_mat->get_color_components().w);
				}
				else{
					image[4 * colorposition] =		unsigned int (this_mat->get_color_components().x);
					image[4 * colorposition + 1] =  unsigned int (this_mat->get_color_components().y);
					image[4 * colorposition + 2] =  unsigned int (this_mat->get_color_components().z);
					image[4 * colorposition + 3] =  unsigned int (this_mat->get_color_components().w);
				}


				double v_value = double(colorposition)/double(width);
				double v_value_int=0;
				this_mat->texture_u = modf(v_value, &v_value_int) + offset_half_pixel;
				this_mat->texture_v = double(v_value_int)/double(width) + offset_half_pixel;					
				colorposition++;		
			}
		}
	}	
	unsigned error = lodepng::encode(url_final, image, width, width);
	if(error!=0){
		return result::AWD_ERROR;
	}
	
	BLOCKS::Material* new_fill_material=reinterpret_cast<BLOCKS::Material*>(awd_project->get_block_by_name_and_type("TextureAtlas_colors",  BLOCK::block_type::SIMPLE_MATERIAL, true));
	new_fill_material->set_texture(newtexture);
	new_fill_material->set_material_type(MATERIAL::type::SOLID_COLOR_MATERIAL);
	std::vector<AWDBlock*> all_geometries;
	awd_project->get_blocks_by_type(all_geometries, BLOCK::block_type::TRI_GEOM);

	for(AWDBlock* one_block:all_geometries){
		BLOCKS::Geometry* this_geom = reinterpret_cast<BLOCKS::Geometry*>(one_block);
		if(this_geom!=NULL){
			this_geom->merge_for_textureatlas(new_fill_material);
		}
	}
	return result::AWD_SUCCESS;
}
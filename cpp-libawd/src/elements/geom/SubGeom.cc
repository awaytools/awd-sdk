#include "blocks/geometry.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "utils/util.h"
#include "utils/awd_globals.h"

#include <math.h>       
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
 


SubGeom::SubGeom(SETTINGS::BlockSettings* subGeomSettings):
	BASE::AttrElementBase(),
	BASE::StateElementBase()
{
	this->target_subgeom=NULL;
	this->uv_transform=new MATRIX2x3();
	this->tri_cnt=0;
	this->material_block=NULL;
	this->allowed_tris=0;
	this->isMerged=false;
	this->isScaled9=false;
	this->isMerged_refactor=false;
	this->merged_address=0;
	this->settings = subGeomSettings->clone_block_settings();
	this->max_x=std::numeric_limits<double>::max()*-1;
	this->max_y=std::numeric_limits<double>::max()*-1;
	this->min_x=std::numeric_limits<double>::max();
	this->min_y=std::numeric_limits<double>::max();
	this->uv_br=NULL;
	this->uv_tl=NULL;
	this->mat_type = MATERIAL::type::UNDEFINED;
}

SubGeom::~SubGeom()
{
	delete this->uv_transform;
	for(SubGeomInternal* subGeo : this->sub_geoms)
		delete subGeo;
	delete this->settings;
}

std::string&
SubGeom::get_name()
{
	return this->name;
}
void
SubGeom::set_name(const std::string& name)
{
	this->name = name;
}

result
SubGeom::modify_font_char(double size)
{
	for(SubGeomInternal* subGeo:this->sub_geoms)
		subGeo->modify_font_char(size);
	return result::AWD_SUCCESS;
}
TYPES::state 
SubGeom::validate_state()
{
	return this->state;
}
void
SubGeom::get_internal_id(std::string& output_str)
{
	// we have a number of triangles that are allowed to get added to the current SubGeominternal.
	// if we added this amount of triangles, we check again, if there is any space left in the current SubGeominternal.
	// if there is any space left, we correct the number for allowe dtriangles.
	// if no space is left, we create a new SubGeominternal and reset the number.
	

	if(this->allowed_tris>0){
		output_str=FILES::int_to_string(this->sub_geoms.size()-1);
		return;
	}

	// calculate the max numbers of triangles allowed to export into the current SubGeominternal.

	// get the max number of bytes for each stream_target-type.
	// the result for vertex is multiplied by 3, because we plan to add max 1 triangle and 3 vertex
	TYPES::UINT32 max_tri_attr = get_max_stream_attributes_length(stream_target::TRIANGLE_STREAM);
	TYPES::UINT32 max_vert_attr = 3 * get_max_stream_attributes_length(stream_target::VERTEX_STREAM);

	TYPES::UINT32 max_attr_length = max_vert_attr;
	if(max_tri_attr > max_vert_attr)
		max_attr_length = max_tri_attr;
	this->allowed_tris = (RESSOURCE_LIMIT / max_attr_length)*9999999;
	
	SubGeomInternal* this_subGeo = new SubGeomInternal();
	this_subGeo->isMerged=this->isMerged;
	this->sub_geoms.push_back(this_subGeo);

	output_str=FILES::int_to_string(this->sub_geoms.size()-1);
	return;
}

void 
SubGeom::set_material(BASE::AWDBlock* material_block)
{
	this->material_block=material_block;
}
void 
SubGeom::get_material_blocks(std::vector<BASE::AWDBlock*>& material_blocks)
{
	for(SubGeomInternal* subGeo:this->sub_geoms)
		material_blocks.push_back(this->material_block);
}
TYPES::UINT32
SubGeom::get_tri_cnt_for_type(GEOM::edge_type edge_type) 
{
	TYPES::UINT32 tricnt=0;
	for(SubGeomInternal* subGeo:this->sub_geoms)
		tricnt+=subGeo->get_tri_cnt_for_type(edge_type);
	return tricnt;
}
TYPES::UINT32
SubGeom::get_tri_cnt()
{
	TYPES::UINT32 tricnt=0;
	for(SubGeomInternal* subGeo:this->sub_geoms)
		tricnt+=subGeo->get_tri_cnt();
	return tricnt;
}

result 
SubGeom::merge_subgeo(SubGeom* subgeom_to_merge)
{
	//if(this->isMerged){
	//	return result::AWD_ERROR;
	//}
	if(subgeom_to_merge->get_sub_geoms().size()==0)
		return result::AWD_SUCCESS;

	// this merges all the subgeoms if possible.
	// important is that all subgeom for a fontchar get merges, as there will
	
	if(this->material_block!=subgeom_to_merge->material_block)
		return result::AWD_ERROR;
	
	if((this->uv_br!=NULL)&&(subgeom_to_merge->uv_br!=NULL)){
		if(this->uv_br->x != subgeom_to_merge->uv_br->x)
			return result::AWD_ERROR;
		if(this->uv_br->y != subgeom_to_merge->uv_br->y)
			return result::AWD_ERROR;
	}
	if((this->uv_tl!=NULL)&&(subgeom_to_merge->uv_tl!=NULL)){
		if(this->uv_tl->x != subgeom_to_merge->uv_tl->x)
			return result::AWD_ERROR;
		if(this->uv_tl->y != subgeom_to_merge->uv_tl->y)
			return result::AWD_ERROR;
	}
	if((this->uv_transform!=NULL)&&(subgeom_to_merge->uv_transform!=NULL)){
		if(!this->uv_transform->compare(subgeom_to_merge->uv_transform))
			return result::AWD_ERROR;
	}
	

	SubGeomInternal* last_subgeo=this->sub_geoms.back();
	this->sub_geoms.pop_back();
	
	for(SubGeomInternal* subGeom : subgeom_to_merge->get_sub_geoms()){
		if(subGeom!=subgeom_to_merge->get_sub_geoms().back()){
			this->sub_geoms.push_back(subGeom);
		}
	}
    this->sub_geoms.push_back(last_subgeo);
	std::vector<GEOM::GeomStreamElementBase*> verts = subgeom_to_merge->get_sub_geoms().back()->get_vertices();
	if((subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::COMBINED_POSITION_2D)!=NULL)||
		(subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::POSITIONS_2D)!=NULL)||
		(subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D__5F)!=NULL)||
		(subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D__2F3B)!=NULL)||
		(subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::COMBINED_POSITION_2D_INT16)!=NULL)||
		(subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D_2xINT16_3xINT8)!=NULL)){
		for(GEOM::Triangle* tri: subgeom_to_merge->get_sub_geoms().back()->get_triangles()){
			std::string id;
			this->get_internal_id(id);
			TYPES::UINT32 idx1 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), false);
			TYPES::UINT32 idx2 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), false);
			TYPES::UINT32 idx3 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), false);
			this->add_triangle(new Triangle( idx1, idx2, idx3));
		}
	}
	else{
		for(GEOM::Triangle* tri: subgeom_to_merge->get_sub_geoms().back()->concave_triangles){
			std::string id;
			this->get_internal_id(id);
			TYPES::UINT32 idx1 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), true);
			TYPES::UINT32 idx2 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), true);
			TYPES::UINT32 idx3 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), true);
			this->sub_geoms.back()->add_triangle_by_type(new Triangle( idx1,  idx2, idx3), GEOM::edge_type::CONCAVE_EDGE);
		}
		for(GEOM::Triangle* tri: subgeom_to_merge->get_sub_geoms().back()->convex_triangles){
			std::string id;
			this->get_internal_id(id);
			TYPES::UINT32 idx1 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), true);
			TYPES::UINT32 idx2 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), true);
			TYPES::UINT32 idx3 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), true);
			this->sub_geoms.back()->add_triangle_by_type(new Triangle( idx1,  idx2, idx3), GEOM::edge_type::CONVEX_EDGE);
		}
		for(GEOM::Triangle* tri: subgeom_to_merge->get_sub_geoms().back()->exterior_triangles){
			std::string id;
			this->get_internal_id(id);
			TYPES::UINT32 idx1 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), true);
			TYPES::UINT32 idx2 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), true);
			TYPES::UINT32 idx3 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), true);
			this->sub_geoms.back()->add_triangle_by_type(new Triangle( idx1,  idx2, idx3), GEOM::edge_type::OUTTER_EDGE);
		}
		for(GEOM::Triangle* tri: subgeom_to_merge->get_sub_geoms().back()->interior_triangles){
			std::string id;
			this->get_internal_id(id);
			TYPES::UINT32 idx1 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), true);
			TYPES::UINT32 idx2 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), true);
			TYPES::UINT32 idx3 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), true);
			this->sub_geoms.back()->add_triangle_by_type(new Triangle( idx1,  idx2, idx3), GEOM::edge_type::INNER_EDGE);
		}
	}
	return result::AWD_SUCCESS;

}


result 
SubGeom::calculate_bounds(){
	this->max_x=std::numeric_limits<double>::max()*-1;
	this->max_y=std::numeric_limits<double>::max()*-1;
	this->min_x=std::numeric_limits<double>::max();
	this->min_y=std::numeric_limits<double>::max();
	for(SubGeomInternal* subGeom : this->sub_geoms){
		std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
		for(GeomStreamElementBase* vert_base: verts){
			GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
			GEOM::VECTOR2D position=vert->get_position();
			if(position.x<this->min_x){
				this->min_x=position.x;
			}
			if(position.y<this->min_y){
				this->min_y=position.y;
			}
			if(position.x>this->max_x){
				this->max_x=position.x;
			}
			if(position.y>this->max_y){
				this->max_y=position.y;
			}
		}
	}
	return result::AWD_SUCCESS;
}
result 
SubGeom::set_uint16_positions(double tx, double ty, double scaleX, double scaleY, double width, double height){
	
	GEOM::VECTOR2D_UINT16 position_uint16=GEOM::VECTOR2D_UINT16(0,0);
	double tmpX = 0;
	double tmpY = 0;

	for(SubGeomInternal* subGeom : this->sub_geoms){
		std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
		for(GeomStreamElementBase* vert_base: verts){
			GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
			GEOM::VECTOR2D position=vert->get_position();
			tmpX=(position.x-tx)*scaleX;
			tmpY=(position.y-ty)*scaleY;
			position_uint16.x=(TYPES::UINT16)tmpX;
			position_uint16.y=(TYPES::UINT16)tmpY;
			vert->set_position_uint16(position_uint16.x, position_uint16.y);
		}
	}

	this->registration_uv_transform=new MATRIX2x3();
	TYPES::F64* uv_transforms_floats=this->registration_uv_transform->get();
	uv_transforms_floats[0]=1/scaleX;
	uv_transforms_floats[1]=0;
	uv_transforms_floats[2]=0;
	uv_transforms_floats[3]=1/scaleY;
	uv_transforms_floats[4]=tx;
	uv_transforms_floats[5]=ty;
	return result::AWD_SUCCESS;
}

void 
SubGeom::split_tris_into_horizontal_groups(std::vector<GEOM::Triangle*>& input, std::vector<GEOM::Triangle*>& tris_left, std::vector<GEOM::Triangle*>& tris_middle, std::vector<GEOM::Triangle*>& tris_right, TYPES::F32 leftx, TYPES::F32 rightx)
{
	GEOM::Vertex2D* vert1;
	GEOM::Vertex2D* vert2;
	GEOM::Vertex2D* vert3;
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
	int tLength=input.size();
	std::vector<GEOM::Triangle*> tmp_list;
	for(int tCnt=0; tCnt<tLength;tCnt++){
		Triangle* tri=input[tCnt];
		vert1=reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]);
		vert2=reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]);
		vert3=reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]);
		// tri is completey on left side
		if((vert1->get_position().x<=leftx)&&(vert2->get_position().x<=leftx)&&(vert3->get_position().x<=leftx)){
			tris_left.push_back(tri);
		}
		// tri is completey on right side
		else if((vert1->get_position().x>=rightx)&&(vert2->get_position().x>=rightx)&&(vert3->get_position().x>=rightx)){
			tris_right.push_back(tri);
		}
		// tri is completey in the middle
		else if(
			(vert1->get_position().x>=leftx)&&(vert1->get_position().x<=rightx)
			&&(vert2->get_position().x>=leftx)&&(vert2->get_position().x<=rightx)
			&&(vert3->get_position().x>=leftx)&&(vert3->get_position().x<=rightx)){
				tris_middle.push_back(tri);
		}
		// tri is in right and middle
		else if((vert1->get_position().x>=leftx)&&(vert2->get_position().x>=leftx)&&(vert3->get_position().x>=leftx)){
			this->split_tri_horizontal(rightx, tri, tris_middle, tris_right);
		}
		// tri is in left and middle
		else if((vert1->get_position().x<=rightx)&&(vert2->get_position().x<=rightx)&&(vert3->get_position().x<=rightx)){
			this->split_tri_horizontal(leftx, tri, tris_left, tris_middle);
		}
		// tri is in left and middle and right
		else {
			tmp_list.clear();
			this->split_tri_horizontal(leftx, tri, tris_left, tmp_list);
			for(Triangle* one_tri:tmp_list){
				this->split_tri_horizontal(rightx, one_tri, tris_middle, tris_right);
			}
		}
	}
}
void  
SubGeom::split_tris_into_vertical_groups(std::vector<GEOM::Triangle*>& input, std::vector<GEOM::Triangle*>& tris_top, std::vector<GEOM::Triangle*>& tris_middle, std::vector<GEOM::Triangle*>& tris_bottom, TYPES::F32 topy, TYPES::F32 bottomy)
{
	GEOM::Vertex2D* vert1;
	GEOM::Vertex2D* vert2;
	GEOM::Vertex2D* vert3;
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
	int tLength=input.size();
	std::vector<GEOM::Triangle*> tmp_list;
	for(int tCnt=0; tCnt<tLength;tCnt++){
		Triangle* tri=input[tCnt];
		vert1=reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]);
		vert2=reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]);
		vert3=reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]);
		// tri complety in top 
		if((vert1->get_position().y<=topy)&&(vert2->get_position().y<=topy)&&(vert3->get_position().y<=topy)){
			tris_top.push_back(tri);
		}
		// tri complety in bottom
		else if((vert1->get_position().y>=bottomy)&&(vert2->get_position().y>=bottomy)&&(vert3->get_position().y>=bottomy)){
			tris_bottom.push_back(tri);
		}
		// tri complety in middle
		else if(
			(vert1->get_position().y>=topy)&&(vert1->get_position().y<=bottomy)
			&&(vert2->get_position().y>=topy)&&(vert2->get_position().y<=bottomy)
			&&(vert3->get_position().y>=topy)&&(vert3->get_position().y<=bottomy)){
				tris_middle.push_back(tri);
		}
		// tri in middle and bottom
		else if((vert1->get_position().y>=topy)&&(vert2->get_position().y>=topy)&&(vert3->get_position().y>=topy)){
			this->split_tri_vertical(bottomy, tri, tris_middle, tris_bottom);
		}
		// tri in middle and top
		else if((vert1->get_position().y<=bottomy)&&(vert2->get_position().y<=bottomy)&&(vert3->get_position().y<=bottomy)){
			this->split_tri_vertical(topy, tri, tris_top, tris_middle);
		}
		// tri in middle and top and bottom
		else {
			tmp_list.clear();
			this->split_tri_vertical(topy, tri, tris_top, tmp_list);
			for(Triangle* one_tri:tmp_list){
				this->split_tri_vertical(bottomy, one_tri, tris_middle, tris_bottom);
			}
		}
	}
}

void 
SubGeom::createSlice9Scale(TYPES::F32 left, TYPES::F32 top, TYPES::F32 right, TYPES::F32 bottom, TYPES::F32 g_left, TYPES::F32 g_top, TYPES::F32 g_right, TYPES::F32 g_bottom){
	if(this->sub_geoms.size()!=1){
		//error
		return;
	}
	if(this->isScaled9){
		return;
	}
	this->isScaled9=true;
	this->isMerged=false;
	std::vector<std::vector<GEOM::Triangle*>> all_tris_grouped;

	std::vector<GEOM::Triangle*> tris_left;
	std::vector<GEOM::Triangle*> tris_left_top;
	std::vector<GEOM::Triangle*> tris_left_middle;
	std::vector<GEOM::Triangle*> tris_left_bottom;
	std::vector<GEOM::Triangle*> tris_middle;
	std::vector<GEOM::Triangle*> tris_middle_top;
	std::vector<GEOM::Triangle*> tris_middle_middle;
	std::vector<GEOM::Triangle*> tris_middle_bottom;
	std::vector<GEOM::Triangle*> tris_right;
	std::vector<GEOM::Triangle*> tris_right_top;
	std::vector<GEOM::Triangle*> tris_right_middle;
	std::vector<GEOM::Triangle*> tris_right_bottom;


	std::vector<GEOM::Triangle*> original_tris = this->sub_geoms.back()->get_triangles();

	this->split_tris_into_horizontal_groups(original_tris, tris_left, tris_middle, tris_right, left, right);	
	
	this->split_tris_into_vertical_groups(tris_left, tris_left_top, tris_left_middle, tris_left_bottom, top, bottom);
	this->split_tris_into_vertical_groups(tris_middle, tris_middle_top, tris_middle_middle, tris_middle_bottom, top, bottom);
	this->split_tris_into_vertical_groups(tris_right, tris_right_top, tris_right_middle, tris_right_bottom, top, bottom);
	
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
		
	all_tris_grouped.push_back(tris_left_top);
	all_tris_grouped.push_back(tris_left_middle);
	all_tris_grouped.push_back(tris_left_bottom);
	
	all_tris_grouped.push_back(tris_middle_top);
	all_tris_grouped.push_back(tris_middle_middle);
	all_tris_grouped.push_back(tris_middle_bottom);

	all_tris_grouped.push_back(tris_right_top);
	all_tris_grouped.push_back(tris_right_middle);
	all_tris_grouped.push_back(tris_right_bottom);
		
	this->slice9ScaleStream.clear();
	this->slice9ScalePositionsStream.clear();

	// the top / bottom / left / right values of the slice 9 - same for all shapes in a graphics

	this->slice9ScaleStream.push_back((int)((left-g_left)*20));
	this->slice9ScaleStream.push_back((int)((top-g_top)*20));
	this->slice9ScaleStream.push_back((int)((g_right-right)*20));
	this->slice9ScaleStream.push_back((int)((g_bottom-bottom)*20));

	// the original size rect of the complete graphics
	this->slice9ScaleStream.push_back((int)((g_left)*20));
	this->slice9ScaleStream.push_back((int)((g_top)*20));
	this->slice9ScaleStream.push_back((int)((g_right-g_left)*20));
	this->slice9ScaleStream.push_back((int)((g_bottom-g_top)*20));
	
	std::vector<double> slice9Offsets_x;
	slice9Offsets_x.push_back(g_left);
	slice9Offsets_x.push_back(left);
	slice9Offsets_x.push_back(right);
	
	std::vector<double> slice9Offsets_y;
	slice9Offsets_y.push_back(g_top);
	slice9Offsets_y.push_back(top);
	slice9Offsets_y.push_back(bottom);

	int cnt_trigrp=0;
	int row_cnt=-1;
	int col_cnt=0;
	int tri_cnt=0;
	double scalex=0;
	double scaley=0;
	double offsetx=0;
	double offsety=0;
	double innerWidth=right-left;
	double innerHeight=bottom-top;
	
	double ax=0;
	double ay=0;
	double bx=0;
	double by=0;
	double cx=0;
	double cy=0;
	for(std::vector<GEOM::Triangle*> onetrilist : all_tris_grouped){
		
			if(row_cnt==2){
				col_cnt++;
				row_cnt=-1;
			}
			row_cnt++;
			
			// only need to x-scale if this is the middle column
			scalex=1;
			if(col_cnt==1){
				scalex=innerWidth;
			}
			// only need to y-scale if this is the middle row
			scaley=1;
			if(row_cnt==1){
				scaley=innerHeight;
			}

			// offsetx is different for each column
			offsetx=slice9Offsets_x[col_cnt];

			// offsety is different for each row
			offsety=slice9Offsets_y[row_cnt];

			for(GEOM::Triangle* one_tri : onetrilist){
				ax=(reinterpret_cast<GEOM::Vertex2D*>(verts[one_tri->get_output_idx_1()])->get_position().x-offsetx) / scalex;
				ay=(reinterpret_cast<GEOM::Vertex2D*>(verts[one_tri->get_output_idx_1()])->get_position().y-offsety) / scaley;
				bx=(reinterpret_cast<GEOM::Vertex2D*>(verts[one_tri->get_output_idx_2()])->get_position().x-offsetx) / scalex;
				by=(reinterpret_cast<GEOM::Vertex2D*>(verts[one_tri->get_output_idx_2()])->get_position().y-offsety) / scaley;
				cx=(reinterpret_cast<GEOM::Vertex2D*>(verts[one_tri->get_output_idx_3()])->get_position().x-offsetx) / scalex;
				cy=(reinterpret_cast<GEOM::Vertex2D*>(verts[one_tri->get_output_idx_3()])->get_position().y-offsety) / scaley;
				if(!this->isClockWiseXY(ax, ay, bx, by, cx, cy)){
					this->slice9ScalePositionsStream.push_back(ax);
					this->slice9ScalePositionsStream.push_back(ay);
					this->slice9ScalePositionsStream.push_back(cx);
					this->slice9ScalePositionsStream.push_back(cy);
					this->slice9ScalePositionsStream.push_back(bx);
					this->slice9ScalePositionsStream.push_back(by);
				}
				else{
					this->slice9ScalePositionsStream.push_back(ax);
					this->slice9ScalePositionsStream.push_back(ay);
					this->slice9ScalePositionsStream.push_back(bx);
					this->slice9ScalePositionsStream.push_back(by);
					this->slice9ScalePositionsStream.push_back(cx);
					this->slice9ScalePositionsStream.push_back(cy);
				}
			}
		
		cnt_trigrp++;
		tri_cnt+=onetrilist.size()*6;
		this->slice9ScaleStream.push_back(tri_cnt);
	}
	//result.push_back(newSubGeom);

}

TYPES::F32 
SubGeom::get_x_for_y_between_verts(GEOM::Vertex2D* v1, GEOM::Vertex2D* v2, TYPES::F32 y){
	TYPES::F32 line_y=v2->get_position().y-v1->get_position().y;
	if(line_y==0){
		return 0;//should not happen
	}
	TYPES::F32 line_x=v2->get_position().x-v1->get_position().x;
	if(line_x==0){
		return v2->get_position().x;
	}
	TYPES::F32 test_y=y - v1->get_position().y;
	return (v1->get_position().x+ (test_y/((line_y/line_x))));


}
TYPES::F32 
SubGeom::get_y_for_x_between_verts(GEOM::Vertex2D* v1, GEOM::Vertex2D* v2, TYPES::F32 x){
	TYPES::F32 line_x=v2->get_position().x-v1->get_position().x;
	if(line_x==0){
		return 0;//should not happen
	}
	TYPES::F32 line_y=v2->get_position().y-v1->get_position().y;
	if(line_y==0){
		return v2->get_position().y;
	}
	TYPES::F32 test_x=x - v1->get_position().x;
	return (v1->get_position().y+((line_y/line_x)*test_x));

}
bool 
SubGeom::split_tri_horizontal(TYPES::F32 x, GEOM::Triangle* original_tri, std::vector<GEOM::Triangle*>& tris_left, std::vector<GEOM::Triangle*>& tris_right){
	GEOM::Vertex2D* vert1;
	GEOM::Vertex2D* vert2;
	GEOM::Vertex2D* vert3;
	GEOM::Triangle* tri1 = new Triangle();
	GEOM::Triangle* tri2 = new Triangle();
	GEOM::Triangle* tri3 = new Triangle();
	GEOM::Vertex2D* vert_1=new Vertex2D();
	GEOM::Vertex2D* vert_2=new Vertex2D();
	GEOM::Vertex2D* vert_11=new Vertex2D();
	GEOM::Vertex2D* vert_21=new Vertex2D();
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
	vert1=reinterpret_cast<GEOM::Vertex2D*>(verts[original_tri->get_output_idx_1()]);
	vert2=reinterpret_cast<GEOM::Vertex2D*>(verts[original_tri->get_output_idx_2()]);
	vert3=reinterpret_cast<GEOM::Vertex2D*>(verts[original_tri->get_output_idx_3()]);
	int in_idx1=original_tri->get_output_idx_1();
	int in_idx2=original_tri->get_output_idx_2();
	int in_idx3=original_tri->get_output_idx_3();
	int idx1=this->add_vertex2D(vert_1, false);
	int idx2=this->add_vertex2D(vert_2, false);
	int idx11=this->add_vertex2D(vert_11, false);
	int idx21=this->add_vertex2D(vert_21, false);

	// left gets the single tri:
	if((vert1->get_position().x<=x)&&(vert2->get_position().x>=x)&&(vert3->get_position().x>=x)){
		
		vert_1->set_position(x, this->get_y_for_x_between_verts(vert1, vert2, x));
		vert_11->set_position(x, vert_1->get_position().y);
		vert_2->set_position(x, this->get_y_for_x_between_verts(vert1, vert3, x));
		vert_21->set_position(x, vert_2->get_position().y);
		tri1->set_output_idx_1(in_idx1);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx2);
		tri3->set_output_idx_1(in_idx2);
		tri3->set_output_idx_2(in_idx3);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_left.push_back(tri1);
		tris_right.push_back(tri2);
		tris_right.push_back(tri3);
		return true;

	}
	else if((vert2->get_position().x<=x)&&(vert1->get_position().x>=x)&&(vert3->get_position().x>=x)){
		vert_1->set_position(x, this->get_y_for_x_between_verts(vert2, vert1, x));
		vert_11->set_position(x, vert_1->get_position().y);
		vert_2->set_position(x, this->get_y_for_x_between_verts(vert2, vert3, x));
		vert_21->set_position(x, vert_2->get_position().y);
		tri1->set_output_idx_1(in_idx2);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx3);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_left.push_back(tri1);
		tris_right.push_back(tri2);
		tris_right.push_back(tri3);
		return true;
	}
	else if((vert3->get_position().x<=x)&&(vert1->get_position().x>=x)&&(vert2->get_position().x>=x)){
		vert_1->set_position(x, this->get_y_for_x_between_verts(vert3, vert1, x));
		vert_11->set_position(x, vert_1->get_position().y);
		vert_2->set_position(x, this->get_y_for_x_between_verts(vert3, vert2, x));
		vert_21->set_position(x, vert_2->get_position().y);
		tri1->set_output_idx_1(in_idx3);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx2);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_left.push_back(tri1);
		tris_right.push_back(tri2);
		tris_right.push_back(tri3);
		return true;
	}
	// right gets the single tri:
	else if((vert1->get_position().x>=x)&&(vert2->get_position().x<=x)&&(vert3->get_position().x<=x)){
		vert_1->set_position(x, this->get_y_for_x_between_verts(vert1, vert2, x));
		vert_11->set_position(x, vert_1->get_position().y);
		vert_2->set_position(x, this->get_y_for_x_between_verts(vert1, vert3, x));
		vert_21->set_position(x, vert_2->get_position().y);
		tri1->set_output_idx_1(in_idx1);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx2);
		tri3->set_output_idx_1(in_idx3);
		tri3->set_output_idx_2(in_idx2);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_right.push_back(tri1);
		tris_left.push_back(tri2);
		tris_left.push_back(tri3);
		return false;
	}
	else if((vert2->get_position().x>=x)&&(vert1->get_position().x<=x)&&(vert3->get_position().x<=x)){
		vert_1->set_position(x, this->get_y_for_x_between_verts(vert2, vert1, x));
		vert_11->set_position(x, vert_1->get_position().y);
		vert_2->set_position(x, this->get_y_for_x_between_verts(vert2, vert3, x));
		vert_21->set_position(x, vert_2->get_position().y);
		tri1->set_output_idx_1(in_idx2);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx3);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_right.push_back(tri1);
		tris_left.push_back(tri2);
		tris_left.push_back(tri3);
		return false;
	}
	else if((vert3->get_position().x>=x)&&(vert1->get_position().x<=x)&&(vert2->get_position().x<=x)){
		vert_1->set_position(x, this->get_y_for_x_between_verts(vert3, vert1, x));
		vert_11->set_position(x, vert_1->get_position().y);
		vert_2->set_position(x, this->get_y_for_x_between_verts(vert3, vert2, x));
		vert_21->set_position(x, vert_2->get_position().y);
		tri1->set_output_idx_1(in_idx3);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx2);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_right.push_back(tri1);
		tris_left.push_back(tri2);
		tris_left.push_back(tri3);
		return false;
	}
	return false;
}
bool 
SubGeom::split_tri_vertical(TYPES::F32 y, GEOM::Triangle* original_tri, std::vector<GEOM::Triangle*>& tris_top, std::vector<GEOM::Triangle*>& tris_bottom){
	GEOM::Vertex2D* vert1;
	GEOM::Vertex2D* vert2;
	GEOM::Vertex2D* vert3;
	GEOM::Triangle* tri1 = new Triangle();
	GEOM::Triangle* tri2 = new Triangle();
	GEOM::Triangle* tri3 = new Triangle();
	GEOM::Vertex2D* vert_1=new Vertex2D();
	GEOM::Vertex2D* vert_2=new Vertex2D();
	GEOM::Vertex2D* vert_11=new Vertex2D();
	GEOM::Vertex2D* vert_21=new Vertex2D();
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
	vert1=reinterpret_cast<GEOM::Vertex2D*>(verts[original_tri->get_output_idx_1()]);
	vert2=reinterpret_cast<GEOM::Vertex2D*>(verts[original_tri->get_output_idx_2()]);
	vert3=reinterpret_cast<GEOM::Vertex2D*>(verts[original_tri->get_output_idx_3()]);
	int in_idx1=original_tri->get_output_idx_1();
	int in_idx2=original_tri->get_output_idx_2();
	int in_idx3=original_tri->get_output_idx_3();
	int idx1=this->add_vertex2D(vert_1, false);
	int idx2=this->add_vertex2D(vert_2, false);
	int idx11=this->add_vertex2D(vert_11, false);
	int idx21=this->add_vertex2D(vert_21, false);

	// top gets the single tri:

	
	if((vert1->get_position().y<=y)&&(vert2->get_position().y>=y)&&(vert3->get_position().y>=y)){
		
		vert_1->set_position(this->get_x_for_y_between_verts(vert1, vert2, y), y);
		vert_11->set_position(vert_1->get_position().x, y);
		vert_2->set_position(this->get_x_for_y_between_verts(vert1, vert3, y), y);
		vert_21->set_position(vert_2->get_position().x, y);
		tri1->set_output_idx_1(in_idx1);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx2);
		tri3->set_output_idx_1(in_idx2);
		tri3->set_output_idx_2(in_idx3);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_top.push_back(tri1);
		tris_bottom.push_back(tri2);
		tris_bottom.push_back(tri3);
		return true;

	}
	if((vert2->get_position().y<=y)&&(vert1->get_position().y>=y)&&(vert3->get_position().y>=y)){
		vert_1->set_position(this->get_x_for_y_between_verts(vert2, vert1, y), y);
		vert_11->set_position(vert_1->get_position().x, y);
		vert_2->set_position(this->get_x_for_y_between_verts(vert2, vert3, y), y);
		vert_21->set_position(vert_2->get_position().x, y);

		tri1->set_output_idx_1(in_idx2);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx3);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_top.push_back(tri1);
		tris_bottom.push_back(tri2);
		tris_bottom.push_back(tri3);
		return true;
	}
	if((vert3->get_position().y<=y)&&(vert1->get_position().y>=y)&&(vert2->get_position().y>=y)){
		vert_1->set_position(this->get_x_for_y_between_verts(vert3, vert1, y), y);
		vert_11->set_position(vert_1->get_position().x, y);
		vert_2->set_position(this->get_x_for_y_between_verts(vert3, vert2, y), y);
		vert_21->set_position(vert_2->get_position().x, y);
		tri1->set_output_idx_1(in_idx3);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx2);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_top.push_back(tri1);
		tris_bottom.push_back(tri2);
		tris_bottom.push_back(tri3);
		return true;
	}
	// bottom  gets the single tri:
	if((vert1->get_position().y>=y)&&(vert2->get_position().y<=y)&&(vert3->get_position().y<=y)){
		vert_1->set_position(this->get_x_for_y_between_verts(vert1, vert2, y), y);
		vert_11->set_position(vert_1->get_position().x, y);
		vert_2->set_position(this->get_x_for_y_between_verts(vert1, vert3, y), y);
		vert_21->set_position(vert_2->get_position().x, y);
		tri1->set_output_idx_1(in_idx1);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx2);
		tri3->set_output_idx_1(in_idx3);
		tri3->set_output_idx_2(in_idx2);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_bottom.push_back(tri1);
		tris_top.push_back(tri2);
		tris_top.push_back(tri3);
		return false;
	}
	if((vert2->get_position().y>=y)&&(vert1->get_position().y<=y)&&(vert3->get_position().y<=y)){
		vert_1->set_position(this->get_x_for_y_between_verts(vert2, vert1, y), y);
		vert_11->set_position(vert_1->get_position().x, y);
		vert_2->set_position(this->get_x_for_y_between_verts(vert2, vert3, y), y);
		vert_21->set_position(vert_2->get_position().x, y);
		tri1->set_output_idx_1(in_idx2);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx3);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_bottom.push_back(tri1);
		tris_top.push_back(tri2);
		tris_top.push_back(tri3);
		return false;
	}
	if((vert3->get_position().y>=y)&&(vert1->get_position().y<=y)&&(vert2->get_position().y<=y)){
		vert_1->set_position(this->get_x_for_y_between_verts(vert3, vert1, y), y);
		vert_11->set_position(vert_1->get_position().x, y);
		vert_2->set_position(this->get_x_for_y_between_verts(vert3, vert2, y), y);
		vert_21->set_position(vert_2->get_position().x, y);
		tri1->set_output_idx_1(in_idx3);
		tri1->set_output_idx_2(idx1);
		tri1->set_output_idx_3(idx2);

		tri2->set_output_idx_1(idx11);
		tri2->set_output_idx_2(idx21);
		tri2->set_output_idx_3(in_idx1);
		tri3->set_output_idx_1(in_idx2);
		tri3->set_output_idx_2(in_idx1);
		tri3->set_output_idx_3(idx21);
		this->add_triangle(tri1);
		this->add_triangle(tri2);
		this->add_triangle(tri3);
		tris_bottom.push_back(tri1);
		tris_top.push_back(tri2);
		tris_top.push_back(tri3);
		return false;
	}
	return false;

}

result 
SubGeom::set_uvs()
{

	double u = 0.0;
	double v = 0.0;
	BLOCKS::Material* thisMat = reinterpret_cast<BLOCKS::Material* >(this->material_block);
	if(thisMat!=NULL){
		u = thisMat->texture_u;
		v = thisMat->texture_v;
		this->mat_type=thisMat->get_material_type();
		this->uv_tl=new GEOM::VECTOR2D(thisMat->texture_u, thisMat->texture_v);
		if(thisMat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL){
			// solid color is simple: just use same uv for each vert
			this->uv_br=new GEOM::VECTOR2D(thisMat->texture_u, thisMat->texture_v);
			for(SubGeomInternal* subGeom : this->sub_geoms){
				std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
				for(GeomStreamElementBase* vert_base: verts){
					GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
					vert->set_uv(u, v);
				}
			}
		}
		else {
			//this->uv_transform->prepend(this->registration_uv_transform);

			// gradient or texture:
			
			// Get the size of the area on which Animate project the uv:

			double projection_width= 1638.4;
			double projection_height= 1638.4;
			if(thisMat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
				projection_width = (double(thisMat->get_texture()->get_width())/double(20));
				projection_height = (double(thisMat->get_texture()->get_height())/double(20));
			}
			double projection_width_half= projection_width * 0.5;
			double projection_height_half= projection_height * 0.5;
			
			//	Get and invert the uv transform:
			TYPES::F64* uv_transform = this->uv_transform->get();
			TYPES::F64 a =  uv_transform[0];
			TYPES::F64 b =  uv_transform[1];
			TYPES::F64 c =  uv_transform[2];
			TYPES::F64 d =  uv_transform[3];
			TYPES::F64 tx =  uv_transform[4];
			TYPES::F64 ty =  uv_transform[5];

			TYPES::F64 a_inv =  d / (a*d - b*c);
			TYPES::F64 b_inv =  -b / (a*d - b*c);
			TYPES::F64 c_inv =  -c / (a*d - b*c);
			TYPES::F64 d_inv =  a / (a*d - b*c);
			TYPES::F64 tx_inv =  (c*ty - d*tx)/(a*d - b*c);
			TYPES::F64 ty_inv =  -(a*ty - b*tx)/(a*d - b*c);
			
			
			TYPES::F64 a_out = (a_inv / projection_width)*(1-(1/256));
			TYPES::F64 b_out = 0;//(b_inv / projection_width)*(1-(1/256));
			TYPES::F64 c_out = (c_inv / projection_width)*(1-(1/256));
			TYPES::F64 d_out = 0;//(d_inv / projection_width)*(1-(1/256));
			TYPES::F64 tx_out = (thisMat->texture_u)+((tx_inv + projection_width_half)/projection_width)*(1-(1/256));
			TYPES::F64 ty_out = (thisMat->texture_v);//+((ty_inv + projection_height_half)/projection_width)*(1-(1/256));
			

			uv_transform[0] = a_out;
			uv_transform[1] = b_out;
			uv_transform[2] = c_out;
			uv_transform[3] = d_out;
			uv_transform[4] = tx_out;
			uv_transform[5] = ty_out; 

			if(thisMat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
				uv_transform[0] = a_inv / projection_width_half;
				uv_transform[1] = b_inv / projection_width_half;
				uv_transform[2] = c_inv / projection_width_half;
				uv_transform[3] = d_inv / projection_width_half;
				uv_transform[4] = ((tx_inv + projection_width_half)/projection_width_half)-1;
				uv_transform[5] = ((ty_inv + projection_height_half)/projection_width_half)-1;
			}
			else if(thisMat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
				uv_transform[0] = a_inv / projection_width;
				uv_transform[1] = b_inv / projection_width;
				uv_transform[2] = c_inv / projection_width;
				uv_transform[3] = d_inv / projection_width;
				uv_transform[4] = ((tx_inv)/projection_width);
				uv_transform[5] = ((ty_inv)/projection_width);
			}
			//switch between uint16verts and float positions
			//this->uv_transform->prepend(this->registration_uv_transform);

			double u_max = thisMat->texture_u_max;
			double v_max = thisMat->texture_v_max;
			this->uv_br=new GEOM::VECTOR2D(thisMat->texture_u_max - thisMat->texture_u, thisMat->texture_v_max - thisMat->texture_v);

			if(thisMat->get_material_type()!=MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
				this->uv_tl=new GEOM::VECTOR2D(0,0);
				this->uv_br=new GEOM::VECTOR2D(1,1);
				u=0.0;
				v=0.0;
				u_max=1.0;
				v_max=1.0;
			}
			double this_width = this->max_x - this->min_x;
			double this_height = this->max_y - this->min_y;
			double this_u_width = u_max - u;
			double this_v_height = v_max - v;
			for(SubGeomInternal* subGeom : this->sub_geoms){
				std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
				for(GeomStreamElementBase* vert_base: verts){
					GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
					double new_x= vert->get_position().x * a_inv + vert->get_position().y * c_inv + tx_inv;
					double new_y= vert->get_position().x * b_inv + vert->get_position().y * d_inv + ty_inv;
					vert->set_uv(new_x, new_y);
				}
				for(GeomStreamElementBase* vert_base: verts){
					GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
					double new_x1=0;
					double new_y1=0;
					if(thisMat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
						new_x1 = vert->get_uv().x / projection_width;
						new_y1 = 1-(vert->get_uv().y / projection_height);
					}
					else{
						new_x1= (vert->get_uv().x + projection_width_half)/(projection_width);
						new_y1= (vert->get_uv().y + projection_height_half)/(projection_height);
					}
					if(new_x1<0){
						if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_EXTEND)
							new_x1=0;
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REFLECT)
							new_x1*=-1;
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REPEAT)
							new_x1=1+new_x1;
					}
					else if(new_x1>1) {
						if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_EXTEND)
							new_x1=1;
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REFLECT){
							int testx=new_x1;
							if(testx<new_x1)
								testx++;
							new_x1=testx-new_x1;
						}
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REPEAT){
							while (new_x1>1)
								new_x1--;
						}
					}
					if(new_y1<0){
						if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_EXTEND)
							new_y1=0;
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REFLECT)
							new_y1*=-1;
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REPEAT)
							new_y1=1+new_y1;
					}
					else if(new_y1>1) {
						if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_EXTEND)
							new_y1=1;
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REFLECT){
							int testx=new_y1;
							if(testx<new_y1)
								testx++;
							new_y1=testx-new_y1;
						}
						else if(thisMat->gradient_spread==MATERIAL::GradientSpread::GRADIENT_SPREAD_REPEAT){
							while (new_y1>1)
								new_y1--;
						}
					}
					vert->set_uv(u+((new_x1)*this_u_width), v+((1-new_y1)*this_v_height));
				}
			}
		}
	}
	else{
		for(SubGeomInternal* subGeom : this->sub_geoms){
			std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
			for(GeomStreamElementBase* vert_base: verts){
				GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
				vert->set_uv(u, v);
			}
		}
	}
	return result::AWD_SUCCESS;
}

result 
SubGeom::merge_stream_refactor(GEOM::SubGeom* target_geom){
	
	if(!this->isMerged_refactor){
		return result::AWD_ERROR;
	}
	this->startIDX = target_geom->tri_cnt * 3;
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
	int vert_cnt=0;
	for(GEOM::Triangle* tri: this->sub_geoms.back()->get_triangles()){
		SUBGEOM_ID_STRING subGeo_id;
		target_geom->get_internal_id(subGeo_id);
		int idx1=target_geom->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), true);
		int idx2=target_geom->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), true);
		int idx3=target_geom->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), true);
		GEOM::Triangle* new_triangle = new Triangle(idx1, idx2, idx3);
		target_geom->add_triangle(new_triangle);
	}
	
	return result::AWD_SUCCESS;
}
result 
SubGeom::merge_stream(GEOM::SubGeom* target_geom){
	
	if(!this->isMerged){
		return result::AWD_ERROR;
	}
	this->startIDX = target_geom->tri_cnt * 3;
	this->target_subgeom = target_geom;
	std::vector<GEOM::GeomStreamElementBase*> verts = this->sub_geoms.back()->get_vertices();
	for(GEOM::Triangle* tri: this->sub_geoms.back()->get_triangles()){
		TYPES::UINT32 idx1 = target_geom->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), false);
		TYPES::UINT32 idx2 = target_geom->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), false);
		TYPES::UINT32 idx3 = target_geom->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), false);
		GEOM::Triangle* new_triangle = new Triangle(idx1, idx2, idx3);
		target_geom->add_triangle(new_triangle);
	}
	
	return result::AWD_SUCCESS;
}
bool
SubGeom::isClockWiseXY(double x1, double y1, double x2, double y2, double x3, double y3)
{
   return (((y2 - y1)*(x3 - x2) - (y3 - y2)*(x2 - x1)) < 0);
}

result
SubGeom::create_triangle(GEOM::edge_type edge_type, GEOM::VECTOR2D v1, GEOM::VECTOR2D v2, GEOM::VECTOR2D v3)
{
	if(this->sub_geoms.size()==0)
		return result::AWD_ERROR;
	this->tri_cnt++;

	
	GEOM::VECTOR2D vert_1 = v1;
	GEOM::VECTOR2D vert_ctr = v2;
	GEOM::VECTOR2D vert_2 = v3;
	
	if(!isClockWiseXY(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y)){
		vert_1 = v3;
		vert_ctr = v2;
		vert_2 = v1;
	}
	
	GEOM::Vertex2D* vert1 = new Vertex2D();
	vert1->set_position(vert_1.x, vert_1.y);

	GEOM::Vertex2D* vert2 = new Vertex2D();
	vert2->set_position(vert_ctr.x, vert_ctr.y);

	GEOM::Vertex2D* vert3 = new Vertex2D();
	vert3->set_position(vert_2.x, vert_2.y);

	GEOM::Triangle* new_triangle;

	if(edge_type == GEOM::edge_type::CONCAVE_EDGE){
		vert1->set_curve_attributes(-128, 127, 127);
		vert2->set_curve_attributes(-128, 63, 0); 
		vert3->set_curve_attributes(-128, 0, 0);
	}
	else if(edge_type == GEOM::edge_type::CONVEX_EDGE){
		vert1->set_curve_attributes(127, 127, 127);
		vert2->set_curve_attributes(127, 63, 0);
		vert3->set_curve_attributes(127, 0, 0);
	}
	else{
		vert1->set_curve_attributes(127, 0, -128); //127, 127, 0);
		vert2->set_curve_attributes(127, 0, -128);
		vert3->set_curve_attributes(127, 0, -128);
	}
	if((this->settings->get_stream_by_type(GEOM::stream_type::COMBINED_POSITION_2D)!=NULL)||
		(this->settings->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D__5F)!=NULL)||
		(this->settings->get_stream_by_type(GEOM::stream_type::POSITIONS_2D)!=NULL)||
		(this->settings->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D__2F3B)!=NULL)||
		(this->settings->get_stream_by_type(GEOM::stream_type::COMBINED_POSITION_2D_INT16)!=NULL)||
		(this->settings->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D_2xINT16_3xINT8)!=NULL)){
		TYPES::UINT32 idx1 = this->add_vertex2D(vert1, false);
		TYPES::UINT32 idx2 = this->add_vertex2D(vert2, false);
		TYPES::UINT32 idx3 = this->add_vertex2D(vert3, false);
		new_triangle = new Triangle(idx1, idx2, idx3);
		this->sub_geoms.back()->add_triangle(new_triangle);
	}
	else{	
		TYPES::UINT32 idx1 = this->add_vertex2D(vert1, true);
		TYPES::UINT32 idx2 = this->add_vertex2D(vert2, true);
		TYPES::UINT32 idx3 = this->add_vertex2D(vert3, true);
		new_triangle = new Triangle(idx1, idx2, idx3);
		this->sub_geoms.back()->add_triangle_by_type(new_triangle, edge_type);
	}
	return result::AWD_SUCCESS;
}

TYPES::INT32
SubGeom::add_triangle(GEOM::Triangle* new_triangle)
{
	if(this->sub_geoms.back()==NULL)
		return -1;
	this->tri_cnt++;
	allowed_tris--;
	return this->sub_geoms.back()->add_triangle(new_triangle);
}

TYPES::INT32 
SubGeom::add_vertex2D(GEOM::Vertex2D* vertex, bool use_cache)
{
	if(this->sub_geoms.back()==NULL)
		return -1;
	if(max_x<vertex->get_position().x)
		max_x=vertex->get_position().x;
	if(min_x>vertex->get_position().x)
		min_x=vertex->get_position().x;
	if(max_y<vertex->get_position().y)
		max_y=vertex->get_position().y;
	if(min_y>vertex->get_position().y)
		min_y=vertex->get_position().y;

	if(!use_cache)
		return this->sub_geoms.back()->add_vertex(vertex);

	return this->sub_geoms.back()->add_vertex_with_cache(vertex);

}
TYPES::INT32 
SubGeom::add_vertex(GEOM::Vertex3D* vertex)
{
	if(this->sub_geoms.back()==NULL)
		return -1;
	allowed_tris--;
	return this->sub_geoms.back()->add_vertex(vertex);
}

void 
SubGeom::set_settings(SETTINGS::BlockSettings * settings)
{
	this->settings = settings;
}

SETTINGS::BlockSettings * 
SubGeom::get_settings()
{
	return this->settings;
}

std::vector<SubGeomInternal*>& 
SubGeom::get_sub_geoms()
{
	return this->sub_geoms;
}

void 
SubGeom::set_sub_geoms(std::vector<SubGeomInternal*>& sub_geoms)
{
	this->sub_geoms = sub_geoms;
}

TYPES::UINT32 
SubGeom::get_max_stream_attributes_length(stream_target stream_target_type)
{
	int max_attr_length=0;
	for(DataStreamRecipe* data_stream : this->settings->get_stream_recipes()){
		int attr_length=0;
		if(data_stream->get_stream_target_type()==stream_target_type){
			for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions())
				attr_length+=attr_desc.get_data_length() * get_data_type_size_for_precision_category(attr_desc.get_data_type(), this->settings, attr_desc.get_storage_cat());
		}
		if(max_attr_length<attr_length)
			max_attr_length=attr_length;
	}		
	return max_attr_length;
}

TYPES::UINT32 
SubGeom::calc_stream_length(DataStreamRecipe* data_stream, SubGeomInternal* subGeo)
{
	int element_cnt=subGeo->get_vert_cnt();
	if(data_stream->get_stream_target_type()==stream_target::TRIANGLE_STREAM)
		element_cnt=subGeo->get_tri_cnt();
	else if(data_stream->get_stream_target_type()==stream_target::INTERIOR_TRIANGLES)
		element_cnt=subGeo->get_tri_cnt_for_type(GEOM::edge_type::INNER_EDGE);
	else if(data_stream->get_stream_target_type()==stream_target::EXTERIOR_TRIANGLES)
		element_cnt=subGeo->get_tri_cnt_for_type(GEOM::edge_type::OUTTER_EDGE);
	else if(data_stream->get_stream_target_type()==stream_target::CONCAVE_TRIANGLES)
		element_cnt=subGeo->get_tri_cnt_for_type(GEOM::edge_type::CONCAVE_EDGE);
	else if(data_stream->get_stream_target_type()==stream_target::CONVEX_TRIANGLES)
		element_cnt=subGeo->get_tri_cnt_for_type(GEOM::edge_type::CONVEX_EDGE);
	
	int attr_length=0;
	for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions())
		attr_length+= get_data_type_size_for_precision_category(attr_desc.get_data_type(), this->settings, attr_desc.get_storage_cat()) * attr_desc.get_data_length();
	return attr_length * element_cnt;
}

TYPES::UINT32 
SubGeom::calc_subgeom_streams_length(SubGeomInternal* subGeo)
{
	int streams_length=0;
	for(DataStreamRecipe* data_stream : this->settings->get_stream_recipes())
		streams_length += 6 +  this->calc_stream_length(data_stream, subGeo);
	/*
	if(this->slice9ScaleStream.size()>0){
		streams_length += 6 +  this->slice9ScaleStream.size() * 4;
	}
	*/
	return streams_length;
}

TYPES::UINT16
SubGeom::get_num_subs()
{
	return TYPES::UINT16(this->sub_geoms.size());
}

TYPES::UINT32
SubGeom::calc_sub_length()
{
	TYPES::UINT32 len = 0;
	if(this->isScaled9){
		len += 4; // length element
		len += 8; // properties + attr
		
		len += 6 + this->slice9ScalePositionsStream.size()*4;

		len += 6 +  this->slice9ScaleStream.size() * 4;
	}
	else if(this->isMerged){
		len += 4; // length element
		len += 8; // properties + attr
		
		len += 1;// stream_type
		len += 1;// stream_data_type
		len += 4;// stream_length

		len += 4;// startIndex
		len += 4;// vertCnt
		len += 4;//target_geom_id
		len += 1;//target_subgeom id
		
	}
	else if(this->isMerged_refactor){
		len += 4; // length element
		len += 8; // properties + attr
		
		len += 1;// stream_type
		len += 1;// stream_data_type
		len += 4;// stream_length

		len += 4;// startIndex
		len += 4;// vertCnt
	}
	else{
		for(SubGeomInternal* subGeom : this->sub_geoms){
			len += 4 + this->calc_subgeom_streams_length(subGeom);
			len += this->calc_attr_length(true,true, this->settings);
		}
	}
	return len;
}

result
SubGeom::write_vertex_stream(FileWriter* filewWriter, DataStreamRecipe* data_stream, SubGeomInternal* subGeo)
{
	result res = result::AWD_SUCCESS;
	// find the attr_length
	int attr_length=0;
	for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions())
		attr_length+=get_data_type_size_for_precision_category(attr_desc.get_data_type(), this->settings, attr_desc.get_storage_cat())*attr_desc.get_data_length();

	if(data_stream->is_homogen()){
		// if the stream is homogen, we can collect all data into one big buffer, and than write this to file directly.
		// malloc enough memory for the stream
		TYPES::union_ptr stream_data;
		TYPES::UINT32 values_cnt=0;
		stream_data.v = (void*)malloc(attr_length*subGeo->get_vert_cnt());
		std::vector<GeomStreamElementBase*> verts = subGeo->get_vertices();
		for(GeomStreamElementBase* vert : verts){
			for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions())
				values_cnt+=GEOM::append_vertex_attr_to_stream(stream_data, reinterpret_cast<Vertex2D*>(vert), &attr_desc, values_cnt);
		}
		int data_type_size=get_single_val_data_type_size_for_precision_category(data_stream->get_attr_descriptions()[0].get_data_type(),this->settings, data_stream->get_attr_descriptions()[0].get_storage_cat());
		if(values_cnt*data_type_size!=attr_length*subGeo->get_vert_cnt())
			return result::AWD_ERROR;
		res = filewWriter->writeBytes(reinterpret_cast<TYPES::UINT8*>(stream_data.v), values_cnt*data_type_size);
		if(res!=result::AWD_SUCCESS)
			return result::AWD_ERROR;
	}
	else{
		// if the stream is not homogen, we need to write each DataStreamAttr on its own.
		// \todo: heterogen vertices streams are not tested yet.
		std::vector<GeomStreamElementBase*> verts = subGeo->get_vertices();
		for(GeomStreamElementBase* vert : verts){
			for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions()){
				/*
				
				*/
				Vertex2D* vertex2d=reinterpret_cast<Vertex2D*>(vert);
				if(attr_desc.get_type()==GEOM::data_stream_attr_type::POSITION2D){
					filewWriter->writeFLOAT32((TYPES::F32)vertex2d->get_position().x);
					filewWriter->writeFLOAT32((TYPES::F32)vertex2d->get_position().y);
				}
				else if(attr_desc.get_type()==GEOM::data_stream_attr_type::POSITION2D_UINT16){
					filewWriter->writeUINT16((TYPES::UINT16)vertex2d->get_position_uint16().x);
					filewWriter->writeUINT16((TYPES::UINT16)vertex2d->get_position_uint16().y);
				}
				else if(attr_desc.get_type()==GEOM::data_stream_attr_type::CURVE_DATA_2D_INT){
					filewWriter->writeINT8((TYPES::INT8)vertex2d->get_curve_attributes().x);
					filewWriter->writeINT8((TYPES::INT8)(vertex2d->get_curve_attributes().y));
					filewWriter->writeINT8((TYPES::INT8)vertex2d->get_curve_attributes().z);
					filewWriter->writeINT8(0);
				}
				//GEOM::write_stream_attr_to_file(filewWriter, vert, &attr_desc);
				//if(res!=result::AWD_SUCCESS)
				//	return result::AWD_ERROR;
			}
		}
	}
	return res;
}

result
SubGeom::write_triangle_stream(FileWriter* filewWriter, DataStreamRecipe* data_stream, SubGeomInternal* subGeo)
{
	result res = result::AWD_SUCCESS;
	// find the attr_length
	int attr_length=0;
	for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions())
		attr_length+=get_data_type_size_for_precision_category(attr_desc.get_data_type(),this->settings, attr_desc.get_storage_cat())*attr_desc.get_data_length();

	if(data_stream->is_homogen()){
		// if the stream is homogen, we can collect all data into one buffer, and than write this to file directly.

		// malloc enough memory for the stream
		TYPES::union_ptr stream_data;
		TYPES::UINT32 values_cnt=0;
		stream_data.v = (void*)malloc(attr_length*subGeo->get_tri_cnt());
		std::vector<Triangle*> tris = subGeo->get_triangles();
		for(Triangle* tri : tris){
			for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions())
				values_cnt+=GEOM::append_triangle_attr_to_stream(stream_data, tri, &attr_desc, values_cnt);
		}
		int data_type_size=get_single_val_data_type_size_for_precision_category(data_stream->get_attr_descriptions()[0].get_data_type(),this->settings, data_stream->get_attr_descriptions()[0].get_storage_cat());
		if(values_cnt*data_type_size!=attr_length*subGeo->get_tri_cnt())
			return result::AWD_ERROR;
		res = filewWriter->writeBytes(reinterpret_cast<TYPES::UINT8*>(stream_data.v), values_cnt*data_type_size);
		if(res!=result::AWD_SUCCESS)
			return result::AWD_ERROR;
	}
	else{
		// if the stream is not homogen, we need to write each DataStreamAttr on its own.
		// \todo: heterogen Triangle streams are not tested yet.
		std::vector<Triangle*> tris = subGeo->get_triangles();
		for(Triangle* tri : tris){
			for(DataStreamAttrDesc attr_desc : data_stream->get_attr_descriptions()){
				GEOM::write_stream_attr_to_file(filewWriter, tri, &attr_desc);
				if(res!=result::AWD_SUCCESS)
					return result::AWD_ERROR;
			}
		}
	}return res;
}

result
SubGeom::write_subgeom_streams(FileWriter* filewWriter, SubGeomInternal* subGeo)
{
	result res = result::AWD_SUCCESS;
	for(DataStreamRecipe* data_stream : this->settings->get_stream_recipes()){
		
		res = filewWriter->writeUINT8(TYPES::UINT8(data_stream->get_stream_type()));
		res = filewWriter->writeUINT8(TYPES::UINT8(data_stream->get_attr_descriptions()[0].get_data_type()));
		res = filewWriter->writeUINT32(this->calc_stream_length(data_stream, subGeo));

		if(data_stream->get_stream_target_type()==stream_target::VERTEX_STREAM)
			res = this->write_vertex_stream(filewWriter, data_stream, subGeo);
		if(data_stream->get_stream_target_type()==stream_target::TRIANGLE_STREAM)
			res = this->write_triangle_stream(filewWriter, data_stream, subGeo);
	}
	/*
	if(this->slice9ScaleStream.size()>0){
		res = filewWriter->writeUINT8(23);
		res = filewWriter->writeUINT8(0);
		res = filewWriter->writeUINT32(this->slice9ScaleStream.size() * 4);
		for(TYPES::UINT32 elem:this->slice9ScaleStream){
			res = filewWriter->writeUINT32(elem);
		}
	}
	*/

	return res;
}

result
SubGeom::write_sub(FileWriter* fileWriter)
{
	result res = result::AWD_SUCCESS;
	if(this->isScaled9){
		int len = 6 + this->slice9ScalePositionsStream.size()*4;
		len += 6 + this->slice9ScaleStream.size()*4;
		fileWriter->writeUINT32(len + 4); // 6stream header + 12 streamdata + 4 props
		fileWriter->writeUINT32(0); // properties
		
		res = fileWriter->writeUINT8(23);
		res = fileWriter->writeUINT8(0);
		res = fileWriter->writeUINT32(this->slice9ScaleStream.size() * 4);
		for(TYPES::INT32 elem:this->slice9ScaleStream){
			res = fileWriter->writeINT32(elem);
		}
		   
		res = fileWriter->writeUINT8(16);
		res = fileWriter->writeUINT8(0);
		res = fileWriter->writeUINT32(this->slice9ScalePositionsStream.size() * 4);
		for(TYPES::F32 elem:this->slice9ScalePositionsStream){
			res = fileWriter->writeFLOAT32(elem);
		}

		fileWriter->writeUINT32(0); // user attr
	}
	else if(this->isMerged){
		// only write 1 stream with 3 attributes (id of target geom, startIndex, length)
		fileWriter->writeUINT32(22); // 6stream header + 12 streamdata + 4 props
		fileWriter->writeUINT32(0); // properties
		
		// 6
		fileWriter->writeUINT8((TYPES::UINT8)GEOM::stream_type::CONCANETEDSTREAM_2F3B);// stream_type
		fileWriter->writeUINT8(0);// stream_data_type
		fileWriter->writeUINT32(13);// stream_length
		
		// 13
		fileWriter->writeUINT32(this->target_subgeom->merged_address);//target_geom_id
		fileWriter->writeUINT8(0);//target_geom_id
		fileWriter->writeUINT32(this->startIDX);// startIndex
		fileWriter->writeUINT32(this->tri_cnt*3);// vertCnt

		// 4
		fileWriter->writeUINT32(0); // user attr
	}
	else if(this->isMerged_refactor){
		fileWriter->writeUINT32(17); // 6stream header + 12 streamdata + 4 props
		fileWriter->writeUINT32(0); // properties
		
		fileWriter->writeUINT8((TYPES::UINT8)GEOM::stream_type::CONCANETEDSTREAM_INCL_INDICES);// stream_type
		fileWriter->writeUINT8(0);// stream_data_type
		fileWriter->writeUINT32(8);// stream_length
		
		fileWriter->writeUINT32(this->startIDX);// startIndex
		fileWriter->writeUINT32(this->tri_cnt*3);// vertCnt
		fileWriter->writeUINT32(0); // user attr
	}
	else{
		for(SubGeomInternal* subGeom : this->sub_geoms){
			TYPES::UINT32 sub_len = this->calc_subgeom_streams_length(subGeom);

			// Write sub-mesh header
			fileWriter->writeUINT32(sub_len);

			this->properties->write_attributes(fileWriter, this->settings);

			// if the length in bytes is bigger 0, we write the subgeom
			if(sub_len>0)
				res = this->write_subgeom_streams(fileWriter, subGeom);
			fileWriter->writeUINT32(0);
			//this->user_attributes->write_attributes(fileWriter, this->settings);
		}	
	}
	return res;
}

#include "blocks/geometry.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "utils/util.h"
#include "utils/awd_globals.h"

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
	this->uv_transform=new MATRIX2x3();
	this->tri_cnt=0;
	this->material_block=NULL;
	this->allowed_tris=0;
	this->settings = subGeomSettings->clone_block_settings();
	this->max_x=std::numeric_limits<double>::max()*-1;
	this->max_y=std::numeric_limits<double>::max()*-1;
	this->min_x=std::numeric_limits<double>::max();
	this->min_y=std::numeric_limits<double>::max();
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
		output_str=std::to_string(this->sub_geoms.size()-1);
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
	this->allowed_tris = RESSOURCE_LIMIT / max_attr_length;
	
	SubGeomInternal* this_subGeo = new SubGeomInternal();
	this->sub_geoms.push_back(this_subGeo);		

	output_str=std::to_string(this->sub_geoms.size()-1);
	return;
}

				void set_material(BASE::AWDBlock*);
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
	if(subgeom_to_merge->get_sub_geoms().size()==0)
		return result::AWD_SUCCESS;


	if(this->material_block!=subgeom_to_merge->material_block)
		return result::AWD_ERROR;


	SubGeomInternal* last_subgeo=this->sub_geoms.back();
	this->sub_geoms.pop_back();
	
	for(SubGeomInternal* subGeom : subgeom_to_merge->get_sub_geoms()){
		if(subGeom!=subgeom_to_merge->get_sub_geoms().back()){
			this->sub_geoms.push_back(subGeom);
		}
	}
	this->sub_geoms.push_back(last_subgeo);
	std::vector<GEOM::GeomStreamElementBase*> verts = subgeom_to_merge->get_sub_geoms().back()->get_vertices();
	if(subgeom_to_merge->get_settings()->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D__9F)!=NULL){
		for(GEOM::Triangle* tri: subgeom_to_merge->get_sub_geoms().back()->get_triangles()){
			std::string id;
			this->get_internal_id(id);
			TYPES::UINT32 idx1 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_1()]), false);
			TYPES::UINT32 idx2 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_2()]), false);
			TYPES::UINT32 idx3 = this->add_vertex2D(reinterpret_cast<GEOM::Vertex2D*>(verts[tri->get_output_idx_3()]), false);
			this->sub_geoms.back()->add_triangle(new Triangle( idx1,  idx2, idx3));
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
SubGeom::set_uvs()
{
	// todo: get the bounds of the subgeo. apply the uv according to bounds and uv-transform

	double u = 0.0;
	double v = 0.0;
	BLOCKS::Material* thisMat = reinterpret_cast<BLOCKS::Material* >(this->material_block);
	if(thisMat!=NULL){
		u = thisMat->texture_u;
		v = thisMat->texture_v;
		if(thisMat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL){
			for(SubGeomInternal* subGeom : this->sub_geoms){
				std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
				for(GeomStreamElementBase* vert_base: verts){
					GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
					vert->set_uv(u, v);
				}
			}
		}
		else {
			double sizex= 1638.4;
			double sizey= 1638.4;
			if(thisMat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
				sizex = (double(thisMat->get_texture()->get_width())/double(20));
				sizey = (double(thisMat->get_texture()->get_height())/double(20));
			}
			double sizex_half= sizex * 0.5;
			double sizey_half= sizey * 0.5;
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
			
			double u_max = thisMat->texture_u_max;
			double v_max = thisMat->texture_v_max;
			//GEOM::MATRIX2x3* uv_transform = thisMat->get_uv_transform_mtx();
			double this_width = this->max_x - this->min_x;
			double this_height = this->max_y - this->min_y;
			double this_u_width = u_max - u;
			double this_v_height = v_max - v;
			double max_u_value=std::numeric_limits<double>::max()*-1;
			double max_v_value=std::numeric_limits<double>::max()*-1;
			double min_u_value=std::numeric_limits<double>::max();
			double min_v_value=std::numeric_limits<double>::max();
			for(SubGeomInternal* subGeom : this->sub_geoms){
				std::vector<GEOM::GeomStreamElementBase*> verts = subGeom->get_vertices();
				for(GeomStreamElementBase* vert_base: verts){
					GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
					//vert->set_uv(u, v);
					
					//double new_x= vert->get_position().x * a + vert->get_position().y * c + tx;
					//double new_y= vert->get_position().x * b + vert->get_position().y * d + ty;
					double new_x= vert->get_position().x * a_inv + vert->get_position().y * c_inv + tx_inv;
					double new_y= vert->get_position().x * b_inv + vert->get_position().y * d_inv + ty_inv;
					//vert->set_uv(u+((((vert->get_position().x-this->min_x)/this_width))*this_u_width), v+(1-((vert->get_position().y-this->min_y)/this_height))*this_v_height);
					vert->set_uv(new_x, new_y);
					//vert->set_position(new_x, new_y);
					//vert->set_uv(u+((((vert->get_position().x-this->min_x)/this_width))*this_u_width), v+(1-((vert->get_position().y-this->min_y)/this_height))*this_v_height);
				}
				double max_u = max_u_value-min_u_value;
				double max_v = max_v_value-min_v_value;
				for(GeomStreamElementBase* vert_base: verts){
					GEOM::Vertex2D* vert = reinterpret_cast<GEOM::Vertex2D*>(vert_base);
					//vert->set_uv(u, v);
					double new_x1=0;
					double new_y1=0;
					if(thisMat->get_material_type()==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
						new_x1 = vert->get_uv().x / sizex;
						new_y1 = 1-(vert->get_uv().y / sizey);
					}
					else{
						new_x1= (vert->get_uv().x + sizex_half)/(sizex);
						new_y1= (vert->get_uv().y + sizey_half)/(sizey);
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
					//vert->set_position(new_x, new_y);
					//vert->set_uv(u+((((vert->get_position().x-this->min_x)/this_width))*this_u_width), v+(1-((vert->get_position().y-this->min_y)/this_height))*this_v_height);
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

bool
SubGeom::isClockWiseXY(double point1x, double point1y, double point2x, double point2y, double point3x, double point3y)
{
   return ((point1x - point2x) * (point3y - point2y) - (point1y - point2y) * (point3x - point2x) < 0);
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
		vert1->set_curve_attributes(-1, 1, 1);
		vert2->set_curve_attributes(-1, 0.5, 0); 
		vert3->set_curve_attributes(-1, 0, 0);
	}
	else if(edge_type == GEOM::edge_type::CONVEX_EDGE){
		vert1->set_curve_attributes(1, 1, 1);
		vert2->set_curve_attributes(1, 0.5, 0);
		vert3->set_curve_attributes(1, 0, 0);
	}
	else if (edge_type==GEOM::edge_type::INNER_EDGE){
		vert1->set_curve_attributes(1, 2, 1);
		vert2->set_curve_attributes(1, 3, 1);
		vert3->set_curve_attributes(1, 4, 1);
	}
	else{
		vert1->set_curve_attributes(1, 2, 0);
		vert2->set_curve_attributes(1, 2, 1);
		vert3->set_curve_attributes(1, 2, 0);
	}
	if(this->settings->get_stream_by_type(GEOM::stream_type::ALLVERTDATA2D__9F)!=NULL){
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
	for(SubGeomInternal* subGeom : this->sub_geoms){
		len += 4 + this->calc_subgeom_streams_length(subGeom);
		len += this->calc_attr_length(true,true, this->settings);
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
				GEOM::write_stream_attr_to_file(filewWriter, vert, &attr_desc);
				if(res!=result::AWD_SUCCESS)
					return result::AWD_ERROR;
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
	return res;
}

result
SubGeom::write_sub(FileWriter* fileWriter)
{
	result res = result::AWD_SUCCESS;
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
	return res;
}

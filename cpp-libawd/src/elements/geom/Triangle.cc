#include "elements/stream_attributes.h"
#include "elements/geom_elements.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;

Triangle::Triangle():
	GeomStreamElementBase()
{
}
Triangle::~Triangle()
{
}
Triangle::Triangle(TYPES::UINT32 v1_index, GEOM::Vertex3D* v1, TYPES::UINT32 v2_index, GEOM::Vertex3D* v2, TYPES::UINT32 v3_index, GEOM::Vertex3D* v3)
{
	this->v1=v1;
	this->original_vert_index_1=v1_index;
	this->v2=v2;
	this->original_vert_index_2=v2_index;
	this->v3=v3;
	this->original_vert_index_3=v3_index;
}
Triangle::Triangle(TYPES::UINT32 v1_index, TYPES::UINT32 v2_index, TYPES::UINT32 v3_index)
{
	this->v1=NULL;
	this->output_idx_1=v1_index;
	this->v2=NULL;
	this->output_idx_2=v2_index;
	this->v3=NULL;
	this->output_idx_3=v3_index;
}



void
Triangle::get_verts(std::vector<Vertex3D*>& all_verts)
{
	all_verts[0]=v1;
	all_verts[1]=v2;
	all_verts[3]=v3;

	return;
}

void 
Triangle::reset_out_indices()
{
	this->tri_out_indices.clear();
}

std::string&
Triangle::intern_get_vertex_string()
{
	this->lookup_string = "";
	return this->lookup_string;
}

void 
Triangle::add_out_index(TYPES::UINT32 idx)
{
	tri_out_indices.push_back(idx);
}

std::vector<TYPES::UINT32>& 
Triangle::get_indices()
{
	return tri_out_indices;
}

void 
Triangle::set_v1(GEOM::Vertex3D* v1)
{
	this->v1 = v1;
}

GEOM::Vertex3D*  
Triangle::get_v1()
{
	return this->v1;
}

void 
Triangle::set_v2(GEOM::Vertex3D* v2)
{
	this->v2=v2;
}

GEOM::Vertex3D*  
Triangle::get_v2()
{
	return this->v2;
}

void 
Triangle::set_v3(GEOM::Vertex3D* v3)
{
	this->v3 = v3;
}

GEOM::Vertex3D*  
Triangle::get_v3()
{
	return this->v3;
}

void 
Triangle::set_original_vert_index_1(TYPES::UINT32 original_vert_index_1)
{
	this->original_vert_index_1 = original_vert_index_1;
}

TYPES::UINT32  
Triangle::get_original_vert_index_1()
{
	return this->original_vert_index_1;
}

void 
Triangle::set_original_vert_index_2(TYPES::UINT32 original_vert_index_2)
{
	this->original_vert_index_2 = original_vert_index_2;
}

TYPES::UINT32  
Triangle::get_original_vert_index_2()
{
	return this->original_vert_index_2;
}

void 
Triangle::set_original_vert_index_3(TYPES::UINT32 original_vert_index_3)
{
	this->original_vert_index_3 = original_vert_index_3;
}

TYPES::UINT32  
Triangle::get_original_vert_index_3()
{
	return this->original_vert_index_3;
}

void 
Triangle::set_output_idx_1(TYPES::UINT16 output_idx_1)
{
	this->output_idx_1 = output_idx_1;
}

TYPES::UINT16  
Triangle::get_output_idx_1()
{
	return this->output_idx_1;
}

void 
Triangle::set_output_idx_2(TYPES::UINT16 output_idx_2)
{
	this->output_idx_2 = output_idx_2;
}

TYPES::UINT16  
Triangle::get_output_idx_2()
{
	return this->output_idx_2;
}

void 
Triangle::set_output_idx_3(TYPES::UINT16 output_idx_3)
{
	this->output_idx_3 = output_idx_3;
}

TYPES::UINT16  
Triangle::get_output_idx_3()
{
	return this->output_idx_3;
}
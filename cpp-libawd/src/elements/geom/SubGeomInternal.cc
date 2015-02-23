#include "elements/subgeometry.h"
#include "utils/awd_types.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
 

SubGeomInternal::SubGeomInternal() 
{
}

SubGeomInternal::~SubGeomInternal()
{
    // \todo Important: delete vert
    /*
	for(GeomStreamElementBase* vert : this->vertices)
        
		delete vert;
         */
	//for(Triangle* tri : this->triangles)
		//delete tri;	
}

TYPES::UINT32 
SubGeomInternal::get_tri_cnt() 
{
	return TYPES::UINT32(this->triangles.size());
}		
TYPES::UINT32
SubGeomInternal::get_vert_cnt() 
{
	return TYPES::UINT32(this->vertices.size());
}

TYPES::UINT32 
SubGeomInternal::add_vertex_with_cache(GeomStreamElementBase* vertex)
{
	std::string vert_cache_str = vertex->get_vertex_string();
	if(this->vertex_cache.find(vert_cache_str)==this->vertex_cache.end()){
		this->vertices.push_back(vertex);
		this->vertex_cache[vert_cache_str] = int(this->vertices.size()-1);
		return TYPES::UINT32(this->vertices.size())-1;
	}
    // \todo Important: delete vert
    /*
	delete vertex;
     */
	return this->vertex_cache[vert_cache_str];
}

TYPES::UINT32 
SubGeomInternal::add_vertex(GeomStreamElementBase* vertex)
{
	this->vertices.push_back(vertex);
	return TYPES::UINT32(this->vertices.size()-1);
}
TYPES::UINT32
SubGeomInternal::add_triangle(Triangle* triangle)
{
	this->triangles.push_back(triangle);
	return TYPES::UINT32(this->triangles.size())-1;
}
std::vector<GeomStreamElementBase*>& 
SubGeomInternal::get_vertices()
{
	return this->vertices;
}	
void 
SubGeomInternal::set_vertices(std::vector<GeomStreamElementBase*>& vertices)
{
	this->vertices = vertices;
}				
std::vector<Triangle*>& 
SubGeomInternal::get_triangles()
{
	return this->triangles;
}
void 
SubGeomInternal::set_triangles(std::vector<Triangle*>& triangles)
{
	this->triangles = triangles;
}
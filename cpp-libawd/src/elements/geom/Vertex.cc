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

Vertex3D::Vertex3D():
	GeomStreamElementBase()
{
}
Vertex3D::~Vertex3D()
{
}

bool 
Vertex3D::compare(Vertex3D*){
	return true;
}

void 
Vertex3D::set_position(TYPES::F64 x, TYPES::F64 y, TYPES::F64 z)
{
	this->position=VECTOR3D(x,y,z);
}

GEOM::VECTOR3D  
Vertex3D::get_position()
{
	return this->position;
}

void 
Vertex3D::set_normal(GEOM::VECTOR3D normal)
{
	this->normal=normal;
}

GEOM::VECTOR3D  
Vertex3D::get_normal()
{
	return this->normal;
}

void 
Vertex3D::set_tangent(GEOM::VECTOR3D tangent)
{
	this->tangent=tangent;
}

GEOM::VECTOR3D  
Vertex3D::get_tangent()
{
	return this->tangent;
}

std::string&
Vertex3D::intern_get_vertex_string()
{
	this->lookup_string = std::to_string(this->position.x);
	this->lookup_string += "#" + std::to_string(this->position.y);
	this->lookup_string += "#" + std::to_string(this->position.z);
	return this->lookup_string;
}
void 
Vertex3D::set_uv_channel(const std::string& channel_id, TYPES::F64 x, TYPES::F64 y)
{
	this->uv_channels[channel_id]=VECTOR2D(x,y);
}
GEOM::VECTOR2D 
Vertex3D::get_uv_channel(std::string& channel_id)
{
	if(this->uv_channels.find(channel_id)==this->uv_channels.end())
		return GEOM::VECTOR2D();
	return this->uv_channels[channel_id];
}





Vertex2D::Vertex2D():
	GeomStreamElementBase()
{
}
Vertex2D::~Vertex2D()
{
}

std::string&
Vertex2D::intern_get_vertex_string()
{
	this->lookup_string = std::to_string(this->position.x);
	this->lookup_string += "#" + std::to_string(this->position.y);
	this->lookup_string += "#" + std::to_string(this->curve_attributes.x);
	this->lookup_string += "#" + std::to_string(this->curve_attributes.y);
	this->lookup_string += "#" + std::to_string(this->curve_attributes.z);
	return this->lookup_string;
}

void 
Vertex2D::set_position(TYPES::F64 x, TYPES::F64 y)
{
	this->position=VECTOR2D(x,y);
	GEOM::DataStreamAttr* data_stream_attr;
	TYPES::union_ptr new_data;
	new_data.v = (TYPES::F64*) malloc (2 * sizeof(TYPES::F64*));
	result res = this->get_attribute(GEOM::data_stream_attr_type::POSITION2D, &data_stream_attr);
	if(res!=result::AWD_SUCCESS){
		data_stream_attr=new GEOM::DataStreamAttr(GEOM::data_stream_attr_type::POSITION2D, new_data);
		this->set_attribute(data_stream_attr);
		return;
	}
	data_stream_attr->set_data(new_data);
}

GEOM::VECTOR2D  
Vertex2D::get_position()
{
	return this->position;
}

GEOM::VECTOR3D  
Vertex2D::get_curve_attributes()
{
	return this->curve_attributes;
}

void 
Vertex2D::set_curve_attributes(TYPES::F64 type, TYPES::F64 curve_1, TYPES::F64 curve_2)
{
	this->curve_attributes=VECTOR3D(type,curve_1, curve_2);
}


GEOM::VECTOR2D  
Vertex2D::get_uv()
{
	return this->uv;
}

void 
Vertex2D::set_uv(TYPES::F64 u, TYPES::F64 v)
{
	this->uv=VECTOR2D(u, v);
}

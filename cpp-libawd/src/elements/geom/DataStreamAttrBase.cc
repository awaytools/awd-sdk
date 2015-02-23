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



GeomStreamElementBase::GeomStreamElementBase()
{
	this->target_vertex = NULL;
}
GeomStreamElementBase::~GeomStreamElementBase()
{
}
int
GeomStreamElementBase::get_idx_for_sub_geo_id(TYPES::SUBGEOM_ID_STRING subgeo_id)
{
	return 0;
}

std::string&
GeomStreamElementBase::get_vertex_string()
{
	return this->intern_get_vertex_string();
}
			

void
GeomStreamElementBase::set_v_n_settings(SETTINGS::BlockSettings* v_n_settings)
{
	this->v_n_settings=v_n_settings;
}

SETTINGS::BlockSettings*
GeomStreamElementBase::get_v_n_settings()
{
	return this->v_n_settings;
}

void
GeomStreamElementBase::set_attribute(GEOM::DataStreamAttr* custom_attr)
{
	this->custom_attributes[custom_attr->get_type()]=custom_attr;
}

result
GeomStreamElementBase::get_attribute(GEOM::data_stream_attr_type stream_type,GEOM::DataStreamAttr** output_attr)
{
	if(this->custom_attributes.find(stream_type)==this->custom_attributes.end()){
		return result::AWD_ERROR;
	}
	*output_attr=this->custom_attributes[stream_type];
	return result::AWD_SUCCESS;
}

void
GeomStreamElementBase::set_attribute_channel(const std::string& channel_id, GEOM::DataStreamAttr* custom_attr)
{
	if(this->custom_attributes_channels.find(custom_attr->get_type())==this->custom_attributes_channels.end()){
		std::map< std::string, GEOM::DataStreamAttr* > new_map;
		this->custom_attributes_channels[custom_attr->get_type()]=new_map;
	}
	this->custom_attributes_channels[custom_attr->get_type()][channel_id] = custom_attr;
}
result
GeomStreamElementBase::get_attribute_channel(const std::string& channel_id, GEOM::data_stream_attr_type stream_type, GEOM::DataStreamAttr** output_attr)
{
	if(this->custom_attributes_channels.find(stream_type)==this->custom_attributes_channels.end()){
		return result::AWD_ERROR;
	}
	if(this->custom_attributes_channels[stream_type].find(channel_id)==this->custom_attributes_channels[stream_type].end()){
		return result::AWD_ERROR;
	}
	*output_attr=this->custom_attributes_channels[stream_type][channel_id];
	return result::AWD_SUCCESS;
}

GEOM::VECTOR4D
GeomStreamElementBase::get_color()
{
	return this->color;
}
void
GeomStreamElementBase::set_color(GEOM::VECTOR4D color)
{
	this->color=color;
}

GEOM::VECTOR3D
GeomStreamElementBase::get_face_normal()
{
	return this->face_normal;
}
void
GeomStreamElementBase::set_face_normal(GEOM::VECTOR3D face_normal)
{
	this->face_normal=face_normal;
}
GeomStreamElementBase*
GeomStreamElementBase::get_target_vertex()
{
	return this->target_vertex;
}
void
GeomStreamElementBase::set_target_vertex(GeomStreamElementBase* target_vertex)
{
	this->target_vertex=target_vertex;
}

TYPES::UINT32
GeomStreamElementBase::get_internal_idx()
{
	return this->internal_index;
}
void
GeomStreamElementBase::set_internal_idx(TYPES::UINT32 internal_index)
{
	this->internal_index=internal_index;
}

TYPES::UINT32
GeomStreamElementBase::get_original_idx()
{
	return this->original_index;
}
void
GeomStreamElementBase::set_original_idx(TYPES::UINT32 original_index)
{
	this->original_index=original_index;
}


std::string&
GeomStreamElementBase::get_face_group_id()
{
	return this->output_matface_id;
}
void
GeomStreamElementBase::set_face_group_id(const std::string& output_matface_id)
{
	this->output_matface_id=output_matface_id;
}		
				 

std::string&
GeomStreamElementBase::get_subgeom_id()
{
	return this->output_subgeom_id;
}
void
GeomStreamElementBase::set_subgeom_id(const std::string& output_subgeom_id)
{
	this->output_subgeom_id=output_subgeom_id;
}		



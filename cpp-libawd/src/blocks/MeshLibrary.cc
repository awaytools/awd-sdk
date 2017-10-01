#include "blocks/mesh_library.h"

#include "base/block.h"

#include "blocks/geometry.h"
#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"
#include "utils/awd_properties.h"


using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;
using namespace AWD::ATTR;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

MeshLibrary::MeshLibrary(std::string& name, AWDBlock *geom) :
	AWDBlock(BLOCK::MESH_INSTANCE_2, name),
	AttrElementBase()
{
	this->set_geom(geom);
	this->defaultMat = NULL;
	
	/*
	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGPOINTX,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGPOINTY,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGSCALEX,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGSCALEY,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	*/
}

MeshLibrary::MeshLibrary():
	AWDBlock(BLOCK::MESH_INSTANCE_2, ""),
	AttrElementBase()
{
	this->geom=NULL;
	this->defaultMat = NULL;
	
	/*
	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGPOINTX,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGPOINTY,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGSCALEX,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGSCALEY,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		*/
}
MeshLibrary::MeshLibrary(std::string& name, AWDBlock *geom, TYPES::F64* mtx) :
	AWDBlock(BLOCK::MESH_INSTANCE_2, name),
	AttrElementBase()
{
	this->set_geom(geom);
	this->defaultMat = NULL;
	
	/*
	TYPES::union_ptr default_union;
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=0.0;
	this->properties->add(PROP_SPRITE_REGPOINTX,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=0.0;
	this->properties->add(PROP_SPRITE_REGPOINTY,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGSCALEX,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(sizeof(TYPES::F64));
	*default_union.F64=1.0;
	this->properties->add(PROP_SPRITE_REGSCALEY,	default_union, 4,   data_types::FLOAT64, storage_precision_category::PROPERIES_VALUES, property_storage_type::STATIC_PROPERTY);
		*/
}

MeshLibrary::~MeshLibrary()
{
}


TYPES::state MeshLibrary::validate_block_state()
{
	return TYPES::state::VALID;
}


AWDBlock *
MeshLibrary::get_geom()
{
	return this->geom;
}

void
MeshLibrary::set_geom(AWDBlock *geom)
{
	this->geom = geom;
}

TYPES::UINT32
MeshLibrary::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	TYPES::UINT32 length = sizeof(TYPES::UINT16) + this->get_name().size();//name
	length += sizeof(BADDR);//geom address
	length += sizeof(TYPES::UINT16);//num submeshes
	length += TYPES::UINT32(this->materials.size() * sizeof(BADDR));//material addresses

	length += sizeof(TYPES::UINT16);//length of suacctual subgeoms
	BLOCKS::Geometry* thisGeom = reinterpret_cast<BLOCKS::Geometry*> (this->geom);
	length += thisGeom->get_uv_bytesize(awd_file, settings);
	
	/*
	if (thisGeom != NULL){
		TYPES::union_ptr properties_union;
		properties_union.v=malloc(sizeof(TYPES::F64));
		*properties_union.F64=thisGeom->uint16_offsetX;
		this->properties->set(PROP_SPRITE_REGPOINTX, properties_union, TYPES::UINT32(sizeof(TYPES::F32)));

		properties_union.v=malloc(sizeof(TYPES::F64));
		*properties_union.F64=thisGeom->uint16_offsetY;
		this->properties->set(PROP_SPRITE_REGPOINTY, properties_union, TYPES::UINT32(sizeof(TYPES::F32)));

		properties_union.v=malloc(sizeof(TYPES::F64));
		*properties_union.F64=thisGeom->uint16_scaleX;
		this->properties->set(PROP_SPRITE_REGSCALEX, properties_union, TYPES::UINT32(sizeof(TYPES::F32)));

		properties_union.v=malloc(sizeof(TYPES::F64));
		*properties_union.F64=thisGeom->uint16_scaleY;
		this->properties->set(PROP_SPRITE_REGSCALEY, properties_union, TYPES::UINT32(sizeof(TYPES::F32)));
	}
	*/
	
	length += this->calc_attr_length(true, true, settings);
	return length;
}

result
MeshLibrary::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	BLOCKS::Geometry* thisGeom = reinterpret_cast<BLOCKS::Geometry*> (this->geom);
	this->materials.clear();
	thisGeom->get_material_blocks(this->materials);
	if (this->geom != NULL)
		this->geom->add_with_dependencies(target_file, instance_type);
	for(AWDBlock* mat : this->materials)
		mat->add_with_dependencies(target_file, instance_type);
	
	return result::AWD_SUCCESS;
}

void
MeshLibrary::createSlice9Scale(TYPES::F32 x, TYPES::F32 y, TYPES::F32 width, TYPES::F32 height)
{
	BLOCKS::Geometry* thisGeom = reinterpret_cast<BLOCKS::Geometry*> (this->geom);
	thisGeom->createSlice9Scale(x, y, width, height);
}

result
MeshLibrary::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{
	
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	TYPES::UINT32 block_addr=0;
	if (this->geom != NULL)
		this->geom->get_block_addr(file, block_addr);
	fileWriter->writeUINT32(block_addr);
	
	fileWriter->writeUINT16(this->materials.size());
		
	for(AWDBlock * mat : this->materials){
		TYPES::UINT32 block_addr_mat=0;
		if (mat != NULL)
			mat->get_block_addr(file, block_addr_mat);
		fileWriter->writeUINT32(block_addr_mat);
	}
	
	BLOCKS::Geometry* thisGeom = reinterpret_cast<BLOCKS::Geometry*> (this->geom);
	thisGeom->write_uvs(fileWriter, settings, file);

	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	
	return result::AWD_SUCCESS;
}

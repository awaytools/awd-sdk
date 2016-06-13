#include "blocks/mesh_library.h"

#include "base/block.h"

#include "blocks/geometry.h"
#include "base/attr.h"
#include "base/state_element_base.h"
#include "base/scene_base.h"


using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

MeshLibrary::MeshLibrary(std::string& name, AWDBlock *geom) :
	AWDBlock(BLOCK::MESH_INSTANCE_2, name),
	AttrElementBase()
{
	this->set_geom(geom);
	this->defaultMat = NULL;
}

MeshLibrary::MeshLibrary():
	AWDBlock(BLOCK::MESH_INSTANCE_2, ""),
	AttrElementBase()
{
	this->geom=NULL;
	this->defaultMat = NULL;
}
MeshLibrary::MeshLibrary(std::string& name, AWDBlock *geom, TYPES::F64* mtx) :
	AWDBlock(BLOCK::MESH_INSTANCE_2, name),
	AttrElementBase()
{
	this->set_geom(geom);
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
	length += this->calc_attr_length(true, true, settings);
	return length;
}

result
MeshLibrary::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	BLOCKS::Geometry* thisGeom = reinterpret_cast<BLOCKS::Geometry*> (this->geom);
	thisGeom->get_material_blocks(this->materials);
	if (this->geom != NULL)
		this->geom->add_with_dependencies(target_file, instance_type);
	for(AWDBlock* mat : this->materials)
		mat->add_with_dependencies(target_file, instance_type);
	
	return result::AWD_SUCCESS;
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

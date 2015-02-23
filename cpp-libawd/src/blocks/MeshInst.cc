#include "blocks/mesh_inst.h"

#include "base/block.h"

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

Mesh::Mesh(std::string& name, AWDBlock *geom) :
	SceneBlockBase(block_type::MESH_INSTANCE, name, NULL)
{
	this->set_geom(geom);
	this->defaultMat = NULL;
}

Mesh::Mesh():
	SceneBlockBase(block_type::MESH_INSTANCE, "", NULL)
{
	this->geom=NULL;
	this->defaultMat = NULL;
}
Mesh::Mesh(std::string& name, AWDBlock *geom, TYPES::F64* mtx) :
	SceneBlockBase(block_type::MESH_INSTANCE, name, mtx)
{
	this->set_geom(geom);
	this->lightPicker = NULL;
	this->defaultMat = NULL;
	this->animator = NULL;
}

Mesh::~Mesh()
{
	this->lightPicker = NULL;
	this->defaultMat = NULL;
	this->geom = NULL;
}


TYPES::state Mesh::validate_block_state()
{
	return TYPES::state::VALID;
}

BASE::AWDBlock* 
Mesh::get_material_by_face_id(const TYPES::SUBGEOM_ID_STRING& face_group)
{
	if(this->available_materials.find(face_group)==this->available_materials.end())
	{
		return NULL;
		// facegroup doesnt exist on mesh. return default-facegroup.
	}
	return this->available_materials[face_group];
}
void
Mesh::get_merged_face_id(const TYPES::SUBGEOM_ID_STRING& face_group, TYPES::SUBGEOM_ID_STRING& face_group_out)
{
	
	if(this->available_materials.find(face_group)==this->available_materials.end())
	{
		// facegroup doesnt exist on mesh. return default-facegroup.
	}
	AWDBlock* this_mat = this->available_materials[face_group];
	if(this_mat==NULL){
		face_group_out="";
		return; //Should never happen
	}
	typedef std::map<TYPES::SUBGEOM_ID_STRING, BASE::AWDBlock*>::iterator mat_map_iterator;
	for(mat_map_iterator iterator = this->available_materials.begin(); iterator != this->available_materials.end(); iterator++) {
		if(iterator->second == this_mat){
			face_group_out = iterator->first;
			return;
		}
	}
}
void 
Mesh::add_final_material(BASE::AWDBlock * final_mat)
{
	this->materials.push_back(final_mat);
}

AWDBlock *
Mesh::get_defaultMat()
{
	return this->defaultMat;
}
void
Mesh::set_defaultMat(AWDBlock *defaultMat)
{
	this->defaultMat=defaultMat;
}

AWDBlock *
Mesh::get_lightPicker()
{
	return this->lightPicker;
}
void
Mesh::set_lightPicker(AWDBlock *lightPicker)
{
	this->lightPicker=lightPicker;
}

AWDBlock *
Mesh::get_geom()
{
	return this->geom;
}

void
Mesh::set_geom(AWDBlock *geom)
{
	this->geom = geom;
}

AWDBlock *
Mesh::get_animator()
{
	return this->animator;
}
void
Mesh::set_animator(AWDBlock *animator)
{
	this->animator = animator;
}
TYPES::UINT32
Mesh::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/
	TYPES::UINT32 length = calc_common_length(settings->get_wide_matrix());
	length += sizeof(BADDR);//geom address
	length += sizeof(TYPES::UINT16);//num submeshes
	length += TYPES::UINT32(this->materials.size() * sizeof(BADDR));//material addresses
	length += this->calc_attr_length(true, true, settings);
	return length;
}

result
Mesh::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	/*
	if (this->geom != NULL)
		this->geom->add_with_dependencies(target_file, instance_type);
	Material *block;
	AWDBlockIterator *it;
	it = new AWDBlockIterator(this->materials);
	// check if all materials(submehses) are using same uv-transform
	TYPES::F64 * uv_trans = NULL;
	bool hasSubMeshUVTransform = false;
	while ((block = (Material *)it->next()) != NULL) {
		if (uv_trans==NULL){
			if (block->get_uv_transform_mtx()!=NULL){
				uv_trans=block->get_uv_transform_mtx();
			}
		}
		else {
			if(!hasSubMeshUVTransform){
				if (block->get_uv_transform_mtx()!=NULL){
					TYPES::F64 * this_uv=block->get_uv_transform_mtx();
					if ((this_uv[0]!=uv_trans[0])||(this_uv[1]!=uv_trans[1])||(this_uv[2]!=uv_trans[2])||(this_uv[3]!=uv_trans[3])||(this_uv[4]!=uv_trans[4])||(this_uv[5]!=uv_trans[5])){
						hasSubMeshUVTransform=true;
					}
				}
			}
		}
		block->add_with_dependencies(target_file, instance_type);
	}
	// if all materials(submehses)use same uv)
	if (uv_trans!=NULL){
		if (!hasSubMeshUVTransform){
			if ((uv_trans[0]!=1.0)||(uv_trans[1]!=0.0)||(uv_trans[2]!=1.0)||(uv_trans[3]!=0.0)||(uv_trans[4]!=0.0)||(uv_trans[5]!=0.0)){
				union_ptr newVal;
				newVal.v = malloc(sizeof(TYPES::F64)*6);
				newVal.F64[0] = uv_trans[0];
				newVal.F64[1] = uv_trans[1];
				newVal.F64[2] = uv_trans[2];
				newVal.F64[3] = uv_trans[3];
				newVal.F64[4] = uv_trans[4];
				newVal.F64[5] = uv_trans[5];
				this->properties->set(6, newVal, sizeof(TYPES::F64)*6, FLOAT64);
			}
		}
		else{
			union_ptr newVal;
			newVal.v = malloc(sizeof(TYPES::F64)*6*this->materials->get_num_blocks());
			it->reset();
			int uvTransCnt=0;
			while ((block = (Material *)it->next()) != NULL) {
				TYPES::F64 * this_uv=block->get_uv_transform_mtx();
				if (this_uv!=NULL){
					newVal.F64[uvTransCnt++] = uv_trans[0];
					newVal.F64[uvTransCnt++] = uv_trans[1];
					newVal.F64[uvTransCnt++] = uv_trans[2];
					newVal.F64[uvTransCnt++] = uv_trans[3];
					newVal.F64[uvTransCnt++] = uv_trans[4];
					newVal.F64[uvTransCnt++] = uv_trans[5];
				}
				else {
					newVal.F64[uvTransCnt++] = 1.0;
					newVal.F64[uvTransCnt++] = 0.0;
					newVal.F64[uvTransCnt++] = 0.0;
					newVal.F64[uvTransCnt++] = 1.0;
					newVal.F64[uvTransCnt++] = 0.0;
					newVal.F64[uvTransCnt++] = 0.0;
				}
			}
			this->properties->set(7, newVal, sizeof(TYPES::F64)*6*this->materials->get_num_blocks(), FLOAT64);
		}
	}
	delete it;
	*/
	return result::AWD_SUCCESS;
}

result
Mesh::write_body(FileWriter * fileWriter, BlockSettings *settings,AWDFile* file)
{
	/*
	AWDBlock *block;
	AWDBlockIterator *it;
	BADDR geom_addr;
	TYPES::UINT16 num_materials;

	this->write_scene_common(fileWriter, settings);

	// Write mesh geom address (can be NULL)
	geom_addr = 0;
	if (this->geom != NULL)
		geom_addr = this->geom->get_addr();
	fileWriter->writeUINT32(geom_addr);

	// Write materials list. First write material count, and then
	// iterate over materials block list and write all addresses
	//printf("material count: %d\n", this->materials->get_num_blocks());
	num_materials = (TYPES::UINT16)this->materials->get_num_blocks();
	fileWriter->writeUINT16(num_materials);
	it = new AWDBlockIterator(this->materials);
	while ((block = it->next()) != NULL) {
		BADDR addr = block->get_addr();
		fileWriter->writeUINT32(addr);
	}
	delete it;
	this->properties->write_attributes(fileWriter, settings);
	this->user_attributes->write_attributes(fileWriter, settings);
	*/
	return result::AWD_SUCCESS;
}

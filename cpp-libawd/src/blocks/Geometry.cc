#include "blocks/geometry.h"

#include "utils/awd_types.h"
#include "utils/util.h"
#include "base/block.h"

#include "base/attr.h"
#include "base/state_element_base.h"
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;

using namespace AWD::SETTINGS;
using namespace AWD::GEOM;

Geometry::Geometry(std::string& name) :
	BASE::AWDBlock(BLOCK::block_type::TRI_GEOM, name),
	BASE::AttrElementBase()
{		
	this->delete_subs=true;
	this->subGeomSettings=new BlockSettings(true);
	this->bind_mtx = NULL;
	this->num_subs = 0;
	this->is_created = false;
	this->split_faces = false;
	this->originalPointCnt = 0;
	this->mesh_instance=NULL;
}

Geometry::Geometry():
	AWDBlock(BLOCK::block_type::TRI_GEOM),
	AttrElementBase()
{
	this->delete_subs=true;
	this->subGeomSettings=new BlockSettings(true);
	this->bind_mtx = NULL;
	this->num_subs = 0;
	this->is_created = false;
	this->split_faces = false;
	this->originalPointCnt = 0;
	this->mesh_instance=NULL;
}

Geometry::Geometry(bool delete_subs):
	AWDBlock(BLOCK::block_type::TRI_GEOM),
	AttrElementBase()
{
	this->delete_subs=delete_subs;
	this->subGeomSettings=new BlockSettings(true);
	this->bind_mtx = NULL;
	this->num_subs = 0;
	this->is_created = false;
	this->split_faces = false;
	this->originalPointCnt = 0;
	this->mesh_instance=NULL;
}
Geometry::~Geometry()
{
    
	for (GEOM::FilledRegion* filled_region: all_filled_regions)
		delete filled_region;
	if(delete_subs){
		for(SubGeom * subGeom: this->subGeometries){
			delete subGeom;
		}
	}
	if (this->bind_mtx) {
		free(this->bind_mtx);
		this->bind_mtx = NULL;
	}
}

void 
	Geometry::add_res_id_geom(const std::string& new_res, GEOM::MATRIX2x3* new_matrix){
	if(has_res_id(new_res))
		return;
	this->ressource_ids.push_back(new_res);
	this->frame_cmd_inst_matrices.push_back(new_matrix);
}

GEOM::MATRIX2x3* 
Geometry::has_res_id_geom(const std::string& new_res){
	int cnt=0;
	for(std::string id:this->ressource_ids){
		if(id==new_res)
			return this->frame_cmd_inst_matrices[cnt];
		cnt++;
	}
	return NULL;
}

void 
Geometry::clear_res_ids_geom()
{	
	this->frame_cmd_inst_matrices.clear();
	this->ressource_ids.clear();
}

result
Geometry::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	return result::AWD_SUCCESS;
}


TYPES::state Geometry::validate_block_state()
{
	return TYPES::state::VALID;
}

/*
std::vector<Mesh*>
Geometry::get_mesh_instance_list()
{
	return this->meshInstances;
}
void
Geometry::set_mesh_instance_list(std::vector<Mesh*> meshInstances)
{
	this->meshInstances=meshInstances;
}
*/

void
Geometry::set_mesh_instance(AWDBlock* mesh_instance)
{
	this->mesh_instance=mesh_instance;
}

AWDBlock*
Geometry::get_mesh_instance()
{
	return this->mesh_instance;
}
void
Geometry::add_mesh_inst(AWDBlock* mesh_inst)
{
	this->meshInstances.push_back(mesh_inst);
}

void
Geometry::clear_mesh_inst_list()
{
	this->meshInstances.clear();
}

void
Geometry::add_subgeo(SubGeom *sub)
{
	this->subGeometries.push_back(sub);
}

unsigned int
Geometry::get_num_subs()
{
	unsigned int subgeo_cnt=0;
	for(SubGeom* subgeo: this->subGeometries){
		subgeo_cnt+=subgeo->get_num_subs();
	}
	return subgeo_cnt;
}

result 
Geometry::add_filled_region(GEOM::FilledRegion* filled_region)
{
	this->all_filled_regions.push_back(filled_region);
	return result::AWD_SUCCESS;
}
	
result 
Geometry::add_vertex3D(Vertex3D*  vert)
{
	/** optionally check if their exists any vertex that shares the same position.	*/
	TYPES::UINT32 original_idx=vert->get_original_idx();
	if(true){		
		VECTOR3D position = vert->get_position();
		std::string lookup_string = FILES::num_to_string(position.x)+"/"+FILES::num_to_string(position.y)+"/"+FILES::num_to_string(position.z);
		if(this->shared_verts_idx_map.find(lookup_string)==shared_verts_idx_map.end())
			shared_verts_idx_map[lookup_string]=original_idx;
		else
			original_idx=shared_verts_idx_map[lookup_string];
		
	}
	// set internal_idx. This is in case the vertex-indicies have changed because of checking for shared positions.
	// we need to keep the original_idx for creating vertex-animations-poses, hence we need the internal_idx
	vert->set_internal_idx(original_idx);

	while(this->shared_verts.size()<=original_idx)
		this->shared_verts.push_back(new SharedVertex3D());
	this->shared_verts[original_idx]->add_vert(vert);
	this->awdVertices.push_back(vert);
	return result::AWD_SUCCESS;
}

Geometry*
Geometry::get_cloned_geometry()
{
	Geometry* new_geo=new Geometry();

	// clone neccessary data
	/*
	while(this->shared_verts.size()<=original_idx)
		this->shared_verts.push_back(new SharedVertex3D());
		*/
	return new_geo;

}

result
Geometry::add_triangle(GEOM::Triangle* triangle)
{
	if(triangle==NULL)
		return result::AWD_ERROR;

	Vertex3D* v1 = triangle->get_v1();
	if(v1==NULL)
		return result::AWD_ERROR;
	Vertex3D* v2 = triangle->get_v2();
	if(v2==NULL)
		return result::AWD_ERROR;
	Vertex3D* v3 = triangle->get_v3();
	if(v3==NULL)
		return result::AWD_ERROR;
	return result::AWD_SUCCESS;
}

SubGeom *
Geometry::get_sub_at(unsigned int idx)
{
	if (idx < this->subGeometries.size()) {
		return this->subGeometries[idx];
	}

	return NULL;
}

TYPES::F64 *
Geometry::get_bind_mtx()
{
	return this->bind_mtx;
}

void
Geometry::set_bind_mtx(TYPES::F64 *bind_mtx)
{
	this->bind_mtx = bind_mtx;
}

std::vector<BASE::AWDBlock*>& 
Geometry::get_mesh_instances()
{
	return this->meshInstances;
}

void 
Geometry::set_mesh_instances(std::vector<BASE::AWDBlock*>& meshInstances)
{
	this->meshInstances = meshInstances;
}
				
std::vector<GEOM::FilledRegion*>& 
Geometry::get_filled_regions()
{
	return this->all_filled_regions;
}

std::vector<GEOM::Triangle*>& 
Geometry::get_all_triangles()
{
	return this->awdTriangles;
}

void 
Geometry::set_all_triangles(std::vector<GEOM::Triangle*>& awdTriangles)
{
	this->awdTriangles=awdTriangles;
}

std::vector<GEOM::Vertex3D*>& 
Geometry::get_all_verts()
{
	return this->awdVertices;
}

void 
Geometry::get_material_blocks(std::vector<BASE::AWDBlock*>& material_list)
{
	for(SubGeom* subgeo: this->subGeometries)
		subgeo->get_material_blocks(material_list);
}

SETTINGS::BlockSettings* 
Geometry::get_sub_geom_settings()
{
	return this->subGeomSettings;	
}
BASE::AWDBlock* 
Geometry::get_first_mesh()
{
	if(this->meshInstances.size()==0)
		return NULL;
	return this->meshInstances[0];	
}

void 
Geometry::set_all_verts(std::vector<GEOM::Vertex3D*>& awdVertices)
{
	this->awdVertices = awdVertices;
}

std::vector<GEOM::SharedVertex3D*>& 
Geometry::get_shared_verts()
{
	return this->shared_verts;
}

bool
Geometry::get_is_created()
{
	return this->is_created;
}

void
Geometry::set_is_created(bool is_created)
{
	this->is_created=is_created;
}

bool
Geometry::get_split_faces()
{
	return this->split_faces;
}

void
Geometry::set_split_faces(bool split_faces)
{
	this->split_faces=split_faces;
}

int
Geometry::get_originalPointCnt()
{
	return this->originalPointCnt;
}

void
Geometry::set_originalPointCnt(int newPointCnt)
{
	this->originalPointCnt = newPointCnt;
}

TYPES::UINT32
Geometry::calc_body_length(AWDFile* awd_file, BlockSettings * settings)
{
	/*if(!this->get_isValid())
		return 0;*/

	TYPES::UINT32 mesh_len = TYPES::UINT32(sizeof(TYPES::UINT16)); // Num subs
	mesh_len += TYPES::UINT32(sizeof(TYPES::UINT16) + this->get_name().size());
	mesh_len += this->calc_attr_length(true,true,  settings);
	
	
	for(SubGeom * subGeom: this->subGeometries){
		mesh_len += subGeom->calc_sub_length();
	}
	

	return mesh_len;
}
result 
Geometry::merge_subgeos()
{
	if(this->subGeometries.size()==0){
		return result::AWD_ERROR;
	}
	std::vector<SubGeom* > new_subgeos;
	new_subgeos.push_back(this->subGeometries.back());
	for(SubGeom * subGeom: this->subGeometries){
		if(subGeom!=this->subGeometries.back()){
			bool found = false;
			for(SubGeom * new_subgeo: new_subgeos){
				if(new_subgeo->merge_subgeo(subGeom)==result::AWD_SUCCESS){
					found=true;
					break;
				}
			}
			if(!found){
				new_subgeos.push_back(subGeom);
			}
		}
	}
	this->subGeometries.clear();
	for(SubGeom * subGeom: new_subgeos){
		this->subGeometries.push_back(subGeom);
	}
	return result::AWD_SUCCESS;
}

result 
Geometry::write_uvs(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * settings, FILES::AWDFile* awd_file)
{
	TYPES::UINT16 num_subs=0;
	for(SubGeom * subGeom: this->subGeometries){
		num_subs += subGeom->get_num_subs();
	}
	fileWriter->writeUINT16(num_subs);
	for(SubGeom * subGeom: this->subGeometries){

		// color  = tx / ty
		// linear = a / c / tx / ty / ?spread
		// radial = a / b / c / d / tx / ty / ?spread / ?focalPoint
		
		if(fileWriter->writeUINT8(TYPES::UINT8(subGeom->mat_type))!=result::AWD_SUCCESS)
			return result::AWD_ERROR;

		if(subGeom->mat_type==MATERIAL::type::SOLID_COLOR_MATERIAL){
			fileWriter->writeFLOAT32(subGeom->uv_tl->x);
			fileWriter->writeFLOAT32(subGeom->uv_tl->y);
		}
		else if(subGeom->mat_type==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL){
			TYPES::F64* uv_transform = subGeom->uv_transform->get();
			fileWriter->writeFLOAT32(uv_transform[0]);
			fileWriter->writeFLOAT32(uv_transform[2]);
			fileWriter->writeFLOAT32(uv_transform[4]);
			fileWriter->writeFLOAT32(uv_transform[5]);
		}
		else if(subGeom->mat_type==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
			fileWriter->writeFLOAT32(subGeom->uv_tl->x);
			fileWriter->writeFLOAT32(subGeom->uv_tl->y);
			fileWriter->writeFLOAT32(subGeom->uv_br->x);
			fileWriter->writeFLOAT32(subGeom->uv_br->y);
			subGeom->uv_transform->write_to_file(fileWriter, settings);
		}
		else if(subGeom->mat_type==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
			subGeom->uv_transform->write_to_file(fileWriter, settings);
		}
		// todo: optional subgeom properties ()
		fileWriter->writeUINT32(0);

	}

	return result::AWD_SUCCESS;
}

TYPES::UINT32  
Geometry::get_uv_bytesize(FILES::AWDFile*, SETTINGS::BlockSettings *)
{
	TYPES::UINT32 uv_bytesize=0;
	for(SubGeom * subGeom: this->subGeometries){
		uv_bytesize += sizeof(TYPES::UINT8); // type

		if(subGeom->mat_type==MATERIAL::type::SOLID_COLOR_MATERIAL){
			uv_bytesize += 2*sizeof(TYPES::F32);
		}
		else if(subGeom->mat_type==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL){
			uv_bytesize += 4*sizeof(TYPES::F32);
		}
		else if(subGeom->mat_type==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
			uv_bytesize += 10*sizeof(TYPES::F32);
		}
		else if(subGeom->mat_type==MATERIAL::type::SOLID_TEXTUREATLAS_MATERIAL){
			uv_bytesize += 6*sizeof(TYPES::F32);
		}
		// todo: optional subgeom properties
		uv_bytesize += sizeof(TYPES::UINT32);
	}
	return uv_bytesize;
}
result 
Geometry::merge_stream(GEOM::SubGeom* target_sub){	
	if(this->subGeometries.size()==0){
		return result::AWD_ERROR;
	}
	for(SubGeom * subGeom: this->subGeometries){
		subGeom->merge_stream(target_sub);
    }
    return result::AWD_SUCCESS;
}

result 
Geometry::merge_for_textureatlas(BLOCKS::Material* thisMat, BLOCKS::Material* thisMat_alpha, BLOCKS::Material* radial_mat,  BLOCKS::Material* radial_mat_alpha)
{
	if(this->subGeometries.size()==0){
		return result::AWD_ERROR;
	}
	for(SubGeom * subGeom: this->subGeometries){
		subGeom->set_uvs();
		BLOCKS::Material* subgeom_mat = reinterpret_cast<BLOCKS::Material*>(subGeom->material_block);
		if(subgeom_mat!=NULL){
			BLOCKS::Material* merged_mat=NULL;
			if(subgeom_mat->get_material_type()==MATERIAL::type::SOLID_COLOR_MATERIAL){
				if(subgeom_mat->needsAlphaTex)
					merged_mat=thisMat_alpha;
				else
					merged_mat=thisMat;
			}
			else if(subgeom_mat->get_material_type()==MATERIAL::type::LINEAR_GRADIENT_TEXTUREATLAS_MATERIAL){
				if(subgeom_mat->needsAlphaTex)
					merged_mat=thisMat_alpha;
				else
					merged_mat=thisMat;
			}
			else if(subgeom_mat->get_material_type()==MATERIAL::type::RADIAL_GRADIENT_TEXTUREATLAS_MATERIAL){
				if(subgeom_mat->needsAlphaTex)
					merged_mat=radial_mat_alpha;
				else
					merged_mat=radial_mat;
			}
			if(merged_mat!=NULL){
				subGeom->material_block=merged_mat;
			}
		}
	}

	
	std::vector<SubGeom* > new_subgeos;
	new_subgeos.push_back(this->subGeometries.back());
	for(SubGeom * subGeom: this->subGeometries){
		if(subGeom!=this->subGeometries.back()){
			bool found = false;
			for(SubGeom * new_subgeo: new_subgeos){
				if(new_subgeo->merge_subgeo(subGeom)==result::AWD_SUCCESS){
					found=true;
					break;
				}
			}
			if(!found){
				new_subgeos.push_back(subGeom);
			}
		}
	}
	this->subGeometries.clear();
	for(SubGeom * subGeom: new_subgeos){
		this->subGeometries.push_back(subGeom);
	}
	
	return result::AWD_SUCCESS;
}

int Geometry::subgeomcnt=0;
void Geometry::set_addr_on_subgeom(FILES::AWDFile* file){
	TYPES::UINT32 adress;
	this->get_block_addr(file, adress);
	this->subGeometries[0]->merged_address = adress;
}
result
Geometry::write_body(FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
	// Write name and sub count
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	TYPES::UINT16 num_subs=0;
	TYPES::UINT16 num_subs_simple=0;
	for(SubGeom * subGeom: this->subGeometries){
		num_subs += subGeom->get_num_subs();
		num_subs_simple+=1;
	}
	if(num_subs_simple!=num_subs){
	}
	BLOCKS::Geometry::subgeomcnt+=num_subs;
	fileWriter->writeUINT16(num_subs);

	// Write list of optional properties
	this->properties->write_attributes(fileWriter, settings);

	// Write all sub-meshes
	for(SubGeom * subGeom: this->subGeometries){
		subGeom->write_sub(fileWriter);
	}

	// Write list of user attributes
	this->user_attributes->write_attributes(fileWriter, settings);
	
	return result::AWD_SUCCESS;
}

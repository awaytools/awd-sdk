#include "blocks/geometry.h"

#include "utils/awd_types.h"
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
	this->subGeomSettings=new BlockSettings(true);
	this->bind_mtx = NULL;
	this->num_subs = 0;
	this->is_created = false;
	this->split_faces = false;
	this->originalPointCnt = 0;
}

Geometry::Geometry():
	AWDBlock(BLOCK::block_type::TRI_GEOM),
	AttrElementBase()
{
	this->subGeomSettings=new BlockSettings(true);
	this->bind_mtx = NULL;
	this->num_subs = 0;
	this->is_created = false;
	this->split_faces = false;
	this->originalPointCnt = 0;
}

Geometry::~Geometry()
{
    
	for(SubGeom * subGeom: this->subGeometries){
		delete subGeom;
	}
	if (this->bind_mtx) {
		free(this->bind_mtx);
		this->bind_mtx = NULL;
	}
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
Geometry::add_path_shape(GEOM::FilledRegion* path_shape)
{
	this->all_path_shapes.push_back(path_shape);
	return result::AWD_SUCCESS;
}
	
result 
Geometry::add_vertex3D(Vertex3D*  vert)
{
	/** optionally check if their exists any vertex that shares the same position.	*/
	TYPES::UINT32 original_idx=vert->get_original_idx();
	if(true){		
		VECTOR3D position = vert->get_position();
		std::string lookup_string = std::to_string(position.x)+"/"+std::to_string(position.y)+"/"+std::to_string(position.z);
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
	if (idx < this->num_subs) {
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
	return this->all_path_shapes;
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
Geometry::merge_for_textureatlas(BLOCKS::Material* this_material)
{
	if(this->subGeometries.size()==0){
		return result::AWD_ERROR;
	}
	for(SubGeom * subGeom: this->subGeometries){
		subGeom->set_uvs();
		BLOCKS::Material* subgeom_mat = reinterpret_cast<BLOCKS::Material*>(subGeom->material_block);
		if(this_material->get_texture()==subgeom_mat->get_texture()){
			subGeom->material_block=this_material;
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
result
Geometry::write_body(FileWriter * fileWriter, BlockSettings *settings, FILES::AWDFile* file)
{
	
	// Write name and sub count
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
	TYPES::UINT16 num_subs=0;
	for(SubGeom * subGeom: this->subGeometries){
		num_subs += subGeom->get_num_subs();
	}
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

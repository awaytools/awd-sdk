#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm> 
#include "utils/awd_types.h"
#include "blocks/geometry.h"
#include "blocks/material.h"
#include "elements/subgeometry.h"
#include "elements/geom_elements.h"
#include "blocks/mesh_inst.h"
#include "utils/util.h"
#include "utils/settings.h"
#include <bitset>
#include "tesselator.h"

using namespace AWD;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::SETTINGS;

#include "Stdafx.h"


using namespace IceCore;
#define SORTER	RadixSort

result 
GEOM::write_stream_attr_to_file(FILES::FileWriter* file_writer, GeomStreamElementBase* stream_attr, DataStreamAttrDesc* attr_desc)
{
	DataStreamAttr* in_attr;
	stream_attr->get_attribute(attr_desc->get_type(), &in_attr);
	TYPES::union_ptr in_ptr = in_attr->get_data();
	// \todo storage precision and scale for this attribute.
	file_writer->writeUnion(in_ptr, attr_desc->get_data_type(), attr_desc->get_data_length(), false, false, 1.0);
	return result::AWD_SUCCESS;
}

result
GEOM::write_triangle_attr_to_file(FILES::FileWriter* file_writer, Triangle* tri, DataStreamAttrDesc* attr_desc)
{
	
	if(attr_desc->get_type()==GEOM::data_stream_attr_type::VERTEX_INDICIES){		
		file_writer->writeUINT16(tri->get_output_idx_1());
		file_writer->writeUINT16(tri->get_output_idx_2());
		file_writer->writeUINT16(tri->get_output_idx_3());
		return result::AWD_SUCCESS;
	}
	return write_stream_attr_to_file(file_writer, tri, attr_desc);

}

result
GEOM::write_vertex_attr_to_file(FILES::FileWriter* file_writer,  Vertex2D* vert, DataStreamAttrDesc* attr_desc)
{
	if(attr_desc->get_type()==GEOM::data_stream_attr_type::POSITION2D){
		file_writer->writeFLOAT32(vert->get_position().x);
		file_writer->writeFLOAT32(vert->get_position().y);
		return result::AWD_SUCCESS;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::POSITION2D_UINT16){
		file_writer->writeUINT16(vert->get_position_uint16().x);
		file_writer->writeUINT16(vert->get_position_uint16().y);
		return result::AWD_SUCCESS;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::COLOR){
		file_writer->writeFLOAT32(vert->get_color().x);
		file_writer->writeFLOAT32(vert->get_color().y);
		file_writer->writeFLOAT32(vert->get_color().z);
		file_writer->writeFLOAT32(vert->get_color().w);
		return result::AWD_SUCCESS;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::CURVE_DATA_2D_INT){
		file_writer->writeINT8(vert->get_curve_attributes().x);
		file_writer->writeINT8(vert->get_curve_attributes().y);
		file_writer->writeINT8(vert->get_curve_attributes().z);
		return result::AWD_SUCCESS;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::CURVE_DATA_2D){
		file_writer->writeFLOAT32(vert->get_curve_attributes().x);
		file_writer->writeFLOAT32(vert->get_curve_attributes().y);
		file_writer->writeFLOAT32(vert->get_curve_attributes().z);
		return result::AWD_SUCCESS;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::UV_2D){
		file_writer->writeFLOAT32(vert->get_uv().x);
		file_writer->writeFLOAT32(vert->get_uv().y);
		return result::AWD_SUCCESS;
	}
	return write_stream_attr_to_file(file_writer, vert, attr_desc);
}

TYPES::UINT32 
GEOM::append_triangle_attr_to_stream(TYPES::union_ptr output_ptr,  Triangle* tri, DataStreamAttrDesc* attr_desc, TYPES::UINT32 all_cnt)
{
	if(attr_desc->get_type()==GEOM::data_stream_attr_type::VERTEX_INDICIES){		
		output_ptr.ui16[all_cnt++] = tri->get_output_idx_1();
		output_ptr.ui16[all_cnt++] = tri->get_output_idx_2();
		output_ptr.ui16[all_cnt++] = tri->get_output_idx_3();
		return 3;
	}
	//\todo Add custom attributes for triangle streams
	//else
	//	return append_stream_attr_to_stream(output_ptr, tri, attr_desc, all_cnt);
	return 0;

}

TYPES::UINT32 
	GEOM::append_vertex_attr_to_stream(TYPES::union_ptr output_ptr,  Vertex2D* vert, DataStreamAttrDesc* attr_desc, TYPES::UINT32 all_cnt)
{
	if(attr_desc->get_type()==GEOM::data_stream_attr_type::POSITION2D){		
		output_ptr.F32[all_cnt++] = vert->get_position().x;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_position().y;
		return 2;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::POSITION2D_UINT16){		
		output_ptr.ui16[all_cnt++] = vert->get_position_uint16().x;// \todo: add scale and storage precision
		output_ptr.ui16[all_cnt++] = vert->get_position_uint16().y;
		return 2;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::COLOR){
		output_ptr.F32[all_cnt++] = vert->get_color().x;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_color().y;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_color().z;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_color().w;// \todo: add scale and storage precision	
		return 4;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::UV_2D){
		output_ptr.F32[all_cnt++] = vert->get_uv().x;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_uv().y;// \todo: add scale and storage precision
		return 2;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::CURVE_DATA_2D){
		output_ptr.F32[all_cnt++] = vert->get_curve_attributes().x;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_curve_attributes().y;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_curve_attributes().z;// \todo: add scale and storage precision	
		return 3;
	}
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::CURVE_DATA_2D_INT){
		output_ptr.F32[all_cnt++] = vert->get_curve_attributes().x;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_curve_attributes().y;// \todo: add scale and storage precision
		output_ptr.F32[all_cnt++] = vert->get_curve_attributes().z;// \todo: add scale and storage precision	
		return 3;
	}
	//else
	//	append_stream_attr_to_stream(output_ptr, vert, attr_desc, all_cnt);	
	return all_cnt;
}

result 
GEOM::append_stream_attr_to_stream(TYPES::union_ptr output_ptr,  GeomStreamElementBase* vert, DataStreamAttrDesc* attr_desc, TYPES::UINT32 all_cnt){
	///\todo: write custom stream attribute into the output_ptr
	/*DataStreamAttr* in_attr;
	stream_attr->get_attribute(attr_desc->get_type(), &in_attr);
	TYPES::union_ptr in_ptr = in_attr->get_data();
	int this_cnt=0;
	for(int i=0; i<attr_desc->get_data_length(); i++){
		switch (attr_desc->get_data_type()) {
			case TYPES::data_types::INT8:
				break;
			case TYPES::data_types::UINT8:
				break;
			case TYPES::data_types::INT16:
				break;
			case TYPES::data_types::UINT16:
				output_ptr.ui16[all_cnt++] = in_ptr.ui16[this_cnt++];
				break;
			case TYPES::data_types::INT32:
				break;
			case TYPES::data_types::UINT32:
			case TYPES::data_types::BADDR:
			case TYPES::data_types::COLOR:
				break;
			case TYPES::data_types::FLOAT32:
				break;
			case TYPES::data_types::FLOAT64:
				break;

			case TYPES::data_types::STRING:
				break;

			case TYPES::data_types::BOOL:
				break;

			case TYPES::data_types::VECTOR2x1:
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
			case TYPES::data_types::VECTOR3x1:
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
			case TYPES::data_types::VECTOR4x1:
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
				output_ptr.F32[all_cnt++] = in_ptr.F32[this_cnt++];
			case TYPES::data_types::MTX3x2:
			case TYPES::data_types::MTX3x3:
			case TYPES::data_types::MTX4x3:
			case TYPES::data_types::MTX4x4:
				//TODO: WRITE THIS ATTRIBUTE TYPES!!!
				break;

			default:
				return result::AWD_ERROR;
		}
	}
	/// \todo Write the stream attributes into file, or make a attribute class that covers both stream-attributes and properties	
	*/
	return result::AWD_SUCCESS;
}

result 
GEOM::ProcessMeshGeometry(Geometry* geom, AWDProject* awd_project){

	if(geom==NULL)
		return result::AWD_ERROR;
	
	/// \section   ProcessGeometry Process Geometry.
	/// \subsection Step1 Step 1: Validate the input.	
	/// We expect this input for each Geometry:
	/// - A list of Triangle. 
	///			- A Triangle must contains a pointer for each Vertex. The Vertex must not be shared between Triangles.
	///			- A Triangle can have a FaceGroup-id. If no FaceGroup-id is provided, the Triangle will be assigned to the default FaceGroup.
	/// - A list of SharedVertices. \n Each SharedVertex contains a list of Vertex that share position. \n This are the Vertex that are contained in the Triangles
	///			- Each Vertex must have a position
	///			- It can also have other attributes like uv and vertex-normals, but also custom-attributes.
	/// - A list of meshes, that are using this Geometry. 
	///			- Each Mesh has a map of FaceGroup-id / Material, that defines what material to use on the mesh for what Face-Group-id.
	///			- A Material that should be used as default material for this mesh, in case the mesh has no material mapped for a FaceGroup-id
	///			- A Material can have SubGeometrySettings, that define how to create the SubGeometry needed to display this mesh in regard to uv and vertex-normals.
	///
	
	std::vector<GEOM::SharedVertex3D*>	all_verts_shared	= geom->get_shared_verts();
	std::vector<Triangle*>				all_triangles		= geom->get_all_triangles();
	///	If the list of triangles is emtpy, we return with error.\n
	if(all_triangles.size() == 0)
		return result::AWD_ERROR;

	/// If the list of shared verticles is smaller than 3, we return with error.\n
	if(all_verts_shared.size() < 3)
		return result::AWD_ERROR;

	/// The number of all Vertex found in SharedVertex must be three times the count of triangles. If not, we return with error.\n
	int count_verts = 0;
	for (SharedVertex3D* shared_vert : all_verts_shared)	
		count_verts += shared_vert->get_verts().size();
	if(count_verts != (all_triangles.size() * 3))
		return result::AWD_ERROR;

	/// If meshes are given, we check if they can be accessed as \ref AWD::BLOCKS::Mesh "Mesh". If not, we return with error.
	//  Geom only stores AWDBlock for meshes, so we cast them into a Mesh* list
	std::vector<Mesh*> meshes;
	for (AWDBlock* base_block : geom->get_mesh_instances()){				
		Mesh* mesh=reinterpret_cast<Mesh*>(base_block);		
		if(mesh==NULL)
			return result::AWD_ERROR;
		meshes.push_back(mesh);
	}
	/// If no mesh is given, we still can create the Geometry, using the Settings, that are set on the geometry.

	
	
	/// \subsection Step2 Step 2: Resolve Instances.	
	///		The Problem: 
	///		-	A Geometry can be used by multiple Meshes. \n
	/// 		SubGeometrySettings affect the output of the geomtry-proccessing.\n
	///			The SubGeometrySettings can be defined per Material, which are assigned to a Mesh. \n\n
	/// 	This follows, that we might need to create clones of the geometry in order to maintain all sub-mesh appearences.\n
	/// 	

	///		The Solution: 
	///		-	Loop over all triangle, and for each mesh-instance, get the SubGeometrySettings to use, and compare it to the SubGeometrySettings of the other mesh-instances.\n
	///			This way, we calculate the minimum of Geometries needed to display all mesh-instances correctly.\n
	///			The geometries are than created, by cloning the original Geometry.\n
	///		-	All Geometries are getting updated, so that contain the correct list of mesh-instances that should use them.\n
	///		-	Mesh-instances also get updated to point to the correct Geometry, and the correct list of materials.\n
	

	std::vector<std::vector<int > >					meshes_sub_geo_ids;		// store a list of subgeo-ids for each mesh
	std::vector<std::vector<AWDBlock* > >			meshes_sub_materials;	// store a list of material for each mesh-instance. in synch with list above.
	
	// create lists for meshes
	for (Mesh* mesh:meshes){
		std::vector<int > new_meshes_sub_geo_ids;
		meshes_sub_geo_ids.push_back(new_meshes_sub_geo_ids);
		std::vector<AWDBlock* > new_meshes_sub_materials;
		meshes_sub_materials.push_back(new_meshes_sub_materials);
	}
	std::map<SUBGEOM_ID_STRING, std::vector<BlockSettings* > > subgeosettings_by_face_group_id; // a dictionary to retrieve BlockSettings by Face-Group-id
	
	
	// For each Triangle, we find the minimum of SubGeometries that must be created to display all mesh-instances correctly.
	for (Triangle* tri: all_triangles){		
		if(tri==NULL){
			// todo: should never happen . raise error (?)
		}
		else{
			// get the SUBGEOM_ID_STRING 
			// If we want to explicit export all different face-ids as subGeometries, we can use the Face-Group-id as lookup-string. 
			SUBGEOM_ID_STRING face_group_id = tri->get_face_group_id();

			// if we want to merge subgeometries that share the same materials,
			// we need to take all references into account, so we build a lookup string from all the merge Face-Group-ids. 
			bool keep_face_groups = true;
			if(!keep_face_groups){
				face_group_id="";
				for (Mesh* mesh:meshes){
					SUBGEOM_ID_STRING face_group;
					mesh->get_merged_face_id(face_group_id, face_group);
					face_group_id+=face_group;
				}
			}
			// set the subgeo_id for the face. we need it later.
			tri->set_subgeom_id(face_group_id);
			if ( subgeosettings_by_face_group_id.find(face_group_id) == subgeosettings_by_face_group_id.end() ) {

				std::vector<BlockSettings*> new_sub_geo_settings;
				subgeosettings_by_face_group_id[face_group_id]=new_sub_geo_settings;

				// for each mesh, set material, subgeomstr and settings.
				// get settings by comparing to other mesh-settings
				TYPES::UINT32 cnt=0;
				for (Mesh* mesh:meshes){
					// get the material for this id (we already have the right id, if we want to merge face-groups)
					AWDBlock* material = mesh->get_material_by_face_id(face_group_id);
					
					// get material for this sub-mesh, or use default. error-return if no default material can be accessed.
					/*if(material==NULL)
						material = mesh->get_defaultMat();					
					if(material==NULL)
						material = awd_project->get_default_material();//TODO get material from AssetLib
					*/
					if(material==NULL)
						return result::AWD_ERROR; // TODO:HANDLE ERRORS

					meshes_sub_materials[cnt].push_back(material);	
					int subGeoID=0;			
					if (meshes.size()>1){
						// get subgeo-settings for this sub-mesh, or use default. error-return if no sub-geom default settings can be accessed.
						SETTINGS::BlockSettings* subGeomSettings=NULL;
						Material* mat = reinterpret_cast<Material*>(material);										
						subGeomSettings = mat->get_sub_geom_settings();
						if(subGeomSettings==NULL)
							subGeomSettings = geom->get_sub_geom_settings();
						//if(subGeomSettings==NULL)
						//	subGeomSettings = awd_project->get_default_subgeomsettings();	
						if(subGeomSettings==NULL)
							return result::AWD_ERROR; // TODO:HANDLE ERRORS	
						int add_this=false;
						for(BlockSettings* subGeoSettings_comp : new_sub_geo_settings){
							if(subGeomSettings->is_compatible(subGeoSettings_comp)){
								add_this=true;
								break;
							}
							subGeoID++;
						}
						if(add_this){
							subGeoID=new_sub_geo_settings.size();
							new_sub_geo_settings.push_back(subGeomSettings);
						}
					}
					meshes_sub_geo_ids[cnt].push_back(subGeoID);
					cnt++;
				}
			}
		}
	}	

	// create the geometries

	std::map<std::string, BLOCKS::Geometry*> final_geoms_map;
	std::vector<BLOCKS::Geometry*> final_geoms;
		
	geom->clear_mesh_inst_list();// clear the mesh-list of the geometry.

	// if no meshes are assigned to this geometry, we need to add it to the geom_list.
	if(meshes.size()==0)
		final_geoms.push_back(geom);
	else{
		TYPES::UINT32 cnt = 0;						// keep track of mesh-instance-count in for-loop	
		bool used_already_existing_geom = false;	// false as long as the initial geometetry hasnt been used.

		for (Mesh* mesh:meshes){		
			std::string mesh_instance_lookup_str;
			// create the lookup string;
			for(int one_sub_mesh_id : meshes_sub_geo_ids[cnt])
				mesh_instance_lookup_str+= FILES::int_to_string(one_sub_mesh_id)+"#";
			// find the geometry to use for this mesh-instance using the mesh_instance_lookup_str we just created.
			BLOCKS::Geometry* new_geom;
			if ( final_geoms_map.find(mesh_instance_lookup_str) == final_geoms_map.end() ) {
			
				if(used_already_existing_geom){				//	Check if the original geometry has already been used, and use it if not.
					new_geom = geom->get_cloned_geometry();
					if(awd_project!=NULL)
						awd_project->add_library_block(new_geom);
				}
				else{
					new_geom = geom;
					used_already_existing_geom = true;
				}
				final_geoms_map[mesh_instance_lookup_str] = geom;	
				final_geoms.push_back(geom);
			}
			else{
				// Geometry already exists. use the existing one
				new_geom = final_geoms_map[mesh_instance_lookup_str];
			}
			cnt++;
			new_geom->add_mesh_inst(mesh);
			mesh->set_geom(new_geom);
		}
	}	
	
	
	/// At this point we have a dedicated Geometry for each Mesh-instance that needs to be exported with own BlockSettings.\n\n
	/// All coming steps will be executed once per Geometry.
	

	for(Geometry* oneGeom: final_geoms){		
		
		/// \subsection Step3 Step 3: Prepare Geometry.	
		///	-	Get the data (mesh-list, shared_verts and triangles). \n
		///		This is either the original Geometry, or one of its clones, so the data already has been validated.

		std::vector<GEOM::Vertex3D*>		this_verts_expanded	= oneGeom->get_all_verts();
		std::vector<GEOM::SharedVertex3D*>	this_verts_shared	= oneGeom->get_shared_verts();
		std::vector<Triangle*>				this_triangles		= oneGeom->get_all_triangles();

		///	-	Loop over all Triangles, and create a SubGeom for each encountered SubGeo-id that is found on a Triangle.\n
		///		A SubGeom is used to fill a SubGeometry with data, without worrying about RessourceLimits. \n
		///		e.g. One SubGeom can hold multiple SubGeometries, in case Ressource-Limits was reached.\n
		///	-	For each Vertex of a Triangle, apply the BlockSettings that should be used for this SubGeo as well as the facegroup_id and the subgeom_id of the triangle thats using this vert.\n
		///		This three things are needed to properly calculate the vertex-normals.
		///		We also set the set target-vertex and IDXmap to NULL for each Vertex.\n
		///		The Target-Vertex and ID-maps are later used to redirect a Vertex to another Vertex and its index in a SubGeometry-vert-list.
		///	-	In this step we add the subgeocontrols to the geom, in synch with adding materials to mesh. If no material can be found for a mesh, we use default material.
		
		std::map<SUBGEOM_ID_STRING, SubGeom*> subgeom_map;
		for(GEOM::Triangle* tri : this_triangles){
			SUBGEOM_ID_STRING sub_geo_id = tri->get_subgeom_id();
			SUBGEOM_ID_STRING face_group_id = tri->get_face_group_id();
			
			BlockSettings* settings = NULL; 
			if ( subgeom_map.find(sub_geo_id) == subgeom_map.end() ) {
								
				for(AWDBlock* awd_block : oneGeom->get_mesh_instances()){
					Mesh* thisMesh = reinterpret_cast<Mesh*>(awd_block);
					Material* thisMat = reinterpret_cast<Material*>(thisMesh->get_material_by_face_id(face_group_id));
					thisMesh->add_final_material(thisMat);
					if(settings==NULL){
						if(thisMat->get_sub_geom_settings()!=NULL)
							settings=thisMat->get_sub_geom_settings();
					}
				}
				SubGeom* new_sub_geom=new SubGeom(settings);			
				subgeom_map[sub_geo_id] = new_sub_geom;
				oneGeom->add_subgeo(new_sub_geom);
			}
			else{
				settings = subgeom_map[sub_geo_id]->get_settings();
			}
			std::vector<Vertex3D*> tri_verts(3);
			tri->get_verts(tri_verts);
			for(Vertex3D* vert:tri_verts){
				vert->set_v_n_settings(settings);
				vert->set_target_vertex(NULL);
				vert->set_face_group_id(face_group_id);
				vert->set_subgeom_id(sub_geo_id);	
			}
		}
		
		/// \subsection Step4 Step 4: Calculate Vertex-Normals and prepare shared-vertex-lookup
		///	-	We want calculate the vertex-data on Geometry-Level, before splitting into the actual SubGeometries.
		///		This way, we do we can preserve and smooth normals accross SubGeometries.\n
		/// -	Loop over all SharedVertex:
		///			-	Call calculate_normals() for each SharedVertex. 
		///				This will calculate the normals based on the settings we set on all the verticles in previous step.
		///			-	After calculating the normals, we call connect_duplicates() force each SharedVertex to check for duplicates inside its vert-list.
		///				When checking for duplicates, custom-attributes can be set to be considered when comparing vertices.
		///				If a duplicate is encountered, we set the target-vertex of this vertex to be the first encountered instance of this duplicates
		///				This will speed up look-up times when creating the final Vertex-lists for each subGeometry.
		
		for(GEOM::SharedVertex3D* shared_vert : this_verts_shared){
			shared_vert->calculate_normals();
			shared_vert->connect_duplicates();
		}
		
		/// \subsection Step5 Step 5: Build SubGeometries.
		///	-	Loop over triangles and fill previous created SubGeom with Triangles and vertices.\n
		///		For each Vertex of the Triangle, we check if it is a duplicate.\n
		///		If it is a Duplicate, we get the unique one (target_vertex()).\n
		///		Each uniqe Vertex stores a map with its index-position inside a subGeom with subGeom_id.\n
		///		This way we can ask a Vertex if it has already been exporter into a subgeom and it will report back the index.\n
		
		for(GEOM::Triangle* tri : this_triangles){			
			SUBGEOM_ID_STRING sub_geo_id = tri->get_subgeom_id();		//	get the sub_geom_id from triangle			
			SubGeom* sub_geom = subgeom_map[sub_geo_id];			//	get the SubGeomcontrol from map that was created in previous step.
			SUBGEOM_ID_STRING this_id;
			sub_geom->get_internal_id(this_id);
			sub_geo_id += "#"+this_id;				//	append internal_id of subGeom to sub_geom_id 
			
			tri->reset_out_indices();
			std::vector<Vertex3D*> tri_verts(3);
			tri->get_verts(tri_verts);
			for(Vertex3D* vert:tri_verts){
				int vert_idx=-1;										//	ask the vert if it has already been exported to this subgeom
				vert_idx = vert->get_idx_for_sub_geo_id(sub_geo_id);	//	if this is a duplicate vertex, it will internally get redirected to the orginal vertex
				if(vert_idx<0)
					sub_geom->add_vertex(vert);							//	the vert hasnt been exported yet, so export it.
				tri->add_out_index(vert_idx);
			}
			sub_geom->add_triangle(tri);
		}
		
		///		Thats all.\n
		///		The Geometries now have been processed completly, and should have a list of Subgeometries for each Geometry.\n
		///		Each SubGeometry contains a list of Vertex and a list of Triangles.\n
		///		The SubGeometry is now in a state, that it could be modifed (scale/merge).\n
		///		The actual data-streams of a SubGeometry will never exist in libawd.\n
		///		They only exist as recipe that can be written into a file.
	}
	return result::AWD_SUCCESS;
}

#define DEBUGSHAPEPROCESS =1;

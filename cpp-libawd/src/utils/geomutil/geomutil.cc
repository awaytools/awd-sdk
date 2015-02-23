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
	else if(attr_desc->get_type()==GEOM::data_stream_attr_type::COLOR){
		file_writer->writeFLOAT32(vert->get_color().x);
		file_writer->writeFLOAT32(vert->get_color().y);
		file_writer->writeFLOAT32(vert->get_color().z);
		file_writer->writeFLOAT32(vert->get_color().w);
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
				mesh_instance_lookup_str+= std::to_string(one_sub_mesh_id)+"#";		
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
		
		BlockSettings* defaultSettings = geom->get_sub_geom_settings();
		Mesh* this_mesh = reinterpret_cast<Mesh*>(oneGeom->get_first_mesh());
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

result
GEOM::ProcessShapeGeometry(Geometry* geom, AWDProject* awd_project)
{	
	
	/// \section   ProcessPathGeometry Process Path Geometry.
	/// \subsection PathGeoStep1 Step 1: Validate the Geometry.
	/// We expect this input for each Geometry:
	/// - A list of FilledRegion. 
	///			-	A FilledRegion has a type
	///				-	NORMAL_FILL			= a normal closed path that is filled with a fill-style, and can have holes
	///				-	CONVERTED_FROM_STROKS = a normal closed path that is filled with a fill-style and was generated froma stroke, and can have holes
	///				-	FILL_FOR_FONT			= a set of pathes that are generated for a gylph in a font
	///			-	A FilledRegion has a multiple pathes (list of PathSegment)
	///				-	A PathSegment is either line-segment = 2 Vertex2D, or curve-segment = 3 Vertex3D
	///			-	A material that is asociated with this Shape. can be empty (fonts)
		
	/// >	__All coming steps are performed for each FilledRegion found in the Geometry__ \n
	/// >	Maybe we will even need to check intersections across FilledRegion (?)
	
	AWD::result path_result;
	AWD::result region_result;

	int region_cnt=0; 
	int path_cnt=0; 
	
	bool has_valid_shape=false;
		
	int all_max_x=std::numeric_limits<int>::max()*-1;
	int all_max_y=std::numeric_limits<int>::max()*-1;
	int all_min_x=std::numeric_limits<int>::max();
	int all_min_y=std::numeric_limits<int>::max();

	int all_path_cnt = 0;

	// one geometry should either contain filled regions created from fills/strokes or filled regions created from fonts

	int cnt_regions_standart = 0;
	int cnt_regions_strokes = 0;
	int cnt_regions_fonts = 0;

	std::string geom_message = "";
	std::string geom_warning = "";
	std::string geom_errors = "";

	std::vector<FilledRegion*> valid_regions;

	// loop over all FilledRegion.
	for(FilledRegion* filled_region : geom->get_filled_regions()){		
		region_cnt++;

		if(filled_region->get_type()==TYPES::filled_region_type::STANDART_FILL)
			cnt_regions_standart++;
		else if(filled_region->get_type()==TYPES::filled_region_type::FILL_CONVERTED_FROM_STROKE)
			cnt_regions_strokes++;
		else
			cnt_regions_fonts++;
		
		geom_message += "Region "+std::to_string(region_cnt)+" -  path-count = "+std::to_string(filled_region->get_pathes().size())+"\n";

///		\subsection PathGeoStep2 Step 2: Validate and prepare the FilledRegion.	
///		-	For each path inside a FilledRegion, we do this:\n		

		// if this is false after the for-loop, we set a warning-message on the geometry, and set the geometry to invalid
		bool has_valid_path=false;
	
		// keep track of the index of the current processed path, so we can give meaningfull output.
		
		// loop over all pathes.
		std::vector<GEOM::Path*> pathes = filled_region->get_pathes();
		for(GEOM::Path* one_path: pathes){
			all_path_cnt++;
			path_cnt++;
			path_result = one_path->validate_path();
			if(path_result==result::PATH_CONTAINS_LESS_THAN_THREE_SEGMENTS)
				geom_errors += "Path contains less than 3 segs.\n";		

			else{				
				has_valid_path=true;
				if(path_result==result::PATH_NOT_CLOSING){
					geom_warning += "Path not closing.\n";		
				}
			}
			
		}
///		-	If all path of this FilledRegion are invalid, we skip to next FilledRegion.\n
		if(!has_valid_path){
			geom_errors += "No path of region valid\n";		
			continue;
		}		
		valid_regions.push_back(filled_region);	
///		- set all pathes of filled region to is_hole = true. only first path is set to is_hole = false;
///		  for pathes generated from fonts, we need to re-check this later.	
		for(GEOM::Path* one_path: pathes)
			one_path->set_is_hole(true);
		pathes[0]->set_is_hole(false);


///		\subsection PathGeoStep3 Step 3: Calculate Curve-Types.
///		-	Up until here, curves have only been handled as 2 types. OUTTER_EDGE and CURVED_EDGE.
///		-	Before tesselation, we want to know the exact curve-types (convex / concave).\n
///			We want to know this, because in tesselation, we only want to include the control-points for concave curves.\n
///			For convex-curves and straight lines, we only want to include strat-point and end-point in tesselation.\n
///		-	Optionally we have a setting that will convert all curves into linear segments, hence disregarding all curves
///	\n __Problem:__ \n
///		-	For each path and each curved-segment, we need to decide the type of curve.\n
/// \n __Solution:__ \n
///		-	We can solve this by testing if the control-point of the curve is outside or inside the path that contains this segment.\n
///			If it is inside, and the path is a contour, than the curve will be convex.\n
///			If it is outside, and the path is a contour, than it is concave.\n
///			If the path is a hole, the result of the test needs to be inverted.
///		-	This solution will fail if the path gets very close to itself.\n
///			To fix this, instead of testing if the control-point is contained in the path,\n
///			we calculate the center of the line start-point <-> end-point, and move it a little step pixel in direction of the control-point.\n
///			Now even for nearly-intersecting pathes, we can savly decide on the type of curve.\n
	
		if(awd_project->get_settings()->get_export_curves()){
			for(GEOM::Path* one_path: filled_region->get_pathes()){
				one_path->get_curve_types(awd_project->get_settings());	
			}
		}
		else{
			// tranform all curves into linear segments.
			for(GEOM::Path* one_path: filled_region->get_pathes())
				one_path->make_linear();				
		}	

		if(all_min_x>filled_region->get_pathes()[0]->min_x)
			all_min_x=filled_region->get_pathes()[0]->min_x;
		if(all_max_x<filled_region->get_pathes()[0]->max_x)
			all_max_x=filled_region->get_pathes()[0]->max_x;
		if(all_min_y>filled_region->get_pathes()[0]->min_y)
			all_min_y=filled_region->get_pathes()[0]->min_y;
		if(all_max_y<filled_region->get_pathes()[0]->max_y)
			all_max_y=filled_region->get_pathes()[0]->max_y;		
	}

	// at this point we have validated all input (all path-segments are continious and the path is closed)
	// self intersecting is not detected yet.
	
	// now we want to sort filled regions based on quantizised intersecting.
			
		
	
	
	/*
	// testing bit-op
	TYPES::UINT16 max = (1 << 8);	//try 6
	TYPES::UINT16 min = (1 << 3);	//try 2
	std::bitset<16> x1(max);
	std::string new_message="Bit-tests\n";
	new_message+="max = "+x1.to_string()+"\n";
	std::bitset<16> x2(min);
	new_message+="min = "+x2.to_string()+"\n";
	TYPES::UINT16 c1 = max - min;	
	TYPES::UINT16 hash = max | c1;	
	TYPES::UINT16 hash_test_b = max | max - min; 	
	std::bitset<16> x3(c1);
	new_message+="c   = "+x3.to_string()+"\n";
	std::bitset<16> x4(hash);
	new_message+="h   = "+x4.to_string()+"\n";	
		
    TYPES::UINT16 dataBoth = ((((c1 & 0xff) * 0x0202020202ULL & 0x010884422010ULL) % 1023) << 8) | ((((c1 & 0xff00) >> 8) * 0x0202020202ULL & 0x010884422010ULL) % 1023);
	std::bitset<16> x7(dataBoth);
	new_message+="hres = "+x7.to_string()+"\n";
	
	geom->add_message(new_message, message_type::STATUS_MESSAGE);
	*/
	// helper-class to accosicate the bit-description with a region-seg
	// provides sorting function.
	
	SORTER RS;
	std::vector<FilledRegionGroup*> region_bits = std::vector<FilledRegionGroup*>( valid_regions.size());
	std::vector<FilledRegionGroup*> final_sorted_filled_groups;
	
	TYPES::UINT32 minx_int = 0;
	TYPES::UINT32 maxx_int = 0;
	TYPES::UINT32 miny_int = 0;
	TYPES::UINT32 maxy_int = 0;
	TYPES::UINT16 x_int = 0;
	TYPES::UINT16 y_int = 0;
	TYPES::UINT32 combined_int = 0;

	int all_width= all_max_x - all_min_x;
	int all_height= all_max_y - all_min_y;

	double all_conversionFactorx = double(1) / double(double(all_width)  / double (16));
	double all_conversionFactory = double(1) / double(double(all_height) / double (16));

	double int_min_x=0;
	double int_max_x=0;
	double int_min_y=0;
	double int_max_y=0;

	double min_x_double = 0.0;
	double max_x_double = 0.0;
	double min_y_double = 0.0;
	double max_y_double = 0.0;
	
	std::vector<FilledRegion*> final_filled_regions_list;

	if((false)||(cnt_regions_fonts>0)){
		// loop over regions:
		region_cnt=0;
		udword* InputValues_regions = new udword[ valid_regions.size()];
		for(FilledRegion* filled_region :  valid_regions){	
		
			// convert the bounds as doubles in range 0.0 - 16.0

			min_x_double = double(filled_region->get_pathes()[0]->min_x - all_min_x ) * double(all_conversionFactorx);
			max_x_double = double(filled_region->get_pathes()[0]->max_x - all_min_x ) * double(all_conversionFactorx);
			min_y_double = double(filled_region->get_pathes()[0]->min_y - all_min_y ) * double(all_conversionFactory);
			max_y_double = double(filled_region->get_pathes()[0]->max_y - all_min_y ) * double(all_conversionFactory);
			
			min_x_double = modf (min_x_double , &int_min_x);
			max_x_double = modf (max_x_double , &int_max_x);
			min_y_double = modf (min_y_double , &int_min_y);
			max_y_double = modf (max_y_double , &int_max_y);

			minx_int = TYPES::UINT32( int_min_x);
			maxx_int = TYPES::UINT32( int_max_x);
			miny_int = TYPES::UINT32( int_min_y);
			maxy_int = TYPES::UINT32( int_max_y); 

			if(minx_int==16)
				minx_int=15;
			if((max_x_double==0.0)&&(maxx_int>0))
				maxx_int--;

			if(miny_int==16)
				miny_int=15;
			if((max_y_double==0.0)&&(maxy_int>0))
				maxy_int--;


			// create a 16bit value descriping the occupied area on x-axis (e.g. bounds of min = 3 and max = 12 looks like this: 0001111111110000)
			x_int = get_bit16(minx_int, maxx_int);
			// create a 16bit value descriping the occupied area on y-axis (e.g. bounds of min = 4 and max = 8  looks like this: 0000111100000000)
			y_int = get_bit16(miny_int, maxy_int);
			// merge both 16bit values into a 32bit value (e.g. 0001111111110000 + 0000111100000000 = 00011111111100000000111100000000)
			combined_int = (TYPES::UINT32(x_int) << 16) | y_int;

			// create the BitGridCell.
			FilledRegionGroup* region_bit = new FilledRegionGroup();
			// set the 32bit value for the BitGridCell
			region_bit->bit_id_x=x_int;
			region_bit->bit_id_y=y_int;
			region_bit->group_id=region_cnt;
			region_bit->add_filled_region(filled_region);
			// add Cell to final list.
			InputValues_regions[region_cnt] = combined_int;
			region_bits[region_cnt++]=region_bit;	
		
			/*
			std::bitset<16> x1(x_int);
			std::bitset<16> x2(y_int);
			std::bitset<32> x3(combined_int);
			std::string new_message = "\nCreated BitCell for Filled-region-group id: "+std::to_string(region_cnt-1)+" (int: "+std::to_string(combined_int)+")\n";
			new_message += "	x-axis-ints: "+std::to_string(minx_int)+" "+std::to_string(maxx_int)+"\n";
			new_message += "	x-axis-bits: "+x1.to_string()+"\n";
			new_message += "	y-axis-ints: "+std::to_string(miny_int)+" "+std::to_string(maxy_int)+"\n";
			new_message += "	y-axis-bits: "+x2.to_string()+"\n";
			new_message += "	32bit value: "+x3.to_string()+"\n";
			geom->add_message(new_message, message_type::STATUS_MESSAGE);
			*/
		}	
		/*
		std::string new_message = "region bits before sort: \n";
		for(region_cnt = 0; region_cnt<geom->get_filled_regions().size(); region_cnt++){
			std::bitset<32> x1(InputValues_regions[region_cnt]);
			new_message += x1.to_string()+" "+std::to_string(InputValues_regions[region_cnt])+"\n";
		}
		geom->add_message(new_message, message_type::STATUS_MESSAGE);
		*/
		const udword* Sorted = RS.Sort(InputValues_regions, valid_regions.size(), RADIX_UNSIGNED).GetRanks();
		/*
		std::string new_message2 = "region bits after sort: \n";
		for(region_cnt = 0; region_cnt<geom->get_filled_regions().size(); region_cnt++){
			std::bitset<32> x1(InputValues_regions[Sorted[region_cnt]]);
			new_message2 += x1.to_string()+" "+std::to_string(InputValues_regions[Sorted[region_cnt]])+"\n";
		}
		geom->add_message(new_message2, message_type::STATUS_MESSAGE);
		*/

		int original_region_cnt = region_cnt;
		region_cnt=0;
		int region_cnt2=0;

		for(region_cnt = original_region_cnt; region_cnt--;){
			FilledRegionGroup* this_rg=region_bits[Sorted[region_cnt]];
			for(region_cnt2 = region_cnt; region_cnt2--;){
				FilledRegionGroup* next_rg=region_bits[Sorted[region_cnt2]];			
				if ((this_rg->bit_id_x & next_rg->bit_id_x)==0)
					break;		
				if ((this_rg->bit_id_y & next_rg->bit_id_y)==0)
					continue;			
				if(!bounds_intersect(this_rg->min_x, this_rg->max_x, this_rg->min_y, this_rg->max_y, next_rg->min_x, next_rg->max_x, next_rg->min_y, next_rg->max_y))
					continue;
				if(next_rg->redirect!=NULL)
					this_rg->redirect=next_rg->redirect;
				else if(this_rg->redirect!=NULL)
					next_rg->redirect=this_rg->redirect;
				else
					next_rg->redirect=this_rg;
				/*std::string new_message = "\nDetected Intersecting: "+std::to_string(this_rg->group_id)+" vs "+std::to_string(next_rg->group_id)+")\n";
				geom->add_message(new_message, message_type::STATUS_MESSAGE);*/
			}
		}
		for(FilledRegionGroup* one_region_group: region_bits){
			if(one_region_group->redirect==NULL)
				final_sorted_filled_groups.push_back(one_region_group);
			else			
				one_region_group->redirect->merge_group_into(one_region_group);

		}

		// at this point we have lists of filled_regions that we know might intersect.
		// we could further check bounding boxes etc, but i think this might be good enough.
	
		std::string geom_message_bkp = geom_message;
		geom_message = "\nInput Regions = "+std::to_string(valid_regions.size())+ " ( "+std::to_string(cnt_regions_standart)+" fill / "+std::to_string(cnt_regions_strokes)+" strokes / "+std::to_string(cnt_regions_fonts)+" font )\n"+geom_message;
		geom_message += "\nCalculated intersecting groups of regions = "+std::to_string(final_sorted_filled_groups.size())+"\n";
		
		// if we do not deal with regions generated for fonts, we can just use the input list of pathes for tesselation.
		if(cnt_regions_fonts==0)
			final_filled_regions_list = valid_regions;
		else
		{
			// if we are dealing with regions generated for fonts, we can need to check what path is a hole, and what is a contour, in order to adjust the curve-type.
			// filled regions coming from fonts, should only contain 1 path.
			// we merge them into one filled_region with multiple pathes (if pathes bounding boxes intersect)

			// we must decide what is a hole and what not.
			// do this by creating one combined path.
			// than we can check for each path, how often the startpoint intersects with the complete point.
			// if it intersects a even number, it is a contour.
			// if it intersects a odd number, it is a hole

			final_filled_regions_list=std::vector<FilledRegion*>();
			Path newPath = Path();
			for(FilledRegionGroup* filled_regions_group : final_sorted_filled_groups){
				FilledRegion* filled_region = new FilledRegion(TYPES::filled_region_type::GENERATED_FONT_OUTLINES);
				for(FilledRegion* filled_region : filled_regions_group->filled_regions){
					for(PathSegment* pathSeg:filled_region->get_pathes()[0]->get_segments()){
						newPath.add_segment_fonts(pathSeg);
					}
					filled_region->add_existing_path(filled_region->get_pathes()[0]);
					final_filled_regions_list.push_back(filled_region);
				}
			}
			newPath.prepare(edge_type::OUTTER_EDGE);
			for(FilledRegionGroup* filled_regions_group : final_sorted_filled_groups){
				for(FilledRegion* filled_region : filled_regions_group->filled_regions){
					PathSegment* this_seg = filled_region->get_pathes()[0]->get_segments()[0];
					int is_hole=newPath.point_inside_path(this_seg->get_startPoint().x, this_seg->get_startPoint().y);
					if(is_hole % 2){
						// this is a hole. so we update the path accordingly
						newPath.set_is_hole(true);
						for(PathSegment* pathSeg:filled_region->get_pathes()[0]->get_segments()){
							if(pathSeg->get_edgeType()==edge_type::CONCAVE_EDGE)
								pathSeg->set_edgeType(edge_type::CONVEX_EDGE);
							else if(pathSeg->get_edgeType()==edge_type::CONVEX_EDGE)
								pathSeg->set_edgeType(edge_type::CONCAVE_EDGE);
						}
					}
				}
			}
		}
	}
	else{
		region_cnt=0;
		final_filled_regions_list = valid_regions;
		for(FilledRegion* filled_region : valid_regions){
			region_cnt++;
			FilledRegionGroup* region_group = new FilledRegionGroup();
			// set the 32bit value for the BitGridCell
			region_group->group_id=region_cnt;
			region_group->add_filled_region(filled_region);
			// add Cell to final list.
			final_sorted_filled_groups.push_back(region_group);
		}
	}
	
	// now we have a grouped the filled regions.
	// we now want to get the pairs of intersecting segments for each group of filled-regions
	
	
	// declare vars:

	int cnt_all_segs = 0;	
	
	int cnt_region_segs = 0;
	int width = 0;
	int height= 0;
	int region_min_x=0;
	int region_max_x=0;
	int region_min_y=0;
	int region_max_y=0;	
	std::vector<int> bounds(4);	
	std::vector<PathSegment*> all_intersecting_segs;
	int cnt_intersects = 0;
	int seg_cnt=0;
	int seg_cnt2=0;
	std::string segbytes_as_string="";
	region_cnt=0;
	PathSegment* seg_1;
	PathSegment* seg_2;
	
	int oiginal_seg_count=0;
	for(FilledRegionGroup* filled_regions_group : final_sorted_filled_groups){
		
		// calculate the bounds of the region-group.
		// if it only contains on region, we can just use the bounds of the first path 
		cnt_region_segs = 0;
		region_min_x=filled_regions_group->min_x;
		region_max_x=filled_regions_group->max_x;
		region_min_y=filled_regions_group->min_y;
		region_max_y=filled_regions_group->max_y;
		if(filled_regions_group->filled_regions.size()==1){	
			for(GEOM::Path* one_path: filled_regions_group->filled_regions[0]->get_pathes())
				cnt_region_segs+=one_path->get_segments().size();
		}
		else{
			for(FilledRegion* filled_region : filled_regions_group->filled_regions){
				for(GEOM::Path* one_path: filled_region->get_pathes())
					cnt_region_segs+=one_path->get_segments().size();
			}
		}		

		// calculate the width and height of the region-group inisde the bit-grid (0-15 on x and on y axis)
		width = (region_max_x - region_min_x);
		height = (region_max_y - region_min_y);
		// calculate the conversion factor used to translate segment-bounds into bit-grid values 
		double conversionFactorx = double(1) / double(double(width) / double(16));
		double conversionFactory = double(1) / double(double(height) / double(16));
		// init a vector of BitGridCells with length of cnt_region_segs
		std::vector<PathSegment*> region_segment_bits = std::vector<PathSegment*>(cnt_region_segs);
		udword* InputValues_segments = new udword[cnt_region_segs];
		oiginal_seg_count=cnt_region_segs;
		cnt_region_segs=0;
		for(FilledRegion* filled_region : filled_regions_group->filled_regions){
			for(GEOM::Path* one_path: filled_region->get_pathes()){
				for(PathSegment* pathSeg:one_path->get_segments()){
					
					min_x_double = double(pathSeg->min_x - region_min_x ) * double(conversionFactorx);
					max_x_double = double(pathSeg->max_x - region_min_x ) * double(conversionFactorx);
					min_y_double = double(pathSeg->min_y - region_min_y ) * double(conversionFactory);
					max_y_double = double(pathSeg->max_y - region_min_y ) * double(conversionFactory);
			
					min_x_double = modf (min_x_double , &int_min_x);
					max_x_double = modf (max_x_double , &int_max_x);
					min_y_double = modf (min_y_double , &int_min_y);
					max_y_double = modf (max_y_double , &int_max_y);

					minx_int = TYPES::UINT32( int_min_x);
					maxx_int = TYPES::UINT32( int_max_x);
					miny_int = TYPES::UINT32( int_min_y);
					maxy_int = TYPES::UINT32( int_max_y); 

					if(minx_int==16)
						minx_int=15;
					if((max_x_double==0.0)&&(maxx_int>0))
						maxx_int--;

					if(miny_int==16)
						miny_int=15;
					if((max_y_double==0.0)&&(maxy_int>0))
						maxy_int--;

					// create a 16bit value descriping the occupied area on x-axis (e.g. bounds of min = 3 and max = 12 looks like this: 0001111111110000)
					x_int = get_bit16(minx_int, maxx_int);
					// create a 16bit value descriping the occupied area on y-axis (e.g. bounds of min = 4 and max = 8  looks like this: 0000111100000000)
					y_int = get_bit16(miny_int, maxy_int);
					// merge both 16bit values into a 32bit value (e.g. 0001111111110000 + 0000111100000000 = 00011111111100000000111100000000)
					combined_int = (((TYPES::UINT32) x_int) << 16) | y_int;
					
					pathSeg->bit_id_x=x_int;
					pathSeg->bit_id_y=y_int;
					pathSeg->set_num_id(cnt_region_segs);

					// add Cell to final list.
					InputValues_segments[cnt_region_segs]=combined_int;
					region_segment_bits[cnt_region_segs++]=pathSeg;			
					//pathSeg->set_num_id(cnt_all_segs++);
					/*
					// control output:
					std::bitset<16> x1(x_int);
					std::bitset<16> x2(y_int);
					std::bitset<32> x3(combined_int);
					std::string new_message = "\nCreated BitGridCell for Segment "+std::to_string(cnt_region_segs-1)+"\n";
					new_message += "	x-axis: "+std::to_string(minx_int)+" "+std::to_string(maxx_int)+"\n";
					new_message += "	x-axis: "+x1.to_string()+"\n";
					new_message += "	y-axis: "+std::to_string(miny_int)+" "+std::to_string(maxy_int)+"\n";
					new_message += "	y-axis: "+x2.to_string()+"\n";
					new_message += "	Result: "+x3.to_string()+"\n";
					geom->add_message(new_message, message_type::STATUS_MESSAGE);
					*/
					
				}
			}
		}
		/*
		std::string new_message = "segment bits before sort: \n";
		for(seg_cnt = 0; seg_cnt<oiginal_seg_count; seg_cnt++){
			std::bitset<32> x1(InputValues_regions[seg_cnt]);
			new_message += x1.to_string()+" "+std::to_string(InputValues_segments[seg_cnt])+"\n";
		}
		geom->add_message(new_message, message_type::STATUS_MESSAGE);
		*/
		SORTER RS_segs;
		const udword* Sorted = RS_segs.Sort(InputValues_segments, oiginal_seg_count, RADIX_UNSIGNED).GetRanks();
		/*
		std::string new_message2 = "segment bits after sort: \n";
		for(seg_cnt = 0; seg_cnt<oiginal_seg_count; seg_cnt++){
			std::bitset<32> x1(InputValues_regions[Sorted[seg_cnt]]);
			new_message2 += x1.to_string()+" "+std::to_string(InputValues_segments[Sorted[seg_cnt]])+"\n";
		}
		geom->add_message(new_message2, message_type::STATUS_MESSAGE);
		*/
		
		for(seg_cnt = oiginal_seg_count; seg_cnt--;){
			seg_1 = region_segment_bits[Sorted[seg_cnt]];
			for(seg_cnt2 = seg_cnt; seg_cnt2--;){				
				seg_2 = region_segment_bits[Sorted[seg_cnt2]];
				if((seg_1->bit_id_x & seg_2->bit_id_x)==0)
					break;
				if((seg_1->bit_id_y & seg_2->bit_id_y)==0)
					continue;
				// if the segments are neighbours, we do not consider them intersecting. maybe we should (?)
				if((seg_1 == seg_2->get_next())||(seg_1 == seg_2->get_last()))
					continue;
				if((seg_1->get_edgeType() == edge_type::OUTTER_EDGE)&&(seg_2->get_edgeType() == edge_type::OUTTER_EDGE ))
					continue;
				// if segments do not intersect on bounding box, they do not intersect at all 	
				if(!bounds_intersect(seg_1->min_x, seg_1->max_x, seg_1->min_y, seg_1->max_y, seg_2->min_x, seg_2->max_x, seg_2->min_y, seg_2->max_y))
					continue;

				all_intersecting_segs.push_back(seg_1);				
				all_intersecting_segs.push_back(seg_2);
			}
		}
		DELETEARRAY(InputValues_segments);

	}
	if(all_intersecting_segs.size()>0)
		geom_message+="Found pairs of possible intersecting segments: "+std::to_string(all_intersecting_segs.size()/2);

	//region_messages_list[region_cnt]+="\nINTERSECTIONS: Found "+std::to_string(all_intersecting_segs.size()/2)+" possible intersecting pairs of segments\n";
	for(seg_cnt=0; seg_cnt<all_intersecting_segs.size(); seg_cnt+=2){
		seg_1 = all_intersecting_segs[seg_cnt];
		seg_2 = all_intersecting_segs[seg_cnt+1];
		seg_1->set_state(edge_state::SUBDIVIDED);
		seg_2->set_state(edge_state::SUBDIVIDED);
		for(PathSegment* subSeg2:seg_1->get_subdivided_path())
			subSeg2->set_state(edge_state::SUBDIVIDED);
		for(PathSegment* subSeg2:seg_2->get_subdivided_path())
			subSeg2->set_state(edge_state::SUBDIVIDED);
		if(seg_1->get_edgeType()==edge_type::OUTTER_EDGE)
			resolve_line_curve_intersection(seg_2, seg_1, awd_project->get_settings());
		else if(seg_2->get_edgeType()==edge_type::OUTTER_EDGE)
			resolve_line_curve_intersection(seg_1, seg_2, awd_project->get_settings());
		else
			resolve_curve_curve_intersection(seg_1, seg_2, awd_project->get_settings());

	}


	
	region_cnt=0;
	for(FilledRegion* filled_region : final_filled_regions_list){	
		region_cnt++;
/// \subsection PathGeoStep5 Step 5: tesselation.	
///	-	Create a new tesselator from libtess2.
///	-	For each path, create a list of Tessreal.\n
///		Loop over list of PathSegments, and create a TESSreal for the StartPoint.
///		If the PathSegment is a convex curve, we also add the control-point of this PathSegment.
///		Each list of TESSreal is added as contour to the tesselator.
///	-	Call the tesselation function on the tesselator.
///		If the tesselation-function is not exectuted without error, we add a warning-message and skip to next FilledRegion

		TESStesselator* tess = tessNewTess(NULL);

		// for each path calculate the number of points needed for tesselation.
		// store this numbers in point_counts list 
		// also create a look-up cache for edge-string (the both vert-idx of a edge) and Path-Segment with edge-type OUTTEREDGE.
		std::map<std::string, GEOM::PathSegment*> edge_cache;
		// save a pointer to all the tesPoints that we are creating. I am not sure if we are suposed to delete them or not.
		std::vector<TESSreal*> tesPoints;	
		int pnt_cnt=0;
		int all_pnt_cnt=0;
		int path_start_point_idx=0;
		path_cnt=0;

		for(GEOM::Path* one_path: filled_region->get_pathes()){
			pnt_cnt=one_path->get_point_count(GEOM::edge_type::CONVEX_EDGE);
			// for each path create the list of TESSreal and add it as contour.
			TESSreal* thisPoints = (TESSreal*)malloc(sizeof(TESSreal) * pnt_cnt*2);
			int pnt_cnt_2=0;
			path_start_point_idx=all_pnt_cnt;
			for(PathSegment* pathSeg:one_path->get_segments())
			{
				// if we want either to sort interior / exterior tis or keep verts in order of path, we need to build a map to map the resulting tris to line-segments
				if((awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d())||(awd_project->get_settings()->get_keep_verticles_in_path_order())){
					if(pathSeg->get_edgeType()==GEOM::edge_type::OUTTER_EDGE){
						std::string edge_str = std::to_string(all_pnt_cnt) + "#" + std::to_string(all_pnt_cnt+1);
						if(pathSeg==one_path->get_segments().back())
							edge_str = std::to_string(path_start_point_idx) + "#" + std::to_string(all_pnt_cnt);
						edge_cache[edge_str]=pathSeg;
					}	
				}
				for(PathSegment* inner_seg:pathSeg->get_subdivided_path())
				{
					all_pnt_cnt++;
					thisPoints[pnt_cnt_2++]=inner_seg->get_startPoint().x;
					thisPoints[pnt_cnt_2++]=inner_seg->get_startPoint().y;
					if(inner_seg->get_edgeType()==GEOM::edge_type::CONVEX_EDGE){
						all_pnt_cnt++;
						thisPoints[pnt_cnt_2++]=inner_seg->get_controlPoint().x;
						thisPoints[pnt_cnt_2++]=inner_seg->get_controlPoint().y;
					}
				}	
			}
			tessAddContour(tess, 2, &thisPoints[0], sizeof(TESSreal)*2, pnt_cnt);
			tesPoints.push_back(thisPoints);		
		}

		/*
		// TESTDATA = A Rectangle with a hole	
		int pntCnt=0;

		TESSreal* thisPoints= (TESSreal*)malloc(sizeof(TESSreal)*8);
		thisPoints[pntCnt++]=-100.0;
		thisPoints[pntCnt++]=-100.0;
		thisPoints[pntCnt++]=-100.0;
		thisPoints[pntCnt++]=-100.0;
		thisPoints[pntCnt++]=100.0;
		thisPoints[pntCnt++]=100.0;
		thisPoints[pntCnt++]=-100.0;
		thisPoints[pntCnt++]=100.0;
		tessAddContour(tess, 2, &thisPoints[0], sizeof(TESSreal)*2, 4);	

		pntCnt=0;
		TESSreal* thisPoints3= (TESSreal*)malloc(sizeof(TESSreal)*8);
		thisPoints3[pntCnt++]=120.0;
		thisPoints3[pntCnt++]=-100.0;
		thisPoints3[pntCnt++]=200.0;
		thisPoints3[pntCnt++]=-100.0;
		thisPoints3[pntCnt++]=200.0;
		thisPoints3[pntCnt++]=100.0;
		thisPoints3[pntCnt++]=120.0;
		thisPoints3[pntCnt++]=100.0;
		tessAddContour(tess, 2, &thisPoints3[0], sizeof(TESSreal)*2, 4);

		pntCnt=0;
		TESSreal* thisPoints2= (TESSreal*)malloc(sizeof(TESSreal)*8);
		thisPoints2[pntCnt++]=-50;
		thisPoints2[pntCnt++]=-50;
		thisPoints2[pntCnt++]=150;
		thisPoints2[pntCnt++]=-50;
		thisPoints2[pntCnt++]=150;
		thisPoints2[pntCnt++]=50;
		thisPoints2[pntCnt++]=-50;
		thisPoints2[pntCnt++]=50;
		tessAddContour(tess, 2, &thisPoints2[0], sizeof(float)*2, 4);
		*/
	
		int hasTriangulated=tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, NULL);
		if(hasTriangulated!=1)
		{
			geom_errors+="TRIANGULATION FAILED (libtess return with failure = no triangulated data is available)";
			continue;
		}
		//AwayJS::Utils::Trace(m_pCallback, "hasTriangulated  = %d\n",hasTriangulated);
		const float* verts	= tessGetVertices(tess);
		const int* vinds	= tessGetVertexIndices(tess);
		const int* elems	= tessGetElements(tess);
		const int nverts	= tessGetVertexCount(tess);
		const int nelems	= tessGetElementCount(tess);	

		
		GEOM::SubGeom* new_subgeom = new SubGeom(awd_project->get_settings());

		BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(filled_region->get_material());

		new_subgeom->set_material(this_mat);

		// different pointers for subgeos, all point to the same subgeo, unless we want to split the shape across multiple subgeos (debug mode)
		GEOM::SubGeom* new_subgeom_inner = new_subgeom;
		GEOM::SubGeom* new_subgeom_outter = new_subgeom;
		GEOM::SubGeom* new_subgeom_concave = new_subgeom;
		GEOM::SubGeom* new_subgeom_convex = new_subgeom;
		GEOM::SubGeom* new_subgeom_intersect = new_subgeom;
		
		// if we want to split the shape into multiple subgeos, we crreate the subgeos, and the needed materials.
		if(awd_project->get_settings()->get_export_shapes_in_debug_mode()){
			new_subgeom_inner = new SubGeom(awd_project->get_settings());
			new_subgeom_outter = new SubGeom(awd_project->get_settings());
			new_subgeom_concave = new SubGeom(awd_project->get_settings());
			new_subgeom_convex = new SubGeom(awd_project->get_settings());
			new_subgeom_intersect = new SubGeom(awd_project->get_settings());
			BLOCKS::Material* this_mat0 = reinterpret_cast<BLOCKS::Material*>(awd_project->get_default_material_by_color(0xffff0000, true, MATERIAL::type::SOLID_COLOR_MATERIAL));
			BLOCKS::Material* this_mat1 = reinterpret_cast<BLOCKS::Material*>(awd_project->get_default_material_by_color(0xff00bdbd, true, MATERIAL::type::SOLID_COLOR_MATERIAL));
			BLOCKS::Material* this_mat2 = reinterpret_cast<BLOCKS::Material*>(awd_project->get_default_material_by_color(0xff00ff00, true, MATERIAL::type::SOLID_COLOR_MATERIAL));
			BLOCKS::Material* this_mat3 = reinterpret_cast<BLOCKS::Material*>(awd_project->get_default_material_by_color(0xff0000ff, true, MATERIAL::type::SOLID_COLOR_MATERIAL));
			BLOCKS::Material* this_mat4 = reinterpret_cast<BLOCKS::Material*>(awd_project->get_default_material_by_color(0xff000000, true, MATERIAL::type::SOLID_COLOR_MATERIAL));
			this_mat0->set_color_components(255,0,0,128);
			this_mat1->set_color_components(0,128,128,128);
			this_mat2->set_color_components(0,255,0,128);
			this_mat3->set_color_components(0,0,255, 128);
			this_mat4->set_color_components(0,0,0,128);
			this_mat0->set_name("Debug_mat_0");
			this_mat1->set_name("Debug_mat_1");
			this_mat2->set_name("Debug_mat_2");
			this_mat3->set_name("Debug_mat_3");
			this_mat4->set_name("Debug_mat_4");
			this_mat0->set_alpha(0.5);
			this_mat1->set_alpha(0.5);
			this_mat2->set_alpha(0.5);
			this_mat3->set_alpha(0.5);
			this_mat4->set_alpha(0.5);
			new_subgeom_intersect->set_material(this_mat0);
			new_subgeom_outter->set_material(this_mat1);
			new_subgeom_concave->set_material(this_mat2);
			new_subgeom_convex->set_material(this_mat3);
			new_subgeom_inner->set_material(this_mat4);
		}
		else{
			// if we do not want to split the shape into multiple subgeos for debug mode, there is still a chance that we want to split between interior and exterior tris. 
			if(awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d()){
				new_subgeom_inner = new SubGeom(awd_project->get_settings());
				new_subgeom_inner->set_material(this_mat);
			}
		}
		

		geom_message  += "\n tesselation result: tricnt: " + std::to_string(nelems);
		geom_message  += " / points in: " + std::to_string(all_pnt_cnt) + "  / points out: " + std::to_string(nverts) + "\n";
		if(all_pnt_cnt!=nverts)
			geom_warning += "\n tesselation changed point-count. points in: " + std::to_string(all_pnt_cnt) + " points out: " + std::to_string(nverts) + "\n";
		
		
		std::vector<GEOM::VECTOR2D> all_verts;
		std::vector<std::vector<GEOM::VECTOR2D> > all_inner_tris;

		for (int i = 0; i < nverts; ++i)
			all_verts.push_back(GEOM::VECTOR2D(verts[i*2], verts[i*2+1]));

		for (int i = 0; i < nelems; ++i)
		{
			const int* p = &elems[i*3];
			
			std::vector<GEOM::VECTOR2D> triangle_vecs(3);
			triangle_vecs[0]=all_verts[p[0]];
			triangle_vecs[1]=all_verts[p[1]];
			triangle_vecs[2]=all_verts[p[2]];
					
			if(((!awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d())&&(!awd_project->get_settings()->get_keep_verticles_in_path_order()))){
				all_inner_tris.push_back(triangle_vecs);
				continue;
			}

			TYPES::INT32 vert_idx0 = vinds[p[0]];
			TYPES::INT32 vert_idx1 = vinds[p[1]];
			TYPES::INT32 vert_idx2 = vinds[p[2]];
			std::vector<GEOM::PathSegment*> path_segs_found;
			
			std::string new_lookup_str;
			if((vert_idx0>=0)&&(vert_idx1>=0)){
				if(vert_idx0>vert_idx1)
					new_lookup_str = std::to_string(vert_idx1) + "#" + std::to_string(vert_idx0);
				else
					new_lookup_str = std::to_string(vert_idx0) + "#" + std::to_string(vert_idx1);
				if(edge_cache.find(new_lookup_str) != edge_cache.end()){
					GEOM::PathSegment* seg_edge = edge_cache[new_lookup_str];	
					seg_edge->set_controlPoint(triangle_vecs[2]);
					path_segs_found.push_back(seg_edge);
				}
			}
			if((vert_idx1>=0)&&(vert_idx2>=0)){
				if(vert_idx1>vert_idx2)
					new_lookup_str = std::to_string(vert_idx2) + "#" + std::to_string(vert_idx1);
				else
					new_lookup_str = std::to_string(vert_idx1) + "#" + std::to_string(vert_idx2);
				if(edge_cache.find(new_lookup_str) != edge_cache.end()){
					GEOM::PathSegment* seg_edge = edge_cache[new_lookup_str];	
					seg_edge->set_controlPoint(triangle_vecs[0]);
					path_segs_found.push_back(seg_edge);
				}				
			}
			if((vert_idx0>=0)&&(vert_idx2>=0)){
				if(vert_idx0>vert_idx2)
					new_lookup_str = std::to_string(vert_idx2) + "#" + std::to_string(vert_idx0);
				else
					new_lookup_str = std::to_string(vert_idx0) + "#" + std::to_string(vert_idx2);
				if(edge_cache.find(new_lookup_str) != edge_cache.end()){
					GEOM::PathSegment* seg_edge = edge_cache[new_lookup_str];	
					seg_edge->set_controlPoint(triangle_vecs[1]);
					path_segs_found.push_back(seg_edge);
				}			
			}
			/*
			*/
				
			std::vector<GEOM::VECTOR2D> new_tri1;
			std::vector<GEOM::VECTOR2D> new_tri2;
			int first_idx=0;
			if (path_segs_found.size()==0){
				// triangle is not connected to a linear-segment. we can export directly as interior
				all_inner_tris.push_back(triangle_vecs);
				continue;
			}
			else if((path_segs_found.size()==1)||(path_segs_found.size()==3)){
				path_segs_found[0]->set_export_this_linear(true);
				if(awd_project->get_settings()->get_exterior_threshold(filled_region->get_type()) > 0.0){
					double distance = abs(distance_point_to_line(path_segs_found[0]->get_startPoint().x, path_segs_found[0]->get_startPoint().y,path_segs_found[0]->get_controlPoint().x, path_segs_found[0]->get_controlPoint().y, path_segs_found[0]->get_endPoint().x, path_segs_found[0]->get_endPoint().y ));
					double size_bias=distance/path_segs_found[0]->get_length();
					if(size_bias>awd_project->get_settings()->get_exterior_threshold(filled_region->get_type())){
						path_segs_found[0]->sub_divide_outside_edge(new_tri1, new_tri2, awd_project->get_settings()->get_exterior_threshold(filled_region->get_type()));
						if(path_segs_found.size()==1){
							all_inner_tris.push_back(new_tri1);
							all_inner_tris.push_back(new_tri2);
							continue;
						}
						first_idx++;
					}
					else{
						if(path_segs_found.size()==3){
							double tempx = (triangle_vecs[0].x + triangle_vecs[1].x + triangle_vecs[2].x)/3;
							double tempy = (triangle_vecs[0].y + triangle_vecs[1].y + triangle_vecs[2].y)/3;
							path_segs_found[0]->set_controlPoint(GEOM::VECTOR2D(tempx, tempy));
							path_segs_found[1]->set_controlPoint(GEOM::VECTOR2D(tempx, tempy));
							path_segs_found[2]->set_controlPoint(GEOM::VECTOR2D(tempx, tempy));
							path_segs_found[1]->set_export_this_linear(true);
							path_segs_found[2]->set_export_this_linear(true);
							continue;
						}
					}
				}
			}
			if(path_segs_found.size()>=2){
				double start_ptx=0;
				double start_pty=0;
				double tempx = 0;
				double tempy = 0;
				
				if(path_segs_found.size()==2){
					tempx = (path_segs_found[first_idx]->get_controlPoint().x + path_segs_found[first_idx+1]->get_controlPoint().x)/2;
					tempy = (path_segs_found[first_idx]->get_controlPoint().y + path_segs_found[first_idx+1]->get_controlPoint().y)/2;
				}
				else{
					tempx = path_segs_found[0]->get_controlPoint().x;
					tempy = path_segs_found[0]->get_controlPoint().y;
				}

				if(awd_project->get_settings()->get_exterior_threshold(filled_region->get_type()) > 0.0){			
					
					if((path_segs_found[first_idx+1]->get_startPoint().x==path_segs_found[first_idx]->get_endPoint().x)&&(path_segs_found[first_idx+1]->get_startPoint().y==path_segs_found[first_idx]->get_endPoint().y)){					
						start_ptx=path_segs_found[first_idx+1]->get_startPoint().x;
						start_pty=path_segs_found[first_idx+1]->get_startPoint().y;						
						if(path_segs_found.size()==2){
							new_tri1.push_back(path_segs_found[first_idx+1]->get_endPoint());
							new_tri1.push_back(path_segs_found[first_idx]->get_startPoint());
						}
						else{							
							new_tri1[2]=path_segs_found[first_idx+1]->get_endPoint();
							new_tri2[0]=path_segs_found[first_idx]->get_startPoint();
						}

					}
					else if ((path_segs_found[first_idx]->get_startPoint().x==path_segs_found[first_idx+1]->get_endPoint().x)&&(path_segs_found[first_idx]->get_startPoint().y==path_segs_found[first_idx+1]->get_endPoint().y)){
						start_ptx=path_segs_found[first_idx]->get_startPoint().x;
						start_pty=path_segs_found[first_idx]->get_startPoint().y;		
						if(path_segs_found.size()==2){
							new_tri1.push_back(path_segs_found[first_idx]->get_endPoint());
							new_tri1.push_back(path_segs_found[first_idx+1]->get_startPoint());	
						}
						else{							
							new_tri1[2]=path_segs_found[first_idx]->get_endPoint();
							new_tri2[0]=path_segs_found[first_idx+1]->get_startPoint();
						}
					}
					else
						int error=0;					

					double tempx2 = 0;
					double tempy2 = 0;
					double combined_length=(path_segs_found[first_idx]->get_length()+path_segs_found[first_idx+1]->get_length())/2;
					double tmpx=tempx-start_ptx;
					double tmpy=tempy-start_pty;
					double distance_points=sqrt((tmpx*tmpx) + (tmpy*tmpy));
					double distance_bias = (distance_points)/(combined_length);
					if(distance_bias>awd_project->get_settings()->get_exterior_threshold(filled_region->get_type())*1.43)
					{
						double target_distance = (awd_project->get_settings()->get_exterior_threshold(filled_region->get_type())*1.43 * combined_length);
						tempx2 = tempx - start_ptx;
						tempy2 = tempy - start_pty;
						double length_new_point=sqrt((tempx2*tempx2)+(tempy2*tempy2));
						tempx2 = tempx2/length_new_point;
						tempy2 = tempy2/length_new_point;
						tempx = start_ptx + target_distance*tempx2;
						tempy = start_pty + target_distance*tempy2;
						if(path_segs_found.size()==2){
							new_tri1.push_back(GEOM::VECTOR2D(tempx, tempy));
							all_inner_tris.push_back(new_tri1);
						}
						else{
							new_tri1[1]=GEOM::VECTOR2D(tempx, tempy);
							new_tri2[1]=GEOM::VECTOR2D(tempx, tempy);
							all_inner_tris.push_back(new_tri1);
							all_inner_tris.push_back(new_tri2);
						}
					}
					else{
						if(path_segs_found.size()>2){
							tempx = path_segs_found[0]->get_controlPoint().x;
							tempy = path_segs_found[0]->get_controlPoint().y;
						}
					}
				}
				path_segs_found[first_idx]->set_controlPoint(GEOM::VECTOR2D(tempx, tempy));
				path_segs_found[first_idx+1]->set_controlPoint(GEOM::VECTOR2D(tempx, tempy));
				path_segs_found[first_idx]->set_export_this_linear(true);
				path_segs_found[first_idx+1]->set_export_this_linear(true);
				continue;
			}
		}
		tessDeleteTess(tess);
		
/// \subsection PathGeoStep6 Step 6: Create and fill SubGeometries.	
///	-	For each FilledRegion, we create a new SubGeometry.\n
///		The SubGeometries can optionally be merged later.
///	-	we want the triangle-list to be in synch with the path-order.
///		e.g. we run over the path and for each segment we add 1 triangle to the list.
///		triangles that are considered to be inner-trianlges (not having any outside edges) are coming last.
		
		int cnt_error_segments=0;
		for(GEOM::Path* one_path: filled_region->get_pathes()){
			for(PathSegment* pathSeg:one_path->get_segments())
			{
				if(pathSeg->get_edgeType()==GEOM::edge_type::OUTTER_EDGE){
					if(((awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d())||(awd_project->get_settings()->get_keep_verticles_in_path_order()))){
						SUBGEOM_ID_STRING subGeo_id;
						if(pathSeg->get_export_this_linear()){
							SUBGEOM_ID_STRING subGeo_id;
							new_subgeom_outter->get_internal_id(subGeo_id);
							new_subgeom_outter->create_triangle(edge_type::OUTTER_EDGE, pathSeg->get_startPoint(), pathSeg->get_endPoint(), pathSeg->get_controlPoint());
						}
						else
							cnt_error_segments++;
					}
				}
				if(pathSeg->get_edgeType()==GEOM::edge_type::CURVED_EDGE)
					bool test=true;	// this means the curve-tppe wasnt calculated correctly. should NEVER happen

				else if((pathSeg->get_edgeType()==GEOM::edge_type::CONCAVE_EDGE) || (pathSeg->get_edgeType()==GEOM::edge_type::CONVEX_EDGE)){					
					for(PathSegment* inner_seg:pathSeg->get_subdivided_path())
					{
						SUBGEOM_ID_STRING subGeo_id;
						if(inner_seg->subdivion_cnt>0){
							new_subgeom_intersect->get_internal_id(subGeo_id);
							new_subgeom_intersect->create_triangle(inner_seg->get_edgeType(), inner_seg->get_startPoint(), inner_seg->get_controlPoint(), inner_seg->get_endPoint());
						}
						else{
							if(inner_seg->get_edgeType()==GEOM::edge_type::CONCAVE_EDGE){
								new_subgeom_concave->get_internal_id(subGeo_id);
								new_subgeom_concave->create_triangle(inner_seg->get_edgeType(), inner_seg->get_startPoint(), inner_seg->get_controlPoint(), inner_seg->get_endPoint());
							}
							else if(inner_seg->get_edgeType()==GEOM::edge_type::CONVEX_EDGE){
								new_subgeom_convex->get_internal_id(subGeo_id);
								new_subgeom_convex->create_triangle(inner_seg->get_edgeType(), inner_seg->get_startPoint(), inner_seg->get_controlPoint(), inner_seg->get_endPoint());
							}
						}
					}
				}
			}
		}
		if(cnt_error_segments>0)
			geom_errors+="ERROR: "+std::to_string(cnt_error_segments)+" linear path segments could not be asociated with a triangle. This segments will not be exported!\n";
					
		for(std::vector<GEOM::VECTOR2D> one_tri : all_inner_tris){
			SUBGEOM_ID_STRING subGeo_id;
			new_subgeom_inner->get_internal_id(subGeo_id);
			new_subgeom_inner->create_triangle(edge_type::INNER_EDGE, one_tri[0], one_tri[1], one_tri[2]);
		}
		

		if((awd_project->get_settings()->get_export_shapes_in_debug_mode())||(awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d())){
			if(new_subgeom_inner->get_tri_cnt()>0)
				geom->add_subgeo(new_subgeom_inner);
		}

		if(awd_project->get_settings()->get_export_shapes_in_debug_mode()){
			if(new_subgeom_outter->get_tri_cnt()>0)
				geom->add_subgeo(new_subgeom_outter);
			if(new_subgeom_concave->get_tri_cnt()>0)
				geom->add_subgeo(new_subgeom_concave);
			if(new_subgeom_convex->get_tri_cnt()>0)
				geom->add_subgeo(new_subgeom_convex);
			if(new_subgeom_intersect->get_tri_cnt()>0)
				geom->add_subgeo(new_subgeom_intersect);
		}
		else{
			if(new_subgeom->get_tri_cnt()>0)
				geom->add_subgeo(new_subgeom);
		}
		has_valid_shape=true;
		region_cnt++;
	}
	// if have not processed anything successfully, we add a error on the geometry and set it to be invalid
	if(!has_valid_shape){
		geom_errors += "No valid regions - no valid geometry created\n";		
		geom->set_state(state::INVALID);
	}
	geom->add_message(geom_message, TYPES::message_type::STATUS_MESSAGE);
	geom->add_message(geom_warning, TYPES::message_type::WARNING_MESSAGE);
	geom->add_message(geom_errors, TYPES::message_type::ERROR_MESSAGE);

	return result::AWD_SUCCESS;
}
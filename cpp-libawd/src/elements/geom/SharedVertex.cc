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

SharedVertex3D::SharedVertex3D()
{
}

SharedVertex3D::~SharedVertex3D()
{
}

void
SharedVertex3D::add_vert(Vertex3D* vert)
{
	for(Vertex3D* existing_vert:this->verts){
		if(vert==existing_vert)
			return;
	}
	this->verts.push_back(vert);
}

void
SharedVertex3D::calculate_normals()
{
	geom_smoothing_mode geo_smooth_mode=geom_smoothing_mode::SMOOTH_NOTHING;

	/// __Step1:__ 
	///	If we have set to use facenormals instead of vertex, we overwrite vertex-normals with face-normals.\n
	for(Vertex3D* vert : this->verts){
		vertex_normals_mode vert_normal_mode=vert->get_v_n_settings()->get_vertex_normals_mode();
		if((vert_normal_mode==vertex_normals_mode::EXPLICIT_FACE_NORMALS)||(vert_normal_mode==vertex_normals_mode::PHONG_FACE_NORMALS)){
			vert->set_normal(vert->get_face_normal());
		}
	}
	/// if the Geometry is set to SMOOTH_NOTHING, we can return instantly\n
	if(geo_smooth_mode==geom_smoothing_mode::SMOOTH_NOTHING)
		return;
	
	/// __Step2:__ \n
	/// For each verex assigned to this SharedVertex, we need to decide with which other Vertices inside this SharedVertex it should merge their vertex-normals.
	/// If we find any Verts that should merge the normal with this vertex, we merge the normals and store the resulting VECTOR3D in a list.
	/// We do not want to change any normals on the vertices, because we want to calculate the shared normal indepentant for each vertex
	std::vector<VECTOR3D> new_normals;
	for(Vertex3D* vert:this->verts){
		// find all verts that should share the normal with this one
		BlockSettings* vn_settings=vert->get_v_n_settings();
		bool use_phong_breaks=vn_settings->get_use_phong_breaks();
		bool use_smooth_groups=vn_settings->get_use_smoothing_groups();
		TYPES::F64 threshold=vn_settings->get_normal_threshold();
		vertex_normals_mode v_n__mode=vn_settings->get_vertex_normals_mode();
		VECTOR3D normal=vert->get_normal();
		
		// this will contain the new vertex-normal. we dont want to change any normals, before all normals have been calculated
		VECTOR3D new_shared_normal=VECTOR3D(normal.x, normal.y, normal.z);
		new_normals.push_back(new_shared_normal);
		TYPES::UINT32 shared_cnt=1;

		if((v_n__mode==vertex_normals_mode::EXPLICIT_FACE_NORMALS)||(v_n__mode==vertex_normals_mode::EXPLICIT_FACE_NORMALS)){
			// this vertex should never share its normal with other vertex.
			// it might be merged later, but its normal should never be affected.
			continue;
		}
		// this vertex possible should share normals with other vertex. we need to traverse the vert-list		
		for(Vertex3D* comp_vert:this->verts){
			if(vert==comp_vert)
				continue; // do nothing if objects are the same
			BlockSettings* comp_vn_settings=comp_vert->get_v_n_settings();
			vertex_normals_mode comp_v_n__mode=comp_vn_settings->get_vertex_normals_mode();
			if((comp_v_n__mode==vertex_normals_mode::EXPLICIT_FACE_NORMALS)||(comp_v_n__mode==vertex_normals_mode::EXPLICIT_FACE_NORMALS)){
				// this vertex should never share its normal with other vertex.
				// it might be merged later, but its normal should never be affected, so we look at next vert
				continue;
			}
			// optional compare the FaceGroup-id of both Vertex. If they differ, we skip to next
			if(geo_smooth_mode==geom_smoothing_mode::BREAK_WITH_FACE_GROUP){
				if(vert->get_face_group_id()!=comp_vert->get_face_group_id())
					continue;
			}
			// optional compare the Material-id of both Vertex. If they differ, we skip to next. This only needs to be checked if FaceGroup-id wasnt checked, because Material
			else if(geo_smooth_mode==geom_smoothing_mode::BREAK_WITH_MATERIAL){
				if(vert->get_subgeom_id()!=comp_vert->get_subgeom_id())
					continue;
			}
			// optional check for phong breaks that have been set on this geometry
			if((use_phong_breaks)||(comp_vn_settings->get_use_phong_breaks())){
				//todo: create a stream-attr for this purpose that can be added to vertex (optional added via tri or edge). If this attr is positive defined for both vertex, they cannot share normals
			}
			// optional check for smoothing groups that have been set on this geometry
			if((use_smooth_groups)||(comp_vn_settings->get_use_smoothing_groups())){
				//todo: create a stream-attr for this purpose that can be added to vertex (optional added via tri or edge). Only if this attr is defined for both vertex, they can share normals
			}
			// if we got to here, the vertexnormals need to be compared by angle and treshold.

			// if both verts have different threshold set, we use the mixed version	of both		
			TYPES::F64 use_threshold=threshold;
			if(comp_vn_settings->get_normal_threshold()!=use_threshold)
				use_threshold=(comp_vn_settings->get_normal_threshold() + threshold)/2;

			// if threshold is smaller or equal 0, the calculation will always return not to share the normal
			if(use_threshold<=0)
				continue;
			VECTOR3D normal2=vert->get_normal();
			// \todo If both normals are exactly the same, do we add another weight on it (add it again to shared normals list)?
			if (normal.x==normal2.x && normal.y==normal2.y && normal.z==normal2.z) {
				continue;
			}
			
			double angle;
			double l0, l1;
			// Calculate lenghts (usually 1.0)
			l0 = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
			l1 = sqrt(normal2.x*normal2.x + normal2.y*normal2.y + normal2.z*normal2.z);
			// Calculate angle and compare to threshold
			angle = acos((normal.x*normal2.x + normal.y*normal2.y + normal.z*normal2.z) / (l0*l1));
			if (angle <= use_threshold) {
				// the normal should be merged
				new_shared_normal.x += normal2.x;
				new_shared_normal.y += normal2.y;
				new_shared_normal.z += normal2.z;
				shared_cnt++;
			}
			else {
				continue;
			}
		}		
		new_shared_normal.x /= shared_cnt;
		new_shared_normal.y /= shared_cnt;
		new_shared_normal.z /= shared_cnt;
	}
	/// __Step3:__ \n
	/// once we have calculated the shared normal for all vertices, we can set them on the vertex.
	TYPES::UINT32 cnt = 0;
	for(Vertex3D* vert:this->verts){
		vert->set_normal(new_normals[cnt]);
		cnt++;
	}

}
void
SharedVertex3D::connect_duplicates()
{
	for(Vertex3D* vert:this->verts){
		if(vert->get_target_vertex()==NULL){
			for(Vertex3D* compare_vert:this->verts){
				if(vert->get_target_vertex()==NULL){
					if(vert->compare(compare_vert)){
						compare_vert->set_target_vertex(vert);
					}
				}
			}
		}
	}
	return;
}

std::vector<Vertex3D*>&
SharedVertex3D::get_verts()
{
	return this->verts;
}

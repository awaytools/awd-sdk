#include "elements/stream_attributes.h"
#include "elements/geom_elements.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "utils/util.h"
#include <stdlib.h>

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;

Path::Path()
{
	this->is_dirty=true;
	this->is_hole = false;
	this->has_curves=false;
	// just malloc some memory on all the internal stuff, so we can always call free before setting them
	// alternative would be to keep track if the memory had been malloc or not.
	polyX = (float*)malloc(sizeof(float));
	polyY = (float*)malloc(sizeof(float));
	constant = (float*)malloc(sizeof(float));
	multiple = (float*)malloc(sizeof(float));
	path_seg_indicies = (int*)malloc(sizeof(int));
	path_seg_indicies_inner = (int*)malloc(sizeof(int));
}

Path::~Path()
{
	free(polyX);
	free(polyY);
	free(constant);
	free(multiple);
	free(path_seg_indicies);
	free(path_seg_indicies_inner);
}

bool	
Path::get_has_curves()
{
	return this->has_curves;
}
bool	
Path::get_is_hole()
{
	return this->is_hole;
}

bool 
Path::check_boundingbox(double max2_x, double max2_y,double min2_x, double min2_y)
{
	if(this->min_x>max2_x)
		return false;
	if(this->max_x<min2_x)
		return false;
	if(this->min_y>max2_y)
		return false;
	if(this->max_y<min2_y)
		return false;
	return true;
}

void	
Path::set_is_hole(bool is_hole)
{
	this->is_hole=is_hole;
}
result	
Path::add_segment_fonts(GEOM::PathSegment* new_segment)
{
	this->path_segments.push_back(new_segment);
	return result::AWD_SUCCESS;
}
result	
Path::add_segment(GEOM::PathSegment* new_segment)
{
	if(this->path_segments.size()>0){
		if( (this->path_segments.back()->get_endPoint().x != new_segment->get_startPoint().x) || (this->path_segments.back()->get_endPoint().y != new_segment->get_startPoint().y)  )
			return result::AWD_ERROR;
	}
	if(new_segment->get_edgeType()!=edge_type::OUTTER_EDGE)
		this->has_curves=true;
	this->path_segments.push_back(new_segment);
	return result::AWD_SUCCESS;
}

result 
Path::validate_path()
{
	if(this->path_segments.size()<3)
		return result::PATH_CONTAINS_LESS_THAN_THREE_SEGMENTS;

	if((this->path_segments.front()->get_startPoint().x != this->path_segments.back()->get_endPoint().x)||(this->path_segments.front()->get_startPoint().y != this->path_segments.back()->get_endPoint().y)){
		GEOM::PathSegment* new_segment = new GEOM::PathSegment();
		new_segment->set_startPoint(GEOM::VECTOR2D(this->path_segments.back()->get_endPoint().x, this->path_segments.back()->get_endPoint().y));
		new_segment->set_endPoint(GEOM::VECTOR2D(this->path_segments.front()->get_startPoint().x, this->path_segments.front()->get_startPoint().y));
		return result::PATH_NOT_CLOSING;
	}

	this->max_x=std::numeric_limits<int>::max()*-1;
	this->max_y=std::numeric_limits<int>::max()*-1;
	this->min_x=std::numeric_limits<int>::max();
	this->min_y=std::numeric_limits<int>::max();
	int segcnt = 0;

	PathSegment* prev_seg = this->path_segments.back();

	for(PathSegment* path_seg : this->path_segments){
		//path_seg->set_parent_path(this);
		prev_seg->set_next(path_seg);
		path_seg->set_last(prev_seg);
		prev_seg = path_seg;
		path_seg->calculate_bounds();
				// update the bounds:
		if(max_x<path_seg->max_x)
			max_x=ceil(path_seg->max_x);
		if(min_x>path_seg->min_x)
			min_x=floor(path_seg->min_x);
		if(max_y<path_seg->max_y)
			max_y=ceil(path_seg->max_y);
		if(min_y>path_seg->min_y)
			min_y=floor(path_seg->min_y);
	}
	return result::AWD_SUCCESS;
}

std::string& 
Path::get_id()
{
	return this->id;
}

void 
Path::set_id(std::string& id)
{
	this->id=id;
}
std::vector<GEOM::PathSegment*>& 
Path::get_segments()
{
	return this->path_segments;
}

void 
Path::prepare(GEOM::edge_type edgetype) {

	if(this->is_dirty){
		polyCorners = get_point_count(edgetype);
		int i, j=polyCorners-1;
		free(polyX);
		free(polyY);
		free(constant);
		free(multiple);
		polyX = (float*)malloc(sizeof(float)*polyCorners);
		polyY = (float*)malloc(sizeof(float)*polyCorners);
		constant = (float*)malloc(sizeof(float)*polyCorners);
		multiple = (float*)malloc(sizeof(float)*polyCorners);
		int pcntX=0;
		int pcntY=0;
		int seg_cnt=0;
		int seg_cnt_inner=0;
		bool export_curve=false;
		for(PathSegment* one_seg: this->path_segments){			
			for(PathSegment* one_inner_seg:one_seg->get_subdivided_path()){
				polyX[pcntX++]=one_inner_seg->get_startPoint().x;
				polyY[pcntY++]=one_inner_seg->get_startPoint().y;
				export_curve=false;
				if((one_inner_seg->get_edgeType()==edge_type::CONCAVE_EDGE)&&((edgetype==edge_type::CURVED_EDGE)||(edgetype==edge_type::CONCAVE_EDGE)))
					export_curve=true;
				else if((one_inner_seg->get_edgeType()==edge_type::CONVEX_EDGE)&&((edgetype==edge_type::CURVED_EDGE)||(edgetype==edge_type::CONVEX_EDGE)))
					export_curve=true;
				if(export_curve){
					polyX[pcntX++]=one_inner_seg->get_controlPoint().x;
					polyY[pcntY++]=one_inner_seg->get_controlPoint().y;
				}
			}
		}
		for(i=0; i<polyCorners; i++) {
			if(polyY[j]==polyY[i]) {
				constant[i]=polyX[i];
				multiple[i]=0; }
			else {
				constant[i]=polyX[i]-(polyY[i]*polyX[j])/(polyY[j]-polyY[i])+(polyY[i]*polyX[i])/(polyY[j]-polyY[i]);
				multiple[i]=(polyX[j]-polyX[i])/(polyY[j]-polyY[i]); }
				j=i;
		}
	}
	this->is_dirty=false;
}

bool 
Path::point_inside_path_bool(float x, float y) {

	int i;
	int j=polyCorners-1;
	bool inPath=false;
	
	for (i=0; i<polyCorners; i++) {
		if (((polyY[i]< y && polyY[j]>=y) || (polyY[j]< y && polyY[i]>=y))) {
				inPath^=(y*multiple[i]+constant[i]<x);
		}
		j=i; 
	}
  /*
	for (i=0; i<polyCorners; i++) {
		if (((polyY[i]< y && polyY[j]>=y) || (polyY[j]< y && polyY[i]>=y)) &&  (polyX[i]<=x || polyX[j]<=x)) {
			if ((polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i]))<x)
				oddNodes=!oddNodes; 
		}		
		j=i; 
	}
	*/
	return inPath;
}
int 
Path::point_inside_path(float x, float y) {

	int i;
	int j=polyCorners-1;
	int cnt_intersects=0;
	
	for (i=0; i<polyCorners; i++) {
		if (((polyY[i]< y && polyY[j]>=y) || (polyY[j]< y && polyY[i]>=y))) {
			if(y*multiple[i]+constant[i]<x){
				cnt_intersects++;
			}
		}
		j=i; 
	}
  /*
	for (i=0; i<polyCorners; i++) {
		if (((polyY[i]< y && polyY[j]>=y) || (polyY[j]< y && polyY[i]>=y)) &&  (polyX[i]<=x || polyX[j]<=x)) {
			if ((polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i]))<x)
				oddNodes=!oddNodes; 
		}		
		j=i; 
	}
	*/
	return cnt_intersects;
}


/**
\brief Calculate the type of curve for all curved segments.

*/
result 
Path::get_curve_types(Settings* settings)
{		
	prepare(edge_type::OUTTER_EDGE);
	result res = result::AWD_SUCCESS;
	for(PathSegment* one_seg: this->path_segments){
//		If the edge-type is not OUTTER_EDGE, we need to figure out if it is concave or convex.\n	
		if(one_seg->get_edgeType()!=GEOM::edge_type::OUTTER_EDGE){	

//			Test if the test-point is inside the path or not.
//			The test-point is the control-point moved towards the segments-line. (if we test for the control-point, we might get errors for pathes that are nearly "self-touching".
//			For self-intersecting pathes, this test might fail.
			bool is_convex = point_inside_path_bool(one_seg->get_test_point().x, one_seg->get_test_point().y);

//			In case this path should be considered a hole, we invert the result coming from the "PointInPath" function. 	
			if(this->is_hole)
				is_convex=!is_convex;

///			Now we can set the edge-type on the PathSegment
			if(is_convex){
				one_seg->set_edgeType(GEOM::edge_type::CONVEX_EDGE);
				one_seg->get_subdivided_path()[0]->set_edgeType(GEOM::edge_type::CONVEX_EDGE);
			}
			else{
				one_seg->set_edgeType(GEOM::edge_type::CONCAVE_EDGE);
				one_seg->get_subdivided_path()[0]->set_edgeType(GEOM::edge_type::CONCAVE_EDGE);
			}
		}
	}
	return res;
}

void 
Path::make_linear()
{
	for(PathSegment* pathSeg:this->path_segments)
	{		
		for(PathSegment* one_inner_seg:pathSeg->get_subdivided_path())
		{
			one_inner_seg->set_edgeType(edge_type::OUTTER_EDGE);
		}
	}
}

result
Path::resolve_curve_intersections(Settings* settings,  std::string& message)
{
	message="";
	return result::AWD_SUCCESS;
}
void 
Path::make_dirty(){
	this->is_dirty=true;
}
result
Path::resolve_curve_intersections_contour_vs_holes(Settings* settings,  std::string& message, std::vector<GEOM::Path*> all_pathes)
{
	result res = result::AWD_SUCCESS;
	message="";	
	TYPES::UINT32 path_cnt = 0;
	TYPES::UINT32 subdivison_hole_cnt = 0;
	TYPES::UINT32 subdivison_contour_cnt = 0;
	for(GEOM::Path* one_path: all_pathes){
		path_cnt++;
		if(path_cnt>1){
			// this is a hole path: we need to make sure that the control points of all convex curves are inside the contour-path.\n
			// if the control point of a convex curve of a hole-path is not inside the contour-path, we have a intersection that we should be solveable by subdividing the curve of the hole path.\n
			subdivison_hole_cnt=0;
			// reset the state for all convex path-segments of the hole-path
			for(PathSegment* seg:one_path->get_segments()){
				if(seg->get_edgeType()==edge_type::CONVEX_EDGE){
					if(seg->get_state()!=edge_state::MAX_SUBDIVISION){
						if(seg->get_state()==edge_state::SUBDIVIDED_RESOLVED){
							seg->set_state(edge_state::SUBDIVIDED);
						}
						else if(seg->get_state()==edge_state::NOT_INTERSECTING){
							seg->set_state(edge_state::TEST_INTERSECTING);
						}
						for(PathSegment* inner_seg:seg->get_subdivided_path()){
							if(inner_seg->get_state()!=edge_state::MAX_SUBDIVISION){
								if(inner_seg->get_state()==edge_state::SUBDIVIDED_RESOLVED){
									inner_seg->set_state(edge_state::SUBDIVIDED);
								}
								else if(inner_seg->get_state()==edge_state::NOT_INTERSECTING){
									inner_seg->set_state(edge_state::TEST_INTERSECTING);
								}
							}
						}
					}
				}
			}

			// reset the state for all convex path-segments of the contour-path (need to reset the contour path for each hole!)
			for(PathSegment* seg:all_pathes[0]->get_segments()){
				if(seg->get_edgeType()==edge_type::CONVEX_EDGE){
					if(seg->get_state()!=edge_state::MAX_SUBDIVISION){
						if(seg->get_state()==edge_state::SUBDIVIDED_RESOLVED){
							seg->set_state(edge_state::SUBDIVIDED);
						}
						else if(seg->get_state()==edge_state::NOT_INTERSECTING){
							seg->set_state(edge_state::TEST_INTERSECTING);
						}
						for(PathSegment* inner_seg:seg->get_subdivided_path()){
							if(inner_seg->get_state()!=edge_state::MAX_SUBDIVISION){
								if(inner_seg->get_state()==edge_state::SUBDIVIDED_RESOLVED){
									inner_seg->set_state(edge_state::SUBDIVIDED);
								}
								else if(inner_seg->get_state()==edge_state::NOT_INTERSECTING){
									inner_seg->set_state(edge_state::TEST_INTERSECTING);
								}
							}
						}
					}
				}
			}

			// we keep repeating the process for this hole vs contour, until either the max_iteration is reached, or if we cannot find any more intersections.	
			int iteration_cnt=0;
			bool intersection_found=true;
			while((iteration_cnt<settings->get_max_iterations())&&(intersection_found)){
				intersection_found=false;
				
				// prepare the path includes control-points for convex curves.
				this->prepare(edge_type::CONVEX_EDGE);

				for(PathSegment* seg:one_path->get_segments()){
					if(seg->get_edgeType()==edge_type::CONVEX_EDGE){
						if((seg->get_state()==edge_state::MAX_SUBDIVISION)||(seg->get_state()==edge_state::NOT_INTERSECTING))
							continue;
						else if(seg->get_state()==edge_state::SUBDIVIDED)
							seg->set_state(edge_state::SUBDIVIDED_RESOLVED);
						else if(seg->get_state()==edge_state::TEST_INTERSECTING)
							seg->set_state(edge_state::NOT_INTERSECTING);

						for(PathSegment* inner_seg:seg->get_subdivided_path()){
							if((inner_seg->get_state()==edge_state::MAX_SUBDIVISION)||(inner_seg->get_state()==edge_state::NOT_INTERSECTING))
								continue;						
							bool is_inside_path = point_inside_path(inner_seg->get_controlPoint().x, inner_seg->get_controlPoint().y);
							if(is_inside_path){
								if(inner_seg->get_state()==edge_state::SUBDIVIDED)
									inner_seg->set_state(edge_state::SUBDIVIDED_RESOLVED);
								else if(inner_seg->get_state()==edge_state::TEST_INTERSECTING)
									inner_seg->set_state(edge_state::NOT_INTERSECTING);
								continue;
							}
							intersection_found=true;
							inner_seg->set_state(edge_state::SUBDIVIDED);
							seg->set_state(edge_state::SUBDIVIDED);
							subdivison_hole_cnt++;
							//	point is not in path. this must be a intersection.
						}
						if(seg->get_state()==edge_state::SUBDIVIDED){
							seg->subdividePath(settings);
							one_path->make_dirty();
							//
						}
					}
				}		
				/*
				one_path->prepare(edge_type::CONVEX_EDGE);
				// next step is to make sure that the control-points of convex curves of the contour-path are all outside of the hole-path
				// if the control point of a convex curve is not inside the hole-path, we have a intersection that we should be solveable by subdividing the curve of the contour path.
				for(PathSegment* seg:all_pathes[0]->get_segments()){
					if(seg->get_edgeType()==edge_type::CONVEX_EDGE){
						if((seg->get_state()==edge_state::MAX_SUBDIVISION)||(seg->get_state()==edge_state::NOT_INTERSECTING))
							continue;
						else if(seg->get_state()==edge_state::SUBDIVIDED)
							seg->set_state(edge_state::SUBDIVIDED_RESOLVED);
						else if(seg->get_state()==edge_state::TEST_INTERSECTING)
							seg->set_state(edge_state::NOT_INTERSECTING);

						for(PathSegment* inner_seg:seg->get_subdivided_path()){
							if((inner_seg->get_state()==edge_state::MAX_SUBDIVISION)||(inner_seg->get_state()==edge_state::NOT_INTERSECTING))
								continue;						
							bool is_inside_path = one_path->point_inside_path(inner_seg->get_controlPoint().x, inner_seg->get_controlPoint().y);
							if(is_inside_path){
								inner_seg->set_state(edge_state::SUBDIVIDED_RESOLVED);
								continue;
							}
							intersection_found=true;
							inner_seg->set_state(edge_state::SUBDIVIDED);
							seg->set_state(edge_state::SUBDIVIDED);
							subdivison_contour_cnt++;
							//	point is not in path. this must be a intersection.
						}
						if(seg->get_state()==edge_state::SUBDIVIDED){
							seg->subdividePath(settings);
							all_pathes[0]->make_dirty();
						}
					}
				}
				*/
				iteration_cnt++;
				if((iteration_cnt==settings->get_max_iterations())&&(intersection_found)){
					message+="ERROR: Intersections for Contour vs hole nr " + std::to_string(path_cnt) + " could not be resolved within max iterations";
					res=result::AWD_ERROR;
				}
			}
			if((subdivison_contour_cnt>0)||(subdivison_hole_cnt>0)){
				res=result::SUBDIVIDED_PATHES;
				if(subdivison_contour_cnt>0)
					message+="Contour vs Hole "+std::to_string(path_cnt)+" divided "+std::to_string(subdivison_contour_cnt)+" curves to resolve intersections";
				if(subdivison_hole_cnt>0)
					message+="Hole "+std::to_string(path_cnt)+" vs Contour - divided "+std::to_string(subdivison_hole_cnt)+" curves to resolve intersections";
			}

		}
	}
	return res;
}

result
Path::resolve_curve_intersections_hole_vs_holes(Settings* settings,  std::string& message, std::vector<GEOM::Path*> all_pathes)
{
	message="";
	return result::AWD_SUCCESS;
}


TYPES::UINT32 
Path::get_point_count(GEOM::edge_type edgetype)
{	
	if(edgetype==edge_type::OUTTER_EDGE)
		return this->path_segments.size();
	
	TYPES::UINT32 point_cnt=0;
	for(PathSegment* pathSeg:this->path_segments)
	{		
		for(PathSegment* one_inner_seg:pathSeg->get_subdivided_path())
		{
			point_cnt++;	
			if((one_inner_seg->get_edgeType()==edge_type::CONCAVE_EDGE)&&((edgetype==edge_type::CURVED_EDGE)||(edgetype==edge_type::CONCAVE_EDGE)))
				point_cnt++;
			else if((one_inner_seg->get_edgeType()==edge_type::CONVEX_EDGE)&&((edgetype==edge_type::CURVED_EDGE)||(edgetype==edge_type::CONVEX_EDGE)))
				point_cnt++;				
		}
	}
	return point_cnt;
}


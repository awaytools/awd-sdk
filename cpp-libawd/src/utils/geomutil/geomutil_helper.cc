
#include "utils/awd_types.h"
#include "blocks/geometry.h"
#include "blocks/material.h"
#include "elements/subgeometry.h"
#include "elements/geom_elements.h"
#include "blocks/mesh_inst.h"
#include "utils/util.h"
#include "utils/settings.h"

#include "tesselator.h"

using namespace AWD;
using namespace AWD::BASE;
using namespace AWD::BLOCKS;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::SETTINGS;


void
GEOM::resolve_line_curve_intersection(PathSegment* curve, PathSegment* line, SETTINGS::Settings* settings)
{
	bool isResolved=false;
	int iterCnt=0;
	for(PathSegment* subSeg:curve->get_subdivided_path()){
		if(subSeg->get_state()!=edge_state::MAX_SUBDIVISION)				
			subSeg->set_state(edge_state::TEST_INTERSECTING);
	}
	VECTOR2D startpoint=line->get_startPoint();
	VECTOR2D endPoint=line->get_endPoint();
	VECTOR2D startpoint_curve;
	VECTOR2D endPoint_curve;
	VECTOR2D controlPoint_curve;
	while((!isResolved)&&(iterCnt<settings->get_max_iterations())){
		iterCnt++;
		isResolved=true;
		for(PathSegment* subSeg:curve->get_subdivided_path()){
			if((subSeg->get_state()==edge_state::NOT_INTERSECTING)||(subSeg->get_state()==edge_state::SUBDIVIDED_RESOLVED)||(subSeg->get_state()==edge_state::MAX_SUBDIVISION))
				continue;
			bool isIntersect=bounds_intersect(subSeg->min_x, subSeg->max_x, subSeg->max_y, subSeg->max_y, line->min_x, line->max_x, line->max_y, line->max_y);
			if(isIntersect){
				startpoint_curve=curve->get_startPoint();
				endPoint_curve=curve->get_endPoint();
				controlPoint_curve=curve->get_controlPoint();
				bool intersects_edge=line_intersect(startpoint.x, startpoint.y, endPoint.x, endPoint.y, startpoint_curve.x, startpoint_curve.y, endPoint_curve.x, endPoint_curve.y);
				bool intersects_curve1=line_intersect(startpoint.x, startpoint.y, endPoint.x, endPoint.y, startpoint_curve.x, startpoint_curve.y, controlPoint_curve.x, controlPoint_curve.y);
				bool intersects_curve2=line_intersect(startpoint.x, startpoint.y, endPoint.x, endPoint.y, endPoint_curve.x, endPoint_curve.y, controlPoint_curve.x, controlPoint_curve.y);
				
				// if a line cuts the baseline of a curve, and a curved line, it is not solveable by subdividing
				if((intersects_edge)&&((intersects_curve1)||(intersects_curve2)))
					subSeg->set_state(edge_state::NOT_INTERSECTING);
				
				else if ((intersects_curve1)||(intersects_curve2)||(intersects_curve2)){
					subSeg->set_state(edge_state::SUBDIVIDED);
					isResolved=false;
				}
				else{
					// no line intersection. line might be completly in triangle. check by testing if the startpoint of line is inside the triangle.
					// if it is inside triangle, and lines are not intersecting, both points must be inside the triangle.
					TYPES::UINT32 in_triangle = PointInTri(startpoint.x, startpoint.y, startpoint_curve.x, startpoint_curve.y, controlPoint_curve.x, controlPoint_curve.y, endPoint_curve.x, endPoint_curve.y);
					if(in_triangle==1){
						subSeg->set_state(edge_state::SUBDIVIDED);
						isResolved=false;
					}
					else{
						subSeg->set_state(edge_state::NOT_INTERSECTING);
					}
				}
			}
			else{
				subSeg->set_state(edge_state::NOT_INTERSECTING);
			}
		}
		if(!isResolved){
			curve->subdividePath(settings);
			if(iterCnt==settings->get_max_iterations())
				iterCnt=settings->get_max_iterations();
		}
			
	}

}
void
GEOM::resolve_curve_curve_intersection(PathSegment* curve, PathSegment* curve_2, SETTINGS::Settings* settings)
{
	bool isResolved=false;
	int iterCnt=0;
	for(PathSegment* subSeg:curve->get_subdivided_path()){
		if(subSeg->get_state()!=edge_state::MAX_SUBDIVISION)				
			subSeg->set_state(edge_state::TEST_INTERSECTING);
	}
	for(PathSegment* subSeg:curve_2->get_subdivided_path()){
		if(subSeg->get_state()!=edge_state::MAX_SUBDIVISION)				
			subSeg->set_state(edge_state::TEST_INTERSECTING);
	}
	VECTOR2D startpoint_curve;
	VECTOR2D endPoint_curve;
	VECTOR2D controlPoint_curve;
	VECTOR2D startpoint_curve2;
	VECTOR2D endPoint_curve2;
	VECTOR2D controlPoint_curve2;
	while((!isResolved)&&(iterCnt<settings->get_max_iterations())){
		iterCnt++;
		isResolved=true;
		for(PathSegment* subSeg:curve->get_subdivided_path()){
			if(subSeg->get_state()!=edge_state::TEST_INTERSECTING)
				continue;
			startpoint_curve=subSeg->get_startPoint();
			endPoint_curve=subSeg->get_endPoint();
			controlPoint_curve=subSeg->get_controlPoint();
			subSeg->calculate_bounds();
			for(PathSegment* subSeg2:curve_2->get_subdivided_path()){
				if(subSeg2->get_state()!=edge_state::TEST_INTERSECTING)
					continue;
				subSeg2->calculate_bounds();
				if(bounds_intersect(subSeg->min_x, subSeg->max_x, subSeg->min_y, subSeg->max_y, subSeg2->min_x, subSeg2->max_x, subSeg2->min_y, subSeg2->max_y)){
					startpoint_curve2=subSeg2->get_startPoint();
					endPoint_curve2=subSeg2->get_endPoint();
					controlPoint_curve2=subSeg2->get_controlPoint();
					TYPES::UINT32 startpoint2_in_triangle1 = PointInTri(startpoint_curve2.x, startpoint_curve2.y, startpoint_curve.x, startpoint_curve.y, controlPoint_curve.x, controlPoint_curve.y, endPoint_curve.x, endPoint_curve.y);
					if (startpoint2_in_triangle1==1){
						isResolved=false;
						subSeg->set_state(edge_state::SUBDIVIDED);
						subSeg2->set_state(edge_state::SUBDIVIDED);
						if(subSeg->get_size()>=subSeg2->get_size())
							subSeg->set_state(edge_state::SUBDIVIDED);
						else
							subSeg2->set_state(edge_state::SUBDIVIDED);
						continue;
					}
					TYPES::UINT32 endpoint2_in_triangle1 = PointInTri(endPoint_curve2.x, endPoint_curve2.y, startpoint_curve.x, startpoint_curve.y, controlPoint_curve.x, controlPoint_curve.y, endPoint_curve.x, endPoint_curve.y);
					if (endpoint2_in_triangle1==1){
						isResolved=false;
						subSeg->set_state(edge_state::SUBDIVIDED);
						subSeg2->set_state(edge_state::SUBDIVIDED);
						if(subSeg->get_size()>=subSeg2->get_size())
							subSeg->set_state(edge_state::SUBDIVIDED);
						else
							subSeg2->set_state(edge_state::SUBDIVIDED);
						continue;
					}
					TYPES::UINT32 curvepoint2_in_triangle1 = PointInTri(controlPoint_curve2.x, controlPoint_curve2.y, startpoint_curve.x, startpoint_curve.y, controlPoint_curve.x, controlPoint_curve.y, endPoint_curve.x, endPoint_curve.y);
					if (curvepoint2_in_triangle1==1){
						isResolved=false;
						subSeg->set_state(edge_state::SUBDIVIDED);
						subSeg2->set_state(edge_state::SUBDIVIDED);
						if(subSeg->get_size()>=subSeg2->get_size())
							subSeg->set_state(edge_state::SUBDIVIDED);
						else
							subSeg2->set_state(edge_state::SUBDIVIDED);
						continue;
					}
					TYPES::UINT32 startpoint1_in_triangle2 = PointInTri(startpoint_curve.x, startpoint_curve.y, startpoint_curve2.x, startpoint_curve2.y, controlPoint_curve2.x, controlPoint_curve2.y, endPoint_curve2.x, endPoint_curve2.y);
					if (startpoint1_in_triangle2==1){
						isResolved=false;
						subSeg->set_state(edge_state::SUBDIVIDED);
						subSeg2->set_state(edge_state::SUBDIVIDED);
						if(subSeg->get_size()>=subSeg2->get_size())
							subSeg->set_state(edge_state::SUBDIVIDED);
						else
							subSeg2->set_state(edge_state::SUBDIVIDED);
						continue;
					}
					TYPES::UINT32 endpoint1_in_triangle2 = PointInTri(endPoint_curve.x, endPoint_curve.y, startpoint_curve2.x, startpoint_curve2.y, controlPoint_curve2.x, controlPoint_curve2.y, endPoint_curve2.x, endPoint_curve2.y);
					if (endpoint1_in_triangle2==1){
						isResolved=false;
						subSeg->set_state(edge_state::SUBDIVIDED);
						subSeg2->set_state(edge_state::SUBDIVIDED);
						if(subSeg->get_size()>=subSeg2->get_size())
							subSeg->set_state(edge_state::SUBDIVIDED);
						else
							subSeg2->set_state(edge_state::SUBDIVIDED);
						continue;
					}
					TYPES::UINT32 curvepoint1_in_triangle2 = PointInTri(controlPoint_curve.x, controlPoint_curve.y, startpoint_curve2.x, startpoint_curve2.y, controlPoint_curve2.x, controlPoint_curve2.y, endPoint_curve2.x, endPoint_curve2.y);
					if (curvepoint1_in_triangle2==1){
						isResolved=false;
						subSeg->set_state(edge_state::SUBDIVIDED);
						subSeg2->set_state(edge_state::SUBDIVIDED);
						if(subSeg->get_size()>=subSeg2->get_size())
							subSeg->set_state(edge_state::SUBDIVIDED);
						else
							subSeg2->set_state(edge_state::SUBDIVIDED);
						continue;
					}
				}
				else{
					subSeg->set_state(edge_state::NOT_INTERSECTING);
				}
			}
		}
		if(!isResolved){
			curve->subdividePath(settings);
			curve_2->subdividePath(settings);
			// if this was last iterations, we might have intersections left
			//if(iterCnt==settings->get_max_iterations())
		}
			
	}
}

result GEOM::update_simplified_path(std::vector<double>& simplyifed_path, std::vector<PathSegment*>& path)
{
	simplyifed_path.clear();
	for(PathSegment* one_seg: path){
		for(PathSegment* one_inner_seg:one_seg->get_subdivided_path()){
			simplyifed_path.push_back(one_seg->get_startPoint().x);
			simplyifed_path.push_back(one_seg->get_startPoint().y);
		}
	}
	return result::AWD_SUCCESS;
}
/*
bool GEOM::PointInTri(double x, double y, double t1x, double t1y, double t2x, double t2y, double t3x, double t3y)
{
	// Compute vectors      
	double v0x = (t3x - t1x);
	double v0y = (t3y - t1y);
	double v1x =(t2x - t1x);
	double v1y =(t2y - t1y);
	double v2x =(x - t1x);
	double v2y =(y - t1y);

	// Compute vectors        

	
	// Compute dot products
	double dot00 = (v0x*v0x + v0y*v0y);
	double dot01 = (v0x*v1x + v0y*v1y);
	double dot02 = (v0x*v2x + v0y*v2y);
	double dot11 = (v1x*v1x + v1y*v1y);
	double dot12 = (v1x*v2x + v1y*v2y);


	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return ((u >= 0) && (v >= 0) && (u + v < 1));

}
*/


double  GEOM::distance_point_to_line(float startx, float starty, float testx, float testy, float endx, float endy)
{
	  return (startx - endx) * (testy - endy) - (testx - endx) * (starty - endy);
}

bool
GEOM::line_intersect(float x1, float y1, float x2, float y2,float x3, float y3,float x4, float y4)
{
	
	// Store the values for fast access and easy
	// equations-to-code conversion
 
	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return false;
	
												
	// Get the x and y
	float pre = (x1*y2 - y1*x2);
	float post = (x3*y4 - y3*x4);
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
	// Check if the x and y coordinates are within both lines
	if ((( x < std::min(x1, x2)) || (x > std::max(x1, x2)) ||	(x < std::min(x3, x4)) || (x > std::max(x3, x4)))) return false;
	if ((( y < std::min(y1, y2)) || (y > std::max(y1, y2)) ||	(y < std::min(y3, y4)) || (y > std::max(y3, y4)))) return false;
 /* // Return the point of intersection
	Point* ret = new Point();
	ret->x = x;
	ret->y = y;
	return ret;*/
	return true;
}
/*

		// Optionally resolve all intersections:
		if(awd->getExporterSettings()->get_resolve_intersections()){
		
			// resolving:
			// step1:	pre-calculate bounds for all segments. store this bounds in list.
			// step2:	check all bounding-boxes agains each other. 
			//			For each encountered intersection, create a AWDPathIntersection.
			//			The AWDPathInteresection gets added to the "interesection"-list of both segments.	
			// step3:	Loop over all segments, and resolve all AWDPathIntersections for segments that are not curved.
			//			These intersections are easier to resolve, so we process them first
			//step4:	Loop over all segmens, and resolve all remaining AWDPathIntersections.

			vector<vector<double> > allBounds;
			vector<double> curBounds;
			vector<double> curTestBounds;
			vector<double> mergedBounds;
			vector<vector<PathSegment*> > allBoundSegs;
			pointlength.clear();	
			pntCnt=0;
			int boundsCnt=0;
			int lastusedBounds=-1;
			bool newBounds=true;
			bool isIntersect=true;
			pathCnt=0;
			PathSegment* pathSeg2;

			// collect a flat list of all segments, and pre-calculate their bounds
			vector<PathSegment*> all_segs_flat;
			vector<AWDPathIntersection*> all_path_intersections;
			double allsize=0;
			int allCnt=0;
			for(vector<PathSegment*> path:all_segments)
			{	
				for(PathSegment* pathSeg:path)
				{
					if(pathSeg->get_edgeType()!=OUTTER_EDGE){
						allCnt++;
						allsize+=pathSeg->get_length();
					}
					curBounds=pathSeg->get_bounds();
					all_segs_flat.push_back(pathSeg);
				}
			}
			// loop over the previous collected list, and collect information about intersecting.
			// for each intersecting, we create a AWDPathIntersection object, and store a reference to it on the Path-Segment->path_intersections
			PathSegment* pathSeg;
			AWDPathIntersection* thisPathIntersec;
			AWDPathIntersection* newIntersection;
			int childCnt=0;
			int childCnt2=0;
			for(segCnt=0; segCnt<all_segs_flat.size(); segCnt++){
				for(childCnt=0; childCnt<all_segs_flat[segCnt]->get_subdivided_path().size(); childCnt++){
					pathSeg=all_segs_flat[segCnt]->get_subdivided_path()[childCnt];
					pathCnt++;
					newBounds=true;
					curBounds=pathSeg->get_bounds();
					//AwayJS::Utils::Trace(m_pCallback, "Bounds A = %f, %f, %f, %f\n", curBounds[0], curBounds[1], curBounds[2], curBounds[3]);
					for(segCnt2=0; segCnt2<segCnt; segCnt2++){
						for(childCnt2=0; childCnt2<all_segs_flat[segCnt2]->get_subdivided_path().size(); childCnt2++){
							pathSeg2=all_segs_flat[segCnt2]->get_subdivided_path()[childCnt2];
							if((pathSeg->get_edgeType()!=OUTTER_EDGE)||(pathSeg2->get_edgeType()!=OUTTER_EDGE)){
								thisPathIntersec=pathSeg->find_path_intersections(pathSeg, pathSeg2);
								if(thisPathIntersec==NULL){
									curTestBounds=pathSeg2->get_bounds();
									isIntersect=bounds_interesect(curTestBounds, curBounds);
									if(isIntersect){
										newIntersection = new AWDPathIntersection(pathSeg, pathSeg2);
										pathSeg->add_path_intersections(newIntersection);
										pathSeg2->add_path_intersections(newIntersection);					
										all_path_intersections.push_back(newIntersection);
									}
								}
							}
						}
					}
				}
			}

			// now each AWDSegment, that intersects with another AWDSegment, has a list of AWDPathIntersection objects.
			// Both intersecting AWDSegments share the same AWDPathIntersection object.

			// first we resolve all PathIntersections, that are involving straight segments
			// they are easier to resolve, because we dont have to decide wich triangle needs subdividng
	
			for(PathSegment* pathSeg:all_segs_flat)
			{	
				if(pathSeg->get_edgeType()==OUTTER_EDGE){
					for(AWDPathIntersection* intersect:pathSeg->get_path_intersections()){
						if(intersect->get_state()==UNCHECKED){
							resolve_segment_intersection(intersect);
						}		
					}
				}	
			}
	
			// loop over the list of PathIntersections, and resolve the remaining
			// (only curve <-> curve intersections should be left unsolved by now)
			// they are more tricky to resolve, because we want no unneccessary subdividing
			for (AWDPathIntersection* oneintersect: all_path_intersections){
				if(oneintersect->get_state()==UNCHECKED){
					resolve_segment_intersection(oneintersect);					
				}	
				delete oneintersect;	
			}	
			// by now, all Curves should have been subdivided, so they no longer intersect each other
		}

		ShapePoint * resultPoint;
		PathSegment* check_seg;
		PathSegment* seg;
		bool removed_overlapping=false;
		segCnt=0;
		segCnt2=0;
		int segCnt_prev=0;
		int segCnt_next=0;
	
		AWDMergedSubShape* mergerSubShape = new AWDMergedSubShape((AWDBlock*)awd_shape->get_fill());
		*/
			/*
		vector<AWDMergedSubShape*> mergershapes=this->awd_shape->get_mergerSubShapes();
		AWDMergedSubShape* mergerSubShape = mergershapes.back();
		if(mergerSubShape==NULL){
			mergerSubShape=new AWDMergedSubShape((AWDBlock*)awd_shape->get_fill());
			this->awd_shape->add_sub_merger_mesh(mergerSubShape);
		}*/
	/*
		//mergerSubShape->checkForRessourceLimits_extern(nverts, nelems);
		int vertexOffset=mergerSubShape->getAllSubsVerts().back().size();
		ShapePoint* new_point_1;
		ShapePoint* new_point_2;
		ShapePoint* new_point_3;
		for (int i = 0; i < nverts; ++i){
			new_point_1=new ShapePoint();
			new_point_1->x=verts[i*2];
			new_point_1->y=verts[i*2+1];
			//AwayJS::Utils::Trace(m_pCallback, "Triangulated vert: = %f, %f\n",verts[i*2], verts[i*2+1]);
			mergerSubShape->addShapePoint(new_point_1);
		}
	
		for (int i = 0; i < nelems; ++i)
		{
			const int* p = &elems[i*3];
			bool addTri=true;	
		
			if(addTri){
				mergerSubShape->addFillFaceIDX(vertexOffset+p[0], vertexOffset+p[1], vertexOffset+p[2]);
			}
			else{
				AwayJS::Utils::Trace(m_pCallback, "Did not add tri because it is in place needed for convex curve ");
			}

		}
	
		this->awd_shape->add_sub_merger_mesh(mergerSubShape);
		if (tess) tessDeleteTess(tess);	
		for(TESSreal* thisPoints : tesPoints){
			free(thisPoints);
		}

		for(vector<PathSegment*> path:all_segments){
			int curveCnt=0;
			int lineCnt=0;
			for(PathSegment* pathSeg:path){		
	
				if(pathSeg->get_edgeType()==CONCAVE_EDGE){
					curveCnt++;
					new_point_1=new ShapePoint();
					new_point_2=new ShapePoint();
					new_point_3=new ShapePoint();
					new_point_1->x=pathSeg->get_endPoint()->x;
					new_point_1->y=pathSeg->get_endPoint()->y;
					new_point_2->x=pathSeg->get_controlPoint()->x;
					new_point_2->y=pathSeg->get_controlPoint()->y;
					new_point_3->x=pathSeg->get_startPoint()->x;
					new_point_3->y=pathSeg->get_startPoint()->y;
					mergerSubShape->addConcaveFace(new_point_1, new_point_2, new_point_3);
				}
				else if(pathSeg->get_edgeType()==CONVEX_EDGE){
					curveCnt++;
					new_point_1=new ShapePoint();
					new_point_2=new ShapePoint();
					new_point_3=new ShapePoint();
					new_point_1->x=pathSeg->get_startPoint()->x;
					new_point_1->y=pathSeg->get_startPoint()->y;
					new_point_2->x=pathSeg->get_controlPoint()->x;
					new_point_2->y=pathSeg->get_controlPoint()->y;
					new_point_3->x=pathSeg->get_endPoint()->x;
					new_point_3->y=pathSeg->get_endPoint()->y;
					mergerSubShape->addConvexFace(new_point_1, new_point_2, new_point_3);
				}
				else {
					lineCnt++;
				}
			
				delete pathSeg;	
			}	
			path.clear();
			//AwayJS::Utils::Trace(m_pCallback, "    -> New CURVECNT=  %d LineCnt = %d\n", curveCnt, lineCnt);
		}
		all_segments.clear();
	
		vector<PathSegment *> boundary_segs2;
		all_segments.push_back(boundary_segs2);
		return;
	*/
	/*
		
				// for all triangles, we check if they have any outter-edges (not curved-edges)
				// if a tri has more than one outter edge, it gets split
				// befor a outter-edge is added to the triangle list, its size gets checked.
				// if the size is to big ("outline_threshold") the trie gets split into 2 inner and 1 outter tri
				// verticles can be shared between inner and outter triangle (not curved triangles).
				
				int lineCnt=0;		
				int lineIdx1=0;	
				int lineIdx2=0;
				//AwayJS::Utils::Trace(m_pCallback, "    number of tesselated tris: %d\n", nonCurvedTris.size());
				for(tcnt=0; tcnt<nonCurvedTris.size(); tcnt+=3){
					thisIdx1=nonCurvedTris[tcnt];
					thisIdx2=nonCurvedTris[tcnt+1];
					thisIdx3=nonCurvedTris[tcnt+2];
					p1=points[thisIdx1];
					p2=points[thisIdx2];
					p3=points[thisIdx3];
					vector<edge_type> edge_types=get_edge_styles(thisIdx1, thisIdx2, thisIdx3);
					//all inner edges
					if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==INNER_EDGE)){	
						add_inner_tri(thisIdx1, thisIdx2, thisIdx3);
					}	

					//one outter edge	
					else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==INNER_EDGE)){
						add_outter_face(thisIdx2, thisIdx3, thisIdx1);
					}
					//one outter edge
					else if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==INNER_EDGE)){
						add_outter_face(thisIdx3, thisIdx1, thisIdx2);
					}
					//one outter edge
					else if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
						add_outter_face(thisIdx1, thisIdx2, thisIdx3);
					}
					//three outter edges
					else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
						//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 1\n");
						SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
						int newIdx=points.size();
						points.push_back(newPoint);				
						add_outter_face(thisIdx2, newIdx, thisIdx1);
						add_outter_face(thisIdx3, newIdx, thisIdx2);
						add_outter_face(thisIdx1, newIdx, thisIdx3);
					}
					// two outter edges
					else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==INNER_EDGE)){
						//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 2\n");
						SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
						int newIdx=points.size();
						points.push_back(newPoint);						
						add_inner_tri(thisIdx1, newIdx, thisIdx3);
						add_outter_face(thisIdx2, newIdx, thisIdx1);
						add_outter_face(thisIdx3, newIdx, thisIdx2);
					}
					// two outter edges
					else if((edge_types[0]==INNER_EDGE)&&(edge_types[1]==OUTTER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
						//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 3\n");
						SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
						int newIdx=points.size();
						points.push_back(newPoint);				
						add_inner_tri(thisIdx2, newIdx, thisIdx1);					
						add_outter_face(thisIdx1, newIdx, thisIdx3);
						add_outter_face(thisIdx3, newIdx, thisIdx2);
					}
					// two outter edges
					else if((edge_types[0]==OUTTER_EDGE)&&(edge_types[1]==INNER_EDGE)&&(edge_types[2]==OUTTER_EDGE)){
						//AwayJS::Utils::Trace(m_pCallback, "    SPLIT TRIANGLE 4\n");
						SimplePoint* newPoint = new SimplePoint (((p1->x+p2->x+p3->x)/3),((p1->y+p2->y+p3->y)/3));
						int newIdx=points.size();
						points.push_back(newPoint);				
						add_inner_tri(thisIdx3, newIdx, thisIdx2);					
						add_outter_face(thisIdx1, newIdx, thisIdx3);
						add_outter_face(thisIdx2, newIdx, thisIdx1);
					}				
				}	

			*/
	//}
	/*
TYPES::UINT16 
TYPES::get_bit16(int start, int end)
{
	TYPES::UINT16 int_min = 1 << start;
	TYPES::UINT16 int_max = 1 << end;
	TYPES::UINT16 result = (int_max | int_max - int_min);
	//return result;
	return ((((result & 0xff) * 0x0202020202ULL & 0x010884422010ULL) % 1023) << 8) | ((((result & 0xff00) >> 8) * 0x0202020202ULL & 0x010884422010ULL) % 1023);
}*/
/*
bool ShapeEncoder::PointInPath(double x, double y)
    {
		
        FCM::Result res;		
        bool oddNodes = false;
        int j = boundaryPoints.size() - 1;
		for (int i = 0; i < boundaryPoints.size(); i++)
        {
			//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
            if (((boundaryPoints[i]->y < y) && (boundaryPoints[j]->y >= y))||
                ((boundaryPoints[j]->y < y) && (boundaryPoints[i]->y >= y)))
            {
                if ((boundaryPoints[i]->x + (y - boundaryPoints[i]->y)/(boundaryPoints[j]->y - boundaryPoints[i]->y)*(boundaryPoints[j]->x - boundaryPoints[i]->x)) < x)
                {
                    oddNodes = !oddNodes;
                }
            }
            j = i;
        }
   return oddNodes;
}
*/
/*
// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(ShapePoint* p, ShapePoint* q, ShapePoint* r)
{
    if (q->x <= max(p->x, r->x) && q->x >= min(p->x, r->x) &&
        q->y <= max(p->y, r->y) && q->y >= min(p->y, r->y))
       return true;
 
    return false;
}
 */
/*
// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(ShapePoint* p, ShapePoint* q, ShapePoint* r)
{
    // See 10th slides from following link for derivation of the formula
    // http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
    int val = (q->y - p->y) * (r->x - q->x) -
              (q->x - p->x) * (r->y - q->y);
 
    if (val == 0) return 0;  // colinear
 
    return (val > 0)? 1: 2; // clock or counterclock wise
}
 */
/*
// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(ShapePoint* p1, ShapePoint* q1, ShapePoint* p2, ShapePoint* q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
 
    // General case
    if (o1 != o2 && o3 != o4)
        return true;
 
    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
 
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
 
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
 
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
 
    return false; // Doesn't fall in any of the above cases
}
*/
/*
int
ShapeEncoder::get_point_idx(SimplePoint * point)
{
	int idx=0;
	for (SimplePoint * p : points){
		if((double(p->x)==double(point->x))&&(double(point->y)==double(p->y))){
			return idx;
		}
		idx++;
	}
	//AwayJS::Utils::Trace(m_pCallback, "    could not find the point: %f, %f\nADD IT", point->x, point->y);
	//points.push_back(point);
	//return points.size()-1;
	//AwayJS::Utils::Trace(m_pCallback, "ERROR:  Tesselation manipulated point-positions - could not find index for point at position: %f, %f\n", point->x, point->y);
	int idxOut=idx;
	
	// sometimes, if curves are near-interseting other lines, poly2tri seams to alter point positions
	// if we use the new positions, we have a mess
	// my workarround for this (seams to work), is to get the closed point, for a point that was not found 
	idx=0;
	double deltaX=numeric_limits<double>::max();
	double deltaY=numeric_limits<double>::max();
	for (SimplePoint * p2 : points){		
		double temp_deltaX=abs((point->x)-double(p2->x));
		double temp_deltaY=abs((point->y)-double(p2->y));
		if((temp_deltaX<=deltaX)&&(temp_deltaY<=deltaY)){
			deltaX=temp_deltaX;
			deltaY=temp_deltaY;
			idxOut= idx;
		}
		idx++;
		}
	
	//AwayJS::Utils::Trace(m_pCallback, "-Exporter tried to resolve this issue by using the index of point at position: %f, %f\n", points[idxOut]->x, points[idxOut]->y);

	return idxOut;
}
*/
/*
vector<double>
ShapeEncoder::subDivideCurve(double startx, double starty, double cx, double cy, double endx, double endy)
  {
   double c1x = startx + (cx - startx) * 0.5;
   double c1y = starty + (cy - starty) * 0.5;
   
   double c2x = cx + (endx - cx) * 0.5;
   double c2y = cy + (endy - cy) * 0.5;
   
   double ax = c1x + (c2x - c1x) * 0.5;
   double ay = c1y + (c2y - c1y) * 0.5;
   vector<double> resultVec;
   resultVec.push_back(startx);
   resultVec.push_back(starty);
   resultVec.push_back(c1x);
   resultVec.push_back(c1y);
   resultVec.push_back(ax);
   resultVec.push_back(ay);
   resultVec.push_back(c2x);
   resultVec.push_back(c2y);
   resultVec.push_back(endx);
   resultVec.push_back(endy);
   return resultVec;
  }
*/
/*
SimplePoint * 
ShapeEncoder::calc_fill_point(SimplePoint * point1, SimplePoint * point2, SimplePoint * point3, double outputSize)
{
	double xmerged=point1->x+((point2->x-point1->x)*0.5);
	double ymerged=point1->y+((point2->y-point1->y)*0.5);
	double xmerged2=xmerged+((point3->x-xmerged)*outputSize);
	double ymerged2=ymerged+((point3->y-ymerged)*outputSize);
	SimplePoint * newPoint = new SimplePoint(xmerged2, ymerged2);
	return newPoint;
}
*/
/*
void
ShapeEncoder::add_outter_face(int idx1, int idx2, int idx3)
{
	*/
	/*
	ShapePoint* p1=points[idx1];
	ShapePoint* p2=points[idx2];
	ShapePoint* p3=points[idx3];
	if(this->outline_threshold==0.0){
		if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
			triangles_straight_edge_out.push_back(idx3);
			triangles_straight_edge_out.push_back(idx2);
			triangles_straight_edge_out.push_back(idx1);
		}
		else{
			triangles_straight_edge_out.push_back(idx1);
			triangles_straight_edge_out.push_back(idx2);
			triangles_straight_edge_out.push_back(idx3);
		}
	}
	else{
		double edgeLength=edge_length(p1,p2);
		double point_Distance=abs(Sign(p1,p3, p2));
		double distance_bias=point_Distance/edgeLength;
		//AwayJS::Utils::Trace(m_pCallback, "Coutline threshold file : %f\n", this->outline_threshold);
		//AwayJS::Utils::Trace(m_pCallback, "distance_bias : %f\n", distance_bias);
		// if the bias is bigger than the threshold, the triangle needs to be splittet
		if(distance_bias>this->outline_threshold){
			//AwayJS::Utils::Trace(m_pCallback, "Split triangle because of outline threshold : %f\n", distance_bias);
			ShapePoint * newPoint = calc_fill_point(p3,p1, p2, this->outline_threshold);
			int newIdx=points.size();
			points.push_back(newPoint);
			if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
				triangles_straight_edge_out.push_back(idx3);
				triangles_straight_edge_out.push_back(newIdx);
				triangles_straight_edge_out.push_back(idx1);
			}
			else{
				triangles_straight_edge_out.push_back(idx1);
				triangles_straight_edge_out.push_back(newIdx);
				triangles_straight_edge_out.push_back(idx3);
			}
			add_inner_tri(idx1, newIdx, idx2);
			add_inner_tri(idx2, newIdx, idx3);
		}
		else{
			if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
				triangles_straight_edge_out.push_back(idx3);
				triangles_straight_edge_out.push_back(idx2);
				triangles_straight_edge_out.push_back(idx1);
			}
			else{
				triangles_straight_edge_out.push_back(idx1);
				triangles_straight_edge_out.push_back(idx2);
				triangles_straight_edge_out.push_back(idx3);
			}
		}
	}*/

//}
/*
vector<edge_type>
ShapeEncoder::get_edge_styles(int thisIdx1, int thisIdx2, int thisIdx3)
{
	edge_type edge1=INNER_EDGE;
	edge_type edge2=INNER_EDGE;
	edge_type edge3=INNER_EDGE;
	int lineIdx1=0;
	int lineIdx2=0;
	int lineCnt=0;
	for (lineCnt=0; lineCnt<allLines.size();lineCnt+=2){
		lineIdx1=get_point_idx(allLines[lineCnt]);
		lineIdx2=get_point_idx(allLines[lineCnt+1]);
		if(((thisIdx1==lineIdx1)&&(thisIdx2==lineIdx2))||
			((thisIdx2==lineIdx1)&&(thisIdx1==lineIdx2))){
				edge1=OUTTER_EDGE;
		}
		if(((thisIdx3==lineIdx1)&&(thisIdx2==lineIdx2))||
			((thisIdx2==lineIdx1)&&(thisIdx3==lineIdx2))){
				edge2=OUTTER_EDGE;
		}
		if(((thisIdx3==lineIdx1)&&(thisIdx1==lineIdx2))||
			((thisIdx1==lineIdx1)&&(thisIdx3==lineIdx2))){
				edge3=OUTTER_EDGE;
		}
		if((edge1==OUTTER_EDGE)&&(edge2==OUTTER_EDGE)&&(edge3==OUTTER_EDGE)){
			lineCnt=allLines.size();
		}
	}
	vector<edge_type> edge_types;
	edge_types.push_back(edge1);
	edge_types.push_back(edge2);
	edge_types.push_back(edge3);

	return edge_types;
}

void
ShapeEncoder::get_edge_style(PathSegment* pathSeg, SimplePoint* c)
{
	
	if(pathSeg->get_edgeType()==CURVED_EDGE){
		double isSide_ctr=Sign(pathSeg->get_startPoint(),pathSeg->get_endPoint(),pathSeg->get_controlPoint());
		double isSide_filled=Sign(pathSeg->get_startPoint(),pathSeg->get_endPoint(),c);
		if((isSide_ctr>0)==(isSide_filled>0)){
			pathSeg->set_edgeType(CONCAVE_EDGE);
		}
		else{
			pathSeg->set_edgeType(CONVEX_EDGE);
		}
	}
	*/
/*
}

double
ShapeEncoder::edge_length(SimplePoint * a, SimplePoint* b)
{

     return sqrt((b->x - a->x)*(b->x - a->x) + (b->y - a->y)*(b->y - a->y));
}
*/
/*
double
ShapeEncoder::side_of_line(ShapePoint * a, ShapePoint * b, ShapePoint * c)
{
     return ((b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x));
}
double 
ShapeEncoder::Sign(ShapePoint * p1, ShapePoint * p2, ShapePoint * p3)
{
  return (p1->x - p3->x) * (p2->y - p3->y) - (p2->x - p3->x) * (p1->y - p3->y);
}

void ShapeEncoder::add_inner_tri(int idx1, int idx2, int idx3){
	*/
	/*SimplePoint* p1=points[idx1];
	SimplePoint* p2=points[idx2];
	SimplePoint* p3=points[idx3];
	if (isClockWiseXY(p1->x, p1->y, p2->x, p2->y, p3->x, p3->y)){
		triangles_inner.push_back(idx3);
		triangles_inner.push_back(idx2);
		triangles_inner.push_back(idx1);
	}
	else{
		triangles_inner.push_back(idx1);
		triangles_inner.push_back(idx2);
		triangles_inner.push_back(idx3);
	}*/
	/*
}

vector<double> 
	ShapeEncoder::mergeBounds(vector<double> bounds1, vector<double> bounds2) {
	vector<double> returnBounds;
	
	if (bounds1[0] <= bounds2[0]){
		returnBounds.push_back(bounds1[0]);
	}
	else{
		returnBounds.push_back(bounds2[0]);
	}
	if (bounds1[1] <= bounds2[1]){
		returnBounds.push_back(bounds2[1]);
	}
	else{
		returnBounds.push_back(bounds1[1]);
	}


	if (bounds1[2] <= bounds2[2]){
		returnBounds.push_back(bounds1[2]);
	}
	else{
		returnBounds.push_back(bounds2[2]);
	}
	if (bounds1[3] <= bounds2[3]){
		returnBounds.push_back(bounds2[3]);
	}
	else{
		returnBounds.push_back(bounds1[3]);
	}
					
	return returnBounds; 
} 

bool ShapeEncoder::resolve_segment_intersection(AWDPathIntersection* thisIntersection) {
	
	double min_Curveiness=awd->getExporterSettings()->get_curve_threshold();
	int maxSteps=awd->getExporterSettings()->get_max_iterations();
	ShapePoint* start1;
	ShapePoint* start2;
	ShapePoint* end1;
	ShapePoint* end2;
	ShapePoint* control1;
	ShapePoint* control2;
	PathSegment* seg1=thisIntersection->get_pathSeg1();
	PathSegment* seg2=thisIntersection->get_pathSeg2();
	PathSegment* curveSeg=NULL;
	PathSegment* edgeSeg=NULL;
	if((seg1->get_edgeType()!=OUTTER_EDGE)&&(seg2->get_edgeType()==OUTTER_EDGE)){
		curveSeg=seg1;
		edgeSeg=seg2;
	}
	else if((seg1->get_edgeType()==OUTTER_EDGE)&&(seg2->get_edgeType()!=OUTTER_EDGE)){
		curveSeg=seg2;
		edgeSeg=seg1;
	}
	else if((seg1->get_edgeType()!=OUTTER_EDGE)&&(seg2->get_edgeType()!=OUTTER_EDGE)){
		bool isResolved=false;
		int iterCnt=0;
		for(PathSegment* subSeg:seg2->get_subdivided_path()){
			subSeg->set_subdivide(true);
		}
		for(PathSegment* subSeg:seg1->get_subdivided_path()){
			subSeg->set_subdivide(true);
		}
		while((thisIntersection->get_state()==UNCHECKED)&&(iterCnt<maxSteps)){
			//AwayJS::Utils::Trace(m_pCallback, "RESOLVE CURVE VS CURVE %d\n", iterCnt);
			iterCnt++;
			isResolved=true;
			for(PathSegment* subSeg:seg1->get_subdivided_path()){
				if(subSeg->get_curviness()<min_Curveiness){
					subSeg->set_subdivide(false);
				}
				if(subSeg->get_subdivide()){
					start1=subSeg->get_startPoint();
					end1=subSeg->get_endPoint();
					control1=subSeg->get_controlPoint();
					bool intersectswith=false;
					for(PathSegment* subSeg2:seg2->get_subdivided_path()){
						bool isIntersect=bounds_interesect(subSeg->get_bounds(), subSeg2->get_bounds());
						if(isIntersect){
							start2=subSeg2->get_startPoint();
							end2=subSeg2->get_endPoint();
							control2=subSeg2->get_controlPoint();
							bool isTriIntersect=tris_intersecting(start1,  control1, end1, start2,  control2, end2);
							if(isTriIntersect){		
								
								intersectswith=true;
								isResolved=false;
							}
						}
					}
					subSeg->set_subdivide(intersectswith);
				}
			}
			for(PathSegment* subSeg:seg2->get_subdivided_path()){
				if(subSeg->get_curviness()<min_Curveiness){
					subSeg->set_subdivide(false);
				}
				if(subSeg->get_subdivide()){
					start1=subSeg->get_startPoint();
					end1=subSeg->get_endPoint();
					control1=subSeg->get_controlPoint();
					bool intersectswith=false;
					for(PathSegment* subSeg2:seg1->get_subdivided_path()){
						bool isIntersect=bounds_interesect(subSeg->get_bounds(), subSeg2->get_bounds());
						if(isIntersect){
							start2=subSeg2->get_startPoint();
							end2=subSeg2->get_endPoint();
							control2=subSeg2->get_controlPoint();
							bool isTriIntersect=tris_intersecting(start1,  control1, end1, start2,  control2, end2);
							if(isTriIntersect){					
								intersectswith=true;
								isResolved=false;
							}
						}
					}
					subSeg->set_subdivide(intersectswith);
				}
			}
			if(isResolved){
				if(thisIntersection->get_state()==UNCHECKED){
					//AwayJS::Utils::Trace(m_pCallback, "RESOLVED CURVE VS CURVE INTERSECTION with %d subdivisions\n", iterCnt);
					thisIntersection->set_state(RESOLVED);
				}
				iterCnt=maxSteps+1;
			}
			else{
				if(iterCnt==maxSteps){
					iterCnt=maxSteps+1;
					//AwayJS::Utils::Trace(m_pCallback, "UNRESOLVED CURVE CURVE\n");
					thisIntersection->set_state(UNSOLVEABLE);
				}
				else{
					seg1->subdividePath();
					seg2->subdividePath();
				}
			}
		}
	}
	if((curveSeg!=NULL)&&(edgeSeg!=NULL)){
		start1=edgeSeg->get_startPoint();
		end1=edgeSeg->get_endPoint();
		//AwayJS::Utils::Trace(m_pCallback, "RESOLVE CURVE VS EDGE\n");
		bool isResolved=false;
		int iterCnt=0;
		for(PathSegment* subSeg:curveSeg->get_subdivided_path()){
			subSeg->set_subdivide(true);
		}
		while((thisIntersection->get_state()==UNCHECKED)&&(iterCnt<maxSteps)){
			iterCnt++;
			isResolved=true;
			for(PathSegment* subSeg:curveSeg->get_subdivided_path()){
				if(subSeg->get_curviness()<min_Curveiness){
					subSeg->set_subdivide(false);}
				if(subSeg->get_subdivide()){
					bool isIntersect=bounds_interesect(subSeg->get_bounds(), edgeSeg->get_bounds());
					if(isIntersect){
						start2=curveSeg->get_startPoint();
						end2=curveSeg->get_endPoint();
						control2=curveSeg->get_controlPoint();
						bool intersects_edge=line_intersect(start1, end1, start2, end2);
						bool intersects_curve1=line_intersect(start1, end1, start2, control2);
						bool intersects_curve2=line_intersect(start1, end1, control2, end2);
						// if a line cuts the base.line of a tri, and a curved line, it is definitve not solveable by subdividing
						if((intersects_edge)&&((intersects_curve1)||(intersects_curve2))){
							thisIntersection->set_state(UNSOLVEABLE);
							//AwayJS::Utils::Trace(m_pCallback, "UNSOLVEABLE EDGE CURVE\n");
						}
						else if ((intersects_curve1)||(intersects_curve2)||(intersects_curve2)){
							//AwayJS::Utils::Trace(m_pCallback, "INTERSECTS EDGE CURVE\n");
							isResolved=false;
						}
						else{
							subSeg->set_subdivide(false);
							//AwayJS::Utils::Trace(m_pCallback, "NO INTERSECTS EDGE CURVE\n");
						}
					}
					else{
						subSeg->set_subdivide(false);
						//AwayJS::Utils::Trace(m_pCallback, "NO INTERSECTS EDGE CURVE\n");
					}
				}
			}
			if(isResolved){
				if(thisIntersection->get_state()==UNCHECKED){
					//AwayJS::Utils::Trace(m_pCallback, "RESOLVED EDGE VS CURVE INTERSECTION with %d subdivisions\n", iterCnt);
					thisIntersection->set_state(RESOLVED);
				}
				iterCnt=maxSteps+1;
			}
			else{
				if(iterCnt==maxSteps){
					iterCnt=maxSteps+1;
					AwayJS::Utils::Trace(m_pCallback, "UNRESOLVED EDGE CURVE\n");
					thisIntersection->set_state(UNSOLVEABLE);
				}
				else{
					curveSeg->subdividePath();
				}
			}

		}

	}
	return true;
} 
bool ShapeEncoder::bounds_interesect(vector<double> bounds1, vector<double> bounds2) {
	
	if(bounds1[1] <= bounds2[0]) {return false;}
	if(bounds1[3] <= bounds2[2]) {return false;}
	if(bounds2[1] <= bounds1[0]) {return false;}
	if(bounds2[3] <= bounds1[2]) {return false;}
	return true; 
} 
double ShapeEncoder::maximum(double x, double y, double z) {
	double max_val = x; 
	if (y > max_val) max_val = y;
	if (z > max_val) max_val = z;
	return max_val; 
} 
double ShapeEncoder::minimum(double x, double y, double z) {
	double min_val = x; 
	if (y < min_val) min_val = y;
	if (z < min_val) min_val = z;
	return min_val; 
} 
*/
/* Check whether P and Q lie on the same side of line AB 
float Side(float px, float py,float qx,float qy, float ax,float ay,  float bx, float by)
{
    float z1 = (bx - ax) * (py - ay) - (px - ax) * (by - ay);
    float z2 = (bx - ax) * (qy - ay) - (qx - ax) * (by - ay);
    return z1 * z2;
}

 Check whether segment P0P1 intersects with triangle t0t1t2

int ShapeEncoder::Intersecting(float p0x,float p0y, float p1x, float p1y, float t0x, float t0y, float t1x, float t1y, float t2x,float t2y)
{
	*/
    /* Check whether segment is outside one of the three half-planes
     * delimited by the triangle. */
	/*
    float f1 = Side(p0x, p0y, t2x, t2y, t0x, t0y, t1x, t1y), f2 = Side(p1x, p1y, t2x, t2y, t0x, t0y, t1x, t1y);
    float f3 = Side(p0x, p0y, t0x, t0y, t1x, t1y, t2x, t2y), f4 = Side(p1x, p1y, t0x, t0y, t1x, t1y, t2x, t2y);
    float f5 = Side(p0x, p0y, t1x, t1y, t2x, t2y, t0x, t0y), f6 = Side(p1x, p1y, t1x, t1y, t2x, t2y, t0x, t0y);
	*/
    /* Check whether triangle is totally inside one of the two half-planes
     * delimited by the segment. */
	/*
    float f7 = Side(t0x, t0y, t1x, t1y, p0x, p0y, p1x, p1y);
    float f8 = Side(t1x, t1y, t2x, t2y, p0x, p0y, p1x, p1y);
	*/

    /* If segment is strictly outside triangle, or triangle is strictly
     * apart from the line, we're not intersecting */
	/*
    if ((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0)
          || (f7 > 0 && f8 > 0))
        return -1;
		*/
    /* If segment is aligned with one of the edges, we're overlapping */
	/*
    if ((f1 == 0 && f2 == 0) || (f3 == 0 && f4 == 0) || (f5 == 0 && f6 == 0))
        return 1;
		*/
    /* If segment is outside but not strictly, or triangle is apart but
     * not strictly, we're touching */
	/*
    if ((f1 <= 0 && f2 <= 0) || (f3 <= 0 && f4 <= 0) || (f5 <= 0 && f6 <= 0)
          || (f7 >= 0 && f8 >= 0))
        return 0;
	*/
    /* If both segment points are strictly inside the triangle, we
     * are not intersecting either */
	/*
    if (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0 && f5 > 0 && f6 > 0)
        return 5;
	*/

    /* Otherwise we're intersecting with at least one edge */
	/*
    return 1;
}

bool ShapeEncoder::PointInPath(double x, double y)
    {
		
        FCM::Result res;		
        bool oddNodes = false;
        int j = boundaryPoints.size() - 1;
		for (int i = 0; i < boundaryPoints.size(); i++)
        {
			//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
            if (((boundaryPoints[i]->y < y) && (boundaryPoints[j]->y >= y))||
                ((boundaryPoints[j]->y < y) && (boundaryPoints[i]->y >= y)))
            {
                if ((boundaryPoints[i]->x + (y - boundaryPoints[i]->y)/(boundaryPoints[j]->y - boundaryPoints[i]->y)*(boundaryPoints[j]->x - boundaryPoints[i]->x)) < x)
                {
                    oddNodes = !oddNodes;
                }
            }
            j = i;
        }
   return oddNodes;
}
bool ShapeEncoder::PointInTri(double x, double y, double x1, double y1, double x2, double y2,double x3, double y3)
{
		
	FCM::Result res;		
	bool oddNodes = false;
	//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
    if (((y1 < y) && (y3 >= y))||((y3 < y) && (y1 >= y)))
    {
		if ((x1 + (y - y1)/(y3 - y1)*(x3 - x1)) < x)
		{
             oddNodes = !oddNodes;
        }
	}
    if (((y2 < y) && (y1 >= y))||((y1 < y) && (y2 >= y)))
    {
		if ((x2 + (y - y2)/(y1 - y2)*(x1 - x2)) < x)
		{
             oddNodes = !oddNodes;
        }
	}
    if (((y3 < y) && (y2 >= y))||((y2 < y) && (y3 >= y)))
    {
		if ((x3 + (y - y3)/(y2 - y3)*(x2 - x3)) < x)
		{
             oddNodes = !oddNodes;
        }
	}
   return oddNodes;
}
bool ShapeEncoder::PointInHole(int holeIdx, double x, double y)
{
		
	FCM::Result res;		
	bool oddNodes = false;
	int j = allPoints[holeIdx].size() - 1;
	for (int i = 0; i < allPoints[holeIdx].size(); i++)
     {
			//Utils::Trace(GetCallback(), "check points %f vs %f\n", newPoints[i]->x, newPoints[i]->y);
            if (((allPoints[holeIdx][i]->y < y) && (allPoints[holeIdx][j]->y >= y))||
                ((allPoints[holeIdx][j]->y < y) && (allPoints[holeIdx][i]->y >= y)))
            {
                if ((allPoints[holeIdx][i]->x + (y - allPoints[holeIdx][i]->y)/(allPoints[holeIdx][j]->y - allPoints[holeIdx][i]->y)*(allPoints[holeIdx][j]->x - allPoints[holeIdx][i]->x)) < x)
                {
                    oddNodes = !oddNodes;
                }
            }
            j = i;
        }
   return oddNodes;
}
bool
ShapeEncoder::tris_intersecting(ShapePoint * a1, ShapePoint* a2, ShapePoint * a3, ShapePoint * b1, ShapePoint * b2, ShapePoint * b3)
{	
	//allready tested for bounding box (with precalculated bounds...)
	//if(!test_bounding_box(a1, a2, a3, b1, b2, b3))	return false;	
	//AwayJS::Utils::Trace(m_pCallback, "		triangle BoundingBox intersect!!\n");
	
	// we only want to check if the control-point of a curve is inside another curve
	//if(point_in_tr(a2, b1, b2, b3)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		Point 1 not in tri!!\n");
	//if(PointInTri(b2->x, b2->y,a1->x, a1->y,a2->x, a2->y,a3->x, a3->y)) return true;
	//if(PointInTri(b1->x, b1->y,a1->x, a1->y,a2->x, a2->y,a3->x, a3->y)) return true;
	//if(PointInTri(b3->x, b3->y,a1->x, a1->y,a2->x, a2->y,a3->x, a3->y)) return true;
	if(point_in_tr(b2, a1, a2, a3)) return true;
	if(point_in_tr(b1, a1, a2, a3)) return true;
	if(point_in_tr(b3, a1, a2, a3)) return true;
	*/
	//AwayJS::Utils::Trace(m_pCallback, "		Point not in tri!!\n");
	/*
	bool base_intersect=false;
	if(doIntersect(a1, a3, b1, b2)){
		base_intersect=true;
	}
	else if(doIntersect(a1, a3, b2, b3)){
		base_intersect=true;
	}
	else if(doIntersect(a1, a3, b1, b3)){
		base_intersect=true;
	}*/
	/*
	if(doIntersect(a1, a2, b2, b3)&&(!base_intersect)) return true;
	if(doIntersect(a1, a2, b1, b2)&&(!base_intersect)) return true;
	if(doIntersect(a1, a2, b3, b1)&&(!base_intersect)) return true;
	if(doIntersect(a2, a3, b1, b2)&&(!base_intersect)) return true;
	if(doIntersect(a2, a3, b2, b3)&&(!base_intersect)) return true;
	if(doIntersect(a2, a3, b3, b1)&&(!base_intersect)) return true;
	*/
	/*
	if(doIntersect(a1, a2, b2, b3)) return true;
	if(doIntersect(a1, a2, b1, b2)) return true;
	if(doIntersect(a1, a2, b3, b1)) return true;
	if(doIntersect(a2, a3, b1, b2)) return true;
	if(doIntersect(a2, a3, b2, b3)) return true;
	if(doIntersect(a2, a3, b3, b1)) return true;
	if(doIntersect(a1, a3, b1, b2)) return true;
	if(doIntersect(a1, a3, b2, b3)) return true;
	if(doIntersect(a1, a3, b3, b1)) return true;
	

	//if((point_in_tr(a1, b1, b2, b3)&&(!base_intersect)) return true;
	//if(point_in_tr(a3, b1, b2, b3)) return true;
	//AwayJS::Utils::Trace(m_pCallback, "		line1 not in tri!!\n");
	//AwayJS::Utils::Trace(m_pCallback, "		line1 not in tri!!\n");
	//AwayJS::Utils::Trace(m_pCallback, "		line1 not in tri!!\n");
	//AwayJS::Utils::Trace(m_pCallback, "		No lines intersect\n");
	return false;

}

bool
ShapeEncoder::test_bounding_box(ShapePoint * a1, ShapePoint* a2, ShapePoint * a3, ShapePoint * b1, ShapePoint * b2, ShapePoint * b3)
{
	if(maximum(a1->x, a2->x, a3->x) <= minimum(b1->x, b2->x, b3->x)) {return false;}
	if(maximum(a1->y, a2->y, a3->y) <= minimum(b1->y, b2->y, b3->y)) {return false;}
	if(maximum(b1->x, b2->x, b3->x) <= minimum(a1->x, a2->x, a3->x)) {return false;}
	if(maximum(b1->y, b2->y, b3->y) <= minimum(a1->y, a2->y, a3->y)) {return false;}
	//AwayJS::Utils::Trace(m_pCallback, "		A maximumy = %f maxX %f\n", maximum(a1->x, a2->x, a3->x), maximum(a1->y, a2->y, a3->y));
	//AwayJS::Utils::Trace(m_pCallback, "		B maximumy = %f maxX %f\n", maximum(b1->x, b2->x, b3->x), maximum(b1->y, b2->y, b3->y));
	//AwayJS::Utils::Trace(m_pCallback, "		A minimumy = %f maxX %f\n", minimum(a1->x, a2->x, a3->x), minimum(a1->y, a2->y, a3->y));
	//AwayJS::Utils::Trace(m_pCallback, "		B minimumy = %f maxX %f\n", minimum(b1->x, b2->x, b3->x), minimum(b1->y, b2->y, b3->y));

	return true;
}
bool
ShapeEncoder::test_bounding_box_lines(ShapePoint* a1, ShapePoint* a2, ShapePoint * b1, ShapePoint * b2)
{
	if(max(a1->x, a2->x) <= min(b1->x, b2->x)) {return false;}
	if(max(a1->y, a2->y) <= min(b1->y, b2->y)) {return false;}
	if(max(b1->x, b2->x) <= min(a1->x, a2->x)) {return false;}
	if(max(b1->y, b2->y) <= min(a1->y, a2->y)) {return false;}

	return true;
}

bool
ShapeEncoder::point_in_tr(ShapePoint * P, ShapePoint * A, ShapePoint * B, ShapePoint * C)
{
	// Compute vectors      
	double v0x = (C->x - A->x);
	double v0y = (C->y - A->y);
	double v1x =(B->x - A->x);
	double v1y =(B->y - A->y);
	double v2x =(P->x - A->x);
	double v2y =(P->y - A->y);

	// Compute vectors        

	
	// Compute dot products
	double dot00 = (v0x*v0x + v0y*v0y);
	double dot01 = (v0x*v1x + v0y*v1y);
	double dot02 = (v0x*v2x + v0y*v2y);
	double dot11 = (v1x*v1x + v1y*v1y);
	double dot12 = (v1x*v2x + v1y*v2y);


	// Compute barycentric coordinates
	double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return ((u >= 0) && (v >= 0) && (u + v < 1));

}

bool
ShapeEncoder::line_intersect(ShapePoint * p1, ShapePoint * p2, ShapePoint * p3, ShapePoint * p4)
{
	
	// Store the values for fast access and easy
	// equations-to-code conversion
	float x1 = p1->x, x2 = p2->x, x3 = p3->x, x4 = p4->x;
	float y1 = p1->y, y2 = p2->y, y3 = p3->y, y4 = p4->y;
 
	float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return false;
	
												
	// Get the x and y
	float pre = (x1*y2 - y1*x2);
	float post = (x3*y4 - y3*x4);
	float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
	// Check if the x and y coordinates are within both lines
	if ((( x < min(x1, x2)) || (x > max(x1, x2)) ||	(x < min(x3, x4)) || (x > max(x3, x4)))) return false;
	if ((( y < min(y1, y2)) || (y > max(y1, y2)) ||	(y < min(y3, y4)) || (y > max(y3, y4)))) return false;
	*/
 /* // Return the point of intersection
	Point* ret = new Point();
	ret->x = x;
	ret->y = y;
	return ret;*/
	/*
	return true;
}
ShapePoint*
ShapeEncoder::line_intersection_point(ShapePoint * p1, ShapePoint * p2, ShapePoint * p3, ShapePoint * p4)
{
	
	// Store the values for fast access and easy
	// equations-to-code conversion
	double x1 = p1->x, x2 = p2->x, x3 = p3->x, x4 = p4->x;
	double y1 = p1->y, y2 = p2->y, y3 = p3->y, y4 = p4->y;
 
	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	// If d is zero, there is no intersection
	if (d == 0) return NULL;
	
 
	// Get the x and y
	double pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	double x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	double y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
	// Check if the x and y coordinates are within both lines
	if ( x < min(x1, x2) || x > max(x1, x2) ||
	x < min(x3, x4) || x > max(x3, x4) ) return NULL;
	if ( y < min(y1, y2) || y > max(y1, y2) ||
	y < min(y3, y4) || y > max(y3, y4) ) return NULL;
	ShapePoint* ret = new ShapePoint();
	ret->x=x;
	ret->y=y;
	return ret;
}
*/
#include "elements/stream_attributes.h"
#include "elements/geom_elements.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "utils/util.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;

PathSegment::PathSegment()
{
	this->state=GEOM::edge_state::TEST_INTERSECTING;
	this->edgeType=GEOM::edge_type::OUTTER_EDGE;
	this->test_point_dirty=true;
	this->test_point_dirty2=true;
	this->no_intersecting=false;
	this->has_control_point = false;
	this->export_this_linear = false;
	this->curviness = -1;
	this->length = 0;
	this->num_id=0;
	this->contains_origin=false;
	this->subdivion_cnt=0;
	this->size=0;
}

PathSegment::~PathSegment()
{
}

double
PathSegment::get_size()
{
	if(this->size==0){		
		if(this->length==0)
			get_length();		
		double length2=sqrt(((startPoint.x-controlPoint.x)*(startPoint.x-controlPoint.x))+((startPoint.y-controlPoint.y)*(startPoint.y-controlPoint.y)));
		double length3=sqrt(((endPoint.x-controlPoint.x)*(endPoint.x-controlPoint.x))+((endPoint.y-controlPoint.y)*(endPoint.y-controlPoint.y)));
		double s = (this->length+length2+length3)/2;
		this->size = sqrt(s*(s-this->length)*(s-length2)*(s-length3));		
	}
	return this->size;
}
bool 
PathSegment::get_contains_origin()
{
	return this->contains_origin;
}
void 
PathSegment::set_contains_origin(bool contains_origin)
{
	this->contains_origin=contains_origin;
}

void
PathSegment::calculate_bounds(){
	if(this->edgeType==edge_type::OUTTER_EDGE){
		if(this->startPoint.x>this->endPoint.x){
			this->min_x=this->endPoint.x;
			this->max_x=this->startPoint.x;
		}
		else{
			this->min_x=this->startPoint.x;
			this->max_x=this->endPoint.x;
		}
		if(this->startPoint.y>this->endPoint.y){
			this->min_y=this->endPoint.y;
			this->max_y=this->startPoint.y;
		}
		else{
			this->min_y=this->startPoint.y;
			this->max_y=this->endPoint.y;
		}
	}
	else{
		this->min_x = minimum(this->startPoint.x, this->endPoint.x, this->controlPoint.x);
		this->max_x = maximum(this->startPoint.x, this->endPoint.x, this->controlPoint.x);
		this->min_y = minimum(this->startPoint.y, this->endPoint.y, this->controlPoint.y);
		this->max_y = maximum(this->startPoint.y, this->endPoint.y, this->controlPoint.y);
	}
}

TYPES::UINT32 
PathSegment::get_num_id()
{
	return this->num_id;
}
void
PathSegment::set_num_id(TYPES::UINT32 num_id)
{
	this->num_id = num_id;
}
GEOM::PathSegment* 
PathSegment::get_next()
{
	return this->next;
}

void 
PathSegment::set_next(GEOM::PathSegment* next)
{
	this->next=next;
}

GEOM::PathSegment* 
PathSegment::get_last()
{
	return this->last;
}

void 
PathSegment::set_last(GEOM::PathSegment* last)
{
	this->last=last;
}

TYPES::UINT16 
PathSegment::get_bit16(int start, int end)
{
	TYPES::UINT16 int_min = 1 << start;
	TYPES::UINT16 int_max = 1 << end;
	return (int_max | int_max - int_min);
}

std::string& 
PathSegment::get_id()
{
	return this->id;
}

void 
PathSegment::set_id(std::string& id)
{
	this->id=id;
}
GEOM::Path* 
PathSegment::get_parent_path()
{
	return this->parent_path;
}

void 
PathSegment::set_parent_path(GEOM::Path* parent_path)
{
	this->parent_path=parent_path;
}

double PathSegment::get_length() {
	if(length==0){
		length=sqrt(((startPoint.x-endPoint.x)*(startPoint.x-endPoint.x))+((startPoint.y-endPoint.y)*(startPoint.y-endPoint.y)));
	}
	return length;
}
double
PathSegment::get_curviness() {
	if(curviness<0.0){		
		double middlex=startPoint.x+((endPoint.x-startPoint.x)*0.5);
		double middley=startPoint.y+((endPoint.y-startPoint.y)*0.5);
		double controlLength=sqrt(((controlPoint.x-middlex)*(controlPoint.x-middlex))+((controlPoint.y-middley)*(controlPoint.y-middley)));
		curviness=controlLength/get_length();
	}
	return curviness;
}
GEOM::edge_state 
PathSegment::get_state(){
	return this->state;
}
void 
PathSegment::set_state(GEOM::edge_state this_state){
	 this->state=this_state;
}

bool
PathSegment::get_export_this_linear(){
	return this->export_this_linear;
}
void
PathSegment::set_export_this_linear(bool export_this_linear)
{
	this->export_this_linear=export_this_linear;
}
bool
PathSegment::get_has_control_point()
{
	return this->has_control_point;
}
GEOM::VECTOR2D 
PathSegment::get_startPoint()
{
	return this->startPoint;
}
void 
PathSegment::set_startPoint(GEOM::VECTOR2D startPoint)
{
	this->startPoint = startPoint;
}
GEOM::VECTOR2D 
PathSegment::get_endPoint()
{
	return this->endPoint;
}
void 
PathSegment::set_endPoint(GEOM::VECTOR2D endPoint)
{
	this->endPoint = endPoint;
}
GEOM::VECTOR2D 
PathSegment::get_controlPoint()
{
	return this->controlPoint;
}
void 
PathSegment::set_controlPoint(GEOM::VECTOR2D controlPoint)
{
	this->controlPoint = controlPoint;
	this->has_control_point = true;
}
GEOM::edge_type 
PathSegment::get_edgeType()
{
	return this->edgeType;
}
void 
PathSegment::set_edgeType(GEOM::edge_type edgeType)
{
	this->edgeType = edgeType;
	for(PathSegment* pathSeg: this->subdividedPath){
		pathSeg->set_edgeType(edgeType);
	}
}

std::vector<PathSegment*>&
PathSegment::get_subdivided_path()
{
	if(subdividedPath.size()==0){
		PathSegment* newseg=new PathSegment();
		newseg->set_edgeType(this->get_edgeType());
		newseg->set_startPoint(GEOM::VECTOR2D(this->startPoint.x, this->startPoint.y));
		newseg->set_controlPoint(GEOM::VECTOR2D(this->controlPoint.x, this->controlPoint.y));
		newseg->set_endPoint(GEOM::VECTOR2D(this->endPoint.x, this->endPoint.y));
		subdividedPath.push_back(newseg);
	}
	return subdividedPath;
}

GEOM::VECTOR2D
PathSegment::get_test_point()
{
	if(this->test_point_dirty){
		double tmpPointx=this->startPoint.x + (( this->endPoint.x - this->startPoint.x)/2);
		double tmpPointy=this->startPoint.y + (( this->endPoint.y - this->startPoint.y)/2);
		double tmpPointx2=(( this->controlPoint.x - tmpPointx)/100);
		double tmpPointy2=(( this->controlPoint.y - tmpPointy)/100);
		double length = sqrt((tmpPointx2*tmpPointx2)+(tmpPointy2*tmpPointy2));
		this->test_point.x=tmpPointx + ((tmpPointx2/length) * 2);
		this->test_point.y=tmpPointy + ((tmpPointy2/length) * 2);
	}
	
	this->test_point_dirty=false;
	return this->test_point;
}
GEOM::VECTOR2D
PathSegment::get_test_point2()
{
	if(this->test_point_dirty2){
		double tmpPointx=this->startPoint.x + (( this->endPoint.x - this->startPoint.x)/2);
		double tmpPointy=this->startPoint.y + (( this->endPoint.y - this->startPoint.y)/2);
		double tmpPointx2=(( this->controlPoint.x - tmpPointx));
		double tmpPointy2=(( this->controlPoint.y - tmpPointy));
		this->test_point2.x=this->controlPoint.x - ((tmpPointx2) * 20);
		this->test_point2.y=this->controlPoint.y - ((tmpPointy2) * 20);
	}
	
	this->test_point_dirty2=false;
	return this->test_point2;
}
GEOM::VECTOR2D
PathSegment::get_test_point3()
{
	if(this->test_point_dirty3){
		double tmpPointx=this->startPoint.x + (( this->endPoint.x - this->startPoint.x)/2);
		double tmpPointy=this->startPoint.y + (( this->endPoint.y - this->startPoint.y)/2);
		double tmpPointx2=(( this->controlPoint.x - tmpPointx));
		double tmpPointy2=(( this->controlPoint.y - tmpPointy));
		this->test_point3.x=this->controlPoint.x + ((tmpPointx2) * 13);
		this->test_point3.y=this->controlPoint.y + ((tmpPointy2) * 13);
	}
	
	this->test_point_dirty3=false;
	return this->test_point3;
}

void
PathSegment::sub_divide_outside_edge(std::vector<GEOM::VECTOR2D>& new_tri1,std::vector<GEOM::VECTOR2D>& new_tri2, double threshold)
{
	double tmpPointx=this->startPoint.x + (( this->endPoint.x - this->startPoint.x)/2);
	double tmpPointy=this->startPoint.y + (( this->endPoint.y - this->startPoint.y)/2);
	
	double tmpPointx2=(( this->controlPoint.x - tmpPointx));
	double tmpPointy2=(( this->controlPoint.y - tmpPointy));
	double length = sqrt( ( tmpPointx2 * tmpPointx2 ) + ( tmpPointy2 * tmpPointy2 ));
	double newLength = threshold*this->get_length();
	GEOM::VECTOR2D new_point = GEOM::VECTOR2D(tmpPointx + ((tmpPointx2/length)*newLength), tmpPointy + ((tmpPointy2/length)*newLength));
	new_tri1.push_back(new_point);
	new_tri1.push_back(this->controlPoint);
	new_tri1.push_back(this->startPoint);
	new_tri2.push_back(this->endPoint);
	new_tri2.push_back(this->controlPoint);
	new_tri2.push_back(new_point);
	this->controlPoint=new_point;
}
bool
PathSegment::get_no_intersecting(){
	return this->no_intersecting;
}
void
PathSegment::set_no_intersecting(bool subdivide)
{
	this->no_intersecting=subdivide;
}
bool
PathSegment::get_subdivide(){
	return this->subdivide;
}
void
PathSegment::set_subdivide(bool subdivide)
{
	this->subdivide=subdivide;
}
bool
PathSegment::get_deleteIt(){
	return this->deleteIt;
}
void
PathSegment::set_deleteIt(bool deleteIt)
{
	this->deleteIt=deleteIt;
}
void
PathSegment::set_dirty()
{
	this->test_point_dirty=true;
	this->test_point_dirty2=true;
	this->test_point_dirty3=true;
}

result
PathSegment::subdividePath(SETTINGS::Settings* settings)
{
	std::vector<PathSegment*> newPath;
	PathSegment* newSeg;
	get_subdivided_path();
	state=edge_state::MAX_SUBDIVISION;
	for(PathSegment* pathSeg: this->subdividedPath){
		if(pathSeg->get_state()==edge_state::SUBDIVIDED){			
			std::vector<double> newPoints;
			GEOM::subdivideCurve(pathSeg->get_startPoint().x, pathSeg->get_startPoint().y, pathSeg->get_controlPoint().x, pathSeg->get_controlPoint().y,  pathSeg->get_endPoint().x,  pathSeg->get_endPoint().y, newPoints);					
		
			pathSeg->set_startPoint(VECTOR2D(newPoints[0], newPoints[1]));
			pathSeg->set_controlPoint(VECTOR2D(newPoints[2], newPoints[3]));
			pathSeg->set_endPoint(VECTOR2D(newPoints[4], newPoints[5]));
			pathSeg->set_edgeType(this->edgeType);
			pathSeg->set_dirty();
			pathSeg->subdivion_cnt++;
			newPath.push_back(pathSeg);
			
			if(pathSeg->get_curviness() <= settings->get_curve_threshold())
				pathSeg->set_state(edge_state::MAX_SUBDIVISION);
			else
				pathSeg->set_state(edge_state::SUBDIVIDED);
			

			newSeg = new PathSegment();
			newSeg->set_startPoint(VECTOR2D(newPoints[4], newPoints[5]));
			newSeg->set_controlPoint(VECTOR2D(newPoints[6], newPoints[7]));
			newSeg->set_endPoint(VECTOR2D(newPoints[8], newPoints[9]));
			newSeg->set_edgeType(this->edgeType);
			newSeg->subdivion_cnt++;
			newPath.push_back(newSeg);
			if(newSeg->get_curviness() <= settings->get_curve_threshold())
				newSeg->set_state(edge_state::MAX_SUBDIVISION);
			else
				newSeg->set_state(edge_state::SUBDIVIDED);
				
			newPoints.clear();
		}
		else{
			newPath.push_back(pathSeg);
		}
	}
	subdividedPath=newPath;
	return result::AWD_SUCCESS;
}

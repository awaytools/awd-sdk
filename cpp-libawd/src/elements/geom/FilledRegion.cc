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

FilledRegionGroup::FilledRegionGroup()
{
	this->redirect=NULL;
	this->max_x=std::numeric_limits<int>::max()*-1;
	this->max_y=std::numeric_limits<int>::max()*-1;
	this->min_x=std::numeric_limits<int>::max();
	this->min_y=std::numeric_limits<int>::max();
}

FilledRegionGroup::~FilledRegionGroup()
{
}
void 
FilledRegionGroup::add_filled_region(FilledRegion* filled_region)
{
	if(this->min_x>filled_region->get_pathes()[0]->min_x)
		this->min_x=filled_region->get_pathes()[0]->min_x;
	if(this->max_x<filled_region->get_pathes()[0]->max_x)
		this->max_x=filled_region->get_pathes()[0]->max_x;
	if(this->min_y>filled_region->get_pathes()[0]->min_y)
		this->min_y=filled_region->get_pathes()[0]->min_y;
	if(this->max_y<filled_region->get_pathes()[0]->max_y)
		this->max_y=filled_region->get_pathes()[0]->max_y;	
	this->filled_regions.push_back(filled_region);
}
void 
FilledRegionGroup::merge_group_into(FilledRegionGroup* filled_group)
{
	if(this->redirect!=NULL)
		this->redirect->merge_group_into(filled_group);
	else{		
		if(this->min_x>filled_group->min_x)
			this->min_x=filled_group->min_x;
		if(this->max_x<filled_group->max_x)
			this->max_x=filled_group->max_x;
		if(this->min_y>filled_group->min_y)
			this->min_y=filled_group->min_y;
		if(this->max_y<filled_group->max_y)
			this->max_y=filled_group->max_y;	
		for(FilledRegion* filled_reg : filled_group->filled_regions)
			this->filled_regions.push_back(filled_reg);
	}
}


FilledRegion::FilledRegion(TYPES::filled_region_type type)
{
	this->uv_transform=new MATRIX2x3();
	this->fill_material=NULL;
	this->type=type;
}

FilledRegion::~FilledRegion()
{
	for(Path* onePath:pathes)
		delete onePath;
}
void
FilledRegion::add_path()
{
	this->pathes.push_back(new GEOM::Path());
}
void
FilledRegion::add_existing_path(GEOM::Path* path)
{
	this->pathes.push_back(path);
}

void
FilledRegion::set_material(BASE::AWDBlock* fill_material)
{
	this->fill_material = fill_material;
}

BASE::AWDBlock*
FilledRegion::get_material()
{
	return this->fill_material;
}

void
FilledRegion::set_type(TYPES::filled_region_type type)
{
	this->type = type;
}

TYPES::filled_region_type
FilledRegion::get_type()
{
	return this->type;
}

std::vector<GEOM::Path* >&
FilledRegion::get_pathes()
{
	return this->pathes;
}

result
FilledRegion::add_segment(GEOM::PathSegment* segment)
{
	if(this->pathes.size()==0)
		this->add_path();	
	return this->pathes.back()->add_segment(segment);
}


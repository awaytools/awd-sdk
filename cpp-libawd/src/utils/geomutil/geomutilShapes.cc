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
GEOM::ProcessShapeGeometry(Geometry* geom, AWDProject* awd_project, SETTINGS::Settings* subgeom_settings)
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

	/*
	std::string geom_message = "";
	std::string geom_warning = "";
	std::string geom_errors = "";
	*/

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
		
		//geom_message += "Region "+std::to_string(region_cnt)+" -  path-count = "+std::to_string(filled_region->get_pathes().size())+"\n";

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
			if(path_result!=result::PATH_CONTAINS_LESS_THAN_THREE_SEGMENTS){	
				has_valid_path=true;
				/*if(path_result==result::PATH_NOT_CLOSING){
					geom_warning += "Path not closing.\n";		
				}*/
			}
			
		}
///		-	If all path of this FilledRegion are invalid, we skip to next FilledRegion.\n
		if(!has_valid_path){
			//geom_errors += "No path of region valid\n";		
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
	
		//if(awd_project->get_settings()->get_export_curves()){
			for(GEOM::Path* one_path: filled_region->get_pathes()){
				one_path->get_curve_types(awd_project->get_settings());	
			}
		//}
			/*
		else{
			// tranform all curves into linear segments.
			for(GEOM::Path* one_path: filled_region->get_pathes())
				one_path->make_linear();				
		}	
		*/

		if(all_min_x>filled_region->get_pathes()[0]->min_x)
			all_min_x=filled_region->get_pathes()[0]->min_x;
		if(all_max_x<filled_region->get_pathes()[0]->max_x)
			all_max_x=filled_region->get_pathes()[0]->max_x;
		if(all_min_y>filled_region->get_pathes()[0]->min_y)
			all_min_y=filled_region->get_pathes()[0]->min_y;
		if(all_max_y<filled_region->get_pathes()[0]->max_y)
			all_max_y=filled_region->get_pathes()[0]->max_y;		
	}

	/*
	int concaves=0;
	int convex=0;
	int straight=0;
	int all_segs=0;
	for(FilledRegion* filled_region : geom->get_filled_regions()){	
		for(GEOM::Path* one_path: filled_region->get_pathes()){
			for(PathSegment* pathSeg:one_path->get_segments()){
				if(pathSeg->get_edgeType()==edge_type::CONCAVE_EDGE)
					concaves++;
				else if(pathSeg->get_edgeType()==edge_type::CONVEX_EDGE)
					convex++;
				else
					straight++;
				all_segs++;

			}

		}
	}
	geom_message += "\nSEGMENT-COUNT IN = "+std::to_string(all_segs)+" | concave = "+std::to_string(concaves)+" | convex "+std::to_string(convex)+" | straight "+std::to_string(straight)+"\n";
	
	*/
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
				
	/*	
	std::string new_message="complete bounds = "+std::to_string(all_width)+","+std::to_string(all_height)+"\n";	
	geom->add_message(new_message, message_type::STATUS_MESSAGE);
	*/


	if((false)||(cnt_regions_fonts>0)){
		// loop over regions:
		region_cnt=0;
		udword* InputValues_regions = new udword[ valid_regions.size()];
		for(FilledRegion* filled_region :  valid_regions){	
		
			// convert the bounds as doubles in range 0.0 - 16.0
			/*
			std::string new_message2="path bounds x = "+std::to_string(filled_region->get_pathes()[0]->min_x )+","+std::to_string(filled_region->get_pathes()[0]->max_x)+"\n";
			new_message2+="path bounds y = "+std::to_string(filled_region->get_pathes()[0]->min_y )+","+std::to_string(filled_region->get_pathes()[0]->max_y)+"\n";
			*/
			//geom->add_message(new_message2, message_type::STATUS_MESSAGE);
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
			// merge both 16bit values into a 32bit value (e.g. 0001111111110000 + 0000111100000000 = 00011111111100000000111100000000)#ifdef _WIN32
#ifdef _WIN32
			combined_int = (((TYPES::UINT32) x_int) << 16) | y_int;
#else
			combined_int = (((TYPES::UINT32) y_int) << 16) | x_int;
#endif

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
		
			
			std::bitset<16> x1(x_int);
			std::bitset<16> x2(y_int);
			std::bitset<32> x3(combined_int);
			/*
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
		DELETEARRAY(InputValues_regions);

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
		//DELETEARRAY(Sorted);

		// at this point we have lists of filled_regions that we know might intersect.
		// we could further check bounding boxes etc, but i think this might be good enough.
	
		/*
		std::string geom_message_bkp = geom_message;
		geom_message = "\nInput Regions = "+std::to_string(valid_regions.size())+ " ( "+std::to_string(cnt_regions_standart)+" fill / "+std::to_string(cnt_regions_strokes)+" strokes / "+std::to_string(cnt_regions_fonts)+" font )\n"+geom_message;
		geom_message += "\nCalculated intersecting groups of regions = "+std::to_string(final_sorted_filled_groups.size())+"\n";
		*/
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
			Path newPath = Path(false);
			for(FilledRegionGroup* filled_regions_group : final_sorted_filled_groups){
				FilledRegion* filled_region = new FilledRegion(TYPES::filled_region_type::GENERATED_FONT_OUTLINES);
				for(FilledRegion* filled_region1 : filled_regions_group->filled_regions){
					for(PathSegment* pathSeg:filled_region1->get_pathes()[0]->get_segments()){
						newPath.add_segment_fonts(pathSeg);
					}
					filled_region->add_existing_path(filled_region1->get_pathes()[0]);
				}
				final_filled_regions_list.push_back(filled_region);
			}
			newPath.prepare(edge_type::OUTTER_EDGE);
			for(FilledRegionGroup* filled_regions_group : final_sorted_filled_groups){
				for(FilledRegion* filled_region : filled_regions_group->filled_regions){
					PathSegment* this_seg = NULL;
					for(PathSegment* pathSeg:filled_region->get_pathes()[0]->get_segments()){
						if((pathSeg->get_edgeType()==edge_type::CONCAVE_EDGE)||(pathSeg->get_edgeType()==edge_type::CONVEX_EDGE)){
							this_seg = pathSeg;
							break;
						}
					}
					if(this_seg!=NULL){
						int is_hole=newPath.point_inside_path(this_seg->get_test_point().x, this_seg->get_test_point().y);
						if(this_seg->get_edgeType()==edge_type::CONCAVE_EDGE){
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
						else if(this_seg->get_edgeType()==edge_type::CONVEX_EDGE){
							if(!(is_hole % 2)){
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

	
	int cnt_region_segs = 0;
	int width = 0;
	int height= 0;
	int region_min_x=0;
	int region_max_x=0;
	int region_min_y=0;
	int region_max_y=0;	
	std::vector<int> bounds(4);	
	std::vector<PathSegment*> all_intersecting_segs;
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
#ifdef _WIN32
					combined_int = (((TYPES::UINT32) x_int) << 16) | y_int;
#else
					combined_int = (((TYPES::UINT32) y_int) << 16) | x_int;
#endif
					
					pathSeg->bit_id_x=x_int;
					pathSeg->bit_id_y=y_int;

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
				//if((seg_1 == seg_2->get_next())||(seg_1 == seg_2->get_last()))
				//	continue;
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
	//if(all_intersecting_segs.size()>0)
	//	geom_message+="Found pairs of possible intersecting segments: "+std::to_string(all_intersecting_segs.size()/2)+"\n";

	int curve_curve_intersects = 0;
	int curve_line_intersects = 0;
	for(seg_cnt=0; seg_cnt<all_intersecting_segs.size(); seg_cnt+=2){
		seg_1 = all_intersecting_segs[seg_cnt];
		seg_2 = all_intersecting_segs[seg_cnt+1];
		seg_1->set_state(edge_state::SUBDIVIDED);
		seg_2->set_state(edge_state::SUBDIVIDED);
		for(PathSegment* subSeg2:seg_1->get_subdivided_path())
			subSeg2->set_state(edge_state::SUBDIVIDED);
		for(PathSegment* subSeg2:seg_2->get_subdivided_path())
			subSeg2->set_state(edge_state::SUBDIVIDED);
		if(seg_1->get_edgeType()==edge_type::OUTTER_EDGE){
			curve_line_intersects++;
			resolve_line_curve_intersection(seg_2, seg_1, awd_project->get_settings());
		}
		else if(seg_2->get_edgeType()==edge_type::OUTTER_EDGE){
			curve_line_intersects++;
			resolve_line_curve_intersection(seg_1, seg_2, awd_project->get_settings());
		}
		else{
			curve_curve_intersects++;
			resolve_curve_curve_intersection(seg_1, seg_2, awd_project->get_settings());
		}
	}
	
	//geom_message+="Found intersecting : curve_line_intersects "+std::to_string(curve_line_intersects)+" curve_curve_intersects "+std::to_string(curve_curve_intersects)+"\n";
	
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
			
			//geom_message+="CURVE POINTCNT = "+std::to_string(pnt_cnt)+"\n";
	
			// for each path create the list of TESSreal and add it as contour.
			TESSreal* thisPoints = (TESSreal*)malloc(sizeof(TESSreal) * pnt_cnt*2);
			int pnt_cnt_2=0;
			path_start_point_idx=all_pnt_cnt;
			for(PathSegment* pathSeg:one_path->get_segments())
			{
				// if we want either to sort interior / exterior tis or keep verts in order of path, we need to build a map to map the resulting tris to line-segments
				if((awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d())||(awd_project->get_settings()->get_keep_verticles_in_path_order())){
					if(pathSeg->get_edgeType()==GEOM::edge_type::OUTTER_EDGE){
						std::string edge_str = FILES::int_to_string(all_pnt_cnt) + "#" + FILES::int_to_string(all_pnt_cnt+1);
						if(pathSeg==one_path->get_segments().back())
							edge_str = FILES::int_to_string(path_start_point_idx) + "#" + FILES::int_to_string(all_pnt_cnt);
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
			//geom_errors+="TRIANGULATION FAILED (libtess return with failure = no triangulated data is available)";
			continue;
		}
		//AwayJS::Utils::Trace(m_pCallback, "hasTriangulated  = %d\n",hasTriangulated);
		const float* verts	= tessGetVertices(tess);
		const int* vinds	= tessGetVertexIndices(tess);
		const int* elems	= tessGetElements(tess);
		const int nverts	= tessGetVertexCount(tess);
		const int nelems	= tessGetElementCount(tess);	

		
		GEOM::SubGeom* new_subgeom = new SubGeom(subgeom_settings);
		if(filled_region->get_type()==filled_region_type::GENERATED_FONT_OUTLINES){
			new_subgeom->get_settings()->create_streams(false, false);
		}
		else{
			new_subgeom->isMerged=true;
			/*
			new_subgeom->target_subgeom=awd_project->shared_geom->get_sub_at(0);
			new_subgeom->startIDX = new_subgeom->target_subgeom->tri_cnt * 3;
			*/
			
		}

		BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(filled_region->get_material());
		new_subgeom->set_material(this_mat);
		new_subgeom->uv_transform->set(filled_region->uv_transform->get());
	
		/*

		geom_message  += "\n tesselation result: tricnt: " + std::to_string(nelems);
		geom_message  += " / points in: " + std::to_string(all_pnt_cnt) + "  / points out: " + std::to_string(nverts) + "\n";
		if(all_pnt_cnt!=nverts)
			geom_warning += "\n tesselation changed point-count. points in: " + std::to_string(all_pnt_cnt) + " points out: " + std::to_string(nverts) + "\n";
		
		*/
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
					
			all_inner_tris.push_back(triangle_vecs);
			//continue;
			/*
			TYPES::INT32 vert_idx0 = vinds[p[0]];
			TYPES::INT32 vert_idx1 = vinds[p[1]];
			TYPES::INT32 vert_idx2 = vinds[p[2]];
			std::vector<GEOM::PathSegment*> path_segs_found;
			
			std::string new_lookup_str;
			if((vert_idx0>=0)&&(vert_idx1>=0)){
				if(vert_idx0>vert_idx1)
					new_lookup_str = FILES::int_to_string(vert_idx1) + "#" + FILES::int_to_string(vert_idx0);
				else
					new_lookup_str = FILES::int_to_string(vert_idx0) + "#" + FILES::int_to_string(vert_idx1);
				if(edge_cache.find(new_lookup_str) != edge_cache.end()){
					GEOM::PathSegment* seg_edge = edge_cache[new_lookup_str];	
					seg_edge->set_controlPoint(triangle_vecs[2]);
					path_segs_found.push_back(seg_edge);
				}
			}
			if((vert_idx1>=0)&&(vert_idx2>=0)){
				if(vert_idx1>vert_idx2)
					new_lookup_str = FILES::int_to_string(vert_idx2) + "#" + FILES::int_to_string(vert_idx1);
				else
					new_lookup_str = FILES::int_to_string(vert_idx1) + "#" + FILES::int_to_string(vert_idx2);
				if(edge_cache.find(new_lookup_str) != edge_cache.end()){
					GEOM::PathSegment* seg_edge = edge_cache[new_lookup_str];	
					seg_edge->set_controlPoint(triangle_vecs[0]);
					path_segs_found.push_back(seg_edge);
				}				
			}
			if((vert_idx0>=0)&&(vert_idx2>=0)){
				if(vert_idx0>vert_idx2)
					new_lookup_str = FILES::int_to_string(vert_idx2) + "#" + FILES::int_to_string(vert_idx0);
				else
					new_lookup_str = FILES::int_to_string(vert_idx0) + "#" + FILES::int_to_string(vert_idx2);
				if(edge_cache.find(new_lookup_str) != edge_cache.end()){
					GEOM::PathSegment* seg_edge = edge_cache[new_lookup_str];	
					seg_edge->set_controlPoint(triangle_vecs[1]);
					path_segs_found.push_back(seg_edge);
				}			
			}
				
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
			*/
		}
		tessDeleteTess(tess);
		
/// \subsection PathGeoStep6 Step 6: Create and fill SubGeometries.	
		
		int cnt_error_segments=0;
		for(GEOM::Path* one_path: filled_region->get_pathes()){
			for(PathSegment* pathSeg:one_path->get_segments())
			{
				if(pathSeg->get_edgeType()==GEOM::edge_type::OUTTER_EDGE){
					if(((awd_project->get_settings()->get_distinglish_interior_exterior_triangles_2d())||(awd_project->get_settings()->get_keep_verticles_in_path_order()))){
						SUBGEOM_ID_STRING subGeo_id;
						if(pathSeg->get_export_this_linear()){
							SUBGEOM_ID_STRING subGeo_id;
							new_subgeom->get_internal_id(subGeo_id);
							new_subgeom->create_triangle(edge_type::OUTTER_EDGE, pathSeg->get_startPoint(), pathSeg->get_controlPoint(), pathSeg->get_endPoint());
						}
						else
							cnt_error_segments++;
					}
				}
				/*
				if(pathSeg->get_edgeType()==GEOM::edge_type::CURVED_EDGE)
					bool test=true;	// this means the curve-tppe wasnt calculated correctly. should NEVER happen
					*/
				else if((pathSeg->get_edgeType()==GEOM::edge_type::CONCAVE_EDGE) || (pathSeg->get_edgeType()==GEOM::edge_type::CONVEX_EDGE)){					
					for(PathSegment* inner_seg:pathSeg->get_subdivided_path())
					{
						SUBGEOM_ID_STRING subGeo_id;
						if(inner_seg->subdivion_cnt>0){
							new_subgeom->get_internal_id(subGeo_id);
							new_subgeom->create_triangle(inner_seg->get_edgeType(), inner_seg->get_startPoint(), inner_seg->get_controlPoint(), inner_seg->get_endPoint());
						}
						else{
							if(inner_seg->get_edgeType()==GEOM::edge_type::CONCAVE_EDGE){
								new_subgeom->get_internal_id(subGeo_id);
								new_subgeom->create_triangle(inner_seg->get_edgeType(), inner_seg->get_startPoint(), inner_seg->get_controlPoint(), inner_seg->get_endPoint());
							}
							else if(inner_seg->get_edgeType()==GEOM::edge_type::CONVEX_EDGE){
								new_subgeom->get_internal_id(subGeo_id);
								new_subgeom->create_triangle(inner_seg->get_edgeType(), inner_seg->get_startPoint(), inner_seg->get_controlPoint(), inner_seg->get_endPoint());
							}
						}
					}
				}
			}
		}
		/*
		if(cnt_error_segments>0)
			geom_errors+="ERROR: "+std::to_string(cnt_error_segments)+" linear path segments could not be asociated with a triangle. This segments will not be exported!\n";
				*/	
		for(std::vector<GEOM::VECTOR2D> one_tri : all_inner_tris){
			SUBGEOM_ID_STRING subGeo_id;
			new_subgeom->get_internal_id(subGeo_id);
			new_subgeom->create_triangle(edge_type::INNER_EDGE, one_tri[0], one_tri[1], one_tri[2]);
		}
		
		if(new_subgeom->get_tri_cnt()>0){
			geom->add_subgeo(new_subgeom);
			has_valid_shape=true;
		}
		region_cnt++;
	}
	// if have not processed anything successfully, we add a error on the geometry and set it to be invalid
	/*
	if(!has_valid_shape){
		geom_errors += "No valid regions - no valid geometry created\n";		
		geom->set_state(state::INVALID);
	}
	geom->add_message(geom_message, TYPES::message_type::STATUS_MESSAGE);
	geom->add_message(geom_warning, TYPES::message_type::WARNING_MESSAGE);
	geom->add_message(geom_errors, TYPES::message_type::ERROR_MESSAGE);
	*/

	return result::AWD_SUCCESS;
}

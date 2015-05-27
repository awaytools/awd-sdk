#include "elements/frame_commands.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "blocks/geometry.h"
#include "blocks/material.h"
#include "blocks/mesh_library.h"
#include "blocks/text_element.h"
#include "utils/awd_properties.h"

#include <bitset>
#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
 

FrameCommandDisplayObject::FrameCommandDisplayObject() :
	FrameCommandBase()
{

	this->adobe_depth_change=false;
	this->display_matrix=new GEOM::MATRIX2x3();
	this->bkp_matrix=new GEOM::MATRIX2x3();
	this->bkp_color_matrix=new GEOM::ColorTransform();	
	this->color_matrix=new GEOM::ColorTransform();
	this->hasDisplayMatrix=false;
	this->hasColorMatrix=false;
	this->hasDepthChange=false;
	this->hasFilterChange=false;
	this->hasBlendModeChange=false;
	this->hasTargetMaskIDs=false;
	this->hasVisiblitiyChange=false;
	this->clip_depth=0;
	this->target_mask_id=-2;
	this->blendMode=0;
	this->isMask=false;
	this->calculated_mask=false;
	this->child_id=0;
	this->cur_depth=0;
	this->visible=true;
	this->does_something=false;
	this->add_at_index=0;

}

FrameCommandDisplayObject::~FrameCommandDisplayObject()
{	
	delete this->display_matrix;	
	delete this->color_matrix;	
}
bool
FrameCommandDisplayObject::get_hasDisplayMatrix(){
	return this->hasDisplayMatrix;
}
bool
FrameCommandDisplayObject::get_hasColorMatrix(){
	return this->hasColorMatrix;
}
bool
FrameCommandDisplayObject::get_hasDepthChange(){
	return this->hasDepthChange;
}
bool
FrameCommandDisplayObject::get_hasFilterChange(){
	return this->hasFilterChange;
}
bool
FrameCommandDisplayObject::get_hasBlendModeChange(){
	return this->hasBlendModeChange;
}
bool
FrameCommandDisplayObject::get_hasTargetMaskIDs(){
	return this->hasTargetMaskIDs;
}
void
FrameCommandDisplayObject::set_hasTargetMaskIDs(bool hasTargetMaskIDs){
	this->hasTargetMaskIDs=hasTargetMaskIDs;
}

bool
FrameCommandDisplayObject::get_hasVisiblitiyChange(){
	return this->hasVisiblitiyChange;
}
void
FrameCommandDisplayObject::set_display_matrix(TYPES::F64* display_matrix)
{
	this->does_something=true;
	this->hasDisplayMatrix=true;
	this->display_matrix->set(display_matrix);
}
GEOM::MATRIX2x3*
FrameCommandDisplayObject::get_display_matrix()
{
	return this->display_matrix;
}
void
FrameCommandDisplayObject::set_color_matrix(TYPES::F64* color_matrix)
{
	this->does_something=true;
	this->hasColorMatrix=true;
    this->color_matrix->set(color_matrix);
}
GEOM::ColorTransform*
FrameCommandDisplayObject::get_color_matrix()
{
	return this->color_matrix;
}
std::string&
FrameCommandDisplayObject::get_instanceName()
{
	return this->instanceName;
}
void
FrameCommandDisplayObject::set_instanceName(const std::string& instanceName)
{
	this->does_something=true;
    this->instanceName = instanceName;
}
void
FrameCommandDisplayObject::set_visible(bool visible)
{
	this->does_something=true;
	this->hasVisiblitiyChange=true;
	this->visible = visible;
}
bool
FrameCommandDisplayObject::get_visible()
{
	return this->visible;
}
int
FrameCommandDisplayObject::get_blendmode()
{
	return this->blendMode;
}
void
FrameCommandDisplayObject::set_blendmode(int blendMode)
{
	this->does_something=true;
	this->blendMode=blendMode;
}
void
FrameCommandDisplayObject::set_clipDepth(TYPES::UINT32 clipDepth)
{
	this->does_something=true;
	this->hasTargetMaskIDs=true;
	this->clip_depth = clipDepth;
}
TYPES::UINT32
FrameCommandDisplayObject::get_clipDepth()
{
	return this->clip_depth;
}
double
FrameCommandDisplayObject::compareColorMatrix(TYPES::F64* color_matrix)
{
	int countvalid=0;
	
	for(int i=0; i<20;i++){
		if(this->color_matrix->get()[i]==color_matrix[i]){
			countvalid++;
		}
	}
	
    return countvalid/20;
}
double
FrameCommandDisplayObject::comparedisplaMatrix(TYPES::F64* display_matrix)
{
	int countvalid=0;
	
	for(int i=0; i<6;i++){
		if(this->display_matrix->get()[i]==display_matrix[i]){
			countvalid++;
		}
	}
	
    return double(countvalid)/double(6.0);
}
result
FrameCommandDisplayObject::get_command_info_specific(std::string& info)
{
	if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)
		info = "	AddChild";
	else if(this->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)
		info = "	Update";

	if(this->child!=NULL)
		info += " | child-name: "+this->child->child->awd_block->get_name()+" | child-id: "+std::to_string(this->child->child->id)+" | depth: "+std::to_string(this->child->depth);
	else{
		info += " | obj_id: "+std::to_string(this->get_objID())+" | child-id: "+std::to_string(this->child->child->id)+" | depth: "+std::to_string(this->child->depth);
	
	}
	if(this->get_hasTargetMaskIDs()){
		info +=" | mask-id: ";
		for(int one_mask_id:this->mask_ids)
			info+=std::to_string(one_mask_id)+"|"; 
	}
	if(this->get_instanceName().size()>0)
		info += " | inst-name: '"+this->get_instanceName()+"'"; 

	if(this->get_hasDisplayMatrix()){
		GEOM::MATRIX2x3* thismtx = this->get_display_matrix();
		TYPES::F64* mtx = thismtx->get();
		info += " | transform: ";
		for(int i = 0; i<6; i++){
			info+=" "+std::to_string(mtx[i]);
		}
	}
	if(this->get_hasColorMatrix()){
		GEOM::ColorTransform* thismtx = this->get_color_matrix();
		TYPES::F64* mtx = thismtx->get();
		info += " | color-transform: ";
		for(int i = 0; i<8; i++){
			info+=" "+std::to_string(mtx[i]);
		}
	}
	if(this->get_hasVisiblitiyChange()){
		if(this->get_visible())
			info += " | visible: true";
		else
			info += " | visible: false";
	}
	if(this->get_hasBlendModeChange()){
		info += " | blendmode: "+ std::to_string(this->get_blendmode());
	}
	

	return AWD::result::AWD_SUCCESS;
}

int
FrameCommandDisplayObject::update_from_prev()
{
	int has_update=0;
	if(this->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_ADD_CHILD){
		has_update++;
	}
	this->hasDisplayMatrix=false;
	this->hasColorMatrix=false;
	this->hasVisiblitiyChange=false;
	this->hasBlendModeChange=false;
	if(this->get_command_type()==ANIM::frame_command_type::FRAME_COMMAND_UPDATE){
		if(this->prev_frame!=NULL){
			int error = 0;
			// todo: update from prev frame
		}
	}
	if(this->prev_frame==NULL){
		TYPES::F64* display_m = this->display_matrix->get();
		if((display_m[0]!=1)||(display_m[1]!=0.0)||(display_m[2]!=0.0)||(display_m[3]!=1.0)||(display_m[4]!=0.0)||(display_m[5]!=0.0))
			this->hasDisplayMatrix=true;
		TYPES::F64* color_m = this->color_matrix->get();
		if((color_m[0]!=1.0)||(color_m[1]!=0.0)||(color_m[2]!=1.0)||(color_m[3]!=0.0)||(color_m[4]!=1.0)||(color_m[5]!=0.0)||(color_m[6]!=1.0)||(color_m[7]!=0.0))
			this->hasColorMatrix=true;
		if(this->blendMode!=0)
			this->hasBlendModeChange=true;
		if(!visible)
			this->hasVisiblitiyChange=true;
	}
	else{
		if((this->prev_frame->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)||(this->prev_frame->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)){
			FrameCommandDisplayObject* prev_fd=(FrameCommandDisplayObject*)this->prev_frame;
			TYPES::F64* display_m = this->display_matrix->get();
			TYPES::F64* display_m_prev = prev_fd->display_matrix->get();
			if((display_m[0]!=display_m_prev[0])||(display_m[1]!=display_m_prev[1])||(display_m[2]!=display_m_prev[2])||(display_m[3]!=display_m_prev[3])||(display_m[4]!=display_m_prev[4])||(display_m[5]!=display_m_prev[5]))
				this->hasDisplayMatrix=true;
			TYPES::F64* color_m = this->color_matrix->get();
			TYPES::F64* color_m_prev = prev_fd->color_matrix->get();
			if((color_m[0]!=color_m_prev[0])||(color_m[1]!=color_m_prev[1])||(color_m[2]!=color_m_prev[2])||(color_m[3]!=color_m_prev[3])||(color_m[4]!=color_m_prev[4])||(color_m[5]!=color_m_prev[5])||(color_m[6]!=color_m_prev[6])||(color_m[7]!=color_m_prev[7]))
				this->hasColorMatrix=true;
			if(this->blendMode!=prev_fd->get_blendmode())
				this->hasBlendModeChange=true;
			if(visible!=prev_fd->get_visible())
				this->hasVisiblitiyChange=true;
		}
		else{
			int error=0;// pürev command should not be other than add or update
		}
	}
	if(this->hasColorMatrix)
		has_update++;
	if(this->hasDisplayMatrix)
		has_update++;
	if(this->hasVisiblitiyChange)
		has_update++;
	if(this->hasBlendModeChange)
		has_update++;

	return has_update;
}
double
FrameCommandDisplayObject::compare_to_command_specific(FrameCommandBase* frameCommand)
{
	FrameCommandDisplayObject* thisFC=(FrameCommandDisplayObject*)frameCommand;
	if(thisFC==NULL){
		return 0;
	}
	if(this->get_objectType()!=thisFC->get_objectType()){
		return 0;
	}
	if(this->get_object_block()!=thisFC->get_object_block()){
		return 0;
	}
	// the commands 
	double equal=0.0;
	int equalCnt=0;

	
	if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(this->hasColorMatrix)){
		equal += this->compareColorMatrix(thisFC->get_color_matrix()->get());
	}	
	else if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(!this->hasColorMatrix)){
		equal += 1;}	
	equalCnt++;

	if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(this->hasDisplayMatrix)){
		equal+=this->comparedisplaMatrix(thisFC->get_display_matrix()->get());
	}
	else if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(!this->hasDisplayMatrix)){
		equal+=1;}
	equalCnt++;
	
	if(this->get_depth()!=thisFC->get_depth()){
		double depth_equal = (1/abs(double(thisFC->get_depth()-double(this->get_depth()))));
		if(depth_equal==1)
			depth_equal=0.75;
		equal+=depth_equal;
	}
	else{
		equal+=1;
	}
	equalCnt++;
	
	if((this->hasVisiblitiyChange==thisFC->get_hasVisiblitiyChange())&&(this->hasVisiblitiyChange)){
		if(this->get_visible()==thisFC->get_visible()){
			equal+=1;
		}	
	}
	else if((this->hasVisiblitiyChange==thisFC->get_hasVisiblitiyChange())&&(!this->hasVisiblitiyChange)){
		equal+=1;}	
	equalCnt++;

	if(this->get_instanceName()==thisFC->get_instanceName()){
		equal+=1;
	}	
	equalCnt++;
	
	return double(equal)/double(equalCnt);
}


void
FrameCommandDisplayObject::update_by_command_specific(FrameCommandBase* frameCommand)
{
	FrameCommandDisplayObject* thisFC=(FrameCommandDisplayObject*)frameCommand;
	int equal=0;
	this->set_objID(frameCommand->get_objID());
	this->set_command_type(ANIM::frame_command_type::FRAME_COMMAND_UPDATE);
	/*if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(this->hasColorMatrix)){
		equal = this->compareColorMatrix(thisFC->get_color_matrix()->get());
		if(equal==1){
			this->hasColorMatrix=false;
		}
	}

	if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(this->hasDisplayMatrix)){
		equal=this->comparedisplaMatrix(thisFC->get_display_matrix()->get());
		if(equal==1){
			this->hasDisplayMatrix=false;
		}
	}
	
	if((this->hasDepthChange==thisFC->get_hasDepthChange())&&(this->hasDepthChange)){
		if(this->get_depth()==thisFC->get_depth()){
			this->hasDepthChange=false;			
		}	
	}

	if((this->hasVisiblitiyChange==thisFC->get_hasVisiblitiyChange())&&(this->hasVisiblitiyChange)){
		if(this->get_visible()==thisFC->get_visible()){
			this->hasVisiblitiyChange=false;			
		}	
	}

	if(this->get_instanceName()==thisFC->get_instanceName()){
		this->instanceName="";			
	}	
	*/
}


TYPES::UINT32
FrameCommandDisplayObject::calc_command_length_specific(SETTINGS::BlockSettings * blockSettings)
{
	int props_length=0;
	if(this->get_command_type()!=frame_command_type::FRAME_COMMAND_REMOVE){
		
		if(this->get_hasDisplayMatrix()){
			props_length+=sizeof(TYPES::F32)*6;
		}
		
		if(this->get_hasColorMatrix()){
			props_length+=sizeof(TYPES::F32)*4;
			props_length+=sizeof(TYPES::INT16)*4;
		}

		if(this->get_hasBlendModeChange()){
			props_length+=sizeof(TYPES::UINT8);
		}
		if(this->get_hasVisiblitiyChange()){
			props_length+=sizeof(TYPES::UINT8);
		}
		
	}		

    int len = sizeof(TYPES::UINT8); // command_type
	if((this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)||(this->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)){
		len+=sizeof(TYPES::UINT16);// objectID
		if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD){
			len+=sizeof(TYPES::INT16);// depth
			if(this->get_instanceName().size()!=0){
				props_length +=sizeof(TYPES::UINT16);// length instance_name
				props_length += TYPES::UINT16(this->get_instanceName().size());//
			}
		}	
		if(this->mask_ids.size()>0){
			props_length+=sizeof(TYPES::UINT16);//mask_ids
			props_length+=sizeof(TYPES::UINT16)*this->mask_ids.size();//mask_ids
		}
		len+=sizeof(TYPES::UINT16);		// props_flag
		len+=props_length;				// props_length
		//len += this->command_properties->calc_length(blockSettings); 

	}
    return len;
}

void
FrameCommandDisplayObject::write_command_specific(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * blockSettings, AWDFile* awd_file)
{
	
	fileWriter->writeUINT8(TYPES::UINT8(this->get_command_type()));
	if((this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)||(this->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)){
		
		std::bitset<16> props_flag(0);
		fileWriter->writeUINT16(this->child->child->id);
		if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD){
			fileWriter->writeINT16(this->child->depth);
			if(this->instanceName.size()>0)
				props_flag.set(6, true);
		}
		//this->command_properties->write_attributes(fileWriter, blockSettings);	

		// calculate and write flag for properties
		if (this->get_hasDisplayMatrix())
			props_flag.set(0, true);
		if (this->get_hasColorMatrix())
			props_flag.set(2, true);
		if (this->get_hasBlendModeChange())
			props_flag.set(4, true);
		if (this->get_hasVisiblitiyChange())
			props_flag.set(5, true);
		
		if(this->mask_ids.size()>0)
			props_flag.set(7, true);
		
		fileWriter->writeUINT16(TYPES::UINT16(props_flag.to_ulong()));
		
		// write properties
		if (this->get_hasDisplayMatrix())
			this->display_matrix->write_to_file(fileWriter, blockSettings);
		if (this->get_hasColorMatrix())
			this->color_matrix->write_to_file(fileWriter, blockSettings);
		if (this->get_hasBlendModeChange())
			fileWriter->writeUINT8(TYPES::UINT8(this->blendMode));
		if (this->get_hasVisiblitiyChange())
			fileWriter->writeBOOL(this->visible);		
		if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD){
			if(this->instanceName.size()>0)
				fileWriter->writeSTRING(instanceName, FILES::write_string_with::LENGTH_AS_UINT16);
		}		
		if(this->mask_ids.size()>0){
			fileWriter->writeUINT16(TYPES::UINT16(this->mask_ids.size()));
			for(TYPES::UINT32 mask_id: this->mask_ids){
				fileWriter->writeINT16(TYPES::INT16(mask_id));
			}
		}
		
	}
	/*
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_SOUND){
		fileWriter->writeUINT32(this->get_objID());
		if(this->get_object_block()!=NULL){
			BADDR obj_addr = 0;
			this->get_object_block()->get_block_addr(awd_file, obj_addr);
			fileWriter->writeUINT32(obj_addr);// fill id
		}
		else
			fileWriter->writeUINT32(0);// this is a error. add a error-message to timeline
	}*/
		
    //this->command_properties->write_attributes(fileWriter, blockSettings);
}

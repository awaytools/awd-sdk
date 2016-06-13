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
#include "utils/util.h"

#include <bitset>
#include "files/file_writer.h"

#include "Stdafx.h"
using namespace IceCore;
#define SORTER	RadixSort

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
 

FrameCommandDisplayObject::FrameCommandDisplayObject()
{
	this->prev_obj=NULL;
	this->display_matrix=new GEOM::MATRIX2x3();
	this->color_matrix=new GEOM::ColorTransform();
	this->hasDisplayMatrix=false;
	this->hasColorMatrix=false;
	this->hasBlendModeChange=false;
	this->hasTargetMaskIDs=false;
	this->hasVisiblitiyChange=false;
	this->clip_depth=0;
	this->blendMode=0;
	this->isMask=false;
	this->calculated_mask=false;
	this->visible=true;
	this->reset_masks=false;
	this->mask=ANIM::mask_type::NONE;

}

FrameCommandDisplayObject::~FrameCommandDisplayObject()
{	
	
	for(GEOM::MATRIX2x3* mtx:this->matrix_parents)
		delete mtx;
	this->matrix_parents.clear();
	
	for(GEOM::ColorTransform* ct:this->color_transform_parents)
		delete ct;
	this->color_transform_parents.clear();
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
void
FrameCommandDisplayObject::set_mask_childs(std::vector<ANIM::TimelineChild_instance*> mask_childs){
	this->mask_childs=mask_childs;
}
void
FrameCommandDisplayObject::add_mask_childs(std::vector<ANIM::TimelineChild_instance*> mask_childs){
	this->mask_child_levels.push_back(mask_childs);
}

bool
FrameCommandDisplayObject::get_hasVisiblitiyChange(){
	return this->hasVisiblitiyChange;
}
void
FrameCommandDisplayObject::set_display_matrix(TYPES::F64* this_display_matrix)
{
	this->hasDisplayMatrix=true;
	this->display_matrix->set(this_display_matrix);
}
GEOM::MATRIX2x3*
FrameCommandDisplayObject::get_display_matrix()
{
	return this->display_matrix;
}
void
FrameCommandDisplayObject::set_color_matrix(TYPES::F64* color_matrix)
{
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
    this->instanceName = instanceName;
}
void
FrameCommandDisplayObject::set_visible(bool visible)
{
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
	this->hasBlendModeChange=true;
	this->blendMode=blendMode;
}
void
FrameCommandDisplayObject::set_clipDepth(TYPES::UINT32 clipDepth)
{
	this->hasTargetMaskIDs=true;
	this->clip_depth = clipDepth;
}
TYPES::UINT32
FrameCommandDisplayObject::get_clipDepth()
{
	return this->clip_depth;
}
bool
FrameCommandDisplayObject::compare_mask_values(std::vector<std::vector<ANIM::TimelineChild_instance* > > prev_mask_ids)
{
	if(prev_mask_ids.size()!=this->mask_child_levels.size())
		return false;
	for(int i=0; i<this->mask_child_levels.size(); i++){
		// if the lists contain same objects, they are also in same order.
		if(prev_mask_ids[i].size()!=this->mask_child_levels[i].size())
			return false;
		for(int k=0; k<this->mask_child_levels[i].size(); k++){
			if(prev_mask_ids[i][k]!=this->mask_child_levels[i][k])
				return false;
		}
	}
    return true;
}
double
FrameCommandDisplayObject::compareColorMatrix(TYPES::F64* color_matrix)
{
	int countvalid=0;
	
	for(int i=0; i<8;i++){
		if(this->color_matrix->get()[i]==color_matrix[i]){
			countvalid++;
		}
	}
	
    return countvalid/8;
}
double
FrameCommandDisplayObject::compareDisplayMatrix(TYPES::F64* display_matrix)
{
    
	int countvalid=0;
	for(int i=0; i<4;i++){
		if(this->display_matrix->get()[i]==display_matrix[i]){
			countvalid++;
		}
	}
	if(countvalid==4)
        this->display_matrix->hasOther=false;
    //this->display_matrix->hasOther=true;
	int countPos=0;
	for(int i=4; i<6;i++){
		if(this->display_matrix->get()[i]==display_matrix[i]){
			countvalid++;
			countPos++;
		}
	}
	if(countPos==2)
        this->display_matrix->hasPosition=false;
    //this->display_matrix->hasPosition=true;
	
    return double(countvalid)/double(6.0);
}
result
FrameCommandDisplayObject::get_command_info(std::string& info)
{
	if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_CHILD)
		info = "	AddChild";
	else if(this->get_command_type()==frame_command_type::FRAME_COMMAND_UPDATE)
		info = "	Update";
	else if(this->get_command_type()==frame_command_type::FRAME_COMMAND_ADD_BUTTON_CHILD)
		info = "	AddButton";


	if(this->child!=NULL)
		info += " | child-name: "+this->child->child->awd_block->get_name()+"| session= "+FILES::int_to_string(this->child->sessionID)+" | child-id: "+FILES::int_to_string(this->child->child->id)+" | depth: "+FILES::int_to_string(this->child->depth);
	else{
		info += " | obj_id: "+FILES::int_to_string(this->get_objID())+" | child-id: "+FILES::int_to_string(this->child->child->id)+" | depth: "+FILES::int_to_string(this->child->depth);
	
	}
	//if(this->get_hasTargetMaskIDs()){
		if(this->mask==ANIM::mask_type::MASK){
			info +=" | IS MASK";
		}
		else if(this->mask==ANIM::mask_type::MASKED){
			int test=0;
			for(std::vector<TimelineChild_instance*> one_level:this->mask_child_levels){
				info+=" | masks "+FILES::int_to_string(test)+": ";
				test++;
				for(TimelineChild_instance* one_mask_child:one_level)
					info+="child ID ="+FILES::int_to_string(one_mask_child->child->id)+" session= "+FILES::int_to_string(one_mask_child->sessionID)+" |";
			}
		}
	//}
	if(this->get_instanceName().size()>0)
		info += " | inst-name: '"+this->get_instanceName()+"'"; 

	if(this->get_hasDisplayMatrix()){
		GEOM::MATRIX2x3* thismtx = this->get_display_matrix();
		if(thismtx->get_save_type()!=0){
			TYPES::F64* mtx = thismtx->get();
			info += " | transform: "+FILES::int_to_string(thismtx->get_save_type())+" / ";
			for(int i = 0; i<6; i++){
				info+=" "+FILES::num_to_string(mtx[i]);
			}
		}
		
		for(GEOM::MATRIX2x3* one_mtx:matrix_parents){
			TYPES::F64* mtx = one_mtx->get();
			info += "\n | transform: ";
			for(int i = 0; i<6; i++){
				info+=" "+FILES::num_to_string(mtx[i]);
			}
		}
		
	}
	if(this->get_hasColorMatrix()){
		GEOM::ColorTransform* thismtx = this->get_color_matrix();
		TYPES::F64* mtx = thismtx->get();
		info += " | color-transform: ";
		for(int i = 0; i<8; i++){
			info+=" "+FILES::num_to_string(mtx[i]);
		}
	}
	if(this->get_hasVisiblitiyChange()){
		if(this->get_visible())
			info += " | visible: true";
		else
			info += " | visible: false";
	}
	if(this->get_hasBlendModeChange()){
		info += " | blendmode: "+ FILES::int_to_string(this->get_blendmode());
	}
	

	return AWD::result::AWD_SUCCESS;
}

double
FrameCommandDisplayObject::compare_to_command(FrameCommandDisplayObject* frameCommand)
{
	
	return 0;
}


void
FrameCommandDisplayObject::update_by_command(FrameCommandDisplayObject* frameCommand)
{
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
FrameCommandDisplayObject::calc_command_length(SETTINGS::BlockSettings * blockSettings)
{
	int props_length=0;
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
		


    int len = sizeof(TYPES::UINT8); // command_type
	len+=sizeof(TYPES::UINT16);// objectID
	if(this->get_command_type()!=frame_command_type::FRAME_COMMAND_UPDATE){
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

    return len;
}

void
FrameCommandDisplayObject::finalize_command()
{
	
	// check if we need matrix for this command
    
	if(this->get_hasDisplayMatrix()){
		if(this->prev_obj==NULL){
			if(this->display_matrix->is_identity()){
				this->hasDisplayMatrix=false;
			}
		}
		else{
			FrameCommandDisplayObject* previous_cmd=this->prev_obj;
            if(previous_cmd->get_hasDisplayMatrix()){
                if(this->compareDisplayMatrix(previous_cmd->get_display_matrix()->get())==1){
                    // check if matrix has changed. if not, disable mtx for this command
                    this->hasDisplayMatrix=false;
                }
			}
			else{
				while(previous_cmd->prev_obj!=NULL){
					if(previous_cmd->prev_obj->get_hasDisplayMatrix()){
                        if(this->compareDisplayMatrix(previous_cmd->prev_obj->get_display_matrix()->get())==1){
                            // check if matrix has changed. if not, disable mtx for this command
                            this->hasDisplayMatrix=false;
                        }
						break;
					}
					previous_cmd=previous_cmd->prev_obj;
				}
			}
		}
	}
    
	
	// check if we need colortransform for this command
	if(this->get_hasColorMatrix()){
		if(this->prev_obj==NULL){
			// check if it is identity. if not. apply
			if(this->color_matrix->is_identity()){
				this->hasColorMatrix=false;
			}
		}
		else{
			FrameCommandDisplayObject* previous_cmd=this->prev_obj;
			if(previous_cmd->get_hasColorMatrix()){
				if(this->compareColorMatrix(previous_cmd->get_color_matrix()->get())==1)// check if matrix has changed. if yes. apply.
					this->hasColorMatrix=false;
			}
			else{
				while(previous_cmd->prev_obj!=NULL){
					if(previous_cmd->prev_obj->get_hasColorMatrix()){
						if(this->compareColorMatrix(previous_cmd->prev_obj->get_color_matrix()->get())==1)
							this->hasColorMatrix=false;
						break;
					}
					previous_cmd=previous_cmd->prev_obj;
				}
			}
		}
	}
	
	this->hasTargetMaskIDs=false;
	if(this->mask==ANIM::mask_type::MASK){
		this->hasTargetMaskIDs=true;
		if(this->prev_obj!=NULL){
			FrameCommandDisplayObject* previous_cmd=this->prev_obj;
			if(previous_cmd->mask==ANIM::mask_type::MASK){
				this->hasTargetMaskIDs=false;
			}
			else{
				while(previous_cmd->prev_obj!=NULL){
					if(previous_cmd->prev_obj->mask==ANIM::mask_type::MASK){
						this->hasTargetMaskIDs=false;
						break;
					}
					previous_cmd=previous_cmd->prev_obj;
				}
			}
		}
	}	
	else if(this->mask==ANIM::mask_type::MASKED){
		this->hasTargetMaskIDs=true;
		if(this->prev_obj!=NULL){
			FrameCommandDisplayObject* previous_cmd=this->prev_obj;
			if(this->prev_obj->mask==ANIM::mask_type::MASKED){
				if(this->compare_mask_values(previous_cmd->mask_child_levels))
					this->hasTargetMaskIDs=false;
			}
			else{
				while(previous_cmd->prev_obj!=NULL){
					if(previous_cmd->prev_obj->mask==ANIM::mask_type::MASKED){
						if(this->compare_mask_values(previous_cmd->prev_obj->mask_child_levels)){
							this->hasTargetMaskIDs=false;
						}
						break;
					}
					previous_cmd=previous_cmd->prev_obj;
				}
			}
		}
	}
	if(this->prev_obj!=NULL){
		if(this->prev_obj->mask==ANIM::mask_type::MASKED){
			//if(this->mask==ANIM::mask_type::NONE)
			//	this->reset_masks=true;
		}
	}
	
}

bool
FrameCommandDisplayObject::has_update_properties()
{

	if(this->get_hasDisplayMatrix())
		return true;	
		
	if(this->get_hasColorMatrix())
		return true;
	
	if(this->get_hasBlendModeChange())
		return true;
	
	if(this->get_hasVisiblitiyChange())
		return true;

	if(this->instanceName!="")
		return true;

	if(this->hasTargetMaskIDs)
		return true;

	return false;
	
}
bool
FrameCommandDisplayObject::has_active_properties()
{
	if(this->command_type!=frame_command_type::FRAME_COMMAND_UPDATE)
		return true;	

	if(this->get_hasDisplayMatrix())
		return true;	
		
	if(this->get_hasColorMatrix())
		return true;
	
	if(this->get_hasBlendModeChange())
		return true;
	
	if(this->get_hasVisiblitiyChange())
		return true;

	if(this->instanceName!="")
		return true;

	if(this->hasTargetMaskIDs)
		return true;

	return false;
	
}
void
FrameCommandDisplayObject::resolve_parenting()
{
	
	// write properties
	//if(false){
	if (this->get_hasDisplayMatrix()){
		if(this->matrix_parents.size()>0){
			// matrix_parents contains list of parent matrices. 
			// the last matrix in the list is the matrix of root-object
			// both version give same results

			
			// version1: starting at bottom, working way up to the root object
			for(GEOM::MATRIX2x3* new_matrix:this->matrix_parents)
				this->display_matrix->append(new_matrix);
			// end version1
			/*

			// version2: traverse hirarchy top to bottom
			int mtx_cnt=this->matrix_parents.size()-1;
			GEOM::MATRIX2x3* new_matrix=this->matrix_parents[mtx_cnt];
			while(mtx_cnt--)
				new_matrix->prepend(this->matrix_parents[mtx_cnt]);
			new_matrix->prepend(this->display_matrix);
			this->display_matrix->set(new_matrix->get());
			*/
			// end version2
		}
	}
	if (this->get_hasColorMatrix()){
		if(this->color_transform_parents.size()>0){
			int mtx_cnt=this->color_transform_parents.size()-1;
			// matrix_parents contains list of parent matrix. last matrix in this list is the matrix of root-object
				
			for(GEOM::ColorTransform* new_ct:this->color_transform_parents)
				this->color_matrix->prepend(new_ct);
		}
	}
	// sort mask values 
	/*
	if (this->get_hasTargetMaskIDs()){
		if(this->mask_ids.size()>0){
			udword* InputValues_depth = new udword[ this->mask_ids.size()];
			int cmd_cnt=0;
			for(TYPES::INT32 one_mask_value:this->mask_ids){
				InputValues_depth[cmd_cnt++]=one_mask_value+1;
			}
			SORTER RS;
			const udword* Sorted = RS.Sort(InputValues_depth, this->mask_ids.size(), RADIX_UNSIGNED).GetRanks();
			int sorted_cnt=this->mask_ids.size();
			this->mask_ids.clear();
			while(sorted_cnt--){
				this->mask_ids.push_back(InputValues_depth[Sorted[sorted_cnt]]-1);
			}
			DELETEARRAY(InputValues_depth);
			//DELETEARRAY(Sorted);
		}
	}
	*/
	

}
void
FrameCommandDisplayObject::write_command(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * blockSettings, AWDFile* awd_file)
{

	fileWriter->writeUINT8(TYPES::UINT8(this->get_command_type()));
	std::bitset<16> props_flag(0);
	fileWriter->writeUINT16(this->child->child->id);
	if(this->get_command_type()!=frame_command_type::FRAME_COMMAND_UPDATE){
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
	if(this->get_command_type()!=frame_command_type::FRAME_COMMAND_UPDATE){
		if(this->instanceName.size()>0)
			fileWriter->writeSTRING(instanceName, FILES::write_string_with::LENGTH_AS_UINT16);
	}		
	if(this->mask_ids.size()>0){
		fileWriter->writeUINT16(TYPES::UINT16(this->mask_ids.size()));
		for(TYPES::UINT32 mask_id: this->mask_ids){
			fileWriter->writeINT16(TYPES::INT16(mask_id));
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


void
FrameCommandDisplayObject::set_depth(TYPES::UINT32 depth)
{
    this->depth = depth;
}
TYPES::UINT32
FrameCommandDisplayObject::get_depth()
{
	return this->depth;
}
TYPES::UINT32
FrameCommandDisplayObject::get_objID()
{
	return this->objID;
}
void
FrameCommandDisplayObject::set_objID(TYPES::UINT32 objID)
{
    this->objID = objID;
}
void
FrameCommandDisplayObject::set_object_block(BASE::AWDBlock * object_block)
{
    this->object_block = object_block;
}
BASE::AWDBlock*
FrameCommandDisplayObject::get_object_block()
{
    return this->object_block;
}
frame_command_type
FrameCommandDisplayObject::get_command_type()
{
    return this->command_type;
}
void
FrameCommandDisplayObject::set_command_type(frame_command_type command_type)
{
    this->command_type=command_type;
}


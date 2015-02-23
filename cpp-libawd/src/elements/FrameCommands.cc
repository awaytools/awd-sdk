#include "elements/frame_commands.h"
#include "elements/stream.h"
#include "utils/awd_types.h"
#include "base/attr.h"
#include "utils/settings.h"
#include "blocks/geometry.h"
#include "blocks/material.h"
#include "blocks/text_element.h"
#include "utils/awd_properties.h"

#include "files/file_writer.h"

using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;
 
FrameCommandBase::FrameCommandBase() 
{
    this->command_properties=new ATTR::NumAttrList();
	this->command_type = frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE;
	this->hasRessource=false;// if true than the command will be "add" not "update"
	this->objID = -1;
	this->object_block = NULL;
	this->hasRessource=false;// if true than the command will be "add" not "update"
}

FrameCommandBase::~FrameCommandBase()
{
    delete this->command_properties;
	this->object_block = NULL;
}

result
FrameCommandBase::get_command_info(std::string& info)
{
	return get_command_info_specific(info);
}
result
FrameCommandBase::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	return collect_dependencies_specific(target_file, instance_type);
}

double
FrameCommandBase::compare_to_command(FrameCommandBase * frame_command)
{
	return compare_to_command_specific(frame_command);
}

void
FrameCommandBase::update_by_command(FrameCommandBase * frame_command)
{
	update_by_command_specific(frame_command);
}
void
FrameCommandBase::write_command(FILES::FileWriter * fileWriter,  SETTINGS::BlockSettings * settings, AWDFile* awd_file)
{
	write_command_specific(fileWriter, settings, awd_file);
}
TYPES::UINT32
FrameCommandBase::calc_command_length( SETTINGS::BlockSettings * settings)
{
	return calc_command_length_specific(settings);
}
std::string&
FrameCommandBase::get_objectType()
{
	return this->objectType;
}
void
FrameCommandBase::set_objectType(const std::string& objectType)
{
    this->objectType = objectType;
}
std::string&
FrameCommandBase::get_resID()
{
	return this->resID;
}
void
FrameCommandBase::set_resID(const std::string& new_res)
{
	this->hasRessource=true;
    this->resID = new_res;
}
TYPES::UINT32
FrameCommandBase::get_objID()
{
	return this->objID;
}
void
FrameCommandBase::set_objID(TYPES::UINT32 objID)
{
    this->objID = objID;
}
void
FrameCommandBase::set_object_block(BASE::AWDBlock * object_block)
{
    this->object_block = object_block;
}
BASE::AWDBlock*
FrameCommandBase::get_object_block()
{
    return this->object_block;
}
ATTR::NumAttrList *
FrameCommandBase::get_command_properties()
{
    return this->command_properties;
}
frame_command_type
FrameCommandBase::get_command_type()
{
    return this->command_type;
}
void
FrameCommandBase::set_layerID(TYPES::UINT32 layerID)
{
	this->layerID = layerID;
}
TYPES::UINT32
FrameCommandBase::get_layerID()
{
	return this->layerID;
}
void
FrameCommandBase::set_command_type(frame_command_type command_type)
{
    this->command_type=command_type;
}
bool
FrameCommandBase::get_hasRessource()
{
    return this->hasRessource;
}
void
FrameCommandBase::set_hasRessource(bool hasRessource)
{
    this->hasRessource=hasRessource;
}

FrameCommandDisplayObject::FrameCommandDisplayObject() :
	FrameCommandBase()
{

	this->display_matrix=new GEOM::MATRIX2x3();	
	this->color_matrix=new GEOM::MATRIX4x5();
	this->hasDisplayMatrix=false;
	this->hasColorMatrix=false;
	this->hasDepthChange=false;
	this->hasFilterChange=false;
	this->hasBlendModeChange=false;
	this->hasDepthClipChange=false;
	this->hasVisiblitiyChange=false;
	this->clip_depth=0;
	this->blendMode=0;


	TYPES::union_ptr default_union;
	default_union.v=malloc(4);
	*default_union.mtx=0;
	this->command_properties->add(PROP_FRAMECOMMAND_DISPLAYMATRIX2D,	default_union, 4,   data_types::MTX3x2, storage_precision_category::MATRIX_VALUES, property_storage_type::SCALED_PROPERTY);
	
	default_union.v=malloc(4);
	*default_union.mtx=0;
	this->command_properties->add(PROP_FRAMECOMMAND_DISPLAYMATRIX3D,	default_union, 4,   data_types::MTX4x3, storage_precision_category::MATRIX_VALUES, property_storage_type::SCALED_PROPERTY);

	default_union.v=malloc(4);
	*default_union.mtx=0;
	this->command_properties->add(PROP_FRAMECOMMAND_COLORTRANSFORM,	default_union, 4,   data_types::MTX5x4, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(1);
	*default_union.ui8=0;
	this->command_properties->add(PROP_FRAMECOMMAND_BLENDMODE,	default_union, 1,   data_types::UINT8, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(1);
	*default_union.ui8=0;
	this->command_properties->add(PROP_FRAMECOMMAND_VISIBLITY,	default_union, 1,   data_types::UINT8, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(4);
	*default_union.ui32=0;
	this->command_properties->add(PROP_FRAMECOMMAND_DEPTH,	default_union, 4,   data_types::UINT32, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);
	
	default_union.v=malloc(4);
	*default_union.ui32=0;
	this->command_properties->add(PROP_FRAMECOMMAND_MASK,	default_union, 4,   data_types::UINT32, storage_precision_category::UNDEFINED_STORAGE_PRECISION, property_storage_type::STATIC_PROPERTY);

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
FrameCommandDisplayObject::get_hasDepthClipChange(){
	return this->hasDepthClipChange;
}
bool
FrameCommandDisplayObject::get_hasVisiblitiyChange(){
	return this->hasVisiblitiyChange;
}
void
FrameCommandDisplayObject::set_display_matrix(TYPES::F64* display_matrix)
{
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
	this->hasColorMatrix=true;
    this->color_matrix->set(color_matrix);
}
GEOM::MATRIX4x5*
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
	this->blendMode=blendMode;
}
void
FrameCommandDisplayObject::set_clipDepth(TYPES::UINT32 clipDepth)
{
	this->hasDepthClipChange=true;
	this->clip_depth = clipDepth;
}
TYPES::UINT32
FrameCommandDisplayObject::get_clipDepth()
{
	return this->clip_depth;
}
void
FrameCommandDisplayObject::set_depth(TYPES::UINT32 depth)
{
	this->hasDepthChange=true;
    this->depth = depth;
}
TYPES::UINT32
FrameCommandDisplayObject::get_depth()
{
	return this->depth;
}
double
FrameCommandDisplayObject::compareColorMatrix(TYPES::F64* color_matrix)
{
	int countvalid=0;
	
	/*for(int i=0; i<20;i++){
		if(this->color_matrix[i]==color_matrix[i]){
			countvalid++;
		}
	}
	*/
    return countvalid/20;
}
double
FrameCommandDisplayObject::comparedisplaMatrix(TYPES::F64* display_matrix)
{
	int countvalid=0;
	/*
	for(int i=0; i<6;i++){
		if(this->display_matrix[i]==display_matrix[i]){
			countvalid++;
		}
	}
	*/
    return countvalid/6;
}
result
FrameCommandDisplayObject::get_command_info_specific(std::string& info)
{
	std::string cmdtype="";
	if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)
		cmdtype="AddLocal";
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE)
		cmdtype="AddGlobal";
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_REMOVE_OBJECT)
		cmdtype="Remove";
	//else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_SOUND)
	//	cmdtype="Sound";
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_UPDATE)
		cmdtype="Update";

	info = "	cmd-type: "+cmdtype+" | obj-id: "+std::to_string(this->get_objID())+" | depth: "+std::to_string(this->get_depth());
	if(this->get_hasDepthClipChange())
		info +=" | mask-id: "+std::to_string(this->get_clipDepth()); 
	if(this->get_object_block()!=NULL)
		info += " | obj-name: '"+this->get_object_block()->get_name()+"'"; 	
	if(this->get_instanceName().size()>0)
		info += " | inst-name: '"+this->get_instanceName()+"'"; 
	if((this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)||(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE)){
		info += " | res-id: "+this->get_resID(); 
	}
	if(this->get_hasDisplayMatrix()){
		GEOM::MATRIX2x3* thismtx = this->get_display_matrix();
		TYPES::F64* mtx = thismtx->get();
		info += " | transform: ";
		for(int i = 0; i<6; i++){
			info+=" "+std::to_string(mtx[i]);
		}
	}
	if(this->get_hasColorMatrix()){
		GEOM::MATRIX4x5* thismtx = this->get_color_matrix();
		TYPES::F64* mtx = thismtx->get();
		info += " | color-transform: ";
		for(int i = 0; i<20; i++){
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
	if(this->get_command_type()!=thisFC->get_command_type()){
		return 0;
	}
	if(this->get_object_block()!=thisFC->get_object_block()){
		return 0;
	}
	// the commands 
	int equal=1;
	int equalCnt=1;

	if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(this->hasColorMatrix)){
		//equal += this->compareColorMatrix(thisFC->get_color_matrix());
	}	

	else if((this->hasColorMatrix==thisFC->get_hasColorMatrix())&&(!this->hasColorMatrix)){
		equal += 1;}	
	equalCnt++;

	if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(this->hasDisplayMatrix)){
		//equal+=this->comparedisplaMatrix(thisFC->get_display_matrix());
	}
	else if((this->hasDisplayMatrix==thisFC->get_hasDisplayMatrix())&&(!this->hasDisplayMatrix)){
		equal+=1;}	
	equalCnt++;
	
	if((this->hasDepthChange==thisFC->get_hasDepthChange())&&(this->hasDepthChange)){
		if(this->get_depth()==thisFC->get_depth()){
			equal+=1;
		}	
	}
	else if((this->hasDepthChange==thisFC->get_hasDepthChange())&&(!this->hasDepthChange)){
		equal+=1;}	
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
	
	double returnEqual=double(equal)/double(equalCnt);
	return returnEqual;
}
void
FrameCommandDisplayObject::update_by_command_specific(FrameCommandBase* frameCommand)
{
	int equal=0;
}

result
FrameCommandDisplayObject::collect_dependencies_specific(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{
	if (this->get_object_block()!=NULL) {	

		this->get_object_block()->add_with_dependencies(target_file, instance_type);

		if(this->get_object_block()->get_type()==block_type::TEXT_ELEMENT){
			BLOCKS::TextElement* thisText = reinterpret_cast<BLOCKS::TextElement*>(this->get_object_block());
			if(thisText!=NULL){
				if(thisText->get_isLocalized()){
					this->set_command_type(frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE);
				}
			}
		}
		if(this->get_object_block()->get_type()==block_type::TRI_GEOM){
			BLOCKS::Geometry* thisShape = reinterpret_cast<BLOCKS::Geometry*>(this->get_object_block());
			if(thisShape!=NULL){
				std::vector<BASE::AWDBlock*> material_list;
				thisShape->get_material_blocks(material_list);
				for(AWDBlock * oneMat : material_list){
					if(oneMat!=NULL){
						BLOCKS::Material* this_mat = reinterpret_cast<BLOCKS::Material*>(oneMat);
						if(this_mat!=NULL){	
							this_mat->add_with_dependencies(target_file, instance_type);	
						}
					}
				}
			}
		}
    }
	return result::AWD_SUCCESS;
}

TYPES::UINT32
FrameCommandDisplayObject::calc_command_length_specific(SETTINGS::BlockSettings * blockSettings)
{
	
	if(this->get_command_type()!=frame_command_type::AWD_FRAME_COMMAND_REMOVE_OBJECT){
		
		if(this->hasDisplayMatrix){
			//if((this->get_command_type()!=frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)&&(this->get_command_type()!=frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE)){
			TYPES::union_ptr properties_union;
			properties_union.v=malloc(sizeof(TYPES::F64)*6);
			properties_union.mtx=this->display_matrix->get();
			this->command_properties->set(PROP_FRAMECOMMAND_DISPLAYMATRIX2D, properties_union, TYPES::UINT32(sizeof(TYPES::F32)*6));
		}
		
		if(this->hasColorMatrix){
			TYPES::union_ptr properties_union;
			properties_union.v=malloc(sizeof(TYPES::F64)*20);
			properties_union.mtx=this->color_matrix->get();
			this->command_properties->set(PROP_FRAMECOMMAND_COLORTRANSFORM, properties_union, TYPES::UINT32(sizeof(TYPES::F32)*20));
		}

		if(this->hasBlendModeChange){
			TYPES::union_ptr properties_union;
			properties_union.v=malloc(1);
			*properties_union.ui8=this->blendMode;
			this->command_properties->set(PROP_FRAMECOMMAND_BLENDMODE, properties_union, 1);
		}
		if(this->hasDepthChange){
			TYPES::union_ptr properties_union;
			properties_union.v=malloc(4);
			*properties_union.ui32=this->depth;
			this->command_properties->set(PROP_FRAMECOMMAND_DEPTH, properties_union, 4);
		}
		if(this->hasDepthClipChange){
			TYPES::union_ptr properties_union;
			properties_union.v=malloc(4);
			*properties_union.ui32=this->clip_depth;
			this->command_properties->set(PROP_FRAMECOMMAND_MASK, properties_union, 4);
		}
		if(this->hasVisiblitiyChange){
			TYPES::union_ptr properties_union;
			properties_union.v=malloc(1);
			*properties_union.ui8=this->visible;
			this->command_properties->set(PROP_FRAMECOMMAND_VISIBLITY, properties_union, 1);
		}
		
	}		

    int len;
    len = sizeof(TYPES::UINT16); // command_type
	if((this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE)||(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)||(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_UPDATE)){
		
		len+=sizeof(TYPES::UINT32);// objectID
		if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE){
			len+=sizeof(TYPES::UINT32);
			len+=sizeof(TYPES::UINT16);// num sub shapes
			BASE::AWDBlock * thisRefObj = this->get_object_block();
			if(thisRefObj!=NULL){			
				if(thisRefObj->get_type()==block_type::TRI_GEOM){
					BLOCKS::Geometry* thisShape = reinterpret_cast<BLOCKS::Geometry*>(this->get_object_block());
					if(thisShape!=NULL){
						len+=thisShape->get_num_subs()*sizeof(TYPES::UINT32);
					}
				}			
			}
			len+=sizeof(TYPES::UINT16);// length instance_name
			if(this->get_instanceName().size()!=0){
				len += TYPES::UINT16(this->get_instanceName().size());//
			}
		}
		else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE){
			len+=sizeof(TYPES::UINT32);				
			BASE::AWDBlock* thisText = this->get_object_block();
			if(thisText!=NULL){
				len += sizeof(TYPES::UINT16) +  TYPES::UINT16(thisText->get_name().size());//
			}
			else{
				len+=sizeof(TYPES::UINT16);
			}						
		}	
		len += this->command_properties->calc_length(blockSettings); 
	}
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_SOUND){
		len+=sizeof(TYPES::UINT32);// objectID
		len+=sizeof(TYPES::UINT32);// resID
	}
		
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_REMOVE_OBJECT){
		len+=sizeof(TYPES::UINT32);// objectID
	}
    return len;
}

void
FrameCommandDisplayObject::write_command_specific(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings * blockSettings, AWDFile* awd_file)
{
	
	fileWriter->writeUINT16(TYPES::UINT16(this->get_command_type()));// command type
	if((this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE)||(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE)||(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_UPDATE)){
		
		fileWriter->writeUINT32(this->get_objID());
		if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_LOCAL_RESSOURCE){
			if(this->get_object_block()!=NULL){
				BADDR obj_addr = 0;
				this->get_object_block()->get_block_addr(awd_file, obj_addr);
				this->set_resID(std::to_string(obj_addr));
				fileWriter->writeUINT32(obj_addr);// fill id				
				AWDBlock * thisRefObj = this->get_object_block();
				if(thisRefObj!=NULL){
					if(thisRefObj->get_type()==BLOCK::block_type::TRI_GEOM){
						BLOCKS::Geometry* thisShape = reinterpret_cast<BLOCKS::Geometry*>(this->get_object_block());
						if(thisShape!=NULL){
							fileWriter->writeUINT16(thisShape->get_num_subs()); // num fills (subShape)
							std::vector<BASE::AWDBlock*> material_list;
							thisShape->get_material_blocks(material_list);
							for(AWDBlock * oneMat : material_list){
								if(oneMat!=NULL){	
									BADDR mat_addr = 0;
									oneMat->get_block_addr(awd_file, mat_addr);
									fileWriter->writeUINT32(mat_addr);// fill id
								}
								else{
									fileWriter->writeUINT32(0); // num fills (subShape)
								}
							}
						}
						else{
							fileWriter->writeUINT16(0); // num fills (subShape)
						}
					}
					// this is no shape (it is timeline or something)
					else{
						fileWriter->writeUINT16(0); // num fills (subShape)
					}
				}
				else{//ERROR OCCURED, write 0 for subshapes 
					fileWriter->writeUINT16(0); // num fills (subShape)
				}
				if(this->instanceName.size()>0){
					fileWriter->writeSTRING(instanceName, FILES::write_string_with::LENGTH_AS_UINT16);
				}
				else{
					fileWriter->writeUINT16(0); // num fills (subShape)
				}
			}
			else
				fileWriter->writeUINT32(0);// this is a error. add a error-message to timeline
		}
		else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_ADD_GLOBAL_RESSOURCE){
			BLOCKS::TextElement* thisText = reinterpret_cast<BLOCKS::TextElement*>(this->get_object_block());
			if(thisText!=NULL){
				fileWriter->writeSTRING(thisText->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);
			}
			else
				fileWriter->writeUINT16(0); // no id found (error)						
		}	
		this->command_properties->write_attributes(fileWriter, blockSettings);	
		
	}
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_SOUND){
		fileWriter->writeUINT32(this->get_objID());
		if(this->get_object_block()!=NULL){
			BADDR obj_addr = 0;
			this->get_object_block()->get_block_addr(awd_file, obj_addr);
			fileWriter->writeUINT32(obj_addr);// fill id
		}
		else
			fileWriter->writeUINT32(0);// this is a error. add a error-message to timeline
	}
		
	else if(this->get_command_type()==frame_command_type::AWD_FRAME_COMMAND_REMOVE_OBJECT){
		fileWriter->writeUINT32(this->get_objID());
	}	
    //this->command_properties->write_attributes(fileWriter, blockSettings);
}

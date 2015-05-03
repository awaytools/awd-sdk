#include "blocks/namespace.h"
#include "utils/awd_types.h"
#include "base/block.h"
#include "base/state_element_base.h"

#include "files/file_writer.h"


using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;

using namespace AWD::SETTINGS;
using namespace AWD::BLOCKS;

Namespace::Namespace() :
	AWDBlock(BLOCK::block_type::NAMESPACE)
{
}

Namespace::~Namespace()
{
}

result
Namespace::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{	
	return result::AWD_SUCCESS;
}

TYPES::state 
Namespace::validate_block_state()
{	
	if(this->name_spaces.size()==0){
		this->set_state(state::EMPTY);
		return state::EMPTY;
	}
	if(this->name_spaces.size()==1)
		this->set_type(BLOCK::block_type::NAMESPACE);
	if(this->name_spaces.size()>1)
		this->set_type(BLOCK::block_type::NAMESPACES);
	
	return TYPES::state::VALID;
}

result
Namespace::find_namespace_by_handle(TYPES::UINT8  name_space_handle)
{
	return result::AWD_SUCCESS;
}

result
Namespace::add_namespace(const std::string& name_space, TYPES::UINT8 ns_handle)
{
	if(name_space.empty()){
		return result::AWD_ERROR;
	}
	for(NameSpace_struct existing_ns : this->name_spaces){
		if((existing_ns.name_space==name_space) && (existing_ns.handle==ns_handle))
			return result::AWD_ERROR;
		if(existing_ns.name_space==name_space)
			return result::AWD_ERROR;
		if(existing_ns.handle==ns_handle)
			return result::AWD_ERROR;
	}
	NameSpace_struct new_ns = NameSpace_struct(name_space, ns_handle);
	this->name_spaces.push_back(new_ns);
	return result::AWD_SUCCESS;
}

TYPES::UINT32
Namespace::calc_body_length(AWDFile* awd_file, BlockSettings * curBlockSetting)
{
	if(this->check_state()!=state::VALID)
		return 0;
	
	TYPES::UINT32 length=0;
	for(NameSpace_struct existing_ns : this->name_spaces)
		length += TYPES::UINT32(sizeof(NS_ID) + sizeof(TYPES::UINT16) + existing_ns.name_space.size());
	return length;
}

result
Namespace::write_body(FileWriter * fileWriter, BlockSettings * settings, AWDFile* file)
{
	if(this->get_state()!=state::VALID)
		return result::AWD_ERROR;

	for(NameSpace_struct existing_ns : this->name_spaces){
		fileWriter->writeUINT8(existing_ns.handle);
		fileWriter->writeSTRING(existing_ns.name_space, FILES::write_string_with::LENGTH_AS_UINT16);
	}
	return result::AWD_SUCCESS;
}
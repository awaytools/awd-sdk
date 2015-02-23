#include "base/state_element_base.h"

#include "utils/util.h"

using namespace AWD;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BASE;
using namespace AWD::BLOCK;

using namespace AWD::SETTINGS;
using namespace AWD::MATERIAL;

StateElementBase::StateElementBase()
{
	this->errors_cnt=0;
	this->warnings_cnt=0;
	this->state = TYPES::state::EMPTY;
	this->process_state = TYPES::process_state::UNPROCESSED;
}

StateElementBase::~StateElementBase()
{
}

TYPES::state 
StateElementBase::check_state()
{
	
	this->state=this->validate_state();
	return this->state ;//this->state;
}
TYPES::state 
StateElementBase::get_state()
{
	return this->state;//this->state;
}

void 
StateElementBase::set_state(TYPES::state state, bool reset)
{
	this->state = state;
	if(reset)
		this->clear_messages();	
}

TYPES::process_state 
StateElementBase::get_process_state()
{
	return this->process_state;
}

void 
StateElementBase::set_process_state(TYPES::process_state process_state)
{
	this->process_state = process_state;
}

result 
StateElementBase::get_messages(std::vector<std::string>& output_string, const std::string& intend)
{
	for(TYPES::Message message : this->messages)
		output_string.push_back(intend+message.message);
	return result::AWD_SUCCESS;
}

result
StateElementBase::get_warnings(std::vector<std::string>& output_string, const std::string& intend)
{
	if((this->errors_cnt>0)||(this->warnings_cnt>0)){
		for(TYPES::Message message : this->messages){
			if(message.type!=TYPES::message_type::STATUS_MESSAGE)
				output_string.push_back(intend+message.message);
		}
	}
	if(output_string.size()==0)
		return result::AWD_ERROR;	
	return result::AWD_SUCCESS;
}				

result 
StateElementBase::get_errors(std::vector<std::string>& output_string, const std::string& intend)
{
	if(this->errors_cnt>0){
		for(TYPES::Message message : this->messages){
			if(message.type==TYPES::message_type::ERROR_MESSAGE)
				output_string.push_back(intend+message.message);
		}
	}
	if(output_string.size()==0)
		return result::AWD_ERROR;	
	return result::AWD_SUCCESS;
}

void 
StateElementBase::clear_messages()
{
	this->messages.clear();
	this->errors_cnt=0;
	this->warnings_cnt=0;

}

void
StateElementBase::add_message(const std::string& message_str, TYPES::message_type new_message_type)
{
	this->messages.push_back(Message(message_str, new_message_type));
	if(new_message_type==TYPES::message_type::ERROR_MESSAGE)
		this->errors_cnt++;
	if(new_message_type==TYPES::message_type::WARNING_MESSAGE)
		this->warnings_cnt++;
}
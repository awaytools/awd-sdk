#ifndef _LIBAWD_STATEELEMENTBASE_H
#define _LIBAWD_STATEELEMENTBASE_H

#include <string>
#include <vector>

#include "utils/awd_types.h"
#include "utils/settings.h"
#include "files/file_writer.h"

namespace AWD{	
	
	namespace BASE	{
		/**\brief Classes that implement StateElementBase can be checked for state, process_state and messages. Should never be instantiated directly.
		* 
		* 
		*/
		class StateElementBase
		{
			private:
				std::vector<TYPES::Message> messages;
				TYPES::UINT32 warnings_cnt;
				TYPES::UINT32 errors_cnt;
				
			protected:
				
				TYPES::state state;				
				TYPES::process_state process_state;

				virtual TYPES::state validate_state()=0;

			public:
				/**\brief StateElementBase is a base-class and should never be created directly.
				*/
				StateElementBase();
				~StateElementBase();
						
				/**\brief Get the state of this StateElementBase
				* @return The state for this StateElementBase
				*/
				TYPES::state get_state();

				/**\brief Get the state of this StateElementBase
				* @return The state for this StateElementBase
				*/
				TYPES::state check_state();

				/**\brief Set the state of this StateElementBase
				* @param[in] state the state to set for this StateElementBase
				* @param[in] reset [optional] if true, the message list will be cleared
				*/
				void set_state(TYPES::state state, bool reset = false);
				
				/**\brief Get the process_state of this StateElementBase
				* @return The process_state for this StateElementBase
				*/
				TYPES::process_state get_process_state();

				/**\brief Set the process_state of this StateElementBase
				* @param[in] state the process_state to set for this StateElementBase
				*/
				void set_process_state(TYPES::process_state state);

				/**\brief get all messages saved for this StateElementBase
				* @return 
				*/
				result get_messages(std::vector<std::string>&, const std::string& intent);

				/**\brief get all warning and error messages that have been saved for this StateElementBase
				* @return
				*/
				result get_warnings(std::vector<std::string>& output, const std::string& intent);
				
				/**\brief get only error warning messages that have been saved for this StateElementBase
				* @return
				*/
				result get_errors(std::vector<std::string>&, const std::string& intent);
				
				/**\brief add a message for this StateElementBase
				* @return
				*/
				void add_message(const std::string& message_str, TYPES::message_type);

				/**\brief clear_messages
				* Clear the list of messages for this Element
				*/
				void clear_messages();
		};	
	}
}

#endif

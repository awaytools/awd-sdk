#ifndef _LIBAWD_ATTR_H
#define _LIBAWD_ATTR_H

#include <string>
#include <vector>
#include "utils/awd_types.h"
#include "blocks/namespace.h"
#include "files/file_writer.h"
#include "files/file_reader.h"
#include "utils/settings.h"

namespace AWD
{
	namespace ATTR
	{
		#define ATTR_RETURN_NULL union_ptr _ptr; _ptr.v = NULL; return _ptr;

		typedef TYPES::UINT16 awd_propkey;
	
		/** \class AttrBase
		*	\brief The BaseClass for all attributes. Should never be instantiated directly.
		*/
		class AttrBase
		{
			protected:
				TYPES::data_types type;
				TYPES::union_ptr value;							
				TYPES::UINT32 value_len;							
		

			public:
				AttrBase();
				~AttrBase();
				result write_data(FILES::FileWriter*, bool storage_precision, bool use_scale);
				
				TYPES::union_ptr get_val(TYPES::UINT32 * value_length, TYPES::data_types * data_type);
				TYPES::UINT32 get_val_len(SETTINGS::BlockSettings *);
		};
	
		/** \class UserAttr
		*	\brief The UserAttr defines one CustomAttribute that can be added to some of the AWDBlocks.
		*	
		* >		__Away3D__\n
		* >		The custom Attributes will be loaded as "Extra"-object on the Away3d objects.\n
		*/
		class UserAttr :
			public AttrBase
		{
			private:
				std::string key;
				TYPES::UINT16 key_len;
				BLOCKS::Namespace *ns;


			public:
				UserAttr(BLOCKS::Namespace *, std::string&);
				~UserAttr();
				result set(TYPES::union_ptr, TYPES::UINT32, TYPES::data_types);
				
				result write_user_attr(FILES::FileWriter*);
				BLOCKS::Namespace *get_ns();
				std::string& get_key();
		};
	
	
		/** \class UserAttrList
		*	\brief A list of UserAttr (custom-block-attributes).
		*/
		class UserAttrList {
			private:		
				std::vector<ATTR::UserAttr*> attributes;
				ATTR::UserAttr *find(BLOCKS::Namespace *, std::string& );

			public:
				UserAttrList();
				~UserAttrList();

				TYPES::UINT32 calc_length(SETTINGS::BlockSettings *);
				result write_attributes(FILES::FileWriter*, SETTINGS::BlockSettings *);

				TYPES::union_ptr get_val_ptr(BLOCKS::Namespace *ns, std::string&);
				result get(BLOCKS::Namespace *, std::string& , TYPES::union_ptr *, TYPES::UINT32 *, TYPES::data_types *);
				result set(BLOCKS::Namespace *, std::string& , TYPES::union_ptr, TYPES::UINT32, TYPES::data_types);

				//void add_namespaces(AWDProject *);
		};
	



		/** \class NumAttr
		*	\brief The UserAttr defines one block-property that can be added to a AWDElement that implements AttrElementBase.\n
		*	Like UserAttr a NumAttr is always part of a dedicated list NumAttrList.\n
		*	A NumAttr must be initiated with a default-value. It will only be exported if its value is different from the default-value.\n
		*	All standart AWD-properties will be made available in constructors of the AWDElements that implements AttrElementBase.\n
		*	This way, all default values for properties are set inside the libawd.
		*	If custom properties are needed, they must be created with NumAttrList.add() before the value can be set with NumAttrList.set().
		* 
		*/
		class NumAttr :
			public AttrBase
		{
			private:
				result compare_against_default();
				TYPES::union_ptr default_value;						/// < only used by NumAttr
				TYPES::storage_precision_category storage_precision_category;			/// < for User Attr this will always be set to TYPES::storage_precision_category::ATTRIBUTES;
				TYPES::property_storage_type storage_type;			/// < for User Attr this will always be set to TYPES::property_storage_type::PROPERTY_STATIC;

			protected:

			public:
				NumAttr();
				~NumAttr();
				awd_propkey key;
				result set(TYPES::union_ptr, TYPES::UINT32, TYPES::data_types, TYPES::storage_precision_category, TYPES::property_storage_type);
				bool use_property(); 
				bool is_floating_value();
				result update_value(TYPES::union_ptr);
				result update_value(TYPES::union_ptr, TYPES::UINT32);
				result write_num_attr(FILES::FileWriter* ,SETTINGS::BlockSettings* );
				TYPES::UINT32 get_num_attr_length(SETTINGS::BlockSettings*);
				bool get_storage_precision(SETTINGS::BlockSettings*);
		};

		/** \class NumAttrList
		*	\brief A list of NumAttr (block-properties) 
		*/
		class NumAttrList {
			private:
				std::vector<ATTR::NumAttr*> attributes;
				ATTR::NumAttr* find(ATTR::awd_propkey);

			public:
				NumAttrList();
				~NumAttrList();
				TYPES::UINT32 calc_length(SETTINGS::BlockSettings *);
				result write_attributes(FILES::FileWriter*, SETTINGS::BlockSettings *);

				TYPES::union_ptr get_val_ptr(ATTR::awd_propkey);
				bool get(ATTR::awd_propkey key, TYPES::union_ptr * value, TYPES::UINT32 * value_length, TYPES::data_types * data_type);
				result set(ATTR::awd_propkey key, TYPES::union_ptr value);
				result set(ATTR::awd_propkey key, TYPES::union_ptr value, TYPES::UINT32 data_length);
				
				result add(ATTR::awd_propkey key, TYPES::union_ptr default_value, TYPES::UINT32 value_length, TYPES::data_types data_type,  TYPES::storage_precision_category storage_cat, TYPES::property_storage_type storage_type);
		};

	}




	namespace BASE
	{
		/** \class AttrElementBase
		*	\brief Provides a NumAttrList (custom-attributes) and a UserAttrList (custom-block-attributes) for a block.
		*/
		class AttrElementBase
		{
			protected:
				AttrElementBase();
				~AttrElementBase();

				ATTR::NumAttrList *properties;
				ATTR::UserAttrList *user_attributes;
				std::vector<ATTR::NumAttr*> property_descriptions;

				TYPES::UINT32 calc_attr_length(bool, bool, SETTINGS::BlockSettings *);

			public:
				result get_attr(BLOCKS::Namespace *, std::string& , TYPES::union_ptr *, TYPES::UINT32 *, TYPES::data_types *);
				result set_attr(BLOCKS::Namespace *, std::string& , TYPES::union_ptr, TYPES::UINT32, TYPES::data_types);
				result add_color_property(TYPES::UINT32, TYPES::COLOR);
				result add_number_property(TYPES::UINT32, TYPES::F64);
				result add_uint32_property(TYPES::UINT32, TYPES::UINT32);
				result add_uint16_property(TYPES::UINT32, TYPES::UINT16);
				result add_uint8_property(TYPES::UINT32, TYPES::UINT8);
				result add_bool_property(TYPES::UINT32, bool);
				result clear_properties_descriptions();
				/**
				* Check if a property description exists for given property.
				* return The index to property or -1 if property is not found.
				*
				*/
				TYPES::INT32 find_property_description(ATTR::awd_propkey id, TYPES::data_types type_in);
				result add_properties_description(ATTR::awd_propkey id, TYPES::data_types type_in, TYPES::union_ptr default_value_in, TYPES::storage_precision_category storage_category_in, TYPES::property_storage_type props_storage_type);
		};
	}
}
#endif

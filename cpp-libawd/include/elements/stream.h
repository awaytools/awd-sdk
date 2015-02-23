#ifndef _LIBAWD_STREAM_H
#define _LIBAWD_STREAM_H
#include <vector>
#include "utils/awd_types.h"
#include "files/file_writer.h"


namespace AWD
{

	namespace GEOM
	{
	
			
		/** \class DataStreamRecipe
		*	\brief DataStream writes and saves sub-geometry-streams, using a StreamRecipe.\n
		*	DataStreams are written for each file they are assigned to, because storage-precision or scale might have been changed. 
		*/
		
		/** 
		*	This is a single attribute of a SubGeom-Stream. \nThis is the class set can either be added to a Vertex or to a Triangle in order to save custom-attribute-streams.\n
		*	This class is made as small as possible, because it must be present per Triangle or Vertex.\n
		*	All properties of the Attr that are the same for all Vertex or Triangle are saved in a 
		*	The DataStreamRecipe contains a DataStreamattrDescription object, that is used to further descripe the DataStreamAttr when writing the streams to file.
		*
		*/
		class DataStreamAttr
		{
			private:
				GEOM::data_stream_attr_type type;		///< the type of this attr - used as identifier of this 
				TYPES::union_ptr			data;		///< the type of this attr - used as identifier of this 


			public:
				DataStreamAttr(GEOM::data_stream_attr_type type, TYPES::union_ptr data);
				DataStreamAttr();
				~DataStreamAttr();
				
				GEOM::data_stream_attr_type get_type();
				TYPES::union_ptr get_data();
				void set_data(TYPES::union_ptr data);

		};	
		
		/** 
		* This descripes properties of a DataSetreamAttr that can be saved per Vertex or Triangle.
		* The StreamRecipe contains a DataStreamattrDescription object, that is used to further descripe the DataStreamAttr when writing the streams to file.
		*
		*/
		class DataStreamAttrDesc
		{
			private:
				GEOM::data_stream_attr_type type;				///<	the type of this attr - used as identifier of this DataStreamAttr must match to the type of a DataStreamAttr
				TYPES::storage_precision_category storage_cat;	///<	which storage categorie should be used to write this attr.
				TYPES::data_types data_type;					///<	the data-type for this attr
				GEOM::DataStreamAttr default_value;				///<	the 
				TYPES::UINT32 data_length;				///<	the 
				bool use_scale;
				bool channel;
				std::string channel_id;
				bool preserve_all_channels;
				bool force_unique;
				
			public:
				DataStreamAttrDesc(GEOM::data_stream_attr_type type, TYPES::data_types data_type, TYPES::UINT32 data_length, TYPES::storage_precision_category storage_prec, bool use_scale,  bool preserve_all_channels, bool force_unique, bool channel, const std::string& channel_id);
				DataStreamAttrDesc();
				~DataStreamAttrDesc();
				
				TYPES::storage_precision_category get_storage_cat();
				GEOM::DataStreamAttr get_default_value();
				GEOM::data_stream_attr_type get_type();
				TYPES::union_ptr get_data();
				TYPES::data_types get_data_type();
				TYPES::UINT32 get_data_length();
				bool has_channel();		
				std::string& get_channel_id();	
				bool preserve_channels();	

		};		
		
		/** \class DataStreamRecipe
		*	\brief Contains constructions to collect the correct DataStreamAttr when writing a sub-geom-stream to file.
		*
		* 
		*/
		class DataStreamRecipe
		{
			private:
				GEOM::stream_type stream_type;	
				GEOM::stream_target stream_target_type;				/// < defines if the stream should be saved per triangle or per vertex. by default only stream_type::TRIANGLES is saved with Stream_target::TRIANGLE
				std::vector<GEOM::DataStreamAttrDesc> attr_descriptions;
				bool homogen;
			public:
				/**	\brief 
				* 
				* 
				*/
				DataStreamRecipe(GEOM::stream_type stream_type, GEOM::stream_target, std::vector<GEOM::DataStreamAttrDesc>& attr_descriptions);
				~DataStreamRecipe();	
				/**
				* If this is true, all attributes of this stream share the same basic data-type.\nThey can still vary in their concrete implementation.\
				* e.g. a stream containing Vector3D and Vector2D is still homogen, because both data-types can be combined to a list o floating values.
				*/
				bool is_homogen();
				std::vector<GEOM::DataStreamAttrDesc>& get_attr_descriptions();	
				GEOM::stream_type get_stream_type();	
				GEOM::stream_target get_stream_target_type();	
		};
		
	}
}

#endif

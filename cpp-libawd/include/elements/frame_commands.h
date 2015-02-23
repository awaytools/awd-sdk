#ifndef _LIBAWD_FRAME_COMMANDS_H
#define _LIBAWD_FRAME_COMMANDS_H

#include <vector>
#include <string>
#include "base/attr.h"
#include "base/block.h"
#include "utils/awd_types.h"
#include "elements/geom_elements.h"



#define CMD_PROPS_OBJID 1
#define CMD_PROPS_RESID 2
#define CMD_PROPS_PLACEAFTER_OBJID 3
#define CMD_PROPS_MATRIX2X3 4
#define CMD_PROPS_VISIBILTIY 5
#define CMD_PROPS_INSTANCENAME 6
#define CMD_PROPS_RESID_FILL 7

namespace AWD
{
	namespace ANIM
	{
		class FrameCommandBase
		{
			private:
				frame_command_type command_type;
				std::string objectType;
				BASE::AWDBlock * object_block;
				TYPES::UINT32 objID;
				std::string resID;
				TYPES::UINT32 layerID;
				bool hasRessource;// if true than the command will be "add" not "update"

			protected:
			
				ATTR::NumAttrList * command_properties;
				virtual TYPES::UINT32 calc_command_length_specific(SETTINGS::BlockSettings *)=0;
				virtual void write_command_specific(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile* awd_file)=0;
				virtual result collect_dependencies_specific(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)=0;
				virtual double compare_to_command_specific(FrameCommandBase *)=0;
				virtual void update_by_command_specific(FrameCommandBase *)=0;
				virtual AWD::result get_command_info_specific(std::string& info)=0;
		
			public:
				FrameCommandBase();
				~FrameCommandBase();
				
				AWD::result get_command_info(std::string& info);
				void set_object_block(BASE::AWDBlock *);
				BASE::AWDBlock* get_object_block();
				TYPES::UINT32 get_layerID();
				void set_layerID(TYPES::UINT32);
				std::string& get_objectType();
				void set_objectType(const std::string&);
				std::string& get_resID();
				void set_resID(const std::string&);
				TYPES::UINT32 get_objID();
				void set_objID(TYPES::UINT32);
				bool get_hasRessource();
				void set_hasRessource(bool hasRessource);
				frame_command_type get_command_type();
				void set_command_type(frame_command_type);
				ATTR::NumAttrList * get_command_properties();

				void write_command(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile* awd_file);
				result collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				TYPES::UINT32 calc_command_length(SETTINGS::BlockSettings *);
				double compare_to_command(FrameCommandBase *);
				void update_by_command(FrameCommandBase *);

		};

		class FrameCommandDisplayObject :
			public FrameCommandBase
		{
			private:

				//FrameCommandDisplayObject * filterFrameCommand;// the filter command is special, in that it has a typesd list itself.
				std::string instanceName;
				GEOM::MATRIX4x5* color_matrix;
				GEOM::MATRIX2x3* display_matrix;
				bool visible;
				TYPES::UINT32 depth;
				TYPES::UINT32 clip_depth;
				TYPES::UINT8 blendMode;
				
				bool hasDisplayMatrix;
				bool hasColorMatrix;
				bool hasDepthChange;
				bool hasFilterChange;
				bool hasBlendModeChange;
				bool hasDepthClipChange;
				bool hasVisiblitiyChange;

			protected:
			
				TYPES::UINT32 calc_command_length_specific(SETTINGS::BlockSettings *);
				void write_command_specific(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile* awd_file);
				result collect_dependencies_specific(FILES::AWDFile* target_file, BLOCK::instance_type instance_type);
				double compare_to_command_specific(FrameCommandBase *);
				void update_by_command_specific(FrameCommandBase *);
				AWD::result get_command_info_specific(std::string& info);

			public:
				FrameCommandDisplayObject();
				~FrameCommandDisplayObject();

				double compareColorMatrix(TYPES::F64* color_matrix);		
				double comparedisplaMatrix(TYPES::F64* display_matrix);
				bool get_hasDisplayMatrix();
				bool get_hasColorMatrix();
				bool get_hasDepthChange();
				bool get_hasFilterChange();
				bool get_hasBlendModeChange();
				bool get_hasDepthClipChange();
				bool get_hasVisiblitiyChange();
				int get_blendmode();
				void set_blendmode(int);
				TYPES::UINT32 get_depth();
				void set_depth(TYPES::UINT32);
				TYPES::UINT32 get_clipDepth();
				void set_clipDepth(TYPES::UINT32);
				std::string& get_instanceName();
				void set_instanceName(const std::string&);

				GEOM::MATRIX2x3* get_display_matrix();
				void set_display_matrix(TYPES::F64*);
				GEOM::MATRIX4x5* get_color_matrix();
				void set_color_matrix(TYPES::F64*);
				bool get_visible();
				void set_visible(bool);


		};
	}
}


#endif
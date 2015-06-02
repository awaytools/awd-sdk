#ifndef _LIBAWD_FRAME_COMMANDS_H
#define _LIBAWD_FRAME_COMMANDS_H

#include <vector>
#include <string>
#include "base/attr.h"
#include "base/block.h"
#include "utils/awd_types.h"
#include "elements/geom_elements.h"
#include "elements/timeline_depth_manager.h"




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
		
		class FrameCommandRemoveObject
		{            
        public:
            FrameCommandRemoveObject();
            ~FrameCommandRemoveObject();
			
			TYPES::UINT16 objID;
			TYPES::INT16 depth;
			TimelineChild_instance* child;
            
		};
		class FrameCommandDisplayObject
		{
			private:
				
				frame_command_type command_type;	// update / add
				BASE::AWDBlock * object_block;		// awd-block
				TYPES::UINT32 objID;				// adobe cmd id
				std::string resID;					// adobe ressource id
				TYPES::UINT32 depth;				// depth
				
				std::string instanceName;			
				GEOM::ColorTransform* color_matrix;	
				GEOM::MATRIX2x3* display_matrix;	
				bool visible;
				TYPES::UINT32 clip_depth;
				TYPES::UINT8 blendMode;
				bool hasDisplayMatrix;
				bool hasColorMatrix;
				bool hasBlendModeChange;
				bool hasVisiblitiyChange;

			public:
				FrameCommandDisplayObject();
				~FrameCommandDisplayObject();

				bool compare_mask_values(std::vector<TYPES::INT32> prev_mask_ids);
				void finalize_command();
				void resolve_parenting();
				double compareColorMatrix(TYPES::F64* color_matrix);
				double comparedisplaMatrix(TYPES::F64* display_matrix);
				
				bool calculated_mask;

				bool get_hasDisplayMatrix();
				bool get_hasColorMatrix();
				bool get_hasBlendModeChange();
				bool get_hasTargetMaskIDs();
				void set_hasTargetMaskIDs(bool);
				bool get_hasVisiblitiyChange();
				int get_blendmode();
				void set_blendmode(int);
				TYPES::UINT32 get_clipDepth();
				void set_clipDepth(TYPES::UINT32);
				std::string& get_instanceName();
				void set_instanceName(const std::string&);

				GEOM::MATRIX2x3* get_display_matrix();
				void set_display_matrix(TYPES::F64*);
				GEOM::ColorTransform* get_color_matrix();
				void set_color_matrix(TYPES::F64*);
				bool get_visible();
				void set_visible(bool);

				bool hasTargetMaskIDs;
				bool isMask;

				std::vector<GEOM::MATRIX2x3*> matrix_parents;
				std::vector<GEOM::ColorTransform*> color_transform_parents;
				FrameCommandDisplayObject* prev_obj;
				
				std::vector<TYPES::INT32> mask_ids;
				std::vector<TimelineChild_instance*> mask_childs;

				TimelineChild_instance* child;
				
				bool has_active_properties();
				AWD::result get_command_info(std::string& info);
				void set_object_block(BASE::AWDBlock *);
				BASE::AWDBlock* get_object_block();

				std::string& get_objectType();
				void set_objectType(const std::string&);
				TYPES::UINT32 get_objID();
				void set_objID(TYPES::UINT32);
				TYPES::UINT32 get_depth();
				void set_depth(TYPES::UINT32);
				frame_command_type get_command_type();
				void set_command_type(frame_command_type);

				void write_command(FILES::FileWriter *, SETTINGS::BlockSettings *, FILES::AWDFile* awd_file);
				TYPES::UINT32 calc_command_length(SETTINGS::BlockSettings *);
				double compare_to_command(FrameCommandDisplayObject *);
				void update_by_command(FrameCommandDisplayObject *);


		};
        
		class FrameCommandSoundObject
		{
        private:
            TYPES::UINT32 repeats;
            TYPES::UINT32 limit_in;
            TYPES::UINT32 limit_out;
                        
        public:
            FrameCommandSoundObject();
            ~FrameCommandSoundObject();
            
            void set_loop_mode(TYPES::UINT32);
            void set_sound_limit(TYPES::UINT32, TYPES::UINT32);
		};
	}
}


#endif
#include "blocks/timeline.h"
#include <string>
//#include <crtdbg.h>
#include "elements/timeline_frame.h"
#include "elements/frame_commands.h"
#include "files/file_writer.h"

#include "base/block.h"
#include "base/attr.h"


using namespace AWD;
using namespace AWD::GEOM;
using namespace AWD::TYPES;
using namespace AWD::FILES;
using namespace AWD::BLOCK;
using namespace AWD::BLOCKS;
using namespace AWD::SETTINGS;
using namespace AWD::ANIM;


Timeline::Timeline(const std::string& name):
	BASE::AWDBlock(BLOCK::block_type::TIMELINE, name),
    BASE::AttrElementBase()
{
	this->is_scene=false;
	this->fps=0.0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	
}
Timeline::Timeline():
	BASE::AWDBlock(BLOCK::block_type::TIMELINE),
    BASE::AttrElementBase()
{
	this->is_scene=false;
	this->fps=0.0;
	this->add_category(BLOCK::category::SCENE_OBJECT);
	
}
Timeline::~Timeline()
{
	for (TimelineFrame * f : this->frames) 
	{
		delete f;
	}
}

TYPES::state Timeline::validate_block_state()
{
	return TYPES::state::VALID;
}

void
Timeline::add_frame(TimelineFrame* newFrame, bool modifyCommands)
{
	// modifyCommands only needed when not using adobe frame generator
	if(modifyCommands){
		if(this->frames.size()>0){
			std::vector<int> activeLayers=newFrame->get_dirty_layers();
			std::vector<FrameCommandBase*> newRemoveCommands;
			std::vector<FrameCommandBase*> newAddCommands;
			std::vector<FrameCommandBase*> newUpdateCommands;
			std::vector<FrameCommandBase*> newCommands=newFrame->get_commands();
			std::vector<FrameCommandBase*> allOldCommands;
			std::vector<FrameCommandBase*> oldCommands=this->frames.back()->get_commands();
			std::vector<FrameCommandBase*> oldInActiveCommands=this->frames.back()->get_inActivecommands();
			for(FrameCommandBase* oldCommand: oldCommands){
				allOldCommands.push_back(oldCommand);
			}
			for(FrameCommandBase* oldInActiveCommand: oldInActiveCommands){
				allOldCommands.push_back(oldInActiveCommand);
			}
			for(FrameCommandBase* oldCmd: allOldCommands){
				int thisLayerID=oldCmd->get_layerID();
				bool isActive=false;
				for(int i:activeLayers){
					if(i==thisLayerID){
						isActive=true;
						break;
					}
				}
				if(isActive){
					// check if one of the new cmds is a update command
					double equal=0;
					double testEqual=0;
					int cmdIdx=-1;
					int cmdCnt=0;
					for (FrameCommandBase* newCmd:newCommands){
						if(newCmd->get_hasRessource()){
							// we only need to check if this is still a add-object-command
							testEqual=oldCmd->compare_to_command(newCmd);
							if(equal<testEqual){
								equal=testEqual;
								cmdIdx=cmdCnt;
							}
							cmdCnt++;
						}
					}
					if(cmdIdx<0){
						// no connected command found for this Command. So we need to create a remove command for it!
						reset_ressourceBlockUsageForID(oldCmd->get_objID()-1);
						FrameCommandDisplayObject* newremoveCommand=new FrameCommandDisplayObject();
						newremoveCommand->set_command_type(ANIM::frame_command_type::AWD_FRAME_COMMAND_REMOVE_OBJECT);
						newremoveCommand->set_objID(oldCmd->get_objID());
						newRemoveCommands.push_back(newremoveCommand);
					}
					else{
						// found a connected command. - if equal is 1, there is no change at all,
						// we put the Command in the inactiveCommands-list, because we still need it on next frame.
						if(equal==1){
							newCommands[cmdIdx]->set_hasRessource(false);
						}
						// found a connected command. - if equal is smaller than 1, there was a change
						// we transform the object into a update command
						else{
							newCommands[cmdIdx]->update_by_command(oldCmd);
							newCommands[cmdIdx]->set_objID(oldCmd->get_objID());
							newUpdateCommands.push_back(newCommands[cmdIdx]);
							newCommands[cmdIdx]->set_hasRessource(false);
						}
					}
				}
				else{
					newFrame->add_inActivecommand(oldCmd);
				}
			}
			for(FrameCommandBase* newCmd: newCommands){
				if(newCmd->get_hasRessource()){
					//if the cmd is still a add command, we get a obj id for it
					newCmd->set_objID(this->get_ressourceBlockID(newCmd->get_object_block()));
					newAddCommands.push_back(newCmd);
				}
			}
			newFrame->clear_commands();
			for(FrameCommandBase* newCmd: newRemoveCommands){
				newFrame->add_command(newCmd);
			}
			for(FrameCommandBase* newCmd: newUpdateCommands){
				newFrame->add_command(newCmd);
			}
			for(FrameCommandBase* newCmd: newAddCommands){
				newFrame->add_command(newCmd);
			}

		}
		else{
			std::vector<FrameCommandBase*> newCommands=newFrame->get_commands();
			for(FrameCommandBase* newCmd: newCommands){
				if(newCmd->get_hasRessource()){
					//if the cmd is still a add command, we get a obj id for it
					newCmd->set_objID(this->get_ressourceBlockID(newCmd->get_object_block()));
				}
			}
		}
	}
	this->frames.push_back(newFrame);
}


ANIM::TimelineFrame*
Timeline:: get_frame()
{
	return this->frames.back();
}
std::vector<TimelineFrame*>&
Timeline::get_frames()
{
	return this->frames;
}

				
bool
Timeline::get_is_scene()
{
	return this->is_scene;
}

int
Timeline::get_scene_id()
{
    return this->scene_id;
}

void
Timeline::set_scene_id(int scene_id)
{
	this->is_scene=true;
    this->scene_id = scene_id;
}


TYPES::F64
Timeline::get_fps()
{
	return this->fps;
}
void
Timeline::set_fps(TYPES::F64 fps)
{
	this->fps=fps;
}
TYPES::UINT32
Timeline::calc_body_length(AWDFile* awd_file, BlockSettings *curBlockSettings)
{

    TYPES::UINT32 len;

    len = sizeof(TYPES::UINT16) +  TYPES::UINT16(this->get_name().size());//name
	len += sizeof(bool);// is scene
	len += sizeof(TYPES::F32);// fps
	len += sizeof(TYPES::UINT8);// sceneID
    len += sizeof(TYPES::UINT16);// num of frames
	for (TimelineFrame * f : this->frames) 
	{
		len+=f->calc_frame_length(curBlockSettings);
	}
    len += this->calc_attr_length(true,true, curBlockSettings);
	
    return len;
}


result
Timeline::get_frames_info(std::vector<std::string>& infos)
{
	int framecnt = 0;
	for (TimelineFrame * f : this->frames) 
	{
		framecnt++;
		infos.push_back("Frame nr "+std::to_string(framecnt)+" duration = "+std::to_string(f->get_frame_duration()));
		f->get_frame_info(infos);
	}
	return AWD::result::AWD_SUCCESS;

}
void 
Timeline::reset_ressourceBlockUsage()
{
	for(bool thisusage: usedReccourseBlocksUsage){
		thisusage=false;
	}
}
void 
Timeline::reset_ressourceBlockUsageForID(int thisID)
{
	usedReccourseBlocksUsage[thisID]=false;
}
int 
Timeline::get_ressourceBlockID(AWDBlock* resBlock)
{
	int resCnt=0;
	for(AWDBlock* thisBlock: usedRessourceBlocks){
		if((resBlock==thisBlock)&&(!usedReccourseBlocksUsage[resCnt])){
			usedReccourseBlocksUsage[resCnt]=true;
			return resCnt;
		}
		resCnt++;
	}
	usedReccourseBlocksUsage.push_back(true);
	usedRessourceBlocks.push_back(resBlock);
	return TYPES::UINT32(usedRessourceBlocks.size());

}
result 
Timeline::collect_dependencies(FILES::AWDFile* target_file, BLOCK::instance_type instance_type)
{

	// get rid of frame that have no commands (empty frames)
	
	std::vector<TimelineFrame*> newFramesList;
	TimelineFrame* lastTimeLine=NULL;
	for (TimelineFrame * f : this->frames) 
	{
		if(f->get_commands().size()>0){
			newFramesList.push_back(f);
			lastTimeLine=f;
		}
		else{			
			if(lastTimeLine!=NULL){
				lastTimeLine->set_frame_duration(f->get_frame_duration()+lastTimeLine->get_frame_duration());
			}
			delete f;
		}
	}
	this->frames.clear();
	for(int frameCnt=0; frameCnt<newFramesList.size(); frameCnt++){
		TimelineFrame * f = newFramesList[frameCnt];
		f->collect_dependencies(target_file, instance_type);
		this->frames.push_back(f);
	}
	return result::AWD_SUCCESS;
}
result
Timeline::write_body(FILES::FileWriter * fileWriter, SETTINGS::BlockSettings *curBlockSettings, AWDFile* awd_file)
{
	fileWriter->writeSTRING(this->get_name(), FILES::write_string_with::LENGTH_AS_UINT16);// name	
	
	fileWriter->writeUINT8(this->is_scene);// is scene
	fileWriter->writeUINT8(this->scene_id);// sceneID //TODO
	fileWriter->writeFLOAT32(this->fps);// sceneID //TODO
	fileWriter->writeUINT16(TYPES::UINT16(this->frames.size()));// num of frames	
	for (TimelineFrame * f : this->frames) 
	{
		f->write_frame(fileWriter, curBlockSettings, awd_file);
	}
	
    this->properties->write_attributes(fileWriter, curBlockSettings);
    this->user_attributes->write_attributes(fileWriter, curBlockSettings);
	return result::AWD_SUCCESS;
}

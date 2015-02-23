#include "base/anim_clip.h"
#include <string>
#include "utils/awd_types.h"
#include "files/awd_file.h"
#include "files/file_writer.h"
#include "files/file_reader.h"

using namespace AWD;
using namespace AWD::BASE;

AnimationClipBase::AnimationClipBase()
{
	this->start_frame = 0;
	this->end_frame = 0;
	this->skip_frame = 0;

	this->stitch_final = false;
	this->loop = false;
	this->useTransforms = false;

	this->is_processed=false;
}
AnimationClipBase::~AnimationClipBase()
{
}

result
AnimationClipBase::set_time_range(TYPES::UINT32 start_frame, TYPES::UINT32 end_frame, TYPES::UINT32 skip_frame)
{
	if(this->num_frames>0){
		// TODO: delete all poses.
		this->num_frames=0;
	}
	this->start_frame = start_frame;
	this->end_frame = end_frame;
	this->skip_frame = skip_frame;

	return this->create_pose_blocks();
}

result
AnimationClipBase::set_clip_properties(bool stitch_final, bool loop, bool useTransforms)
{
	this->stitch_final = stitch_final;
	this->loop = loop;
	this->useTransforms = useTransforms;
	return result::AWD_SUCCESS;
}

std::string&
AnimationClipBase::get_sourceID()
{
	return this->sourceID;
}
void
AnimationClipBase::set_sourceID(const std::string& newSourceID)
{
	this->sourceID = newSourceID;
}

bool
AnimationClipBase::get_is_processed()
{
	return this->is_processed;
}

void
AnimationClipBase::set_is_processed(bool isProcessed)
{
	this->is_processed = isProcessed;
}

TYPES::UINT32
AnimationClipBase::get_start_frame()
{
	return this->start_frame;
}
TYPES::UINT32
AnimationClipBase::get_end_frame()
{
	return this->end_frame;
}
TYPES::UINT32
AnimationClipBase::get_skip_frame()
{
	return this->skip_frame;
}
bool
AnimationClipBase::get_stitch_final()
{
	return this->stitch_final;
}
bool
AnimationClipBase::get_loop()
{
	return this->loop;
}
bool
AnimationClipBase::get_use_transforms()
{
	return this->useTransforms;
}

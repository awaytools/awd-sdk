#include "base/anim_frame.h"
#include "utils/awd_types.h"
#include "awd_project.h"

using namespace AWD;
using namespace AWD::BASE;
using namespace AWD::TYPES;

AnimationFrameBase::AnimationFrameBase(TYPES::UINT32 duration, TYPES::UINT32 external_start_time)

{
	this->duration = duration;
	this->external_start_time = external_start_time;
}

AnimationFrameBase::~AnimationFrameBase()
{
}

TYPES::UINT32
AnimationFrameBase::get_duration()
{
	return this->duration;
}
TYPES::UINT32
AnimationFrameBase::get_external_start_time()
{
	return this->external_start_time;
}

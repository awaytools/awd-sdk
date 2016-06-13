#ifndef _LIBAWD_AWD_GLOBALS_H
#define _LIBAWD_AWD_GLOBALS_H

namespace AWD
{
	static const int VERSION_MAJOR = 3;		///< The Major version number of libAWD
	static const int VERSION_MINOR = 0;		///< The Minor version number of libAWD
	static const int VERSION_BUILD = 1;		///< The Build version number of libAWD
	static const char VERSION_RELEASE = 'b';	///< The Release version char of libAWD

	static const int RESSOURCE_LIMIT = 3 * 0xffffff;		///< The Maximal stream length in bytes \todo Make sure this is correct.
}
#endif

#ifndef _LIBAWD_AWDW32_H
#define _LIBAWD_AWDW32_H

#ifdef WIN32
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define open _open
#define lseek _lseek
#define write _write
#define read _read
#define close _close
#define unlink _unlink
#define snprintf _snprintf

#define off_t int

#else // UNIX

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>



#define _TRUNCATE 0

#define strncpy_s( dst, dstSize, src, count )   strncpy( dst, src, dstSize )
#define strcat_s(dest, num, src)                strcat( dest, src )
#define itoa( val, str, radix )                 sprintf( str, "%d", val )



#endif

#endif

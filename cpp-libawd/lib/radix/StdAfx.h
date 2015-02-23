// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D74B7B03_00D1_11D4_8B0F_0050BAC83302__INCLUDED_)
#define AFX_STDAFX_H__D74B7B03_00D1_11D4_8B0F_0050BAC83302__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

	#include <Stdlib.h>
	#include <Stdio.h>
	#include <String.h>
	#include "IceTypes.h"

	#define ICE_ALLOC_TMP(x)	::malloc(x)
	#define ICE_ALLOC(x)		::malloc(x)
	#define	ICE_FREE(x)			if(x)	{::free(x); x=null;}

	#define DELETEARRAY(x)	{ if (x != null) delete []x;	x = null; }
	#define CHECKALLOC(x)	if(!x) return false;

	// ANSI compliance
	#ifdef  _DEBUG
		// Remove painful warning in debug
		__forceinline bool ReturnsFalse(){ return false; }
		#define for if(ReturnsFalse()){}	else for
	#else
		#define for if(0){}	else for
	#endif

#define ICECORE_API

	// Our own inline keyword, so that:
	// - we can switch to __forceinline to check it's really better or not
	// - we can remove __forceinline if the compiler doesn't support it
	#define inline_				__forceinline
//	#define inline_				inline

	// Prevent nasty user-manipulations (strategy borrowed from Charles Bloom)
//	#define PREVENT_COPY(curclass)	void operator = (const curclass& object)	{	ASSERT(!"Bad use of operator =");	}
	// ... actually this is better !
	#define PREVENT_COPY(curclass)	private: curclass(const curclass& object);	curclass& operator=(const curclass& object);

//!	A function to clear a buffer.
//!	\param	addr		buffer address
//!	\param	size		buffer length
//!	\see	FillMemory
//!	\see	CopyMemory
//!	\see	MoveMemory
inline void ZeroMemory(void* addr, udword size)					{ memset(addr, 0, size);		}

	namespace IceCore
	{
		#include "IceRevisitedRadix.h"
		#include "IceRadix3Passes.h"
	}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D74B7B03_00D1_11D4_8B0F_0050BAC83302__INCLUDED_)

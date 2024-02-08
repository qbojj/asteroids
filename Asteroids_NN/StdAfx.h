// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D20AA801_0526_4B90_A325_D78FE745B72E__INCLUDED_)
#define AFX_STDAFX_H__D20AA801_0526_4B90_A325_D78FE745B72E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS

#define DI_VER 0x0700
#define DIRECTINPUT_VERSION 0x0700

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "mmsystem.h"
#include "dinput.h"

#include "math.h"

unsigned int NumericalRecipesRandom(); // 32 bit
unsigned int GlibCRandom(); // 31 bit
unsigned __int64 MimixRandom(); // 64 bit

inline long Round( double x )
{
	double afterPt = x - (int)x;
	return afterPt >= 0.5 ? (int)x + 1: afterPt >= -0.5 ? (int)x : (int)x - 1;
}

inline long Random( long min, long max )
{
	return min + NumericalRecipesRandom() % ( max - min + 1 );
}

CRect ScaleRect( const CRect &rect, double Scale );

CString VKToText( unsigned int VirtualKey );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D20AA801_0526_4B90_A325_D78FE745B72E__INCLUDED_)

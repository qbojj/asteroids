// stdafx.cpp : source file that includes just the standard includes
//	Asteroids.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

unsigned int NumericalRecipesRandom()
{
	static unsigned int previous = (unsigned int) time(NULL);

	return previous = ( 1664525 * previous + 1013904223 ); // modulo 2^32
}


unsigned int GlibCRandom()
{
	static unsigned int previous = (unsigned int) time(NULL);

	return previous = ( 1103515245 * previous + 12345 ) & 0x7fffFFFF; // modulo 2^31
}


unsigned __int64 MimixRandom()
{
	static unsigned __int64 previous = (unsigned __int64) time(NULL);

	return previous = ( 6364136223846793005i64 * previous + 1442695040888963407i64 ); // modulo 2^64
}

CRect ScaleRect( const CRect &rect, double Scale )
{
	return CRect( round( rect.left * Scale ), round( rect.top * Scale ), round( rect.right * Scale ), round( rect.bottom * Scale ) );
}

CString VKToText( unsigned int VirtualKey )
{
	CString Name;
	UINT scanCode = MapVirtualKey( VirtualKey, 0 );

	switch ( VirtualKey )
	{
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_RCONTROL: case VK_RMENU:
		case VK_LWIN: case VK_RWIN: case VK_APPS:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			GetKeyNameText(scanCode << 16, Name.GetBuffer( 256 ), 256 );
			Name.ReleaseBuffer();
	}

	return Name;
}
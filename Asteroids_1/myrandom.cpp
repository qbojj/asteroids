#include <stdafx.h>

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
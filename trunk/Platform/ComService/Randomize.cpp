//
// Randomize.cpp
//

#include "ComService.h"
#include "Randomize.h"

//////////////////////////////////////////////////////////////////////
// CRandomize construction
// 
// CRandomize::CRandomize()
// {
// }
// 
// CRandomize::~CRandomize()
// {
// }

//////////////////////////////////////////////////////////////////////
// CRandomize random handler
#ifdef WIN32
void CRandomize::CreateRandom()
{
    DWORD nRandId = GetCurrentProcessId();
	
	FILETIME pFileTime;
	SYSTEMTIME pSystemTime;
	
	GetSystemTime( &pSystemTime );
	SystemTimeToFileTime( &pSystemTime, &pFileTime );
	
	nRandId += pFileTime.dwLowDateTime;
	
	nRandId ^= (DWORD)((BYTE *)0x12345678 - (BYTE *)0);
	srand( (unsigned int)nRandId );
}
#else
void CRandomize::CreateRandom()
{
    DWORD nRandId = ::getpid();

    timeval tv;
    ::gettimeofday( &tv, 0 );

    nRandId += tv.tv_usec;
	
	nRandId ^= (DWORD)((BYTE *)0x12345678 - (BYTE *)0);
	srand( (unsigned int)nRandId );
}
#endif

WORD CRandomize::GetRandom(WORD *pnId) /*const*/
{
	WORD x;
	if ( ! pnId ) pnId = &x;
	
	*pnId = (WORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	return (*pnId);
}

DWORD CRandomize::GetRandom(DWORD *pnId) /*const*/
{
	DWORD x,y;
	
	x = (DWORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	y = x;
	x = (DWORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	y ^= (x<<8);
	x = (DWORD)(65536.0*((double)rand())/((double)RAND_MAX+1.0));
	y ^= (x<<16);
	
	return y;
}

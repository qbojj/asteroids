
#ifndef _NNDATACOLLECTOR_H
#define _NNDATACOLLECTOR_H

#include "GameObjsArray.h"

class CNNDataCollector
{
public:
	CNNDataCollector();
	virtual ~CNNDataCollector();

	void Clear();
	void Save( int Score );
	void Load( LPCSTR FileName );

	void LogMoment( CGameObjsArray *pObjsArray );

	void ImportMoment( int TickNum, CGameObjsArray *pObjsArray );

	struct TickData { void *pData; size_t Size; };

	CArray< TickData, TickData &> m_oData;

	static void GetAnnInput( CGameObjsArray *pObjsArray, float Input[ 128 ] );
protected:
	static int cmp( const void *A, const void *B );
	static void NormAngle( double &Angle );
};

#endif
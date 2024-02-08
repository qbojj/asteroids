// MyKann.h: interface for the CMyKann class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYKANN_H__7F00F8CB_99BF_4E6D_A301_EC12B559DE23__INCLUDED_)
#define AFX_MYKANN_H__7F00F8CB_99BF_4E6D_A301_EC12B559DE23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "kann.h"

#define INPUT_NEURONS 256
#define OUTPUT_NEURONS 5

class CMyKann  
{
private:
	void PreInit();
public:
	CMyKann();
	CMyKann( const char *FileName );
	virtual ~CMyKann();

	static kann_t* CreateNew();
	static kad_node_t* CreateGraph();

	inline static float Rnd(float mass);

	bool Load( const char *FileName );
	bool Save( const char *FileName );

	void RandWeights(float mass);

	kann_t *m_pKann;
};

#endif // !defined(AFX_MYKANN_H__7F00F8CB_99BF_4E6D_A301_EC12B559DE23__INCLUDED_)

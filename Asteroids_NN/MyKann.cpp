// MyKann.cpp: implementation of the CMyKann class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyKann.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CMyKann::PreInit()
{
	m_pKann = NULL;
}

CMyKann::CMyKann()
{
	PreInit();
}

CMyKann::CMyKann( const char *FileName )
{
	PreInit();

	if( !Load( FileName ) ) m_pKann = CreateNew();
}

kann_t* CMyKann::CreateNew()
{
	return kann_new(CreateGraph(), 0);
}

CMyKann::~CMyKann()
{
	kann_delete( m_pKann );
}

bool CMyKann::Load( const char *FileName )
{
	FILE *fo = fopen( FileName, "rb" );
	
	if( fo )
	{
		m_pKann = kann_load_fp( fo );
		fclose( fo );

		//fprintf( stderr, "Ann loaded\n" );

		return true;
	}

	return false;
}

bool CMyKann::Save( const char *FileName )
{
	FILE *fo = fopen( FileName, "wb" );
	
	if( fo )
	{
		kann_save_fp( fo, m_pKann );
		fclose( fo );

		//fprintf( stderr, "Ann saved\n" );

		return true;
	}

	return false;
}

kad_node_t* CMyKann::CreateGraph()
{
	kad_node_t *t;

	t = kann_layer_input(INPUT_NEURONS);
	t = kad_relu(kann_layer_dense(t, 128));
	t = kad_relu(kann_layer_dense(t, 32));
	t = kann_layer_cost(t, OUTPUT_NEURONS, KANN_C_CEM); 

	return t;
}

float CMyKann::Rnd(float mass)
{
	return (float)( Random(-1000000, 1000000) * 0.000001 * mass );
}

void CMyKann::RandWeights(float mass)
{
	for (int i = 0; i < m_pKann->n; i++)
	{
		kad_node_t *p = m_pKann->v[i];
		if (kad_is_var(p))
		{
			// apply randomness
			int size = kad_len(p);

			for (int j = 0; j < size; j++) p->x[j] = Rnd(mass);
		}
	}
}
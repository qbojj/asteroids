// KannTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyKann.h"
#include <afxtempl.h>
#include <search.h>
#include <conio.h>
#include <signal.h>

#include "AnnPlayer.h"

#include "..\Asteroids\Asteroids_NN\NNDataCollector.h"
#include "..\Asteroids\Asteroids_NN\GameObjsArray.h"
#include "..\Asteroids\Asteroids_NN\AsteroidsView.h"

#define RepsDir "Reps\\"

#define THREAD_NUM 8
#define POPULATION 8

float g_fNeuralNetworkChangeRandomness = 0;

typedef CArray< float *, float *& > CFloatArrayArray;

volatile bool g_bContinue = true;

BOOL WINAPI consoleHandler(DWORD signal)
{
    switch(signal)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
								g_bContinue = false;
								break;
	}

    return TRUE;
}

void GetDataFromFile( CFloatArrayArray &Input, 
					  CFloatArrayArray &Output, 
					  LPCSTR FileName )
{
	CGameObjsArray GameState;
	CNNDataCollector AsterodsGameLog;

	AsterodsGameLog.Load( FileName );

	for( int i = 0; i < AsterodsGameLog.m_oData.GetSize(); i++ )
	{
		AsterodsGameLog.ImportMoment( i, &GameState );

		if( GameState.m_bEndGame || 
			GameState.m_oShip.m_bForDestruction || 
			GameState.m_iState != STATE_NORMAL )
			continue;

		float *pIn	= (float *) calloc( INPUT_NEURONS, sizeof( float ) );
		float *pOut	= (float *) calloc( OUTPUT_NEURONS, sizeof( float ) );

		for( int j = 0; j < OUTPUT_NEURONS; j++ )
		{
			pOut[ j ] = (float)( GameState.m_oShip.m_aControls[ j ] );
		}

		CNNDataCollector::GetAnnInput( &GameState, pIn );

		GameState.DeleteAll();

		Input.Add( pIn );
		Output.Add( pOut );
	}

	memcpy( &GameState.m_sndDestroy, &CDirectSound(), sizeof( CDirectSound ) );
	memcpy( &GameState.m_sndUFODestroy, &CDirectSound(), sizeof( CDirectSound ) );

	AsterodsGameLog.Clear();
}

void SupervisedLerning( LPCSTR AnnFileName )
{
	CMyKann ann( AnnFileName );

	CFloatArrayArray Input, Output;
	CFileFind FileFind;

	int Next = FileFind.FindFile( RepsDir "*.rep" );

	while( Next )
	{
		Next = FileFind.FindNextFile();

		GetDataFromFile( Input, Output, FileFind.GetFilePath() );

		CString NewName;

		NewName.Format( RepsDir "Used\\%s", FileFind.GetFileName() );

		MoveFile( FileFind.GetFilePath(), NewName );
	}

	int DataProbesCount = Input.GetSize();

	// train
	kann_train_fnn1( ann.m_pKann, 0.001f, 16, 64, 10, 0.1f, 
						DataProbesCount, Input.GetData(), Output.GetData() );

	for( int k = 0; k < DataProbesCount; k++ )
	{
		free( Input.GetAt( k ) );
		free( Output.GetAt( k ) );
	}

	ann.Save( AnnFileName );
}

struct AnnAndScore
{
	CMyKann *m_pAnn;
	int m_iFitness;
};

int cmp(const void *A, const void *B)
{
	return ((AnnAndScore *)B)->m_iFitness - ((AnnAndScore *)A)->m_iFitness;
}

void UnsupLrn_Evolution( LPCSTR AnnDir )
{
	CString DirName;
	CreateDirectory(AnnDir, NULL);

	DirName.Format( "%sBestOfGenerations", AnnDir );
	CreateDirectory(DirName, NULL);

	DirName.Format( "%sBestOfAll", AnnDir );
	CreateDirectory(DirName, NULL);

	DirName.Format( "%sAnns", AnnDir );
	CreateDirectory(DirName, NULL);

	printf("Input NN change randomness: ");
	scanf("%f", &g_fNeuralNetworkChangeRandomness);
	printf("\n\n");

	CAnnPlayer *pAnnPlayers = new CAnnPlayer[THREAD_NUM];
	CMyKann *pNeuralNetworks = new CMyKann[POPULATION];
	CString (*pFileNames)[2] = new CString[POPULATION][ 2 ];

	int i;

	for( i = 0; i < POPULATION; i++ )
	{
		pFileNames[ i ][ 0 ].Format( "%sAnns\\ann%d.ann", AnnDir, i );
		pFileNames[ i ][ 1 ].Format( "%s.old", pFileNames[ i ][ 0 ], i );

		if( !pNeuralNetworks[ i ].Load( pFileNames[ i ][ 0 ] ) ) 
			pNeuralNetworks[ i ].m_pKann = CMyKann::CreateNew();
	}

	int Generation = 0, MaxScore = 0;

	CString DataFile;
	DataFile.Format( "%sUnsupLrn.info", AnnDir );

	FILE *fo = fopen( DataFile, "r+" );

	if( fo )
	{
		fscanf( fo, "%d %d", &Generation, &MaxScore );
	}
	else fo = fopen( DataFile, "w" );

	if( fo )
	{
		HANDLE *pHandles = new HANDLE[THREAD_NUM];

		for (i = 0; i < THREAD_NUM; i++)
			pHandles[i] = pAnnPlayers[i].m_oReady;

		AnnAndScore *pAnnP = new AnnAndScore[POPULATION];

		while( g_bContinue )
		{
			DWORD Ret;
			int nextSim = 0;

			int SimsRunning = 0;

			for (i = 0; i < min( THREAD_NUM, POPULATION ); i++)
			{
				if (!pAnnPlayers[i].NewSim(&pNeuralNetworks[nextSim])) g_bContinue = false;
				nextSim++, SimsRunning++;
			}

			memset( pAnnP, 0, sizeof( AnnAndScore[POPULATION] ) );
			int CurrentAnnP = 0;

			while( g_bContinue )
			{
				//Czekem na ich ocenienie
				Ret = WaitForMultipleObjects(THREAD_NUM, pHandles, FALSE, 50);

				if (Ret >= WAIT_OBJECT_0 && Ret < WAIT_OBJECT_0 + THREAD_NUM)
				{
					pAnnP[CurrentAnnP].m_iFitness = pAnnPlayers[Ret - WAIT_OBJECT_0].m_iFitness;
					pAnnP[CurrentAnnP++].m_pAnn   = pAnnPlayers[Ret - WAIT_OBJECT_0].m_pAnn;


					SimsRunning--;
					if (nextSim < POPULATION)
					{
						if (!pAnnPlayers[Ret - WAIT_OBJECT_0].NewSim(&pNeuralNetworks[nextSim++]))
							g_bContinue = false;
						else SimsRunning++;
					}
				}

				if (nextSim == POPULATION && SimsRunning == 0) break;
				if (Ret == WAIT_ABANDONED) g_bContinue = false;
			}
			
			if (!g_bContinue) break;

			//Sprawdzam ktore najlepsze
			qsort(pAnnP, POPULATION, sizeof(AnnAndScore), cmp);

			int i;

			int sum = 0;
			for( i = 0; i < POPULATION; i++ )
				sum += pAnnP[i].m_iFitness;

			int avarage = sum/POPULATION;

			int sum2 = 0;
			for( i = 0; i < POPULATION; i++ )
			{
				int v =	pAnnP[i].m_iFitness - avarage;
				sum2 += v*v;
			}

			printf("Generation %d - avarage score: %g, std deviation: %g \t\t\t\t\t\t\t\r", Generation, 1.0*sum/POPULATION, sqrt(sum2 / POPULATION) );
			
			if ((Generation % 32) == 0 )
			{
				printf("\n" );

				//Zapisuje Najlepsze SI
				CString bestName;
				bestName.Format("%sBestOfGenerations\\%d_%d.ann", AnnDir, Generation, pAnnP[0].m_iFitness );
				pAnnP[0].m_pAnn->Save(bestName);
			}
			
			if (pAnnP[0].m_iFitness > MaxScore )
			{
				MaxScore =  pAnnP[0].m_iFitness;

				CString bestName;
				bestName.Format("%sBestOfAll\\%d_%d.ann", AnnDir, Generation, pAnnP[0].m_iFitness);
				pAnnP[0].m_pAnn->Save(bestName);
			}
			
			
			const int SurvivePart = 4;
			//Zabijam .5 najgorszych i robie lepsze, bo ewolucja
			for (i = 0; i < (POPULATION / SurvivePart); i++ )
			{
				for( int j = 1; j < 4; j++ )
				{
					int off = j*(POPULATION / SurvivePart);

					kann_delete(pAnnP[i + off ].m_pAnn->m_pKann);
					pAnnP[i + off ].m_pAnn->m_pKann = kann_clone(pAnnP[i].m_pAnn->m_pKann, 64);
					pAnnP[i + off ].m_pAnn->RandWeights( g_fNeuralNetworkChangeRandomness );
				}
			}

			Generation++;

			fseek( fo, 0, 0 );
			fprintf( fo, "%d %d", Generation, MaxScore );
		}

		for( i = 0; i < POPULATION; i++ )
		{
			DeleteFile( pFileNames[ i ][ 1 ] );
			MoveFile( pFileNames[ i ][ 0 ], pFileNames[ i ][ 1 ] );

			pNeuralNetworks[ i ].Save( pFileNames[ i ][ 0 ] );
		}
		
		delete[] pAnnP;
		delete[] pHandles;

		fclose( fo );
	}

	delete[] pFileNames;
	delete[] pAnnPlayers;
	delete[] pNeuralNetworks;
}

void UnsupLrn_BestOfAll( LPCSTR AnnDir )
{
	CreateDirectory(AnnDir, NULL);

	CString DirName;
	DirName.Format( "%sNewBests", AnnDir );
	CreateDirectory(DirName, NULL);

	printf("Input NN change randomness: ");
	scanf("%f", &g_fNeuralNetworkChangeRandomness);
	printf("\n\n");

	CAnnPlayer *pAnnPlayers = new CAnnPlayer[THREAD_NUM];
	CMyKann *pNeuralNetworks = new CMyKann[POPULATION];
	CString FileNames[2];

	FileNames[ 0 ].Format( "%sann.ann", AnnDir );
	FileNames[ 1 ].Format( "%s.old", FileNames[ 0 ] );

	CMyKann Best( FileNames[ 0 ] );

	int Generation = 0, i = 0;
	double BestAvarage = 0;

	pNeuralNetworks[0].m_pKann = kann_clone( Best.m_pKann , 64 );
	pNeuralNetworks[0].RandWeights( g_fNeuralNetworkChangeRandomness );
	for( i = 1; i < POPULATION; i++ ) pNeuralNetworks[i].m_pKann = kann_clone( pNeuralNetworks[0].m_pKann, 64 );

	CString DataFile;
	DataFile.Format( "%sUnsupLrn.info", AnnDir );

	FILE *fo = fopen( DataFile, "r+" );

	if( fo )
	{
		fscanf( fo, "%d %lf", &Generation, &BestAvarage );
	}
	else fo = fopen( DataFile, "w" );

	if( fo )
	{
		HANDLE *pHandles = new HANDLE[THREAD_NUM];

		for (i = 0; i < THREAD_NUM; i++)
			pHandles[i] = pAnnPlayers[i].m_oReady;

		while( g_bContinue )
		{
			DWORD Ret;
			int nextSim = 0;

			int SimsRunning = 0;

			for (i = 0; i < min( THREAD_NUM, POPULATION ); i++)
			{
				if (!pAnnPlayers[i].NewSim(&pNeuralNetworks[nextSim])) g_bContinue = false;
				nextSim++, SimsRunning++;
			}

			int Sum = 0;

			while( g_bContinue )
			{
				//Czekem na ich ocenienie
				Ret = WaitForMultipleObjects(THREAD_NUM, pHandles, FALSE, 50);

				if (Ret >= WAIT_OBJECT_0 && Ret < WAIT_OBJECT_0 + THREAD_NUM)
				{
					Sum += pAnnPlayers[Ret - WAIT_OBJECT_0].m_iFitness;

					SimsRunning--;
					if (nextSim < POPULATION)
					{
						if (!pAnnPlayers[Ret - WAIT_OBJECT_0].NewSim(&pNeuralNetworks[nextSim++]))
							g_bContinue = false;
						else SimsRunning++;
					}
				}

				if (nextSim == POPULATION && SimsRunning == 0) break;
				if (Ret == WAIT_ABANDONED) g_bContinue = false;
			}
			
			if (!g_bContinue) break;

			double Avarage = 1.0 * Sum / POPULATION;

			printf("Generation %d - avarage %.0lf\t\t\t\t\t\t\t\r", Generation, Avarage );
			
			if( Avarage > BestAvarage )
			{
				BestAvarage = Avarage;

				CString NewBest;
				NewBest.Format( "%sNewBests\\%d_%.0lf", AnnDir, Generation, Avarage );
				pNeuralNetworks[0].Save( NewBest );

				MoveFile( FileNames[0], FileNames[1] );
				pNeuralNetworks[0].Save( FileNames[0] );
				printf("\n");

				kann_delete( Best.m_pKann );
				Best.m_pKann = kann_clone( pNeuralNetworks[0].m_pKann, 64 );
			}
			else
			{
				kann_delete( pNeuralNetworks[0].m_pKann );
				pNeuralNetworks[0].m_pKann = kann_clone( Best.m_pKann, 64 );
			}

			pNeuralNetworks[0].RandWeights( g_fNeuralNetworkChangeRandomness );
			for( i = 1; i < POPULATION; i++ ) 
			{
				kann_delete( pNeuralNetworks[i].m_pKann );
				pNeuralNetworks[i].m_pKann = kann_clone( pNeuralNetworks[0].m_pKann, 64 );
			}

			Generation++;
			fseek( fo, 0, 0 );
			fprintf( fo, "%d %lf", Generation, BestAvarage );
		}

		delete[] pHandles;

		fclose( fo );
	}

	delete[] pAnnPlayers;
	delete[] pNeuralNetworks;
}

int main(void)
{
	if( !SetConsoleCtrlHandler(consoleHandler, TRUE) ) printf( "Can't create event handler\n" );


	//SupervisedLerning( "Asteroids.ann" );
	UnsupLrn_Evolution( "Unsup_Evol\\" );
	//UnsupLrn_BestOfAll( "Unsup_BOA\\" );

	printf( "\n\nStopped execution\n\n" );
	getchar();

	return 0;
}

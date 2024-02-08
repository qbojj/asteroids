// AsteroidsDoc.cpp : implementation of the CAsteroidsDoc class
//

#include "stdafx.h"
#include "Asteroids.h"

#include "AsteroidsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsDoc

IMPLEMENT_DYNCREATE(CAsteroidsDoc, CDocument)

BEGIN_MESSAGE_MAP(CAsteroidsDoc, CDocument)
	//{{AFX_MSG_MAP(CAsteroidsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsDoc construction/destruction

void CAsteroidsDoc::DeleteAllEntries()
{
	while( !m_aScoreboard.IsEmpty() )
	{
		delete m_aScoreboard.GetHead();
		m_aScoreboard.RemoveHead();
	}
}

CAsteroidsDoc::CAsteroidsDoc()
{
	CreateDirectory( "dat", NULL );
	CreateDirectory( "snd", NULL );

	Load( "dat\\Scoreboard.scb" );

	m_bChanged = false;
}

CAsteroidsDoc::~CAsteroidsDoc()
{
	if( m_bChanged | 1 )
	{
		//Save changes
		Save( "dat\\Scoreboard.scb" );
	}

	DeleteAllEntries();
}

BOOL CAsteroidsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAsteroidsDoc serialization

void CAsteroidsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//Store
	}
	else
	{
		//Load
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsDoc diagnostics

#ifdef _DEBUG
void CAsteroidsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAsteroidsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAsteroidsDoc commands

BOOL CAsteroidsDoc::IsNewRecord( int score )
{
	return m_aScoreboard.GetCount() < 10 || m_aScoreboard.GetTail()->m_iScore < score;
}

void CAsteroidsDoc::AddScoreboardEntry( CScoreboardEntry *NewEntry )
{
	if( m_aScoreboard.IsEmpty() )
	{
		m_aScoreboard.AddHead( NewEntry );
		return;
	}

	POSITION pos = m_aScoreboard.GetHeadPosition();
	CScoreboardEntry *pEntry;

	POSITION LastPos;

	do
	{
		LastPos = pos;
		pEntry = m_aScoreboard.GetNext( pos );

		if( pEntry->m_iScore < NewEntry->m_iScore )
		{
			m_aScoreboard.InsertBefore( LastPos, NewEntry );
			if( m_aScoreboard.GetCount() > 10 )
			{
				delete m_aScoreboard.GetTail();
				m_aScoreboard.RemoveAt( m_aScoreboard.GetTailPosition() );
			}
			return;
		}
	}
	while( pos );

	if( m_aScoreboard.GetCount() < 10 )
	{
		m_aScoreboard.AddTail( NewEntry );
	}
}

UINT CAsteroidsDoc::Encrypt( const CString &Content, LPBYTE lpBuf, UINT nMax )
{
	if( Content.IsEmpty() )
		return 0;

	UINT StrSize = Content.GetLength();

	for( UINT i = 0; i < StrSize; i++ )
	{
		BYTE byte = Content.GetAt( i );
			
		byte += 100;

		*lpBuf++ = byte;
	}

	return StrSize;
}

UINT CAsteroidsDoc::Decrypt( LPBYTE lpBuf, UINT nMax, CString &Content )
{
	for( UINT i = 0; i < nMax; i++ )
	{
		BYTE byte = ( *lpBuf++ );

		byte -= 100;

		Content.Insert( i, byte );
	}

	return nMax;
}

BOOL CAsteroidsDoc::Save( LPCSTR path )
{
	FILE *of = fopen( path, "w" "b" );

	if( of == NULL ) return FALSE;

	CString NewFileContent;

	POSITION pos = m_aScoreboard.GetHeadPosition();
	CScoreboardEntry *pEntry;

	do
	{
		CString Cell;

		if( m_aScoreboard.IsEmpty() ) break;

		pEntry = m_aScoreboard.GetNext( pos );

		Cell.Format( "%s:%d", pEntry->m_sName, pEntry->m_iScore );

		NewFileContent += Cell;

		if( pos ) NewFileContent += ';';
	}
	while( pos );

	UINT MaxSize = 1024;

	LPBYTE FileBuffer = (LPBYTE)malloc( MaxSize );
	
	UINT Size = Encrypt( NewFileContent, FileBuffer, MaxSize );

	fwrite( FileBuffer, sizeof(BYTE), Size, of );

	free( FileBuffer );

	fclose( of );

	return TRUE;
}

BOOL CAsteroidsDoc::Load( LPCSTR path )
{
	FILE *of = fopen( path, "r" "b" );

	if( of == NULL ) return FALSE;

	DeleteAllEntries();

	fseek( of, 0L, SEEK_END );

	UINT FileSize = ftell( of );

	rewind( of );

	LPBYTE FileBuffer = (LPBYTE)malloc( FileSize * sizeof(BYTE) );

	UINT BytesRead = fread( FileBuffer, sizeof(BYTE), FileSize, of );

	fclose( of );

	CString FileContent;
	
	Decrypt( FileBuffer, BytesRead, FileContent );

	free( FileBuffer );

	//ar.ReadString( FileContent );

	CString Cell;

	int i = 0;
	while( AfxExtractSubString( Cell, FileContent, i++, ';' ) )
	{
		int pos = Cell.Find( ':' );

		if( pos != -1 ) AddScoreboardEntry( new CScoreboardEntry( Cell.Left( pos ), atoi( Cell.Right( Cell.GetLength() - pos - 1 ) ) ) );
	}

	return TRUE;
}
// AsteroidsDoc.h : interface of the CAsteroidsDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASTEROIDSDOC_H__204EC424_E20E_47CF_A063_A22F9EFCC2BC__INCLUDED_)
#define AFX_ASTEROIDSDOC_H__204EC424_E20E_47CF_A063_A22F9EFCC2BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class CScoreboardEntry
{
public:
	CString m_sName;
	int m_iScore;

	CScoreboardEntry( LPCSTR name, int score )
	{
		m_sName = name;
		m_iScore = score;
	};

	virtual ~CScoreboardEntry()
	{

	};

	CScoreboardEntry &operator =( const CScoreboardEntry &Entry )
	{
		m_sName = Entry.m_sName;
		m_iScore = Entry.m_iScore;
	};
};

class CAsteroidsDoc : public CDocument
{
protected: // create from serialization only
	CAsteroidsDoc();
	DECLARE_DYNCREATE(CAsteroidsDoc)

	void DeleteAllEntries();

// Attributes
public:
	bool m_bChanged;

	UINT Encrypt( const CString &Content, LPBYTE lpBuf, UINT nMax );
	UINT Decrypt( LPBYTE lpBuf, UINT nMax, CString &Content );

	BOOL Save( LPCSTR path );
	BOOL Load( LPCSTR path );

// Operations
public:
	CList< CScoreboardEntry *, CScoreboardEntry *> m_aScoreboard;

	void AddScoreboardEntry( CScoreboardEntry *NewEntry );
	BOOL IsNewRecord( int score );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAsteroidsDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAsteroidsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAsteroidsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASTEROIDSDOC_H__204EC424_E20E_47CF_A063_A22F9EFCC2BC__INCLUDED_)

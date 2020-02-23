// dle-xp.h : main header file for the DLC application
//

#ifndef _dle_xp_h
#define _dle_xp_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include <windows.h>
#include "dle-xp-res.h"       // main symbols
//#include "dlc_i.h"
#include "MainFrame.h"
#include "mine.h"

extern int nLayout;
extern int bSingleToolPane;

/////////////////////////////////////////////////////////////////////////////
// CDLE:
// See dlc.cpp for the implementation of this class
//

class CDLE : public CWinApp
{
public:
	CDlcDocTemplate*	m_pDlcDoc;
	char					m_szCaption [256];
	char					m_szExtCaption [256];
	int					m_delayUndo;
	int					m_nModified;
	int					m_bSplashScreen;
	int					m_bExpertMode;
	int					m_bMaximized;
	char					m_modFolders [3][256];
	char					m_appFolder [256];
	char					m_iniFile [256];
	
	CDLE ();

	~CDLE ();

	inline CMainFrame *MainFrame (void) { return (CMainFrame *) m_pMainWnd; }

	inline CMineView *MineView (void) { CMainFrame *h; return (h = MainFrame ()) ? h->MineView () : null; }

	inline CTextureView *TextureView (void) { CMainFrame* h; return (h = MainFrame ()) ? h->TextureView () : null; }

	inline CToolView *ToolView (void) { CMainFrame* h; return (h = MainFrame ()) ? MainFrame ()->ToolView () : null; }

	inline CDlcDoc *GetDocument () { CMineView *h; return (h = MineView ()) ? h->GetDocument () : null; }

	inline int FileType (void) { return theMine ? theMine->FileType (): RL2_FILE; }

	inline int FileVersion (void) { return theMine ? theMine->FileInfo ().version : 0; }

	inline int LevelVersion (void) { return theMine ? theMine->LevelVersion () : 7; }

	inline int LevelType (void) { return theMine ? theMine->LevelType () : 1; }

	inline bool IsStdLevel (void) { return LevelVersion () < 9; }

	inline bool IsD2XLevel (void) { return LevelVersion () >= 9; }

	inline bool IsD1File (void) { return FileType () == RDL_FILE; }

	inline bool IsD2File (void) { return FileType () != RDL_FILE; }

	inline bool IsD2XFile (void) { return (FileType () != RDL_FILE) && IsD2XLevel (); }

	inline CWnd *TexturePane (void) { return MainFrame ()->TexturePane (); }

	inline CWnd *MinePane (void) { return MainFrame ()->MinePane (); }

	inline CWnd *ToolPane (void) { return MainFrame ()->ToolPane (); }

	inline CSize& ToolPaneSize (void) { return ToolView ()->PaneSize (); }

	inline CSize& TexturePaneSize (void) { return TextureView ()->PaneSize (); }

	inline int& SplashScreen (void) { return m_bSplashScreen; }

	inline int& ExpertMode (void) { return m_bExpertMode; }

	bool SetModified (BOOL bModified);

	CDocument* CDLE::OpenDocumentFile(LPCTSTR lpszFileName);

	void WritePrivateProfileInt (LPSTR szKey, int nValue);

	void SaveLayout (void);

	void LoadLayout (void);

	bool MakeModFolders (char* szSubFolder);

	inline char* AppFolder (void) { return m_appFolder; }

	inline char* ModFolder (int i) { return m_modFolders [i]; }

	inline char* IniFile (void) { return m_iniFile; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLE)
	public:
	virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CDLE)
	afx_msg void OnAppAbout();
	afx_msg BOOL OnOpenRecentFile(UINT nID);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bATLInited;
private:
	BOOL InitATL();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

extern CDLE	DLE;

void IndentLog (int nIndent);
int SetLogIndent (int nIndent);
void OpenLogFile (char* pszFolder);
void CloseLogFile (void);
void _cdecl PrintLog (const int nIndent, const char *fmt, ...);

#endif //_dle_xp_h

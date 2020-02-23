// dlc.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "dle-xp.h"
#include "afxadv.h"

#include "MainFrame.h"
#include "dlcDoc.h"
#include "mineview.h"
#include <initguid.h>
#include "Dlc_i.c"
//#include "ComMine.h"
//#include "ComCube.h"
//#include "ComObj.h"
#include "global.h"
#include "TextureManager.h"
#include "FileManager.h"
#ifdef _OPENMP
#	include "omp.h"
#endif

int nLayout = 1;
int bSingleToolPane = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	UINT_PTR m_nTimer;
	int		m_nTimeout;

	CAboutDlg(int m_nTimeout = 0);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog ();
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnTimer (UINT_PTR nIdEvent);
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg(int nTimeout) : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_nTimer = -1;
	m_nTimeout = nTimeout;
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
		ON_WM_LBUTTONDOWN ()
		ON_WM_RBUTTONDOWN ()
		ON_WM_TIMER ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDLE::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog ()
{
CDialog::OnInitDialog ();
if (m_nTimeout)
	m_nTimer = SetTimer (4, (UINT) 100, null);
return TRUE;
}

void CAboutDlg::OnLButtonDown (UINT nFlags, CPoint point)
{
EndDialog (1);
}

void CAboutDlg::OnRButtonDown (UINT nFlags, CPoint point)
{
EndDialog (1);
}

void CAboutDlg::OnTimer (UINT_PTR nIdEvent)
{
if (m_nTimer == (int) nIdEvent) {
	m_nTimeout--;
	if (m_nTimeout <= 0) {
		KillTimer (m_nTimer);
		m_nTimer = -1;
		EndDialog (1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDLE

BEGIN_MESSAGE_MAP(CDLE, CWinApp)
	//{{AFX_MSG_MAP(CDLE)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_NEWFILE, CWinApp::OnFileNew)
	ON_COMMAND(ID_OPENFILE, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	// Overriding the CWinApp-provided default since we have different
	// requirements (don't want to wipe files off the recent files list
	// just because a user cancelled out of the HOG manager)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLE construction

CDLE::CDLE()
{
m_pDlcDoc = null;
m_bSplashScreen = 1;
m_bMaximized = false;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CDLE::~CDLE()
{
CloseLogFile ();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDLE object

CDLE DLE;


// {3F315842-67AC-11d2-AE2A-00C0F03014A5}
static const CLSID clsid  = 
{ 0x3f315842, 0x67ac, 0x11d2, { 0xae, 0x2a, 0x0, 0xc0, 0xf0, 0x30, 0x14, 0xa5 } };

/////////////////////////////////////////////////////////////////////////////
// CDLE initialization

#ifdef _DEBUG

LPCTSTR PaletteResource (void);

void DLE_XP_invalid_parameter(
   const wchar_t * expression,
   const wchar_t * function, 
   const wchar_t * file, 
   uint line,
   uintptr_t pReserved)
{
errno = EINVAL;
}

#endif

//------------------------------------------------------------------------------

void SetupOpenMP (void)
{
#ifdef _OPENMP
int nThreads = omp_get_num_threads ();
if (nThreads < 2)
#pragma omp parallel 
	{
	nThreads = omp_get_max_threads ();
	}
if (nThreads > 4)
	omp_set_num_threads (4);
#endif
}

//------------------------------------------------------------------------------

BOOL CDLE::InitInstance()
{
#ifdef _DEBUG
	_set_invalid_parameter_handler (DLE_XP_invalid_parameter);
#endif
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	if (!InitATL())
		return FALSE;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("DLE"));

	::GetModuleFileName (0, m_appFolder, sizeof (m_appFolder));
	char* ps = strrchr (m_appFolder, '\\');
	if (ps)
		ps [1] = '\0';
	sprintf (m_iniFile, "%sdle.ini", m_appFolder);
	OpenLogFile (m_appFolder);

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pDlcDoc = new CDlcDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDlcDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMineView));
	AddDocTemplate(m_pDlcDoc);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	//cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;
	if (!ProcessShellCommand (cmdInfo))
		return FALSE;

	// Dispatch commands specified on the command line
	SetupOpenMP ();
	if (theMine == null)
		theMine = new CMine;
	textureManager.Setup ();
	MineView ()->DelayRefresh (true);
	MainFrame ()->ToolView ()->Setup ();
	hogManager = new CHogManager (MainFrame ());
	theMine->Load ();
	if (!textureManager.Available ())
		ToolView ()->SetActive (11); // invoke preferences dialog
	TextureView ()->Setup ();
	MainFrame ()->SetSelectMode (eSelectSide);
	MainFrame ()->ShowWindow (SW_SHOW);
	MainFrame ()->GetWindowText (m_szCaption, sizeof (m_szCaption));
	MainFrame ()->FixToolBars ();
	LoadLayout ();
	MainFrame ()->ToolView ()->RecalcLayout (MainFrame ()->m_toolMode, MainFrame ()->m_textureMode);
	//MainFrame ()->ToolView ()->CalcToolSize ();
	MainFrame ()->UpdateWindow ();
	if (m_bSplashScreen || DEMO) {
		CAboutDlg aboutDlg (100);
		aboutDlg.DoModal ();
		}
	if (*cmdInfo.m_strFileName)
		GetDocument ()->OpenFile (false, cmdInfo.m_strFileName.GetBuffer (256), null /*"*"*/);
	if (ToolView ()->SettingsTool ())
		appSettings.Set (-1);
	if (*missionFolder)
		::SetCurrentDirectory (missionFolder);
	MineView ()->DelayRefresh (false);
	//DLE.GetDocument ()->SetModifiedFlag (1); // allow saving right away
	MineView ()->Refresh ();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDLE commands



/////////////////////////////////////////////////////////////////////////////
// CDLE message handlers
	
CDLCModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
#if 0
	OBJECT_ENTRY(CLSID_ComMine, CComMine)
	OBJECT_ENTRY(CLSID_ComCube, CComCube)
	OBJECT_ENTRY(CLSID_ComObj, CComObj)
#endif
END_OBJECT_MAP()

//------------------------------------------------------------------------------

LONG CDLCModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}

//------------------------------------------------------------------------------

LONG CDLCModule::Lock()
{
	AfxOleLockApp();
	return 1;
}

//------------------------------------------------------------------------------

int CDLE::ExitInstance()
{
if (m_bATLInited) {
	_Module.RevokeClassObjects();
	_Module.Term();
	}
return CWinApp::ExitInstance();
}

//------------------------------------------------------------------------------

CDocument* CDLE::OpenDocumentFile (LPCTSTR lpszFileName)
{
	int	nAction = IDOK;

ASSERT(m_pDocManager != null);
//	GetDocument ()->SetPathName ("(new document)");
return CWinApp::OpenDocumentFile (lpszFileName);
}

//------------------------------------------------------------------------------

BOOL CDLE::OnOpenRecentFile (UINT nID)
{
if (!m_pRecentFileList)
	return FALSE;
if (nID < ID_FILE_MRU_FIRST || nID > ID_FILE_MRU_LAST)
	return FALSE;

int nRecentFileIndex = nID - ID_FILE_MRU_FIRST;
if (nRecentFileIndex >= m_pRecentFileList->GetSize ())
	return FALSE;
OpenDocumentFile ((*m_pRecentFileList) [nRecentFileIndex]);
return TRUE;
}

//------------------------------------------------------------------------------

BOOL CDLE::InitATL()
{
m_bATLInited = TRUE;
_Module.Init(ObjectMap, AfxGetInstanceHandle());
_Module.dwThreadID = GetCurrentThreadId();
return TRUE;
}

//------------------------------------------------------------------------------

void CDLE::WritePrivateProfileInt (LPSTR szKey, int nValue)
{
	char	szValue [20];

sprintf_s (szValue, sizeof (szValue), "%d", nValue);
WritePrivateProfileString ("DLE", szKey, szValue, DLE.IniFile ());
}

//------------------------------------------------------------------------------

void CDLE::SaveLayout ()
{
CHECKMINE;

	CRect	rc;

MainFrame ()->GetWindowRect (rc);
WritePrivateProfileInt ("xWin", rc.left);
WritePrivateProfileInt ("yWin", rc.top);
WritePrivateProfileInt ("cxWin", rc.Width ());
WritePrivateProfileInt ("cyWin", rc.Height ());
MainFrame ()->m_toolBar.GetWindowRect (rc);
WritePrivateProfileInt ("xMainTB", rc.left);
WritePrivateProfileInt ("yMainTB", rc.top);
WritePrivateProfileInt ("cxMainTB", rc.Width ());
WritePrivateProfileInt ("cyMainTB", rc.Height ());
#if EDITBAR
MainFrame ()->m_editBar.GetWindowRect (rc);
WritePrivateProfileInt ("xEditTB", rc.left);
WritePrivateProfileInt ("yEditTB", rc.top);
WritePrivateProfileInt ("cxEditTB", rc.Width ());
WritePrivateProfileInt ("cyEditTB", rc.Height ());
#endif
WritePrivateProfileInt ("ToolMode", MainFrame ()->m_toolMode);
WritePrivateProfileInt ("TextureMode", MainFrame ()->m_textureMode);
WritePrivateProfileInt ("AutoFixBugs", ToolView ()->DiagTool ()->m_bAutoFixBugs);
WritePrivateProfileInt ("SplashScreen", m_bSplashScreen);
}

//------------------------------------------------------------------------------

void CDLE::LoadLayout ()
{
CHECKMINE;

	CRect	rc, rcIntersect, tbrc;
	HMONITOR hMonitor = null;
	UINT bMaximize = 0;
	UINT h = AFX_IDW_DOCKBAR_TOP;

MainFrame ()->m_toolMode = GetPrivateProfileInt ("DLE", "ToolMode", 1, DLE.IniFile ());
MainFrame ()->m_textureMode = GetPrivateProfileInt ("DLE", "TextureMode", 1, DLE.IniFile ());
MainFrame ()->m_paneMode = ((MainFrame ()->m_toolMode == 1) && (MainFrame ()->m_textureMode == 1)) ? 1 : 0;
if (ToolView ()->DiagTool ())
	ToolView ()->DiagTool ()->m_bAutoFixBugs = GetPrivateProfileInt ("DLE", "AutoFixBugs", 1, DLE.IniFile ());
rc.left = GetPrivateProfileInt ("DLE", "xWin", 0, DLE.IniFile ());
rc.top = GetPrivateProfileInt ("DLE", "yWin", 0, DLE.IniFile ());
rc.right = rc.left + GetPrivateProfileInt ("DLE", "cxWin", 0, DLE.IniFile ());
rc.bottom = rc.top + GetPrivateProfileInt ("DLE", "cyWin", 0, DLE.IniFile ());

hMonitor = MonitorFromRect (&rc, MONITOR_DEFAULTTONULL);
if (!hMonitor) {
	// Window rectangle lies entirely offscreen; move to primary monitor work area
	SystemParametersInfo (SPI_GETWORKAREA, 0, &rc, 0);
	bMaximize = 1;
	}
else {
	MONITORINFO mi;
	ZeroMemory (&mi, sizeof (MONITORINFO));
	mi.cbSize = sizeof (MONITORINFO);
	if (GetMonitorInfo (hMonitor, &mi)) {
		// If the rect is larger than the monitor we'll maximize the window
		IntersectRect (&rcIntersect, &rc, &mi.rcWork);
		bMaximize = EqualRect (&mi.rcWork, &rcIntersect);
		// Clip the window to this monitor
		CopyRect (&rc, &rcIntersect);
		}
	}
// Move it first anyway so we maximize it on the right monitor
if (!IsRectEmpty (&rc))
	MainFrame ()->MoveWindow (&rc, TRUE);
if (bMaximize)
	MainFrame ()->ShowWindow (SW_SHOWMAXIMIZED);

#if 0
tbrc.left = GetPrivateProfileInt ("DLE", "xMainTB", 0, DLE.IniFile ());
if (tbrc.left < 0)
	tbrc.left = 0;
tbrc.top = GetPrivateProfileInt ("DLE", "yMainTB", 0, DLE.IniFile ());
tbrc.right = tbrc.left + GetPrivateProfileInt ("DLE", "cxMainTB", 0, DLE.IniFile ());
tbrc.bottom = tbrc.top + GetPrivateProfileInt ("DLE", "cyMainTB", 0, DLE.IniFile ());
	if (tbrc.Width () > tbrc.Height ())	//horizontal
	if (tbrc.bottom >= rc.bottom - GetSystemMetrics (SM_CYFRAME))
		h = AFX_IDW_DOCKBAR_BOTTOM;
	else
		h = AFX_IDW_DOCKBAR_TOP;
else //vertical
	if (tbrc.right >= rc.right - GetSystemMetrics (SM_CXFRAME))
		h = AFX_IDW_DOCKBAR_RIGHT;
	else
		h = AFX_IDW_DOCKBAR_LEFT;
if (tbrc.Width () && tbrc.Height ())
	MainFrame ()->DockControlBar (&MainFrame ()->m_toolBar, (UINT) h, &tbrc);
#endif

tbrc.left = GetPrivateProfileInt ("DLE", "xEditTB", 0, DLE.IniFile ());
if (tbrc.left < 0)
	tbrc.left = 0;
tbrc.top = GetPrivateProfileInt ("DLE", "yEditTB", 0, DLE.IniFile ());
#if EDITBAR
tbrc.right = tbrc.left + GetPrivateProfileInt ("DLE", "cxEditTB", 0, DLE.IniFile ());
tbrc.bottom = tbrc.top + GetPrivateProfileInt ("DLE", "cyEditTB", 0, DLE.IniFile ());
if (tbrc.Width () > tbrc.Height ())	//horizontal
	if (tbrc.bottom >= rc.bottom - GetSystemMetrics (SM_CYFRAME))
		h = AFX_IDW_DOCKBAR_BOTTOM;
	else if (!tbrc.top)
		h = AFX_IDW_DOCKBAR_TOP;
else //vertical
	if (tbrc.right >= rc.right - GetSystemMetrics (SM_CXFRAME))
		h = AFX_IDW_DOCKBAR_RIGHT;
	else if (!tbrc.left)
		h = AFX_IDW_DOCKBAR_LEFT;
if (tbrc.Width () && tbrc.Height ()) {
	CPoint p;
	p.x = tbrc.left;
	p.y = tbrc.top;
	MainFrame ()->FloatControlBar (&MainFrame ()->m_editBar, p, (UINT) h);
	}
#endif
m_bSplashScreen = GetPrivateProfileInt ("DLE", "SplashScreen", 1, DLE.IniFile ());
}

//------------------------------------------------------------------------------

bool CDLE::MakeModFolders (char* szSubFolder)
{
	char szMission [256], szLevel [256];

strcpy (szMission, hogManager->MissionName ());
char* pszMission = strrchr (szMission, '.');
if (pszMission)
	*pszMission = '\0';
if (!((pszMission = strrchr (szMission, '\\')) || (pszMission = strrchr (szMission, ':'))))
	pszMission = szMission;
sprintf (m_modFolders [2], "%s\\%s\\", modFolder, pszMission ? pszMission + 1 : pszMission);
sprintf (m_modFolders [1], "%s%s", m_modFolders [2], szSubFolder);

strcpy (szLevel, hogManager->LevelName ());
char* pszName = strrchr (szLevel, '.');
if (pszName)
	*pszName = '\0';
if (!*szLevel)
	return false;

int nLevel = DLE.ToolView ()->MissionTool ()->LevelNumber (szLevel);
if (!nLevel)
	return false;
sprintf (m_modFolders [0], "%s%s\\level%02d", m_modFolders [2], szSubFolder, nLevel);
strcat (m_modFolders [2], pszMission);
return true;
}

//------------------------------------------------------------------------------

FILE *fLog = null;

static int nLogIndent = 0;
static int nLastIndent = 0;

//------------------------------------------------------------------------------

void IndentLog (int nIndent)
{
if (nIndent) {
	if (abs (nIndent) == 1)
		nIndent *= 3; // default indentation
	nLastIndent = abs (nIndent);
	}
nLogIndent += nIndent;
if (nLogIndent < 0) {
	PrintLog (0, "Log indentation error!\n");
	nLogIndent = 0;
	}
else if (nLogIndent > 100) {
	PrintLog (0, "Log indentation error!\n");
	nLogIndent = 100;
	}
}

//------------------------------------------------------------------------------

int SetLogIndent (int nIndent)
{
int nOldIndent = nLogIndent;
nLogIndent = nIndent;
return nOldIndent;
}

//------------------------------------------------------------------------------

#include "share.h"

void OpenLogFile (char* pszFolder)
{
static int nLogId = 0;
char fnErr [FILENAME_LEN];
sprintf (fnErr, "%s\\dle.log", pszFolder);
while (!(fLog = _fsopen (fnErr, "wt", _SH_DENYWR))) 
	sprintf (fnErr, "%s\\dle.log.%d", pszFolder, ++nLogId);
}

//------------------------------------------------------------------------------

void CloseLogFile (void)
{
if (fLog) {
	fclose (fLog);
	fLog = null;
	}
}

//------------------------------------------------------------------------------

void _cdecl PrintLog (const int nIndent, const char *fmt, ...)
{
if (fLog) {
	if (nIndent < 0)
		IndentLog (nIndent);
	if (fmt && *fmt) {
		va_list arglist;
			static char	szLogLine [2][100000] = {{'\0'}, {'\0'}};
			static int nLogLine = 0;

		va_start (arglist, fmt);
		if (nLogIndent > 0)
			memset (szLogLine [nLogLine], ' ', nLogIndent);
		else if (nLogIndent < 0)
			nLogIndent = 0;
		nLogLine &= 3;
		vsprintf (szLogLine [nLogLine] + nLogIndent, fmt, arglist);
		va_end (arglist);
		if (strcmp (szLogLine [nLogLine], szLogLine [!nLogLine])) {
			fprintf (fLog, szLogLine [nLogLine]);
			fflush (fLog);
			nLogLine = !nLogLine;
			}
		}
	if (nIndent > 0)
		IndentLog (nIndent);
	}
}

//------------------------------------------------------------------------------

void ArrayError (const char* pszMsg)
{
pszMsg = pszMsg;
ErrorMsg (pszMsg);
}

//------------------------------------------------------------------------------
//eof dlc.cpp
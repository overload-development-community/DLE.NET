// dlcView.cpp : implementation of the CMineView class
//

#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "TextureManager.h"
#include "global.h"
#include "FileManager.h"
#include "ObjectManager.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	MAX_VIEWDIST	30

/////////////////////////////////////////////////////////////////////////////
// CToolView

BEGIN_MESSAGE_MAP(CFileSettingsTool, CSettingsTabDlg)
	ON_BN_CLICKED (IDC_PREFS_BROWSE_D1PIG, OnOpenD1PIG)
	ON_BN_CLICKED (IDC_PREFS_BROWSE_D2PIG, OnOpenD2PIG)
	ON_BN_CLICKED (IDC_PREFS_BROWSE_MISSIONS, OnOpenMissions)
	ON_EN_KILLFOCUS (IDC_PREFS_BROWSE_D1PIG, OnChange)
	ON_EN_KILLFOCUS (IDC_PREFS_BROWSE_D2PIG, OnChange)
	ON_EN_KILLFOCUS (IDC_PREFS_BROWSE_MISSIONS, OnChange)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL CFileSettingsTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
	return FALSE;
m_btnBrowseD1PIG.AutoLoad (IDC_PREFS_BROWSE_D1PIG, this);
m_btnBrowseD2PIG.AutoLoad (IDC_PREFS_BROWSE_D2PIG, this);
m_btnBrowseMissions.AutoLoad (IDC_PREFS_BROWSE_MISSIONS, this);
m_bInited = true;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CFileSettingsTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;
DDX_Text (pDX, IDC_PREFS_PATH_D1PIG, appSettings.m_d1Folder, sizeof (appSettings.m_d1Folder));
DDX_Text (pDX, IDC_PREFS_PATH_D2PIG, appSettings.m_d2Folder, sizeof (appSettings.m_d2Folder));
DDX_Text (pDX, IDC_PREFS_PATH_MISSIONS, appSettings.m_missionFolder, sizeof (appSettings.m_missionFolder));
}

//------------------------------------------------------------------------------

bool CFileSettingsTool::BrowseFile (LPSTR fileType, LPSTR fileName, LPSTR fileExt, BOOL bOpen)
{
   char        s [256];
   int         nResult;
   char		   pn [256];

strcpy_s (pn, sizeof (pn), fileName);
sprintf_s (s, sizeof (s), "%s (%s)|%s|all files (*.*)|*.*||", fileType, fileExt, fileExt);
CFileDialog d (bOpen, fileExt, pn, 0, s, this);
d.m_ofn.hInstance = AfxGetInstanceHandle ();
d.m_ofn.lpstrInitialDir = pn;
//d.m_ofn.Flags |= OFN_EXPLORER | OFN_PATHMUSTEXIST;
if ((nResult = int (d.DoModal ())) != IDOK)
	return false;
strcpy_s (fileName, 256, d.m_ofn.lpstrFile);
return true;
}

//------------------------------------------------------------------------------

void CFileSettingsTool::OnOpenD1PIG (void)
{
if (BrowseFile ("Descent 1 PIG", appSettings.m_d1Folder, "*.pig", TRUE)) {
	UpdateData (FALSE);
	OnChange ();
	}
}

//------------------------------------------------------------------------------

void CFileSettingsTool::OnOpenD2PIG (void)
{
if (BrowseFile ("Descent 2 PIG", appSettings.m_d2Folder, "*.pig", TRUE)) {
	UpdateData (FALSE);
	OnChange ();
	}
}

//------------------------------------------------------------------------------

void CFileSettingsTool::OnOpenMissions (void)
{
if (BrowseFile ("Descent mission files", appSettings.m_missionFolder, "*.hog", TRUE)) {
	UpdateData (FALSE);
	char* p;
	if ((p = strrchr (appSettings.m_missionFolder, '\\')) || (p = strrchr (appSettings.m_missionFolder, '/')) || (p = strrchr (appSettings.m_missionFolder, ':')))
		*(p + 1) = '\0';
	::SetCurrentDirectory (appSettings.m_missionFolder);
	UpdateData (FALSE);
	OnChange ();
	}
}

//------------------------------------------------------------------------------

//eof SettingsTool.Files.cpp
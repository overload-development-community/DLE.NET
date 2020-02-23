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

BEGIN_MESSAGE_MAP(CMiscSettingsTool, CSettingsTabDlg)
	ON_BN_CLICKED (IDC_PREFS_EXPERTMODE, OnChange)
	ON_BN_CLICKED (IDC_PREFS_SPLASHSCREEN, OnChange)
	ON_BN_CLICKED (IDC_PREFS_USETEXCOLORS, OnChange)
//	ON_EN_UPDATE (IDC_PREFS_MOVERATE, OnChange)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL CMiscSettingsTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
   return FALSE;
m_bInited = true;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CMiscSettingsTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;

DDX_Check (pDX, IDC_PREFS_EXPERTMODE, appSettings.m_bExpertMode);
DDX_Check (pDX, IDC_PREFS_SPLASHSCREEN, appSettings.m_bSplashScreen);
DDX_Check (pDX, IDC_PREFS_USETEXCOLORS, appSettings.m_bApplyFaceLightSettingsGlobally);
DDX_Text (pDX, IDC_PREFS_PLAYER, szPlayerProfile, sizeof (szPlayerProfile));
}

//------------------------------------------------------------------------------

//eof SettingsTool.Misc.cpp
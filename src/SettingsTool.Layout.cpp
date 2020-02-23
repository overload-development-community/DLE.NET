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

BEGIN_MESSAGE_MAP(CLayoutSettingsTool, CSettingsTabDlg)
	ON_BN_CLICKED (IDC_PREFS_LAYOUT0, OnLayout0)
	ON_BN_CLICKED (IDC_PREFS_LAYOUT1, OnLayout1)
	ON_BN_CLICKED (IDC_PREFS_LAYOUT2, OnLayout2)
	ON_BN_CLICKED (IDC_PREFS_LAYOUT3, OnLayout3)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL CLayoutSettingsTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
   return FALSE;
m_bInited = true;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CLayoutSettingsTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;
int h = bSingleToolPane ? 3 : nLayout;
DDX_Radio (pDX, IDC_PREFS_LAYOUT0, h);
bSingleToolPane = (h == 3);
nLayout = bSingleToolPane ? 1 : h;
}


//------------------------------------------------------------------------------

void CLayoutSettingsTool::OnLayout0 (void) { appSettings.SetLayout (0); }
void CLayoutSettingsTool::OnLayout1 (void) { appSettings.SetLayout (1); }
void CLayoutSettingsTool::OnLayout2 (void) { appSettings.SetLayout (2); }
void CLayoutSettingsTool::OnLayout3 (void) { appSettings.SetLayout (3); }

//------------------------------------------------------------------------------

//eof SettingsTool.Layout.cpp
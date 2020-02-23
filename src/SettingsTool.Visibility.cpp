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

BEGIN_MESSAGE_MAP(CVisibilitySettingsTool, CSettingsTabDlg)
	ON_BN_CLICKED (IDC_PREFS_VIEW_MINE_NONE, OnViewMineNone)
	ON_BN_CLICKED (IDC_PREFS_VIEW_MINE_ALL, OnViewMineAll)
	ON_BN_CLICKED (IDC_PREFS_VIEW_OBJECTS_NONE, OnViewObjectsNone)
	ON_BN_CLICKED (IDC_PREFS_VIEW_OBJECTS_ALL, OnViewObjectsAll)
	ON_BN_CLICKED (IDC_PREFS_VIEW_PLAYERS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_ROBOTS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_WEAPONS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_POWERUPS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_KEYS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_HOSTAGES, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_REACTOR, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_EFFECTS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_WALLS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_SPECIAL, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_LIGHTS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_SHADING, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_DELTALIGHTS, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_HIDE_TAGGED, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_ALLTEXTURES, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_SKYBOX, OnChange)
	ON_BN_CLICKED (IDC_PREFS_VIEW_DEPTHTEST, OnChange)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL CVisibilitySettingsTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
   return FALSE;
m_bInited = true;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CVisibilitySettingsTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;

	int h, i;

for (i = 0; i <= IDC_PREFS_VIEW_SKYBOX - IDC_PREFS_VIEW_WALLS; i++) {
	h = ((appSettings.m_mineViewFlags & (1 << i)) != 0);
	DDX_Check (pDX, IDC_PREFS_VIEW_WALLS + i, h);
	if (h)
		appSettings.m_mineViewFlags |= (1 << i);
	else
		appSettings.m_mineViewFlags &= ~(1 << i);
	}
if (!pDX->m_bSaveAndValidate) {
	appSettings.m_mineViewFlags = DLE.MineView ()->GetMineViewFlags ();
	appSettings.m_objViewFlags = DLE.MineView ()->GetObjectViewFlags ();
	appSettings.m_texViewFlags = DLE.TextureView ()->GetViewFlags ();
	}
h = ((appSettings.m_texViewFlags & eViewMineUsedTextures) != 0);
DDX_Check (pDX, IDC_PREFS_VIEW_ALLTEXTURES, h);
appSettings.m_texViewFlags = h ? eViewMineUsedTextures : 0;
for (i = 0; i <= IDC_PREFS_VIEW_EFFECTS - IDC_PREFS_VIEW_ROBOTS; i++) {
	h = ((appSettings.m_objViewFlags & (1 << i)) != 0);
	DDX_Check (pDX, IDC_PREFS_VIEW_ROBOTS + i, h);
	if (h)
		appSettings.m_objViewFlags |= (1 << i);
	else
		appSettings.m_objViewFlags &= ~(1 << i);
	}
}

//------------------------------------------------------------------------------

void CVisibilitySettingsTool::OnViewObjectsNone (void)
{
appSettings.m_objViewFlags = eViewObjectsNone;
DLE.MineView ()->SetViewObjectFlags (appSettings.m_objViewFlags);
UpdateData (FALSE);
OnChange ();
}

//------------------------------------------------------------------------------

void CVisibilitySettingsTool::OnViewObjectsAll (void)
{
appSettings.m_objViewFlags = eViewObjectsAll;
DLE.MineView ()->SetViewObjectFlags (appSettings.m_objViewFlags);
UpdateData (FALSE);
OnChange ();
}

//------------------------------------------------------------------------------

void CVisibilitySettingsTool::OnViewMineNone (void)
{
appSettings.m_mineViewFlags &= ~(eViewMineLights | eViewMineSpecial | eViewMineWalls | eViewMineVariableLights);
DLE.MineView ()->SetViewMineFlags (appSettings.m_mineViewFlags);
UpdateData (FALSE);
OnChange ();
}

//------------------------------------------------------------------------------

void CVisibilitySettingsTool::OnViewMineAll (void)
{
appSettings.m_mineViewFlags |= (eViewMineLights | eViewMineSpecial | eViewMineWalls | eViewMineVariableLights);
DLE.MineView ()->SetViewMineFlags (appSettings.m_mineViewFlags);
UpdateData (FALSE);
OnChange ();
}

//------------------------------------------------------------------------------

//eof SettingsTool.Visibility.cpp
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

BEGIN_MESSAGE_MAP(CEditorSettingsTool, CSettingsTabDlg)
	ON_WM_HSCROLL ()
	ON_BN_CLICKED (IDC_PREFS_ALLOWOBJECTOVERLAP, OnChange)
	ON_BN_CLICKED (IDC_PREFS_UPDATETEXALIGN, OnChange)
	ON_EN_KILLFOCUS (IDC_PREFS_UNDO, OnChange)
	ON_EN_KILLFOCUS (IDC_PREFS_MOVERATE, OnChange)
	ON_EN_KILLFOCUS (IDC_PREFS_VIEW_MOVERATE, OnChange)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

static char* __cdecl FormatRotateRate (char* szValue, int nValue)
{
	static char* szRotateRates [] = {"1.40625°", "2.8125°", "5.625°", "7.5°", "11.25°", "15°", "22.5°", "30°", "45°"};

strcpy (szValue, szRotateRates [nValue]);
return szValue;
}

//------------------------------------------------------------------------------

BOOL CEditorSettingsTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
   return FALSE;
m_rotateRate.Init (this, IDC_PREFS_ROTATE_RATE_SLIDER, IDC_PREFS_ROTATE_RATE_SPINNER, -IDT_PREFS_ROTATE_RATE, 0, 8);
m_rotateRate.SetFormatter (FormatRotateRate);
m_bInited = true;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CEditorSettingsTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;
m_rotateRate.DoDataExchange (pDX, appSettings.m_iRotateRate);
DDX_Text (pDX, IDC_PREFS_MOVERATE, appSettings.m_moveRate [0]);
DDX_Text (pDX, IDC_PREFS_VIEW_MOVERATE, appSettings.m_moveRate [1]);
DDX_Text (pDX, IDC_PREFS_UNDO, appSettings.m_nMaxUndo);
DDX_Check (pDX, IDC_PREFS_ALLOWOBJECTOVERLAP, appSettings.m_bAllowObjectOverlap);
DDX_Check (pDX, IDC_PREFS_UPDATETEXALIGN, appSettings.m_bUpdateTexAlign);
if (pDX->m_bSaveAndValidate)
	appSettings.Set ();
}

//------------------------------------------------------------------------------

void CEditorSettingsTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (m_rotateRate.OnScroll (scrollCode, thumbPos, pScrollBar)) {
	UpdateData (TRUE);
	}
}

//------------------------------------------------------------------------------

//eof SettingsTool.Editor.cpp
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

BEGIN_MESSAGE_MAP(CRendererSettingsTool, CSettingsTabDlg)
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_BN_CLICKED (IDC_PREFS_DEPTH_OFF, OnChange)
	ON_BN_CLICKED (IDC_PREFS_DEPTH_LOW, OnChange)
	ON_BN_CLICKED (IDC_PREFS_DEPTH_MEDIUM, OnChange)
	ON_BN_CLICKED (IDC_PREFS_DEPTH_HIGH, OnChange)
	ON_BN_CLICKED (IDC_PREFS_RENDERER_1ST_PERSON, OnChange)
	ON_BN_CLICKED (IDC_PREFS_RENDERER_3RD_PERSON, OnChange)
	ON_CBN_SELCHANGE (IDC_PREFS_MINECENTER, OnSetMineCenter)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

BOOL CRendererSettingsTool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
   return FALSE;
InitSlider (IDC_PREFS_VIEWDIST, 0, MAX_VIEWDIST);
for (int i = 0; i <= MAX_VIEWDIST; i++)
	ViewDistSlider ()->SetTic (i);
CComboBox *pcb = CBMineCenter ();
pcb->AddString ("None");
pcb->AddString ("Crosshairs");
pcb->AddString ("Globe");
m_bInited = true;
UpdateData (FALSE);
return TRUE;
}

//------------------------------------------------------------------------------

void CRendererSettingsTool::DoDataExchange (CDataExchange * pDX)
{
if (!HaveData (pDX)) 
	return;
DDX_Radio (pDX, IDC_PREFS_RENDERER_3RD_PERSON, appSettings.m_nPerspective);
DDX_Radio (pDX, IDC_PREFS_DEPTH_OFF, appSettings.m_depthPerception);
if (pDX->m_bSaveAndValidate)
	appSettings.m_nMineCenter = CBMineCenter ()->GetCurSel ();
else {
	char	szViewDist [10];
	if (appSettings.m_nViewDist)
		sprintf_s (szViewDist, sizeof (szViewDist), "%d", DLE.MineView ()->ViewDist ());
	else
		strcpy_s (szViewDist, sizeof (szViewDist), "all");
	((CWnd *) GetDlgItem (IDC_PREFS_VIEWDIST_TEXT))->SetWindowText (szViewDist);
	ViewDistSlider ()->SetPos (appSettings.m_nViewDist);
	CBMineCenter ()->SetCurSel (appSettings.m_nMineCenter);
	}
}

//------------------------------------------------------------------------------

void CRendererSettingsTool::OnSetMineCenter (void)
{
DLE.ToolView ()->SettingsTool ()->Update ();
}

//------------------------------------------------------------------------------

void CRendererSettingsTool::OnRendererSW (void) { appSettings.SetRenderer (0); }

void CRendererSettingsTool::OnRendererGL (void) { appSettings.SetRenderer (1); }

//------------------------------------------------------------------------------

void CRendererSettingsTool::OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if ((void*) pScrollBar != (void*) ViewDistSlider ()) 
	pScrollBar->SetScrollPos (pScrollBar->GetScrollPos (), TRUE);
else {
	int nPos = ViewDistSlider ()->GetPos ();
	switch (scrollCode) {
		case SB_LINEUP:
			nPos--;
			break;
		case SB_LINEDOWN:
			nPos++;
			break;
		case SB_PAGEUP:
			nPos -= 10;
			break;
		case SB_PAGEDOWN:
			nPos += 10;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	if (nPos < 0)
		nPos = 0;
	else if (nPos > MAX_VIEWDIST)
		nPos = MAX_VIEWDIST;
	DLE.MineView ()->SetViewDistIndex (appSettings.m_nViewDist = nPos);
	}
}

//------------------------------------------------------------------------------

void CRendererSettingsTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
OnScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

void CRendererSettingsTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
OnScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

//eof prefsdlg.cpp
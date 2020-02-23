// dlcView.cpp : implementation of the CMineView class
//

#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "textures.h"
#include "global.h"
#include "FileManager.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolView

IMPLEMENT_DYNCREATE(CToolView, CWnd)

BEGIN_MESSAGE_MAP(CToolView, CWnd)
	ON_WM_CREATE ()
	ON_WM_DESTROY ()
	ON_WM_PAINT ()
	ON_WM_ERASEBKGND ()
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_WM_SIZE ()
	ON_COMMAND(ID_SEL_PREV_TAB, OnSelectPrevTab)
	ON_COMMAND(ID_SEL_NEXT_TAB, OnSelectNextTab)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CToolView::CToolView ()
	: CWnd ()
{
m_bRecalcLayout = FALSE;
m_segmentTool = null;
m_wallTool = null;
m_triggerTool = null;
m_textureTool = null;
m_objectTool = null;
m_effectTool = null;
m_lightTool = null;
m_missionTool = null;
m_reactorTool = null;
m_diagTool = null;
m_settingsTool = null;
m_txtFilterTool = null;
m_pTools = null;
m_paneSize.cx = 
m_paneSize.cy = 0;
m_scrollOffs [0] = 0;
m_scrollPage [0] = 0;
m_scrollRange [0] = 0;
m_bHScroll = 
m_bVScroll = FALSE;
}

//------------------------------------------------------------------------------

CToolView::~CToolView ()
{
Destroy ();
}

//------------------------------------------------------------------------------

void CToolView::Destroy (void)
{
if (m_pTools) {
	delete m_pTools;
	m_pTools = null;
	}
if (m_segmentTool)
	delete m_segmentTool;
if (m_wallTool)
	delete m_wallTool;
if (m_triggerTool)
	delete m_triggerTool;
if (m_textureTool)
	delete m_textureTool;
if (m_objectTool)
	delete m_objectTool;
if (m_effectTool)
	delete m_effectTool;
if (m_lightTool)
	delete m_lightTool;
if (m_reactorTool)
	delete m_reactorTool;
if (m_diagTool)
	delete m_diagTool;
if (m_missionTool)
	delete m_missionTool;
if (m_settingsTool)
	delete m_settingsTool;
if (m_txtFilterTool)
	delete m_txtFilterTool;
memset (m_tools, 0, sizeof (m_tools));
}

								/*---------------------------*/

void CToolView::Refresh (void)
{
CHECKMINE;

if (DLE.MineView ()->DelayRefresh ())
	return;

CPropertyPage* p = m_pTools ? m_pTools->GetActivePage () : null;

#if 1
for (int i = 0; i < sizeofa (m_tools); i++)
	if (m_tools [i] && (!p || (p == m_tools [i])))
		m_tools [i]->Refresh ();
#else
if (m_settingsTool && (!p || (p == m_settingsTool)))
	m_settingsTool->Refresh ();
if (m_textureTool && (!p || (p == m_textureTool)))
	m_textureTool->Refresh ();
if (m_wallTool && (!p || (p == m_wallTool)))
	m_wallTool->Refresh ();
if (m_triggerTool && (!p || (p == m_triggerTool)))
	m_triggerTool->Refresh ();
if (m_segmentTool && (!p || (p == m_segmentTool)))
	m_segmentTool->Refresh ();
if (m_objectTool && (!p || (p == m_objectTool)))
	m_objectTool->Refresh ();
if (m_effectTool && (!p || (p == m_effectTool)))
	m_effectTool->Refresh ();
if (m_missionTool && (!p || (p == m_missionTool)))
	m_missionTool->Refresh ();
if (m_reactorTool && (!p || (p == m_reactorTool)))
	m_reactorTool->Refresh ();
if (m_diagTool && (!p || (p == m_diagTool)))
	m_diagTool->Refresh ();
#endif
}

								/*---------------------------*/

int CToolView::Setup (void)
{
memset (m_tools, 0, sizeof (m_tools));
if (nLayout == 2) {
	CWnd* pParent = DLE.MainFrame ();
	if (!m_settingsTool) { // must happen first because program settings that are subsequently required will be loaded here
		m_settingsTool = new CSettingsTool (null);
		m_settingsTool->Create (IDD_SETTINGS_FLOAT);
		}
	if (!m_textureTool) {
		m_textureTool = new CTextureTool (null);
		m_textureTool->Create (IDD_TEXTUREDATA_FLOAT);
		}
	if (!m_segmentTool) {
		m_segmentTool = new CSegmentTool (null);
		m_segmentTool->Create (IDD_SEGMENTDATA_FLOAT);
		}
	if (!m_wallTool) {
		m_wallTool = new CWallTool (null);
		m_wallTool->Create (IDD_WALLDATA_FLOAT);
		}
	if (!m_triggerTool) {
		m_triggerTool = new CTriggerTool (null);
		m_triggerTool->Create (IDD_TRIGGERDATA_FLOAT);
		}
	if (!m_objectTool) {
		m_objectTool = new CObjectTool (null);
		m_objectTool->Create (IDD_OBJECTDATA_FLOAT);
		}
	if (!m_effectTool) {
		m_effectTool = new CEffectTool (null);
		m_effectTool->Create (IDD_EFFECTDATA_FLOAT);
		}
	if (!m_lightTool) {
		m_lightTool = new CLightTool (null);
		m_lightTool->Create (IDD_LIGHTDATA_FLOAT);
		}
	if (!m_reactorTool) {
		m_reactorTool = new CReactorTool (null);
		m_reactorTool->Create (IDD_REACTORDATA_FLOAT);
		}
	if (!m_missionTool) {
		m_missionTool = new CMissionTool (null);
		m_missionTool->Create (IDD_MISSIONDATA_FLOAT);
		}
	if (!m_diagTool) {
		m_diagTool = new CDiagTool (null);
		m_diagTool->Create (IDD_DIAGDATA_FLOAT);
		}
	if (!m_txtFilterTool) {
		m_txtFilterTool = new CTxtFilterTool (null);
		m_txtFilterTool->Create (IDD_TXTFILTERS_FLOAT);
		}
	}
else {
	if (m_pTools)
		return 0;
	m_pTools = new CPropertySheet ();
	m_pTools->AddPage (m_textureTool = new CTextureTool (m_pTools));
	m_pTools->AddPage (m_segmentTool = new CSegmentTool (m_pTools));
	m_pTools->AddPage (m_wallTool = new CWallTool (m_pTools));
	m_pTools->AddPage (m_triggerTool = new CTriggerTool (m_pTools));
	m_pTools->AddPage (m_objectTool = new CObjectTool (m_pTools));
	m_pTools->AddPage (m_effectTool = new CEffectTool (m_pTools));
	m_pTools->AddPage (m_lightTool = new CLightTool (m_pTools));
	m_pTools->AddPage (m_reactorTool = new CReactorTool (m_pTools));
	m_pTools->AddPage (m_missionTool = new CMissionTool (m_pTools));
	m_pTools->AddPage (m_diagTool = new CDiagTool (m_pTools));
	m_pTools->AddPage (m_txtFilterTool = new CTxtFilterTool (m_pTools));
	m_pTools->AddPage (m_settingsTool = new CSettingsTool (m_pTools));
	if (!m_pTools->Create (this, WS_CHILD | WS_VISIBLE, 0))
		return -1;
	}

m_tools [0] = m_textureTool;
m_tools [1] = m_segmentTool;
m_tools [2] = m_wallTool;
m_tools [3] = m_triggerTool;
m_tools [4] = m_objectTool;
m_tools [5] = m_effectTool;
m_tools [6] = m_lightTool;
m_tools [7] = m_reactorTool;
m_tools [8] = m_missionTool;
m_tools [9] = m_diagTool;
m_tools [10] = m_txtFilterTool;
m_tools [11] = m_settingsTool;

CalcToolSize ();

if (nLayout == 2) {
	for (int i = 0; i < sizeofa (m_tools); i++)
		if (!m_tools [i]->m_bVisible)
			m_tools [i]->Hide ();
	}
return 0;
}

//------------------------------------------------------------------------------

int CToolView::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
if (CWnd::OnCreate (lpCreateStruct) == -1)
	return -1;
return 0;
//return Setup ();
}

//------------------------------------------------------------------------------

void CToolView::CalcToolSize (void) 
{
#if 0
if (m_pTools) {
	CRect	rc;
	m_pTools->GetWindowRect (rc);
	m_paneSize.cx = rc.Width ();
	m_paneSize.cy = rc.Height () + 6/*+ 12*/;
	}
#endif
}

//------------------------------------------------------------------------------

void CToolView::OnDestroy (void) 
{
CWnd::OnDestroy ();
if (m_pTools)
	m_pTools->DestroyWindow ();
}

								/*---------------------------*/

void CToolView::OnSize (UINT nType, int cx, int cy)
{
CWnd::OnSize (nType, cx, cy);
}

								/*---------------------------*/

afx_msg void CToolView::OnPaint ()
{
	CRect	rc;
	CDC	*pDC;
	PAINTSTRUCT	ps;
	CSize	toolSize;

if (!GetUpdateRect (rc))
	return;
pDC = BeginPaint (&ps);
RecalcLayout ();
EndPaint (&ps);
}

								/*---------------------------*/

afx_msg BOOL CToolView::OnEraseBkgnd (CDC* pDC)
{
	CRect			rc;

GetClientRect (rc);
pDC->FillSolidRect (rc, RGB (128,128,128));
return TRUE;
/*
   CRect    rc;
   CBrush   bkGnd, * pOldBrush;
   CPoint   pt (0,0);

ClientToScreen (&pt);
bkGnd.CreateStockObject (BLACK_BRUSH);
bkGnd.UnrealizeObject ();
pDC->SetBrushOrg (pt);
pOldBrush = pDC->SelectObject (&bkGnd);
GetClientRect (rc);
pDC->FillRect (&rc, &bkGnd);
pDC->SelectObject (pOldBrush);
bkGnd.DeleteObject ();
return 1;
*/
}

void CToolView::MoveWindow (int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
/*
if (nHeight > 100) {
	x += nHeight - 100;
	nHeight = 100;
	}
*/
CWnd::MoveWindow (x, y, nWidth, nHeight, bRepaint);
}

void CToolView::MoveWindow (LPCRECT lpRect, BOOL bRepaint)
{
MoveWindow (lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, bRepaint);
}

//------------------------------------------------------------------------------

void CToolView::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
switch (nSBCode) {
	case SB_LEFT:
		--m_scrollOffs [0];
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINELEFT:
		m_scrollOffs [0] -= m_scrollPage [0];
		break;
	case SB_LINERIGHT:
		m_scrollOffs [0] += m_scrollPage [0];
		break;
	case SB_PAGELEFT:
		m_scrollOffs [0] -= m_scrollPage [0];
		break;
	case SB_PAGERIGHT:
		m_scrollOffs [0] += m_scrollPage [0];
		break;
	case SB_RIGHT:
		m_scrollOffs [0] = m_scrollRange [0];
		break;
	case SB_THUMBPOSITION:
		m_scrollOffs [0] = nPos;
		break;
	case SB_THUMBTRACK:
		m_scrollOffs [0] = nPos;
		break;
	default:
		CWnd::OnHScroll (nSBCode, nPos, pScrollBar);
	}
if (m_scrollOffs [0] < 0)
	m_scrollOffs [0] = 0;
else if (m_scrollOffs [0] >= m_scrollRange [0])
	m_scrollOffs [0] = m_scrollRange [0];
SetScrollPos (SB_HORZ, m_scrollOffs [0], TRUE);
RecalcLayout ();
}

//------------------------------------------------------------------------------

void CToolView::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
switch (nSBCode) {
	case SB_LEFT:
		--m_scrollOffs [1];
		break;
	case SB_ENDSCROLL:
		break;
	case SB_LINELEFT:
		m_scrollOffs [1] -= m_scrollPage [1];
		break;
	case SB_LINERIGHT:
		m_scrollOffs [1] += m_scrollPage [1];
		break;
	case SB_PAGELEFT:
		m_scrollOffs [1] -= m_scrollPage [1];
		break;
	case SB_PAGERIGHT:
		m_scrollOffs [1] += m_scrollPage [1];
		break;
	case SB_RIGHT:
		m_scrollOffs [1] = m_scrollRange [1];
		break;
	case SB_THUMBPOSITION:
		m_scrollOffs [1] = nPos;
		break;
	case SB_THUMBTRACK:
		m_scrollOffs [1] = nPos;
		break;
	default:
		CWnd::OnVScroll (nSBCode, nPos, pScrollBar);
	}
if (m_scrollOffs [1] < 0)
	m_scrollOffs [1] = 0;
else if (m_scrollOffs [1] >= m_scrollRange [1])
	m_scrollOffs [1] = m_scrollRange [1];
SetScrollPos (SB_VERT, m_scrollOffs [1], TRUE);
RecalcLayout ();
}

//------------------------------------------------------------------------------

void CToolView::RecalcLayout (int nToolMode, int nTextureMode)
{
if (!m_pTools)
	return;
if (m_bRecalcLayout)
	return;

	CRect	rc;

m_bRecalcLayout = TRUE;
if (DLE.MainFrame ())
	DLE.MainFrame ()->RecalcLayout (nToolMode, nTextureMode);
GetWindowRect (rc);
CSize desiredSize = DLE.MainFrame ()->ToolPaneSize ();
CSize currentSize = CSize (rc.Width (), rc.Height ());
if (m_bHScroll)
	currentSize.cy -= GetSystemMetrics (SM_CYHSCROLL);
if (m_bVScroll)
	currentSize.cx -= GetSystemMetrics (SM_CXVSCROLL);

if (currentSize.cx >= desiredSize.cx) {
	ShowScrollBar (SB_HORZ, FALSE);
	rc.left = 0;
	if (m_bHScroll) {
		m_bHScroll = FALSE;
		currentSize.cy += GetSystemMetrics (SM_CYHSCROLL);
		}
	}
else {
	m_scrollRange [0] = desiredSize.cx - currentSize.cx;
	m_scrollPage [0] = (m_scrollRange [0] < desiredSize.cx) ? m_scrollRange [0] : desiredSize.cx;
	SetScrollRange (SB_HORZ, 0, m_scrollRange [0], TRUE);
	ShowScrollBar (SB_HORZ, TRUE);
	rc.left = -GetScrollPos (SB_HORZ);
	if (!m_bHScroll) {
		m_bHScroll = TRUE;
		currentSize.cy -= GetSystemMetrics (SM_CYHSCROLL);
		}
	}
if (currentSize.cy >= desiredSize.cy) {
	ShowScrollBar (SB_VERT, FALSE);
	rc.top = 0;
	if (m_bVScroll) {
		m_bVScroll = FALSE;
		currentSize.cx += GetSystemMetrics (SM_CXVSCROLL);
		}
	}
else {
	m_scrollRange [1] = desiredSize.cy - currentSize.cy;
	m_scrollPage [1] = (m_scrollRange [1] < desiredSize.cy) ? m_scrollRange [1] : desiredSize.cy;
	SetScrollRange (SB_VERT, 0, m_scrollRange [1], TRUE);
	ShowScrollBar (SB_VERT, TRUE);
	rc.top = -GetScrollPos (SB_VERT);
	if (!m_bVScroll) {
		m_bVScroll = TRUE;
		currentSize.cx -= GetSystemMetrics (SM_CXVSCROLL);
		}
	}
rc.right = rc.left + max (currentSize.cx, desiredSize.cx);
rc.bottom = rc.top + max (currentSize.cy, desiredSize.cy);
m_pTools->MoveWindow (rc);
m_bRecalcLayout = FALSE;
}


//------------------------------------------------------------------------------

void CToolView::SetActive (int nPage)
{
if ((nPage < 0) || (nPage >= sizeofa (m_tools)))
	return;
if (m_pTools) {
	m_pTools->SetActivePage (nPage);
	DLE.MainFrame ()->RecalcLayout (-1, -1);
	}
else if (nLayout == 2) {
#if 1
	m_tools [nPage]->Show ();
#else
	if (nPage == 0)
		m_textureTool->Show ();
	else if (nPage == 1)
		m_segmentTool->Show ();
	else if (nPage == 2)
		m_wallTool->Show ();
	else if (nPage == 3)
		m_triggerTool->Show ();
	else if (nPage == 4)
		m_objectTool->Show ();
	else if (nPage == 5)
		m_effectTool->Show ();
	else if (nPage == 6)
		m_lightTool->Show ();
	else if (nPage == 7)
		m_reactorTool->Show ();
	else if (nPage == 8)
		m_missionTool->Show ();
	else if (nPage == 9)
		m_diagTool->Show ();
	else if (nPage == 10)
		m_txtFilterTool->Show ();
	else if (nPage == 11)
		m_settingsTool->Show ();
#endif
	}
}	

//------------------------------------------------------------------------------

void CToolView::CycleTab (int nDir)
{
if (m_pTools) {
	int i = m_pTools->GetPageIndex (m_pTools->GetActivePage ()) + nDir;
	int h = m_pTools->GetPageCount ();
	if (i < 0)
		i = h - 1;
	else if (i >= h)
		i = 0;
	m_pTools->SetActivePage (i);
	}
}

//------------------------------------------------------------------------------

void CToolView::NextTab ()
{
CycleTab (1);
}

//------------------------------------------------------------------------------

void CToolView::PrevTab ()
{
CycleTab (-1);
}

//------------------------------------------------------------------------------

void CToolView::OnSelectPrevTab ()
{
DLE.MainFrame ()->ShowTools ();
PrevTab ();
}

//------------------------------------------------------------------------------

void CToolView::OnSelectNextTab ()
{
DLE.MainFrame ()->ShowTools ();
NextTab ();
}

/////////////////////////////////////////////////////////////////////////////
// CTabDlg

BEGIN_MESSAGE_MAP(CTabDlg, CDialog)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CTabDlg::CTabDlg (UINT nId, CWnd *pParent)
	: m_bInited (0), m_bValid (0), m_bHaveData (0), CDialog (nId, pParent), CDlgHelpers (this)
{
EnableToolTips (true);
}

//------------------------------------------------------------------------------

BOOL CTabDlg::OnToolTipNotify (UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
return CDlgHelpers::OnToolTipNotify (id, pNMHDR, pResult);
}

//------------------------------------------------------------------------------

#if 0
int CDlcBitmapButton::OnToolHitTest (CPoint point, TOOLINFO* pTI)
{
	CRect	rc;

GetClientRect (rc);
if (!rc.PtInRect (point))
	return -1;
return m_nId;
}
#endif
//------------------------------------------------------------------------------

void CTextureTabDlg::CreateImgWnd (CWnd * pImgWnd, int nIdC)
{
CWnd *pParentWnd = GetDlgItem (nIdC);
CRect rc;
pParentWnd->GetClientRect (rc);
pImgWnd->Create (null, null, WS_CHILD | WS_VISIBLE, rc, pParentWnd, 0);
}

//------------------------------------------------------------------------------

void CTextureTabDlg::GetCtrlClientRect (CWnd *pWnd, CRect& rc)
{
	CRect	rcc;
	int	dx, dy;

pWnd->GetClientRect (&rcc);
pWnd->GetWindowRect (rc);
dx = rc.Width () - rcc.Width ();
dy = rc.Height () - rcc.Height ();
ScreenToClient (rc);
rc.DeflateRect (dx / 2, dy / 2);
}

//------------------------------------------------------------------------------

//eof toolview.cpp
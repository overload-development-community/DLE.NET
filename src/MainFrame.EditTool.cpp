
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "dle-xp.h"

#include "MainFrame.h"
#include "global.h"
#include "toolview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------

void CMainFrame::OnEditorToolbar ()
{
m_bEditorTB = !m_bEditorTB;
if (m_bEditorTB) {
	if (m_pEditTool = new CEditTool ()) {
		m_pEditTool->Create (IDD_EDITTOOL, MineView ());
		CRect rc0, rc1, rc2, rcWork;
		MineView ()->GetClientRect (rc1);
		MineView ()->ClientToScreen (rc1);
		m_pEditTool->GetWindowRect (rc2);
		SystemParametersInfo (SPI_GETWORKAREA, 0, &rcWork, 0);
		rc0.left = GetPrivateProfileInt ("DLE", "xEditTB", rcWork.left, DLE.IniFile ());
		rc0.top = GetPrivateProfileInt ("DLE", "yEditTB", rcWork.top, DLE.IniFile ());
		if (rc0.left < 0)
			rc0.left = 0;
		else if (rc0.left > rc1.right)
			rc0.left = rc1.right - rc2.Width ();
		if (rc0.top < 0)
			rc0.top = 0;
		else if (rc0.top > rc1.bottom)
			rc0.top = rc1.bottom - rc2.Height ();
		rc0.right = rc0.left + rc2.Width ();
		rc0.bottom = rc0.top + rc2.Height ();
		//MineView ()->ClientToScreen (rc0);
		m_pEditTool->MoveWindow (rc0, TRUE);
		m_pEditTool->ShowWindow (SW_SHOW);
		}
	else
		m_bEditorTB = null;
	}
else {
	CRect rc;
	m_pEditTool->GetWindowRect (rc);
	DLE.WritePrivateProfileInt ("xEditTB", rc.left);
	DLE.WritePrivateProfileInt ("yEditTB", rc.top);
	m_pEditTool->DestroyWindow ();
	delete m_pEditTool;
	m_pEditTool = null;
	}
#if EDITBAR
if (m_bShowCtrlBar && m_bEditorTB)
	CreateToolBars ();
CWnd *pWnd = m_editBar.GetParent ();
m_editBar.GetParent ()->ShowWindow ( (m_bShowCtrlBar && m_bEditorTB) ? SW_SHOW : SW_HIDE);
m_editBar.ShowWindow ( (m_bShowCtrlBar && m_bEditorTB) ? SW_SHOW : SW_HIDE);
FixToolBars ();
#endif
}

//------------------------------------------------------------------------------

#if EDITBAR == 0

BEGIN_MESSAGE_MAP (CEditTool, CDialog)
	ON_WM_TIMER ()
	ON_WM_MOUSEMOVE ()
	ON_WM_LBUTTONUP ()
	ON_WM_SETFOCUS ()
	ON_WM_KILLFOCUS ()
	ON_BN_CLICKED (IDC_EDITGEO0, OnEditGeo0)
	ON_BN_CLICKED (IDC_EDITGEO1, OnEditGeo1)
	ON_BN_CLICKED (IDC_EDITGEO2, OnEditGeo2)
	ON_BN_CLICKED (IDC_EDITGEO3, OnEditGeo3)
	ON_BN_CLICKED (IDC_EDITGEO4, OnEditGeo4)
	ON_BN_CLICKED (IDC_EDITGEO5, OnEditGeo5)
	ON_BN_CLICKED (IDC_EDITGEO6, OnEditGeo6)
	ON_BN_CLICKED (IDC_EDITGEO7, OnEditGeo7)
	ON_BN_CLICKED (IDC_EDITGEO8, OnEditGeo8)
	ON_BN_CLICKED (IDC_EDITGEO9, OnEditGeo9)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

CEditTool::CEditTool ()
{
m_nTimer = -1;
m_nEditFunc = -1;
}

//------------------------------------------------------------------------------

BOOL CEditTool::OnInitDialog ()
{
if (!CDialog::OnInitDialog ())
	return FALSE;

for (int i = 0; i < 10; i++)
	m_btns [i].AutoLoad (IDC_EDITGEO7 + i, this);
return TRUE;
}

//------------------------------------------------------------------------------

bool CEditTool::EditGeo0 (void) { return DLE.MainFrame ()->EditGeoShrink (); }
bool CEditTool::EditGeo1 (void) { return DLE.MainFrame ()->EditGeoRotLeft (); }
bool CEditTool::EditGeo2 (void) { return DLE.MainFrame ()->EditGeoDown (); }
bool CEditTool::EditGeo3 (void) { return DLE.MainFrame ()->EditGeoRotRight (); }
bool CEditTool::EditGeo4 (void) { return DLE.MainFrame ()->EditGeoLeft (); }
bool CEditTool::EditGeo5 (void) { return DLE.MainFrame ()->EditGeoGrow (); }
bool CEditTool::EditGeo6 (void) { return DLE.MainFrame ()->EditGeoRight (); }
bool CEditTool::EditGeo7 (void) { return DLE.MainFrame ()->EditGeoBack (); }
bool CEditTool::EditGeo8 (void) { return DLE.MainFrame ()->EditGeoUp (); }
bool CEditTool::EditGeo9 (void) { return DLE.MainFrame ()->EditGeoFwd (); }

void CEditTool::OnEditGeo0 () { EditGeo0 (); }
void CEditTool::OnEditGeo1 () { EditGeo1 (); }
void CEditTool::OnEditGeo2 () { EditGeo2 (); }
void CEditTool::OnEditGeo3 () { EditGeo3 (); }
void CEditTool::OnEditGeo4 () { EditGeo4 (); }
void CEditTool::OnEditGeo5 () { EditGeo5 (); }
void CEditTool::OnEditGeo6 () { EditGeo6 (); }
void CEditTool::OnEditGeo7 () { EditGeo7 (); }
void CEditTool::OnEditGeo8 () { EditGeo8 (); }
void CEditTool::OnEditGeo9 () { EditGeo9 (); }

/*---------------------------*/

void CEditTool::QuitEditFunc (void)
{
if (m_nEditFunc >= 0) {
	m_nEditFunc = -1;
	if (m_nTimer >= 0) {
		KillTimer (m_nTimer);
		m_nTimer = -1;
		}
	}
}

//------------------------------------------------------------------------------

afx_msg void CEditTool::OnSetFocus (CWnd* pOldWnd)
{
QuitEditFunc ();
CDialog::OnSetFocus (pOldWnd);
}

//------------------------------------------------------------------------------

afx_msg void CEditTool::OnKillFocus (CWnd* pNewWnd)
{
QuitEditFunc ();
CDialog::OnKillFocus (pNewWnd);
}

//------------------------------------------------------------------------------

void CEditTool::OnLButtonUp (UINT nFlags, CPoint pos)
{
QuitEditFunc ();
CDialog::OnLButtonUp (nFlags, pos);
}

//------------------------------------------------------------------------------

afx_msg void CEditTool::OnMouseMove (UINT nFlags, CPoint pos)
{
QuitEditFunc ();
CDialog::OnMouseMove (nFlags, pos);
}

//------------------------------------------------------------------------------

void CEditTool::OnTimer (UINT_PTR nIdEvent)
{
if (m_nEditFunc == UINT (-1))
	return;
POINT pos;
GetCursorPos (&pos);
CRect rc;
GetWindowRect (rc);
if ( (pos.x < rc.left) || (pos.y < rc.top) || (pos.x >= rc.right) || (pos.y >= rc.bottom)) {
	QuitEditFunc ();
	return;
	}
WPARAM editFunc = UINT (m_nEditFunc);
m_nEditFunc = UINT (-1);
switch (editFunc) {
	case IDC_EDITGEO0:
		if (EditGeo0 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO1:
		if (EditGeo1 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO2:
		if (EditGeo2 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO3:
		if (EditGeo3 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO4:
		if (EditGeo4 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO5:
		if (EditGeo5 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO6:
		if (EditGeo6 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO7:
		if (EditGeo7 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO8:
		if (EditGeo8 ()) 
			m_nEditFunc = editFunc;
		break;
	case IDC_EDITGEO9:
		if (EditGeo9 ()) 
			m_nEditFunc = editFunc;
		break;
	default:
		m_nEditFunc = UINT (-1);
		break;
	}
UINT i = (m_nTimerDelay * 9) / 10;
if ( (m_nEditFunc == UINT (-1)) || (i >= 25)) {
	if (m_nTimer != UINT (-1))
		KillTimer (m_nTimer);
	if (m_nEditFunc == UINT (-1))
		m_nTimer = -1;
	else
		m_nTimer = SetTimer (1, m_nTimerDelay = i, null);
	}
}

//------------------------------------------------------------------------------

BOOL CEditTool::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	LPNMHDR	nmHdr = (LPNMHDR) lParam;
	int		nMsg = nmHdr->code;

if ( (wParam < IDC_EDITGEO7) || (wParam > IDC_EDITGEO0))
	return CWnd::OnNotify (wParam, lParam, pResult);
if ( ( (LPNMHDR) lParam)->code == WM_LBUTTONDOWN) {
	POINT pos;
	GetCursorPos (&pos);
	CRect rc;
	GetWindowRect (rc);
	if ( (pos.x < 0) || (pos.y < 0) || (pos.x >= rc.right) || (pos.y >= rc.bottom))
		QuitEditFunc ();
	m_nEditFunc = wParam;
	m_nTimer = SetTimer (1, 250U, null);
	m_nTimerDelay = 250;
	}
else {
	QuitEditFunc ();
	}
*pResult = 0;
return TRUE;
}
		
#endif

//------------------------------------------------------------------------------

//eof MainFrame.EditTool.cpp
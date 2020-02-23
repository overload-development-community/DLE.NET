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
#include "Selection.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(CToolDlg, CPropertyPage)

BEGIN_MESSAGE_MAP(CToolDlg, CPropertyPage)
	ON_WM_CLOSE ()
	ON_WM_SETFOCUS ()
	ON_COMMAND(ID_SEL_PREV_TAB, OnSelectPrevTab)
	ON_COMMAND(ID_SEL_NEXT_TAB, OnSelectNextTab)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

CToolDlg::CToolDlg (UINT nIdTemplate, CPropertySheet *pParent)
	: CPropertyPage (nIdTemplate, 0), CDlgHelpers (this)
{
m_pParent = pParent; 
m_bInited = false;
m_bHaveData = false;
m_bVisible = false;
m_nId = nIdTemplate;
EnableToolTips (true);
}

//------------------------------------------------------------------------------

#if 0
int CToolDlg::OnToolHitTest (CPoint point, TOOLINFO* pTI)
{
	CRect	rc;

GetClientRect (rc);
if (!rc.PtInRect (point))
	return -1;
return 1;
}
#endif

//------------------------------------------------------------------------------

BOOL CToolDlg::OnToolTipNotify (UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
return CDlgHelpers::OnToolTipNotify (id, pNMHDR, pResult);
}

//------------------------------------------------------------------------------

void CToolDlg::GetCtrlClientRect (CWnd *pWnd, CRect& rc)
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

void CToolDlg::CreateImgWnd (CWnd * pImgWnd, int nIdC)
{
CWnd *pParentWnd = GetDlgItem (nIdC);
CRect rc;
pParentWnd->GetClientRect (rc);
pImgWnd->Create (null, null, WS_CHILD | WS_VISIBLE, rc, pParentWnd, 0);
}

//------------------------------------------------------------------------------

void CToolDlg::OnSelectPrevTab ()
{
DLE.MainFrame ()->ShowTools ();
DLE.ToolView ()->PrevTab ();
}

//------------------------------------------------------------------------------

void CToolDlg::OnSelectNextTab ()
{
DLE.MainFrame ()->ShowTools ();
DLE.ToolView ()->NextTab ();
}

//------------------------------------------------------------------------------

void CToolDlg::PreSubclassWindow () 
{
#if 1
if (nLayout == 2) {
	if(m_hWnd != null) {
		char szId [20];
		sprintf (szId, "Dialog%d", m_nId);
		m_bVisible = GetPrivateProfileInt ("DLE", szId, 0, DLE.IniFile ()) != 0;
		LONG lStyle = GetWindowLong (m_hWnd, GWL_STYLE);
		lStyle &= ~(WS_CHILD | WS_DISABLED);   
		lStyle |= WS_POPUP | WS_SYSMENU;
		if (m_bVisible)
			lStyle |= WS_VISIBLE;
		SetWindowLong (m_hWnd, GWL_STYLE, lStyle);  
		}
	}
#endif
CPropertyPage::PreSubclassWindow ();
}

//------------------------------------------------------------------------------

void CToolDlg::SavePos (void)
{
if (IsWindow (m_hWnd) && (nLayout == 2)) {
	CRect rc;
	GetWindowRect (rc);
	char szId [20];
	sprintf (szId, "Dialog%d", m_nId);
	DLE.WritePrivateProfileInt (szId, m_bVisible ? 1 : 0);
	sprintf (szId, "xDialog%d", m_nId);
	DLE.WritePrivateProfileInt (szId, rc.left);
	sprintf (szId, "yDialog%d", m_nId);
	DLE.WritePrivateProfileInt (szId, rc.top);
	}
}

//------------------------------------------------------------------------------

BOOL CToolDlg::OnInitDialog ()
{ 
if (nLayout != 2) 
	return CPropertyPage::OnInitDialog ();
else {
#if 1
	CWnd* pParent = AfxGetMainWnd ();
	if (pParent) {
		CRect rectFrame, rectDlg;
		pParent->GetClientRect (rectFrame);
		pParent->ClientToScreen (rectFrame);
		GetWindowRect (rectDlg);

		char szId [20];
		sprintf (szId, "Dialog%d", m_nId);
		m_bVisible = GetPrivateProfileInt ("DLE", szId, 0, DLE.IniFile ()) != 0;
		sprintf (szId, "xDialog%d", m_nId);
		int nXPos = GetPrivateProfileInt ("DLE", szId, 0, DLE.IniFile ());
		sprintf (szId, "yDialog%d", m_nId);
		int nYPos = GetPrivateProfileInt ("DLE", szId, 0, DLE.IniFile ());

		if ((nXPos | nYPos) == 0) {
			nXPos = rectFrame.left + (rectFrame.Width () / 2) - (rectDlg.Width () / 2);
			nYPos = rectFrame.top + (rectFrame.Height () / 2) - (rectDlg.Height () / 2);
			}
		::SetWindowPos (m_hWnd, HWND_TOP, nXPos, nYPos, rectDlg.Width (), rectDlg.Height (), SWP_NOCOPYBITS);
		}
#endif
	return CDialog::OnInitDialog ();
	}
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof tooldlg.cpp
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

IMPLEMENT_DYNCREATE(CToolTabCtrl, CTabCtrl)

BEGIN_MESSAGE_MAP(CToolTabCtrl, CTabCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TOOL_TAB, OnSelchangeTab)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------

bool CToolTabCtrl::Add (CDialog* pDlg, UINT nId, char* pszTitle) 
{
if (m_nDlgCount >= 24)
	return false;
pDlg->Create (nId, m_pParent);
pDlg->ShowWindow (m_nDlgCount ? SW_HIDE : SW_SHOW);
m_dialogs [m_nDlgCount] = pDlg;
InsertItem (m_nDlgCount++, pszTitle);
return true;
}

//------------------------------------------------------------------------------

void CToolTabCtrl::Setup (void)
{
	CRect	dlgRect, tabRect, screenRect, parentRect;

GetItemRect (0, &tabRect);
GetClientRect (&dlgRect);
screenRect = dlgRect;
ClientToScreen (&screenRect);
m_pParent->GetClientRect (&parentRect);
m_pParent->ClientToScreen (&parentRect);

if (!(this->GetStyle () & TCS_VERTICAL)) {
	dlgRect.left = tabRect.left + screenRect.left - parentRect.left;
	dlgRect.top = tabRect.bottom + 1 + screenRect.top - parentRect.top;
} else {
	dlgRect.left = tabRect.right + 1 + screenRect.left - parentRect.left;
	dlgRect.top = tabRect.top + screenRect.top - parentRect.top;
	}
dlgRect.right += screenRect.left - parentRect.left - 8;
dlgRect.bottom += screenRect.top - parentRect.top - 8;

for (int i = 0; i < m_nDlgCount; i++) {
	m_dialogs [i]->SetWindowPos (&wndTop, dlgRect.left, dlgRect.top, dlgRect.Width (), dlgRect.Height (), i ? SWP_HIDEWINDOW : SWP_SHOWWINDOW);
	}
m_dialogs [0]->SetFocus ();
}

//------------------------------------------------------------------------------

void CToolTabCtrl::Update (int nCurrent)
{
if ((nCurrent >= 0) && (nCurrent < m_nDlgCount) && (m_nCurrent != nCurrent))
	SetCurSel (nCurrent);
if (m_nCurrent != GetCurFocus ()) {
	m_dialogs [m_nCurrent]->ShowWindow (SW_HIDE);
	m_nCurrent = GetCurFocus ();
	m_dialogs [m_nCurrent]->ShowWindow (SW_SHOW);
	((CTabDlg*) m_dialogs [m_nCurrent])->Refresh ();
	// Delete the following line to move between tabs with arrow keys
	//m_dialogs [m_nCurrent]->SetFocus ();
	}
}

//------------------------------------------------------------------------------

void CToolTabCtrl::OnLButtonDown (UINT nFlags, CPoint point) 
{
CTabCtrl::OnLButtonDown (nFlags, point);
Update ();
}

//------------------------------------------------------------------------------

void CToolTabCtrl::OnSelchangeTab (NMHDR* pNMHDR, LRESULT* pResult)
{
// This function handles all possible ways of tab selection
// using the keyboard or the mouse
*pResult = 0;
Update ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof tooldlg.cpp
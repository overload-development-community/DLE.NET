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

IMPLEMENT_DYNCREATE (CExtBitmapButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CExtBitmapButton, CBitmapButton)
	ON_WM_LBUTTONDOWN ()
	ON_WM_LBUTTONUP ()
	ON_WM_MOUSEMOVE ()
//	ON_WM_SETFOCUS ()
//	ON_WM_KILLFOCUS ()
//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

								/*---------------------------*/

void CExtBitmapButton::Notify (UINT nMsg)
{
	NMHDR	nmHdr;

nmHdr.hwndFrom = m_hWnd;
nmHdr.idFrom = m_nId;
nmHdr.code = nMsg;
m_pParent->SendMessage (WM_NOTIFY, m_nId, (LPARAM) &nmHdr);
}

								/*---------------------------*/

void CExtBitmapButton::OnLButtonDown (UINT nFlags, CPoint point)
{
CBitmapButton::OnLButtonDown (nFlags, point);
Notify (m_nState = WM_LBUTTONDOWN);
}

								/*---------------------------*/

void CExtBitmapButton::OnLButtonUp (UINT nFlags, CPoint point)
{
CBitmapButton::OnLButtonUp (nFlags, point);
Notify (m_nState = WM_LBUTTONUP);
}

								/*---------------------------*/

afx_msg void CExtBitmapButton::OnMouseMove (UINT nFlags, CPoint point)
{
if (nFlags & MK_LBUTTON) {
	CRect	rc;

	GetClientRect (&rc);
	if ((point.x < 0) || (point.y < 0) || (point.x >= rc.right) || (point.y >= rc.bottom)) {
		if (m_nValue == 1) {
			Notify (WM_LBUTTONUP);
			m_nValue = 0;
			}
		}
	else {
		if (m_nValue != 1) {
			Notify (WM_LBUTTONDOWN);
			m_nValue = 1;
			}
		}
	}
CBitmapButton::OnMouseMove (nFlags, point);
}

								/*---------------------------*/

void CExtBitmapButton::OnSetFocus (CWnd* pOldWnd)
{
if (m_nState == WM_LBUTTONDOWN)
	Notify (WM_LBUTTONDOWN);
}

								/*---------------------------*/

void CExtBitmapButton::OnKillFocus (CWnd* pNewWnd)
{
if (m_nState == WM_LBUTTONDOWN)
	Notify (WM_LBUTTONUP);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof ExtBitmapButton.cpp
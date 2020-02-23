
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

BEGIN_MESSAGE_MAP (CExtToolBar, CToolBar)
	ON_WM_LBUTTONDOWN ()
	ON_WM_LBUTTONUP ()
	ON_WM_MOUSEMOVE ()
	ON_WM_TIMER ()
//	ON_WM_SETFOCUS ()
//	ON_WM_KILLFOCUS ()
//	ON_NOTIFY_EX_RANGE (TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
//	ON_NOTIFY_EX_RANGE (TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

CExtToolBar::CExtToolBar ()
{
m_nId = -1;
m_nState = WM_LBUTTONUP;
m_nPos = -1;
m_nTimer = -1;
}

//------------------------------------------------------------------------------

void CExtToolBar::Notify (UINT nMsg)
{
DLE.MainFrame ()->SendMessage (WM_COMMAND, nMsg, null);
}

//------------------------------------------------------------------------------

void CExtToolBar::OnLButtonDown (UINT nFlags, CPoint pos)
{
   CRect rc;
   int   h, i;

GetWindowRect (rc);
for (i = 0, h = GetToolBarCtrl ().GetButtonCount (); i < h; i++) {
   GetItemRect (i, rc);
	if ( (pos.x >= rc.left) && (pos.x < rc.right) && (pos.y >= rc.top) && (pos.y < rc.bottom)) {
		if ( (i >= 11) && (i <= 23) && (i != 13)) {
			m_nId = i;
			m_nTimer = SetTimer (1, m_nTimerDelay = 250U, null);
			}
		break;
		}
	}
CToolBar::OnLButtonDown (nFlags, pos);
}

//------------------------------------------------------------------------------

void CExtToolBar::OnLButtonUp (UINT nFlags, CPoint pos)
{
if (m_nId >= 0) {
	m_nId = -1;
	if (m_nTimer >= 0) {
		KillTimer (m_nTimer);
		m_nTimer = -1;
		}
	}
CToolBar::OnLButtonUp (nFlags, pos);
}

//------------------------------------------------------------------------------

afx_msg void CExtToolBar::OnMouseMove (UINT nFlags, CPoint pos)
{
if (nFlags & MK_LBUTTON) {
	CRect	rc;

	GetItemRect (m_nId, rc);
	if ( (pos.x < 0) || (pos.y < 0) || (pos.x >= rc.right) || (pos.y >= rc.bottom)) {
		if (m_nPos == 1) {
			m_nPos = 0;
			if (m_nTimer >= 0) {
				KillTimer (m_nTimer);
				m_nTimer = -1;
				}
			}
		}
	else {
		if (m_nPos != 1) {
			m_nPos = 1;
			m_nTimer = SetTimer (1, m_nTimerDelay = 250U, null);
			}
		}
	}
CToolBar::OnMouseMove (nFlags, pos);
}

//------------------------------------------------------------------------------

int CExtToolBar::Width ()
{
   CRect rc;
   int   h, i, dx;

GetWindowRect (rc);
dx = 0;
for (i = 0, h = GetToolBarCtrl ().GetButtonCount (); i < h; i++) {
   GetItemRect (i, rc);
   dx += rc.Width ();
//	if (GetButtonStyle (i) != TBBS_SEPARATOR)
//      ++dx;
   }
return dx;// - 4;
}

//------------------------------------------------------------------------------

void CExtToolBar::OnTimer (UINT_PTR nIdEvent)
{
if (nIdEvent == 1) {
	switch (m_nId) {
		case 11:
			Notify (ID_VIEW_ZOOMIN);
			break;
		case 12:
			Notify (ID_VIEW_ZOOMOUT);
			break;
		case 14:
			Notify (ID_VIEW_PAN_LEFT);
			break;
		case 15:
			Notify (ID_VIEW_PAN_RIGHT);
			break;
		case 16:
			Notify (ID_VIEW_PAN_UP);
			break;
		case 17:
			Notify (ID_VIEW_PAN_DOWN);
			break;
		case 18:
			Notify (ID_VIEW_ROTATE_RIGHT);
			break;
		case 19:
			Notify (ID_VIEW_ROTATE_LEFT);
			break;
		case 20:
			Notify (ID_VIEW_ROTATE_DOWN);
			break;
		case 21:
			Notify (ID_VIEW_ROTATE_UP);
			break;
		case 22:
			Notify (ID_VIEW_ROTATE_CLOCKWISE);
			break;
		case 23:
			Notify (ID_VIEW_ROTATE_COUNTERCLOCKWISE);
			break;
		default:
			break;
		}
	}
UINT i = (m_nTimerDelay * 9) / 10;
if (i >= 25) {
	KillTimer (m_nTimer);
	m_nTimer = SetTimer (1, m_nTimerDelay = i, null);
	}
}

//------------------------------------------------------------------------------

//eof MainFrame.ExtToolBar.cpp
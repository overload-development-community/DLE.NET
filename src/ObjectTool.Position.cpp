#include "stdafx.h"
#include "dle-xp.h"
#include "toolview.h"

BEGIN_MESSAGE_MAP (CObjectPositionTool, CObjectTabDlg)
	ON_BN_CLICKED (IDC_OBJ_SETLOC, OnSetLoc)
	ON_BN_CLICKED (IDC_OBJ_RESETLOC, OnResetLoc)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

BOOL CObjectPositionTool::OnInitDialog ()
{
if (!CObjectTabDlg::OnInitDialog ())
	return FALSE;
Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectPositionTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;
for (int i = 0; i < 3; i++) {
	DDX_Double (pDX, IDC_OBJ_LOCX + i, m_nPos [i]);
	if (m_nPos [i] < -0x7fff)
		m_nPos [i] = -0x7fff;
	else if (m_nPos [i] > 0x7fff)
		m_nPos [i] = 0x7fff;
	DDX_Double (pDX, IDC_OBJ_ROTP + i, m_nOrient [i]);
	if (m_nOrient [i] <= -180.0) {
		m_nOrient [i] = fmod (m_nOrient [i], 360.0);
		if (m_nOrient [i] <= -180.0)
			m_nOrient [i] += 360.0;
	} else if (m_nOrient [i] > 180.0) {
		m_nOrient [i] = fmod (m_nOrient [i], 360.0);
		if (m_nOrient [i] > -180.0)
			m_nOrient [i] -= 360.0;
		}
	}
}

//------------------------------------------------------------------------------

void CObjectPositionTool::EnableControls (BOOL bEnable)
{
CDlgHelpers::EnableControls (IDC_OBJ_LOCX, IDC_OBJ_LOCZ, bEnable);
}

//------------------------------------------------------------------------------

bool CObjectPositionTool::Refresh (void)
{
if (!(m_bInited && theMine))
	return false;

EnableControls (current->ObjectId () != objectManager.Count ());
RefreshRobot ();
UpdateData (FALSE);
DLE.MineView ()->Refresh (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CObjectPositionTool::RefreshRobot (void)
{
if (current->ObjectId () == objectManager.Count ()) {
	memset (m_nPos, 0, sizeof (double) * 3);
	RefreshObjOrient (objectManager.SecretOrient ());
} else {
	CGameObject* pObject = current->Object ();
	m_nPos [0] = pObject->m_location.pos.v.x;
	m_nPos [1] = pObject->m_location.pos.v.y;
	m_nPos [2] = pObject->m_location.pos.v.z;
	RefreshObjOrient (pObject->m_location.orient);
	}
}

//------------------------------------------------------------------------------

void CObjectPositionTool::UpdateRobot (void)
{
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udObjects);
if (current->ObjectId () == objectManager.Count ()) {
	// secret object
	objectManager.SecretOrient ().Set(sin (Radians (m_nOrient [0])), cos (Radians (m_nOrient [0])),
									  sin (Radians (m_nOrient [1])), cos (Radians (m_nOrient [1])),
									  sin (Radians (m_nOrient [2])), cos (Radians (m_nOrient [2])));
} else {
	current->Object ()->Position ().Set (m_nPos [0], m_nPos [1], m_nPos [2]);
	current->Object ()->Orient ().Set(sin (Radians (m_nOrient [0])), cos (Radians (m_nOrient [0])),
									  sin (Radians (m_nOrient [1])), cos (Radians (m_nOrient [1])),
									  sin (Radians (m_nOrient [2])), cos (Radians (m_nOrient [2])));
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh (false);
}

//------------------------------------------------------------------------------

afx_msg void CObjectPositionTool::OnSetLoc ()
{
UpdateRobot ();
}

//------------------------------------------------------------------------------

afx_msg void CObjectPositionTool::OnResetLoc ()
{
Refresh ();
}

//------------------------------------------------------------------------------

void CObjectPositionTool::RefreshObjOrient (CDoubleMatrix &orient)
{
orient.CopyTo (m_nOrient [0], m_nOrient [1], m_nOrient [2]);
m_nOrient [0] = Degrees (m_nOrient [0]);
m_nOrient [1] = Degrees (m_nOrient [1]);
m_nOrient [2] = Degrees (m_nOrient [2]);
// Keep display roughly in the -179 to 180 range
if (m_nOrient [0] + 180 < 0.01)
	m_nOrient [0] += 360;
else if (m_nOrient [0] > 180.01)
	m_nOrient [0] -= 360;
if (m_nOrient [1] + 180 < 0.01)
	m_nOrient [1] += 360;
else if (m_nOrient [1] > 180.01)
	m_nOrient [1] -= 360;
if (m_nOrient [2] + 180 < 0.01)
	m_nOrient [2] += 360;
else if (m_nOrient [2] > 180.01)
	m_nOrient [2] -= 360;
}

//------------------------------------------------------------------------------

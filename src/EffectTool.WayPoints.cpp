
#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#undef abs
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>

#include "mine.h"
#include "dle-xp.h"
#include "toolview.h"

//------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CWayPointTool, CEffectTabDlg)
	ON_WM_HSCROLL ()
	ON_EN_KILLFOCUS (IDC_WAYPOINT_ID, OnEdit)
	ON_EN_KILLFOCUS (IDC_WAYPOINT_SUCC, OnEdit)
	ON_EN_KILLFOCUS (IDC_WAYPOINT_SPEED, OnEdit)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------

BOOL CWayPointTool::OnInitDialog ()
{
if (!CDialog::OnInitDialog ())
	return FALSE;
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------

void CWayPointTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;
CGameObject *pObject = GetEffect (null, false);
EnableControls (pObject != null);
if (pObject) {
	pObject->cType.wayPointInfo.nId = DDX_Int (pDX, IDC_WAYPOINT_ID, pObject->cType.wayPointInfo.nId);
	pObject->cType.wayPointInfo.nSuccessor = DDX_Int (pDX, IDC_WAYPOINT_SUCC, pObject->cType.wayPointInfo.nSuccessor);
	pObject->cType.wayPointInfo.nSpeed = DDX_Int (pDX, IDC_WAYPOINT_SPEED, pObject->cType.wayPointInfo.nSpeed);
	}
}

//------------------------------------------------------------------------

void CWayPointTool::EnableControls (BOOL bEnable)
{
if (!(m_bInited && theMine))
	return;
CDlgHelpers::EnableControls (IDC_WAYPOINT_ID, IDC_WAYPOINT_SPEED, GetEffect (null, false) != null);
}

//------------------------------------------------------------------------

void CWayPointTool::Add (void)
{
if (!AddEffect ())
	return;
CGameObject *pObject = current->Object ();
pObject->Type () = OBJ_EFFECT;
pObject->Id () = WAYPOINT_ID;
pObject->m_info.movementType = MT_NONE;
pObject->m_info.controlType = CT_WAYPOINT;
pObject->m_info.renderType = RT_NONE;
pObject->cType.wayPointInfo.nId = 0;
pObject->cType.wayPointInfo.nSuccessor = 0;
pObject->cType.wayPointInfo.nSpeed = 20;
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CWayPointTool::Copy (void)
{
CGameObject *pObject = GetEffect ();
if (pObject) {
	m_wayPoint = pObject->cType.wayPointInfo;
	m_bValid = 1;
	}
}

//------------------------------------------------------------------------

void CWayPointTool::Paste (CGameObject* pObject, bool bRefresh, bool bVerbose)
{
if (Valid () && (pObject = GetEffect (pObject, bVerbose))) {
	pObject->cType.wayPointInfo = m_wayPoint;
	if (bRefresh)
		Refresh ();
	}
}

//------------------------------------------------------------------------

void CWayPointTool::OnEdit (void)
{
UpdateData (TRUE);
}

//------------------------------------------------------------------------
//eof WayPointTool.cpp
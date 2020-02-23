
#include "stdafx.h"
#include "afxpriv.h"
#include <malloc.h>
#include <stdlib.h>
#undef abs
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>

#include "mine.h"
#include "dle-xp.h"
#include "toolview.h"
#include "TextureManager.h"

//------------------------------------------------------------------------------

static CSliderData sliderData [] = {
	{IDC_OBJ_STRENGTH_SLIDER, 0, 8, 1, null},
	{IDC_OBJ_MASS_SLIDER, 10, 20, 1, null},
	{IDC_OBJ_DRAG_SLIDER, 1, 13, F1_0 / 100, null},
	{IDC_OBJ_EBLOBS_SLIDER, 0, 100, 1, null},
	{IDC_OBJ_LIGHT_SLIDER, 0, 10, 1, null},
	{IDC_OBJ_GLOW_SLIDER, 0, 12, 1, null}
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CObjectPhysicsTool, CObjectTabDlg)
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_WM_PAINT ()
	ON_BN_CLICKED (IDC_OBJ_BRIGHT, OnChange)
	ON_BN_CLICKED (IDC_OBJ_CLOAKED, OnChange)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

BOOL CObjectPhysicsTool::OnInitDialog ()
{
if (!CObjectTabDlg::OnInitDialog ())
	return FALSE;
InitSliders (this, m_data, sliderData, sizeofa (m_data));
Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectPhysicsTool::DoDataExchange (CDataExchange *pDX)
{
}

//------------------------------------------------------------------------------

void CObjectPhysicsTool::EnableControls (BOOL bEnable)
{
CDlgHelpers::EnableControls (IDC_OBJ_STRENGTH_SLIDER, IDT_OBJ_GLOW, bEnable && (objectManager.Count () > 0) && (current->Object ()->Type () == OBJ_ROBOT));
}

//------------------------------------------------------------------------------

bool CObjectPhysicsTool::OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
for (int i = 0; i < sizeofa (m_data); i++) {
	if (m_data [i].OnScroll (scrollCode, thumbPos, pScrollBar)) {
		UpdateRobot ();
		return true;
		}
	}
return false;
}

//------------------------------------------------------------------------------

void CObjectPhysicsTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CObjectTabDlg::OnHScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

void CObjectPhysicsTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CObjectTabDlg::OnVScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

bool CObjectPhysicsTool::Refresh (void)
{
if (!(m_bInited && theMine))
	return false;

if (current->ObjectId () == objectManager.Count ()) {
	EnableControls (FALSE);
	return true;
	}
EnableControls (TRUE);

CGameObject* pObject = current->Object ();
RefreshRobot ();
UpdateData (FALSE);
DLE.MineView ()->Refresh (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CObjectPhysicsTool::RefreshRobot (void)
{
int nType = current->Object ()->Type ();
if (nType != OBJ_ROBOT) {
	BtnCtrl (IDC_OBJ_BRIGHT)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_CLOAKED)->SetCheck (FALSE);
	for (int i = 0; i < sizeofa (m_data); i++)
		m_data [i].SetValue (0);
	return;
	}
int nId = int (current->Object ()->Id ());
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);

BtnCtrl (IDC_OBJ_BRIGHT)->SetCheck (robotInfo.Info ().lighting);
BtnCtrl (IDC_OBJ_CLOAKED)->SetCheck (robotInfo.Info ().cloakType);

m_data [0].SetValue (robotInfo.GetStrength ());
m_data [1].SetValue (FixLog (robotInfo.Info ().mass));
m_data [2].SetValue (robotInfo.Info ().drag);
m_data [3].SetValue (robotInfo.Info ().energyBlobs);
m_data [4].SetValue (robotInfo.Info ().lightCast);
m_data [5].SetValue (robotInfo.Info ().glow);
}
  
//------------------------------------------------------------------------------

void CObjectPhysicsTool::UpdateRobot (void)
{
int nId = int (current->Object ()->Id ());
if ((nId < 0) || (nId >= MAX_ROBOT_ID_D2))
	nId = 0;
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);
robotInfo.Info ().bCustom |= 1;
robotInfo.SetModified (true);

robotInfo.SetStrength (m_data [0].GetValue ());
robotInfo.Info ().mass = m_data [1].GetValue ();
robotInfo.Info ().drag = m_data [2].GetValue ();
robotInfo.Info ().energyBlobs = m_data [3].GetValue ();
robotInfo.Info ().lightCast = m_data [4].GetValue ();
robotInfo.Info ().glow = m_data [5].GetValue ();
robotInfo.Info ().lighting = BtnCtrl (IDC_OBJ_BRIGHT)->GetCheck ();
robotInfo.Info ().cloakType = BtnCtrl (IDC_OBJ_CLOAKED)->GetCheck ();

undoManager.Begin (__FUNCTION__, udRobots);
*robotManager.RobotInfo (nId) = robotInfo;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof objectdlg.cpp

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

#define MAX_WEAPON_TYPE_TABLE 30
ubyte weaponTypeTable [MAX_WEAPON_TYPE_TABLE] = {
	0x00, 0x05, 0x06, 0x0a, 0x0b, 0x0e, 0x11, 0x14, 0x15, 0x16,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
	0x2e, 0x30, 0x32, 0x34, 0x35, 0x37, 0x39, 0x3a, 0x3c, 0x3d
	};

static CSliderData sliderData [] = {
	{IDC_OBJ_SKILL_SLIDER, 0, 4, 1, null},
	{IDC_OBJ_FOV_SLIDER, -10, 10, F1_0 / 10, null},
	{IDC_OBJ_FIREWAIT1_SLIDER, 1, 35, F1_0 / 5, null},
	{IDC_OBJ_FIREWAIT2_SLIDER, 1, 35, F1_0 / 5, null},
	{IDC_OBJ_TURNTIME_SLIDER, 0, 10, F1_0 / 10, null},
	{IDC_OBJ_MAXSPEED_SLIDER, 0, 140, F1_0, null},
	{IDC_OBJ_FIRERATE_SLIDER, 1, 18, 1, null},
	{IDC_OBJ_EVADESPEED_SLIDER, 0, 6, 1, null},
	{IDC_OBJ_CIRCLEDIST_SLIDER, 0, 60, 1, null},
	{IDC_OBJ_AIM_SLIDER, 2, 4, 0x40, null}
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CObjectCombatTool, CObjectTabDlg)
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_CBN_SELCHANGE (IDC_OBJ_WEAPON1, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_WEAPON2, OnChange)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

char* __cdecl FormatSkill (char* szValue, int nValue)
{
	static char *szSkills [5] = {"Trainee", "Rookie", "Hotshot", "Ace", "Insane"};

strcpy (szValue, szSkills [nValue]);
return szValue;
}

//------------------------------------------------------------------------------

BOOL CObjectCombatTool::OnInitDialog ()
{
if (!CObjectTabDlg::OnInitDialog ())
	return FALSE;
InitSliders (this, m_data, sliderData, sizeofa (m_data));
m_data [0].SetFormatter (FormatSkill);
CBInit (CBWeapon1 (), (char **) 7000, null, null, MAX_WEAPON_TYPES, 3, true);
CBInit (CBWeapon2 (), (char **) 7000, null, null, MAX_WEAPON_TYPES, 3, true);
Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectCombatTool::DoDataExchange (CDataExchange *pDX)
{
}

//------------------------------------------------------------------------------

void CObjectCombatTool::EnableControls (BOOL bEnable)
{
CDlgHelpers::EnableControls (IDC_OBJ_WEAPON1, IDT_OBJ_AIM, bEnable && (objectManager.Count () > 0) && (current->Object ()->Type () == OBJ_ROBOT));
}

//------------------------------------------------------------------------------

bool CObjectCombatTool::OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
for (int i = 0; i < sizeofa (m_data); i++) {
	if (m_data [i].OnScroll (scrollCode, thumbPos, pScrollBar)) {
		if (i)
			UpdateRobot ();
		else
			RefreshRobot ();
		return true;
		}
	}
return false;
}

//------------------------------------------------------------------------------

void CObjectCombatTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CObjectTabDlg::OnHScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

void CObjectCombatTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CObjectTabDlg::OnVScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

bool CObjectCombatTool::Refresh (void)
{
if (!(m_bInited && theMine))
	return false;
if (current->ObjectId () == objectManager.Count ()) {
	EnableControls (FALSE);
	return true;
	}
EnableControls (TRUE);
RefreshRobot ();
UpdateData (FALSE);
DLE.MineView ()->Refresh (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CObjectCombatTool::RefreshRobot (void)
{
int nType = current->Object ()->Type ();

  // get selection
if (nType != OBJ_ROBOT) {
	CBWeapon1 ()->SetCurSel (-1);
	CBWeapon2 ()->SetCurSel (-1);
	for (int i = 0; i < sizeofa (m_data); i++)
		m_data [i].SetValue (0);
	return;
	}
int nId = int (current->Object ()->Id ());
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);

SelectItemData (CBWeapon1 (), (int) (robotInfo.Info ().weaponType [0] ? robotInfo.Info ().weaponType [0] : -1));
SelectItemData (CBWeapon2 (), (int) robotInfo.Info ().weaponType [1]);
int nSkill = m_data [0].GetValue ();
m_data [1].SetValue (robotInfo.Info ().combat [nSkill].fieldOfView);
m_data [2].SetValue (robotInfo.Info ().combat [nSkill].firingWait [0]);
m_data [3].SetValue (robotInfo.Info ().combat [nSkill].firingWait [1]);
m_data [4].SetValue (robotInfo.Info ().combat [nSkill].turnTime);
m_data [5].SetValue (robotInfo.Info ().combat [nSkill].maxSpeed);
m_data [6].SetValue (robotInfo.Info ().combat [nSkill].rapidFire);
m_data [7].SetValue (robotInfo.Info ().combat [nSkill].evadeSpeed);
m_data [8].SetValue (robotInfo.Info ().combat [nSkill].circleDistance);
m_data [9].SetValue (robotInfo.Info ().aim + 1);
}
  
//------------------------------------------------------------------------------

void CObjectCombatTool::UpdateRobot ()
{
int nId = int (current->Object ()->Id ());
if ((nId < 0) || (nId >= MAX_ROBOT_ID_D2))
	nId = 0;
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);

int index;

if (0 <= (index = CBWeapon1 ()->GetCurSel ())) {
	robotInfo.Info ().weaponType [0] = (ubyte) CBWeapon1 ()->GetItemData (index);
	if (robotInfo.Info ().weaponType [0] < 0)
		robotInfo.Info ().weaponType [0] = 0;
	}
if (0 <= (index = CBWeapon2 ()->GetCurSel ()))
	robotInfo.Info ().weaponType [1] = (ubyte) CBWeapon2 ()->GetItemData (index);

int nSkill = SlCtrl (IDC_OBJ_SKILL_SLIDER)->GetPos ();
robotInfo.Info ().bCustom |= 1;
robotInfo.SetModified (true);
robotInfo.Info ().combat [nSkill].fieldOfView = m_data [1].GetValue ();
robotInfo.Info ().combat [nSkill].firingWait [0] = m_data [2].GetValue ();
robotInfo.Info ().combat [nSkill].firingWait [1] = m_data [3].GetValue ();
robotInfo.Info ().combat [nSkill].turnTime = m_data [4].GetValue ();
robotInfo.Info ().combat [nSkill].maxSpeed = m_data [5].GetValue ();
robotInfo.Info ().combat [nSkill].rapidFire = m_data [6].GetValue ();
robotInfo.Info ().combat [nSkill].evadeSpeed = m_data [7].GetValue ();
robotInfo.Info ().combat [nSkill].circleDistance = m_data [8].GetValue ();
robotInfo.Info ().aim = m_data [9].GetValue () - 1;

undoManager.Begin (__FUNCTION__, udRobots);
*robotManager.RobotInfo (nId) = robotInfo;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof objectdlg.cpp
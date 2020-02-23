
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
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CObjectSoundTool, CObjectTabDlg)
	ON_CBN_SELCHANGE (IDC_OBJ_SOUND_EXPLODE, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_SOUND_ATTACK, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_SOUND_SEE, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_SOUND_CLAW, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_SOUND_DEATH, OnChange)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BOOL CObjectSoundTool::OnInitDialog ()
{
if (!CObjectTabDlg::OnInitDialog ())
	return FALSE;

CGameObject *pObject = current->Object ();

CStringResource res;

for (int i = 0; i < 196; i++) {
	res.Clear ();
	res.Load (6000 + i);
	int nSound = atoi (res.Value ());
	int index = CBSoundExpl ()->AddString (res.Value () + 3);
	CBSoundExpl ()->SetItemData (index, nSound);
	index = CBSoundSee ()->AddString (res.Value () + 3);
	CBSoundSee ()->SetItemData (index, nSound);
	index = CBSoundAttack ()->AddString (res.Value () + 3);
	CBSoundAttack ()->SetItemData (index, nSound);
	index = CBSoundClaw ()->AddString (res.Value () + 3);
	CBSoundClaw ()->SetItemData (index, nSound);
	index = CBSoundDeath ()->AddString (res.Value () + 3);
	CBSoundDeath ()->SetItemData (index, nSound);
	}
Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectSoundTool::DoDataExchange (CDataExchange *pDX)
{
}

//------------------------------------------------------------------------------

void CObjectSoundTool::EnableControls (BOOL bEnable)
{
CDlgHelpers::EnableControls (IDC_OBJ_SOUND_EXPLODE, IDC_OBJ_SOUND_DEATH, bEnable && (objectManager.Count () > 0) && (current->Object ()->Type () == OBJ_ROBOT));
}

//------------------------------------------------------------------------------

bool CObjectSoundTool::Refresh (void)
{
if (!(m_bInited && theMine))
	return false;

if (current->ObjectId () == objectManager.Count ()) {
	EnableControls (FALSE);
	return true;
	}
EnableControls (TRUE);

// update object list box
CGameObject* pObject = current->Object ();
// gray contains and behavior if not a robot type object
if (pObject->Type () != OBJ_ROBOT) {
	for (int i = IDC_OBJ_SOUND_EXPLODE; i <= IDC_OBJ_SOUND_DEATH; i++)
		CBCtrl (i)->SetCurSel (-1);
	}
else {
	for (int i = IDC_OBJ_SOUND_EXPLODE; i <= IDC_OBJ_SOUND_DEATH; i++)
		CBCtrl (i)->SetCurSel (0);
	}

RefreshRobot ();
UpdateData (FALSE);
DLE.MineView ()->Refresh (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CObjectSoundTool::RefreshRobot (void)
{
CGameObject* pObject = current->Object ();
int nType = pObject->Type ();

if (nType != OBJ_ROBOT) {
	CBSoundExpl ()->SetCurSel (-1);
	CBSoundSee ()->SetCurSel (-1);
	CBSoundAttack ()->SetCurSel (-1);
	CBSoundClaw ()->SetCurSel (-1);
	CBSoundDeath ()->SetCurSel (-1);
	return;
	}

int nId = int (pObject->Id ());
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);

SelectItemData (CBSoundExpl (), (int) robotInfo.Info ().expl [1].nSound);
SelectItemData (CBSoundSee (), (int) robotInfo.Info ().sounds.see);
SelectItemData (CBSoundAttack (), (int) robotInfo.Info ().sounds.attack);
SelectItemData (CBSoundClaw (), (int) robotInfo.Info ().sounds.claw);
SelectItemData (CBSoundDeath (), (int) robotInfo.Info ().deathRollSound);
}
  
//------------------------------------------------------------------------------

void CObjectSoundTool::UpdateRobot (void)
{
CGameObject* pObject = current->Object ();
int nId = int (pObject->Id ());
if (nId < 0 || nId >= MAX_ROBOT_ID_D2)
	nId = 0;
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);
robotInfo.Info ().bCustom |= 1;
robotInfo.SetModified (true);

int index;

if (0 <= (index = CBSoundExpl ()->GetCurSel ()))
	robotInfo.Info ().expl [1].nSound = (ubyte) CBSoundExpl ()->GetItemData (index);
if (0 <= (index = CBSoundSee ()->GetCurSel ()))
	robotInfo.Info ().sounds.see = (ubyte) CBSoundSee ()->GetItemData (index);
if (0 <= (index = CBSoundAttack ()->GetCurSel ()))
	robotInfo.Info ().sounds.attack = (ubyte) CBSoundAttack ()->GetItemData (index);
if (0 <= (index = CBSoundClaw ()->GetCurSel ()))
	robotInfo.Info ().sounds.claw = (ubyte) CBSoundClaw ()->GetItemData (index);
if (0 <= (index = CBSoundDeath ()->GetCurSel ()))
	robotInfo.Info ().deathRollSound = (ubyte) CBSoundDeath ()->GetItemData (index);

undoManager.Begin (__FUNCTION__, udRobots);
*robotManager.RobotInfo (nId) = robotInfo;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof objectdlg.cpp
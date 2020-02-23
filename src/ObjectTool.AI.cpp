
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

char *aiOptionTable [MAX_AI_OPTIONS_D2] = {
	"Still", "Normal", "Get behind", "Drop Bombs", "Snipe", "Station", "Follow", "Static", "Smart Bombs"
};

//------------------------------------------------------------------------------

ubyte aiBehaviorTable [MAX_AI_BEHAVIOR_TABLE] = {
	AIB_STILL, AIB_NORMAL, AIB_GET_BEHIND, AIB_RUN_FROM, AIB_FOLLOW_PATH, AIB_STATION, AIB_SNIPE
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CObjectAITool, CObjectTabDlg)
	ON_BN_CLICKED (IDC_OBJ_AI_KAMIKAZE, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_COMPANION, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_THIEF, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_SMARTBLOBS, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_PURSUE, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_CHARGE, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_EDRAIN, OnChange)
	ON_BN_CLICKED (IDC_OBJ_AI_ENDSLEVEL, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_AI_BOSSTYPE, OnChange)
	ON_CBN_SELCHANGE (IDC_OBJ_CLASS_AI, OnChange)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

char *pszBossTypes [] = {"none", "Boss 1", "Boss 2", "Red Fatty", "Water Boss", "Fire Boss", "Ice Boss", "Alien 1", "Alien 2", "Vertigo", "Red Guard", null};

BOOL CObjectAITool::OnInitDialog ()
{
if (!CTabDlg::OnInitDialog ())
	return FALSE;
CBInit (CBObjClassAI (), (char**) aiOptionTable, null, aiBehaviorTable, (DLE.IsD1File ()) ? MAX_AI_OPTIONS_D1 : MAX_AI_OPTIONS_D2);
CComboBox* pcb = CBBossType ();
pcb->ResetContent ();
for (char** psz = pszBossTypes; *psz; psz++) {
	int index = pcb->AddString (*psz);
	pcb->SetItemData(index++, (int) (psz - pszBossTypes));
	}
CGameObject *pObject = current->Object ();
Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectAITool::DoDataExchange (CDataExchange *pDX)
{
}

//------------------------------------------------------------------------------

void CObjectAITool::EnableControls (BOOL bEnable)
{
CDlgHelpers::EnableControls (IDC_OBJ_CLASS_AI, IDC_OBJ_AI_BOSSTYPE, bEnable && (objectManager.Count () > 0) && (current->Object ()->Type () == OBJ_ROBOT));
}

//------------------------------------------------------------------------------

bool CObjectAITool::Refresh (void)
{
if (!(m_bInited && theMine))
	return false;
if (current->ObjectId () == objectManager.Count ()) {
	EnableControls (FALSE);
	return true;
	}
EnableControls (TRUE);

CGameObject* pObject = current->Object ();
m_bEndsLevel = (pObject->Type () == OBJ_ROBOT) && (robotManager.RobotInfo (pObject->Id ())->Info ().bossFlag > 0);
RefreshRobot ();
UpdateData (FALSE);
DLE.MineView ()->Refresh (FALSE);
return true;
}

//------------------------------------------------------------------------------

void CObjectAITool::RefreshRobot (void)
{
CGameObject* pObject = current->Object ();
int nType = pObject->Type ();

  // get selection
if (nType != OBJ_ROBOT) {
	CBObjClassAI ()->SetCurSel (-1);
	// update check boxes
	BtnCtrl (IDC_OBJ_AI_KAMIKAZE)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_COMPANION)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_THIEF)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_SMARTBLOBS)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_PURSUE)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_CHARGE)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_EDRAIN)->SetCheck (FALSE);
	BtnCtrl (IDC_OBJ_AI_ENDSLEVEL)->SetCheck (FALSE);
	return;
	}

int nId = int (pObject->Id ());
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);

SelectItemData (CBObjClassAI (), (int) robotInfo.Info ().behavior);
char bossFlag = (robotInfo.Info ().bossFlag < 0) ? -robotInfo.Info ().bossFlag : robotInfo.Info ().bossFlag;
SelectItemData (CBBossType (), (int) (bossFlag < 21) ? bossFlag : bossFlag - 18);
// update check boxes
BtnCtrl (IDC_OBJ_AI_KAMIKAZE)->SetCheck (robotInfo.Info ().kamikaze);
BtnCtrl (IDC_OBJ_AI_COMPANION)->SetCheck (robotInfo.Info ().companion);
BtnCtrl (IDC_OBJ_AI_THIEF)->SetCheck (robotInfo.Info ().thief);
BtnCtrl (IDC_OBJ_AI_SMARTBLOBS)->SetCheck (robotInfo.Info ().smartBlobs);
BtnCtrl (IDC_OBJ_AI_PURSUE)->SetCheck (robotInfo.Info ().pursuit);
BtnCtrl (IDC_OBJ_AI_CHARGE)->SetCheck (robotInfo.Info ().attackType);
BtnCtrl (IDC_OBJ_AI_EDRAIN)->SetCheck (robotInfo.Info ().drainEnergy);
BtnCtrl (IDC_OBJ_AI_ENDSLEVEL)->SetCheck (m_bEndsLevel);
}
  
//------------------------------------------------------------------------------

void CObjectAITool::UpdateRobot (void)
{
CGameObject* pObject = current->Object ();
int nId = int (pObject->Id ());

if (nId < 0 || nId >= MAX_ROBOT_ID_D2)
	nId = 0;

CRobotInfo robotInfo = *robotManager.RobotInfo (nId);
robotInfo.Info ().bCustom |= 1;
robotInfo.SetModified (true);
robotInfo.Info ().kamikaze = BtnCtrl (IDC_OBJ_AI_KAMIKAZE)->GetCheck ();
robotInfo.Info ().companion = BtnCtrl (IDC_OBJ_AI_COMPANION)->GetCheck ();
robotInfo.Info ().thief = BtnCtrl (IDC_OBJ_AI_THIEF)->GetCheck ();
robotInfo.Info ().smartBlobs = BtnCtrl (IDC_OBJ_AI_SMARTBLOBS)->GetCheck ();
robotInfo.Info ().pursuit = BtnCtrl (IDC_OBJ_AI_PURSUE)->GetCheck ();
robotInfo.Info ().attackType = BtnCtrl (IDC_OBJ_AI_CHARGE)->GetCheck ();
robotInfo.Info ().drainEnergy = BtnCtrl (IDC_OBJ_AI_EDRAIN)->GetCheck ();
m_bEndsLevel = BtnCtrl (IDC_OBJ_AI_ENDSLEVEL)->GetCheck ();

// get list box changes
int index = CBObjClassAI ()->GetCurSel ();
robotInfo.Info ().behavior = (ubyte) CBObjClassAI ()->GetItemData (index);
index = CBBossType ()->GetCurSel ();
if (0 <= index) {
	robotInfo.Info ().bossFlag = (ubyte) CBBossType ()->GetItemData (index);
	if ((robotInfo.Info ().bossFlag = (ubyte) CBBossType ()->GetItemData (index)) > 2)
			robotInfo.Info ().bossFlag += 18;
	if (!m_bEndsLevel)
		robotInfo.Info ().bossFlag = -robotInfo.Info ().bossFlag;
	}

undoManager.Begin (__FUNCTION__, udRobots);
*robotManager.RobotInfo (nId) = robotInfo;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof objectdlg.cpp
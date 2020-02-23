
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

static CSliderData sliderData [] = {
	{IDC_OBJ_SCORE_SLIDER, 0, 600, 50, null},
	{IDC_OBJ_DEATHROLL_SLIDER, 0, 10, 1, null},
	{IDC_OBJ_EXPLSIZE_SLIDER, 0, 100, 1, null},
	{IDC_OBJ_CONT_COUNT_SLIDER, 0, 100, 1, null},
	{IDC_OBJ_CONT_PROB_SLIDER, 0, 16, 1, null}
	};

#define MAX_EXP2_VCLIP_NUM_TABLE 4
ubyte exp2VClipTable [MAX_EXP2_VCLIP_NUM_TABLE] = {
	0x00, 0x03, 0x07, 0x3c
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CObjectDeathTool, CObjectTabDlg)
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_CBN_SELCHANGE (IDC_OBJ_CONT_TYPE, OnSetContType)
	ON_CBN_SELCHANGE (IDC_OBJ_CONT_ID, OnChange)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

char* __cdecl FormatProbability (char* szValue, int nValue)
{
sprintf (szValue, "%d %%", int (float (nValue) * 100.0f / 16.0f + 0.5f));
return szValue;
}

//------------------------------------------------------------------------------

BOOL CObjectDeathTool::OnInitDialog ()
{
if (!CObjectTabDlg::OnInitDialog ())
	return FALSE;
InitSliders (this, m_data, sliderData, sizeofa (m_data));
m_data [4].SetFormatter (FormatProbability);

CGameObject *pObject = current->Object ();
CBInit (CBExplType (), (char **) "explosion", null, exp2VClipTable, MAX_EXP2_VCLIP_NUM_TABLE, 2);
CBContType ()->SetItemData (CBContType ()->AddString ("Robot"), OBJ_ROBOT);
CBContType ()->SetItemData (CBContType ()->AddString ("Powerup"), OBJ_POWERUP);
CBContType ()->SetItemData (CBContType ()->AddString ("(none)"), -1);
Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectDeathTool::DoDataExchange (CDataExchange *pDX)
{
}

//------------------------------------------------------------------------------

void CObjectDeathTool::EnableControls (BOOL bEnable)
{
CDlgHelpers::EnableControls (IDC_OBJ_SCORE_SLIDER, IDC_OBJ_CONT_ID, bEnable && (objectManager.Count () > 0) && (current->Object ()->Type () == OBJ_ROBOT));
}

//------------------------------------------------------------------------------

bool CObjectDeathTool::OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
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

void CObjectDeathTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CObjectTabDlg::OnHScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

void CObjectDeathTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CObjectTabDlg::OnVScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

bool CObjectDeathTool::Refresh (void)
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

void CObjectDeathTool::RefreshRobot (void)
{
int nType = current->Object ()->Type ();
if (nType != OBJ_ROBOT) {
	CBExplType ()->SetCurSel (-1);
	CBContType ()->SetCurSel (-1);
	CBContId ()->SetCurSel (-1);
	for (int i = 0; i < sizeofa (m_data); i++)
		m_data [i].SetValue (0);
	return;
	}

int nId = int (current->Object ()->Id ());
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);

CBContId ()->ResetContent ();
switch (robotInfo.Info ().contents.type) {
	case OBJ_ROBOT: /* an evil enemy */
		CBInit (CBContId (), (char **) ROBOT_STRING_TABLE, null, null, MAX_ROBOT_ID_D2, 1, true);
		break;
	case OBJ_POWERUP: // a powerup you can pick up
		CBInit (CBContId (), (char **) POWERUP_STRING_TABLE, null, null, MAX_POWERUP_IDS, 3, true);
		break;
	}

SelectItemData (CBContId (), (int) robotInfo.Info ().contents.id);
SelectItemData (CBContType (), (int) robotInfo.Info ().contents.type);
m_data [0].SetValue (robotInfo.Info ().scoreValue);
m_data [1].SetValue (robotInfo.Info ().deathRoll);
m_data [2].SetValue (robotInfo.Info ().badass);
m_data [3].SetValue (robotInfo.Info ().contents.count);
m_data [4].SetValue (robotInfo.Info ().contents.prob);
}
  
//------------------------------------------------------------------------------

void CObjectDeathTool::UpdateRobot ()
{
int nId = int (current->Object ()->Id ());
if ((nId < 0) || (nId >= MAX_ROBOT_ID_D2))
	nId = 0;
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);
robotInfo.Info ().bCustom |= 1;
robotInfo.SetModified (true);

robotInfo.Info ().scoreValue = m_data [0].GetValue ();
robotInfo.Info ().deathRoll = m_data [1].GetValue ();
robotInfo.Info ().badass = m_data [2].GetValue ();
robotInfo.Info ().contents.count = m_data [3].GetValue ();
robotInfo.Info ().contents.prob = m_data [4].GetValue ();

int index;
if (0 <= (index = CBExplType ()->GetCurSel ()))
	robotInfo.Info ().expl [1].nClip = (ubyte) CBExplType ()->GetItemData (index);
if (0 <= (index = CBContType ()->GetCurSel ()))
	robotInfo.Info ().contents.type = (ubyte) CBContType ()->GetItemData (index);
if (0 <= (index = CBContId ()->GetCurSel ()) - 1)
	robotInfo.Info ().contents.id = (ubyte) CBContId ()->GetItemData (index);

undoManager.Begin (__FUNCTION__, udRobots);
*robotManager.RobotInfo (nId) = robotInfo;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------

void CObjectDeathTool::OnSetContType ()
{
int i = CBContType ()->GetCurSel ();
if (0 > i)
	return;
int nId = int (current->Object ()->Id ());
CRobotInfo *robotInfo = robotManager.RobotInfo (nId);
robotInfo->Info ().contents.type = (ubyte) CBContType ()->GetItemData (i);
RefreshRobot ();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof objectdlg.cpp
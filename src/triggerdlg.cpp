// Copyright (C) 1997 Bryan Aamot
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
#include "TextureManager.h"

                        /*--------------------------*/

BEGIN_MESSAGE_MAP (CTriggerTool, CTexToolDlg)
	ON_WM_PAINT ()
	ON_WM_HSCROLL ()
	ON_BN_CLICKED (IDC_TRIGGER_ADD, OnAddTrigger)
	ON_BN_CLICKED (IDC_TRIGGER_DELETE, OnDeleteTrigger)
	ON_BN_CLICKED (IDC_TRIGGER_DELETEALL, OnDeleteTriggerAll)
	ON_BN_CLICKED (IDC_TRIGGER_ADDTGT, OnAddTarget)
	ON_BN_CLICKED (IDC_TRIGGER_DELTGT, OnDeleteTarget)
	ON_BN_CLICKED (IDC_TRIGGER_ADDWALLTGT, OnAddWallTarget)
	ON_BN_CLICKED (IDC_TRIGGER_ADDOBJTGT, OnAddObjTarget)
	ON_BN_CLICKED (IDC_TRIGGER_COPY, OnCopyTrigger)
	ON_BN_CLICKED (IDC_TRIGGER_PASTE, OnPasteTrigger)
	ON_BN_CLICKED (IDC_TRIGGER_STANDARD, OnStandardTrigger)
	ON_BN_CLICKED (IDC_TRIGGER_OBJECT, OnObjectTrigger)

	ON_BN_CLICKED (IDC_TRIGGER_NOMESSAGE, OnD2Flag1)
	ON_BN_CLICKED (IDC_TRIGGER_ONESHOT, OnD2Flag2)
	ON_BN_CLICKED (IDC_TRIGGER_PERMANENT, OnD2Flag3)
	ON_BN_CLICKED (IDC_TRIGGER_ALTERNATE, OnD2Flag4)
	ON_BN_CLICKED (IDC_TRIGGER_SET_ORIENT, OnD2Flag5)
	ON_BN_CLICKED (IDC_TRIGGER_AUTOPLAY, OnD2Flag6)
	ON_BN_CLICKED (IDC_TRIGGER_SILENT, OnD2Flag7)

	ON_BN_CLICKED (IDC_TRIGGER_CONTROLDOORS, OnD1Flag1)
	ON_BN_CLICKED (IDC_TRIGGER_SHIELDDRAIN, OnD1Flag2)
	ON_BN_CLICKED (IDC_TRIGGER_ENERGYDRAIN, OnD1Flag3)
	ON_BN_CLICKED (IDC_TRIGGER_ENDLEVEL, OnD1Flag4)
	ON_BN_CLICKED (IDC_TRIGGER_SECRETEXIT, OnD1Flag5)
	ON_BN_CLICKED (IDC_TRIGGER_ACTIVE, OnD1Flag6)
	ON_BN_CLICKED (IDC_TRIGGER_ONESHOTD1, OnD1Flag7)
	ON_BN_CLICKED (IDC_TRIGGER_ROBOTMAKER, OnD1Flag8)
	ON_BN_CLICKED (IDC_TRIGGER_ILLUSIONOFF, OnD1Flag9)
	ON_BN_CLICKED (IDC_TRIGGER_ILLUSIONON, OnD1Flag10)
	ON_BN_CLICKED (IDC_TRIGGER_OPENWALL, OnD1Flag11)
	ON_BN_CLICKED (IDC_TRIGGER_CLOSEWALL, OnD1Flag12)

	ON_BN_CLICKED (IDC_TRIGGER_ADD_OPENDOOR, OnAddOpenDoor)
	ON_BN_CLICKED (IDC_TRIGGER_ADD_ROBOTMAKER, OnAddRobotMaker)
	ON_BN_CLICKED (IDC_TRIGGER_ADD_SHIELDDRAIN, OnAddShieldDrain)
	ON_BN_CLICKED (IDC_TRIGGER_ADD_ENERGYDRAIN, OnAddEnergyDrain)
	ON_BN_CLICKED (IDC_TRIGGER_ADD_CONTROLPANEL, OnAddControlPanel)
	ON_CBN_SELCHANGE (IDC_TRIGGER_TRIGGERNO, OnSetTrigger)
	ON_CBN_SELCHANGE (IDC_TRIGGER_D2TYPE, OnSetType)
	ON_CBN_SELCHANGE (IDC_TRIGGER_TARGETLIST, OnSetTarget)
	ON_CBN_SELCHANGE (IDC_TRIGGER_TEXTURE1, OnSelect1st)
	ON_CBN_SELCHANGE (IDC_TRIGGER_TEXTURE2, OnSelect2nd)
	ON_EN_KILLFOCUS (IDC_TRIGGER_STRENGTH, OnStrength)
	ON_EN_KILLFOCUS (IDC_TRIGGER_TIME, OnTime)
	//ON_EN_UPDATE (IDC_TRIGGER_STRENGTH, OnStrength)
	//ON_EN_UPDATE (IDC_TRIGGER_TIME, OnTime)
END_MESSAGE_MAP ()

static short triggerFlagsD1 [MAX_TRIGGER_FLAGS] = {
	TRIGGER_CONTROL_DOORS,
	TRIGGER_SHIELD_DAMAGE,
	TRIGGER_ENERGY_DRAIN,
	TRIGGER_EXIT,
	TRIGGER_SECRET_EXIT,
	TRIGGER_ON,
	TRIGGER_ONE_SHOT,
	TRIGGER_MATCEN,
	TRIGGER_ILLUSION_OFF,
	TRIGGER_ILLUSION_ON,
	TRIGGER_OPEN_WALL,
	TRIGGER_CLOSE_WALL
	};

static int d2FlagXlat [] = {0, 1, 3, 4, 5, 7, 6};

//------------------------------------------------------------------------
// DIALOG - CTriggerTool (constructor)
//------------------------------------------------------------------------

CTriggerTool::CTriggerTool (CPropertySheet *pParent)
	: CTexToolDlg (nLayout ? IDD_TRIGGERDATA2 : IDD_TRIGGERDATA, pParent, IDC_TRIGGER_SHOW, 6, RGB (0,0,0), true)
{
Reset ();
}

								/*--------------------------*/

CTriggerTool::~CTriggerTool ()
{
if (m_bInited) {
	m_showObjWnd.DestroyWindow ();
	m_showTexWnd.DestroyWindow ();
	}
}

                        /*--------------------------*/

void CTriggerTool::LoadTextureListBoxes () 
{
	int			nTextures, iTexture, index;
	CComboBox	*cbTexture1 = CBTexture1 ();
	CComboBox	*cbTexture2 = CBTexture2 ();

short texture1 = Texture1 ();
short texture2 = Texture2 ();

if ((texture1 < 0) || (texture1 >= MAX_TEXTURES))
	texture1 = 0;
if ((texture2 < 0) || (texture2 >= MAX_TEXTURES))
	texture2 = 0;

cbTexture1->ResetContent ();
cbTexture2->ResetContent ();
index = cbTexture1->AddString ("(none)");
nTextures = textureManager.MaxTextures ();
for (iTexture = 0; iTexture < nTextures; iTexture++) {
	char* p = textureManager.Name (iTexture);
	if (!strstr (p, "frame")) {
		index = cbTexture1->AddString (p);
		cbTexture1->SetItemData (index, iTexture);
		if (texture1 == iTexture)
			cbTexture1->SetCurSel (index);
		index = cbTexture2->AddString (iTexture ? p : "(none)");
		if (texture2 == iTexture)
			cbTexture2->SetCurSel (index);
		cbTexture2->SetItemData (index, iTexture);
		}
	}
}

								/*--------------------------*/

void CTriggerTool::Reset ()
{
m_nTrigger = 
m_nStdTrigger = 
m_nObjTrigger = -1;
m_nClass = 0;
m_nType = 0;
m_nStrength = 0;
m_nTime = 0;
m_bAutoAddWall = 1;
m_targets = 0;
m_iTarget = -1;
m_nSliderValue = 10;
m_bFindTrigger = true;
memset (m_bD1Flags, 0, sizeof (m_bD1Flags));
memset (m_bD2Flags, 0, sizeof (m_bD2Flags));
*m_szTarget = '\0';
}

                        /*--------------------------*/

typedef struct tTriggerData {
	char	*pszName;
	int	nType;
} tTriggerData;

static tTriggerData triggerData [] = {
	{"open door", TT_OPEN_DOOR},
	{"close door", TT_CLOSE_DOOR},
	{"make robots", TT_MATCEN},
	{"exit", TT_EXIT},
	{"secret exit", TT_SECRET_EXIT},
	{"illusion off", TT_ILLUSION_OFF},
	{"illusion on", TT_ILLUSION_ON},
	{"unlock door", TT_UNLOCK_DOOR},
	{"lock door", TT_LOCK_DOOR},
	{"open wall", TT_OPEN_WALL},
	{"close wall", TT_CLOSE_WALL},
	{"illusory wall", TT_ILLUSORY_WALL},
	{"light off", TT_LIGHT_OFF},
	{"light on", TT_LIGHT_ON},
	{"teleport", TT_TELEPORT},
	{"speed boost", TT_SPEEDBOOST},
	{"camera", TT_CAMERA},
	{"damage shields", TT_SHIELD_DAMAGE_D2},
	{"drain energy", TT_ENERGY_DRAIN_D2},
	{"change texture", TT_CHANGE_TEXTURE},
	{"countdown", TT_COUNTDOWN},
	{"spawn bot", TT_SPAWN_BOT},
	{"set spawn", TT_SET_SPAWN},
	{"message", TT_MESSAGE},
	{"sound", TT_SOUND},
	{"master", TT_MASTER},
	{"enable", TT_ENABLE_TRIGGER},
	{"disable", TT_DISABLE_TRIGGER}
	};


BOOL CTriggerTool::OnInitDialog ()
{
CTexToolDlg::OnInitDialog ();
CreateImgWnd (&m_showObjWnd, IDC_TRIGGER_SHOW_OBJ);
CreateImgWnd (&m_showTexWnd, IDC_TRIGGER_SHOW_TEXTURE);
CComboBox *pcb = CBType ();
pcb->ResetContent();
if (DLE.IsD2File ()) {
	int h, i, j = sizeof (triggerData) / sizeof (tTriggerData);
	for (i = 0; i < j; i++) {
		h = pcb->AddString (triggerData [i].pszName);
		pcb->SetItemData (h, triggerData [i].nType);
		}
	}
else
	pcb->AddString ("n/a");
pcb->SetCurSel (0);
InitSlider (IDC_TRIGGER_SLIDER, 1, 10);
int i;
for (i = 1; i <= 10; i++)
	SlCtrl (IDC_TRIGGER_SLIDER)->SetTic (i);
LoadTextureListBoxes ();
m_bInited = true;
return TRUE;
}

								/*--------------------------*/

bool CTriggerTool::TriggerHasSlider (void)
{
return 
	(m_nType == TT_SPEEDBOOST) || 
	(m_nType == TT_TELEPORT) ||
	(m_nType == TT_SPAWN_BOT);
}

								/*--------------------------*/

void CTriggerTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;

	static char *pszSmokeParams [] = {"life", "speed", "density", "volume", "drift", "", "", "brightness"};

DDX_CBIndex (pDX, IDC_TRIGGER_TRIGGERNO, m_nTrigger);
DDX_CBIndex (pDX, IDC_TRIGGER_D2TYPE, m_nType);
if (pDX->m_bSaveAndValidate)
	m_nType = int (CBType ()->GetItemData (CBType ()->GetCurSel ()));
else
	SelectItemData (CBType (), m_nType);
DDX_Text (pDX, IDC_TRIGGER_TIME, m_nTime);
int i;
for (i = 0; i < 2; i++)
	DDX_Check (pDX, IDC_TRIGGER_NOMESSAGE + i, m_bD2Flags [i]);
for (i = 2; i < 7; i++)
	DDX_Check (pDX, IDC_TRIGGER_NOMESSAGE + i, m_bD2Flags [d2FlagXlat [i]]);
for (i = 0; i < MAX_TRIGGER_FLAGS; i++)
	DDX_Check (pDX, IDC_TRIGGER_CONTROLDOORS + i, m_bD1Flags [i]);
if (TriggerHasSlider () || (m_nType == TT_SHIELD_DAMAGE_D2) || (m_nType == TT_ENERGY_DRAIN_D2))
	DDX_Double (pDX, IDC_TRIGGER_STRENGTH, m_nStrength, -1000, 1000, "%3.1f");
else if ((m_nType == TT_MESSAGE) || (m_nType == TT_SOUND))
	DDX_Double (pDX, IDC_TRIGGER_STRENGTH, m_nStrength, 0, 1000, "%1.0f");
else if ((DLE.IsD1File ()) && (m_bD1Flags [1] || m_bD1Flags [2]))	// D1 shield/energy drain
	DDX_Double (pDX, IDC_TRIGGER_STRENGTH, m_nStrength, -10000, 10000, "%1.0f");
else
	DDX_Double (pDX, IDC_TRIGGER_STRENGTH, m_nStrength, 0, 64, "%1.0f");
DDX_Text (pDX, IDC_TRIGGER_TARGET, m_szTarget, sizeof (m_szTarget));
DDX_Check (pDX, IDC_TRIGGER_AUTOADDWALL, m_bAutoAddWall);
char szLabel [40];
if (m_nType == TT_SPEEDBOOST) {
	DDX_Slider (pDX, IDC_TRIGGER_SLIDER, m_nSliderValue);
	sprintf_s (szLabel, sizeof (szLabel), "boost: %d%c", m_nSliderValue * 10, '%');
	}
else if ((m_nType == TT_TELEPORT) || (m_nType == TT_SPAWN_BOT)) {
	DDX_Slider (pDX, IDC_TRIGGER_SLIDER, m_nSliderValue);
	sprintf_s (szLabel, sizeof (szLabel), "damage: %d%c", m_nSliderValue * 10, '%');
	}
else
	strcpy_s (szLabel, sizeof (szLabel), "n/a");
SetDlgItemText (IDC_TRIGGER_SLIDER_TEXT, szLabel);
DDX_Radio (pDX, IDC_TRIGGER_STANDARD, m_nClass);
if (m_nType == TT_MESSAGE) {
	strcpy_s (szLabel, sizeof (szLabel), "msg #");
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT, szLabel);
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT2, "");
	}
else if (m_nType == TT_EXIT) {
	strcpy_s (szLabel, sizeof (szLabel), "dest:");
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT, szLabel);
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT2, "");
	}
else if (m_nType == TT_SOUND) {
	strcpy_s (szLabel, sizeof (szLabel), "sound #");
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT, szLabel);
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT2, "");
	}
else if (m_nType == TT_MASTER) {
	strcpy_s (szLabel, sizeof (szLabel), "semaphore:");
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT, szLabel);
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT2, "");
	}
else {
	strcpy_s (szLabel, sizeof (szLabel), "strength:");
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT, szLabel);
	SetDlgItemText (IDC_TRIGGER_STRENGTH_TEXT2, "%");
	}
}

								/*--------------------------*/

BOOL CTriggerTool::OnSetActive ()
{
Refresh ();
return TRUE; //CTexToolDlg::OnSetActive ();
}

								/*--------------------------*/

BOOL CTriggerTool::OnKillActive ()
{
Refresh ();
return CTexToolDlg::OnKillActive ();
}

								/*--------------------------*/

void CTriggerTool::EnableControls (BOOL bEnable)
{
CToolDlg::EnableControls (IDC_TRIGGER_STANDARD, IDC_TRIGGER_OBJECT, theMine->LevelVersion () >= 12);
CToolDlg::EnableControls (IDC_TRIGGER_SHOW_OBJ, IDC_TRIGGER_SHOW_OBJ, theMine->LevelVersion () >= 12);
CToolDlg::EnableControls (IDC_TRIGGER_TRIGGERNO + 1, IDC_TRIGGER_ADD_CONTROLPANEL, bEnable);
CToolDlg::EnableControls (IDC_TRIGGER_SLIDER, IDC_TRIGGER_SLIDER, bEnable && TriggerHasSlider ());
CToolDlg::EnableControls (IDC_TRIGGER_STRENGTH, IDC_TRIGGER_STRENGTH, bEnable && (m_nType != TT_SPEEDBOOST) && (m_nType != TT_CHANGE_TEXTURE));
CToolDlg::EnableControls (IDC_TRIGGER_SHOW_TEXTURE, IDC_TRIGGER_TEXTURE2, bEnable && (m_nType == TT_CHANGE_TEXTURE));
//int i;
//for (i = IDC_TRIGGER_TRIGGER_NO; i <= IDC_TRIGGER_PASTE; i++)
//	GetDlgItem (i)->EnableWindow (bEnable);
}

								/*--------------------------*/

int CTriggerTool::NumTriggers ()
{
return m_nClass ? triggerManager.Count (m_nClass);
}

								/*--------------------------*/

void CTriggerTool::InitCBTriggerNo ()
{
CComboBox *pcb = CBTriggerNo ();
pcb->ResetContent ();
int i, j = NumTriggers ();
for (i = 0; i < j; i++) {
	_itoa_s (i, message, sizeof (message), 10);
	pcb->AddString (message);
	}
pcb->SetCurSel (m_nTrigger);
}

								/*--------------------------*/

void CTriggerTool::InitLBTargets ()
{
CListBox *plb = LBTargets ();
m_iTarget = plb->GetCurSel ();
plb->ResetContent ();
if (m_triggerP) {
	m_targets = m_triggerP->m_count;
	int i;
	for (i = 0; i < m_targets ; i++) {
		sprintf_s (m_szTarget, sizeof (m_szTarget), "   %d, %d", m_triggerP->Segment (i), m_triggerP->Side (i) + 1);
		plb->AddString (m_szTarget);
		}
	if ((m_iTarget < 0) || (m_iTarget >= m_targets))
		m_iTarget = 0;
	*m_szTarget = '\0';
	}
else
	m_targets =
	m_iTarget = 0;
plb->SetCurSel (m_iTarget);
}

								/*--------------------------*/

void CTriggerTool::SetTriggerPtr (void)
{
if (m_nTrigger == -1) {
	m_triggerP = null;
	m_nStdTrigger = 
	m_nObjTrigger = -1;
	ClearObjWindow ();
	}	
else if (m_nClass) {
	m_triggerP = theMine->ObjTriggers (m_nTrigger);
	DrawObjectImage ();
	}
else {
	m_triggerP = triggerManager.Trigger (m_nTrigger);
	ClearObjWindow ();
	}
}

//------------------------------------------------------------------------

void CTriggerTool::ClearObjWindow (void)
{
CDC *pDC = m_showObjWnd.GetDC ();
if (pDC) {
	CRect rc;
	m_showObjWnd.GetClientRect (rc);
	pDC->FillSolidRect (&rc, IMG_BKCOLOR);
	m_showObjWnd.ReleaseDC (pDC);
	}
}

//------------------------------------------------------------------------

void CTriggerTool::DrawObjectImage ()
{
if (m_nClass) {
	CGameObject *objP = current.Object ();
	if ((objP->Type () == OBJ_ROBOT) || (objP->Type () == OBJ_CAMBOT) || (objP->Type () == OBJ_MONSTERBALL) || (objP->Type () == OBJ_SMOKE))
		theMine->DrawObject (&m_showObjWnd, objP->Type (), objP->Id ());
	}
}

//------------------------------------------------------------------------

void CTriggerTool::OnPaint ()
{
CToolDlg::OnPaint ();
DrawObjectImage ();
}

//------------------------------------------------------------------------

bool CTriggerTool::FindTrigger (short &nTrigger)
{
if (!m_bFindTrigger)
	nTrigger = m_nTrigger;
else {
	if (m_nClass) {
		if (current.m_nObject == theMine->ObjTriggers (m_nTrigger)->m_info.nObject)
			return false;
		for (int i = 0, j = theMine->ObjTriggerCount (); j; j--, i++) {
			if (current.m_nObject == theMine->ObjTriggers (i)->m_info.nObject) {
				m_nTrigger = i;
				return false;
				}
			}
		m_nTrigger = -1;
		return false;
		}
	else {
		// use current side's trigger
		m_nTrigger = (nTrigger == NO_TRIGGER) ? -1 : nTrigger;
		// if found, proceed
		if ((m_nTrigger == -1) || (wallManager.FindByTrigger (m_nTrigger) == null))
			return false;
		}
	}
return true;
}

//------------------------------------------------------------------------
// CTriggerTool - RefreshData
//------------------------------------------------------------------------

void CTriggerTool::Refresh ()
{
if (!(m_bInited && theMine))
	return;

	int			i;
	short			nTrigger;
	CComboBox	*cbTexture1 = CBTexture1 ();
	CComboBox	*cbTexture2 = CBTexture2 ();
	CSide		*sideP;

FindTrigger (nTrigger);
InitCBTriggerNo ();
if (m_nClass || (m_nTrigger == -1)) {
	SetTriggerPtr ();
	EnableControls (FALSE);
	CToolDlg::EnableControls (IDC_TRIGGER_STANDARD, IDC_TRIGGER_OBJECT, TRUE);
	CToolDlg::EnableControls (IDC_TRIGGER_ADD_OPENDOOR, IDC_TRIGGER_ADD_CONTROLPANEL, TRUE);
	if (DLE.IsD2File ())
		CToolDlg::EnableControls (IDC_TRIGGER_ADD_SHIELDDRAIN, IDC_TRIGGER_ADD_ENERGYDRAIN, FALSE);
	GetDlgItem (IDC_TRIGGER_ADD)->EnableWindow (TRUE);
	GetDlgItem (IDC_TRIGGER_D2TYPE)->EnableWindow (TRUE);
	InitLBTargets ();
	ClearObjWindow ();
	}
if (m_nTrigger != -1) {
	SetTriggerPtr ();
	m_nType = m_triggerP->m_info.type;
	if (m_nType != TT_CHANGE_TEXTURE) {
		cbTexture1->SetCurSel (cbTexture1->SelectString (-1, "(none)"));  // unselect if string not found
		cbTexture2->SetCurSel (cbTexture2->SelectString (-1, "(none)"));  // unselect if string not found
		}
	else {
		strcpy_s (message, sizeof (message), textureManager.Name (Texture1 ()));
		cbTexture1->SetCurSel (cbTexture1->SelectString (-1, message));  // unselect if string not found
		if (Texture2 ()) {
			strcpy_s (message, sizeof (message), textureManager.Name (Texture2 ()));
			cbTexture2->SetCurSel (cbTexture2->SelectString (-1, message));  // unselect if string not found
			}
		else
			cbTexture2->SetCurSel (cbTexture2->SelectString (-1, "(none)"));  // unselect if string not found
		}

	EnableControls (TRUE);
	if (!m_nClass)
		GetDlgItem (IDC_TRIGGER_ADD)->EnableWindow (FALSE);
	m_nTime = m_triggerP->m_info.time;
	m_targets = m_triggerP->m_count;
	InitLBTargets ();
	//TriggerCubeSideList ();
	// if D2 file, use trigger.type
	if (DLE.IsD2File ()) {
		SelectItemData (CBType (), m_nType);
		CToolDlg::EnableControls (IDC_TRIGGER_CONTROLDOORS, IDC_TRIGGER_CLOSEWALL, FALSE);
		CToolDlg::EnableControls (IDC_TRIGGER_ADD_SHIELDDRAIN, IDC_TRIGGER_ADD_ENERGYDRAIN, FALSE);
		m_bD2Flags [0] = ((m_triggerP->m_info.flags & TF_NO_MESSAGE) != 0);
		m_bD2Flags [1] = ((m_triggerP->m_info.flags & TF_ONE_SHOT) != 0);
		m_bD2Flags [2] = 0;
		m_bD2Flags [3] = ((m_triggerP->m_info.flags & TF_PERMANENT) != 0);
		m_bD2Flags [4] = ((m_triggerP->m_info.flags & TF_ALTERNATE) != 0);
		m_bD2Flags [5] = ((m_triggerP->m_info.flags & TF_SET_ORIENT) != 0);
		m_bD2Flags [6] = ((m_triggerP->m_info.flags & TF_SILENT) != 0);
		m_bD2Flags [7] = ((m_triggerP->m_info.flags & TF_AUTOPLAY) != 0);
		if (m_nType == TT_SPEEDBOOST)
			m_nSliderValue = m_triggerP->m_info.value;
		if (m_nType == TT_TELEPORT)
			m_nSliderValue = m_triggerP->m_info.value;
		if (m_nType == TT_SPAWN_BOT)
			m_nSliderValue = m_triggerP->m_info.value;
		else if (m_nType != TT_CHANGE_TEXTURE)
			m_nStrength = (double) m_triggerP->m_info.value / F1_0;
		}
	else {
		CBType ()->EnableWindow (FALSE);
		CToolDlg::EnableControls (IDC_TRIGGER_NOMESSAGE, IDC_TRIGGER_ONESHOT, FALSE);
		for (i = 0; i < MAX_TRIGGER_FLAGS; i++)
			m_bD1Flags [i] = ((m_triggerP->m_info.flags & triggerFlagsD1 [i]) != 0);
		m_nStrength = (double) m_triggerP->m_info.value / F1_0;
		}
	OnSetTarget ();
	}
CToolDlg::EnableControls (IDC_TRIGGER_TRIGGERNO, IDC_TRIGGER_TRIGGERNO, NumTriggers () > 0);
CToolDlg::EnableControls (IDC_TRIGGER_DELETEALL, IDC_TRIGGER_DELETEALL, NumTriggers () > 0);
sideP = other.Side ();
CTexToolDlg::Refresh (sideP->m_info.nBaseTex, sideP->m_info.nOvlTex, 1);
if ((m_nTrigger >= 0) && (m_nType == TT_CHANGE_TEXTURE))
	PaintTexture (&m_showTexWnd, RGB (128,128,128), -1, -1, Texture1 (), Texture2 ());
else
	PaintTexture (&m_showTexWnd, RGB (128,128,128), -1, -1, MAX_TEXTURES);
UpdateData (FALSE);
}

//------------------------------------------------------------------------

void CTriggerTool::OnStandardTrigger (void)
{
m_nObjTrigger = m_nTrigger;
m_nClass = 0;
UpdateData (FALSE);
m_triggerP = m_pStdTrigger;
m_nTrigger = m_nStdTrigger;
Refresh ();
}

//------------------------------------------------------------------------

void CTriggerTool::OnObjectTrigger (void)
{
m_nStdTrigger = m_nTrigger;
m_pStdTrigger = m_triggerP;
m_nClass = 1;
UpdateData (FALSE);
m_triggerP = theMine->ObjTriggers (m_nTrigger);
m_nTrigger = m_nObjTrigger;
Refresh ();
}

//------------------------------------------------------------------------
// CTriggerTool - Add Trigger
//------------------------------------------------------------------------

void CTriggerTool::OnAddTrigger ()
{
//m_nTrigger = nTrigger;
m_bAutoAddWall = ((CButton *) GetDlgItem (IDC_TRIGGER_AUTOADDWALL))->GetCheck ();
if (m_nClass) {
	m_triggerP = theMine->AddObjTrigger (-1, m_nType);
	m_nTrigger = m_triggerP ? int (m_triggerP - theMine->ObjTriggers (0)) : -1; 
	}
else {
	m_triggerP = theMine->AddTrigger (-1, m_nType, (BOOL) m_bAutoAddWall /*TT_OPEN_DOOR*/);
	m_nTrigger = m_triggerP ? int (m_triggerP - triggerManager.Trigger (0)) : -1;
	}
// Redraw trigger window
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CTriggerTool - Delete Trigger
//------------------------------------------------------------------------

void CTriggerTool::OnDeleteTrigger () 
{
// check to see if trigger already exists on wall
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nClass)
	theMine->DeleteObjTrigger (m_nTrigger);
else
	triggerManager.Delete (m_nTrigger);
// Redraw trigger window
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CTriggerTool - Delete All (Marked) Triggers
//------------------------------------------------------------------------

void CTriggerTool::OnDeleteTriggerAll () 
{
undoManager.Begin (udTriggers);
DLE.MineView ()->DelayRefresh (true);
CSegment *segP = segmentManager.Segment (0);
CSide *sideP;
bool bAll = (theMine->MarkedSegmentCount (true) == 0);
int i, j, nDeleted = 0;
for (i = segmentManager.Count (); i; i--, segP++) {
	sideP = segP->m_sides;
	for (j = 0; j < MAX_SIDES_PER_SEGMENT; j++, sideP++) {
		if (sideP->m_info.nWall >= MAX_WALLS)
			continue;
		CWall *wallP = wallManager.Wall (sideP->m_info.nWall);
		if (wallP->Info ().nTrigger >= NumTriggers ())
			continue;
		if (bAll || segmentManager.IsMarked (CSideKey (i, j))) {
			triggerManager.Delete (wallP->Info ().nTrigger);
			nDeleted++;
			}
		}
	}
DLE.MineView ()->DelayRefresh (false);
undoManager.End ();
if (nDeleted) {
	DLE.MineView ()->Refresh ();
	Refresh ();
	}
}

//------------------------------------------------------------------------
// CTriggerTool - TrigNumberMsg
//------------------------------------------------------------------------

void CTriggerTool::OnSetTrigger ()
{
ushort nWall;
CWall *wallP;

// find first wall with this trigger
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if ((m_nTrigger == -1) || (m_nTrigger >= NumTriggers ()))
	return;
if (m_nClass) {
	current.m_nObject = theMine->ObjTriggers (m_nTrigger)->m_info.nObject;
	}
else {
	for (nWall = 0, wallP = wallManager.Wall (0); nWall < wallManager.WallCount; nWall++, wallP++)
		if (wallP->Info ().nTrigger == m_nTrigger)
			break;
	if (nWall >= wallManager.WallCount) {
		EnableControls (FALSE);
		GetDlgItem (IDC_TRIGGER_DELETE)->EnableWindow (TRUE);
		return;
		}
	if ((wallP->m_nSegment >= segmentManager.Count ()) || (wallP->m_nSegment < 0) || 
		 (wallP->m_nSide < 0) || (wallP->m_nSide > 5)) {
		EnableControls (FALSE);
		GetDlgItem (IDC_TRIGGER_DELETE)->EnableWindow (TRUE);
		return;
		}
	if ((current.m_nSegment != wallP->m_nSegment) ||
		 (current.m_nSide != wallP->m_nSide)) {
		theMine->SetCurrent (wallP->m_nSegment, wallP->m_nSide);
		}
	}
SetTriggerPtr ();
m_bFindTrigger = false;
Refresh ();
m_bFindTrigger = true;
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CTriggerTool - TrigTypeMsg
//------------------------------------------------------------------------

void CTriggerTool::OnSetType ()
{
int nType = int (CBType ()->GetItemData (CBType ()->GetCurSel ()));
if ((nType == TT_SMOKE_BRIGHTNESS) || ((nType >= TT_SMOKE_LIFE) && (nType <= TT_SMOKE_DRIFT))) {
	ErrorMsg ("This trigger type is not supported any more.\nYou can use the effects tool to edit smoke emitters.");
	return;
	}
if ((nType >= TT_TELEPORT) && (theMine->IsStdLevel ())) {
	SelectItemData (CBType (), m_nType);
	return;
	}
m_nType = nType;
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
undoManager.Begin (udTriggers);
m_triggerP->m_info.type = m_nType;
undoManager.End ();
Refresh ();
}

//------------------------------------------------------------------------
// CTriggerTool - TrigValueMsg
//------------------------------------------------------------------------

void CTriggerTool::OnStrength () 
{
UpdateData (TRUE);
if ((m_nTrigger == -1) || (m_nType == TT_SPEEDBOOST) || (m_nType == TT_CHANGE_TEXTURE))
	return;
SetTriggerPtr ();
UpdateData (FALSE);
undoManager.Begin (udTriggers);
m_triggerP->m_info.value = (int) (m_nStrength * F1_0);
undoManager.End ();
}

//------------------------------------------------------------------------
// CTriggerTool - TrigTimeMsg
//------------------------------------------------------------------------

void CTriggerTool::OnTime () 
{
UpdateData (TRUE);
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
undoManager.Begin (udTriggers);
m_triggerP->m_info.time = m_nTime;
undoManager.End ();
}

//------------------------------------------------------------------------
// CTriggerTool - TriggerFlags0Msg
//------------------------------------------------------------------------

bool CTriggerTool::OnD1Flag (int i, int j)
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return false;
SetTriggerPtr ();
undoManager.Begin (udTriggers);
if ((m_bD1Flags [i] = !m_bD1Flags [i]))
//if ((m_bD1Flags [i] = ((CButton *) GetDlgItem (IDC_TRIGGER_CONTROLDOORS + j))->GetCheck ()))
	m_triggerP->m_info.flags |= triggerFlagsD1 [i];
else
	m_triggerP->m_info.flags &= ~triggerFlagsD1 [i];
((CButton *) GetDlgItem (IDC_TRIGGER_CONTROLDOORS + i))->SetCheck (m_bD1Flags [i]);
if (m_bD1Flags [i] && (j >= 0)) {
	m_bD1Flags [j] = 0;
	m_triggerP->m_info.flags &= ~triggerFlagsD1 [j];
	((CButton *) GetDlgItem (IDC_TRIGGER_CONTROLDOORS + j))->SetCheck (0);
	}
UpdateData (FALSE);
undoManager.End ();
return m_bD1Flags [i] != 0;
}

                        /*--------------------------*/

void CTriggerTool::OnD2Flag (int i, int j)
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
undoManager.Begin (udTriggers);
j = d2FlagXlat [i];
int h = 1 << j;
m_triggerP->m_info.flags ^= h;
m_bD2Flags [j] = ((m_triggerP->m_info.flags & h) != 0);
((CButton *) GetDlgItem (IDC_TRIGGER_NOMESSAGE + i))->SetCheck (m_bD2Flags [j]);
UpdateData (FALSE);
undoManager.End ();
}

//------------------------------------------------------------------------
// CTriggerTool - TriggerFlags1Msg
//------------------------------------------------------------------------

void CTriggerTool::OnD1Flag1 () { OnD1Flag (0); }
void CTriggerTool::OnD1Flag2 () { OnD1Flag (1); }
void CTriggerTool::OnD1Flag3 () { OnD1Flag (2); }
void CTriggerTool::OnD1Flag4 () { OnD1Flag (3, 4); }
void CTriggerTool::OnD1Flag5 () { OnD1Flag (4, 3); }
void CTriggerTool::OnD1Flag6 () { OnD1Flag (5); }
void CTriggerTool::OnD1Flag7 () { OnD1Flag (6); }
void CTriggerTool::OnD1Flag8 () { OnD1Flag (7); }
void CTriggerTool::OnD1Flag9 () { OnD1Flag (8, 9); }
void CTriggerTool::OnD1Flag10 () { OnD1Flag (9, 8); }
void CTriggerTool::OnD1Flag11 () { OnD1Flag (10, 11); }
void CTriggerTool::OnD1Flag12 () { OnD1Flag (11, 10); }

void CTriggerTool::OnD2Flag1 () { OnD2Flag (0); }
void CTriggerTool::OnD2Flag2 () { OnD2Flag (1); }
// caution: 4 is TF_DISABLED in Descent 2 - do not use here!
void CTriggerTool::OnD2Flag3 () { OnD2Flag (2); }
void CTriggerTool::OnD2Flag4 () { OnD2Flag (3); }
void CTriggerTool::OnD2Flag5 () { OnD2Flag (4); }
void CTriggerTool::OnD2Flag6 () { OnD2Flag (5); }
void CTriggerTool::OnD2Flag7 () { OnD2Flag (6); }

//------------------------------------------------------------------------
// CTriggerTool - Add cube/side to trigger list
//------------------------------------------------------------------------

void CTriggerTool::AddTarget (short nSegment, short nSide) 
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
m_targets = m_triggerP->m_count;
if (m_targets >= MAX_TRIGGER_TARGETS) {
	DEBUGMSG (" Trigger tool: No more targets possible for this trigger.");
	return;
	}
if (FindTarget (nSegment, nSide) > -1) {
	DEBUGMSG (" Trigger tool: Trigger already has this target.");
	return;
	}
undoManager.Begin (udTriggers);
m_triggerP->Add (nSegment, nSide - 1);
undoManager.End ();
sprintf_s (m_szTarget, sizeof (m_szTarget), "   %d,%d", nSegment, nSide);
LBTargets ()->AddString (m_szTarget);
LBTargets ()->SetCurSel (m_targets++);
*m_szTarget = '\0';
Refresh ();
}


                        /*--------------------------*/

void CTriggerTool::OnAddTarget () 
{
int nSegment, nSide;
UpdateData (TRUE);
sscanf_s (m_szTarget, "%d, %d", &nSegment, &nSide);
if (nSegment < 0)
	return;
if (nSide < 0)
	nSide = 0;
else if (nSide > 6)
	nSide = 6;
if (nSegment > ((nSide == 0) ? objectManager.Count () : segmentManager.Count ()))
	return;
AddTarget (nSegment, nSide);
}

                        /*--------------------------*/

void CTriggerTool::OnAddWallTarget ()
{
CSelection *other = (current == selections [0]) ? &theMine->Current2 () : selections [0];
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
if ((DLE.IsD1File ()) 
	 ? (m_triggerP->m_info.flags & TRIGGER_MATCEN) != 0 
	 : (m_triggerP->m_info.type == TT_MATCEN) && (segmentManager.Segment (other->nSegment)->m_info.function != SEGMENT_FUNC_ROBOTMAKER)
	) {
	DEBUGMSG (" Trigger tool: Target is no robot maker");
	return;
	}
int i = FindTarget (other->nSegment, other->nSide);
if (i > -1)
	return;
AddTarget (other->nSegment, other->nSide + 1);
}

                        /*--------------------------*/

void CTriggerTool::OnAddObjTarget ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
AddTarget (current.m_nObject, 0);
}

//------------------------------------------------------------------------
// CTriggerTool - Delete cube/side
//------------------------------------------------------------------------

void CTriggerTool::OnDeleteTarget ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
m_iTarget = LBTargets ()->GetCurSel ();
if ((m_iTarget < 0) || (m_iTarget >= MAX_TRIGGER_TARGETS))
	return;
SetTriggerPtr ();
undoManager.Begin (udTriggers);
m_targets = m_triggerP->Delete (m_iTarget);
undoManager.End ();
LBTargets ()->DeleteString (m_iTarget);
if (m_iTarget >= LBTargets ()->GetCount ())
	m_iTarget--;
LBTargets ()->SetCurSel (m_iTarget);
Refresh ();
}

                        /*--------------------------*/

int CTriggerTool::FindTarget (short nSegment, short nSide)
{
return m_triggerP->Find (nSegment, nSide);
}

//------------------------------------------------------------------------
// CTriggerTool - Cube/Side list box message
//
// sets "other cube" to selected item
//------------------------------------------------------------------------

void CTriggerTool::OnSetTarget () 
{
short nTrigger;
if (!FindTrigger (nTrigger))
	return;
SetTriggerPtr ();
// get affected cube/side list box index
m_iTarget = LBTargets ()->GetCurSel ();
// if selected and within range, then set "other" cube/side
if ((m_iTarget < 0) || (m_iTarget >= MAX_TRIGGER_TARGETS) || (m_iTarget >= m_triggerP->m_count))
	return;
short nSegment = m_triggerP->Segment (m_iTarget);
if ((nSegment < 0) || (nSegment >= segmentManager.Count ()))
	 return;
short nSide = m_triggerP->Side (m_iTarget);
if ((nSide < 0) || (nSide > 5))
	return;

CSelection *other = theMine->Other ();
if ((current.m_nSegment == nSegment) && (current.m_nSide == nSide))
	return;
other->nSegment = m_triggerP->Segment (m_iTarget);
other->nSide = m_triggerP->Side (m_iTarget);
DLE.MineView ()->Refresh ();
}

                        /*--------------------------*/

void CTriggerTool::OnCopyTrigger ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
m_defTrigger = triggerManager.Trigger () [m_nTrigger];
}

                        /*--------------------------*/

void CTriggerTool::OnPasteTrigger ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
undoManager.Begin (udTriggers);
*triggerManager.Trigger (m_nTrigger) = m_defTrigger;
undoManager.End ();
Refresh ();
DLE.MineView ()->Refresh ();
}

                        /*--------------------------*/

afx_msg void CTriggerTool::OnAddOpenDoor ()
{
theMine->AddOpenDoorTrigger ();
}

                        /*--------------------------*/

afx_msg void CTriggerTool::OnAddRobotMaker ()
{
theMine->AddRobotMakerTrigger ();
}

                        /*--------------------------*/

afx_msg void CTriggerTool::OnAddShieldDrain ()
{
theMine->AddShieldTrigger ();
}

                        /*--------------------------*/

afx_msg void CTriggerTool::OnAddEnergyDrain ()
{
theMine->AddEnergyTrigger ();
}

                        /*--------------------------*/

afx_msg void CTriggerTool::OnAddControlPanel ()
{
theMine->AddUnlockTrigger ();
}

                        /*--------------------------*/

BOOL CTriggerTool::TextureIsVisible ()
{
return !m_nClass && (m_triggerP != null) && (m_iTarget >= 0) && (m_iTarget < m_triggerP->m_count);
}

                        /*--------------------------*/

void CTriggerTool::SelectTexture (int nIdC, bool bFirst)
{
	CSide		*sideP = current.Side ();
	CComboBox	*pcb = bFirst ? CBTexture1 () : CBTexture2 ();
	int			index = pcb->GetCurSel ();
	
if (index <= 0)
	SetTexture (0, 0);
else {
	short texture = (short) pcb->GetItemData (index);
	if (bFirst)
		SetTexture (texture, -1);
	else
		SetTexture (-1, texture);
	}
Refresh ();
}

                        /*--------------------------*/

void CTriggerTool::OnSelect1st () 
{
SelectTexture (IDC_TRIGGER_TEXTURE1, true);
}

                        /*--------------------------*/

void CTriggerTool::OnSelect2nd () 
{
SelectTexture (IDC_TRIGGER_TEXTURE2, false);
}

                        /*--------------------------*/

void CTriggerTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
	int	nPos = pScrollBar->GetScrollPos ();
	CRect	rc;

if (!m_triggerP || !TriggerHasSlider ())
	return;
if (pScrollBar == SpeedBoostSlider ()) {
	switch (scrollCode) {
		case SB_LINEUP:
			nPos--;
			break;
		case SB_LINEDOWN:
			nPos++;
			break;
		case SB_PAGEUP:
			nPos -= 1;
			break;
		case SB_PAGEDOWN:
			nPos += 1;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	if (nPos < 1)
		nPos = 1;
	else if (nPos > 10)
		nPos = 10;
	m_nSliderValue = m_triggerP->m_info.value = nPos;
	UpdateData (FALSE);
#if 0
	pScrollBar->SetScrollPos (nPos, TRUE);
	if (!(nPos = pScrollBar->GetScrollPos ())) {
		int	h, i, j;
		pScrollBar->GetScrollRange (&i, &j);
		h = i;
		h = j;
		}
#endif
	}
}

                        /*--------------------------*/


//eof triggerdlg.cpp
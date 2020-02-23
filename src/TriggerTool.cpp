
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

//------------------------------------------------------------------------------

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
	: CTexToolDlg (IDD_TRIGGERDATA_HORZ + nLayout, pParent, IDC_TRIGGER_SHOW, 6, RGB (0,0,0), true)
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

//------------------------------------------------------------------------------

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
	char* p = textureManager.Name (-1, iTexture);
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

//------------------------------------------------------------------------------

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
	{"disable", TT_DISABLE_TRIGGER},
	{"disarm robot", TT_DISARM_ROBOT},
	{"reprogram robot", TT_REPROGRAM_ROBOT},
	{"shake mine", TT_SHAKE_MINE}
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
CToolDlg::EnableControls (IDC_TRIGGER_STANDARD, IDC_TRIGGER_OBJECT, DLE.LevelVersion () >= 12);
CToolDlg::EnableControls (IDC_TRIGGER_SHOW_OBJ, IDC_TRIGGER_SHOW_OBJ, DLE.LevelVersion () >= 12);
CToolDlg::EnableControls (IDC_TRIGGER_TRIGGERNO + 1, IDC_TRIGGER_ADD_CONTROLPANEL, bEnable);
CToolDlg::EnableControls (IDC_TRIGGER_SLIDER, IDC_TRIGGER_SLIDER, bEnable && TriggerHasSlider ());
CToolDlg::EnableControls (IDC_TRIGGER_STRENGTH, IDC_TRIGGER_STRENGTH, bEnable && (m_nType != TT_SPEEDBOOST) && (m_nType != TT_CHANGE_TEXTURE));
CToolDlg::EnableControls (IDC_TRIGGER_SHOW_TEXTURE, IDC_TRIGGER_TEXTURE2, bEnable && (m_nType == TT_CHANGE_TEXTURE));
//int i;
//for (i = IDC_TRIGGER_TRIGGER_NO; i <= IDC_TRIGGER_PASTE; i++)
//	GetDlgItem (i)->EnableWindow (bEnable);
}

								/*--------------------------*/

int CTriggerTool::TriggerCount ()
{
return triggerManager.Count (m_nClass);
}

								/*--------------------------*/

void CTriggerTool::InitCBTriggerNo ()
{
CComboBox *pcb = CBTriggerNo ();
pcb->ResetContent ();
int i, j = TriggerCount ();
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
if (m_pTrigger) {
	m_targets = m_pTrigger->Count ();
	int i;
	for (i = 0; i < m_targets ; i++) {
		sprintf_s (m_szTarget, sizeof (m_szTarget), "   %d, %d", m_pTrigger->Segment (i), m_pTrigger->Side (i) + 1);
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
	m_pTrigger = null;
	m_nStdTrigger = 
	m_nObjTrigger = -1;
	ClearObjWindow ();
	}	
else if (m_nClass) {
	m_pTrigger = triggerManager.ObjTrigger (m_nTrigger);
	DrawObjectImage ();
	}
else {
	m_pTrigger = triggerManager.Trigger (m_nTrigger);
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
	CGameObject *pObject = current->Object ();
	if ((pObject->Type () == OBJ_ROBOT) || (pObject->Type () == OBJ_CAMBOT) || (pObject->Type () == OBJ_MONSTERBALL) || (pObject->Type () == OBJ_SMOKE))
		pObject->DrawBitmap (&m_showObjWnd);
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
		if (current->ObjectId () == triggerManager.ObjTrigger (m_nTrigger)->Info ().nObject)
			return false;
		for (int i = 0, j = triggerManager.ObjTriggerCount (); j; j--, i++) {
			if (current->ObjectId () == triggerManager.ObjTrigger (i)->Info ().nObject) {
				m_nTrigger = i;
				return false;
				}
			}
		m_nTrigger = -1;
		return false;
		}
	else {
		m_nTrigger = (current->Trigger () == null) ? -1 : triggerManager.Index (current->Trigger ());
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
	short			nTrigger = -1;
	CComboBox	*cbTexture1 = CBTexture1 ();
	CComboBox	*cbTexture2 = CBTexture2 ();
	CSide		*pSide;

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
	m_nType = m_pTrigger->Type ();
	if (m_nType != TT_CHANGE_TEXTURE) {
		cbTexture1->SetCurSel (cbTexture1->SelectString (-1, "(none)"));  // unselect if string not found
		cbTexture2->SetCurSel (cbTexture2->SelectString (-1, "(none)"));  // unselect if string not found
		}
	else {
		strcpy_s (message, sizeof (message), textureManager.Name (-1, Texture1 ()));
		cbTexture1->SetCurSel (cbTexture1->SelectString (-1, message));  // unselect if string not found
		if (Texture2 ()) {
			strcpy_s (message, sizeof (message), textureManager.Name (-1, Texture2 ()));
			cbTexture2->SetCurSel (cbTexture2->SelectString (-1, message));  // unselect if string not found
			}
		else
			cbTexture2->SetCurSel (cbTexture2->SelectString (-1, "(none)"));  // unselect if string not found
		}

	EnableControls (TRUE);
	if (!m_nClass)
		GetDlgItem (IDC_TRIGGER_ADD)->EnableWindow (FALSE);
	m_nTime = m_pTrigger->Info ().time;
	m_targets = m_pTrigger->Count ();
	InitLBTargets ();
	//TriggerCubeSideList ();
	// if D2 file, use trigger.type
	if (DLE.IsD2File ()) {
		SelectItemData (CBType (), m_nType);
		CToolDlg::EnableControls (IDC_TRIGGER_CONTROLDOORS, IDC_TRIGGER_CLOSEWALL, FALSE);
		CToolDlg::EnableControls (IDC_TRIGGER_ADD_SHIELDDRAIN, IDC_TRIGGER_ADD_ENERGYDRAIN, FALSE);
		m_bD2Flags [0] = ((m_pTrigger->Info ().flags & TF_NO_MESSAGE) != 0);
		m_bD2Flags [1] = ((m_pTrigger->Info ().flags & TF_ONE_SHOT) != 0);
		m_bD2Flags [2] = 0;
		m_bD2Flags [3] = ((m_pTrigger->Info ().flags & TF_PERMANENT) != 0);
		m_bD2Flags [4] = ((m_pTrigger->Info ().flags & TF_ALTERNATE) != 0);
		m_bD2Flags [5] = ((m_pTrigger->Info ().flags & TF_SET_ORIENT) != 0);
		m_bD2Flags [6] = ((m_pTrigger->Info ().flags & TF_SILENT) != 0);
		m_bD2Flags [7] = ((m_pTrigger->Info ().flags & TF_AUTOPLAY) != 0);
		if (m_nType == TT_SPEEDBOOST)
			m_nSliderValue = m_pTrigger->Info ().value;
		if (m_nType == TT_TELEPORT)
			m_nSliderValue = m_pTrigger->Info ().value;
		if (m_nType == TT_SPAWN_BOT)
			m_nSliderValue = m_pTrigger->Info ().value;
		else if (m_nType != TT_CHANGE_TEXTURE)
			m_nStrength = (double) m_pTrigger->Info ().value / F1_0;
		}
	else {
		CBType ()->EnableWindow (FALSE);
		CToolDlg::EnableControls (IDC_TRIGGER_NOMESSAGE, IDC_TRIGGER_ONESHOT, FALSE);
		for (i = 0; i < MAX_TRIGGER_FLAGS; i++)
			m_bD1Flags [i] = ((m_pTrigger->Info ().flags & triggerFlagsD1 [i]) != 0);
		m_nStrength = (double) m_pTrigger->Info ().value / F1_0;
		}
	OnSetTarget ();
	}
CToolDlg::EnableControls (IDC_TRIGGER_TRIGGERNO, IDC_TRIGGER_TRIGGERNO, TriggerCount () > 0);
CToolDlg::EnableControls (IDC_TRIGGER_DELETEALL, IDC_TRIGGER_DELETEALL, TriggerCount () > 0);
pSide = other->Side ();
CTexToolDlg::Refresh (pSide->Info ().nBaseTex, pSide->Info ().nOvlTex, 1);
if ((m_nTrigger >= 0) && (m_nType == TT_CHANGE_TEXTURE))
	PaintTexture (&m_showTexWnd, RGB (128,128,128), Texture1 (), Texture2 ());
else
	PaintTexture (&m_showTexWnd, RGB (128,128,128), -1);
UpdateData (FALSE);
}

//------------------------------------------------------------------------

void CTriggerTool::OnStandardTrigger (void)
{
m_nObjTrigger = m_nTrigger;
m_nClass = 0;
UpdateData (FALSE);
m_pTrigger = m_pStdTrigger;
m_nTrigger = m_nStdTrigger;
Refresh ();
}

//------------------------------------------------------------------------

void CTriggerTool::OnObjectTrigger (void)
{
m_nStdTrigger = m_nTrigger;
m_pStdTrigger = m_pTrigger;
m_nClass = 1;
UpdateData (FALSE);
m_pTrigger = triggerManager.ObjTrigger (m_nTrigger);
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
	m_pTrigger = triggerManager.AddToObject (-1, m_nType);
	m_nTrigger = m_pTrigger ? int (m_pTrigger - triggerManager.ObjTrigger (0)) : -1; 
	}
else {
	m_pTrigger = triggerManager.AddToWall (-1, m_nType, m_bAutoAddWall != 0);
	m_nTrigger = m_pTrigger ? int (m_pTrigger - triggerManager.Trigger (0)) : -1;
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
	triggerManager.DeleteFromObject (m_nTrigger);
else
	triggerManager.DeleteFromWall (m_nTrigger);
// Redraw trigger window
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CTriggerTool - Delete All (Tagged) Triggers
//------------------------------------------------------------------------

void CTriggerTool::OnDeleteTriggerAll () 
{
undoManager.Begin (__FUNCTION__, udTriggers);
DLE.MineView ()->DelayRefresh (true);
CSegment *pSegment = segmentManager.Segment (0);
ubyte nType = current->Trigger () ? current->Trigger ()->Type () : 255;
bool bAll = !segmentManager.HasTaggedSegments ();
int i, j, nDeleted = 0;
for (i = segmentManager.Count (); i; i--, pSegment++) {
	CSide *pSide = pSegment->m_sides;
	for (j = 0; j < MAX_SIDES_PER_SEGMENT; j++, pSide++) {
		CWall *pWall = pSide->Wall ();
		if (pWall == null)
			continue;
		CTrigger *pTrigger = pWall->Trigger ();
		if (pTrigger == null)
			continue;
		if ((nType != 255) && (pTrigger->Type () != nType))
			continue;
		if (bAll || segmentManager.IsTagged (CSideKey (i, j))) {
			triggerManager.Delete (pWall->Info ().nTrigger);
			nDeleted++;
			}
		}
	}
DLE.MineView ()->DelayRefresh (false);
undoManager.End (__FUNCTION__);
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
CWall *pWall;

// find first wall with this trigger
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if ((m_nTrigger == -1) || (m_nTrigger >= TriggerCount ()))
	return;
if (m_nClass) {
	current->SetObjectId (triggerManager.ObjTrigger (m_nTrigger)->Info ().nObject);
	}
else {
	for (nWall = 0, pWall = wallManager.Wall (0); nWall < wallManager.WallCount (); nWall++, pWall++)
		if (pWall->Info ().nTrigger == m_nTrigger)
			break;
	if (nWall >= wallManager.WallCount ()) {
		EnableControls (FALSE);
		GetDlgItem (IDC_TRIGGER_DELETE)->EnableWindow (TRUE);
		return;
		}
	if ((pWall->m_nSegment >= segmentManager.Count ()) || (pWall->m_nSegment < 0) || 
		 (pWall->m_nSide < 0) || (pWall->m_nSide > 5)) {
		EnableControls (FALSE);
		GetDlgItem (IDC_TRIGGER_DELETE)->EnableWindow (TRUE);
		return;
		}
	if ((current->SegmentId () != pWall->m_nSegment) ||
		 (current->SideId () != pWall->m_nSide)) {
		*((CSideKey *) current) = *((CSideKey *) pWall);
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
if ((nType >= TT_TELEPORT) && (DLE.IsStdLevel ())) {
	SelectItemData (CBType (), m_nType);
	return;
	}
m_nType = nType;
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
undoManager.Begin (__FUNCTION__, udTriggers);
m_pTrigger->Type () = m_nType;
undoManager.End (__FUNCTION__);
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
undoManager.Begin (__FUNCTION__, udTriggers);
m_pTrigger->Info ().value = (int) (m_nStrength * F1_0);
undoManager.End (__FUNCTION__);
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
undoManager.Begin (__FUNCTION__, udTriggers);
m_pTrigger->Info ().time = m_nTime;
undoManager.End (__FUNCTION__);
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
undoManager.Begin (__FUNCTION__, udTriggers);
if ((m_bD1Flags [i] = !m_bD1Flags [i]))
//if ((m_bD1Flags [i] = ((CButton *) GetDlgItem (IDC_TRIGGER_CONTROLDOORS + j))->GetCheck ()))
	m_pTrigger->Info ().flags |= triggerFlagsD1 [i];
else
	m_pTrigger->Info ().flags &= ~triggerFlagsD1 [i];
((CButton *) GetDlgItem (IDC_TRIGGER_CONTROLDOORS + i))->SetCheck (m_bD1Flags [i]);
if (m_bD1Flags [i] && (j >= 0)) {
	m_bD1Flags [j] = 0;
	m_pTrigger->Info ().flags &= ~triggerFlagsD1 [j];
	((CButton *) GetDlgItem (IDC_TRIGGER_CONTROLDOORS + j))->SetCheck (0);
	}
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
return m_bD1Flags [i] != 0;
}

//------------------------------------------------------------------------------

void CTriggerTool::OnD2Flag (int i, int j)
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
undoManager.Begin (__FUNCTION__, udTriggers);
j = d2FlagXlat [i];
int h = 1 << j;
m_pTrigger->Info ().flags ^= h;
m_bD2Flags [j] = ((m_pTrigger->Info ().flags & h) != 0);
((CButton *) GetDlgItem (IDC_TRIGGER_NOMESSAGE + i))->SetCheck (m_bD2Flags [j]);
UpdateData (FALSE);
undoManager.End (__FUNCTION__);
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
// CTriggerTool - Add segment/side to trigger list
//------------------------------------------------------------------------

void CTriggerTool::AddTarget (short nSegment, short nSide) 
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
m_targets = m_pTrigger->Count ();
if (m_targets >= MAX_TRIGGER_TARGETS) {
	DEBUGMSG (" Trigger tool: No more targets possible for this trigger.");
	return;
	}
if (FindTarget (nSegment, nSide) > -1) {
	DEBUGMSG (" Trigger tool: Trigger already has this target.");
	return;
	}
undoManager.Begin (__FUNCTION__, udTriggers);
m_pTrigger->Add (nSegment, nSide - 1);
undoManager.End (__FUNCTION__);
sprintf_s (m_szTarget, sizeof (m_szTarget), "   %d,%d", nSegment, nSide);
LBTargets ()->AddString (m_szTarget);
LBTargets ()->SetCurSel (m_targets++);
*m_szTarget = '\0';
Refresh ();
}


//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

void CTriggerTool::OnAddWallTarget ()
{
other = &selections [!current->Index ()];
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
if ((DLE.IsD1File ()) 
	 ? (m_pTrigger->Info ().flags & TRIGGER_MATCEN) != 0 
	 : (m_pTrigger->Type () == TT_MATCEN) && (other->Segment ()->Info ().function != SEGMENT_FUNC_ROBOTMAKER)
	) {
	DEBUGMSG (" Trigger tool: Target is no robot maker");
	return;
	}
int i = FindTarget (other->m_nSegment, other->m_nSide);
if (i > -1)
	return;
AddTarget (other->m_nSegment, other->m_nSide + 1);
}

//------------------------------------------------------------------------------

void CTriggerTool::OnAddObjTarget ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
SetTriggerPtr ();
AddTarget (current->ObjectId (), 0);
}

//------------------------------------------------------------------------
// CTriggerTool - Delete segment/side
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
undoManager.Begin (__FUNCTION__, udTriggers);
m_targets = m_pTrigger->Delete (m_iTarget);
undoManager.End (__FUNCTION__);
LBTargets ()->DeleteString (m_iTarget);
if (m_iTarget >= LBTargets ()->GetCount ())
	m_iTarget--;
LBTargets ()->SetCurSel (m_iTarget);
Refresh ();
}

//------------------------------------------------------------------------------

int CTriggerTool::FindTarget (short nSegment, short nSide)
{
return m_pTrigger->Find (nSegment, nSide);
}

//------------------------------------------------------------------------
// CTriggerTool - Cube/Side list box message
//
// sets "other segment" to selected item
//------------------------------------------------------------------------

void CTriggerTool::OnSetTarget () 
{
short nTrigger = -1;
if (!FindTrigger (nTrigger))
	return;
SetTriggerPtr ();
// get affected segment/side list box index
m_iTarget = LBTargets ()->GetCurSel ();
// if selected and within range, then set "other" segment/side
if ((m_iTarget < 0) || (m_iTarget >= MAX_TRIGGER_TARGETS) || (m_iTarget >= m_pTrigger->Count ()))
	return;
short nSegment = m_pTrigger->Segment (m_iTarget);
if ((nSegment < 0) || (nSegment >= segmentManager.Count ()))
	 return;
short nSide = m_pTrigger->Side (m_iTarget);
if ((nSide < 0) || (nSide > 5))
	return;

if ((current->SegmentId () == nSegment) && (current->SideId () == nSide))
	return;
*((CSideKey *) other) = *(m_pTrigger->Target (m_iTarget));
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CTriggerTool::OnCopyTrigger ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
m_defTrigger = *triggerManager.Trigger (m_nTrigger);
}

//------------------------------------------------------------------------------

void CTriggerTool::OnPasteTrigger ()
{
m_nTrigger = CBTriggerNo ()->GetCurSel ();
if (m_nTrigger == -1)
	return;
undoManager.Begin (__FUNCTION__, udTriggers);
*triggerManager.Trigger (m_nTrigger) = m_defTrigger;
undoManager.End (__FUNCTION__);
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

afx_msg void CTriggerTool::OnAddOpenDoor ()
{
triggerManager.AddOpenDoor ();
}

//------------------------------------------------------------------------------

afx_msg void CTriggerTool::OnAddRobotMaker ()
{
triggerManager.AddRobotMaker ();
}

//------------------------------------------------------------------------------

afx_msg void CTriggerTool::OnAddShieldDrain ()
{
triggerManager.AddShieldDrain ();
}

//------------------------------------------------------------------------------

afx_msg void CTriggerTool::OnAddEnergyDrain ()
{
triggerManager.AddEnergyDrain ();
}

//------------------------------------------------------------------------------

afx_msg void CTriggerTool::OnAddControlPanel ()
{
triggerManager.AddUnlock ();
}

//------------------------------------------------------------------------------

BOOL CTriggerTool::TextureIsVisible ()
{
return !m_nClass && (m_pTrigger != null) && (m_iTarget >= 0) && (m_iTarget < m_pTrigger->Count ());
}

//------------------------------------------------------------------------------

void CTriggerTool::SelectTexture (int nIdC, bool bFirst)
{
	CSide		*pSide = current->Side ();
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

//------------------------------------------------------------------------------

void CTriggerTool::OnSelect1st () 
{
SelectTexture (IDC_TRIGGER_TEXTURE1, true);
}

//------------------------------------------------------------------------------

void CTriggerTool::OnSelect2nd () 
{
SelectTexture (IDC_TRIGGER_TEXTURE2, false);
}

//------------------------------------------------------------------------------

void CTriggerTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
	int	nPos = pScrollBar->GetScrollPos ();
	CRect	rc;

if (!m_pTrigger || !TriggerHasSlider ())
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
	m_nSliderValue = m_pTrigger->Info ().value = nPos;
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

//------------------------------------------------------------------------------


//eof triggerdlg.cpp
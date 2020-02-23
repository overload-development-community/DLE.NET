
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

char *objectNameTable [MAX_OBJECT_TYPES] = {
	"Wall",
	"Fireball",
	"Robot",
	"Hostage",
	"Player",
	"Mine",
	"Camera",
	"Power Up",
	"Debris",
	"Reactor",
	"Flare",
	"Clutter",
	"Ghost",
	"Light",
	"Player (Coop)",
	"Marker",
	"Camera",
	"Monsterball",
	"Smoke",
	"Explosion",
	"Effect"
};

ubyte objectTypeList [MAX_OBJECT_NUMBER] = {
	OBJ_ROBOT,
	OBJ_HOSTAGE,
	OBJ_PLAYER,
	OBJ_WEAPON,
	OBJ_POWERUP,
	OBJ_REACTOR,
	OBJ_COOP,
	OBJ_CAMBOT,
	OBJ_MONSTERBALL,
	OBJ_SMOKE,
	OBJ_EXPLOSION,
	OBJ_EFFECT
};

ubyte contentsList [MAX_CONTAINS_NUMBER] = {
	OBJ_ROBOT,
	OBJ_POWERUP
};

// the following array is used to select a list item by pObject->Type ()
char objectSelection [MAX_OBJECT_TYPES] = {
	-1, -1, 0, 1, 2, 3, -1, 4, -1, 5, -1, -1, -1, -1, 6, -1, 7, 8, 9, 10, 11
};

char contentsSelection [MAX_OBJECT_TYPES] = {
	-1, -1, 0, -1, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

//------------------------------------------------------------------------------

// list box tables
int modelNumList [N_ROBOT_TYPES_D2] = {
	0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x0f, 0x11,
	0x13, 0x14, 0x15, 0x16, 0x17, 0x19, 0x1a, 0x1c, 0x1d, 0x1f,
	0x21, 0x23, 0x25, 0x27, 0x28, 0x29, 0x2b, 0x2c, 0x2d, 0x2e,
	0x2f, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37, 0x38, 0x3a, 0x3c,
	0x3e, 0x40, 0x41, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x50, 0x52, 0x53, 0x55, 0x56,
	0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c
};

char *pszExplosionIds [] = {"small explosion", "medium explosion", "big explosion", "huge explosion", "red blast"};
int nExplosionIds [] = {7, 58, 0, 60, 106};

int powerupIdStrXlat [MAX_POWERUP_IDS_D2];

extern char *aiOptionTable [MAX_AI_OPTIONS_D2];
extern ubyte aiBehaviorTable [MAX_AI_BEHAVIOR_TABLE];

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CObjectTool, CToolDlg)
	ON_WM_HSCROLL ()
	ON_WM_VSCROLL ()
	ON_WM_PAINT ()
	ON_BN_CLICKED (IDC_OBJ_ADD, OnAdd)
	ON_BN_CLICKED (IDC_OBJ_DELETE, OnDelete)
	ON_BN_CLICKED (IDC_OBJ_DELETEALL, OnDeleteAll)
	ON_BN_CLICKED (IDC_OBJ_MOVE, OnMove)
	ON_BN_CLICKED (IDC_OBJ_RESET, OnReset)
	ON_BN_CLICKED (IDC_OBJ_DEFAULT, OnDefault)
	ON_BN_CLICKED (IDC_OBJ_ADVANCED, OnAdvanced)
	ON_BN_CLICKED (IDC_OBJ_MULTIPLAYER, OnMultiplayer)
	ON_BN_CLICKED (IDC_OBJ_SORT, OnSort)

	ON_CBN_SELCHANGE (IDC_OBJ_OBJNO, OnSetObject)
	ON_CBN_SELCHANGE (IDC_OBJ_TYPE, OnSetObjType)
	ON_CBN_SELCHANGE (IDC_OBJ_ID, OnSetObjId)
	ON_CBN_SELCHANGE (IDC_OBJ_AI, OnSetObjAI)
	ON_CBN_SELCHANGE (IDC_OBJ_TEXTURE, OnSetTexture)
	ON_CBN_SELCHANGE (IDC_OBJ_SPAWN_TYPE, OnSetSpawnType)
	ON_CBN_SELCHANGE (IDC_OBJ_SPAWN_ID, OnSetSpawnId)
	ON_EN_KILLFOCUS (IDC_OBJ_SPAWN_QTY, OnSetSpawnQty)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CObjectTool::CObjectTool (CPropertySheet *pParent)
	: CToolDlg (IDD_OBJECTDATA_HORZ + nLayout, pParent)
{
//Reset ();
}

//------------------------------------------------------------------------------

CObjectTool::~CObjectTool ()
{
if (m_bInited) {
	m_showObjWnd.DestroyWindow ();
	m_showSpawnWnd.DestroyWindow ();
	m_showTextureWnd.DestroyWindow ();
	}
}

//------------------------------------------------------------------------------

BOOL CObjectTool::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
	return FALSE;
CreateImgWnd (&m_showObjWnd, IDC_OBJ_SHOW);
CreateImgWnd (&m_showSpawnWnd, IDC_OBJ_SHOW_SPAWN);
CreateImgWnd (&m_showTextureWnd, IDC_OBJ_SHOW_TEXTURE);
CBInit (CBObjType (), (char**) objectNameTable, objectTypeList, null, MAX_OBJECT_NUMBER);
CBInit (CBSpawnType (), (char**) objectNameTable, contentsList, null, MAX_CONTAINS_NUMBER, 0, true);
CBInit (CBObjAI (), (char**) aiOptionTable, null, aiBehaviorTable, (DLE.IsD1File ()) ? MAX_AI_OPTIONS_D1 : MAX_AI_OPTIONS_D2);

m_objSpawnQty.Init (this, 0, IDC_OBJ_SPAWN_QTY_SPINNER, IDC_OBJ_SPAWN_QTY, 0, 100);

m_objectTools.Init (this);
if (nLayout) {
	m_objectTools.Add (new CObjectPhysicsTool (IDD_OBJECT_PHYSICS_VERT, this), IDD_OBJECT_PHYSICS_VERT, "Physics");
	m_objectTools.Add (new CObjectCombatTool (IDD_OBJECT_COMBAT_VERT, this), IDD_OBJECT_COMBAT_VERT, "Combat");
	m_objectTools.Add (new CObjectSoundTool (IDD_OBJECT_SOUND_VERT, this), IDD_OBJECT_SOUND_VERT, "Sounds");
	m_objectTools.Add (new CObjectAITool (IDD_OBJECT_AI_VERT, this), IDD_OBJECT_AI_VERT, "AI");
	m_objectTools.Add (new CObjectDeathTool (IDD_OBJECT_DEATH_VERT, this), IDD_OBJECT_DEATH_VERT, "Death");
	m_objectTools.Add (new CAdvancedObjTool (IDD_ADVOBJDATA_VERT, this), IDD_ADVOBJDATA_VERT, "Advanced");
	m_objectTools.Add (new CObjectPositionTool (IDD_OBJECT_POSITION_VERT, this), IDD_OBJECT_POSITION_VERT, "Position");
	}
else {
	m_objectTools.Add (new CObjectPhysicsTool (IDD_OBJECT_PHYSICS_HORZ, this), IDD_OBJECT_PHYSICS_HORZ, "Physics");
	m_objectTools.Add (new CObjectCombatTool (IDD_OBJECT_COMBAT_HORZ, this), IDD_OBJECT_COMBAT_HORZ, "Combat");
	m_objectTools.Add (new CObjectSoundTool (IDD_OBJECT_SOUND_HORZ, this), IDD_OBJECT_SOUND_HORZ, "Sounds");
	m_objectTools.Add (new CObjectAITool (IDD_OBJECT_AI_HORZ, this), IDD_OBJECT_AI_HORZ, "AI");
	m_objectTools.Add (new CObjectDeathTool (IDD_OBJECT_DEATH_HORZ, this), IDD_OBJECT_DEATH_HORZ, "Death");
	m_objectTools.Add (new CAdvancedObjTool (IDD_ADVOBJDATA_HORZ, this), IDD_ADVOBJDATA_HORZ, "Advanced");
	m_objectTools.Add (new CObjectPositionTool (IDD_OBJECT_POSITION_HORZ, this), IDD_OBJECT_POSITION_HORZ, "Position");
	}
m_objectTools.Setup ();

short nTextures = (DLE.IsD1File ()) ? MAX_TEXTURES_D1 : MAX_TEXTURES_D2;

CComboBox *pcb = CBObjTexture ();
pcb->AddString ("(none)");

int i;

for (i = 0; i < nTextures; i++) {
	char* p = textureManager.Name (-1, i);
	if (!strstr (p, "frame")) {
		int index = pcb->AddString (p);
		pcb->SetItemData(index++, i);
		}
	}

for (i = 0; i < MAX_POWERUP_IDS; i++)
	powerupIdStrXlat [i] = i;

Refresh ();
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CObjectTool::DoDataExchange (CDataExchange *pDX)
{
DDX_Control (pDX, IDC_TOOL_TAB, m_objectTools);
if (!HaveData (pDX)) 
	return;
//DDX_Text (pDX, IDC_OBJ_SPAWN_QTY, m_nSpawnQty);
DDX_Text (pDX, IDC_OBJ_INFO, m_szInfo, sizeof (m_szInfo));
if (!pDX->m_bSaveAndValidate) {
	char szCount [4];

	sprintf_s (szCount, sizeof (szCount), "%d", ObjOfAKindCount ());
	AfxSetWindowText (GetDlgItem (IDT_OBJ_COUNT)->GetSafeHwnd (), szCount);
	}
DDX_Check (pDX, IDC_OBJ_SORT, objectManager.SortObjects ());
}

//------------------------------------------------------------------------------

BOOL CObjectTool::OnSetActive ()
{
Refresh ();
return CToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

void CObjectTool::Reset ()
{
//m_nSpawnQty = 0;
}

//------------------------------------------------------------------------------

void CObjectTool::EnableControls (BOOL bEnable)
{
CToolDlg::EnableControls (IDC_OBJ_OBJNO, IDT_OBJ_CONT_PROB, bEnable);
}

//------------------------------------------------------------------------------

bool CObjectTool::OnScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (m_objSpawnQty.OnScroll (scrollCode, thumbPos, pScrollBar)) {
	OnSetSpawnQty ();
	return true;
	}
return false;
}

//------------------------------------------------------------------------------

void CObjectTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CToolDlg::OnHScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

void CObjectTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!OnScroll (scrollCode, thumbPos, pScrollBar))
	CToolDlg::OnVScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

void CObjectTool::Refresh (void)
{
if (!(m_bInited && theMine))
	return;


// update object list box
CBObjNo ()->ResetContent ();
CGameObject *pObject = objectManager.Object (0);

int i;
short type;
CStringResource res;

for (i = 0; i < objectManager.Count (); i++, pObject++) {
	res.Clear ();
	switch(pObject->Type ()) {
		case OBJ_ROBOT: /* an evil enemy */
			res.Load (ROBOT_STRING_TABLE + pObject->Id ());
			break;
		case OBJ_HOSTAGE: // a hostage you need to rescue
			sprintf_s (res.Value (), res.Size (), "Hostage");
			break;
		case OBJ_PLAYER: // the player on the console
			sprintf_s (res.Value (), res.Size (), "Player #%d", pObject->Id () + 1);
			break;
		case OBJ_WEAPON: //
			strcpy_s (res.Value (), res.Size (), "Red Mine");
			break;
		case OBJ_POWERUP: // a powerup you can pick up
			res.Load (POWERUP_STRING_TABLE + powerupIdStrXlat [pObject->Id ()]);
			break;
		case OBJ_REACTOR: // a control center */
			sprintf_s (res.Value (), res.Size (), "Reactor");
			break;
		case OBJ_COOP: // a cooperative player object
			sprintf_s (res.Value (), res.Size (), "Coop Player #%d", pObject->Id () + 1);
			break;
		case OBJ_CAMBOT: // a camera */
			sprintf_s (res.Value (), res.Size (), "Camera");
			break;
		case OBJ_MONSTERBALL: // a camera */
			sprintf_s (res.Value (), res.Size (), "Monsterball");
			break;
		case OBJ_EXPLOSION:
			sprintf_s (res.Value (), res.Size (), "Explosion");
			break;
		case OBJ_SMOKE: 
			sprintf_s (res.Value (), res.Size (), "Smoke");
			break;
		case OBJ_EFFECT:
			sprintf_s (res.Value (), res.Size (), "Effect");
			break;
		default:
			*res.Value () = '\0';
	}
	sprintf_s (message, sizeof (message), (i < 10) ? "%3d: %s": "%d: %s", i, res.Value ());
	CBObjNo ()->AddString (message);
	}
// add secret object to list
for (i = 0; i < triggerManager.WallTriggerCount (); i++)
	if (triggerManager.Trigger (i)->Type () == TT_SECRET_EXIT) {
		CBObjNo ()->AddString ("secret object");
		break;
		}
// select current object
CBObjNo ()->SetCurSel (current->ObjectId ());

// if secret object, disable everything but the "move" button
// and the object list, then return
if (current->ObjectId () == objectManager.Count ()) {
	CToolDlg::EnableControls (IDC_OBJ_OBJNO, IDC_OBJ_SPAWN_QTY, FALSE);
	CToolDlg::EnableControls (IDC_OBJ_MULTIPLAYER, IDC_OBJ_MULTIPLAYER, FALSE);
	CBObjNo ()->EnableWindow (TRUE);
	BtnCtrl (IDC_OBJ_MOVE)->EnableWindow (TRUE);

	strcpy_s (m_szInfo, sizeof (m_szInfo), "Secret Level Return");
	CBObjType ()->SetCurSel (-1);
	CBObjId ()->SetCurSel (-1);
	CBObjAI ()->SetCurSel (-1);
	CBObjTexture ()->SetCurSel (-1);
	CBSpawnType ()->SetCurSel (-1);
	CBSpawnId ()->SetCurSel (-1);

	CDC *pDC = m_showObjWnd.GetDC ();
	if (pDC) {
		CRect rc;
		m_showObjWnd.GetClientRect (rc);
		pDC->FillSolidRect (&rc, IMG_BKCOLOR);
		m_showObjWnd.ReleaseDC (pDC);
		CToolDlg::EnableControls (IDC_OBJ_DELETEALL, IDC_OBJ_DELETEALL, objectManager.Count () > 0);
		UpdateData (FALSE);
		}
	return;
	}

// otherwise (non-secret object), setup the rest of the dialog.
pObject = current->Object ();
sprintf_s (m_szInfo, sizeof (m_szInfo), "segment %d", pObject->Info ().nSegment);
if (robotManager.RobotInfo (pObject->Id ())->Info ().bCustom)
	strcat_s (m_szInfo, sizeof (m_szInfo), "\r\nmodified");

SelectItemData (CBObjType (), pObject->Type ());
SetObjectId (CBObjId (), pObject->Type (), pObject->Id ());

// ungray most buttons and combo boxes
CToolDlg::EnableControls (IDC_OBJ_OBJNO, IDC_OBJ_SPAWN_QTY, TRUE);
CToolDlg::EnableControls (IDC_OBJ_MULTIPLAYER, IDC_OBJ_MULTIPLAYER, TRUE);

// gray contains and behavior if not a robot type object
if (pObject->Type () != OBJ_ROBOT) {
	CToolDlg::EnableControls (IDC_OBJ_SPAWN_TYPE, IDC_OBJ_SPAWN_QTY, FALSE);
	if (pObject->Type () != OBJ_POWERUP)
		CToolDlg::EnableControls (IDC_OBJ_MULTIPLAYER, IDC_OBJ_MULTIPLAYER, FALSE);
	}
else {
	CToolDlg::EnableControls (IDC_OBJ_BRIGHT, IDT_OBJ_CONT_PROB, TRUE);
	}

// gray texture override if not a poly object
if (pObject->m_info.renderType != RT_POLYOBJ)
	CBObjTexture ()->EnableWindow (FALSE);

// gray edit if this is an RDL file
if (DLE.IsD1File ())
	CToolDlg::EnableControls (IDC_OBJ_BRIGHT, IDT_OBJ_CONT_PROB, FALSE);

// set contains data
type = (pObject->m_info.contents.type == (ubyte)-1) ? MAX_CONTAINS_NUMBER : contentsSelection [pObject->m_info.contents.type];

CBSpawnType ()->SetCurSel (type + 1);
BtnCtrl (IDC_OBJ_MULTIPLAYER)->SetCheck (current->Object ()->m_info.multiplayer);
current->Object ()->m_info.multiplayer = BtnCtrl (IDC_OBJ_MULTIPLAYER)->GetCheck ();
SetObjectId (CBSpawnId (), pObject->m_info.contents.type, pObject->m_info.contents.id, 1);
m_objSpawnQty.SetValue (pObject->m_info.contents.count);

if ((pObject->Type () == OBJ_ROBOT) || (pObject->Type () == OBJ_CAMBOT)) {
	int index = ((pObject->cType.aiInfo.behavior == AIB_RUN_FROM) && (pObject->cType.aiInfo.flags [4] & 0x02))  // smart bomb flag
					? 8 
					: pObject->cType.aiInfo.behavior - 0x80;
	CBObjAI ()->SetCurSel (index);
	}
else
	CBObjAI ()->SetCurSel (1); // Normal
RefreshRobot ();
DrawObjectImages ();
SetTextureOverride ();
CToolDlg::EnableControls (IDC_OBJ_DELETEALL, IDC_OBJ_DELETEALL, objectManager.Count () > 0);
Current ()->Refresh ();
UpdateData (FALSE);
DLE.MineView ()->Refresh (FALSE);
}

//------------------------------------------------------------------------------

void CObjectTool::RefreshRobot (void)
{
	  
int nType = current->Object ()->Type (); //CBObjType ()->GetItemData (CBObjType ()->GetCurSel ());

if (nType != OBJ_ROBOT) {
	if (nType != OBJ_POWERUP)
		BtnCtrl (IDC_OBJ_MULTIPLAYER)->SetCheck (FALSE);
	return;
	}
int nId = int (current->Object ()->Id ()); //CBObjId ()->GetItemData (CBObjId ()->GetCurSel ()));
CRobotInfo robotInfo = *robotManager.RobotInfo (nId);
Current ()->RefreshRobot ();
}
  
//------------------------------------------------------------------------------

void CObjectTool::UpdateRobot (void)
{
int nId = int (current->Object ()->Id ()); //CBObjId ()->GetItemData (CBObjId ()->GetCurSel ()));
if ((nId < 0) || (nId >= MAX_ROBOT_ID_D2))
	nId = 0;

CRobotInfo robotInfo = *robotManager.RobotInfo (nId);
robotInfo.Info ().bCustom |= 1;
robotInfo.SetModified (true);
Current ()->UpdateRobot ();
#if 0
undoManager.Begin (__FUNCTION__, udRobots);
*robotManager.RobotInfo (nId) = robotInfo;
undoManager.End (__FUNCTION__);
#endif
}

//------------------------------------------------------------------------
// CObjectTool - SetTextureOverride
//------------------------------------------------------------------------

void CObjectTool::SetTextureOverride (void)
{
CGameObject *pObject = current->Object ();
short nBaseTex = -1;

if (pObject->m_info.renderType != RT_POLYOBJ)
	CBObjTexture ()->SetCurSel (0);
else {
	nBaseTex = (short) current->Object ()->rType.polyModelInfo.nOverrideTexture;
	if ((nBaseTex < 0) || (nBaseTex >= ((DLE.IsD1File ()) ? MAX_TEXTURES_D1 : MAX_TEXTURES_D2))) {
		CBObjTexture ()->SetCurSel (0);
		nBaseTex = -1;	// -> force PaintTexture to clear the texture display window
		}
	}
PaintTexture (&m_showTextureWnd, IMG_BKCOLOR, nBaseTex, 0);
}

//------------------------------------------------------------------------
// CObjectTool - DrawObjectImage
//------------------------------------------------------------------------

void CObjectTool::DrawObjectImages (void) 
{
CGameObject *pObject = current->Object ();
pObject->DrawBitmap (&m_showObjWnd);
pObject->DrawBitmap (&m_showSpawnWnd);
}

//------------------------------------------------------------------------
// CObjectTool - Set Object Id Message
//------------------------------------------------------------------------

void CObjectTool::SetObjectId (CComboBox *pcb, short type, short id, short flag) 
{
	int h, i, j;
	short maxRobotIds = flag 
							  ? DLE.IsD1File () 
								  ? MAX_ROBOT_ID_D1 
								  : 64 
							  : DLE.IsD1File () 
								  ? MAX_ROBOT_ID_D1 
								  : MAX_ROBOT_ID_D2;

pcb->ResetContent ();

CStringResource res;

switch(type) {
	case OBJ_ROBOT: /* an evil enemy */
		for (i = 0; i < maxRobotIds; i++) {
			sprintf_s (res.Value (), res.Size (), (i < 10) ? "%3d: ": "%d: ", i);
			res.Load (ROBOT_STRING_TABLE + i);
			if (!strcmp (res.Value (), "(not used)"))
				continue;
			h = CBAddString (pcb, res.Value ());
			pcb->SetItemData (h, i);
			}
		if (id < 0 || id >= maxRobotIds) {
			sprintf_s (message, sizeof (message), " ObjectTool: Unknown robot id (%d)", id);
			DEBUGMSG (message);
			}
		SelectItemData (pcb, id); // if out of range, nothing is selected
		break;

	case OBJ_HOSTAGE: // a hostage you need to rescue
		for (i = 0; i <= 1; i++) {
			sprintf_s (res.Value (), res.Size (), "%d", i);
			h = pcb->AddString (res.Value ());
			pcb->SetItemData (h, i);
			}
		SelectItemData (pcb, id);
		break;

	case OBJ_PLAYER: // the player on the console
		for (i = 0; i < MAX_PLAYERS; i++) {
			sprintf_s (res.Value (), res.Size (), (i < 9) ? "%3d" : "%d", i + 1);
			h = pcb->AddString (res.Value ());
			pcb->SetItemData (h, i);
			}
		SelectItemData (pcb, id);
		break;

	case OBJ_WEAPON: //
		h = pcb->AddString ("Mine");
		pcb->SetItemData (0, h);
		pcb->SetCurSel (0);
		break;

	case OBJ_CAMBOT: //
		h = pcb->AddString ("Camera");
		pcb->SetItemData (0, h);
		pcb->SetCurSel (0);
		break;

	case OBJ_MONSTERBALL: //
		h = pcb->AddString ("Monsterball");
		pcb->SetItemData (0, h);
		pcb->SetCurSel (0);
		break;

	case OBJ_EXPLOSION: //
		for (i = 0; i < 5; i++) {
			h = pcb->AddString (pszExplosionIds [i]);
			pcb->SetItemData (h, nExplosionIds [i]);
			}
		SelectItemData (pcb, id); // if out of range, nothing is selected
		break;

	case OBJ_SMOKE: //
		h = pcb->AddString ("Smoke");
		pcb->SetItemData (0, h);
		pcb->SetCurSel (0);
		break;

	case OBJ_EFFECT: //
		h = pcb->AddString ("Effect");
		pcb->SetItemData (0, h);
		pcb->SetCurSel (0);
		break;

	case OBJ_POWERUP: // a powerup you can pick up
		int xlat [100];
		memset (xlat, 0xff, sizeof (xlat));
		h = pcb->AddString ("(none)");
		pcb->SetItemData (h, -1);
		for (i = 0; i < MAX_POWERUP_IDS; i++) {
			j = powerupIdStrXlat [i];
			res.Clear ();
			res.Load (POWERUP_STRING_TABLE + j);
			if (!strcmp (res.Value (), "(not used)"))
				continue;
			h = pcb->AddString (res.Value ());
			xlat [i] = j;
			pcb->SetItemData (h, j);
			}
#if 0//def _DEBUG // hack to int bogus powerup ids
		CGameObject *pObject;
		for (i = 0, pObject = objectManager.Object (0); i < objectManager.Count (); i++, pObject++)
			if ((pObject->Type () == OBJ_POWERUP) && (xlat [pObject->Id ()] == -1)) {
				for (i = 0, pObject = objectManager.Object (0); i < objectManager.Count (); i++, pObject++)
					if (pObject->Type () == OBJ_POWERUP)
						pObject->Id () = xlat [pObject->Id ()]; 
				break;
				}
#endif
		SelectItemData (pcb, id);
		break;

	case OBJ_REACTOR: // a control center */
		if (DLE.IsD1File ()) {
			for ( i = 0; i <= 25; i++) { //??? not sure of max
				sprintf_s (res.Value (), res.Size (), "%d", i);
				h = pcb->AddString (res.Value ());
				pcb->SetItemData (h, i);
				}
			}
		else {
			for (i = 0; i <= 6; i++) {
				sprintf_s (res.Value (), res.Size (), "%d", i);
				h = pcb->AddString (res.Value ());
				pcb->SetItemData (h, i);
				}
			}
		SelectItemData (pcb, id);
		break;

	case OBJ_COOP: // a cooperative player object
		for (i = MAX_PLAYERS; i < MAX_PLAYERS + MAX_COOP_PLAYERS; i++) {
			sprintf_s (res.Value (), res.Size (), "%d", i);
			h = pcb->AddString (res.Value ());
			pcb->SetItemData (h, i - MAX_PLAYERS);
			}
		SelectItemData (pcb, id - MAX_PLAYERS);
		break;

	default:
		h = pcb->AddString ("(none)");
		pcb->SetItemData (0, h);
		pcb->SetCurSel (0);
	break;
	}
}

//------------------------------------------------------------------------
// CObjectTool - WMDrawItem
//------------------------------------------------------------------------

void CObjectTool::OnPaint ()
{
CToolDlg::OnPaint ();
DrawObjectImages ();
SetTextureOverride ();
}

//------------------------------------------------------------------------
// CObjectTool - Add Object Message
//------------------------------------------------------------------------

void CObjectTool::OnAdd () 
{
if (current->ObjectId () == objectManager.Count ()) {
	ErrorMsg ("Cannot add another secret return.");
	return;
 }

if (objectManager.Count () >= MAX_OBJECTS) {
	ErrorMsg ("Maximum numbers of objects reached");
	return;
	}
objectManager.Create (OBJ_NONE);
Refresh ();
}

//------------------------------------------------------------------------
// CObjectTool - Delete Object Message
//------------------------------------------------------------------------

void CObjectTool::OnDelete ()
{
if (current->ObjectId () == objectManager.Count ()) {
	ErrorMsg ("Cannot delete the secret return.");
	return;
	}
if (objectManager.Count () == 1) {
	ErrorMsg ("Cannot delete the last object");
	return;
	}
if (QueryMsg ("Are you sure you want to delete this object?") == IDYES) {
	objectManager.Delete ();
	Refresh ();
	DLE.MineView ()->Refresh (false);
	}
}

//------------------------------------------------------------------------
// CObjectTool - Delete All (taggd) Triggers
//------------------------------------------------------------------------

void CObjectTool::OnDeleteAll () 
{
	int nDeleted = 0;

if (QueryMsg ("Are you sure you want to delete all objects of this type and ID?") == IDYES) {
	undoManager.Begin (__FUNCTION__, udObjects);
	DLE.MineView ()->DelayRefresh (true);
	CGameObject *pObject = current->Object ();
	int nType = pObject->Type ();
	int nId = pObject->Id ();
	pObject = objectManager.Object (0);
	bool bAll = (segmentManager.TaggedCount (true) == 0);

	for (int h = objectManager.Count (), i = 0; i < h; ) {
		if ((pObject->Type () == nType) && (pObject->Id () == nId) && (bAll || segmentManager.Segment (pObject->m_info.nSegment)->IsTagged ())) {
			objectManager.Delete (i);
			nDeleted++;
			h--;
			}
		else
			i++, pObject++;
		}
	DLE.MineView ()->DelayRefresh (false);
	undoManager.End (__FUNCTION__);
	}

if (nDeleted) {
	DLE.MineView ()->Refresh ();
	Refresh ();
	}
}

//------------------------------------------------------------------------
// CObjectTool - Reset Object Message
//------------------------------------------------------------------------

void CObjectTool::OnReset () 
{
CDoubleMatrix* orient;

undoManager.Begin (__FUNCTION__, udObjects);
if (current->ObjectId () == objectManager.Count ()) {
	orient = &objectManager.SecretOrient ();
	orient->Set (1, 0, 0, 0, 1, 0, 0, 0, 1);
} else {
	orient = &current->Object ()->m_location.orient;
	orient->Set (1, 0, 0, 0, 1, 0, 0, 0, 1);
	}
undoManager.End (__FUNCTION__);
Refresh ();
DLE.MineView ()->Refresh (false);
}

//------------------------------------------------------------------------
// CObjectTool - AdvancedMsg
//------------------------------------------------------------------------

void CObjectTool::OnAdvanced () 
{
}


//------------------------------------------------------------------------
// CObjectTool - Move Object Message
//------------------------------------------------------------------------

void CObjectTool::OnMove ()
{
objectManager.Move ();
Refresh ();
}

//------------------------------------------------------------------------
// CObjectTool - Object Number Message
//------------------------------------------------------------------------

void CObjectTool::OnSetObject ()
{
short old_object = current->ObjectId ();
short new_object = CBObjNo ()->GetCurSel ();
DLE.MineView ()->RefreshObject (old_object, new_object);
//Refresh ();
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------

bool CObjectTool::SetPlayerId (CGameObject& obj, int objType, int *ids, int idCount, char *pszError)
{
CGameObject *pObject = objectManager.Object (0);
int		i, n = 0;

for (i = objectManager.Count (); i && (n < idCount); i--, pObject++)
	if (pObject->m_info.type == objType)
		ids [n++] = -1;
if (n == idCount) {
	ErrorMsg (pszError);
	return false;
	}
for (i = 0; i < idCount; i++)
	if (ids [i] >= 0) {
		obj.Id () = ids [i];
		break;
		}
return true;
}

//------------------------------------------------------------------------
// CObjectTool - Type ComboBox Message
//------------------------------------------------------------------------

void CObjectTool::OnSetObjType () 
{
CGameObject *pObject = current->Object ();
//int selection = objectTypeList [CBObjType ()->GetCurSel ()];
int selection = (int) CBObjType ()->GetItemData (CBObjType ()->GetCurSel ());
if (DLE.IsD1File () && (selection == OBJ_WEAPON)) {
	ErrorMsg ("You can not use this type of object in a Descent 1 level");
	return;
	}
if ((selection == OBJ_SMOKE) || (selection == OBJ_EFFECT)) {
	ErrorMsg ("You can use the effects tool to create and edit this type of object");
	return;
	}
// set id
int playerIds [16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int coopIds [3];
for (int i = 0; i < 3; i++)
	coopIds [i] = MAX_PLAYERS + i;

switch (selection) {
	case OBJ_PLAYER:
		if (!SetPlayerId (*pObject, selection, playerIds, MAX_PLAYERS, "Only 8/16 players allowed."))
			return;
		break;

	case OBJ_COOP:
		if (!SetPlayerId (*pObject, selection, coopIds, 3, "Only 3 coop players allowed."))
			return;
		break;

	case OBJ_WEAPON:
		pObject->Id () = SMALLMINE_ID;
		break;

	case OBJ_REACTOR:
		pObject->Id () = DLE.IsD1File () ? 0: 2;
		break;

	case OBJ_EXPLOSION:
		pObject->Id () = 0;
		break;

	default:
		pObject->Id () = 0;
	}
pObject->Setup (selection);
SetObjectId (CBObjId (), selection, 0);
Refresh ();
DLE.MineView ()->Refresh (false);
}

//------------------------------------------------------------------------
// CObjectTool - IdMsg Message
//
// This routine resets the size, shield, vclip if the id changes.
//------------------------------------------------------------------------

int CObjectTool::GetObjectsOfAKind (int nType, CGameObject *objList [])
{
	int i, nObjects = 0;
	CGameObject *pObject;

for (i = objectManager.Count (), pObject = objectManager.Object (0); i; i--, pObject++)
	if (pObject->Type () == nType)
		objList [nObjects++] = pObject;
return nObjects;
}

//------------------------------------------------------------------------

void CObjectTool::SetNewObjId (CGameObject *pObject, int nType, int nId, int nMaxId)
{
if (nId == pObject->Id ())
	return;

	int nObjects = ObjOfAKindCount (nType);

CGameObject **objList = new CGameObject* [nObjects];
GetObjectsOfAKind (nType, objList);
if ((nMaxId > 0) && (nId >= nMaxId)) {
	nId = nMaxId;
	if (nId == pObject->Id ())
		return;
	}
// find object that currently has id nCurSel and swap ids
for (int i = 0; i < nObjects; i++)
	if (objList [i]->m_info.id == nId) {
		objList [i]->m_info.id = pObject->Id ();
		break;
		}
pObject->Id () = nId;
delete objList;
}

//------------------------------------------------------------------------

void CObjectTool::OnSetObjId ()
{
	int	id;

CGameObject *pObject = current->Object ();
CComboBox *pcb = CBObjId ();
int nCurSel = int (pcb->GetItemData (pcb->GetCurSel ()));

undoManager.Begin (__FUNCTION__, udObjects);
switch (pObject->Type ()) {
	case OBJ_PLAYER:
		SetNewObjId (pObject, OBJ_PLAYER, nCurSel, MAX_PLAYERS);
		break;

	case OBJ_COOP:
		SetNewObjId (pObject, OBJ_COOP, nCurSel + MAX_PLAYERS, MAX_PLAYERS + MAX_COOP_PLAYERS);
		break;

	case OBJ_WEAPON:
		pObject->Id () = SMALLMINE_ID;
		break;

	case OBJ_REACTOR:
		pObject->Id () = nCurSel; // + (IsD2File ());
		pObject->rType.animationInfo.nAnimation = nCurSel;
		break;

	default:
		pObject->Id () = nCurSel;
	}

switch (pObject->Type ()) {
	case OBJ_POWERUP:
		id = (pObject->Id () < MAX_POWERUP_IDS_USED) ? pObject->Id () : POW_AMMORACK;
		pObject->m_info.size = powerupSize [id];
		pObject->m_info.shields = DEFAULT_SHIELD;
		pObject->rType.animationInfo.nAnimation = powerupClip [id];
		break;

	case OBJ_ROBOT:
		pObject->m_info.size = robotSize [pObject->Id ()];
		pObject->m_info.shields = robotShield [pObject->Id ()];
		pObject->rType.polyModelInfo.nModel = robotClip [pObject->Id ()];
		break;

	case OBJ_REACTOR:
		pObject->m_info.size = REACTOR_SIZE;
		pObject->m_info.shields = REACTOR_SHIELD;
		if (DLE.IsD1File ())
			pObject->rType.polyModelInfo.nModel = REACTOR_CLIP_NUMBER;
		else {
			int model;
			switch(pObject->Id ()) {
				case 0: model = 93; break;
				case 1: model = 95; break;
				case 2: model = 97; break;
				case 3: model = 99; break;
				case 4: model = 101; break;
				case 5: model = 103; break;
				case 6: model = 105; break;
				default: model = 97; break; // level 1's reactor
				}
			pObject->rType.polyModelInfo.nModel = model;
		}
		break;

	case OBJ_PLAYER:
		pObject->m_info.size = PLAYER_SIZE;
		pObject->m_info.shields = DEFAULT_SHIELD;
		pObject->rType.polyModelInfo.nModel = PLAYER_CLIP_NUMBER;
		break;

	case OBJ_WEAPON:
		pObject->m_info.size = WEAPON_SIZE;
		pObject->m_info.shields = WEAPON_SHIELD;
		pObject->rType.polyModelInfo.nModel = MINE_CLIP_NUMBER;
		break;

	case OBJ_COOP:
		pObject->m_info.size = PLAYER_SIZE;
		pObject->m_info.shields = DEFAULT_SHIELD;
		pObject->rType.polyModelInfo.nModel = COOP_CLIP_NUMBER;
		break;

	case OBJ_HOSTAGE:
		pObject->m_info.size = PLAYER_SIZE;
		pObject->m_info.shields = DEFAULT_SHIELD;
		pObject->rType.animationInfo.nAnimation = HOSTAGE_CLIP_NUMBER;
		break;
	}
objectManager.Sort ();
SelectItemData (pcb, pObject->Id ());
undoManager.End (__FUNCTION__);
Refresh ();
}

//------------------------------------------------------------------------
// CObjectTool - ContainsQtyMsg
//------------------------------------------------------------------------

void CObjectTool::OnSetSpawnQty ()
{
if (m_objSpawnQty.OnEdit ()) {
	UpdateData (TRUE);
	undoManager.Begin (__FUNCTION__, udObjects);
	current->Object ()->m_info.contents.count = m_objSpawnQty.GetValue ();
	undoManager.End (__FUNCTION__);
	Refresh ();
	}
}

//------------------------------------------------------------------------
// CObjectTool - Container Type ComboBox Message
//------------------------------------------------------------------------

void CObjectTool::OnSetSpawnType () 
{
CGameObject *pObject = current->Object ();
int selection;
undoManager.Begin (__FUNCTION__, udObjects);
int i = CBSpawnType ()->GetCurSel () - 1;
if ((i < 0) || (i == MAX_CONTAINS_NUMBER)) {
	pObject->m_info.contents.count = 0;
	pObject->m_info.contents.type = -1;
	pObject->m_info.contents.id = -1;
	UpdateData (TRUE);
	Refresh ();
	}
else {
	pObject->m_info.contents.type = 
	selection = contentsList [i];
	SetObjectId (CBSpawnId (), selection, 0, 1);
	UpdateData (TRUE);
	if (m_objSpawnQty.GetValue () < 1) {
		m_objSpawnQty.SetValue (1);
		UpdateData (FALSE);
		}
	OnSetSpawnQty ();
	OnSetSpawnId ();
	}
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------
// CObjectTool - ContainsIdMsg
//------------------------------------------------------------------------

void CObjectTool::OnSetSpawnId () 
{
CGameObject *pObject = current->Object ();

undoManager.Begin (__FUNCTION__, udObjects);
if (pObject->m_info.contents.count < -1)
	pObject->m_info.contents.count = -1;
int i = CBSpawnType ()->GetCurSel () - 1;
if ((i > -1) && (pObject->m_info.contents.count > 0)) {
	pObject->m_info.contents.type = contentsList [i];
	pObject->m_info.contents.id = (char) CBSpawnId ()->GetItemData (CBSpawnId ()->GetCurSel ());
	}
else {
	pObject->m_info.contents.type = -1;
	pObject->m_info.contents.id = -1;
	}
Refresh ();
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------
// CObjectTool - Options ComboBox Message
//------------------------------------------------------------------------

void CObjectTool::OnSetObjAI ()
{
undoManager.Begin (__FUNCTION__, udObjects);
CGameObject *pObject = current->Object ();
if ((pObject->Type () == OBJ_ROBOT) || (pObject->Type () == OBJ_CAMBOT)) {
	int index = CBObjAI ()->GetCurSel ();
	if (index == 8) {
		index = AIB_RUN_FROM;
		pObject->cType.aiInfo.flags [4] |= 2; // smart bomb flag
		}
	else {
		index += 0x80;
		pObject->cType.aiInfo.flags [4] &= ~2;
		}
	pObject->cType.aiInfo.behavior = index;
	}
else
	CBObjAI ()->SetCurSel (1); // Normal
Refresh ();
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------
// CObjectTool - Texture ComboBox Message
//------------------------------------------------------------------------

void CObjectTool::OnSetTexture ()
{
CGameObject *pObject = current->Object ();

if (pObject->m_info.renderType == RT_POLYOBJ) {
	undoManager.Begin (__FUNCTION__, udObjects);
	int index = CBObjTexture ()->GetCurSel ();
	pObject->rType.polyModelInfo.nOverrideTexture = (index > 0) ? (short)CBObjTexture ()->GetItemData (index): -1;
	undoManager.End (__FUNCTION__);
	Refresh ();
	}
}

//------------------------------------------------------------------------
// CObjectTool - Texture ComboBox Message
//------------------------------------------------------------------------

void CObjectTool::OnDefault ()
{
if (CBObjType ()->GetItemData (CBObjType ()->GetCurSel ()) != OBJ_ROBOT)
	return;
int i = int (CBObjId ()->GetItemData (CBObjId ()->GetCurSel ()));
undoManager.Begin (__FUNCTION__, udRobots);
memcpy (robotManager.RobotInfo (i), robotManager.DefRobotInfo (i), sizeof (tRobotInfo));
undoManager.End (__FUNCTION__);
Refresh ();
}

//------------------------------------------------------------------------------

afx_msg void CObjectTool::OnMultiplayer ()
{
current->Object ()->m_info.multiplayer = BtnCtrl (IDC_OBJ_MULTIPLAYER)->GetCheck ();
Refresh ();
}

//------------------------------------------------------------------------------

afx_msg void CObjectTool::OnSort ()
{
if ((objectManager.SortObjects () = BtnCtrl (IDC_OBJ_SORT)->GetCheck ())) {
	objectManager.Sort ();
	Refresh ();
	}
DLE.WritePrivateProfileInt ("SortObjects", objectManager.SortObjects ());
}

//------------------------------------------------------------------------------

int CObjectTool::ObjOfAKindCount (int nType, int nId)
{
if (nType < 0)
	nType = current->Object ()->m_info.type;
if (nId < 0)
	nId =  current->Object ()->m_info.id;
int nCount = 0;
CGameObject *pObject = objectManager.Object (0);
int i;
for (i = objectManager.Count (); i; i--, pObject++)
	if ((pObject->Type () == nType) && ((pObject->Type () == OBJ_PLAYER) || (pObject->Type () == OBJ_COOP) || (pObject->Id () == nId))) 
		nCount++;
return nCount;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CObjectTabDlg::InitSliders (CDialog* parent, CExtSliderCtrl* sliders, CSliderData* data, int nSliders)
{
for (int i = 0; i < nSliders; i++)
	sliders [i].Init (parent, data [i].nId, data [i].nId + 1, -data [i].nId - 2, 
							int (data [i].nMin * data [i].nFactor + 0.5), 
							int (data [i].nMax * data [i].nFactor + 0.5), 
							data [i].nFactor, 1.0 / data [i].nFactor);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//eof objectdlg.cpp
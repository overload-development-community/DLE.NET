
#include <math.h>
#include "stdafx.h"
#include "afxpriv.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"
#include "toolview.h"

#include "PaletteManager.h"
#include "textures.h"
#include "global.h"
#include "FileManager.h"

#include <math.h>

ushort wallFlagTable [MAX_WALL_FLAGS] = {
	WALL_BLASTED,
	WALL_DOOR_OPENED,
	WALL_DOOR_LOCKED,
	WALL_DOOR_AUTO,
	WALL_ILLUSION_OFF,
	WALL_WALL_SWITCH,
	WALL_BUDDY_PROOF,
	WALL_RENDER_ADDITIVE,
	WALL_IGNORE_MARKER
	};

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CWallTool, CTexToolDlg)
	ON_WM_HSCROLL ()
	ON_BN_CLICKED (IDC_WALL_ADD, OnAddWall)
	ON_BN_CLICKED (IDC_WALL_DELETE, OnDeleteWall)
	ON_BN_CLICKED (IDC_WALL_DELETEALL, OnDeleteWallAll)
	ON_BN_CLICKED (IDC_WALL_OTHERSIDE, OnOtherSide)
	ON_BN_CLICKED (IDC_WALL_LOCK, OnLock)
	ON_BN_CLICKED (IDC_WALL_NOKEY, OnNoKey)
	ON_BN_CLICKED (IDC_WALL_BLUEKEY, OnBlueKey)
	ON_BN_CLICKED (IDC_WALL_GOLDKEY, OnGoldKey)
	ON_BN_CLICKED (IDC_WALL_REDKEY, OnRedKey)
	ON_BN_CLICKED (IDC_WALL_BLASTED, OnBlasted)
	ON_BN_CLICKED (IDC_WALL_DOOROPEN, OnDoorOpen)
	ON_BN_CLICKED (IDC_WALL_DOORLOCKED, OnDoorLocked)
	ON_BN_CLICKED (IDC_WALL_DOORAUTO, OnDoorAuto)
	ON_BN_CLICKED (IDC_WALL_ILLUSIONOFF, OnIllusionOff)
	ON_BN_CLICKED (IDC_WALL_SWITCH, OnSwitch)
	ON_BN_CLICKED (IDC_WALL_BUDDYPROOF, OnBuddyProof)
	ON_BN_CLICKED (IDC_WALL_RENDER_ADDITIVE, OnRenderAdditive)
	ON_BN_CLICKED (IDC_WALL_IGNORE_MARKER, OnIgnoreMarker)
	ON_BN_CLICKED (IDC_WALL_ADD_DOOR_NORMAL, OnAddDoorNormal)
	ON_BN_CLICKED (IDC_WALL_ADD_DOOR_EXIT, OnAddDoorExit)
	ON_BN_CLICKED (IDC_WALL_ADD_DOOR_SECRETEXIT, OnAddDoorSecretExit)
	ON_BN_CLICKED (IDC_WALL_ADD_DOOR_PRISON, OnAddDoorPrison)
	ON_BN_CLICKED (IDC_WALL_ADD_DOOR_GUIDEBOT, OnAddDoorGuideBot)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_FUELCELL, OnAddWallFuelCell)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_ILLUSION, OnAddWallIllusion)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_FORCEFIELD, OnAddWallForceField)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_FAN, OnAddWallFan)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_GRATE, OnAddWallGrate)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_WATERFALL, OnAddWallWaterfall)
	ON_BN_CLICKED (IDC_WALL_ADD_WALL_LAVAFALL, OnAddWallLavafall)
	ON_BN_CLICKED (IDC_WALL_BOTHSIDES, OnBothSides)
	ON_BN_CLICKED (IDC_WALL_FLYTHROUGH, OnStrength)
	ON_CBN_SELCHANGE (IDC_WALL_WALLNO, OnSetWall)
	ON_CBN_SELCHANGE (IDC_WALL_TYPE, OnSetType)
	ON_CBN_SELCHANGE (IDC_WALL_CLIPNO, OnSetClip)
	ON_EN_KILLFOCUS (IDC_WALL_STRENGTH, OnStrength)
	ON_EN_KILLFOCUS (IDC_WALL_CLOAK, OnCloak)
	ON_EN_UPDATE (IDC_WALL_STRENGTH, OnStrength)
#ifdef RELEASE
	ON_EN_UPDATE (IDC_WALL_CLOAK, OnCloak)
#endif
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------
// DIALOG - CWallTool (constructor)
//------------------------------------------------------------------------------

CWallTool::CWallTool (CPropertySheet *pParent)
	: CTexToolDlg (IDD_WALLDATA_HORZ + nLayout, pParent, IDC_WALL_SHOW, 5, RGB (0,0,0))
{
m_defWall.Clear ();
m_defWall.Type () = WALL_DOOR;
m_defWall.Info ().flags = WALL_DOOR_AUTO;
m_defWall.Info ().keys = KEY_NONE;
m_defWall.Info ().nClip = -1;
m_defWall.Info ().cloakValue = 0; // 0% transparency (opaque) - since this is a door
m_defDoorTexture = -1;
m_defTexture = -1;
m_defOvlTexture = 414;
m_nType = 0;
memcpy (&m_defDoor, &m_defWall, sizeof (m_defDoor));
m_bBothSides = FALSE;
m_bLock = false;
m_bDelayRefresh = false;
Reset ();
}

//------------------------------------------------------------------------------

CWallTool::~CWallTool ()
{
}

//------------------------------------------------------------------------------

void CWallTool::Reset ()
{
m_nSegment = 0;
m_nSide = 1;
m_nTrigger = 0;
m_nWall [0] = -1;
m_nWall [1] = -1;
m_nClip = 0;
m_nStrength = 0;
m_nCloak = 0;
m_bFlyThrough = 0;
memset (m_bKeys, 0, sizeof (m_bKeys));
memset (m_bFlags, 0, sizeof (m_bFlags));
*m_szMsg = '\0';
}

//------------------------------------------------------------------------------

BOOL CWallTool::TextureIsVisible ()
{
return m_pWall [0] != null;
}

//------------------------------------------------------------------------------
// CWallTool - SetupWindow
//------------------------------------------------------------------------------

BOOL CWallTool::OnInitDialog ()
{
	static char* pszWallTypes [] = {
		"Normal",
		"Blastable",
		"Door",
		"Illusion",
		"Open",
		"Close",
		"Overlay",
		"Cloaked",
		"Colored"
		};

	CComboBox *pcb;

CTexToolDlg::OnInitDialog ();
InitCBWallNo ();

pcb = CBType ();
pcb->ResetContent ();

int h, i, j = sizeof (pszWallTypes) /  sizeof (*pszWallTypes);
for (i = 0; i < j; i++) {
	h = pcb->AddString (pszWallTypes [i]);
	pcb->SetItemData (h, i);
	}	

pcb = CBClipNo ();
pcb->ResetContent ();
j = (DLE.IsD2File ()) ? NUM_OF_CLIPS_D2 : NUM_OF_CLIPS_D1;
for (i = 0; i < j; i++) {
	sprintf_s (m_szMsg, sizeof (m_szMsg), i ? "door%02d" : "wall%02d", doorClipTable [i]);
	pcb->AddString (m_szMsg);
	}
#if 1
m_transpSlider.Init (this, IDC_WALL_TRANSP_SLIDER, IDC_WALL_TRANSP_SPINNER, -IDC_WALL_TRANSP_TEXT, 0, 100, 10.0, 1.0, 1, "transp: %d%%");
#else
TransparencySpinner ()->SetRange (0, 10);
InitSlider (IDC_WALL_TRANSP_SLIDER, 0, 10);
for (i = 0; i <= 10; i++)
	SlCtrl (IDC_WALL_TRANSP_SLIDER)->SetTic (i);
#endif
*m_szMsg = '\0';
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CWallTool::InitCBWallNo ()
{
CComboBox *pcb = CBWallNo ();
pcb->ResetContent ();
int i;
for (i = 0; i < wallManager.WallCount (); i++) {
	_itoa_s (i, message, sizeof (message), 10);
	pcb->AddString (message);
	}
pcb->SetCurSel (m_nWall [0]);
}

//------------------------------------------------------------------------------

void CWallTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;
DDX_Text (pDX, IDC_WALL_CUBE, m_nSegment);
DDX_Text (pDX, IDC_WALL_SIDE, m_nSide);
DDX_Text (pDX, IDC_WALL_TRIGGER, m_nTrigger);
DDX_CBIndex (pDX, IDC_WALL_WALLNO, m_nWall [0]);
//DDX_CBIndex (pDX, IDC_WALL_TYPE, m_nType);
SelectItemData (CBType (), m_nType);
DDX_CBIndex (pDX, IDC_WALL_CLIPNO, m_nClip);
DDX_Double (pDX, IDC_WALL_STRENGTH, m_nStrength, -100, 100, "%3.1f");
DDX_Double (pDX, IDC_WALL_CLOAK, m_nCloak, 0, 100, "%3.1f");
int i;
for (i = 0; i < 4; i++)
	DDX_Check (pDX, IDC_WALL_NOKEY + i, m_bKeys [i]);
for (i = 0; i < MAX_WALL_FLAGS; i++)
	DDX_Check (pDX, IDC_WALL_BLASTED + i, m_bFlags [i]);
DDX_Check (pDX, IDC_WALL_FLYTHROUGH, m_bFlyThrough);
DDX_Text (pDX, IDC_WALL_MSG, m_szMsg, sizeof (m_szMsg));

#if 1
if (!pDX->m_bSaveAndValidate)
	m_transpSlider.SetValue (int (((m_nType == WALL_COLORED) ? m_nStrength : m_nCloak) + 0.5));
else {
	if (m_nType == WALL_COLORED) 
		m_nStrength = m_transpSlider.GetValue ();
	else
		m_nCloak = m_transpSlider.GetValue ();
	DLE.MineView ()->Refresh (false);
	}
#else
int t = (int) (((m_nType == WALL_COLORED) ? m_nStrength : m_nCloak) + 5) / 10;
char szTransparency [20];
DDX_Slider (pDX, IDC_WALL_TRANSP_SLIDER, t);
TransparencySpinner ()->SetPos (t);
sprintf_s (szTransparency, sizeof (szTransparency), "transp: %d%c", t * 10, '%');
SetDlgItemText (IDC_WALL_TRANSP_TEXT, szTransparency);
#endif
}

//------------------------------------------------------------------------------

void CWallTool::EnableControls (BOOL bEnable)
{
int i;
for (i = IDC_WALL_WALLNO + 1; i <= IDC_WALL_FLYTHROUGH; i++)
	GetDlgItem (i)->EnableWindow (bEnable);
}

//------------------------------------------------------------------------------

BOOL CWallTool::OnSetActive ()
{
Refresh ();
GetWalls ();
return CTexToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

BOOL CWallTool::OnKillActive ()
{
Refresh ();
return CTexToolDlg::OnKillActive ();
}

//------------------------------------------------------------------------------

void CWallTool::OnLock ()
{
m_bLock = !m_bLock;
GetDlgItem (IDC_WALL_LOCK)->SetWindowText (m_bLock ? "&unlock" : "&lock");
}

//------------------------------------------------------------------------------
// CWallTool - RefreshData
//------------------------------------------------------------------------------

void CWallTool::Refresh ()
{
if (m_bDelayRefresh)
	return;
if (!(m_bInited && theMine))
	return;

InitCBWallNo ();
m_pWall [0] = current->Wall ();
if (m_pWall [0] == null) {
	strcpy_s (m_szMsg, sizeof (m_szMsg), "No wall for current side");
	EnableControls (FALSE);
	if (current->Segment ()->ChildId (current->SideId ()) >= 0)
		CToolDlg::EnableControls (IDC_WALL_ADD_DOOR_NORMAL, IDC_WALL_ADD_WALL_LAVAFALL, TRUE);
	GetDlgItem (IDC_WALL_ADD)->EnableWindow (TRUE);
	GetDlgItem (IDC_WALL_TYPE)->EnableWindow (TRUE);
	//CBType ()->SetCurSel (m_nType);
	SelectItemData (CBType (), m_nType);
	CBClipNo ()->SetCurSel (-1);
	Reset ();
	} 
else {
    // enable all
	EnableControls (TRUE);
	GetDlgItem (IDC_WALL_ADD)->EnableWindow (FALSE);
   if ((DLE.IsD2File ()) && (m_pWall [0]->Type () == WALL_COLORED))
		GetDlgItem (IDC_WALL_STRENGTH)->EnableWindow (FALSE);
	else {
		GetDlgItem (IDC_WALL_FLYTHROUGH)->EnableWindow (FALSE);
		}
   if ((DLE.IsD1File ()) || (m_pWall [0]->Type () == WALL_COLORED))
		GetDlgItem (IDC_WALL_CLOAK)->EnableWindow (FALSE);

    // enable buddy proof and switch checkboxes only if d2 level
	if (DLE.IsD1File ()) {
		int i;
		for (i = 0; i < 2; i++)
			GetDlgItem (IDC_WALL_SWITCH + i)->EnableWindow (FALSE);
		}
	// update wall data
	if (m_pWall [0]->Info ().nTrigger == NO_TRIGGER)
		sprintf_s (m_szMsg, sizeof (m_szMsg), "segment = %ld, side = %ld, no trigger", m_pWall [0]->m_nSegment, m_pWall [0]->m_nSide);
	else
		sprintf_s (m_szMsg, sizeof (m_szMsg), "segment = %ld, side = %ld, trigger= %d", m_pWall [0]->m_nSegment, m_pWall [0]->m_nSide, (int)m_pWall [0]->Info ().nTrigger);

	m_nWall [0] = int (m_pWall [0] - wallManager.Wall (0));
	GetOtherWall ();
	m_nSegment = m_pWall [0]->m_nSegment;
	m_nSide = m_pWall [0]->m_nSide + 1;
	m_nTrigger = (m_pWall [0]->Info ().nTrigger < triggerManager.WallTriggerCount ()) ? m_pWall [0]->Info ().nTrigger : -1;
	m_nType = m_pWall [0]->Type ();
	m_nClip = m_pWall [0]->Info ().nClip;
	m_nStrength = ((double) m_pWall [0]->Info ().hps) / F1_0;
	if (m_bFlyThrough = (m_nStrength < 0))
		m_nStrength = -m_nStrength;
	m_nCloak = ((double) (m_pWall [0]->Info ().cloakValue % 32)) * 100.0 / 31.0;
	CBWallNo ()->SetCurSel (m_nWall [0]);
	//CBType ()->SetCurSel (m_nType);
	SelectItemData (CBType (), m_nType);
	CBClipNo ()->EnableWindow ((m_nType == WALL_BLASTABLE) || (m_nType == WALL_DOOR));
	// select list box index for clip
	int i;
	for (i = 0; i < NUM_OF_CLIPS_D2; i++)
		if (animClipTable [i] == m_nClip)
			break;
	m_nClip = i;
	CBClipNo ()->SetCurSel ((i < NUM_OF_CLIPS_D2) ? i : 0);
	for (i = 0; i < MAX_WALL_FLAGS; i++)
		m_bFlags [i] = ((m_pWall [0]->Info ().flags & wallFlagTable [i]) != 0);
	for (i = 0; i < 4; i++)
		m_bKeys [i] = ((m_pWall [0]->Info ().keys & (1 << i)) != 0);
	if (!m_bLock) {
		m_defWall = *m_pWall [0];
		i = segmentManager.Segment (m_defWall.m_nSegment)->m_sides [m_defWall.m_nSide].Info ().nBaseTex;
		if (m_defWall.Type () == WALL_CLOAKED)
			m_defOvlTexture = i;
		else
			m_defTexture = i;
		}
	if (m_nType == WALL_DOOR) {
		memcpy (&m_defDoor, &m_defWall, sizeof (m_defDoor));
		m_defDoorTexture = m_defTexture;
		}
   }
GetDlgItem (IDC_WALL_BOTHSIDES)->EnableWindow (TRUE);
GetDlgItem (IDC_WALL_OTHERSIDE)->EnableWindow (TRUE);
GetDlgItem (IDC_WALL_WALLNO)->EnableWindow (wallManager.WallCount () > 0);
CTexToolDlg::Refresh ();
CToolDlg::EnableControls (IDC_WALL_DELETEALL, IDC_WALL_DELETEALL, wallManager.WallCount () > 0);
CBWallNo ()->SetCurSel (m_nWall [0]);
UpdateData (FALSE);
}

//------------------------------------------------------------------------------
// CWallTool - Add Wall
//------------------------------------------------------------------------------

void CWallTool::OnAddWall ()
{

CWall *pWall;
CSegment *pSegment [2];
CSide *pSide [2];
CSideKey keys [2];

bool bRefresh = false;

m_bDelayRefresh = true;
pSegment [0] = current->Segment ();
pSide [0] = current->Side ();
keys [0] = *current;
pSide [1] = segmentManager.BackSide (keys [0], keys [1]);
if (pSide [1] != null) 
	pSegment [1] = segmentManager.Segment (keys [1].m_nSegment);

for (BOOL bSide = FALSE; (bSide <= m_bBothSides) && pSide [bSide] != null; bSide++)
	if (pSide [bSide]->Info ().nWall < wallManager.WallCount ())
		ErrorMsg ("There is already a wall at that side of the current segment.");
	else if (wallManager.WallCount () >= MAX_WALLS)
		ErrorMsg ("The maximum number of walls is already reached.");
	else {
		if ((DLE.IsD2File ()) && (pSegment [bSide]->ChildId (keys [bSide].m_nSide) == -1))
			wallManager.Create (*current, WALL_OVERLAY, 0, KEY_NONE, -2, m_defOvlTexture);
		else if (pWall = wallManager.Create (keys [bSide], m_defWall.Type (), m_defWall.Info ().flags, m_defWall.Info ().keys, m_defWall.Info ().nClip, m_defTexture)) {
			if (pWall->Type () == m_defWall.Type ()) {
				pWall->Info ().hps = m_defWall.Info ().hps;
				pWall->Info ().cloakValue = m_defWall.Info ().cloakValue;
				}
			else if (pWall->Type () == WALL_CLOAKED) {
				pWall->Info ().hps = 0;
				pWall->Info ().cloakValue = 16;
				}
			else {
				pWall->Info ().hps = 0;
				pWall->Info ().cloakValue = 31;
				}
			}
			// update main window
		bRefresh = true;
		}
m_bDelayRefresh = false;
if (bRefresh) {
	DLE.MineView ()->Refresh ();
	Refresh ();
	}
}

//------------------------------------------------------------------------------
// CWallTool - Delete Wall
//------------------------------------------------------------------------------

void CWallTool::OnDeleteWall () 
{
	bool bRefresh = false;
	short nWall;

GetWalls ();
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++) {
	if (bSide)
		GetOtherWall ();
	nWall = m_nWall [bSide];
	if (nWall >= 0) {
		m_bDelayRefresh = true;
		wallManager.Delete ((ushort) nWall);
		m_bDelayRefresh = false;
		bRefresh = true;
		}
	else if (!DLE.ExpertMode ())
		if (wallManager.WallCount () == 0)
			ErrorMsg ("There are no walls in this mine.");
		else
			ErrorMsg ("There is no wall at this side of the current segment.");
	}
if (bRefresh) {
	DLE.MineView ()->Refresh ();
	Refresh ();
	}
}

//------------------------------------------------------------------------------
// CWallTool - Delete WallAll
//------------------------------------------------------------------------------

void CWallTool::OnDeleteWallAll () 
{
undoManager.Begin (__FUNCTION__, udWalls | udTriggers);
DLE.MineView ()->DelayRefresh (true);
CSegment *pSegment = segmentManager.Segment (0);
bool bAll = !segmentManager.HasTaggedSegments ();
int h, i, j, nDeleted = 0;
for (h = segmentManager.Count (), i = 0; i < h; i++, pSegment++) {
	CSide* pSide = pSegment->m_sides;
	for (j = 0; j < MAX_SIDES_PER_SEGMENT; j++, pSide++) {
		if (pSide->Info ().nWall >= MAX_WALLS)
			continue;
		if (bAll || segmentManager.IsTagged (CSideKey (i, j))) {
			wallManager.Delete (pSide->Info ().nWall);
			nDeleted++;
			}
		}
	}
DLE.MineView ()->DelayRefresh (false);
if (nDeleted) {
	undoManager.End (__FUNCTION__);
	DLE.MineView ()->Refresh ();
	Refresh ();
	}
else
	undoManager.Unroll (__FUNCTION__);
}

//------------------------------------------------------------------------------
// CWallTool - Other Side
//------------------------------------------------------------------------------

void CWallTool::OnOtherSide () 
{
DLE.MineView ()->SelectOtherSide ();
}

//------------------------------------------------------------------------------

CWall *CWallTool::GetOtherWall (void)
{
CSideKey opp;

if (!segmentManager.BackSide (opp))
	return m_pWall [1] = null;
m_nWall [1] = segmentManager.Segment (opp.m_nSegment)->m_sides [opp.m_nSide].Info ().nWall;
return m_pWall [1] = (m_nWall [1] < wallManager.WallCount () ? wallManager.Wall (m_nWall [1]) : null);
}

//------------------------------------------------------------------------------

bool CWallTool::GetWalls ()
{
m_nWall [0] = CBWallNo ()->GetCurSel ();
if (m_nWall [0] < 0) {
	m_nWall [1] = -1;
	m_pWall [0] =
	m_pWall [1] = null;
	return false;
	}
m_pWall [0] = wallManager.Wall (m_nWall [0]);
*((CSideKey *) current) = *((CSideKey *) m_pWall [0]);
m_nTrigger = m_pWall [0]->Info ().nTrigger;
GetOtherWall ();
return true;
}

//------------------------------------------------------------------------------

void CWallTool::OnSetWall ()
{
if (GetWalls ())
	DLE.MineView ()->Refresh ();
Refresh ();
}

//------------------------------------------------------------------------------

void CWallTool::OnSetType ()
{
	CSegment	*pSegment [2];
	CSide		*pSide [2];
	CWall		*pWall;
	CSideKey	keys [2];
	int		nType;

GetWalls ();
nType = int (CBType ()->GetItemData (CBType ()->GetCurSel ()));
if ((nType > WALL_CLOSED) && DLE.IsD1File ()) 
	return;
if ((nType > WALL_CLOAKED) && (DLE.IsStdLevel ())) 
	return;

m_defWall.Type () = m_nType = nType;
/*
m_nWall [0] = CBWallNo ()->GetCurSel ();
m_pWall [0] = wallManager.Wall (m_nWall [0]);
*/
pSegment [0] = current->Segment ();
pSide [0] = current->Side ();
keys [0] = *current;
if (segmentManager.BackSide (keys [0], keys [1])) {
	pSegment [1] = segmentManager.Segment (keys [1].m_nSegment);
	pSide [1] = pSegment [1]->m_sides + keys [1].m_nSide;
	}
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++)
	if ((pWall = m_pWall [bSide]) && pSide [bSide]) {
		short nBaseTex  = pSide [bSide]->Info ().nBaseTex;
		short nOvlTex = pSide [bSide]->Info ().nOvlTex;
		wallManager.Wall (m_nWall [bSide])->Setup (keys [bSide], m_nWall [bSide], m_nType, m_pWall [0]->Info ().nClip, -1, true);
		if ((pWall->Type () == WALL_OPEN) || (pWall->Type () == WALL_CLOSED))
			segmentManager.SetTextures (*pWall, nBaseTex, nOvlTex);
//		else if ((pWall->Type () == WALL_CLOAKED) || (pWall->Type () == WALL_COLORED))
//			pWall->Info ().cloakValue = m_defWall.cloakValue;
		}
DLE.MineView ()->Refresh ();
Refresh ();
}

//------------------------------------------------------------------------------

void CWallTool::OnSetClip ()
{
	int		nClip;
	CWall	*pWall;
/*
m_nWall [0] = CBWallNo ()->GetCurSel ();
m_pWall [0] = wallManager.Wall () + m_nWall [0];
*/
GetWalls ();
m_nClip = CBClipNo ()->GetCurSel ();
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++)
	if (pWall = m_pWall [bSide])
		if ((pWall->Type () == WALL_BLASTABLE) || (pWall->Type () == WALL_DOOR)) {
			if (m_nWall [bSide] < wallManager.WallCount ()) {
				undoManager.Begin (__FUNCTION__, udWalls);
				nClip = animClipTable [m_nClip];
				pWall->Info ().nClip = nClip;
				// define door textures based on clip number
				if (pWall->Info ().nClip >= 0)
					wallManager.Wall (m_nWall [bSide])->SetTextures (m_defTexture);
				undoManager.End (__FUNCTION__);
				DLE.MineView ()->Refresh ();
				Refresh ();
				}
			}
		else
			pWall->Info ().nClip = -1;
}

//------------------------------------------------------------------------------

void CWallTool::OnKey (int i) 
{
GetWalls ();
memset (m_bKeys, 0, sizeof (m_bKeys));
m_bKeys [i] = TRUE;
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++)
	if (m_pWall [bSide]) {
		undoManager.Begin (__FUNCTION__, udWalls);
		m_pWall [bSide]->Info ().keys = (1 << i);
		undoManager.End (__FUNCTION__);
		Refresh ();
		}
}

void CWallTool::OnFlag (int i) 
{
GetWalls ();
m_bFlags [i] = BtnCtrl (IDC_WALL_BLASTED + i)->GetCheck ();
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++)
	if (m_pWall [bSide]) {
		undoManager.Begin (__FUNCTION__, udWalls);
		if (m_bFlags [i])
			m_pWall [bSide]->Info ().flags |= wallFlagTable [i];
		else
			m_pWall [bSide]->Info ().flags &= ~wallFlagTable [i];
		undoManager.End (__FUNCTION__);
		Refresh ();
		}
}

void CWallTool::OnNoKey () { OnKey (0); }
void CWallTool::OnBlueKey () { OnKey (1); }
void CWallTool::OnGoldKey () { OnKey (2); }
void CWallTool::OnRedKey () { OnKey (3); }

void CWallTool::OnBlasted () { OnFlag (0); }
void CWallTool::OnDoorOpen () { OnFlag (1); }
void CWallTool::OnDoorLocked () { OnFlag (2); }
void CWallTool::OnDoorAuto () { OnFlag (3); }
void CWallTool::OnIllusionOff () { OnFlag (4); }
void CWallTool::OnSwitch () { OnFlag (5); }
void CWallTool::OnBuddyProof () { OnFlag (6); }
void CWallTool::OnRenderAdditive () { OnFlag (7); }
void CWallTool::OnIgnoreMarker () { OnFlag (8); }

//------------------------------------------------------------------------------

void CWallTool::OnStrength ()
{
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++)
	if (m_pWall [bSide]) {
		UpdateData (TRUE);
		undoManager.Begin (__FUNCTION__, udWalls);
		m_pWall [bSide]->Info ().hps = (int) m_nStrength * F1_0;
		if ((m_pWall [bSide]->Type () == WALL_COLORED) && m_bFlyThrough)
			m_pWall [bSide]->Info ().hps = -m_pWall [bSide]->Info ().hps;
		undoManager.End (__FUNCTION__);
		}
}

//------------------------------------------------------------------------------

void CWallTool::OnCloak ()
{
for (BOOL bSide = FALSE; bSide <= m_bBothSides; bSide++)
	if (m_pWall [bSide]) {
		UpdateData (TRUE);
		undoManager.Begin (__FUNCTION__, udWalls);
		m_defWall.Info ().cloakValue =
		m_pWall [bSide]->Info ().cloakValue = (char) (m_nCloak * 31.0 / 100.0) % 32;
		undoManager.End (__FUNCTION__);
		}
	else
		INFOMSG ("wall not found");
}

//------------------------------------------------------------------------------

void CWallTool::UpdateTransparency (int nValue)
{
if (nValue < 0)
	nValue = 0;
else if (nValue > 100)
	nValue = 10;
if (m_pWall [0]->Type () == WALL_COLORED) {
	m_nStrength = nValue;
	UpdateData (FALSE);
	OnStrength ();
	}
else {
	m_nCloak = nValue;
	UpdateData (FALSE);
	OnCloak ();
	}
}

//------------------------------------------------------------------------------

void CWallTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (!m_pWall [0])
	return;
#if 1
if (m_transpSlider.OnScroll (scrollCode, thumbPos, pScrollBar))
	UpdateTransparency (m_transpSlider.GetValue ());
else
	pScrollBar->SetScrollPos (thumbPos, TRUE);
#else
	int nPos;
if (pScrollBar == TransparencySlider ()) {
	nPos = pScrollBar->GetScrollPos ();
	switch (scrollCode) {
		case SB_LINEUP:
			nPos--;
			break;
		case SB_LINEDOWN:
			nPos++;
			break;
		case SB_PAGEUP:
			nPos -= 10;
			break;
		case SB_PAGEDOWN:
			nPos += 10;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos = thumbPos;
			break;
		case SB_ENDSCROLL:
			return;
		}
	//TransparencySpinner ()->SetPos (nPos);
	//pScrollBar->SetScrollPos (nPos, TRUE);
	}
else if (pScrollBar == (CScrollBar *) TransparencySpinner ()) {
	if (scrollCode != SB_THUMBPOSITION)
		return;
	nPos = thumbPos;
	//TransparencySlider ()->SetScrollPos (nPos);
	//UpdateData (FALSE);
	}
else {
	pScrollBar->SetScrollPos (thumbPos, TRUE);
	return;
	}
UpdateTransparency (nPos);
#endif
}

//------------------------------------------------------------------------------

void CWallTool::OnAddDoorNormal ()
{
wallManager.CreateAutoDoor (m_defDoor.Info ().nClip, m_defDoorTexture);
}

void CWallTool::OnAddDoorExit ()
{
wallManager.CreateNormalExit ();
}

void CWallTool::OnAddDoorSecretExit ()
{
wallManager.CreateSecretExit ();
}

void CWallTool::OnAddDoorPrison ()
{
wallManager.CreatePrisonDoor ();
}

void CWallTool::OnAddDoorGuideBot ()
{
wallManager.CreateGuideBotDoor ();
}

void CWallTool::OnAddWallFuelCell ()
{
wallManager.CreateFuelCell ();
}

void CWallTool::OnAddWallIllusion ()
{
wallManager.CreateIllusion ();
}

void CWallTool::OnAddWallForceField ()
{
wallManager.CreateForceField ();
}

void CWallTool::OnAddWallFan ()
{
wallManager.CreateFan ();
}

void CWallTool::OnAddWallGrate ()
{
wallManager.CreateGrate ();
}

void CWallTool::OnAddWallWaterfall ()
{
wallManager.CreateWaterFall ();
}

void CWallTool::OnAddWallLavafall ()
{
wallManager.CreateLavaFall ();
}

void CWallTool::OnBothSides ()
{
m_bBothSides = BtnCtrl (IDC_WALL_BOTHSIDES)->GetCheck ();
}

//eof walldlg.cpp
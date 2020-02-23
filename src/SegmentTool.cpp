
#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#undef abs
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>

#include "mine.h"
#include "dle-xp.h"

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CSegmentTool, CToolDlg)
	ON_WM_VSCROLL ()
	ON_BN_CLICKED (IDC_SEGMENT_SETCOORD, OnSetCoord)
	ON_BN_CLICKED (IDC_SEGMENT_RESETCOORD, OnResetCoord)
	ON_BN_CLICKED (IDC_SEGMENT_ADD, OnAddSegment)
	ON_BN_CLICKED (IDC_SEGMENT_ADD_MATCEN, OnAddRobotMaker)
	ON_BN_CLICKED (IDC_SEGMENT_ADD_EQUIPMAKER, OnAddEquipMaker)
	ON_BN_CLICKED (IDC_SEGMENT_ADD_PRODUCER, OnAddProducer)
	ON_BN_CLICKED (IDC_SEGMENT_ADD_REPAIRCEN, OnAddRepairCenter)
	ON_BN_CLICKED (IDC_SEGMENT_ADD_CONTROLCEN, OnAddReactor)
	ON_BN_CLICKED (IDC_SEGMENT_SPLIT_IN_7, OnSplitSegmentIn7)
	ON_BN_CLICKED (IDC_SEGMENT_SPLIT_IN_8, OnSplitSegmentIn8)
	ON_BN_CLICKED (IDC_SEGMENT_CREATE_WEDGE, OnCreateWedge)
	ON_BN_CLICKED (IDC_SEGMENT_CREATE_PYRAMID, OnCreatePyramid)
	ON_BN_CLICKED (IDC_SEGMENT_DEL, OnDeleteSegment)
	ON_BN_CLICKED (IDC_SEGMENT_OTHER, OnOtherSegment)
	ON_BN_CLICKED (IDC_SEGMENT_ENDOFEXIT, OnEndOfExit)
	ON_BN_CLICKED (IDC_SEGMENT_WATER, OnProp1)
	ON_BN_CLICKED (IDC_SEGMENT_LAVA, OnProp2)
	ON_BN_CLICKED (IDC_SEGMENT_BLOCKED, OnProp3)
	ON_BN_CLICKED (IDC_SEGMENT_NODAMAGE, OnProp4)
	ON_BN_CLICKED (IDC_SEGMENT_OUTDOORS, OnProp5)
	ON_BN_CLICKED (IDC_SEGMENT_LIGHT_FOG, OnProp6)
	ON_BN_CLICKED (IDC_SEGMENT_DENSE_FOG, OnProp7)
	ON_BN_CLICKED (IDC_SEGMENT_SIDE1, OnSide1)
	ON_BN_CLICKED (IDC_SEGMENT_SIDE2, OnSide2)
	ON_BN_CLICKED (IDC_SEGMENT_SIDE3, OnSide3)
	ON_BN_CLICKED (IDC_SEGMENT_SIDE4, OnSide4)
	ON_BN_CLICKED (IDC_SEGMENT_SIDE5, OnSide5)
	ON_BN_CLICKED (IDC_SEGMENT_SIDE6, OnSide6)
	ON_BN_CLICKED (IDC_SEGMENT_POINT1, OnPoint1)
	ON_BN_CLICKED (IDC_SEGMENT_POINT2, OnPoint2)
	ON_BN_CLICKED (IDC_SEGMENT_POINT3, OnPoint3)
	ON_BN_CLICKED (IDC_SEGMENT_POINT4, OnPoint4)
	ON_BN_CLICKED (IDC_SEGMENT_ADDBOT, OnAddObjectToProducer)
	ON_BN_CLICKED (IDC_SEGMENT_DELBOT, OnDeleteObjectFromProducer)
	ON_BN_CLICKED (IDC_SEGMENT_TRIGGERDETAILS, OnTriggerDetails)
	ON_BN_CLICKED (IDC_SEGMENT_WALLDETAILS, OnWallDetails)
	ON_CBN_SELCHANGE (IDC_SEGMENT_ID, OnSetSegment)
	ON_CBN_SELCHANGE (IDC_SEGMENT_FUNCTION, OnSetType)
	ON_CBN_SELCHANGE (IDC_SEGMENT_OWNER, OnSetOwner)
	ON_EN_KILLFOCUS (IDC_SEGMENT_LIGHT, OnLight)
	ON_EN_KILLFOCUS (IDC_SEGMENT_SHIELD_DAMAGE, OnDamage0)
	ON_EN_KILLFOCUS (IDC_SEGMENT_ENERGY_DAMAGE, OnDamage1)
	ON_EN_KILLFOCUS (IDC_SEGMENT_GROUP, OnSetGroup)
	ON_EN_UPDATE (IDC_SEGMENT_LIGHT, OnLight)
	ON_LBN_DBLCLK (IDC_SEGMENT_TRIGGERS, OnTriggerDetails)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------------

CSegmentTool::CSegmentTool (CPropertySheet *pParent)
	: CToolDlg (IDD_SEGMENTDATA_HORZ + nLayout, pParent)
{
Reset ();
}

//------------------------------------------------------------------------------

void CSegmentTool::Reset ()
{
CHECKMINE;

m_nSegment =
m_nSide =
m_nPoint = 0;
m_nFunction = 0;
m_nProps = 0;
m_nVertex = 0;
m_bEndOfExit = 0;
m_nDamage [0] =
m_nDamage [1] = 0;
m_nLight = 0;
m_nLastSegment =
m_nLastSide = -1;
m_bSetDefTexture = 0;
m_nOwner = segmentManager.Segment (0)->m_info.owner;
m_nGroup = segmentManager.Segment (0)->m_info.group;
memset (m_nCoord, 0, sizeof (m_nCoord));
}

//------------------------------------------------------------------------------

void CSegmentTool::InitCBSegmentId (void)
{
CHECKMINE;
CComboBox *pcb = CBSegmentId ();
if (segmentManager.Count () != pcb->GetCount ()) {
	pcb->ResetContent ();
	for (int i = 0; i < segmentManager.Count (); i++) {
		_itoa_s (i, message, sizeof (message), 10);
		pcb->AddString (message);
		}
	SegIdSpinner ()->SetRange (0, segmentManager.Count () - 1);
	}
SegIdSpinner ()->SetPos (m_nSegment);
pcb->SetCurSel (m_nSegment);
}

//------------------------------------------------------------------------------

typedef struct tSegFuncDescr {
	char* szName;
	int	bStandard;
} tSegFuncDescr;

static tSegFuncDescr segmentFuncs [] = {
	{"None", 1},
	{"Fuel Center", 1},
	{"Repair Center", 0},
	{"Reactor", 1},
	{"Robot Maker", 1},
	{"Blue Goal", 1},
	{"Red Goal", 1},
	{"Blue Team", 0},
	{"Red Team", 0},
	{"Speed Boost", 0},
	{"Sky Box", 0},
	{"Equip Maker", 0}
	};

BOOL CSegmentTool::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
	return FALSE;


CComboBox *pcb = CBFunction ();
pcb->ResetContent ();

int h, i, j = sizeof (segmentFuncs) / sizeof (*segmentFuncs);

SegIdSpinner ()->SetRange (0, segmentManager.Count () - 1);
SegFuncSpinner ()->SetRange (0, j - 1);

for (i = 0; i < j; i++) {
	h = pcb->AddString (segmentFuncs [i].szName);
	pcb->SetItemData (h, i);
	}
pcb = CBOwner ();
pcb->ResetContent ();

pcb->AddString ("Neutral");
pcb->AddString ("Unowned");
pcb->AddString ("Blue Team");
pcb->AddString ("Red Team");

m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------------

void CSegmentTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;

DDX_CBIndex (pDX, IDC_SEGMENT_ID, m_nSegment);
//DDX_CBIndex (pDX, IDC_SEGMENT_FUNCTION, m_nFunction);
SelectItemData (CBFunction (), m_nFunction);
DDX_Double (pDX, IDC_SEGMENT_LIGHT, m_nLight);
DDX_Radio (pDX, IDC_SEGMENT_SIDE1, m_nSide);
DDX_Radio (pDX, IDC_SEGMENT_POINT1, m_nPoint);
for (int	i = 0; i < 3; i++) {
	DDX_Double (pDX, IDC_SEGMENT_POINTX + i, m_nCoord [i]);
	if (m_nCoord [i] < -0x7fff)
		m_nCoord [i] = -0x7fff;
	else if (m_nCoord [i] > 0x7fff)
		m_nCoord [i] = 0x7fff;
//	DDV_MinMaxInt (pDX, (int) m_nCoord [i], -0x7fff, 0x7fff);
	}

int i;

for (i = 0; i < 7; i++) {
	int h = (m_nProps & (1 << i)) != 0;
	DDX_Check (pDX, IDC_SEGMENT_WATER + i, h);
	if (h)
		m_nProps |= (1 << i);
	else
		m_nProps &= ~(1 << i);
	}

DDX_Check (pDX, IDC_SEGMENT_ENDOFEXIT, m_bEndOfExit);
DDX_Check (pDX, IDC_SEGMENT_SETDEFTEXTURE, m_bSetDefTexture);
++m_nOwner;
DDX_CBIndex (pDX, IDC_SEGMENT_OWNER, m_nOwner);
m_nOwner--;
DDX_Text (pDX, IDC_SEGMENT_GROUP, m_nGroup);
if (m_nGroup < -1)
	m_nGroup = -1;
else if (m_nGroup > 127)
	m_nGroup = 127;
for (i = 0; i < 2; i++)
	DDX_Text (pDX, IDC_SEGMENT_SHIELD_DAMAGE + i, m_nDamage [i]);

//DDV_MinMaxInt (pDX, m_nGroup, -1, 127);
}


//------------------------------------------------------------------------------

BOOL CSegmentTool::OnSetActive ()
{
Refresh ();
return CToolDlg::OnSetActive ();
}

//------------------------------------------------------------------------------

bool CSegmentTool::IsRobotMaker (CSegment *pSegment)
{
return 
	(pSegment->m_info.function == SEGMENT_FUNC_ROBOTMAKER) &&
	(pSegment->m_info.nProducer >= 0) &&
	(pSegment->m_info.nProducer < segmentManager.RobotMakerCount ());
}

//------------------------------------------------------------------------------

bool CSegmentTool::IsEquipMaker (CSegment *pSegment)
{
return 
	(pSegment->m_info.function == SEGMENT_FUNC_EQUIPMAKER) &&
	(pSegment->m_info.nProducer >= 0) &&
	(pSegment->m_info.nProducer < segmentManager.EquipMakerCount ());
}

//------------------------------------------------------------------------------

void CSegmentTool::EnableControls (BOOL bEnable)
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
// enable/disable "end of exit tunnel" button
EndOfExit ()->EnableWindow (pSegment->ChildId (m_nSide) < 0);
// enable/disable add segment button
GetDlgItem (IDC_SEGMENT_ADD)->EnableWindow ((segmentManager.Count () < SEGMENT_LIMIT) &&
													  (vertexManager.Count () < VERTEX_LIMIT - 4) &&
													  (pSegment->ChildId (m_nSide) < 0));
GetDlgItem (IDC_SEGMENT_DEL)->EnableWindow (segmentManager.Count () > 1);
// enable/disable add robot button
GetDlgItem (IDC_SEGMENT_ADDBOT)->EnableWindow ((IsRobotMaker (pSegment) || IsEquipMaker (pSegment)) && (LBAvailBots ()->GetCount () > 0));
GetDlgItem (IDC_SEGMENT_DELBOT)->EnableWindow ((IsRobotMaker (pSegment) || IsEquipMaker (pSegment)) && (LBUsedBots ()->GetCount () > 0));
GetDlgItem (IDC_SEGMENT_WALLDETAILS)->EnableWindow (LBTriggers ()->GetCount () > 0);
GetDlgItem (IDC_SEGMENT_TRIGGERDETAILS)->EnableWindow (LBTriggers ()->GetCount () > 0);
GetDlgItem (IDC_SEGMENT_ADD_REPAIRCEN)->EnableWindow (DLE.IsD2File ());
GetDlgItem (IDC_SEGMENT_OWNER)->EnableWindow (DLE.IsD2XLevel ());
GetDlgItem (IDC_SEGMENT_GROUP)->EnableWindow (DLE.IsD2XLevel ());
}

//------------------------------------------------------------------------------

void CSegmentTool::OnSetCoord (void)
{
CHECKMINE;
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udVertices);
current->Segment ()->Vertex (current->Side ()->VertexIdIndex (current->Point ()))->Set (m_nCoord [0], m_nCoord [1], m_nCoord [2]);
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh (false);
}

//------------------------------------------------------------------------------

void CSegmentTool::OnResetCoord (void)
{
CHECKMINE;
CVertex* pVertex = current->Segment ()->Vertex (current->Side ()->VertexIdIndex (current->Point ()));
m_nCoord [0] = pVertex->v.x;
m_nCoord [1] = pVertex->v.y;
m_nCoord [2] = pVertex->v.z;
UpdateData (FALSE);
DLE.MineView ()->Refresh (false);
}

//------------------------------------------------------------------------------

void CSegmentTool::OnProp (int nProp)
{
CHECKMINE;
if (Prop (nProp)->GetCheck ())
	m_nProps |= 1 << nProp;
else
	m_nProps &= ~(1 << nProp);

BOOL bTagged = segmentManager.HasTaggedSegments ();

undoManager.Begin (__FUNCTION__, udSegments);
DLE.MineView ()->DelayRefresh (true);
UpdateData (TRUE);
if (bTagged) {
	CSegment *pSegment = segmentManager.Segment (0);
	for (short nSegNum = 0; nSegNum < segmentManager.Count (); nSegNum++, pSegment++)
		if (pSegment->IsTagged ())
			pSegment->m_info.props = m_nProps;
	}
else 					
	current->Segment ()->m_info.props = m_nProps;
DLE.MineView ()->DelayRefresh (false);
undoManager.End (__FUNCTION__);
}

void CSegmentTool::OnProp1 () { OnProp (0); }
void CSegmentTool::OnProp2 () { OnProp (1); }
void CSegmentTool::OnProp3 () { OnProp (2); }
void CSegmentTool::OnProp4 () { OnProp (3); }
void CSegmentTool::OnProp5 () { OnProp (4); }
void CSegmentTool::OnProp6 () { OnProp (5); }
void CSegmentTool::OnProp7 () { OnProp (6); }

//------------------------------------------------------------------------------

void CSegmentTool::OnSide (int nSide)
{
CHECKMINE;
current->SetSideId (m_nSide = nSide);
DLE.MineView ()->Refresh ();
}

void CSegmentTool::OnSide1 () { OnSide (0); }
void CSegmentTool::OnSide2 () { OnSide (1); }
void CSegmentTool::OnSide3 () { OnSide (2); }
void CSegmentTool::OnSide4 () { OnSide (3); }
void CSegmentTool::OnSide5 () { OnSide (4); }
void CSegmentTool::OnSide6 () { OnSide (5); }

//------------------------------------------------------------------------------

void CSegmentTool::OnPoint (int nPoint)
{
CHECKMINE;
current->SetPoint (m_nPoint = nPoint);
DLE.MineView ()->Refresh ();
}

void CSegmentTool::OnPoint1 () { OnPoint (0); }
void CSegmentTool::OnPoint2 () { OnPoint (1); }
void CSegmentTool::OnPoint3 () { OnPoint (2); }
void CSegmentTool::OnPoint4 () { OnPoint (3); }

//------------------------------------------------------------------------------

void CSegmentTool::SetDefTexture (short nTexture)
{
CSegment *pSegment = segmentManager.Segment (m_nSegment);
if (m_bSetDefTexture = ((CButton *) GetDlgItem (IDC_SEGMENT_SETDEFTEXTURE))->GetCheck ()) {
	int i;
	for (i = 0; i < 6; i++)
		if (pSegment->ChildId (i) == -1)
			segmentManager.SetTextures (CSideKey (m_nSegment, i), nTexture, 0);
	}
}

//------------------------------------------------------------------------
// CSegmentTool - RefreshData
//------------------------------------------------------------------------

void CSegmentTool::Refresh (void) 
{
if (!(m_bInited && theMine))
	return;
InitCBSegmentId ();
OnResetCoord ();

int h, i, j;

// update automatic data
// update segment number combo box if number of segments has changed
CSegment *pSegment = current->Segment ();
m_bEndOfExit = (pSegment->ChildId (current->SideId ()) == -2);
m_nSegment = current->SegmentId ();
m_nSide = current->SideId ();
m_nPoint = current->Point ();
m_nFunction = pSegment->m_info.function;
m_nDamage [0] = pSegment->m_info.damage [0];
m_nDamage [1] = pSegment->m_info.damage [1];
m_nProps = pSegment->m_info.props;
m_nOwner = pSegment->m_info.owner;
m_nGroup = pSegment->m_info.group;
//CBFunction ()->SetCurSel (m_nFunction);
SelectItemData (CBFunction (), m_nFunction);
SegFuncSpinner ()->SetPos (m_nFunction);
OnResetCoord ();
  // show Triggers () that point at this segment
LBTriggers()->ResetContent();
CTrigger *pTrigger = triggerManager.Trigger (0);
int nTrigger;
for (nTrigger = 0; nTrigger < triggerManager.WallTriggerCount (); nTrigger++, pTrigger++) {
	for (i = 0; i < pTrigger->Count (); i++) {
		if ((*pTrigger) [i] == CSideKey (m_nSegment, m_nSide)) {
			// find the pWall with this pTrigger
			CWall *pWall = wallManager.Wall (0);
			int nWall;
			for (nWall = 0; nWall < wallManager.WallCount (); nWall++, pWall++) {
				if (pWall->Info ().nTrigger == nTrigger) 
					break;
				}
			if (nWall < wallManager.WallCount ()) {
				sprintf_s (message, sizeof (message),  "%d,%d", (int) pWall->m_nSegment, (int) pWall->m_nSide + 1);
				h = LBTriggers ()->AddString (message);
				LBTriggers ()->SetItemData (h, (int) pWall->m_nSegment * 0x10000L + pWall->m_nSide);
				}
			}
		}
	}
// show if this is segment/side is trigPed by the control_center
CReactorTrigger* reactorTrigger = triggerManager.ReactorTrigger (0);
for (short nTrigger = 0; nTrigger < MAX_REACTOR_TRIGGERS; nTrigger++, reactorTrigger++) {
	if (-1 < (reactorTrigger->Find (CSideKey (m_nSegment, m_nSide)))) {
		LBTriggers ()->AddString ("Reactor");
		break;
		}
	}

// show "none" if there is no Triggers ()
if (!LBTriggers()->GetCount()) {
	LBTriggers()->AddString ("none");
	}

m_nLight = ((double) pSegment->m_info.staticLight) / (24 * 327.68);

CListBox *plb [2] = { LBAvailBots (), LBUsedBots () };
if (IsRobotMaker (pSegment)) {
	int nProducer = pSegment->m_info.nProducer;
	// if # of items in list box totals to less than the number of robots
	//    if (LBAvailBots ()->GetCount() + LBAvailBots ()->GetCount() < MAX_ROBOT_IDS) {
	
	int objFlags [2];
	CStringResource res;

	for (i = 0; i < 2; i++)
		objFlags [i] = segmentManager.RobotMaker (nProducer)->m_info.objFlags [i];
	if ((m_nLastSegment != m_nSegment) || (m_nLastSide != m_nSide)) {
		for (i = 0; i < 2; i++) {
			plb [i]->ResetContent ();
			int n = DLE.IsD1File () ? N_ROBOT_TYPES_D1 : 64;
			for (j = 0; j < n; j++) {
				if (i) {
					 h = ((objFlags [j / 32] & (1L << (j % 32))) != 0);
					if (!h)	//only add flagged objects to 2nd list box
						continue;
					}
				res.Clear ();
				res.Load (ROBOT_STRING_TABLE + j);
				h = plb [i]->AddString (res.Value ());
				plb [i]->SetItemData (h, j);
				}
			plb [i]->SetCurSel (0);
			}
		}
	}
else if (IsEquipMaker (pSegment)) {
	int nProducer = pSegment->m_info.nProducer;
	// if # of items in list box totals to less than the number of robots
	//    if (LBAvailBots ()->GetCount() + LBAvailBots ()->GetCount() < MAX_ROBOT_IDS) {
	int objFlags [2];
	CStringResource res;

	for (i = 0; i < 2; i++)
		objFlags [i] = segmentManager.EquipMaker (nProducer)->m_info.objFlags [i];
	if ((m_nLastSegment != m_nSegment) || (m_nLastSide != m_nSide)) {
		for (i = 0; i < 2; i++) {
			plb [i]->ResetContent ();
			int n = DLE.IsD1File () ? MAX_POWERUP_IDS_D1 : MAX_POWERUP_IDS_D2;
			for (j = 0; j < n; j++) {
				if (i) {
					h = ((objFlags [j / 32] & (1L << (j % 32))) != 0);
					if (!h)	//only add flagged objects to 2nd list box
						continue;
					}
				res.Clear ();
				res.Load (POWERUP_STRING_TABLE + j);
				if (!strcmp (res.Value (), "(not used)"))
					continue;
				h = plb [i]->AddString (res.Value ());
				plb [i]->SetItemData (h, j);
				}
			plb [i]->SetCurSel (0);
			}
		}
	}
else {
	sprintf_s (message, sizeof (message), "n/a");
	for (i = 0; i < 2; i++) {
		plb [i]->ResetContent();
		plb [i]->AddString(message);
		}
	}
m_nLastSegment = m_nSegment;
m_nLastSide = m_nSide;
EnableControls (TRUE);
UpdateData (FALSE);
}


//------------------------------------------------------------------------
// CSegmentTool - EndOfExitTunnel ()
//------------------------------------------------------------------------

void CSegmentTool::OnEndOfExit ()
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
undoManager.Begin (__FUNCTION__, udSegments);
m_bEndOfExit = EndOfExit ()->GetCheck ();
pSegment->SetChild (m_nSide, m_bEndOfExit ? -2 : -1);
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------
// CSegmentTool - Add Segment
//------------------------------------------------------------------------

void CSegmentTool::OnAddSegment () 
{
CHECKMINE;
segmentManager.AddSegments ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CSegmentTool - Delete Segment
//------------------------------------------------------------------------

void CSegmentTool::OnDeleteSegment () 
{
CHECKMINE;
segmentManager.Delete (current->SegmentId ());
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSegmentTool::OnSetOwner (void)
{
CHECKMINE;

	BOOL	bChangeOk = TRUE;
	BOOL	bTagged = segmentManager.HasTaggedSegments ();

undoManager.Begin (__FUNCTION__, udSegments);
DLE.MineView ()->DelayRefresh (true);
UpdateData (TRUE);
if (bTagged) {
	CSegment *pSegment = segmentManager.Segment (0);
	for (short nSegNum = 0; nSegNum < segmentManager.Count (); nSegNum++, pSegment++)
		if (pSegment->IsTagged ())
			pSegment->m_info.owner = m_nOwner;
	}
else 					
	current->Segment ()->m_info.owner = m_nOwner;
undoManager.End (__FUNCTION__);
DLE.MineView ()->DelayRefresh (false);
}

//------------------------------------------------------------------------

void CSegmentTool::OnSetGroup (void)
{
CHECKMINE;

	BOOL	bChangeOk = TRUE;
	BOOL	bTagged = segmentManager.HasTaggedSegments ();

undoManager.Begin (__FUNCTION__, udSegments);
DLE.MineView ()->DelayRefresh (true);
UpdateData (TRUE);
if (bTagged) {
	CSegment *pSegment = segmentManager.Segment (0);
	for (short nSegNum = 0; nSegNum < segmentManager.Count (); nSegNum++, pSegment++)
		if (pSegment->IsTagged ())
			pSegment->m_info.group = m_nGroup;
	}
else 					
	current->Segment ()->m_info.group = m_nGroup;
undoManager.End (__FUNCTION__);
DLE.MineView ()->DelayRefresh (false);
}

//------------------------------------------------------------------------
// CSegmentTool - SpecialMsg
//------------------------------------------------------------------------

void CSegmentTool::OnSetType (void)
{
CHECKMINE;

ubyte nNewFunction = ubyte (CBFunction ()->GetItemData (CBFunction ()->GetCurSel ()));
if (DLE.IsStdLevel () && !segmentFuncs [nNewFunction].bStandard) {
	if (!DLE.ExpertMode ())
		ErrorMsg ("Convert the level to a D2X-XL level to use this segment type.");
	SelectItemData (CBFunction (), m_nFunction);
	SegFuncSpinner ()->SetPos (m_nFunction);
	return;
	}

	BOOL		bChangeOk = TRUE;
	BOOL		bTagged = segmentManager.HasTaggedSegments ();
	int		nSegment, nMinSeg, nMaxSeg;

DLE.MineView ()->DelayRefresh (true);
m_nLastSegment = -1; //force Refresh() to rebuild all dialog data
SegFuncSpinner ()->SetPos (nNewFunction);
if (bTagged) {
	nMinSeg = 0;
	nMaxSeg = segmentManager.Count ();
	}
else {
	nMinSeg = int (current->Segment () - segmentManager.Segment (0));
	nMaxSeg = nMinSeg + 1;
	}
undoManager.Begin (__FUNCTION__, udSegments);
CSegment* pSegment = segmentManager.Segment (nMinSeg);
for (nSegment = nMinSeg; nSegment < nMaxSeg; nSegment++, pSegment++) {
	if (bTagged && !pSegment->IsTagged ())
		continue;
	if (nNewFunction == pSegment->m_info.function)
		continue;
	//m_nFunction = pSegment->m_info.function;
	switch (nNewFunction) {
		// check to see if we are adding a robot maker
		case SEGMENT_FUNC_ROBOTMAKER:
			if (!segmentManager.CreateRobotMaker (nSegment, false, m_bSetDefTexture == 1)) {
				undoManager.Unroll (__FUNCTION__);
				goto funcExit;
				}
			Refresh ();
			break;

		// check to see if we are adding a fuel center
		case SEGMENT_FUNC_REPAIRCEN:
		case SEGMENT_FUNC_PRODUCER:
			if (!segmentManager.CreateProducer (nSegment, nNewFunction, false, (nNewFunction == SEGMENT_FUNC_PRODUCER) && (m_bSetDefTexture == 1))) {
				undoManager.Unroll (__FUNCTION__);
				goto funcExit;
				}
			break;

		case SEGMENT_FUNC_REACTOR:
			if (!segmentManager.CreateReactor (nSegment, false, m_bSetDefTexture == 1)) {
				undoManager.Unroll (__FUNCTION__);
				goto funcExit;
				}
			break;

		case SEGMENT_FUNC_GOAL_BLUE:
		case SEGMENT_FUNC_GOAL_RED:
			if (!segmentManager.CreateGoal (nSegment, false, m_bSetDefTexture == 1, nNewFunction, -1))
				goto errorExit;		
			break;

		case SEGMENT_FUNC_TEAM_BLUE:
		case SEGMENT_FUNC_TEAM_RED:
			if (!segmentManager.CreateTeam (nSegment, false, false, nNewFunction, -1))
				goto errorExit;		
			break;

		case SEGMENT_FUNC_SPEEDBOOST:
			if (!segmentManager.CreateSpeedBoost (nSegment, false))
				goto errorExit;
			break;

		case SEGMENT_FUNC_SKYBOX:
			if (!segmentManager.CreateSkybox (nSegment, false))
				goto errorExit;
			break;

		case SEGMENT_FUNC_EQUIPMAKER:
			if (!segmentManager.CreateEquipMaker (nSegment, false))
				goto errorExit;
			Refresh ();
			break;

		case SEGMENT_FUNC_NONE:
			segmentManager.Undefine (nSegment);
			break;

		default:
			break;
		}
	m_nFunction = nNewFunction;
	}

errorExit:

undoManager.End (__FUNCTION__);
triggerManager.UpdateReactor ();

funcExit:

DLE.MineView ()->DelayRefresh (false);
UpdateData (TRUE);
}

//------------------------------------------------------------------------
// CSegmentTool - Segment Number Message
//------------------------------------------------------------------------

void CSegmentTool::OnSetSegment () 
{
CHECKMINE;
current->SetSegmentId (CBSegmentId ()->GetCurSel ());
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------
// CSegmentTool - LightMsg
//------------------------------------------------------------------------

void CSegmentTool::OnLight () 
{
CHECKMINE;
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
current->Segment ()->m_info.staticLight = (int) (m_nLight * 24 * 327.68);
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

void CSegmentTool::OnDamage (int i) 
{
CHECKMINE;
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udSegments);
current->Segment ()->m_info.damage [i] = m_nDamage [i];
undoManager.End (__FUNCTION__);
}

void CSegmentTool::OnDamage0 () { OnDamage (0); }
void CSegmentTool::OnDamage1 () { OnDamage (1); }

//------------------------------------------------------------------------------

int CSegmentTool::FindObjectInRobotMaker (CListBox *plb, LPSTR pszObj)
{
	int i, j;

i = plb->GetCurSel ();
if (i < 0)
	return -1;
j = int (plb->GetItemData (i));
if (pszObj)
	LoadString (AfxGetInstanceHandle(), ROBOT_STRING_TABLE + j, pszObj, 80);
return j;
}

//------------------------------------------------------------------------------

int CSegmentTool::FindObjectInEquipMaker (CListBox *plb, LPSTR pszObj)
{
	int i, j;

i = plb->GetCurSel ();
if (i < 0)
	return -1;
j = int (plb->GetItemData (i));
if (pszObj)
	LoadString (AfxGetInstanceHandle(), POWERUP_STRING_TABLE + j, pszObj, 80);
return j;
}

//------------------------------------------------------------------------

void CSegmentTool::AddObjectToRobotMaker ()
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
char szObj [80];
int i = FindObjectInRobotMaker (LBAvailBots (), szObj);
if ((i < 0) || (i >= (DLE.IsD1File () ? N_ROBOT_TYPES_D1 : 64)))
	return;
undoManager.Begin (__FUNCTION__, udProducers);
segmentManager.RobotMaker (pSegment->m_info.nProducer)->m_info.objFlags [i / 32] |= (1L << (i % 32));
undoManager.End (__FUNCTION__);
int h = LBAvailBots ()->GetCurSel ();
LBAvailBots ()->DeleteString (h);
LBAvailBots ()->SetCurSel (h);
h = LBUsedBots ()->AddString (szObj);
LBUsedBots ()->SetItemData (h, i);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSegmentTool::AddObjectToEquipMaker ()
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
char szObj [80];
int i = FindObjectInEquipMaker (LBAvailBots (), szObj);
if ((i < 0) || (i >= (DLE.IsD1File () ? MAX_POWERUP_IDS_D1 : MAX_POWERUP_IDS_D2)))
	return;
undoManager.Begin (__FUNCTION__, udProducers);
segmentManager.EquipMaker (pSegment->m_info.nProducer)->m_info.objFlags [i / 32] |= (1L << (i % 32));
undoManager.End (__FUNCTION__);
int h = LBAvailBots ()->GetCurSel ();
LBAvailBots ()->DeleteString (h);
LBAvailBots ()->SetCurSel (h);
h = LBUsedBots ()->AddString (szObj);
LBUsedBots ()->SetItemData (h, i);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSegmentTool::OnAddObjectToProducer ()
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
if (IsRobotMaker (pSegment))
	AddObjectToRobotMaker ();
else if (IsEquipMaker (pSegment))
	AddObjectToEquipMaker ();
}

//------------------------------------------------------------------------

void CSegmentTool::DeleteObjectFromRobotMaker () 
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
char szObj [80];
int i = FindObjectInRobotMaker (LBUsedBots (), szObj);
if ((i < 0) || (i >= 64))
	return;
undoManager.Begin (__FUNCTION__, udProducers);
segmentManager.RobotMaker (pSegment->m_info.nProducer)->m_info.objFlags [i / 32] &= ~(1L << (i % 32));
undoManager.End (__FUNCTION__);
int h = LBUsedBots ()->GetCurSel ();
LBUsedBots ()->DeleteString (h);
LBUsedBots ()->SetCurSel (h);
h = LBAvailBots ()->AddString (szObj);
LBAvailBots ()->SetItemData (h, i);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSegmentTool::DeleteObjectFromEquipMaker () 
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
char szObj [80];
int i = FindObjectInEquipMaker (LBUsedBots (), szObj);
if ((i < 0) || (i >= 64))
	return;
undoManager.Begin (__FUNCTION__, udProducers);
segmentManager.EquipMaker (pSegment->m_info.nProducer)->m_info.objFlags [i / 32] &= ~(1L << (i % 32));
undoManager.End (__FUNCTION__);
int h = LBUsedBots ()->GetCurSel ();
LBUsedBots ()->DeleteString (h);
LBUsedBots ()->SetCurSel (h);
h = LBAvailBots ()->AddString (szObj);
LBAvailBots ()->SetItemData (h, i);
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSegmentTool::OnDeleteObjectFromProducer () 
{
CHECKMINE;
CSegment *pSegment = current->Segment ();
if (IsRobotMaker (pSegment))
	DeleteObjectFromRobotMaker ();
else if (IsEquipMaker (pSegment))
	DeleteObjectFromEquipMaker ();
}

//------------------------------------------------------------------------

void CSegmentTool::OnOtherSegment () 
{
DLE.MineView ()->SelectOtherSegment ();
}

//------------------------------------------------------------------------

void CSegmentTool::OnWallDetails () 
{
CHECKMINE;
if (!LBTriggers ()->GetCount ())
	return;
int i = LBTriggers ()->GetCurSel ();
if (i < 0)
	return;
long h = long (LBTriggers ()->GetItemData (i));
current->SetSegmentId (short (h / 0x10000L));
current->SetSideId (short (h % 0x10000L));
DLE.ToolView ()->EditWall ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSegmentTool::OnTriggerDetails ()
{
CHECKMINE;
if (!LBTriggers ()->GetCount ())
	return;
int i = LBTriggers ()->GetCurSel ();
if ((i < 0) || (i >= LBTriggers ()->GetCount ()))
	return;
long h = long (LBTriggers ()->GetItemData (i));
other->m_nSegment = current->SegmentId ();
other->m_nSide = current->SideId ();
current->SetSegmentId (short (h / 0x10000L));
current->SetSideId (short (h % 0x10000L));
DLE.ToolView ()->EditTrigger ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnAddRobotMaker ()
{
CHECKMINE;
segmentManager.CreateRobotMaker ();
m_nLastSegment = -1;
Refresh ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnAddEquipMaker ()
{
CHECKMINE;
segmentManager.CreateEquipMaker ();
m_nLastSegment = -1;
Refresh ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnAddProducer ()
{
CHECKMINE;
segmentManager.CreateProducer ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnAddRepairCenter ()
{
CHECKMINE;
segmentManager.CreateProducer (-1, SEGMENT_FUNC_REPAIRCEN);
}

//------------------------------------------------------------------------------

void CSegmentTool::OnAddReactor ()
{
CHECKMINE;
segmentManager.CreateReactor ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnSplitSegmentIn7 ()
{
CHECKMINE;
segmentManager.SplitIn7 ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnCreateWedge ()
{
CHECKMINE;
segmentManager.CreateWedge ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnCreatePyramid ()
{
CHECKMINE;
segmentManager.CreatePyramid ();
}

//------------------------------------------------------------------------------

void CSegmentTool::OnSplitSegmentIn8 ()
{
CHECKMINE;
BOOL bTagged = segmentManager.HasTaggedSegments ();

undoManager.Begin (__FUNCTION__, udSegments | udVertices | udWalls);
DLE.MineView ()->DelayRefresh (true);
UpdateData (TRUE);
if (!bTagged) 
	segmentManager.SplitIn8 (current->Segment ());
else {
	short nSegCount = segmentManager.Count ();
	for (short nSegment = 0; nSegment < nSegCount; nSegment++)
		if (segmentManager.Segment (nSegment)->IsTagged () && !segmentManager.SplitIn8 (segmentManager.Segment (nSegment)))
			break;
	}
DLE.MineView ()->DelayRefresh (false);
DLE.MineView ()->Refresh ();
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

void CSegmentTool::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (pScrollBar == (CScrollBar *) SegIdSpinner ()) {
	if (scrollCode == SB_THUMBPOSITION) {
		CBSegmentId ()->SetCurSel (thumbPos);
		OnSetSegment ();
		}
	}
else if (pScrollBar == (CScrollBar *) SegFuncSpinner ()) {
	if (scrollCode == SB_THUMBPOSITION)
		SelectItemData (CBFunction (), m_nFunction = thumbPos);
	}
else
	CToolDlg::OnVScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------------

//eof segmenttool.cpp
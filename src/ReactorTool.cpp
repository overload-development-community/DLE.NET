
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

// D1/2 use an algorithm to determine the "shield count" of a reactor that starts at
// 200 for the first level in a mission, increasing by 50 each level. (Secret levels
// appear to start from 350, adding 150 for successive secret levels.)
// BASE_REACTOR_STRENGTH is meant to be used to show that default value when the user
// unchecks "default" to give them an idea what a "normal" value is; ideally it would
// show whatever the default value for the current level is but that would require
// calculating the level number, which is going to be more complicated than it's
// really worth for a mere guideline. So I'm just using the base value for level 1.
#define BASE_REACTOR_STRENGTH 200

//------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CReactorTool, CToolDlg)
	ON_BN_CLICKED (IDC_REACTOR_ADDTGT, OnAddTarget)
	ON_BN_CLICKED (IDC_REACTOR_DELTGT, OnDeleteTarget)
	ON_BN_CLICKED (IDC_REACTOR_ADDWALLTGT, OnAddWallTarget)
	ON_BN_CLICKED (IDC_REACTOR_DELWALLTGT, OnDeleteWallTarget)
	ON_BN_CLICKED (IDC_REACTOR_STRENGTHDEFAULT, OnStrengthDefault)
	ON_EN_KILLFOCUS (IDC_REACTOR_COUNTDOWN, OnCountDown)
	ON_EN_KILLFOCUS (IDC_REACTOR_SECRETRETURN, OnSecretReturn)
	ON_EN_KILLFOCUS (IDC_REACTOR_STRENGTH, OnStrength)
	ON_EN_UPDATE (IDC_REACTOR_COUNTDOWN, OnCountDown)
	ON_EN_UPDATE (IDC_REACTOR_SECRETRETURN, OnSecretReturn)
	ON_EN_UPDATE (IDC_REACTOR_STRENGTH, OnStrength)
	ON_LBN_SELCHANGE (IDC_REACTOR_TARGETLIST, OnSetTarget)
	ON_LBN_DBLCLK (IDC_REACTOR_TARGETLIST, OnSetTarget)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------
// DIALOG - CReactorTool (constructor)
//------------------------------------------------------------------------

CReactorTool::CReactorTool (CPropertySheet *pParent)
	: CToolDlg (IDD_REACTORDATA_HORZ + nLayout, pParent)
{
m_pTrigger = null;
Reset ();
}

								/*--------------------------*/

void CReactorTool::Reset ()
{
m_nCountDown = 30;
m_nSecretReturn = 0;
m_nTrigger = 0;
m_targets = 0;
m_iTarget = -1;
*m_szTarget = '\0';
m_nStrength = BASE_REACTOR_STRENGTH;
m_bDefaultStrength = true;
}

//------------------------------------------------------------------------------

BOOL CReactorTool::OnInitDialog ()
{
if (!CToolDlg::OnInitDialog ())
	return FALSE;
// Descent only uses the first control center, #0
m_bInited = true;
return TRUE;
}

								/*--------------------------*/

void CReactorTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;
DDX_Text (pDX, IDC_REACTOR_COUNTDOWN, m_nCountDown);
DDX_Text (pDX, IDC_REACTOR_SECRETRETURN, m_nSecretReturn);
DDX_Text (pDX, IDC_REACTOR_TARGET, m_szTarget, sizeof (m_szTarget));
// Checking m_bDefaultStrength before updating it because, if it's being set,
// the reactor strength text field won't be appropriately set yet
if (!m_bDefaultStrength)
	DDX_Text (pDX, IDC_REACTOR_STRENGTH, m_nStrength);
else if (!pDX->m_bSaveAndValidate)
	// Default strength: clear the text box value, it's meaningless
	DDX_Text (pDX, IDC_REACTOR_STRENGTH, CString (""));
DDX_Check (pDX, IDC_REACTOR_STRENGTHDEFAULT, m_bDefaultStrength);
}

								/*--------------------------*/

BOOL CReactorTool::OnSetActive ()
{
Refresh ();
return CToolDlg::OnSetActive ();
}

								/*--------------------------*/

void CReactorTool::EnableControls (BOOL bEnable)
{
CToolDlg::EnableControls (IDC_REACTOR_COUNTDOWN, IDC_REACTOR_STRENGTHDEFAULT, bEnable);
//int i;
//for (i = IDC_TRIGGER_TRIGGER_NO; i <= IDC_TRIGGER_PASTE; i++)
//	GetDlgItem (i)->EnableWindow (bEnable);
}

								/*--------------------------*/

void CReactorTool::InitLBTargets ()
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

//------------------------------------------------------------------------
// CReactorTool - RefreshData
//------------------------------------------------------------------------

void CReactorTool::Refresh ()
{
if (!(m_bInited && theMine))
	return;
EnableControls (DLE.IsD2File ());
m_pTrigger = triggerManager.ReactorTrigger (m_nTrigger);
m_nCountDown = triggerManager.ReactorTime ();
m_nStrength = triggerManager.ReactorStrength ();
m_bDefaultStrength = (m_nStrength == -1);
if (DLE.IsD2File () && m_bDefaultStrength)
	GetDlgItem (IDC_REACTOR_STRENGTH)->EnableWindow (FALSE);
m_nSecretReturn = objectManager.SecretSegment ();
InitLBTargets ();
OnSetTarget ();
UpdateData (FALSE);
}

//------------------------------------------------------------------------
// CReactorTool - TrigValueMsg
//------------------------------------------------------------------------

void CReactorTool::OnCountDown () 
{
char szVal [5];
::GetWindowText (GetDlgItem (IDC_REACTOR_COUNTDOWN)->m_hWnd, szVal, sizeof (szVal));
if (!*szVal)
	return;
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udTriggers);
triggerManager.ReactorTime () = m_nCountDown;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------
// CReactorTool - TrigTimeMsg
//------------------------------------------------------------------------

void CReactorTool::OnSecretReturn () 
{
char szVal [5];
::GetWindowText (GetDlgItem (IDC_REACTOR_SECRETRETURN)->m_hWnd, szVal, sizeof (szVal));
if (!*szVal)
	return;
UpdateData (TRUE);
undoManager.Begin (__FUNCTION__, udObjects);
objectManager.SecretSegment () = m_nSecretReturn;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

void CReactorTool::OnStrength ()
{
char szVal [5];
::GetWindowText (GetDlgItem (IDC_REACTOR_STRENGTH)->m_hWnd, szVal, sizeof (szVal));
if (!*szVal)
	return;
UpdateData (TRUE);

// Clamp reactor strength - 16-bit value and we reserve negative values for default (-1)
// Hopefully nobody actually wants negative reactor strength.
if (m_nStrength < 0)
	m_nStrength = 0;
else if (m_nStrength >= 0x8000)
	m_nStrength = 0x7fff;

undoManager.Begin (__FUNCTION__, udTriggers);
triggerManager.ReactorStrength () = m_bDefaultStrength ? -1 : m_nStrength;
undoManager.End (__FUNCTION__);
}

//------------------------------------------------------------------------------

void CReactorTool::OnStrengthDefault ()
{
UpdateData (TRUE);

if (m_nStrength == -1)
	m_nStrength = BASE_REACTOR_STRENGTH;

undoManager.Begin (__FUNCTION__, udTriggers);
triggerManager.ReactorStrength () = m_bDefaultStrength ? -1 : m_nStrength;
undoManager.End (__FUNCTION__);

Refresh ();
}

//------------------------------------------------------------------------
// CReactorTool - Add segment/side to trigger list
//------------------------------------------------------------------------

void CReactorTool::AddTarget (short nSegment, short nSide) 
{
m_targets = m_pTrigger->Count ();
if (m_targets >= MAX_TRIGGER_TARGETS) {
	DEBUGMSG (" Reactor tool: No more targets possible for this trigger.");
	return;
	}
if (FindTarget (nSegment, nSide) >= 0) {
	DEBUGMSG (" Reactor tool: Trigger already has this target.");
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

void CReactorTool::OnAddTarget () 
{
int nSegment, nSide;
UpdateData (TRUE);
sscanf_s (m_szTarget, "%d,%d", &nSegment, &nSide);
if ((nSegment < 0) || (nSegment >= segmentManager.Count ()) || (nSide < 1) || (nSide > 6))
	return;
AddTarget (nSegment, nSide);
}

//------------------------------------------------------------------------------

void CReactorTool::OnAddWallTarget ()
{
other = &selections [!current->Index ()];
int i = FindTarget (other->m_nSegment, other->m_nSide);
if (i >= 0)
	return;
LBTargets ()->SetCurSel (i);
AddTarget (other->m_nSegment, other->m_nSide + 1);
}

//------------------------------------------------------------------------
// CReactorTool - Delete segment/side
//------------------------------------------------------------------------

void CReactorTool::OnDeleteTarget ()
{
m_iTarget = LBTargets ()->GetCurSel ();
if ((m_iTarget < 0) || (m_iTarget >= MAX_TRIGGER_TARGETS))
	return;
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

int CReactorTool::FindTarget (short nSegment, short nSide)
{
return m_pTrigger->Find (nSegment, nSide);
}

//------------------------------------------------------------------------------

void CReactorTool::OnDeleteWallTarget ()
{
other = &selections [!current->Index ()];
int i = FindTarget (other->m_nSegment, other->m_nSide);
if (i < 0) {
	DEBUGMSG (" Reactor tool: Trigger doesn't target other segment's current side.");
	return;
	}
LBTargets ()->SetCurSel (i);
OnDeleteTarget ();
}

//------------------------------------------------------------------------
// CReactorTool - Cube/Side list box message
//
// sets "other segment" to selected item
//------------------------------------------------------------------------

void CReactorTool::OnSetTarget () 
{
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
*((CSideKey *) other) = (*m_pTrigger) [m_iTarget];
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------------

//eof reactordlg.cpp
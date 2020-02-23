// Copyright (C) 1997 Bryan Aamot
#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#undef abs
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>
#include "stophere.h"
#include "define.h"
#include "types.h"
#include "dle-xp.h"
#include "mine.h"
#include "global.h"
#include "toolview.h"

                        /*--------------------------*/

BEGIN_MESSAGE_MAP (CReactorTool, CToolDlg)
	ON_BN_CLICKED (IDC_REACTOR_ADDTGT, OnAddTarget)
	ON_BN_CLICKED (IDC_REACTOR_DELTGT, OnDeleteTarget)
	ON_BN_CLICKED (IDC_REACTOR_ADDWALLTGT, OnAddWallTarget)
	ON_BN_CLICKED (IDC_REACTOR_DELWALLTGT, OnDeleteWallTarget)
	ON_EN_KILLFOCUS (IDC_REACTOR_COUNTDOWN, OnCountDown)
	ON_EN_KILLFOCUS (IDC_REACTOR_SECRETRETURN, OnSecretReturn)
	ON_EN_UPDATE (IDC_REACTOR_COUNTDOWN, OnCountDown)
	ON_EN_UPDATE (IDC_REACTOR_SECRETRETURN, OnSecretReturn)
	ON_LBN_SELCHANGE (IDC_REACTOR_TARGETLIST, OnSetTarget)
	ON_LBN_DBLCLK (IDC_REACTOR_TARGETLIST, OnSetTarget)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------
// DIALOG - CReactorTool (constructor)
//------------------------------------------------------------------------

CReactorTool::CReactorTool (CPropertySheet *pParent)
	: CToolDlg (nLayout ? IDD_REACTORDATA2 : IDD_REACTORDATA, pParent)
{
m_triggerP = null;
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
}

                        /*--------------------------*/

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
CToolDlg::EnableControls (IDC_REACTOR_COUNTDOWN, IDC_REACTOR_SECRETRETURN, bEnable);
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

//------------------------------------------------------------------------
// CReactorTool - RefreshData
//------------------------------------------------------------------------

void CReactorTool::Refresh ()
{
if (!(m_bInited && theMine))
	return;
EnableControls (DLE.IsD2File ());
m_triggerP = theMine->ReactorTriggers (m_nTrigger);
m_nCountDown = triggerManager.ReactorTime ();
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
undoManager.Begin (udTriggers);
triggerManager.ReactorTime () = m_nCountDown;
undoManager.End ();
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
undoManager.Begin (udObjects);
objectManager.SecretSegment () = m_nSecretReturn;
undoManager.End ();
}

//------------------------------------------------------------------------
// CReactorTool - Add cube/side to trigger list
//------------------------------------------------------------------------

void CReactorTool::AddTarget (short nSegment, short nSide) 
{
m_targets = m_triggerP->m_count;
if (m_targets >= MAX_TRIGGER_TARGETS) {
	DEBUGMSG (" Reactor tool: No more targets possible for this trigger.");
	return;
	}
if (FindTarget (nSegment, nSide) >= 0) {
	DEBUGMSG (" Reactor tool: Trigger already has this target.");
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

void CReactorTool::OnAddTarget () 
{
int nSegment, nSide;
UpdateData (TRUE);
sscanf_s (m_szTarget, "%d,%d", &nSegment, &nSide);
if ((nSegment < 0) || (nSegment >= segmentManager.Count ()) || (nSide < 1) || (nSide > 6))
	return;
AddTarget (nSegment, nSide);
}

                        /*--------------------------*/

void CReactorTool::OnAddWallTarget ()
{
CSelection *other = (theMine->Current () == &theMine->Current1 ()) ? &theMine->Current2 () : &theMine->Current1 ();
int i = FindTarget (other->nSegment, other->nSide);
if (i >= 0)
	return;
LBTargets ()->SetCurSel (i);
AddTarget (other->nSegment, other->nSide + 1);
}

//------------------------------------------------------------------------
// CReactorTool - Delete cube/side
//------------------------------------------------------------------------

void CReactorTool::OnDeleteTarget ()
{
m_iTarget = LBTargets ()->GetCurSel ();
if ((m_iTarget < 0) || (m_iTarget >= MAX_TRIGGER_TARGETS))
	return;
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

int CReactorTool::FindTarget (short nSegment, short nSide)
{
return m_triggerP->Find (nSegment, nSide);
}

                        /*--------------------------*/

void CReactorTool::OnDeleteWallTarget ()
{
CSelection *other = (theMine->Current () == &theMine->Current1 ()) ? &theMine->Current2 () : &theMine->Current1 ();
int i = FindTarget (other->nSegment, other->nSide);
if (i < 0) {
	DEBUGMSG (" Reactor tool: Trigger doesn't target other cube's current side.");
	return;
	}
LBTargets ()->SetCurSel (i);
OnDeleteTarget ();
}

//------------------------------------------------------------------------
// CReactorTool - Cube/Side list box message
//
// sets "other cube" to selected item
//------------------------------------------------------------------------

void CReactorTool::OnSetTarget () 
{
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

//eof reactordlg.cpp
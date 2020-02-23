
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

//------------------------------------------------------------------------

BEGIN_MESSAGE_MAP (CSoundEffectTool, CEffectTabDlg)
	ON_WM_HSCROLL ()
	ON_BN_CLICKED (IDC_EFFECT_ENABLED, OnEdit)
	ON_EN_KILLFOCUS (IDC_SOUND_FILE, OnEdit)
END_MESSAGE_MAP ()

//------------------------------------------------------------------------

BOOL CSoundEffectTool::OnInitDialog ()
{
if (!CEffectTabDlg::OnInitDialog ())
	return FALSE;
m_soundVolume.Init (this, IDC_SOUND_VOLUME_SLIDER, IDC_SOUND_VOLUME_SPINNER, -IDT_SOUND_VOLUME, 0, 10, 1.0, 10.0, 1, "%d%%");
m_bInited = true;
return TRUE;
}

//------------------------------------------------------------------------

void CSoundEffectTool::DoDataExchange (CDataExchange *pDX)
{
if (!HaveData (pDX)) 
	return;
CGameObject *pObject = GetEffect (null, false);
EnableControls (pObject != null);
if (pObject) {
	pObject->rType.soundInfo.bEnabled = DDX_Flag (pDX, IDC_EFFECT_ENABLED, pObject->rType.soundInfo.bEnabled);
	DDX_Text (pDX, IDC_SOUND_FILE, pObject->rType.soundInfo.szFilename, sizeof (pObject->rType.soundInfo.szFilename));
	m_soundVolume.DoDataExchange (pDX, pObject->rType.soundInfo.nVolume);
	}
}

//------------------------------------------------------------------------

void CSoundEffectTool::EnableControls (BOOL bEnable)
{
if (!(m_bInited && theMine))
	return;
CDlgHelpers::EnableControls (IDC_SOUND_FILE, IDC_SOUND_VOLUME_SLIDER, GetEffect (null, false) != null);
}

//------------------------------------------------------------------------

void CSoundEffectTool::Add (void)
{
if (!AddEffect ())
	return;
CGameObject *pObject = current->Object ();
pObject->Type () = OBJ_EFFECT;
pObject->Id () = SOUND_ID;
pObject->m_info.movementType = MT_NONE;
pObject->m_info.controlType = CT_NONE;
pObject->m_info.renderType = RT_SOUND;
*pObject->rType.soundInfo.szFilename = '\0';
pObject->rType.soundInfo.nVolume = 10;
Refresh ();
DLE.MineView ()->Refresh ();
}

//------------------------------------------------------------------------

void CSoundEffectTool::Copy (void)
{
CGameObject *pObject = GetEffect ();
if (!pObject) 
	ErrorMsg ("No effect object currently selected");
else 
	m_sound = pObject->rType.soundInfo;
}

//------------------------------------------------------------------------

void CSoundEffectTool::Paste (CGameObject* pObject, bool bRefresh, bool bVerbose)
{
if (Valid () && (pObject = GetEffect (pObject, bVerbose))) {
	pObject->rType.soundInfo = m_sound;
	if (bRefresh)
		Refresh ();
	}
}

//------------------------------------------------------------------------

void CSoundEffectTool::OnEdit (void)
{
UpdateData (TRUE);
}

//------------------------------------------------------------------------

void CSoundEffectTool::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
if (m_soundVolume.OnScroll (scrollCode, thumbPos, pScrollBar))
	UpdateData (TRUE);
else
	CEffectTabDlg::OnHScroll (scrollCode, thumbPos, pScrollBar);
}

//------------------------------------------------------------------------
//eof SoundEffectTool.cpp
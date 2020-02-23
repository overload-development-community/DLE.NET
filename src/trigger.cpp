
#include "stdafx.h"
#include <malloc.h>
#include <stdlib.h>
#undef abs
#include <math.h>
#include <mmsystem.h>
#include <stdio.h>

#include "define.h"
#include "types.h"
#include "FileManager.h"
#include "trigger.h"
#include "dle-xp.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

short CTriggerTargets::Add (CSideKey key) 
{
if (m_count < MAX_TRIGGER_TARGETS) {
	m_targets [m_count] = key;
	m_count++;
	}
return m_count;
}

//------------------------------------------------------------------------------

short CTriggerTargets::Delete (short i) 
{
if (i < 0)
	i = m_count - 1;
if ((m_count > 0) && (i < m_count)) {
	m_count--;
	int l = m_count - i;
	if (l)
		memcpy_s (m_targets + i, sizeof (m_targets) - (i * sizeof (m_targets [0])),
			m_targets + i + 1, l * sizeof (m_targets [0]));
	m_targets [m_count] = CSideKey (-1,-1);
	}
return m_count;
}	

//------------------------------------------------------------------------------

int CTriggerTargets::Delete (CSideKey key) 
{ 
	short i = -1;

if (key.m_nSegment < 0) {
	// delete all sides of segment (-key.m_nSegment - 1) from the target list
	key.m_nSegment = -key.m_nSegment - 1;
	for (int j = m_count - 1; j >= 0; j--) {
		if (m_targets [j].m_nSegment == key.m_nSegment) {
			Delete (j);
			if (i < 0)
				i = j;
			}
		}
	}
else {
	i = Find (key);
	if (i >= 0)
		Delete (i);
	}
return i;
}

//------------------------------------------------------------------------------

int CTriggerTargets::Find (CSideKey key) 
{ 
for (int i = 0; i < m_count; i++)
	if (m_targets [i] == key)
		return i;
return -1;
}

//------------------------------------------------------------------------------

void CTriggerTargets::Update (short nOldSeg, short nNewSeg) 
{ 
for (int i = 0; i < m_count; i++)
	if (m_targets [i].m_nSegment == nOldSeg)
		m_targets [i].m_nSegment = nNewSeg;
}

//------------------------------------------------------------------------------

void CTriggerTargets::Clear (void) 
{ 
m_count = 0;
for (int i = 0; i < MAX_TRIGGER_TARGETS; i++)
	m_targets [i].Clear ();
}

//------------------------------------------------------------------------------

void CTriggerTargets::Read (CFileManager* fp) 
{
	int i;

for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	m_targets [i].m_nSegment = fp->ReadInt16 ();
for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	m_targets [i].m_nSide = fp->ReadInt16 ();
}

//------------------------------------------------------------------------------

void CTriggerTargets::Write (CFileManager* fp) 
{
	int i;

for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	fp->WriteInt16 ((short) (((m_targets [i].m_nSegment < 0) || (m_targets [i].m_nSide <= 0)) ? m_targets [i].m_nSegment : segmentManager.Segment (m_targets [i].m_nSegment)->Index ()));
for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	fp->Write (m_targets [i].m_nSide);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CTrigger::Setup (short type, short flags)
{
m_info.type = (char) type;
m_info.flags = (char) flags;
if (type == TT_SPEEDBOOST)
	m_info.value = 10;
else if (type == TT_SHAKE_MINE)
	m_info.value = 10;
else if ((type == TT_CHANGE_TEXTURE) || (type == TT_MASTER))
	m_info.value = 0;
else if ((type == TT_MESSAGE) || (type == TT_SOUND))
	m_info.value = 1;
else 	
	m_info.value = I2X (5); // 5% shield or energy damage
m_info.time = -1;
CTriggerTargets::Clear ();
}

//------------------------------------------------------------------------------

void CTrigger::Read (CFileManager* fp, bool bObjTrigger)
{
if (DLE.IsD2File ()) {
	m_info.type = fp->ReadUByte ();
	m_info.flags = bObjTrigger ? fp->ReadInt16 () : (ushort) fp->ReadUByte ();
	m_count = fp->ReadUByte ();
	fp->ReadUByte ();
	m_info.value = fp->ReadInt32 ();
	if ((DLE.LevelVersion () < 21) && (m_info.type == TT_EXIT))
		m_info.value = 0;
	if ((DLE.FileVersion () < 39) && (m_info.type == TT_MASTER))
		m_info.value = 0;
	m_info.time = fp->ReadInt32 ();
	}
else {
	m_info.type = fp->ReadUByte ();
	m_info.flags = fp->ReadInt16 ();
	m_info.value = fp->ReadInt32 ();
	m_info.time = fp->ReadInt32 ();
	fp->ReadUByte (); //skip 8 bit value "link_num"
	m_count = char (fp->ReadInt16 ());
	if (m_count < 0)
		m_count = 0;
	else if (m_count > MAX_TRIGGER_TARGETS)
		m_count = MAX_TRIGGER_TARGETS;
	}
this->CTriggerTargets::Read (fp);
}

//------------------------------------------------------------------------------

void CTrigger::Write (CFileManager* fp, bool bObjTrigger)
{
if (DLE.IsD2File ()) {
	fp->Write (m_info.type);
	if (bObjTrigger)
		fp->Write (m_info.flags);
	else
		fp->WriteSByte ((sbyte) m_info.flags);
	fp->WriteSByte ((sbyte) m_count);
	fp->WriteByte (0);
	fp->Write (m_info.value);
	fp->Write (m_info.time);
	}
else {
	fp->Write (m_info.type);
	fp->Write (m_info.flags);
	fp->Write (m_info.value);
	fp->Write (m_info.time);
	fp->WriteSByte ((sbyte) m_count);
	fp->Write (m_count);
	}
this->CTriggerTargets::Write (fp);
}

//------------------------------------------------------------------------------

bool CTrigger::IsExit (bool bSecret)
{
return DLE.IsD1File () 
		 ? (m_info.flags & (TRIGGER_EXIT | TRIGGER_SECRET_EXIT)) != 0
		 : (m_info.type == TT_EXIT) || (bSecret && (m_info.type == TT_SECRET_EXIT));
}

// -----------------------------------------------------------------------------

CGameItem* CTrigger::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CTrigger*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CTrigger::Clone (void)
{
return Copy (new CTrigger);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CTrigger::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CReactorTrigger::Read (CFileManager* fp, bool bFlag)
{
m_count = char (fp->ReadInt16 ());
#if 1
this->CTriggerTargets::Read (fp);
#else
	int	i;

for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	m_targets [i].m_nSegment = fp->ReadInt16 ();
for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	m_targets [i].m_nSide = fp->ReadInt16 ();
#endif
}

//------------------------------------------------------------------------------

void CReactorTrigger::Write (CFileManager* fp, bool bFlag)
{
fp->Write (m_count);
#if 1
this->CTriggerTargets::Write (fp);
#else
	int	i;

for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	fp->Write (m_targets [i].m_nSegment);
for (i = 0; i < MAX_TRIGGER_TARGETS; i++)
	fp->Write (m_targets [i].m_nSide);
#endif
}

// -----------------------------------------------------------------------------

void CTrigger::Clear (void) 
{ 
memset (&m_info, 0, sizeof (m_info)); 
CTriggerTargets::Clear ();
}

// -----------------------------------------------------------------------------

CGameItem* CReactorTrigger::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CReactorTrigger*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CReactorTrigger::Clone (void)
{
return Copy (new CReactorTrigger);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CReactorTrigger::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof trigger.cpp
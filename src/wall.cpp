
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

ubyte animClipTable [NUM_OF_CLIPS_D2] = {
	 0,  1,  3,  4,  5,  6,  7,  9, 10, 11, 
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 
	42, 43, 44, 45, 46, 47, 48, 49, 50
	};

ubyte doorClipTable [NUM_OF_CLIPS_D2] = {
	 1,  1,  4,  5, 10, 24,  8, 11, 13, 12, 
	14, 17, 18, 19, 20, 21, 22, 23, 25, 26, 
	28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 
	38, 39, 40, 41, 42, 43, 44, 45, 47, 48, 
	49, 50, 51, 52, 53, 54, 55, 56, 57
	};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Note: if nClip == -1, then it is overriden for blastable and auto door
//       if nTexture == -1, then it is overriden for illusion Walls ()
//       if nClip == -2, then texture is applied to nOvlTex instead
//------------------------------------------------------------------------------

void CWall::Setup (CSideKey key, ushort nWall, ubyte type, char nClip, short nTexture, bool bRedefine) 
{
undoManager.Begin (__FUNCTION__, udWalls);
// define new pWall
*((CSideKey*) this) = key;
//m_nSegment = nSegment;
//m_nSide = nSide;
m_info.type = type;
if (!bRedefine) {
	m_info.nTrigger = NO_TRIGGER;
	m_info.linkedWall = -1; //OppositeWall (nOppWall, nSegment, nSide) ? nOppWall : -1;
	}
switch (type) {
	case WALL_BLASTABLE:
		m_info.nClip = (nClip == -1) ?  6 : nClip;
		m_info.hps = WALL_HPS;
		// define door textures based on clip number
		SetTextures (nTexture);
		break;

	case WALL_DOOR:
		m_info.nClip = (nClip == -1) ? 1 : nClip;
		m_info.hps = 0;
		// define door textures based on clip number
		SetTextures (nTexture);
		break;

	case WALL_CLOSED:
	case WALL_ILLUSION:
		m_info.nClip = -1;
		m_info.hps = 0;
		// define texture to be energy
		if (nTexture == -1)
			segmentManager.SetTextures (key, DLE.IsD1File () ? 328 : 353, 0); // energy
		else if (nClip == -2)
			segmentManager.SetTextures (key, 0, nTexture);
		else
			segmentManager.SetTextures (key, nTexture, 0);
		break;

	case WALL_OVERLAY: // d2 only
		m_info.nClip = -1;
		m_info.hps = 0;
		// define box01a
		segmentManager.SetTextures (key, -1, 414);
		break;

	case WALL_CLOAKED:
		m_info.cloakValue = 17;
		break;

	case WALL_COLORED:
		m_info.cloakValue = 0;
		break;

	default:
		m_info.nClip = -1;
		m_info.hps = 0;
		segmentManager.SetTextures (key, nTexture, 0);
		break;
	}
m_info.flags = 0;
m_info.state = 0;
m_info.keys = 0;
m_info.controllingTrigger = 0;

// set uvls of new texture
segmentManager.Segment (key.m_nSegment)->SetUV (key.m_nSide, 0.0, 0.0);
undoManager.End (__FUNCTION__);
}

//--------------------------------------------------------------------------
// SetWallTextures()
//--------------------------------------------------------------------------

void CWall::SetTextures (short nTexture) 
{
	static short wallTexturesD1 [N_WALL_TEXTURES_D1][2] = {
		{371,0},{0,376},{0,0},  {0,387},{0,399},{413,0},{419,0},{0,424},  {0,0},{436,0},
		{0,444},{0,459},{0,472},{486,0},{492,0},{500,0},{508,0},{515,0},{521,0},{529,0},
		{536,0},{543,0},{0,550},{563,0},{570,0},{577,0}
		};

	static short wallTexturesD2 [N_WALL_TEXTURES_D2][2] = {
		{435,0},{0,440},{0,0},{0,451},{0,463},{477,0},{483,0},{0,488},{0,0},  {500,0},
		{0,508},{0,523},{0,536},{550,0},{556,0},{564,0},{572,0},{579,0},{585,0},{593,0},
		{600,0},{608,0},{0,615},{628,0},{635,0},{642,0},{0,649},{664,0},{0,672},{0,687},
		{0,702},{717,0},{725,0},{731,0},{738,0},{745,0},{754,0},{763,0},{772,0},{780,0},
		{0,790},{806,0},{817,0},{827,0},{838,0},{849,0},{858,0},{863,0},{0,871},{0,886},
		{901,0}
		};

CSide *pSide = Side ();
char nClip = m_info.nClip;

undoManager.Begin (__FUNCTION__, udWalls);
if (IsDoor ()) {
	if (DLE.IsD1File ())
		pSide->SetTextures (wallTexturesD1 [nClip][0], wallTexturesD1 [nClip][1]);
	else
		pSide->SetTextures (wallTexturesD2 [nClip][0], wallTexturesD2 [nClip][1]);
		}
else if (nTexture >= 0) {
	pSide->SetTextures (nTexture, 0);
	}
else {
	undoManager.Unroll (__FUNCTION__);
	return;
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
}

// -----------------------------------------------------------------------------

void CWall::Read (CFileManager* fp, bool bFlag)
{
m_nSegment = fp->ReadInt32 ();
m_nSide = fp->ReadInt32 (); 
m_info.hps = fp->ReadInt32 ();
m_info.linkedWall = fp->ReadInt32 ();
m_info.type = fp->ReadUByte ();
m_info.flags = ushort ((DLE.FileVersion () < 37) ? fp->ReadSByte () : fp->ReadInt16 ());         
m_info.state = fp->ReadUByte ();         
m_info.nTrigger = fp->ReadUByte ();       
m_info.nClip = fp->ReadUByte ();      
m_info.keys = fp->ReadUByte ();          
m_info.controllingTrigger = fp->ReadSByte ();
m_info.cloakValue = fp->ReadSByte ();
}

// -----------------------------------------------------------------------------

void CWall::Write (CFileManager* fp, bool bFlag)
{
fp->WriteInt32 ((m_nSegment < 0) ? m_nSegment : segmentManager.Segment (m_nSegment)->Index ());
fp->WriteInt32 ((int) m_nSide); 
fp->Write (m_info.hps);
fp->Write (m_info.linkedWall);
fp->Write (m_info.type);
if (DLE.FileVersion () < 37) 
	fp->WriteSByte ((sbyte) m_info.flags);
else
	fp->Write (m_info.flags);         
fp->Write (m_info.state);   
if (m_info.nTrigger == NO_TRIGGER)
	fp->WriteByte (NO_TRIGGER);       
else
	fp->WriteByte ((ubyte) triggerManager.Trigger (m_info.nTrigger)->Index ());
fp->Write (m_info.nClip);      
fp->Write (m_info.keys);          
fp->Write (m_info.controllingTrigger);
fp->Write (m_info.cloakValue);
}

// -----------------------------------------------------------------------------

CSide* CWall::Side (void)
{
return segmentManager.Side (*this);
}

// -----------------------------------------------------------------------------

CTrigger* CWall::Trigger (void)
{
return triggerManager.Trigger (m_info.nTrigger);
}

// -----------------------------------------------------------------------------

bool CWall::IsDoor (void)
{
return (m_info.type == WALL_BLASTABLE) || (m_info.type == WALL_DOOR);
}

//------------------------------------------------------------------------------

bool CWall::IsVisible (void)
{
return (m_info.type != WALL_OPEN);
}

//------------------------------------------------------------------------------

bool CWall::IsVariable (void)
{
CTrigger* pTrigger = Trigger ();
if (pTrigger == null)
	return false;
char trigType = pTrigger->Type ();
return (trigType == TT_ILLUSION_OFF) ||
		 (trigType == TT_ILLUSION_ON) ||
		 (trigType == TT_CLOSE_WALL) ||
		 (trigType == TT_OPEN_WALL) ||
		 (trigType == TT_LIGHT_OFF) ||
		 (trigType == TT_LIGHT_ON);
}

//------------------------------------------------------------------------------

int CWall::SetClip (short nTexture)
{
	char *ps, *pszName = textureManager.Name (-1, nTexture);

if (!strcmp (pszName, "wall01 - anim"))
	return m_info.nClip = 0;
if (ps = strstr (pszName, "door")) {
	int nDoor = atol (ps + 4);
	for (int i = 1; i < NUM_OF_CLIPS_D2; i++)
		if (nDoor == doorClipTable [i]) {
			Backup ();
			m_info.nClip = animClipTable [i];
			DLE.MineView ()->Refresh ();
			return i;
			}
	}
return -1;
}

// -----------------------------------------------------------------------------

CGameItem* CWall::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CWall*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CWall::Clone (void)
{
return Copy (new CWall);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CWall::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

// -----------------------------------------------------------------------------

void CWall::Undo (void)
{
switch (EditType ()) {
	case opAdd:
		wallManager.Remove (Index ());
		break;
	case opDelete:
		wallManager.Add (true);
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------

void CWall::Redo (void)
{
switch (EditType ()) {
	case opDelete:
		wallManager.Remove (Index ());
		break;
	case opAdd:
		wallManager.Add (false);
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CDoor::Read (CFileManager* fp, bool bFlag)
{
m_info.nParts = fp->ReadInt32 ();
m_info.nFrontWall [0] = fp->ReadInt16 ();
m_info.nFrontWall [1] = fp->ReadInt16 ();
m_info.nBackWall [0] = fp->ReadInt16 (); 
m_info.nBackWall [1] = fp->ReadInt16 (); 
m_info.time = fp->ReadInt32 ();		  
}

// -----------------------------------------------------------------------------

void CDoor::Write (CFileManager* fp, bool bFlag)
{
fp->Write (m_info.nParts);
fp->Write (m_info.nFrontWall [0]);
fp->Write (m_info.nFrontWall [1]);
fp->Write (m_info.nBackWall [0]); 
fp->Write (m_info.nBackWall [1]); 
fp->Write (m_info.time);		  
}

// -----------------------------------------------------------------------------

CGameItem* CDoor::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CDoor*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CDoor::Clone (void)
{
return Copy (new CDoor);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CDoor::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

//eof wall.cpp
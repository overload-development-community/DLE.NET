
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <io.h>

#include "mine.h"
#include "dle-xp.h"
#include "toolview.h"
#include "FileManager.h"
#include "HogManager.h"

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

void tRobotGunInfo::Read (CFileManager* fp, int nField) 
{
if (nField == 0)
	fp->Read (points);
else
	subModels = ubyte (fp->ReadSByte ());
}

void tRobotGunInfo::Write (CFileManager* fp, int nField) 
{
if (nField == 0)
	fp->Write (points);
else
	fp->WriteSByte ((sbyte) subModels);
}

//------------------------------------------------------------------------

void tRobotExplInfo::Read (CFileManager* fp) 
{
nClip = fp->ReadInt16 ();
nSound = fp->ReadInt16 ();
}

void tRobotExplInfo::Write (CFileManager* fp) 
{
fp->Write (nClip);
fp->Write (nSound);
}

//------------------------------------------------------------------------

void tRobotContentsInfo::Read (CFileManager* fp) 
{
id = fp->ReadSByte ();
count = fp->ReadSByte ();
prob = fp->ReadSByte ();
type = fp->ReadSByte ();
}

void tRobotContentsInfo::Write (CFileManager* fp) 
{
fp->Write (id);
fp->Write (count);
fp->Write (prob);
fp->Write (type);
}

//------------------------------------------------------------------------

void tRobotSoundInfo::Read (CFileManager* fp) 
{
see = fp->ReadUByte ();
attack = fp->ReadUByte ();
claw = fp->ReadUByte ();
taunt = fp->ReadUByte ();
}

void tRobotSoundInfo::Write (CFileManager* fp) 
{
fp->Write (see);
fp->Write (attack);
fp->Write (claw);
fp->Write (taunt);
}

//------------------------------------------------------------------------

void tRobotCombatInfo::Read (CFileManager* fp, int nField) 
{
	switch (nField) {
		case 0:
			fieldOfView = fp->ReadInt32 ();
			break;
		case 1:
			firingWait [0] = fp->ReadInt32 ();
			break;
		case 2:
			firingWait [1] = fp->ReadInt32 ();
			break;
		case 3:
			turnTime = fp->ReadInt32 ();
			break;
		case 4:
			maxSpeed = fp->ReadInt32 ();
			break;
		case 5:
			circleDistance = fp->ReadInt32 ();
			break;
		case 6:
			rapidFire = fp->ReadSByte ();
			break;
		case 7:
			evadeSpeed = fp->ReadSByte ();
			break;
		}
	}

void tRobotCombatInfo::Write (CFileManager* fp, int nField) {
	switch (nField) {
		case 0:
			fp->Write (fieldOfView);
			break;
		case 1:
			fp->Write (firingWait [0]);
			break;
		case 2:
			fp->Write (firingWait [1]);
			break;
		case 3:
			fp->Write (turnTime);
			break;
		case 4:
			fp->Write (maxSpeed);
			break;
		case 5:
			fp->Write (circleDistance);
			break;
		case 6:
			fp->Write (rapidFire);
			break;
		case 7:
			fp->Write (evadeSpeed);
			break;
		}
	}

//------------------------------------------------------------------------

void CRobotInfo::Read (CFileManager* fp, int version, bool bFlag) 
{ 
	int i, j;

m_info.nModel = fp->ReadInt32 ();
for (j = 0; j < 2; j++)
	for (i = 0; i < MAX_GUNS; i++)
		m_info.guns [i].Read (fp, j);
for (i = 0; i < 2; i++)
	m_info.expl [i].Read (fp);
for (i = 0; i < 2; i++)
	m_info.weaponType [i] = fp->ReadSByte ();
m_info.n_guns = fp->ReadSByte ();
m_info.contents.Read (fp);
m_info.kamikaze = fp->ReadSByte ();
m_info.scoreValue = fp->ReadInt16 ();
m_info.badass = fp->ReadSByte ();
m_info.drainEnergy = fp->ReadSByte ();
m_info.lighting = fp->ReadInt32 ();
m_info.strength = fp->ReadInt32 ();
m_info.mass = fp->ReadInt32 ();
m_info.drag = fp->ReadInt32 ();
for (j = 0; j < 8; j++)
	for (i = 0; i < NDL; i++)
		m_info.combat [i].Read (fp, j);
m_info.cloakType = fp->ReadSByte ();
m_info.attackType = fp->ReadSByte ();
m_info.sounds.Read (fp);
m_info.bossFlag = fp->ReadSByte ();
m_info.companion = fp->ReadSByte ();
m_info.smartBlobs = fp->ReadSByte ();
m_info.energyBlobs = fp->ReadSByte ();
m_info.thief = fp->ReadSByte ();
m_info.pursuit = fp->ReadSByte ();
m_info.lightCast = fp->ReadSByte ();
m_info.deathRoll = fp->ReadSByte ();
m_info.flags = (ubyte) fp->ReadSByte ();
m_info.bCustom = fp->ReadSByte (); 
m_info.pad [0] = fp->ReadSByte (); 
m_info.pad [1] = fp->ReadSByte (); 
m_info.deathRollSound = (ubyte) fp->ReadSByte ();
m_info.glow = (ubyte) fp->ReadSByte ();
m_info.behavior = (ubyte) fp->ReadSByte ();
m_info.aim = (ubyte) fp->ReadSByte ();
for (i = 0; i <= MAX_GUNS; i++)
	for (j = 0; j < N_ANIM_STATES; j++)
		m_info.animStates [i][j].Read (fp);
m_info.always_0xabcd = fp->ReadInt32 ();
}

//------------------------------------------------------------------------

void CRobotInfo::Write (CFileManager* fp, int version, bool bFlag) 
{
	int i, j;

fp->Write (m_info.nModel);
for (j = 0; j < 2; j++)
	for (i = 0; i < MAX_GUNS; i++)
		m_info.guns [i].Write (fp, j);
for (i = 0; i < 2; i++)
	m_info.expl [i].Write (fp);
for (i = 0; i < 2; i++)
	fp->Write (m_info.weaponType [i]);
fp->Write (m_info.n_guns);
m_info.contents.Write (fp);
fp->Write (m_info.kamikaze);
fp->Write (m_info.scoreValue);
fp->Write (m_info.badass);
fp->Write (m_info.drainEnergy);
fp->Write (m_info.lighting);
fp->Write (m_info.strength);
fp->Write (m_info.mass);
fp->Write (m_info.drag);
for (j = 0; j < 8; j++)
	for (i = 0; i < NDL; i++)
		m_info.combat [i].Write (fp, j);
fp->Write (m_info.cloakType);
fp->Write (m_info.attackType);
m_info.sounds.Write (fp);
fp->Write (m_info.bossFlag);
fp->Write (m_info.companion);
fp->Write (m_info.smartBlobs);
fp->Write (m_info.energyBlobs);
fp->Write (m_info.thief);
fp->Write (m_info.pursuit);
fp->Write (m_info.lightCast);
fp->Write (m_info.deathRoll);
fp->Write (m_info.flags);
fp->Write (m_info.bCustom); // skip
fp->Write (m_info.pad [1]); // skip
fp->Write (m_info.pad [2]); // skip
(ubyte) fp->Write (m_info.deathRollSound);
(ubyte) fp->Write (m_info.glow);
(ubyte) fp->Write (m_info.behavior);
(ubyte) fp->Write (m_info.aim);
for (i = 0; i <= MAX_GUNS; i++)
	for (j = 0; j < N_ANIM_STATES; j++)
		m_info.animStates [i][j].Write (fp);
fp->Write (m_info.always_0xabcd);
}

// -----------------------------------------------------------------------------

CGameItem* CRobotInfo::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CRobotInfo*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CRobotInfo::Clone (void)
{
return Copy (new CRobotInfo);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CRobotInfo::Backup (eEditType editType)
{
Id () = undoManager.Backup (this, editType);
}

//--------------------------------------------------------------------------
//eof robot.cpp



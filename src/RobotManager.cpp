
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <io.h>

#include "define.h"
#include "global.h"
#include "FileManager.h"
#include "Mine.h"
#include "dle-xp.h"
#include "ResourceManager.h"
#include "ModelManager.h"

CRobotManager robotManager;

//------------------------------------------------------------------------------

void CRobotManager::Reset (void)
{
if (m_hxmExtraData) {
	free (m_hxmExtraData);
	m_hxmExtraData = null;
	m_hxmExtraDataSize = 0;
	}
}

//------------------------------------------------------------------------------

#define MAKESIG(_sig)	(uint) *((int *) &(_sig))

int CRobotManager::ReadHAM (CFileManager* fp, int type) 
{
  int				t, t0;
  uint			id;
  CPolyModel	pm;
  char			szFile [256];
  CFileManager	fm;

  static char d2HamSig [4] = {'H','A','M','!'};
  static char d2xHamSig [4] = {'M','A','H','X'};

if (fp == null) {
	fp = &fm;
	if (DLE.IsD2File ()) {
		CFileManager::SplitPath (descentFolder [1], szFile, null, null);
		strcat_s (szFile, sizeof (szFile), "descent2.ham");
		}
	else {
		CFileManager::SplitPath (descentFolder [0], szFile, null, null);
		strcat_s (szFile, sizeof (szFile), "descent.ham");
		}
	if (!fp->Open (szFile, "rb")) {
		sprintf_s (message, sizeof (message), " Ham manager: Cannot open robot file <%s>.", szFile);
		DEBUGMSG (message);
		return 0;
		}
	}

pm.m_info.nModels = 0;

// The extended HAM only contains part of the normal HAM file.
// Therefore, we have to skip some items if we are reading
// a normal HAM cause we are only interested in reading
// the information which is found in the extended ham
// (the robot information)
if (type == NORMAL_HAM)  {
	id = fp->ReadInt32 (); // "HAM!"
	if (id != MAKESIG (d2HamSig)) {//0x214d4148L) {
		sprintf_s (message, sizeof (message), "Not a D2 HAM file (%s)", fp->Name ());
		ErrorMsg (message);
	    return 0;
		}
	fp->ReadInt32 (); // version (0x00000007)
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (ushort) * t, SEEK_CUR);
	fp->Seek (sizeof (tTextureEffectInfo) * t, SEEK_CUR);
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (ubyte) * t, SEEK_CUR);
	fp->Seek (sizeof (ubyte) * t, SEEK_CUR);
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (tAnimationInfo) * t, SEEK_CUR);
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (tEffectInfo) * t, SEEK_CUR);
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (tWallEffectInfo) * t, SEEK_CUR);
	}
else if (type == EXTENDED_HAM)  {
	id = fp->ReadInt32 (); // "HAM!"
	if (id != MAKESIG (d2xHamSig)) {//0x214d4148L) {
		sprintf_s (message, sizeof (message), "Not a D2X HAM file (%s)", fp->Name ());
		ErrorMsg (message);
	    return 0;
		}
	fp->ReadInt32 (); //skip version
	t = fp->ReadInt32 (); //skip weapon count
	fp->Seek (t * sizeof (WEAPON_INFO), SEEK_CUR); //skip weapon info
	}

	// read robot information
t = fp->ReadInt32 ();
t0 = (type == NORMAL_HAM) ? 0: N_ROBOT_TYPES_D2;
m_nRobotTypes = t0 + t;
if (m_nRobotTypes > MAX_ROBOT_TYPES) {
	sprintf_s (message, sizeof (message), "Too many robots (%d) in <%s>.  Max is %d.", 
				  t, fp->Name (), MAX_ROBOT_TYPES - N_ROBOT_TYPES_D2);
	ErrorMsg (message);
	m_nRobotTypes = MAX_ROBOT_TYPES;
	t = m_nRobotTypes - t0;
	}
for (; t; t--, t0++) {
	RobotInfo (t0)->Read (fp);
	*DefRobotInfo (t0) = *RobotInfo (t0);
	}

// skip joints weapons, and powerups
t = fp->ReadInt32 ();
fp->Seek (sizeof (JOINTPOS) * t, SEEK_CUR);
if (type == NORMAL_HAM) {
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (WEAPON_INFO) * t, SEEK_CUR);
	t = fp->ReadInt32 ();
	fp->Seek (sizeof (POWERUP_TYPE_INFO) * t, SEEK_CUR);
	}

  // read poly model data and write it to a file
t = fp->ReadInt32 ();
if (t > MAX_POLYGON_MODELS) {
	sprintf_s (message, sizeof (message), "Too many polygon models (%d) in <%s>.  Max is %d.",
				t, fp->Name (), MAX_POLYGON_MODELS - N_POLYGON_MODELS_D2);
	ErrorMsg (message);
	return 0;
	}

fp->Close ();
return 1;
}

//------------------------------------------------------------------------------

int CRobotManager::ReadHXM (CFileManager& fp, long size, bool bCustom) 
{
	CRobotInfo	rInfo;
	int			n, i, j;

if (!fp.File ()) {
	ErrorMsg ("Invalid file handle for reading HXM data.");
	return 0;
	}

long p = fp.Tell ();
if (size < 0)
	size = fp.Length ();
uint id;
id = fp.ReadInt32 (); // "HXM!"
if (id != 0x21584d48L) {
	ErrorMsg ("Not a HXM file");
	return 0;
	}

fp.ReadInt32 (); // version (0x00000001)

// read robot information
n = fp.ReadInt32 ();
for (j = 0; j < n; j++) {
	i = fp.ReadInt32 ();
	if ((i < 0) || (i >= (int) m_nRobotTypes)) {
		sprintf_s (message, sizeof (message), "Robots number (%d) out of range.  Range = [0..%d].", i, m_nRobotTypes - 1);
		ErrorMsg (message);
		return 0;
		}
	rInfo.Read (&fp);
	// compare this to existing data
	if (memcmp (&rInfo, RobotInfo (i), sizeof (tRobotInfo)) != 0) {
		memcpy (RobotInfo (i), &rInfo, sizeof (tRobotInfo));
		RobotInfo (i)->Info ().bCustom = 1; // mark as custom
		}
	}

m_hxmExtraDataSize = size - fp.Tell () + p;
if (m_hxmExtraDataSize > 0) {
	m_hxmExtraData = new ubyte [m_hxmExtraDataSize];
	if (m_hxmExtraData == null) {
		ErrorMsg ("Couldn'n allocate extra data from hxm file.\nThis data will be lost when saving the level!");
		return 0;
		}
	if (fp.Read (m_hxmExtraData, m_hxmExtraDataSize, 1) != 1) {
		ErrorMsg ("Couldn'n read extra data from hxm file.\nThis data will be lost when saving the level!");
		return 0;
		}
	modelManager.ReadCustomModelData (m_hxmExtraData, m_hxmExtraDataSize, bCustom);
	}
return 1;
}

//------------------------------------------------------------------------------

int CRobotManager::ReadHXM (const char* filename, char *szSubFile, bool bCustom)
{
	CFileManager fp;

if (!fp.Open (filename, "rb")) 
	return 1;

long size = szSubFile ? hogManager->FindSubFile (fp, filename, szSubFile, null) : fp.Size ();
if (0 >= size) {
	fp.Close ();
	return 0;
	}

int nResult = ReadHXM (fp, size);
fp.Close ();
return nResult;
}

//------------------------------------------------------------------------------

int CRobotManager::WriteHXM (CFileManager& fp) 
{
	int i, t;

for (i = 0, t = 0; i < m_nRobotTypes; i++)
	if (IsCustomRobot (i))
		t++;
if ((t == 0) && (m_hxmExtraDataSize == 0))
	return 1;

if (!fp.File ()) {
	ErrorMsg ("Invalid file handle for writing HXM data.");
	return 0;
	}

fp.WriteInt32 (0x21584d48);	// "HXM!"
fp.WriteInt32 (1);   // version 1

// write robot information
fp.Write (t); // number of robot info structs stored
for (i = 0; i < m_nRobotTypes; i++) {
	if (RobotInfo (i)->Info ().bCustom) {
		fp.Write (i);
		RobotInfo (i)->Write (&fp);
		}
	}

if (m_hxmExtraDataSize)
	fp.Write (m_hxmExtraData, m_hxmExtraDataSize, 1);
else {
	// write zeros for the rest of the data
	fp.WriteInt32 (0);  //number of joints
	fp.WriteInt32 (0);  //number of polygon models
	fp.WriteInt32 (0);  //number of objbitmaps
	fp.WriteInt32 (0);  //number of objbitmaps
	fp.WriteInt32 (0);  //number of objbitmapptrs
	}

if (t) {
	sprintf_s (message, sizeof (message)," Hxm manager: Saving %d custom robots",t);
	DEBUGMSG (message);
	}
return fp.Close ();
}

//------------------------------------------------------------------------------

void CRobotManager::Init (void) 
{
LoadResource (-1);
}

//------------------------------------------------------------------------------

void CRobotManager::LoadResource (int nRobot) 
{
  int			i,	j,	t;
  ubyte*		pBuffer;
  CResource	res;

if (!(pBuffer = res.Load ("ROBOT.HXM"))) {
	ErrorMsg ("Could not lock robot resource data");
	return;
	}
t = (ushort) (*((uint *) pBuffer));
m_nRobotTypes = min (t, MAX_ROBOT_TYPES);
pBuffer += sizeof (uint);
for (j = 0; j < t; j++) {
	i = (ushort) (*((uint *) pBuffer));
	if (i > MAX_ROBOT_TYPES) 
		break;
	pBuffer += sizeof (uint);
	// copy the robot info for one robot, or all robots
	if ((j == nRobot) || (nRobot == -1)) 
		memcpy (RobotInfo (i), pBuffer, sizeof (tRobotInfo));
	pBuffer += sizeof (tRobotInfo);
	}
}

//------------------------------------------------------------------------------

bool CRobotManager::IsCustomRobot (int nId)
{
if (!RobotInfo (nId)->Info ().bCustom) //changed?
	return false;
	// check if actually different from defaults

bool bFound = false;

ubyte bCustom = DefRobotInfo (nId)->Info ().bCustom;
DefRobotInfo (nId)->Info ().bCustom = RobotInfo (nId)->Info ().bCustom; //make sure it's equal for the comparison
if (!memcmp (RobotInfo (nId), DefRobotInfo (nId), sizeof (tRobotInfo))) 
	RobotInfo (nId)->Info ().bCustom = 0; //same as default
else { //they're different
	// find a robot of that type
	CGameObject* pObject = objectManager.FindRobot (nId);
	if (pObject != null) // found one
		bFound = true;
	else { //no robot of that type present
		// find a matcen producing a robot of that type
		CSegment* pSegment;
		for (short i = 0; (pSegment = segmentManager.FindRobotMaker (i)) != null; i = segmentManager.Index (pSegment) + 1) {
			int nBotGen = pSegment->Info ().nProducer;
			if ((nId < 32) 
				 ? segmentManager.RobotMaker (nBotGen)->Info ().objFlags [0] & (1 << nId) 
				 : segmentManager.RobotMaker (nBotGen)->Info ().objFlags [1] & (1 << (nId - 32)))
				break;
			}
		if (pSegment != null) // found one
			bFound = true;
		else
			RobotInfo (nId)->Info ().bCustom = 0; // no matcens or none producing that robot type
		}
	}
DefRobotInfo (nId)->Info ().bCustom = bCustom; //restore
return bFound;
}

//------------------------------------------------------------------------------

bool CRobotManager::HasCustomRobots (void) 
{
for (int i = 0; i < (int) m_nRobotTypes; i++)
	if (IsCustomRobot (i))
		return true;
return (m_hxmExtraDataSize > 0);
}

//------------------------------------------------------------------------------

bool CRobotManager::HasModifiedRobots ()
{
for (int i = 0; i < (int) m_nRobotTypes; i++)
	if (RobotInfo (i)->IsModified ())
		return true;
return false;
}

//------------------------------------------------------------------------------

void CRobotManager::ClearHXMData ()
{
for (int i = 0; i < (int) m_nRobotTypes; i++) {
	*RobotInfo (i) = *DefRobotInfo (i);
	}
Reset ();
}

//------------------------------------------------------------------------------
//eof RobotManager.cpp



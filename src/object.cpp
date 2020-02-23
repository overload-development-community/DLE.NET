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
#include "mine.h"
#include "dle-xp.h"
#include "global.h"
#include "cfile.h"

//------------------------------------------------------------------------
// SortObjects ()
//------------------------------------------------------------------------

static short sortObjType [MAX_OBJECT_TYPES] = {7, 8, 5, 4, 0, 2, 9, 3, 10, 6, 11, 12, 13, 14, 1, 16, 15, 17, 18, 19, 20};


int CMine::QCmpObjects (CGameObject *pi, CGameObject *pm)
{
	short ti = sortObjType [pi->m_info.type];
	short tm = sortObjType [pm->m_info.type];
if (ti < tm)
	return -1;
if (ti > tm)
	return 1;
return (pi->m_info.id < pm->m_info.id) ? -1 : (pi->m_info.id > pm->m_info.id) ? 1 : 0;
}


short CMine::FindObjBySig (short signature)
{
	CGameObject*	objP = Objects (0);

for (short i = ObjCount (); i; i--, objP++)
	if (objP->m_info.signature == signature)
		return short (objP - Objects (0));
return -1;
}


void CMine::RenumberTriggerTargetObjs (void)
{
	CTrigger*	trigP = Triggers (0);

for (int i = TriggerCount (); i; i--, trigP++) {
	for (int j = 0; j < trigP->m_count; ) {
		if (trigP->Side (j) >= 0) 
			j++;
		else {
			int h = FindObjBySig (trigP->Segment (j));
			if (h >= 0)
				trigP->Side (j++) = h;
			else if (j < --trigP->m_count) {
				trigP [j] = trigP [trigP->m_count];
				}
			}
		}
	}
}


void CMine::RenumberObjTriggers (void)
{
	CTrigger*	trigP = ObjTriggers (0);
	int			i;

for (i = NumObjTriggers (); i; i--, trigP++)
	trigP->m_info.nObject = FindObjBySig (trigP->m_info.nObject);
i = NumObjTriggers ();
while (i) {
	if (ObjTriggers (--i)->m_info.nObject < 0)
		DeleteObjTrigger (i);
	}
SortObjTriggers ();
}


void CMine::QSortObjects (short left, short right)
{
	CGameObject	median = *Objects ((left + right) / 2);
	short	l = left, r = right;

do {
	while (QCmpObjects (Objects (l), &median) < 0)
		l++;
	while (QCmpObjects (Objects (r), &median) > 0)
		r--;
	if (l <= r) {
		if (l < r) {
			CGameObject o = *Objects (l);
			*Objects (l) = *Objects (r);
			*Objects (r) = o;
			if (Current ()->nObject == l)
				Current ()->nObject = r;
			else if (Current ()->nObject == r)
				Current ()->nObject = l;
			}
		l++;
		r--;
		}
	}
while (l < r);
if (l < right)
	QSortObjects (l, right);
if (left < r)
	QSortObjects (left, r);
}


void CMine::SortObjects ()
{
	int	i, j;

if (m_bSortObjects && ( (i = MineInfo ().objects.count) > 1)) {
	for (j = 0; j < i; j++)
		Objects (j)->m_info.signature = j;
	QSortObjects (0, i - 1);
	RenumberObjTriggers ();
	RenumberTriggerTargetObjs ();
	}
}

//------------------------------------------------------------------------
// make_object ()
//
// Action - Defines a standard object (currently assumed to be a player)
//------------------------------------------------------------------------

void CMine::MakeObject (CGameObject *objP, char type, short nSegment) 
{
  CVertex	location;

undoManager.SetModified (TRUE);
undoManager.Lock ();
CalcSegCenter (location,nSegment);
objP->Clear ();
objP->m_info.signature = 0;
objP->m_info.type = type;
objP->m_info.id = (type == OBJ_WEAPON) ? SMALLMINE_ID : 0;
objP->m_info.controlType = CT_NONE; /* player 0 only */
objP->m_info.movementType = MT_PHYSICS;
objP->m_info.renderType = RT_POLYOBJ;
objP->m_info.flags	= 0;
objP->m_info.nSegment = Current ()->nSegment;
objP->m_location.pos = location;
objP->m_location.orient.rVec.Set (F1_0, 0, 0);
objP->m_location.orient.uVec.Set (0, F1_0, 0);
objP->m_location.orient.fVec.Set (0, 0, F1_0);
objP->m_info.size = PLAYER_SIZE;
objP->m_info.shields = DEFAULT_SHIELD;
objP->rType.polyModelInfo.nModel = PLAYER_CLIP_NUMBER;
objP->rType.polyModelInfo.tmap_override = -1;
objP->m_info.contents.type = 0;
objP->m_info.contents.id = 0;
objP->m_info.contents.count = 0;
undoManager.Unlock ();
return;
}

//------------------------------------------------------------------------
// set_object_data ()
//
// Action - Sets control type, movement type, render type
// 	    size, and shields (also nModel & texture if robot)
//------------------------------------------------------------------------

void CMine::SetObjectData (char type) 
{
  CGameObject *objP;
  int  id;

undoManager.SetModified (TRUE);
undoManager.Lock ();
objP = Objects (Current ()->nObject);
id = objP->m_info.id;
memset (&objP->mType, 0, sizeof (objP->mType));
memset (&objP->cType, 0, sizeof (objP->cType));
memset (&objP->rType, 0, sizeof (objP->rType));
switch (type) {
	case OBJ_ROBOT: // an evil enemy
	  objP->m_info.controlType  = CT_AI;
	  objP->m_info.movementType = MT_PHYSICS;
	  objP->m_info.renderType   = RT_POLYOBJ;
	  objP->m_info.size          = robotSize[id];
	  objP->m_info.shields       = robot_shield[id];
	  objP->rType.polyModelInfo.nModel = robotClip[id];
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to none
	  objP->cType.aiInfo.behavior = AIB_NORMAL;
	  break;

	case OBJ_HOSTAGE  : // a hostage you need to rescue
	  objP->m_info.controlType = CT_POWERUP;
	  objP->m_info.movementType = MT_NONE;
	  objP->m_info.renderType   = RT_HOSTAGE;
	  objP->rType.vClipInfo.vclip_num = HOSTAGE_CLIP_NUMBER;
	  objP->m_info.size          = PLAYER_SIZE;
	  objP->m_info.shields       = DEFAULT_SHIELD;
	  break;

	case OBJ_PLAYER   : // the player on the console
	  if (objP->m_info.id == 0) {
		objP->m_info.controlType = CT_NONE; /* player 0 only */
	  } else {
		objP->m_info.controlType = CT_SLEW; /* all other players */
	  }
	  objP->m_info.movementType = MT_PHYSICS;
	  objP->m_info.renderType   = RT_POLYOBJ;
	  objP->m_info.size          = PLAYER_SIZE;
	  objP->m_info.shields       = DEFAULT_SHIELD;
	  objP->rType.polyModelInfo.nModel = PLAYER_CLIP_NUMBER;
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to normal
	  break;

	case OBJ_WEAPON   : // a poly-type weapon
	  objP->m_info.controlType  = CT_WEAPON;
	  objP->m_info.movementType = MT_PHYSICS;
	  objP->m_info.renderType   = RT_POLYOBJ;
	  objP->m_info.size          = WEAPON_SIZE;
	  objP->m_info.shields       = WEAPON_SHIELD;
	  objP->rType.polyModelInfo.nModel = MINE_CLIP_NUMBER;
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to normal
	  objP->mType.physInfo.mass      = 65536L;
	  objP->mType.physInfo.drag      = 2162;
	  objP->mType.physInfo.rotvel.x  = 0;
	  objP->mType.physInfo.rotvel.y  = 46482L;  // don't know exactly what to put here
	  objP->mType.physInfo.rotvel.z  = 0;
	  objP->mType.physInfo.flags     = 260;
	  objP->cType.laserInfo.parent_type      = 5;
	  objP->cType.laserInfo.parent_num       = 146; // don't know exactly what to put here
	  objP->cType.laserInfo.parent_signature = 146; // don't know exactly what to put here
	  break;

	case OBJ_POWERUP  : // a powerup you can pick up
	  objP->m_info.controlType  = CT_POWERUP;
	  objP->m_info.movementType = MT_NONE;
	  objP->m_info.renderType   = RT_POWERUP;
	  objP->rType.vClipInfo.vclip_num = powerupClip[id];
	  objP->m_info.size          = powerupSize[id];
	  objP->m_info.shields       = DEFAULT_SHIELD;
	  break;

	case OBJ_CNTRLCEN : // the reactor
	  objP->m_info.controlType = CT_CNTRLCEN;
	  objP->m_info.movementType = MT_NONE;
	  objP->m_info.renderType   = RT_POLYOBJ;
	  objP->m_info.size          = REACTOR_SIZE;
	  objP->m_info.shields       = REACTOR_SHIELD;
	  if (IsD1File ())
			objP->rType.polyModelInfo.nModel = REACTOR_CLIP_NUMBER;
	  else {
		int model;
		switch (id) {
		  case 1:  model = 95;  break;
		  case 2:  model = 97;  break;
		  case 3:  model = 99;  break;
		  case 4:  model = 101; break;
		  case 5:  model = 103; break;
		  case 6:  model = 105; break;
		  default: model = 97;  break; // level 1's reactor
		}
		objP->rType.polyModelInfo.nModel = model;
	  }
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to none
	  break;

	case OBJ_COOP     : // a cooperative player object
	  objP->m_info.controlType = CT_NONE;
	  objP->m_info.movementType = MT_PHYSICS;
	  objP->m_info.renderType   = RT_POLYOBJ;
	  objP->m_info.size          = PLAYER_SIZE;
	  objP->m_info.shields       = DEFAULT_SHIELD;
	  objP->rType.polyModelInfo.nModel = COOP_CLIP_NUMBER;
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to none
	  break;

	case OBJ_CAMBOT:
	case OBJ_SMOKE:
	case OBJ_MONSTERBALL:
	  objP->m_info.controlType  = CT_AI;
	  objP->m_info.movementType = MT_NONE;
	  objP->m_info.renderType   = RT_POLYOBJ;
	  objP->m_info.size          = robotSize[0];
	  objP->m_info.shields       = DEFAULT_SHIELD;
	  objP->rType.polyModelInfo.nModel = robotClip [0];
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to none
	  objP->cType.aiInfo.behavior = AIB_STILL;
	  break;

	case OBJ_EXPLOSION:
	  objP->m_info.controlType  = CT_POWERUP;
	  objP->m_info.movementType = MT_NONE;
	  objP->m_info.renderType   = RT_POWERUP;
	  objP->m_info.size          = robotSize[0];
	  objP->m_info.shields       = DEFAULT_SHIELD;
	  objP->rType.vClipInfo.vclip_num = VCLIP_BIG_EXPLOSION;
	  objP->rType.polyModelInfo.tmap_override = -1; // set texture to none
	  objP->cType.aiInfo.behavior = AIB_STILL;
	  break;

	case OBJ_EFFECT:
	  objP->m_info.controlType  = CT_NONE;
	  objP->m_info.movementType = MT_NONE;
	  objP->m_info.renderType   = RT_NONE;
	  objP->m_info.size          = f1_0;
	  objP->m_info.shields       = DEFAULT_SHIELD;

  }
	undoManager.Unlock ();
}

//------------------------------------------------------------------------
// copy_object ()
//
// Action - Copies the current object to a new object
//          If object is a player or coop, it chooses the next available id
//
// Parameters - char new_type = new type of object
//
// Returns - TRUE upon success
//
//------------------------------------------------------------------------

bool CMine::CopyObject (byte new_type, short nSegment) 
{
	short objnum,id;
	short ids [MAX_PLAYERS_D2X + MAX_COOP_PLAYERS] = {0,0,0,0,0,0,0,0,0,0,0};
	CGameObject *objP,*current_obj;
	byte type;
	short i,count;

if (MineInfo ().objects.count >= MAX_OBJECTS) {
	ErrorMsg ("The maximum number of objects has already been reached.");
	return false;
	}
// If OBJ_NONE used, then make a copy of the current object type
// otherwise use the type passed as the parameter "new_type"
//--------------------------------------------------------------

type = (new_type == OBJ_NONE) ? CurrObj ()->m_info.type : new_type;

// Make sure it is ok to add another object of this type
// Set the id if it's a player or coop
//------------------------------------------------------
if (type == OBJ_PLAYER || type == OBJ_COOP) {
	objP = Objects (0);
	for (objnum = MineInfo ().objects.count; objnum; objnum--, objP++)
		if (objP->m_info.type == type) {
			id = objP->m_info.id;
			if (id >= 0 && id < (MAX_PLAYERS + MAX_COOP_PLAYERS))
				ids[id]++;
			}
	if (type == OBJ_PLAYER) {
		for (id = 0; (id <= MAX_PLAYERS) && ids[id]; id++)
				;// loop until 1st id with 0
		if (id > MAX_PLAYERS) {
				char szMsg [80];

			sprintf_s (szMsg, sizeof (szMsg), "There are already %d players in the mine", MAX_PLAYERS);
			ErrorMsg (szMsg);
			return FALSE;
			}
		}
	else {
		for (id = MAX_PLAYERS; (id < MAX_PLAYERS + MAX_COOP_PLAYERS) && ids[id]; id++)
			;// loop until 1st id with 0
		if (id > MAX_PLAYERS + MAX_COOP_PLAYERS) {
				char szMsg [80];

			sprintf_s (szMsg, sizeof (szMsg), "There are already %d cooperative players in the mine", MAX_COOP_PLAYERS);
			ErrorMsg (szMsg);
			return FALSE;
			}
		}
	}

// Now we can add the object
// Make a new object
undoManager.SetModified (TRUE);
undoManager.Lock ();
if (MineInfo ().objects.count == 0) {
	MakeObject (Objects (0), OBJ_PLAYER, (nSegment < 0) ? Current ()->nSegment : nSegment);
	MineInfo ().objects.count = 1;
	objnum = 0;
	}
else {
	// Make a copy of the current object
	objnum = MineInfo ().objects.count++;
	objP = Objects (objnum);
	current_obj = CurrObj ();
	memcpy (objP, current_obj, sizeof (CGameObject));
	}
objP->m_info.flags = 0;                                      // new: 1/27/97
objP->m_info.nSegment = Current ()->nSegment;
// set object position in the center of the cube for now
CalcSegCenter (objP->m_location.pos, Current ()->nSegment);
objP->m_location.lastPos = objP->m_location.pos;
Current ()->nObject = objnum;
// bump position over if this is not the first object in the cube
count = 0;
for (i = 0; i < MineInfo ().objects.count - 1; i++)
	if (Objects (i)->m_info.nSegment == Current ()->nSegment)
		count++;
objP->m_location.pos.v.y += count * 2 * F1_0;
objP->m_location.lastPos.v.y += count * 2 * F1_0;
// set the id if new object is a player or a coop
if (type == OBJ_PLAYER || type == OBJ_COOP)
	objP->m_info.id = (char) id;
// set object data if new object being added
if (new_type != OBJ_NONE) {
	objP->m_info.type = new_type;
	SetObjectData (objP->m_info.type);
	}
// set the contents to zero
objP->m_info.contents.type = 0;
objP->m_info.contents.id = 0;
objP->m_info.contents.count = 0;
SortObjects ();
DLE.MineView ()->Refresh (false);
DLE.ToolView ()->ObjectTool ()->Refresh ();
undoManager.Unlock ();
return TRUE;
}

//------------------------------------------------------------------------
// DeleteObject ()
//------------------------------------------------------------------------

void CMine::DeleteObject (short nDelObj)
{
if (MineInfo ().objects.count == 0) {
	if (!bExpertMode)
		ErrorMsg ("There are no Objects () in the mine.");
	return;
	}
if (MineInfo ().objects.count == 1) {
	if (!bExpertMode)
		ErrorMsg ("Cannot delete the last object.");
	return;
	}
if (nDelObj < 0)
	nDelObj = Current ()->nObject;
if (nDelObj == MineInfo ().objects.count) {
	if (!bExpertMode)
		ErrorMsg ("Cannot delete the secret return.");
	return;
	}
undoManager.SetModified (TRUE);
undoManager.Lock ();
DeleteObjTriggers (nDelObj);
int i, j = MineInfo ().objects.count;
for (i = nDelObj; i < j; i++)
	Objects (i)->m_info.signature = i;
if (nDelObj < --j)
	memcpy (Objects (nDelObj), Objects (nDelObj + 1), (MineInfo ().objects.count - nDelObj) * sizeof (CGameObject));
MineInfo ().objects.count = j;
RenumberObjTriggers ();
RenumberTriggerTargetObjs ();
if (Current1 ().nObject >= j)
	Current1 ().nObject = j - 1;
if (Current2 ().nObject >= j)
	Current2 ().nObject = j - 1;
undoManager.Unlock ();
}

//------------------------------------------------------------------------
/// CObjectTool - DrawObject
//------------------------------------------------------------------------

void CMine::DrawObject (CWnd *wndP, int type, int id)
{
	int powerup_lookup[48] = {
		 0, 1, 2, 3, 4, 5, 6,-1,-1,-1,
		 7, 8, 9,10,11,12,13,14,15,16,
		17,18,19,20,-1,21,-1,-1,22,23,
		24,25,26,27,28,29,30,31,32,33,
		34,35,36,37,38,39,40,41
		};
	int object_number;

// figure out object number based on object type and id
object_number = -1; // assume that we can't find the object
switch (type) {
	case OBJ_PLAYER:
		object_number = 0;
		break;
	case OBJ_COOP:
		object_number = 0;
		break;
	case OBJ_ROBOT:
		object_number = (id < 66) ? 1 + id : 118 + (id - 66);
		break;
	case OBJ_CNTRLCEN:
	if (IsD1File ())
		object_number = 67;
	else
		switch (id) {
			case 1: object_number = 68; break;
			case 2: object_number = 69; break;
			case 3: object_number = 70; break;
			case 4: object_number = 71; break;
			case 5: object_number = 72; break;
			case 6: object_number = 73; break;
			default: object_number = 69; break; // level 1's reactor
			}
		break;
	case OBJ_WEAPON:
		object_number = 74;
		break;
	case OBJ_HOSTAGE:
		object_number = 75;
		break;
	case OBJ_POWERUP:
		if ( (id >= 0) && (id < MAX_POWERUP_IDS) && (powerup_lookup[id] >= 0))
			object_number = 76 + powerup_lookup[id];
		break;
	default:
		object_number = -1; // undefined
}

CDC *pDC = wndP->GetDC ();
CRect rc;
wndP->GetClientRect (rc);
pDC->FillSolidRect (&rc, IMG_BKCOLOR);
if ( (object_number >= 0) && (object_number <= 129)) {
	sprintf_s (message, sizeof (message),"OBJ_%03d_BMP", object_number);
	CResource res;
	char *resP = (char*) res.Load (message, RT_BITMAP);
	BITMAPINFO *bmi = (BITMAPINFO *) resP;
	if (bmi) {	//if not, there is a problem in the resource file
		int ncolors = (int) bmi->bmiHeader.biClrUsed;
		if (ncolors == 0)
			ncolors = 1 << (bmi->bmiHeader.biBitCount); // 256 colors for 8-bit data
		char *pImage = resP + sizeof (BITMAPINFOHEADER) + ncolors * 4;
		int width = (int) bmi->bmiHeader.biWidth;
		int height = (int) bmi->bmiHeader.biHeight;
		int xoffset = (64 - width) / 2;
		int yoffset = (64 - height) / 2;
		SetDIBitsToDevice (pDC->m_hDC,  xoffset, yoffset, width, height, 0, 0, 0, height,pImage, bmi, DIB_RGB_COLORS);
		}
	}
wndP->ReleaseDC (pDC);
wndP->InvalidateRect (NULL, TRUE);
wndP->UpdateWindow ();
}

// ------------------------------------------------------------------------

int CObjPhysicsInfo::Read (CFileManager& fp, int version)
{
fp.ReadVector (velocity);
fp.ReadVector (thrust);
mass = fp.ReadInt32 ();
drag = fp.ReadInt32 ();
brakes = fp.ReadInt32 ();
fp.ReadVector (rotvel);
fp.ReadVector (rotthrust);
turnroll = fp.ReadFixAng ();
flags = fp.ReadInt16 ();
return 1;
}

// ------------------------------------------------------------------------

void CObjPhysicsInfo::Write (CFileManager& fp, int version)
{
fp.WriteVector (velocity);
fp.WriteVector (thrust);
fp.WriteInt32 (mass);
fp.WriteInt32 (drag);
fp.WriteInt32 (brakes);
fp.WriteVector (rotvel);
fp.WriteVector (rotthrust);
fp.WriteInt16 (turnroll);
fp.WriteInt16 (flags);
}

// ------------------------------------------------------------------------

int CObjAIInfo::Read (CFileManager& fp, int version)
{
behavior = fp.ReadSByte ();
for (int i = 0; i < MAX_AI_FLAGS; i++)
	flags [i] = fp.ReadSByte ();
hide_segment = fp.ReadInt16 ();
hide_index = fp.ReadInt16 ();
path_length = fp.ReadInt16 ();
cur_path_index = fp.ReadInt16 ();
if (DLE.IsD1File ()) {
	follow_path_start_seg = fp.ReadInt16 ();
	follow_path_end_seg = fp.ReadInt16 ();
	}
return 1;
}

// ------------------------------------------------------------------------

void CObjAIInfo::Write (CFileManager& fp, int version)
{
fp.Write (behavior);
for (int i = 0; i < MAX_AI_FLAGS; i++)
	fp.Write (flags [i]);
fp.Write (hide_segment);
fp.Write (hide_index);
fp.Write (path_length);
fp.Write (cur_path_index);
if (DLE.IsD1File ()) {
	fp.Write (follow_path_start_seg);
	fp.Write (follow_path_end_seg);
	}
}

// ------------------------------------------------------------------------

int CObjExplosionInfo::Read (CFileManager& fp, int version)
{
spawn_time = fp.ReadInt32 ();
delete_time = fp.ReadInt32 ();
delete_objnum = (byte)fp.ReadInt16 ();
next_attach = 
prev_attach = 
attach_parent =-1;
return 1;
}

// ------------------------------------------------------------------------

void CObjExplosionInfo::Write (CFileManager& fp, int version)
{
fp.Write (spawn_time);
fp.Write (delete_time);
fp.Write (delete_objnum);
}

// ------------------------------------------------------------------------

int CObjLaserInfo::Read (CFileManager& fp, int version)
{
parent_type = fp.ReadInt16 ();
parent_num = fp.ReadInt16 ();
parent_signature = fp.ReadInt32 ();
return 1;
}

// ------------------------------------------------------------------------

void CObjLaserInfo::Write (CFileManager& fp, int version)
{
fp.Write (parent_type);
fp.Write (parent_num);
fp.Write (parent_signature);
}

// ------------------------------------------------------------------------

int CObjPowerupInfo::Read (CFileManager& fp, int version)
{
count = (version >= 25) ? fp.ReadInt32 () : 1;
return 1;
}

// ------------------------------------------------------------------------

void CObjPowerupInfo::Write (CFileManager& fp, int version)
{
if (version >= 25) 
	fp.Write (count);
}

// ------------------------------------------------------------------------

int CObjLightInfo::Read (CFileManager& fp, int version)
{
intensity = fp.ReadInt32 ();
return 1;
}

// ------------------------------------------------------------------------

void CObjLightInfo::Write (CFileManager& fp, int version)
{
fp.Write (intensity);
}

// ------------------------------------------------------------------------

int CObjPolyModelInfo::Read (CFileManager& fp, int version)
{
nModel = fp.ReadInt32 ();
for (int i = 0; i < MAX_SUBMODELS; i++)
	fp.ReadVector (anim_angles [i]);
subobj_flags = fp.ReadInt32 ();
tmap_override = fp.ReadInt32 ();
alt_textures = 0;
return 1;
}

// ------------------------------------------------------------------------

void CObjPolyModelInfo::Write (CFileManager& fp, int version)
{
fp.Write (nModel);
for (int i = 0; i < MAX_SUBMODELS; i++)
	fp.WriteVector (anim_angles [i]);
fp.Write (subobj_flags);
fp.Write (tmap_override);
}

// ------------------------------------------------------------------------

int CObjVClipInfo::Read (CFileManager& fp, int version)
{
vclip_num = fp.ReadInt32 ();
frametime = fp.ReadInt32 ();
framenum = fp.ReadSByte ();
return 1;
}

// ------------------------------------------------------------------------

void CObjVClipInfo::Write (CFileManager& fp, int version)
{
fp.Write (vclip_num);
fp.Write (frametime);
fp.Write (framenum);
}

// ------------------------------------------------------------------------

int CSmokeInfo::Read (CFileManager& fp, int version)
{
nLife = fp.ReadInt32 ();
nSize [0] = fp.ReadInt32 ();
nParts = fp.ReadInt32 ();
nSpeed = fp.ReadInt32 ();
nDrift = fp.ReadInt32 ();
nBrightness = fp.ReadInt32 ();
for (int i = 0; i < 4; i++)
	color [i] = fp.ReadSByte ();
nSide = fp.ReadSByte ();
nType = (version < 18) ? 0 : fp.ReadSByte ();
bEnabled = (version < 19) ? 1 : fp.ReadSByte ();
return 1;
}

// ------------------------------------------------------------------------

void CSmokeInfo::Write (CFileManager& fp, int version)
{
fp.Write (nLife);
fp.Write (nSize [0]);
fp.Write (nParts);
fp.Write (nSpeed);
fp.Write (nDrift);
fp.Write (nBrightness);
for (int i = 0; i < 4; i++)
	fp.Write (color [i]);
fp.Write (nSide);
fp.Write (nSide);
fp.Write (bEnabled);
}

// ------------------------------------------------------------------------

int CLightningInfo::Read (CFileManager& fp, int version)
{
nLife = fp.ReadInt32 ();
nDelay = fp.ReadInt32 ();
nLength = fp.ReadInt32 ();
nAmplitude = fp.ReadInt32 ();
nOffset = fp.ReadInt32 ();
nLightnings = fp.ReadInt16 ();
nId = fp.ReadInt16 ();
nTarget = fp.ReadInt16 ();
nNodes = fp.ReadInt16 ();
nChildren = fp.ReadInt16 ();
nSteps = fp.ReadInt16 ();
nAngle = fp.ReadSByte ();
nStyle = fp.ReadSByte ();
nSmoothe = fp.ReadSByte ();
bClamp = fp.ReadSByte ();
bPlasma = fp.ReadSByte ();
bSound = fp.ReadSByte ();
bRandom = fp.ReadSByte ();
bInPlane = fp.ReadSByte ();
for (int i = 0; i < 4; i++)
	color [i] = fp.ReadSByte ();
bEnabled = (version < 19) ? 1 : fp.ReadSByte ();
return 1;
}

// ------------------------------------------------------------------------

void CLightningInfo::Write (CFileManager& fp, int version)
{
fp.Write (nLife);
fp.Write (nDelay);
fp.Write (nLength);
fp.Write (nAmplitude);
fp.Write (nOffset);
fp.Write (nLightnings);
fp.Write (nId);
fp.Write (nTarget);
fp.Write (nNodes);
fp.Write (nChildren);
fp.Write (nSteps);
fp.Write (nAngle);
fp.Write (nStyle);
fp.Write (nSmoothe);
fp.Write (bClamp);
fp.Write (bPlasma);
fp.Write (bSound);
fp.Write (bRandom);
fp.Write (bInPlane);
for (int i = 0; i < 4; i++)
	fp.Write (color [i]);
fp.Write (bEnabled);
}

// ------------------------------------------------------------------------

int CSoundInfo::Read (CFileManager& fp, int version)
{
fp.Read (szFilename, 1, sizeof (szFilename));
nVolume = fp.ReadInt32 ();
bEnabled = (version < 19) ? 1 : fp.ReadSByte ();
return 1;
}
// ------------------------------------------------------------------------

void CSoundInfo::Write (CFileManager& fp, int version)
{
fp.Write (szFilename, 1, sizeof (szFilename));
fp.Write (nVolume);
fp.Write (bEnabled);
}

// ------------------------------------------------------------------------

int CGameObject::Read (CFileManager& fp, int version, bool bFlag) 
{
m_info.type = fp.ReadSByte ();
m_info.id = fp.ReadSByte ();
m_info.controlType = fp.ReadSByte ();
m_info.movementType = fp.ReadSByte ();
m_info.renderType = fp.ReadSByte ();
m_info.flags = fp.ReadSByte ();
m_info.multiplayer = (version > 37) ? fp.ReadSByte () : 0;
m_info.nSegment = fp.ReadInt16 ();
m_location.pos.Read (fp);
fp.Read (m_location.orient);
m_info.size = fp.ReadInt32 ();
m_info.shields = fp.ReadInt32 ();
m_location.lastPos.Read (fp);
m_info.contents.type = fp.ReadSByte ();
m_info.contents.id = fp.ReadSByte ();
m_info.contents.count = fp.ReadSByte ();

switch (m_info.movementType) {
	case MT_PHYSICS:
		mType.physInfo.Read (fp, version);
		break;
	case MT_SPINNING:
		fp.ReadVector (mType.spinRate);
		break;
	case MT_NONE:
		break;
	default:
		break;
	}

switch (m_info.controlType) {
	case CT_AI:
		cType.aiInfo.Read (fp, version);
		break;
	case CT_EXPLOSION:
		cType.explInfo.Read (fp, version);
		break;
	case CT_WEAPON:
		cType.laserInfo.Read (fp, version);
		break;
	case CT_LIGHT:
		cType.lightInfo.Read (fp, version);
		break;
	case CT_POWERUP:
		cType.powerupInfo.Read (fp, version);
		break;
	case CT_NONE:
	case CT_FLYING:
	case CT_DEBRIS:
		break;
	case CT_SLEW:    /*the player is generally saved as slew */
		break;
	case CT_CNTRLCEN:
		break;
	case CT_MORPH:
	case CT_FLYTHROUGH:
	case CT_REPAIRCEN:
		default:
		break;
	}

switch (m_info.renderType) {
	case RT_NONE:
		break;
	case RT_MORPH:
	case RT_POLYOBJ: 
		rType.polyModelInfo.Read (fp, version);
		break;
	case RT_WEAPON_VCLIP:
	case RT_HOSTAGE:
	case RT_POWERUP:
	case RT_FIREBALL:
		rType.vClipInfo.Read (fp, DLE.LevelVersion ());
		break;
	case RT_LASER:
		break;
	case RT_SMOKE:
		rType.smokeInfo.Read (fp, DLE.LevelVersion ());
		break;
	case RT_LIGHTNING:
		rType.lightningInfo.Read (fp, DLE.LevelVersion ());
		break;
	case RT_SOUND:
		rType.soundInfo.Read (fp, DLE.LevelVersion ());
		break;
	default:
	break;
	}

return 1;
}

// ------------------------------------------------------------------------
// WriteObject ()
// ------------------------------------------------------------------------

void CGameObject::Write (CFileManager& fp, int version, bool bFlag)
{
if (theMine->IsStdLevel () && (m_info.type >= OBJ_CAMBOT))
	return;	// not a d2x-xl level, but a d2x-xl object

fp.Write (m_info.type);
fp.Write (m_info.id);
fp.Write (m_info.controlType);
fp.Write (m_info.movementType);
fp.Write (m_info.renderType);
fp.Write (m_info.flags);
fp.Write (m_info.multiplayer);
fp.Write (m_info.nSegment);
fp.Write (m_location.pos);
fp.Write (m_location.orient);
fp.Write (m_info.size);
fp.Write (m_info.shields);
fp.Write (m_location.lastPos);
fp.Write (m_info.contents.type);
fp.Write (m_info.contents.id);
fp.Write (m_info.contents.count);

switch (m_info.movementType) {
	case MT_PHYSICS:
		mType.physInfo.Write (fp, version);
		break;
	case MT_SPINNING:
		fp.Write (mType.spinRate);
		break;
	case MT_NONE:
		break;
	default:
		break;
	}

switch (m_info.controlType) {
	case CT_AI:
		cType.aiInfo.Write (fp, version);
		break;
	case CT_EXPLOSION:
		cType.explInfo.Write (fp, version);
		break;
	case CT_WEAPON:
		cType.laserInfo.Write (fp, version);
		break;
	case CT_LIGHT:
		cType.lightInfo.Write (fp, version);
		break;
	case CT_POWERUP:
		cType.powerupInfo.Write (fp, version);
		break;
	case CT_NONE:
	case CT_FLYING:
	case CT_DEBRIS:
		break;
	case CT_SLEW:    /*the player is generally saved as slew */
		break;
	case CT_CNTRLCEN:
		break;
	case CT_MORPH:
	case CT_FLYTHROUGH:
	case CT_REPAIRCEN:
		default:
		break;
	}

switch (m_info.renderType) {
	case RT_NONE:
		break;
	case RT_MORPH:
	case RT_POLYOBJ:
		rType.polyModelInfo.Write (fp, version);
	break;
	case RT_WEAPON_VCLIP:
	case RT_HOSTAGE:
	case RT_POWERUP:
	case RT_FIREBALL:
		rType.vClipInfo.Write (fp, version);
		break;
	case RT_LASER:
		break;
	case RT_SMOKE:
		rType.smokeInfo.Write (fp, version);
		break;
	case RT_LIGHTNING:
		rType.lightningInfo.Write (fp, version);
		break;
	case RT_SOUND:
		rType.soundInfo.Write (fp, version);
		break;
	default:
		break;
	}
}

// ------------------------------------------------------------------------
// eof object.cpp
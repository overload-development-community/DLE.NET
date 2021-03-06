#include "stdafx.h"
#include "GameObject.h"
#include "textures.h"
#include "PolyModel.h"

static int nDbgObject = -1;

// -----------------------------------------------------------------------------

int powerupSize [MAX_POWERUP_IDS_D2] = {
	0x28000, 0x30000, 0x28000, 0x40000, 0x30000, 0x30000, 0x30000, 0x30000, 
	0x30000, 0x30000, 0x28000, 0x30000, 0x30000, 0x40000, 0x40000, 0x40000, 
	0x40000, 0x30000, 0x28000, 0x30000, 0x28000, 0x30000, 0x1cccc, 0x20000, 
	0x30000, 0x29999, 0x30000, 0x30000, 0x40000, 0x40000, 0x40000, 0x40000, 
	0x40000, 0x40000, 0x40000, 0x40000, 0x48000, 0x30000, 0x28000, 0x28000, 
	0x30000, 0x30000, 0x40000, 0x40000, 0x40000, 0x40000, 0x38000, 0x38000
};


int robotSize [MAX_ROBOT_IDS_TOTAL] = {
	399147, 368925, 454202, 316909, 328097, 345407, 399147, 293412, 
	300998, 308541, 246493, 283415, 283415, 227232, 200000, 598958, 
	399147, 1597221, 290318, 345407, 323879, 339488, 294037, 1443273, 
	378417, 408340, 586422, 302295, 524232, 405281, 736493, 892216, 
	400000, 204718, 400000, 400000, 354534, 236192, 373267, 351215, 
	429512, 169251, 310419, 378181, 381597, 1101683, 853047, 423359, 
	402717, 289744, 187426, 361065, 994374, 758384, 429512, 408340, 
	289744, 408340, 289744, 400000, 402717, 169251, 1312272, 169251, 
	905234, 1014749, 
	374114, 318151, 377386, 492146, 257003, 403683,     // vertigo robots
	342424, 322628, 332831, 1217722, 907806, 378960    // vertigo robots
};

int robotShield [MAX_ROBOT_IDS_TOTAL] = {
	6553600, 6553600, 6553600, 1638400, 2293760, 6553600, 9830400, 16384000, 
	2293760, 16384000, 2293760, 2293760, 2000000, 9830400, 1310720, 26214400, 
	21299200, 131072000, 6553600, 3276800, 3276800, 4587520, 4587520, 327680000, 
	5570560, 5242880, 9830400, 2949120, 6553600, 6553600, 7864320, 196608000, 
	5000000, 45875200, 5000000, 5000000, 5242880, 786432, 1966080, 4587520, 
	9830400, 1310720, 29491200, 9830400, 11796480, 262144000, 262144000, 13107200, 
	7208960, 655360, 983040, 11141120, 294912000, 32768000, 7864320, 3932160, 
	4587520, 5242880, 4587520, 5000000, 7208960, 1310720, 196608000, 1310720, 
	294912000, 19660800, 
	6553600, 6553600, 6553600, 10485760, 4587520, 16384000,   // vertigo robots
	6553600, 7864320, 7864320, 180224000, 360448000, 9830400 // vertigo robots
};

ubyte robotClip[MAX_ROBOT_IDS_TOTAL] = {
	0x00, 0x02, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 
	0x0e, 0x10, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 
	0x19, 0x1b, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 
	40, 41, 43, 44, 45, 46, 47, 49, 
	50, 51, 52, 53, 55, 56, 58, 60, 	// 50,  52,  53,  and 86 were guessed at but seem to work ok
	62, 64, 65, 67, 68, 69, 70, 71, 
	72, 73, 74, 75, 76, 77, 78, 80, 
	82, 83, 85, 86, 87, 88, 89, 90, 
	91, 92, 
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1  // vertigo clip numbers
};

// note1: 0 == not used,
// note2: 100 and 101 are flags but will appear as shields
//        in non multiplayer level
ubyte powerupClip [MAX_POWERUP_IDS_D2] = {
	36, 18, 19, 20, 24, 26, 25,  0,
	 0,  0, 34, 35, 51, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47,
	 0, 49,  0,  0, 69, 70, 71, 72,
	77, 73, 74, 75, 76, 83, 78, 89,
	79, 90, 91, 81,102, 82,100,101
};

ubyte powerupTypeTable [MAX_POWERUP_IDS_D2] = {
		POWERUP_POWERUP_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_KEY_MASK,
		POWERUP_KEY_MASK,
		POWERUP_KEY_MASK,
		POWERUP_UNKNOWN_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_UNKNOWN_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_UNKNOWN_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_UNKNOWN_MASK,
		POWERUP_UNKNOWN_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_WEAPON_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_POWERUP_MASK,
		POWERUP_POWERUP_MASK
};

// -----------------------------------------------------------------------------

void CGameObject::Create (ubyte type, short nSegment) 
{
  CVertex	location;

g_data.undoManager->Begin (__FUNCTION__, udObjects);
g_data.segmentManager->CalcCenter (location, nSegment);
Clear ();
m_info.signature = 0;
m_info.type = type;
m_info.id = (type == OBJ_WEAPON) ? SMALLMINE_ID : 0;
m_info.controlType = CT_NONE; /* player 0 only */
m_info.movementType = MT_PHYSICS;
m_info.renderType = RT_POLYOBJ;
m_info.flags	= 0;
m_info.nSegment = nSegment;
m_location.pos = location;
m_location.orient.m.rVec.Set (I2X (1), 0, 0);
m_location.orient.m.uVec.Set (0, I2X (1), 0);
m_location.orient.m.fVec.Set (0, 0, I2X (1));
m_info.size = PLAYER_SIZE;
m_info.shields = DEFAULT_SHIELD;
rType.polyModelInfo.nModel = PLAYER_CLIP_NUMBER;
rType.polyModelInfo.nOverrideTexture = -1;
m_info.contents.type = 0;
m_info.contents.id = 0;
m_info.contents.count = 0;
g_data.undoManager->End (__FUNCTION__);
return;
}

// -----------------------------------------------------------------------------

void CGameObject::Setup (ubyte type) 
{
  int  id;

g_data.undoManager->Begin (__FUNCTION__, udObjects);
id = m_info.id;
memset (&mType, 0, sizeof (mType));
memset (&cType, 0, sizeof (cType));
memset (&rType, 0, sizeof (rType));
m_info.type = type;
switch (type) {
	case OBJ_ROBOT: // an evil enemy
		m_info.controlType = CT_AI;
		m_info.movementType = MT_PHYSICS;
		m_info.renderType = RT_POLYOBJ;
		m_info.size = robotSize[id];
		m_info.shields = robotShield[id];
		rType.polyModelInfo.nModel = robotClip[id];
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to none
		cType.aiInfo.behavior = AIB_NORMAL;
		break;

	case OBJ_HOSTAGE: // a hostage you need to rescue
		m_info.controlType = CT_POWERUP;
		m_info.movementType = MT_NONE;
		m_info.renderType = RT_HOSTAGE;
		rType.animationInfo.nAnimation = HOSTAGE_CLIP_NUMBER;
		m_info.size = PLAYER_SIZE;
		m_info.shields = DEFAULT_SHIELD;
		break;

	case OBJ_PLAYER: // the player on the console
		m_info.controlType = (m_info.id == 0) ? CT_NONE : CT_SLEW; /* player 0 only */
		m_info.movementType = MT_PHYSICS;
		m_info.renderType = RT_POLYOBJ;
		m_info.size = PLAYER_SIZE;
		m_info.shields = DEFAULT_SHIELD;
		rType.polyModelInfo.nModel = PLAYER_CLIP_NUMBER;
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to normal
		break;

	case OBJ_WEAPON: // a poly-type weapon
		m_info.controlType = CT_WEAPON;
		m_info.movementType = MT_PHYSICS;
		m_info.renderType = RT_POLYOBJ;
		m_info.size = WEAPON_SIZE;
		m_info.shields = WEAPON_SHIELD;
		rType.polyModelInfo.nModel = MINE_CLIP_NUMBER;
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to normal
		mType.physInfo.mass = 65536L;
		mType.physInfo.drag = 2162;
		mType.physInfo.rotVel.x = 0;
		mType.physInfo.rotVel.y = 46482L;  // don't know exactly what to put here
		mType.physInfo.rotVel.z = 0;
		mType.physInfo.flags = 260;
		cType.laserInfo.parent_type = 5;
		cType.laserInfo.parent_num = 146; // don't know exactly what to put here
		cType.laserInfo.parent_signature = 146; // don't know exactly what to put here
		break;

	case OBJ_POWERUP: // a powerup you can pick up
		m_info.controlType = CT_POWERUP;
		m_info.movementType = MT_NONE;
		m_info.renderType = RT_POWERUP;
		rType.animationInfo.nAnimation = powerupClip[id];
		m_info.size = powerupSize[id];
		m_info.shields = DEFAULT_SHIELD;
		break;

	case OBJ_REACTOR: // the reactor
		m_info.controlType = CT_CNTRLCEN;
		m_info.movementType = MT_NONE;
		m_info.renderType = RT_POLYOBJ;
		m_info.size = REACTOR_SIZE;
		m_info.shields = REACTOR_SHIELD;
		if (g_data.IsD1File ())
			rType.polyModelInfo.nModel = REACTOR_CLIP_NUMBER;
		else {
			int model;
			switch (id) {
				case 0:  model = 93;  break;
				case 1:  model = 95;  break;
				case 2:  model = 97;  break;
				case 3:  model = 99;  break;
				case 4:  model = 101; break;
				case 5:  model = 103; break;
				case 6:  model = 105; break;
				default: model = 97;  break; // level 1's reactor
				}
			rType.polyModelInfo.nModel = model;
			}
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to none
		break;

	case OBJ_COOP: // a cooperative player object
		m_info.controlType = CT_NONE;
		m_info.movementType = MT_PHYSICS;
		m_info.renderType = RT_POLYOBJ;
		m_info.size = PLAYER_SIZE;
		m_info.shields = DEFAULT_SHIELD;
		rType.polyModelInfo.nModel = COOP_CLIP_NUMBER;
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to none
		break;

	case OBJ_CAMBOT:
	case OBJ_SMOKE:
	case OBJ_MONSTERBALL:
		m_info.controlType = CT_AI;
		m_info.movementType = MT_NONE;
		m_info.renderType = RT_POLYOBJ;
		m_info.size = robotSize[0];
		m_info.shields = DEFAULT_SHIELD;
		rType.polyModelInfo.nModel = robotClip [0];
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to none
		cType.aiInfo.behavior = AIB_STILL;
		break;

	case OBJ_EXPLOSION:
		m_info.controlType = CT_POWERUP;
		m_info.movementType = MT_NONE;
		m_info.renderType = RT_POWERUP;
		m_info.size = robotSize[0];
		m_info.shields = DEFAULT_SHIELD;
		rType.animationInfo.nAnimation = VCLIP_BIG_EXPLOSION;
		rType.polyModelInfo.nOverrideTexture = -1; // set texture to none
		cType.aiInfo.behavior = AIB_STILL;
		break;

	case OBJ_EFFECT:
		m_info.controlType = CT_NONE;
		m_info.movementType = MT_NONE;
		m_info.renderType = RT_NONE;
		m_info.size = f1_0;
		m_info.shields = DEFAULT_SHIELD;
	}
g_data.undoManager->End (__FUNCTION__);
}

// -----------------------------------------------------------------------------

void CObjPhysicsInfo::Read (IFileManager* fp)
{
fp->ReadVector (velocity);
fp->ReadVector (thrust);
mass = fp->ReadInt32 ();
drag = fp->ReadInt32 ();
brakes = fp->ReadInt32 ();
fp->ReadVector (rotVel);
fp->ReadVector (rotThrust);
turnRoll = fp->ReadFixAng ();
flags = fp->ReadInt16 ();
}

// -----------------------------------------------------------------------------

void CObjPhysicsInfo::Write (IFileManager* fp)
{
fp->WriteVector (velocity);
fp->WriteVector (thrust);
fp->WriteInt32 (mass);
fp->WriteInt32 (drag);
fp->WriteInt32 (brakes);
fp->WriteVector (rotVel);
fp->WriteVector (rotThrust);
fp->WriteInt16 (turnRoll);
fp->WriteInt16 (flags);
}

// -----------------------------------------------------------------------------

void CObjAIInfo::Read (IFileManager* fp)
{
behavior = fp->ReadSByte ();
for (int i = 0; i < MAX_AI_FLAGS; i++)
	flags [i] = fp->ReadSByte ();
hide_segment = fp->ReadInt16 ();
hide_index = fp->ReadInt16 ();
path_length = fp->ReadInt16 ();
cur_path_index = fp->ReadInt16 ();
if (g_data.IsD1File ()) {
	follow_path_start_seg = fp->ReadInt16 ();
	follow_path_end_seg = fp->ReadInt16 ();
	}
}

// ------------------------------------------------------------------------

void CObjAIInfo::Write (IFileManager* fp)
{
fp->Write (behavior);
for (int i = 0; i < MAX_AI_FLAGS; i++)
	fp->Write (flags [i]);
fp->Write (hide_segment);
fp->Write (hide_index);
fp->Write (path_length);
fp->Write (cur_path_index);
if (g_data.IsD1File ()) {
	fp->Write (follow_path_start_seg);
	fp->Write (follow_path_end_seg);
	}
}

// ------------------------------------------------------------------------

void CObjExplosionInfo::Read (IFileManager* fp)
{
spawn_time = fp->ReadInt32 ();
delete_time = fp->ReadInt32 ();
delete_objnum = (ubyte)fp->ReadInt16 ();
next_attach = 
prev_attach = 
attach_parent =-1;
}

// ------------------------------------------------------------------------

void CObjExplosionInfo::Write (IFileManager* fp)
{
fp->Write (spawn_time);
fp->Write (delete_time);
fp->Write (delete_objnum);
}

// ------------------------------------------------------------------------

void CObjLaserInfo::Read (IFileManager* fp)
{
parent_type = fp->ReadInt16 ();
parent_num = fp->ReadInt16 ();
parent_signature = fp->ReadInt32 ();
}

// ------------------------------------------------------------------------

void CObjLaserInfo::Write (IFileManager* fp)
{
fp->Write (parent_type);
fp->Write (parent_num);
fp->Write (parent_signature);
}

// ------------------------------------------------------------------------

void CObjPowerupInfo::Read (IFileManager* fp)
{
count = (g_data.FileVersion () >= 25) ? fp->ReadInt32 () : 1;
}

// ------------------------------------------------------------------------

void CObjPowerupInfo::Write (IFileManager* fp)
{
if (g_data.FileVersion () >= 25) 
	fp->Write (count);
}

// ------------------------------------------------------------------------

void CObjLightInfo::Read (IFileManager* fp)
{
intensity = fp->ReadInt32 ();
}

// ------------------------------------------------------------------------

void CObjLightInfo::Write (IFileManager* fp)
{
fp->Write (intensity);
}

// ------------------------------------------------------------------------

void CObjPolyModelInfo::Read (IFileManager* fp)
{
nModel = fp->ReadInt32 ();
for (int i = 0; i < MAX_SUBMODELS; i++)
	fp->ReadVector (anim_angles [i]);
subobj_flags = fp->ReadInt32 ();
nOverrideTexture = fp->ReadInt32 ();
alt_textures = 0;
}

// ------------------------------------------------------------------------

void CObjPolyModelInfo::Write (IFileManager* fp)
{
fp->Write (nModel);
for (int i = 0; i < MAX_SUBMODELS; i++)
	fp->WriteVector (anim_angles [i]);
fp->Write (subobj_flags);
fp->Write (nOverrideTexture);
}

// ------------------------------------------------------------------------

void CObjAnimationInfo::Read (IFileManager* fp)
{
nAnimation = fp->ReadInt32 ();
nFrameTime = fp->ReadInt32 ();
nFrame = fp->ReadSByte ();
}

// ------------------------------------------------------------------------

void CObjAnimationInfo::Write (IFileManager* fp)
{
fp->Write (nAnimation);
fp->Write (nFrameTime);
fp->Write (nFrame);
}

// ------------------------------------------------------------------------

void CParticleInfo::Read (IFileManager* fp)
{
nLife = fp->ReadInt32 ();
nSize [0] = fp->ReadInt32 ();
nParts = fp->ReadInt32 ();
nSpeed = fp->ReadInt32 ();
nDrift = fp->ReadInt32 ();
nBrightness = fp->ReadInt32 ();
for (int i = 0; i < 4; i++)
	color [i] = fp->ReadSByte ();
nSide = fp->ReadSByte ();
nType = (g_data.FileVersion () < 18) ? 0 : fp->ReadSByte ();
bEnabled = (g_data.LevelVersion () < 19) ? 1 : fp->ReadSByte ();
}

// ------------------------------------------------------------------------

void CParticleInfo::Write (IFileManager* fp)
{
fp->Write (nLife);
fp->Write (nSize [0]);
fp->Write (nParts);
fp->Write (nSpeed);
fp->Write (nDrift);
fp->Write (nBrightness);
for (int i = 0; i < 4; i++)
	fp->Write (color [i]);
fp->Write (nSide);
fp->Write (nType);
fp->Write (bEnabled);
}

// ------------------------------------------------------------------------

void CLightningInfo::Read (IFileManager* fp)
{
nLife = fp->ReadInt32 ();
nDelay = fp->ReadInt32 ();
nLength = fp->ReadInt32 ();
nAmplitude = fp->ReadInt32 ();
nOffset = fp->ReadInt32 ();
nWayPoint = (g_data.LevelVersion () < 23) ? -1 : fp->ReadInt32 ();
nBolts = fp->ReadInt16 ();
nId = fp->ReadInt16 ();
nTarget = fp->ReadInt16 ();
nNodes = fp->ReadInt16 ();
nChildren = fp->ReadInt16 ();
nFrames = fp->ReadInt16 ();
nWidth = (g_data.LevelVersion () < 22) ? 3 : fp->ReadUByte ();
nAngle = fp->ReadSByte ();
nStyle = fp->ReadSByte ();
nSmoothe = fp->ReadSByte ();
bClamp = fp->ReadSByte ();
bPlasma = fp->ReadSByte ();
bSound = fp->ReadSByte ();
bRandom = fp->ReadSByte ();
bInPlane = fp->ReadSByte ();
for (int i = 0; i < 4; i++)
	color [i] = fp->ReadSByte ();
bEnabled = (g_data.LevelVersion () < 19) ? 1 : fp->ReadSByte ();
}

// ------------------------------------------------------------------------

void CLightningInfo::Write (IFileManager* fp)
{
fp->Write (nLife);
fp->Write (nDelay);
fp->Write (nLength);
fp->Write (nAmplitude);
fp->Write (nOffset);
fp->Write (nWayPoint);
fp->Write (nBolts);
fp->Write (nId);
fp->Write (nTarget);
fp->Write (nNodes);
fp->Write (nChildren);
fp->Write (nFrames);
fp->Write (nWidth);
fp->Write (nAngle);
fp->Write (nStyle);
fp->Write (nSmoothe);
fp->Write (bClamp);
fp->Write (bPlasma);
fp->Write (bSound);
fp->Write (bRandom);
fp->Write (bInPlane);
for (int i = 0; i < 4; i++)
	fp->Write (color [i]);
fp->Write (bEnabled);
}

// ------------------------------------------------------------------------

void CSoundInfo::Read (IFileManager* fp)
{
fp->Read (szFilename, 1, sizeof (szFilename));
nVolume = fp->ReadInt32 ();
bEnabled = (g_data.LevelVersion () < 19) ? 1 : fp->ReadSByte ();
}
// ------------------------------------------------------------------------

void CSoundInfo::Write (IFileManager* fp)
{
fp->Write (szFilename, 1, sizeof (szFilename));
fp->Write (nVolume);
fp->Write (bEnabled);
}

// ------------------------------------------------------------------------

void CWayPointInfo::Read (IFileManager* fp)
{
nId = fp->ReadInt32 ();
nSuccessor = fp->ReadInt32 ();
nSpeed = fp->ReadInt32 ();
}

// ------------------------------------------------------------------------

void CWayPointInfo::Write (IFileManager* fp)
{
fp->Write (nId);
fp->Write (nSuccessor);
fp->Write (nSpeed);
}

// ------------------------------------------------------------------------

bool CGameObject::CheckEffectType (bool bFix)
{
if (m_info.type != OBJ_EFFECT)
	return true;

	bool bResult = false;

if (m_info.type == OBJ_EFFECT) {
	switch (m_info.id) {
		default:

		case PARTICLE_ID:
			if (m_info.renderType == RT_PARTICLE)
				bResult = true;
			else if (bFix)
				m_info.renderType = RT_PARTICLE;
			break;

		case LIGHTNING_ID:
			if (m_info.renderType == RT_LIGHTNING)
				bResult = true;
			else if (bFix)
				m_info.renderType = RT_LIGHTNING;
			break;

		case SOUND_ID:
			if (m_info.renderType == RT_SOUND)
				bResult = true;
			else if (bFix)
				m_info.renderType = RT_SOUND;
			break;
		}
	}
return bResult;
}

// ------------------------------------------------------------------------

void CGameObject::Read (IFileManager* fp, bool bFlag) 
{
m_info.type = fp->ReadSByte ();
m_info.id = fp->ReadSByte ();
m_info.controlType = fp->ReadSByte ();
m_info.movementType = fp->ReadSByte ();
m_info.renderType = fp->ReadSByte ();
m_info.flags = fp->ReadSByte ();
m_info.multiplayer = (g_data.FileVersion () > 37) ? fp->ReadSByte () : 0;
m_info.nSegment = fp->ReadInt16 ();
m_location.pos.Read (fp);
fp->Read (m_location.orient);
m_info.size = fp->ReadInt32 ();
m_info.shields = fp->ReadInt32 ();
m_location.lastPos.Read (fp);
m_info.contents.type = fp->ReadSByte ();
m_info.contents.id = fp->ReadSByte ();
m_info.contents.count = fp->ReadSByte ();

switch (m_info.movementType) {
	case MT_PHYSICS:
		mType.physInfo.Read (fp);
		break;
	case MT_SPINNING:
		fp->ReadVector (mType.spinRate);
		break;
	case MT_NONE:
		break;
	default:
		break;
	}

switch (m_info.controlType) {
	case CT_AI:
		cType.aiInfo.Read (fp);
		break;
	case CT_EXPLOSION:
		cType.explInfo.Read (fp);
		break;
	case CT_WEAPON:
		cType.laserInfo.Read (fp);
		break;
	case CT_LIGHT:
		cType.lightInfo.Read (fp);
		break;
	case CT_POWERUP:
		cType.powerupInfo.Read (fp);
		break;
	case CT_NONE:
	case CT_FLYING:
	case CT_DEBRIS:
		break;
	case CT_SLEW:    /*the player is generally saved as slew */
		break;
	case CT_CNTRLCEN:
		break;
	case CT_WAYPOINT:
		cType.wayPointInfo.Read (fp);
		// Fix IDs from old D2X-XL levels
		if (m_info.id != WAYPOINT_ID)
			m_info.id = WAYPOINT_ID;
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
		rType.polyModelInfo.Read (fp);
		break;
	case RT_WEAPON_VCLIP:
	case RT_HOSTAGE:
	case RT_POWERUP:
	case RT_FIREBALL:
		rType.animationInfo.Read (fp);
		break;
	case RT_LASER:
		break;
	case RT_PARTICLE:
		rType.particleInfo.Read (fp);
		break;
	case RT_LIGHTNING:
		rType.lightningInfo.Read (fp);
		break;
	case RT_SOUND:
		rType.soundInfo.Read (fp);
		// Fix IDs from old D2X-XL levels
		if (m_info.id != SOUND_ID)
			m_info.id = SOUND_ID;
		break;
	default:
	break;
	}
}

// ------------------------------------------------------------------------

void CGameObject::Write (IFileManager* fp, bool bFlag)
{
if (g_data.IsStdLevel () && (m_info.type >= OBJ_CAMBOT))
	return;	// not a d2x-xl level, but a d2x-xl object

fp->Write (m_info.type);
fp->Write (m_info.id);
fp->Write (m_info.controlType);
fp->Write (m_info.movementType);
fp->Write (m_info.renderType);
fp->Write (m_info.flags);
if (g_data.FileVersion () > 37)
	fp->Write (m_info.multiplayer);
fp->Write (m_info.nSegment);
fp->Write (m_location.pos);
fp->Write (m_location.orient);
fp->Write (m_info.size);
fp->Write (m_info.shields);
fp->Write (m_location.lastPos);
fp->Write (m_info.contents.type);
fp->Write (m_info.contents.id);
fp->Write (m_info.contents.count);

switch (m_info.movementType) {
	case MT_PHYSICS:
		mType.physInfo.Write (fp);
		break;
	case MT_SPINNING:
		fp->Write (mType.spinRate);
		break;
	case MT_NONE:
		break;
	default:
		break;
	}

switch (m_info.controlType) {
	case CT_AI:
		cType.aiInfo.Write (fp);
		break;
	case CT_EXPLOSION:
		cType.explInfo.Write (fp);
		break;
	case CT_WEAPON:
		cType.laserInfo.Write (fp);
		break;
	case CT_LIGHT:
		cType.lightInfo.Write (fp);
		break;
	case CT_POWERUP:
		cType.powerupInfo.Write (fp);
		break;
	case CT_NONE:
	case CT_FLYING:
	case CT_DEBRIS:
		break;
	case CT_SLEW:    /*the player is generally saved as slew */
		break;
	case CT_CNTRLCEN:
		break;
	case CT_WAYPOINT:
		cType.wayPointInfo.Write (fp);
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
		rType.polyModelInfo.Write (fp);
	break;
	case RT_WEAPON_VCLIP:
	case RT_HOSTAGE:
	case RT_POWERUP:
	case RT_FIREBALL:
		rType.animationInfo.Write (fp);
		break;
	case RT_LASER:
		break;
	case RT_PARTICLE:
		rType.particleInfo.Write (fp);
		break;
	case RT_LIGHTNING:
		rType.lightningInfo.Write (fp);
		break;
	case RT_SOUND:
		rType.soundInfo.Write (fp);
		break;
	default:
		break;
	}
}

// -----------------------------------------------------------------------------

CVertex& CGameObject::Transform (CVertex& vDest, const CVertex& vSource)
{
vDest.v.x = m_location.orient.m.rVec.v.x * vSource.v.x + m_location.orient.m.uVec.v.x * vSource.v.y + m_location.orient.m.fVec.v.x * vSource.v.z;
vDest.v.y = m_location.orient.m.rVec.v.y * vSource.v.x + m_location.orient.m.uVec.v.y * vSource.v.y + m_location.orient.m.fVec.v.y * vSource.v.z;
vDest.v.z = m_location.orient.m.rVec.v.z * vSource.v.x + m_location.orient.m.uVec.v.z * vSource.v.y + m_location.orient.m.fVec.v.z * vSource.v.z;
vDest += m_location.pos;
return vDest;
}

// -----------------------------------------------------------------------------

int CGameObject::CheckNormal (IViewMatrix& viewMatrix, CVertex& a, CVertex& b) 
{
CVertex _a, _b;
Transform (_a, a);
Transform (_b, b);
_a += m_location.pos;
_a += viewMatrix.Data (0).m_translate;
_b += _a;
return Dot (viewMatrix.Data (0).m_transformation [0].m.fVec, _a) > Dot (viewMatrix.Data (0).m_transformation [0].m.fVec, _b);
}

// -----------------------------------------------------------------------------

int CGameObject::CheckNormal (IViewMatrix& viewMatrix, CFixVector& a, CFixVector& b) 
{
CVertex _a (a), _b (b);
return CheckNormal (viewMatrix, _a, _b);
}

// -----------------------------------------------------------------------------

int CGameObject::Contains (ubyte nType)
{
if (Type () != OBJ_ROBOT)
	return -1;
return (m_info.contents.count > 0) && (m_info.contents.type == nType) ? (int) m_info.contents.id : -1;
}

// -----------------------------------------------------------------------------

int CGameObject::TypeContains (ubyte nType)
{
if (Type () != OBJ_ROBOT)
	return false;
CRobotInfo *robotInfo = g_data.robotManager->RobotInfo (Id ());
return (robotInfo->Info ().contents.count > 0) && (robotInfo->Info ().contents.type == nType) ? (int) robotInfo->Info ().contents.id : -1;
}

// -----------------------------------------------------------------------------

bool CGameObject::IsBoss (void)
{
return (Type () == OBJ_ROBOT) && g_data.robotManager->RobotInfo (Id ())->Info ().bossFlag > 0;
}

// -----------------------------------------------------------------------------

CSegment* CGameObject::Segment (void) 
{ 
return g_data.segmentManager->Segment (m_info.nSegment); 
}

// -----------------------------------------------------------------------------

CGameItem* CGameObject::Copy (CGameItem* pDest)
{
if (pDest != null)
	*dynamic_cast<CGameObject*> (pDest) = *this;
return pDest;
}

// -----------------------------------------------------------------------------

CGameItem* CGameObject::Clone (void)
{
return Copy (new CGameObject);	// only make a copy if modified
}

// -----------------------------------------------------------------------------

void CGameObject::Backup (eEditType editType)
{
CGameItem::Id () = g_data.undoManager->Backup (this, editType);
}

// -----------------------------------------------------------------------------

void CGameObject::Undo (void)
{
switch (EditType ()) {
	case opAdd:
		g_data.objectManager->Delete (Index (), false);
		break;
	case opDelete:
		g_data.objectManager->Add (false);
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------

void CGameObject::Redo (void)
{
switch (EditType ()) {
	case opDelete:
		g_data.objectManager->Delete (Index ());
		break;
	case opAdd:
		g_data.objectManager->Add (false);
		// fall through
	case opModify:
		*GetParent () = *this;
		break;
	}
}

// -----------------------------------------------------------------------------
// eof object.cpp
#include "stdafx.h"
#include "ModelTextures.h"
#include "PolyModel.h"
#include "ASEModel.h"
#include "OOFModel.h"
#include "rendermodel.h"

//------------------------------------------------------------------------------

static int bCenterGuns [] = {0, 1, 1, 0, 0, 0, 1, 1, 0, 1};

int RenderModel::CSubModel::Filter (int nGunId, int nBombId, int nMissileId, int nMissiles)
{
if (!m_bRender)
	return 0;
if (m_bFlare)
	return 1;
if (m_nGunPoint >= 0)
	return 1;
if (m_bBullets)
	return 1;
if (m_bThruster && ((m_bThruster & (REAR_THRUSTER | FRONTAL_THRUSTER)) != (REAR_THRUSTER | FRONTAL_THRUSTER)))
	return 1;
if (m_bHeadlight)
	return 0;
if (m_bBombMount)
	return (nBombId == 0);

if (m_bWeapon) {
	int bLasers = 1;
	int bSuperLasers = 1;
	int bQuadLasers = 1;
	int bCenterGun = bCenterGuns [nGunId];
	int nWingtip = bQuadLasers ? bSuperLasers : 2; //gameOpts->render.ship.nWingtip;

	if (nWingtip == 0)
		nWingtip = bLasers && bSuperLasers && bQuadLasers;
	else if (nWingtip == 1)
		nWingtip = !bLasers || bSuperLasers;
	
	if (m_nGun == nGunId + 1) {
		if (bLasers) {
			if ((m_nWeaponPos > 2) && !bQuadLasers && (nWingtip != bSuperLasers))
				return 1;
			}
		}
	else if (m_nGun == LASER_INDEX + 1) {
		if (nWingtip)
			return 1;
		return !bCenterGun && (m_nWeaponPos < 3);
		}
	else if (m_nGun == SUPER_LASER_INDEX + 1) {
		if (nWingtip != 1)
			return 1;
		return !bCenterGun && (m_nWeaponPos < 3);
		}
	else if (!m_nGun) {
		if (bLasers && bQuadLasers)
			return 1;
		if (m_nType != nWingtip)
			return 1;
		return 0;
		}
	else 
		return 1;
	}
return 0;
}

//------------------------------------------------------------------------------

static inline int PlayerColor (int nObject)
{
return 1;
}

//------------------------------------------------------------------------------
//eof

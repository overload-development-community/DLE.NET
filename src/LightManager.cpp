
#include "Mine.h"
#include "dle-xp.h"

CLightManager lightManager;

//------------------------------------------------------------------------------

tTextureLight textureLightD1 [NUM_LIGHTS_D1] = {
	{250, 0x00b333L}, {251, 0x008000L}, {252, 0x008000L}, {253, 0x008000L},
	{264, 0x01547aL}, {265, 0x014666L}, {268, 0x014666L}, {278, 0x014cccL},
	{279, 0x014cccL}, {280, 0x011999L}, {281, 0x014666L}, {282, 0x011999L},
	{283, 0x0107aeL}, {284, 0x0107aeL}, {285, 0x011999L}, {286, 0x014666L},
	{287, 0x014666L}, {288, 0x014666L}, {289, 0x014666L}, {292, 0x010cccL},
	{293, 0x010000L}, {294, 0x013333L}, {330, 0x010000L}, {333, 0x010000L}, 
	{341, 0x010000L}, {343, 0x010000L}, {345, 0x010000L}, {347, 0x010000L}, 
	{349, 0x010000L}, {351, 0x010000L}, {352, 0x010000L}, {354, 0x010000L}, 
	{355, 0x010000L}, {356, 0x020000L}, {357, 0x020000L}, {358, 0x020000L}, 
	{359, 0x020000L}, {360, 0x020000L}, {361, 0x020000L}, {362, 0x020000L}, 
	{363, 0x020000L}, {364, 0x020000L}, {365, 0x020000L}, {366, 0x020000L}, 
	{367, 0x020000L}, {368, 0x020000L}, {369, 0x020000L}, {370, 0x020000L}
};

tTextureLight textureLightD2 [NUM_LIGHTS_D2] = {
	{235, 0x012666L}, {236, 0x00b5c2L}, {237, 0x00b5c2L}, {243, 0x00b5c2L},
	{244, 0x00b5c2L}, {275, 0x01547aL}, {276, 0x014666L}, {278, 0x014666L},
	{288, 0x014cccL}, {289, 0x014cccL}, {290, 0x011999L}, {291, 0x014666L},
	{293, 0x011999L}, {295, 0x0107aeL}, {296, 0x011999L}, {298, 0x014666L},
	{300, 0x014666L}, {301, 0x014666L}, {302, 0x014666L}, {305, 0x010cccL},
	{306, 0x010000L}, {307, 0x013333L}, {340, 0x00b333L}, {341, 0x00b333L},
	{343, 0x004cccL}, {344, 0x003333L}, {345, 0x00b333L}, {346, 0x004cccL},
	{348, 0x003333L}, {349, 0x003333L}, {353, 0x011333L}, {356, 0x00028fL},
	{357, 0x00028fL}, {358, 0x00028fL}, {359, 0x00028fL}, {364, 0x010000L},
	{366, 0x010000L}, {368, 0x010000L}, {370, 0x010000L}, {372, 0x010000L},
	{374, 0x010000L}, {375, 0x010000L}, {377, 0x010000L}, {378, 0x010000L},
	{380, 0x010000L}, {382, 0x010000L}, {383, 0x020000L}, {384, 0x020000L},
	{385, 0x020000L}, {386, 0x020000L}, {387, 0x020000L}, {388, 0x020000L},
	{389, 0x020000L}, {390, 0x020000L}, {391, 0x020000L}, {392, 0x020000L},
	{393, 0x020000L}, {394, 0x020000L}, {395, 0x020000L}, {396, 0x020000L},
	{397, 0x020000L}, {398, 0x020000L}, {404, 0x010000L}, {405, 0x010000L},
	{406, 0x010000L}, {407, 0x010000L}, {408, 0x010000L}, {409, 0x020000L},
	{410, 0x008000L}, {411, 0x008000L}, {412, 0x008000L}, {419, 0x020000L},
	{420, 0x020000L}, {423, 0x010000L}, {424, 0x010000L}, {425, 0x020000L},
	{426, 0x020000L}, {427, 0x008000L}, {428, 0x008000L}, {429, 0x008000L},
	{430, 0x020000L}, {431, 0x020000L}, {432, 0x00e000L}, {433, 0x020000L},
	{434, 0x020000L}
};

short blastableLightsD2 [] = {
	276, 278, 360, 361, 364, 366, 368,
	370, 372, 374, 375, 377, 380, 382, 
	420, 432, 431,  -1
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool CLightManager::IsVariableLight (CSideKey key)
{
return VariableLight (key) >= 0;
}

//------------------------------------------------------------------------------

int CLightManager::IsLight (int nBaseTex) 
{
return (m_lightMap [nBaseTex & 0x1fff] > 0) ? 0 : -1;
}

//------------------------------------------------------------------------------

int CLightManager::IsExplodingLight (int nBaseTex) 
{
	switch (nBaseTex) {
	case 291:
	case 292:
	case 293:
	case 294:
	case 296:
	case 297:
	case 298:
	case 299:
		return 1;
	}
return 0;
}

//------------------------------------------------------------------------------

bool CLightManager::IsBlastableLight (int nBaseTex) 
{
nBaseTex &= TEXTURE_MASK;
if (IsExplodingLight (nBaseTex))
	return true;
if (DLE.IsD1File ())
	return false;
for (short* p = blastableLightsD2; *p >= 0; p++)
	if (*p == nBaseTex)
		return true;
return false;
}

//------------------------------------------------------------------------------

CLightDeltaIndex* CLightManager::LightDeltaIndex (CSideKey key)
{
for (int nIndex = 0; nIndex < DeltaIndexCount (); nIndex++) {
	CLightDeltaIndex* pIndex = LightDeltaIndex (nIndex);
	if (*pIndex == key)
		return pIndex;
	}
return null;
}

//------------------------------------------------------------------------------

short CLightManager::VariableLight (CSideKey key)
{
current->Get (key);
CVariableLight* flP = VariableLight (0);
int i;
for (i = Count (); i; i--, flP++)
	if (*flP == key)
		break;
if (i > 0)
	return Count () - i;
return -1;
}

//------------------------------------------------------------------------------

CVariableLight* CLightManager::AddVariableLight (short index) 
{
if (Count () >= MAX_VARIABLE_LIGHTS) {
	if (!DLE.ExpertMode () && (index < 0)) {
		sprintf_s (message, sizeof (message),
					  "Maximum number of variable lights (%d) have already been added",
					  MAX_VARIABLE_LIGHTS);
		ErrorMsg (message);
		}
	return null;
	}

short nBaseTex, nOvlTex;
current->Side ()->GetTextures (nBaseTex, nOvlTex);
if ((IsLight (nBaseTex) == -1) && (IsLight (nOvlTex) == -1)) {
	if (!DLE.ExpertMode () && (index < 0))
		ErrorMsg ("Blinking lights can only be added to a side\n"
					 "that has a light emitting texture.\n"
					 "Hint: You can use the texture tool's brightness control\n"
					 "to make any texture emit light.");
	return null;
	}

if (index < 0)
	index = Count ();
Count ()++;
return VariableLight (index);
}

//------------------------------------------------------------------------------

short CLightManager::AddVariableLight (CSideKey key, uint mask, int time) 
{
current->Get (key);
if (VariableLight (key) != -1) {
	if (!DLE.ExpertMode ())
		ErrorMsg ("There is already a variable light on this side");
	return -1;
	}
// we are adding a new variable light
undoManager.Begin (__FUNCTION__, udVariableLights);
CVariableLight* pLight = AddVariableLight ();
if (pLight == null) {
	undoManager.End (__FUNCTION__);
	return -1;
	}
pLight->Setup (key, time, mask);
pLight->Backup (opAdd);
undoManager.End (__FUNCTION__);
if (CLightDeltaIndex* pIndex = LightDeltaIndex (key))
	pIndex->RecalculateVariableLightIndex ();
return Count ();
}

//------------------------------------------------------------------------------

void CLightManager::DeleteVariableLight (short index, bool bUndo) 
{
if (index > -1) {
	if (!bUndo) {
		VariableLight (index)->Index () = index;
		VariableLight (index)->Backup (opDelete);
		}
	undoManager.Begin (__FUNCTION__, udVariableLights);
	if (index < --Count ()) {
		if (!bUndo) {
			VariableLight (index)->Index () = Count ();
			VariableLight (index)->Backup (opMove);
			}
		memcpy (VariableLight (index), VariableLight (m_nCount), sizeof (CVariableLight));
		}
	undoManager.End (__FUNCTION__);
	}
}

//------------------------------------------------------------------------------

bool CLightManager::DeleteVariableLight (CSideKey key) 
{
current->Get (key);
short index = VariableLight (key);
if (index == -1)
	return false;
DeleteVariableLight (index);
if (CLightDeltaIndex* pIndex = LightDeltaIndex (key))
	pIndex->RecalculateVariableLightIndex ();
return true;
}

//------------------------------------------------------------------------------

CColor* CLightManager::LightColor (CSideKey key, bool bUseTexColors) 
{ 
current->Get (key);
if (bUseTexColors && ApplyFaceLightSettingsGlobally ()) {
	short nBaseTex, nOvlTex;
	segmentManager.Textures (key, nBaseTex, nOvlTex);
	CColor *pColor;
	if (nOvlTex > 0) {
		pColor = GetTexColor (nOvlTex, false);
		if (pColor != null)
			return pColor;
		}
	CWall* pWall = segmentManager.Wall (key);
	pColor = GetTexColor (nBaseTex, (pWall != null) && pWall->IsTransparent ());
	if (pColor != null)
		return pColor;
	}	
return FaceColor (key.m_nSegment, key.m_nSide); 
}

//------------------------------------------------------------------------------

int CLightManager::FindLight (int nTexture, tTextureLight* texLightP, int nLights)
{
	int	l = 0;
	int	r = nLights - 1;
	int	m, t;

while (l <= r) {
	m = (l + r) / 2;
	t = texLightP [m].nBaseTex;
	if (nTexture > t)
		l = m + 1;
	else if (nTexture < t)
		r = m - 1;
	else
		return m;
	}
return -1;
}

//-------------------------------------------------------------------------

int CLightManager::LightIsOn (CLightDeltaIndex light)
{
short nLight = light.VariableLightIndex ();
if (nLight < 0)
	return -1;
CVariableLight* pLight = lightManager.VariableLight (nLight);
tLightTimer* pTimer = lightManager.LightTimer (nLight);
return (pLight->m_info.mask & (1 << pTimer->impulse)) != 0;
}

//-------------------------------------------------------------------------

void CLightManager::CreateLightMap (void)
{
LoadDefaults ();
}

// ------------------------------------------------------------------------

bool CLightManager::HasCustomLightMap (void)
{
CResource res;
ubyte *pData;
if (!(pData = res.Load (DLE.IsD1File () ? IDR_LIGHT_D1 : IDR_LIGHT_D2)))
	return false;
return memcmp (m_lightMap, pData, sizeof (m_lightMap)) != 0;
}

// ------------------------------------------------------------------------

ubyte* CLightManager::ColorToFloat (CColor* pColor, ubyte* pData)
{
pColor->m_info.index = *pData++;
pColor->m_info.color.r = (double) *((int *) pData) / (double) 0x7fffffff;
pData += sizeof (int);
pColor->m_info.color.g = (double) *((int *) pData) / (double) 0x7fffffff;
pData += sizeof (int);
pColor->m_info.color.b = (double) *((int *) pData) / (double) 0x7fffffff;
pData += sizeof (int);
return pData;
}

// ------------------------------------------------------------------------

bool CLightManager::HasCustomLightColors (void)
{
CResource res;
ubyte *pData;
if (!(pData = res.Load (DLE.IsD1File () ? IDR_COLOR_D1 : IDR_COLOR_D2)))
	return false;
for (uint i = 0, l = m_texColors.Length (); i < l; i++) {
	CColor color;
	pData = ColorToFloat (&color, pData);
	if (m_texColors [i] != color)
		return true;
	}
return false;
}

// ------------------------------------------------------------------------

short CLightManager::LoadDefaults (void)
{
CResource res;
ubyte *pData;
if (!(pData = res.Load (DLE.IsD1File () ? IDR_COLOR_D1 : IDR_COLOR_D2)))
	return false;
int i = int (res.Size () / (3 * sizeof (int) + sizeof (ubyte)));
#if _DEBUG
if (i > (int) m_texColors.Length ())
	i = (int) m_texColors.Length ();
#else
if (i > sizeofa (m_texColors))
	i = sizeofa (m_texColors);
#endif
for (CColor* pColor = &m_texColors [0]; i; i--, pColor++) 
	pData = ColorToFloat (pColor, pData);

if (!(pData = res.Load (DLE.IsD1File () ? IDR_LIGHT_D1 : IDR_LIGHT_D2)))
	return false;
memcpy (m_lightMap, pData, min (res.Size (), sizeof (m_lightMap)));
return 1;
}

// ------------------------------------------------------------------------

	void CLightManager::SortDeltaIndex (int left, int right)
	{
		int	l = left,
				r = right,
				m = (left + right) / 2;
		short	mSeg = LightDeltaIndex (m)->m_nSegment, 
				mSide = LightDeltaIndex (m)->m_nSide;
		CSideKey mKey = CSideKey (mSeg, mSide);
		CLightDeltaIndex	*pl, *pr;

	do {
		pl = LightDeltaIndex (l);
		//while ((pl->m_nSegment < mSeg) || ((pl->m_nSegment == mSeg) && (pl->nSide < mSide))) {
		while (*pl < mKey) {
			pl++;
			l++;
			}
		pr = LightDeltaIndex (r);
		//while ((pr->m_info.nSegment > mSeg) || ((pr->m_info.nSegment == mSeg) && (pr->nSide > mSide))) {
		while (*pr > mKey) {
			pr--;
			r--;
			}
		if (l <= r) {
			if (l < r)
				Swap (*pl, *pr);
			l++;
			r--;
			}
		} while (l <= r);
	if (right > l)
		SortDeltaIndex (l, right);
	if (r > left)
		SortDeltaIndex (left, r);
	}

// ------------------------------------------------------------------------

void CLightManager::UnsortDeltaIndex (int left, int right)
{
	int	l = left,
			r = right,
			m = (left + right) / 2;
	int	mKey = LightDeltaIndex (m)->Index ();
	CLightDeltaIndex	*pl, *pr;

do {
	pl = LightDeltaIndex (l);
	//while ((pl->m_nSegment < mSeg) || ((pl->m_nSegment == mSeg) && (pl->nSide < mSide))) {
	while (pl->Index () < mKey) {
		pl++;
		l++;
		}
	pr = LightDeltaIndex (r);
	//while ((pr->m_info.nSegment > mSeg) || ((pr->m_info.nSegment == mSeg) && (pr->nSide > mSide))) {
	while (pr->Index () > mKey) {
		pr--;
		r--;
		}
	if (l <= r) {
		if (l < r)
			Swap (*pl, *pr);
		l++;
		r--;
		}
	} while (l <= r);
if (right > l)
   UnsortDeltaIndex (l, right);
if (r > left)
   UnsortDeltaIndex (left, r);
}

// -----------------------------------------------------------------------------

void CLightManager::SortDeltaIndex (void) 
{ 
for (int i = 0; i < DeltaIndexCount (); i++)
	LightDeltaIndex (i)->Index () = i;
SortDeltaIndex (0, DeltaIndexCount () - 1); 
}

// -----------------------------------------------------------------------------

void CLightManager::UnsortDeltaIndex (void) 
{ 
UnsortDeltaIndex (0, DeltaIndexCount () - 1); 
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

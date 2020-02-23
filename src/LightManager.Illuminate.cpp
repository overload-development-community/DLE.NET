
#include "mine.h"
#include "dle-xp.h"

// external globals
//extern int DLE.MineView ()->RenderVariableLights (); // uvls.cpp
extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

#define CHECK_POINT_VISIBILITY 1

#define LINEAR_ATTENUATION 0.05
#define QUADRATIC_ATTENUATION 0.005

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
// Tag each segment that is within a given range (in segments to traverse) to a root segment

class CVicinity {
	private:
		short* m_depth;
		short m_null;

	public:
		CVicinity () {
			m_null = -1;
			m_depth = new short [segmentManager.Count ()];
			if (m_depth != null)
				memset (m_depth, 0, segmentManager.Count () * sizeof (m_depth [0]));
			}

		~CVicinity () {
			if (m_depth != null) {
				delete[] m_depth;
				m_depth = null;
				}
			}

		inline short Depth (short i) { return (m_depth == null) ? -1 : m_depth [i]; }

		const short& operator[] (int i) { return m_depth ? m_depth [i] : m_null; } 

		void Compute (short nSegment, short nDepth) {
			if (nSegment < 0)
				return;
			if (m_depth == null)
				return;
			if (nDepth <= m_depth [nSegment])
				return;

			m_depth [nSegment] = nDepth;

			if (nDepth > 1) {
				// check each side of this segment for more children
				CSegment* pSegment = segmentManager.Segment (nSegment);
				for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
					// skip if there is a wall and its a door
					CWall* pWall = segmentManager.Wall (CSideKey (nSegment, nSide));
					if ((pWall == null) || (pWall->Type () != WALL_DOOR))
						Compute (pSegment->ChildId (nSide), nDepth - 1);
					}
				}
			}
};

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

uint CLightManager::LightWeight (short nTexture) 
{
return uint (double (m_lightMap [nTexture]) * m_fLightScale / (Quality () ? 256.0 : 200.0)); //(TEXLIGHT_SCALE * 200.0));
}

//---------------------------------------------------------------------------------

void CLightManager::ScaleCornerLight (bool bAll) 
{
	double fLight = m_fBrightness / 100.0;

undoManager.Begin (__FUNCTION__, udSegments);
int nSegments = segmentManager.Count ();
#pragma omp parallel for if (nSegments > 15)
for (int i = 0; i < nSegments; i++) {
	CSegment* pSegment = segmentManager.Segment (i);
	CSide* pSide = pSegment->m_sides;
	for (int j = 0; j < 6; j++, pSide++) {
		if (bAll || pSide->IsTagged ()) {
			CUVL* pTexCoord = pSide->m_info.uvls;
			for (int i = 0; i < 4; i++) {
				double l = ((double) pTexCoord [i].l) * fLight;
				pTexCoord [i].l = (uint) l;
				}
			}
		}
	}
undoManager.End (__FUNCTION__);
}

//---------------------------------------------------------------------------------
// When lighting a mine, DLE will take each light in the mine, walk from it up
// <render depth> segments away, lighting all segments it reaches. For each of these
// segments, it will compute the brightness and color of each side's four corners,
// generating 24 light values. Segments adjacent to those reached by a given light
// will not receive any contribution from that light. This will lead to sharp light
// borders, since each segment's lights stores its own light values, this disregarding
// that they may share the same vertex. CalcAverageCornerLight fixes this by determining
// the highest brightness any side corner with to a given vertex has and assigning
// that value to all side corners related to that vertex.

#define USE_AVG_LIGHT 1

static inline double sqr (uint v) { return double (v) * double (v); }

typedef struct tAvgCornerLight {
	double	light;
	ubyte		count;
} tAvgCornerLight;

void CLightManager::CalcAverageCornerLight (bool bAll)
{
  tAvgCornerLight* maxBrightness = new tAvgCornerLight [vertexManager.Count ()];
  int nSegments = segmentManager.Count ();

memset (maxBrightness, 0, vertexManager.Count () * sizeof (tAvgCornerLight));

undoManager.Begin (__FUNCTION__, udSegments);
// smooth corner light by averaging all corners which share a vertex
//#pragma omp parallel for
for (int nSegment = 0; nSegment < nSegments; nSegment++) {
	CSegment *pSegment = segmentManager.Segment (nSegment);
#if DBG
	if (pSegment - segmentManager.Segments ().Buffer () == nDbgSeg)
		nDbgSeg = nDbgSeg;
#endif
	for (ubyte nPoint = 0; nPoint < 8; nPoint++) {
		int nVertex = pSegment->m_info.vertexIds [nPoint];
		if (nVertex > MAX_VERTEX)
			continue;
		if (bAll || (vertexManager.Status (nVertex) & TAGGED_MASK)) {
			CSide* pSide = pSegment->Side (0);
			for (int nSide = 0; nSide < 6; nSide++, pSide++) {
				int nCorner = pSide->FindVertexIdIndex (nPoint);
				if (nCorner < 0) 
					continue;
				if (!pSide->IsVisible ())
					continue;
#if DBG
				if (nVertex == nDbgVertex)
					nDbgVertex = nDbgVertex;
#endif
#if USE_AVG_LIGHT == 2
				maxBrightness [nVertex].light += sqr (pSide->m_info.uvls [nCorner].l);
#elif USE_AVG_LIGHT == 1
				maxBrightness [nVertex].light += pSide->m_info.uvls [nCorner].l;
#else
				if (maxBrightness [nVertex].light < pSide->m_info.uvls [nCorner].l)
					maxBrightness [nVertex].light = pSide->m_info.uvls [nCorner].l;
#endif
				maxBrightness [nVertex].count++;
				}
			}
		}
	}

#if USE_AVG_LIGHT
for (int i = 0, j = vertexManager.Count (); i < j; i++)
	if (maxBrightness [i].count) 
#	if USE_AVG_LIGHT == 2
		maxBrightness [i].light = sqrt (maxBrightness [i].light / maxBrightness [i].count);
#	else
		maxBrightness [i].light /= maxBrightness [i].count;
#	endif
#endif

//#pragma omp parallel for
for (int i = 0; i < nSegments; i++) {
	CSegment *pSegment = segmentManager.Segment (i);
	bool bUndo = false;
	bool bSelfIlluminate = DLE.IsD2XLevel () && (pSegment->Props () & SEGMENT_PROP_SELF_ILLUMINATE);
	for (int nPoint = 0; nPoint < 8; nPoint++) {
		int nVertex = pSegment->m_info.vertexIds [nPoint];
		if ((maxBrightness [nVertex].count > 0) && (bAll || (vertexManager.Status (nVertex) & TAGGED_MASK))) {
			CSide* pSide = pSegment->Side (0);
			for (int nSide = 0; nSide < 6; nSide++, pSide++) {
				if (bSelfIlluminate && (Brightness (pSegment, pSide) > 0))
					continue;
				int nCorner = pSide->FindVertexIdIndex (nPoint);
				if (nCorner < 0) 
					continue;
				if (!pSide->IsVisible ())
					continue;
				if (!bUndo) {
					pSegment->Backup ();
					bUndo = true;
					}
				pSide->m_info.uvls [nCorner].l = uint (maxBrightness [nVertex].light);
				}
			}
		}
	}

undoManager.End (__FUNCTION__);
delete[] maxBrightness;
}

//---------------------------------------------------------------------------------

uint CLightManager::Brightness (CSegment* pSegment, CSide* pSide)
{
uint brightness = (pSegment->m_info.function == SEGMENT_FUNC_SKYBOX) ? 65535 : 0;
int nTexture = pSide->BaseTex ();
if ((nTexture >= 0) && (nTexture < MAX_TEXTURES))
	brightness = max (brightness, LightWeight (nTexture));
nTexture = pSide->OvlTex (0);
if ((nTexture > 0) && (nTexture < MAX_TEXTURES))
	brightness = max (brightness, LightWeight (nTexture));
return brightness;
}

//---------------------------------------------------------------------------------

uint CLightManager::Brightness (CSideKey& key)
{
CSegment* pSegment = segmentManager.Segment (key.m_nSegment);
return Brightness (pSegment, pSegment->Side (key.m_nSide));
}

//---------------------------------------------------------------------------------

void CLightManager::ComputeStaticLight (bool bAll, bool bCopyTexLights) 
{
// clear all lighting on marked segments
segmentManager.ComputeNormals (true);
undoManager.Begin (__FUNCTION__, udSegments | udStaticLight);
if (bAll)
	VertexColors ().Clear ();
int nSegments = segmentManager.Count ();
for (int i = 0; i < nSegments; i++) {
	CSegment *pSegment = segmentManager.Segment (i);
	pSegment->Backup ();
	CSide* pSide = pSegment->m_sides;
	for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++, pSide++) {
		if (bAll || pSide->IsTagged ()) {
			for (int i = 0; i < 4; i++) {
				pSide->m_info.uvls [i].l = 0;
				if (!bAll)
					VertexColor (pSegment->m_info.vertexIds [pSegment->Side (nSide)->VertexIdIndex (i)])->Clear ();
				}
			}
		}
	}

// Calculate segment side corner light values
// for each marked side in the level
// (range: 0 = min, 0x8000 = max)
DLE.MainFrame ()->InitProgress (nSegments);
short nSegment = 0;
for (; nSegment < nSegments; nSegment++) {
#if 0
	char szMsg [200];
	sprintf (szMsg, "Segment %d", nSegment + 1);
	STATUSMSG (szMsg);
#endif
	DLE.MainFrame ()->Progress ().StepIt ();
	CSegment *pSegment = segmentManager.Segment (nSegment);
#if DBG
	if (nSegment == nDbgSeg)
		nDbgSeg = nDbgSeg;
#endif
	CSide* pSide = pSegment->m_sides;
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
#if DBG
		if ((nSegment == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
			nDbgSeg = nDbgSeg;
#endif
		if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
			continue;
		if (!(bAll || pSide->IsTagged ()))
			continue;
		if (!segmentManager.IsWall (CSideKey (nSegment, nSide)))
			continue;
		if (bCopyTexLights)
			FaceColor (nSegment, nSide)->Clear ();
		uint brightness = Brightness (pSegment, pSide);
		if (brightness > 0)
			GatherLight (nSegment, nSide, brightness, bAll, bCopyTexLights);
		}
	}
DLE.MainFrame ()->Progress ().DestroyWindow ();
undoManager.End (__FUNCTION__);

}

//---------------------------------------------------------------------------------
// set_illumination()
//---------------------------------------------------------------------------------
// blend two colors depending on each colors brightness
// psc: source color (light source)
// pdc: destination color (vertex/side color)
// srcBr: source brightness (remaining brightness at current vertex/side)
// destBr: vertex/side brightness

void CLightManager::BlendColors (CColor* pSrcColor, CColor* pDestColor, double srcBrightness, double destBrightness)
{
if (destBrightness)
	destBrightness /= 65536.0;
else {
	if (srcBrightness)
		*pDestColor = *pSrcColor;
	return;
	}
if (srcBrightness)
	srcBrightness /= 65536.0;
else
	return;

if (pDestColor->m_info.index) {
	pDestColor->m_info.color.r += pSrcColor->m_info.color.r * srcBrightness;
	pDestColor->m_info.color.g += pSrcColor->m_info.color.g * srcBrightness;
	pDestColor->m_info.color.b += pSrcColor->m_info.color.b * srcBrightness;
	double cMax = pDestColor->m_info.color.r;
		if (cMax < pDestColor->m_info.color.g)
			cMax = pDestColor->m_info.color.g;
		if (cMax < pDestColor->m_info.color. b)
			cMax = pDestColor->m_info.color. b;
		if (cMax > 1) {
			pDestColor->m_info.color.r /= cMax;
			pDestColor->m_info.color.g /= cMax;
			pDestColor->m_info.color.b /= cMax;
			}
	}
else {
	if (destBrightness) {
		pDestColor->m_info.index = 1;
		pDestColor->m_info.color.r = pSrcColor->m_info.color.r * destBrightness;
		pDestColor->m_info.color.g = pSrcColor->m_info.color.g * destBrightness;
		pDestColor->m_info.color.b = pSrcColor->m_info.color.b * destBrightness;
		}
	else
		*pDestColor = *pSrcColor;
	}
}

//---------------------------------------------------------------------------------

void CLightManager::GatherFaceLight (double cornerLights [], CSegment* pSegment, short nSide, uint brightness, CColor* pLightColor)
{
	CSide*	pSide = pSegment->Side (nSide);
	CUVL*		pTexCoord = pSide->m_info.uvls;

#if DBG
if ((pSegment - segmentManager.Segment (0) == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif
for (int i = 0, j = pSide->VertexCount (); i < j; i++, pTexCoord++) {
	CColor* pVertexColor = VertexColor (pSegment->VertexId (nSide, i));
	if (pSegment->m_info.function == SEGMENT_FUNC_SKYBOX) {
		pTexCoord->l = (ushort) 0x7FFF;
		pVertexColor->m_info.color.r =
		pVertexColor->m_info.color.g =
		pVertexColor->m_info.color.b = 255;
		}
	else {
		uint lightBrightness = (uint) (brightness * cornerLights [i]);
#pragma omp critical
		{
		BlendColors (pLightColor, pVertexColor, lightBrightness, pTexCoord->l);
		pTexCoord->l += lightBrightness;
		}
		}
	}
#if DBG
if ((pSegment - segmentManager.Segment (0) == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif
}

//---------------------------------------------------------------------------------
// Walk through all segments up to a given max. distance from the source segment
// and add the source side's light to all child sides.

void CLightManager::GatherLight (short nSourceSeg, short nSourceSide, uint brightness, bool bAll, bool bCopyTexLights) 
{
	CSegment* pSrcSeg = segmentManager.Segment (nSourceSeg);
	// find orthogonal angle of source segment
	CVertex sourceNormal = Average (pSrcSeg->Side (nSourceSide)->m_vNormal [0], pSrcSeg->Side (nSourceSide)->m_vNormal [1]);
	// remember to flip the sign since we want it to point inward
	// calculate the center of the source segment
	CVertex sourceCenter = segmentManager.CalcSideCenter (CSideKey (nSourceSeg, nSourceSide));
	// mark those segmentManager.Segment () within N children of current segment

// set child numbers
//segmentManager.Segment ()[nSourceSeg].nIndex = m_staticRenderDepth;

CVicinity vicinity;
vicinity.Compute (nSourceSeg, m_staticRenderDepth);	//mark all children that are at most lightRenderDepth segments away

CColor *pLightColor = LightColor (CSideKey (nSourceSeg, nSourceSide));
if (!pLightColor->m_info.index) {
	pLightColor->m_info.index = 255;
	pLightColor->m_info.color.r =
	pLightColor->m_info.color.g =
	pLightColor->m_info.color.b = 1.0;
	}
if (ApplyFaceLightSettingsGlobally () && bCopyTexLights) {
	CColor* pColor = LightColor (CSideKey (nSourceSeg, nSourceSide), false);
	*pLightColor = *pColor;
	}
bool bWall = false; 

#ifdef _DEBUG
if ((nSourceSeg == nDbgSeg) && ((nDbgSide < 0) || (nSourceSide == nDbgSide)))
	nSourceSeg = nSourceSeg;
#endif

CVertex sourceCorners [5];
short nVertices = pSrcSeg->Side (nSourceSide)->VertexCount ();
sourceCorners [nVertices].Clear ();
for (short i = 0; i < nVertices; i++) {
	sourceCorners [i] = *pSrcSeg->Vertex (nSourceSide, i);
	sourceCorners [nVertices] += sourceCorners [i];
	}
sourceCorners [nVertices] /= double (nVertices);

int nSegments = segmentManager.Count ();
#pragma omp parallel for if (nSegments > 15)
for (int nChildSeg = 0; nChildSeg < nSegments; nChildSeg++) {
#if DBG
	if (nChildSeg == nDbgSeg)
		nDbgSeg = nDbgSeg;
#endif
	// skip if this is too far away
	if (vicinity [nChildSeg] <= 0)
		continue;
	CSegment *pChildSeg = segmentManager.Segment (nChildSeg);
	// setup source corner vertex for length calculation later
	// loop on child sides
	for (int nChildSide = 0; nChildSide < 6; nChildSide++) {
		// if side has a child..
		if (!(bAll || pChildSeg->Side (nChildSide)->IsTagged ()))
			continue;
#if 0 // unfortunately this breaks object lighting because it needs values even where there is no wall
		if (pChildSeg->ChildId (nChildSide) >= 0) {
			CWall* pWall = pChildSeg->Side (nChildSide)->Wall ();
			if ((pWall == null) || !pWall->IsVisible ())
				continue;
			}
#endif

		double cornerLights [4];

		// if the child side is the same as the source side, then set light and continue
		if ((nChildSide == nSourceSide) && (nChildSeg == nSourceSeg))
			cornerLights [0] = cornerLights [1] = cornerLights [2] = cornerLights [3] = 1.0;
		// calculate vector between center of source segment and center of child
		else if (DLE.IsD2XLevel () && (pSrcSeg->Props () & SEGMENT_PROP_SELF_ILLUMINATE))
			continue;
		else if (!CalcCornerLights (cornerLights, nChildSeg, nChildSide, nSourceSeg, nSourceSide, sourceCorners, sourceNormal, bWall)) 
			continue;
		GatherFaceLight (cornerLights, pChildSeg, nChildSide, brightness, pLightColor);
		}
	}
}

//------------------------------------------------------------------------
// ComputeVariableLight()
//
// Action - Calculates lightDeltaIndices and CLightDeltaValues arrays.
//
//	Delta lights are only created if the segment has a nOvlTex
//	which matches one of the entries in the broken[] table.
//
//	For a given side, we calculate the distance and angle to all
//	nearby sides.  A nearby side is a side which is on a segment
//	which is a child or sub-child of the current segment.  Light does
//	not shine through wallManager.Wall ().
//
//	We use the following equation to calculate
//	how much the light effects a side:
//              if (angle < 120 degrees)
//		  cornerLights = k / distance
//
//	where,
//
// 		angle = acos (A dot B / |A|*|B|)
//		A = vector orthogonal to light source side
//		B = vector from center of light source side to effected side.
//		distance is from center of light souce to effected side's corner
//		k = 0x20 (maximum value for CLightDeltaValue)
//
//	Delta light values are stored for each corner of the effected side.
//	(0x00 = no cornerLights, 0x20 is maximum cornerLights)
//
// A little int to take care of too many delta light values leading
// to some variable lights not being visible in Descent:
// For each recursion step, the delta light calculation will be executed
// for all children of sides with variable lights with exactly that
// distance (recursionDepth) to their parent. In case of too many delta 
// light values for the full lighting depth at least the lighting of the
// segmentManager.Segment () close to variable lights will be computed properly.
//------------------------------------------------------------------------

void CLightManager::ComputeVariableLight (int force) 
{
segmentManager.ComputeNormals (true);
undoManager.Begin (__FUNCTION__, udDynamicLight);
for (int nDepth = m_deltaRenderDepth; nDepth; nDepth--)
	if (ComputeLightDeltas (force, nDepth))
		break;
undoManager.End (__FUNCTION__);
}

//---------------------------------------------------------------------------------

int CLightManager::FindLightDelta (short nSegment, short nSide, short *pi)
{
	int	i = pi ? *pi : 0;
	int	j = DeltaIndexCount ()++;
	CLightDeltaIndex	*pIndex = LightDeltaIndex (0);

if ((DLE.LevelVersion () >= 15) && (theMine->Info ().fileInfo.version >= 34)) {
	for (; i < j; i++, pIndex++)
		if ((pIndex->m_nSegment == nSegment) && (pIndex->m_nSide = (ubyte) nSide))
			return i;
	}
else {
	for (; i < j; i++, pIndex++)
		if ((pIndex->m_nSegment == nSegment) && (pIndex->m_nSide = (ubyte) nSide))
			return i;
	}
return -1;
}

//---------------------------------------------------------------------------------

bool CLightManager::FaceHasVariableLight (CSideKey key, CSide* pSide, CWall* pWall) 
{
short nBaseTex, nOvlTex;
pSide->GetTextures (nBaseTex, nOvlTex);
if ((IsLight (nBaseTex) == -1) && ((nOvlTex == 0) || (IsLight (nOvlTex) == -1)))
	return false;
if (pWall != null) {
	if (!pWall->IsVisible ())
		return false;
	if (pWall->IsVariable ())
		return true;
	}
if (IsVariableLight (key))
	return true;
if (IsBlastableLight (nBaseTex) || ((nOvlTex > 0) && IsBlastableLight (nOvlTex)))
	return true;
CTrigger* pTrigger = triggerManager.FindByTarget (key);
if ((pTrigger != null) && (pTrigger->Type () >= TT_LIGHT_OFF))
	return true;
return false;
}

//---------------------------------------------------------------------------------
// Delta lights are stored as follows: Each variable light has an entry in the 
// delta light index. That entry points to the list of segment sides the variable
// light affects (light delta values), and the light difference (delta) for each 
// side when the light is off.

bool CLightManager::ComputeLightDeltas (int bForce, int nDepth) 
{
	// initialize totals
	bool	bD2XLights = (DLE.LevelVersion () >= 15) && (theMine->Info ().fileInfo.version >= 34);
	int	nErrors = 0;
	int	nMaxIndices = MAX_LIGHT_DELTA_INDICES;
	int	nMaxValues = (bD2XLights ? 8191 : 255);
	int	nMaxDeltas = MAX_LIGHT_DELTA_VALUES;

DeltaValueCount () = 0;
DeltaIndexCount () = 0;
m_deltaIndex.Clear ();
m_deltaValues.Clear ();

int nSegments = segmentManager.Count ();
DLE.MainFrame ()->InitProgress (nSegments);
for (short nSourceSeg = 0; !nErrors && (nSourceSeg < nSegments); nSourceSeg++) {
	DLE.MainFrame ()->Progress ().StepIt ();
	CSegment* pSrcSeg = segmentManager.Segment (nSourceSeg);
	// skip if not marked unless we are automatically saving
	// loop on all sides
#if DBG
	if (nSourceSeg == nDbgSeg)
		nDbgSeg = nDbgSeg;
#endif
	CSide* pSrcSide = pSrcSeg->m_sides;
	for (int nSourceSide = 0; !nErrors && (nSourceSide < 6); nSourceSide++, pSrcSide++) {
		if  (!(bForce || pSrcSide->IsTagged ())) 
			continue;
		CSideKey key (nSourceSeg, nSourceSide);
		CWall* pWall = segmentManager.Wall (key);
		if (!FaceHasVariableLight (key, pSrcSide, pWall))
			continue;

		if (DeltaIndexCount () >= nMaxIndices) {
			char szMsg [256];
			sprintf_s (szMsg, sizeof (szMsg), " Light tool: Too many dynamic lights in the mine", nDepth);
			DEBUGMSG (szMsg);
			break;
			}
			
		int indexCount = int (DeltaIndexCount ()++);
		CVertex sourceNormal, sourceCenter;

// get index number and increment total number of lightDeltaIndices
		CLightDeltaIndex *pIndex = LightDeltaIndex (indexCount);
		pIndex->m_nSegment = nSourceSeg;
		pIndex->m_nSide = nSourceSide;
		pIndex->m_info.count = 0; // will be incremented below
		pIndex->m_info.index = (ushort) DeltaValueCount ();
		pIndex->RecalculateVariableLightIndex ();

		// find orthogonal angle of source segment
		sourceNormal = Average (pSrcSide->m_vNormal [0], pSrcSide->m_vNormal [1]);

		// calculate the center of the source segment
		sourceCenter = segmentManager.CalcSideCenter (CSideKey (nSourceSeg, nSourceSide));

		double fBrightness = double (Brightness (pSrcSeg, pSrcSide)) * m_fDeltaLightScale / 100.0;

		// mark those segmentManager.Segment () within N children of current segment
		//(note: this is done once per light instead of once per segment
		//       even though some segmentManager.Segment () have multiple lights.
		//       This actually reduces the number of calls since most
		//       segmentManager.Segment () do not have lights)
		CVicinity vicinity;
		vicinity.Compute (nSourceSeg, nDepth);

		// setup source corner vertex for length calculation later
		CVertex sourceCorners [5];
		int nVertices = segmentManager.Segment (nSourceSeg)->Side (nSourceSide)->VertexCount ();
		sourceCorners [nVertices].Clear ();
		for (int j = 0; j < nVertices; j++) {
			ubyte nVertex = segmentManager.Segment (nSourceSeg)->Side (nSourceSide)->VertexIdIndex (j);
			int h = pSrcSeg->m_info.vertexIds [nVertex];
			sourceCorners [j] = *vertexManager.Vertex (h);
			sourceCorners [nVertices] += sourceCorners [j];
			}
		sourceCorners [nVertices] /= double (nVertices);

		// loop on child segmentManager.Segment ()
//#pragma omp parallel for
		for (int nChildSeg = 0; (nChildSeg < nSegments); nChildSeg++) {
//#pragma omp flush (nErrors)
			if (nErrors)
				continue;
#if DBG
			if (nChildSeg == nDbgSeg)
				nDbgSeg = nDbgSeg;
#endif
			if (vicinity [nChildSeg] == 0)
				continue;
			CSegment *pChildSeg = segmentManager.Segment (nChildSeg);
			CSide* pChildSide = pChildSeg->Side (0);
			// loop on child sides
			for (int nChildSide = 0; !nErrors && (nChildSide < 6); nChildSide++, pChildSide++) {
				if (pChildSeg->ChildId (nChildSide) >= 0) {
					CWall* pWall = pChildSide->Wall ();
					if (pWall == null)
						continue;
					if (pWall->Type () == WALL_OPEN) 
						continue; // don't put light because there is no texture here
					}
				// don't affect non-variable light emitting textures (e.g. lava)
				if (m_nNoLightDeltas == 1) {
					if (((IsLight (pChildSide->BaseTex ()) >= 0) || (IsLight (pChildSide->OvlTex (0)) >= 0)) && !IsVariableLight (CSideKey (nChildSeg, nChildSide)))
						continue;
					}
				else if (m_nNoLightDeltas == 2) {
					if ((textureManager.IsLava (pChildSide->BaseTex ()) || textureManager.IsLava (pChildSide->OvlTex (0))))
						continue;
					}

				double cornerLights [4];
				// if the child side is the same as the source side, then set light and continue
				if ((nChildSide == nSourceSide) && (nChildSeg == nSourceSeg)) 
					cornerLights [0] =
					cornerLights [1] =
					cornerLights [2] =
					cornerLights [3] = 1.0;
				else if (DLE.IsD2XLevel () && (pSrcSeg->Props () & SEGMENT_PROP_SELF_ILLUMINATE))
					continue;
				else if (!CalcCornerLights (cornerLights, nChildSeg, nChildSide, nSourceSeg, nSourceSide, sourceCorners, sourceNormal, pWall != null))
					continue;
				CLightDeltaValue* pLightDelta;
//#pragma omp critical
				{
				if ((DeltaValueCount () >= nMaxDeltas) || (pIndex->m_info.count >= nMaxValues)) {
					if (++nErrors == 1) {
						char szMsg [256];
						sprintf_s (szMsg, sizeof (szMsg), " Light tool: Too many vertices affected by dynamic lights at render depth %d", nDepth);
						DEBUGMSG (szMsg);
						}
					}
				else {
					pLightDelta = LightDeltaValue (DeltaValueCount ()++);
					pIndex->m_info.count++;
					}
				}
//#pragma omp flush (nErrors)
				if (nErrors)
					continue;

				pLightDelta->m_nSegment = nChildSeg;
				pLightDelta->m_nSide = nChildSide;
#if DBG
				if ((nChildSeg == nDbgSeg) && ((nDbgSide < 0) || (nChildSide == nDbgSide)))
						nDbgSeg = nDbgSeg;
#endif
				for (int nCorner = 0; nCorner < 4; nCorner++)
					pLightDelta->m_info.vertLight [nCorner] = uint (cornerLights [nCorner] * fBrightness);
				}
			}
		}
	}
DLE.MainFrame ()->Progress ().DestroyWindow ();
return (nErrors == 0);
}

//---------------------------------------------------------------------------------

bool CLightManager::PointSeesPoint (CVertex* p1, CVertex* p2, short nStartSeg, short nDestSeg, short nDestSide)
{
	CVertex	lightDir = *p2 - *p1;
	double	lightDist = lightDir.Mag ();
	double	maxDist = m_staticRenderDepth * 20.0;
	int		nSegments = segmentManager.Count ();
	int		nExpanded = 0;
#if 1
	CDynamicArray< short> segmentList;
	CDynamicArray< ubyte> visited;
	segmentList.Create (nSegments);
	visited.Create (nSegments);
	visited.Clear ();
	segmentList.Clear (0xff);
#else
	short		segmentList [SEGMENT_LIMIT];
	short		bVisited [SEGMENT_LIMIT];
	memset (visited, 0, nSegments * sizeof (short));
#endif
	int		nHead = 1, nTail = 0;

visited [nStartSeg] = 1;

segmentList [0] = nStartSeg;
while (nTail < nHead) {
	short nSegment = segmentList [nTail++];
	CSegment* pSegment = segmentManager.Segment (nSegment);
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		if (0.0 > pSide->LineHitsFace (p1, p2, pSegment->m_info.vertexIds, maxDist))
			continue;
		if ((nSegment == nDestSeg) && (nSide == nDestSide))
			return true;
		if (!pSide->IsVisible ()) {
			if (nHead >= nSegments)
				return false;
			short nChildSeg = pSegment->ChildId (nSide);
			if ((nChildSeg >= 0) && !visited [nChildSeg]) {
				if (nHead >= nSegments) {
					STATUSMSG ("Buffer overflow in PointSeesPoint!");
					return false;
					}
				visited [nChildSeg] = 1;
				segmentList [nHead++] = nChildSeg;
				}
			}
#if 0
		else if ((d > 1e-6) && (d < lightDist))
			return false;
#endif
		}
	}
return false;
}

//---------------------------------------------------------------------------------

double CLightManager::CornerLight (CVertex* sourceCorners, CVertex& sourceNormal, short nSourceVerts, 
											  CVertex* destCorner, CVertex& destNormal, 
											  short nStartSeg, short nDestSeg, short nDestSide, bool bIgnoreAngle)
{
	double	bestLight = 0.0;

for (int i = 0; i < nSourceVerts - 1; i++) {
	CVertex destDir = *destCorner - sourceCorners [i];
	double length = destDir.Mag ();
	double light = 1.0;
	//if (!bIgnoreAngle) {
		destDir.Normalize ();
		double destAngle = destNormal ^ destDir;
		double lightAngle = (destAngle > 0.0) ? -destAngle : sourceNormal ^ destDir;
#if CHECK_POINT_VISIBILITY
		if (!PointSeesPoint (sourceCorners + i, destCorner, nStartSeg, nDestSeg, nDestSide))
			light = 0.3;
#endif
		if (lightAngle < 0.0) {
			light *= (1.0 + lightAngle);
			//light *= light;
			}
		//}
	double attenuation = (length * LINEAR_ATTENUATION) + (length * length * QUADRATIC_ATTENUATION);
	if (attenuation > 1.0)
		light /= attenuation;
	if (bestLight < light) 
		bestLight = light;
	}
return bestLight;
}

//---------------------------------------------------------------------------------

bool CLightManager::CalcCornerLights (double cornerLights [], int nChildSeg, int nChildSide, int nSourceSeg, int nSourceSide, CVertex* sourceCorners, CVertex& sourceNormal, bool bIgnoreAngle)
{
#if DBG
if ((nChildSeg == nDbgSeg) && ((nDbgSide < 0) || (nChildSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif
	double lightScale = 1.0;
	
CVertex destNormal = Average (segmentManager.Segment (nChildSeg)->Side (nChildSide)->m_vNormal [0], segmentManager.Segment (nChildSeg)->Side (nChildSide)->m_vNormal [1]);
// calculate vector between center of source segment and center of child
// calculate angle between vectors (use dot product equation)
if (Quality () == 1) {
	if (!bIgnoreAngle) {
		CVertex destDir = segmentManager.CalcSideCenter (CSideKey (nChildSeg, nChildSide)) - segmentManager.CalcSideCenter (CSideKey (nSourceSeg, nSourceSide));
		destDir.Normalize ();
		double destMag = destDir.Mag ();
		if (destMag < 0.000001)
			return false;
		double dot = sourceNormal ^ destDir;
		double ratio = dot / destMag;
		if (ratio < -1.0 || ratio > 1.0) 
			return false;
		double angle = acos (ratio);  
		// if angle is less than 180 degrees then we found a match
		if (angle >= Radians (180.0))
			return false;
		}
	}

CSegment* pSegment = segmentManager.Segment (nChildSeg);
int nDestVertices = pSegment->Side (nChildSide)->VertexCount ();

CVertex destCorners [4];
for (int i = 0; i < nDestVertices; i++) {
	destCorners [i] = *vertexManager.Vertex (pSegment->m_info.vertexIds [pSegment->Side (nChildSide)->VertexIdIndex (i)]);
	}

short nSourceVertices = segmentManager.Segment (nSourceSeg)->Side (nSourceSide)->VertexCount ();
for (int i = 0; i < nDestVertices; i++) {
	if (Quality () == 0)
		cornerLights [i] = CornerLight (sourceCorners, sourceNormal, nSourceVertices + 1, &destCorners [i], destNormal, nSourceSeg, nChildSeg, nChildSide, bIgnoreAngle);
	else {
		double length = 20.0 * m_staticRenderDepth;
		for (int j = 0; j < nSourceVertices; j++)
			length = min (length, Distance (sourceCorners [j], destCorners [i]));
		length /= 10.0 * m_staticRenderDepth / 6.0; // divide by 1/2 a segments length so opposite side
		cornerLights [i] = lightScale;
		if (length > 1.0) 
			cornerLights [i] /= (length * length);
		}
	}
// if any of the effects are > 0, then increment the light for that side
#if DBG
if ((nChildSeg == nDbgSeg) && ((nDbgSide < 0) || (nChildSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif
return (cornerLights [0] != 0.0) || (cornerLights [1] != 0.0) || (cornerLights [2] != 0.0) || (cornerLights [3 % nDestVertices] != 0.0);
}

// -----------------------------------------------------------------------------

void CLightManager::SetObjectLight (bool bAll, bool bDynSegLights)
{
	double fLight = m_fObjectLight / 100.0;
	// SegmentTool scales light values by about 12. I don't really know why, and haven't found anything
	// that explains it in D1/D2 source, but it looks about right when examining in-built levels.
	// So leaving it like this for now.
	long nLight = D2X (fLight) * 12;

undoManager.Begin (__FUNCTION__, udSegments);
int nSegments = segmentManager.Count ();
for (int si = 0; si < nSegments; si++) {
	CSegment* pSegment = segmentManager.Segment (si);
	if (bAll || (pSegment->IsTagged ())) {
		if (!bDynSegLights) {
			pSegment->m_info.staticLight = nLight;
			}
		else {
			int l = 0;
			int c = 0;
			CSide* pSide = pSegment->m_sides;
			for (short nSide = 0; nSide < 6; nSide++) {
				if (pSide->FaceCount () > 0) {
					for (short nCorner = 0; nCorner < pSide->VertexCount (); nCorner++) {
						l += pSide [nSide].m_info.uvls [nCorner].l;
						c++;
						}
					}
				}
			pSegment->Backup ();
			// Multiply by an extra 2 because of how corner lights are scaled (65536 = 200%)
			pSegment->m_info.staticLight = c ? D2X (fLight * (X2D (l) / (double) c)) * 2 * 12 : nLight;
			}
		}
	}
undoManager.End (__FUNCTION__);
}

// -----------------------------------------------------------------------------

void CLightManager::Normalize (void)
{
	short	nSegments = segmentManager.Count ();
	uint	maxLight = uint (65536 * m_fLightScale / 200.0) - 1;

#pragma omp parallel for if (nSegments > 15)
for (short nSegment = 0; nSegment < nSegments; nSegment++) {
	CSegment* pSegment = segmentManager.Segment (nSegment);
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		CUVL* pTexCoord = pSide->m_info.uvls;
		short nVertices = pSide->VertexCount ();
		for (short nVertex = 0; nVertex < nVertices; nVertex++, pTexCoord++) {
			if (pTexCoord->l > maxLight) {
#ifdef _DEBUG
				if ((nSegment == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
					nDbgSeg = nDbgSeg;
#endif
				int indexCount = lightManager.DeltaIndexCount ();
				CLightDeltaIndex* pIndex = lightManager.LightDeltaIndex (0);
				uint deltaLight = 0;
				for (int nIndex = 0; nIndex < indexCount; nIndex++, pIndex++) {
#ifdef _DEBUG
					if ((pIndex->m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (pIndex->m_nSide == nDbgSide)))
						nDbgSeg = nDbgSeg;
#endif
					if (VariableLight (*pIndex) < 0)
						continue; // there's no variable light for this delta light index (-> light data error!)
					CLightDeltaValue* pLightDelta = lightManager.LightDeltaValue (pIndex->m_info.index);
					for (ushort nDelta = pIndex->m_info.count; nDelta; nDelta--, pLightDelta++) {
						if ((pLightDelta->m_nSegment == nSegment) && (pLightDelta->m_nSide == nSide))
							deltaLight += pLightDelta->m_info.vertLight [nVertex];
						}
					}
	
				if (deltaLight > pTexCoord->l)
					pTexCoord->l = deltaLight;
				double lightScale = double (maxLight) / double (pTexCoord->l);
				pTexCoord->l = maxLight;
				indexCount = lightManager.DeltaIndexCount ();
				pIndex = lightManager.LightDeltaIndex (0);
				for (int nIndex = 0; nIndex < indexCount; nIndex++, pIndex++) {
#ifdef _DEBUG
					if ((pIndex->m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (pIndex->m_nSide == nDbgSide)))
						nDbgSeg = nDbgSeg;
#endif
					if (VariableLight (*pIndex) < 0)
						continue; // there's no variable light for this delta light index (-> light data error!)
					CLightDeltaValue* pLightDelta = lightManager.LightDeltaValue (pIndex->m_info.index);
					for (ushort nDelta = pIndex->m_info.count; nDelta; nDelta--, pLightDelta++) {
						if ((pLightDelta->m_nSegment == nSegment) && (pLightDelta->m_nSide == nSide))
#pragma omp critical 
							{
							pLightDelta->m_info.vertLight [nVertex] = uint (double (pLightDelta->m_info.vertLight [nVertex]) * lightScale + 0.5);
							}
						}
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------
//eof Illumination.cpp

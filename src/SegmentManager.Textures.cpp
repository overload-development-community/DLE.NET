// Segment.cpp

#include "mine.h"
#include "dle-xp.h"

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

// -----------------------------------------------------------------------------

void CSegmentManager::Textures (CSideKey key, short& nBaseTex, short& nOvlTex)
{
current->Get (key);
Side (key)->GetTextures (nBaseTex, nOvlTex);
}

// -----------------------------------------------------------------------------

bool CSegmentManager::SetTextures (CSideKey key, int nBaseTex, int nOvlTex)
{
	bool bChange = false;

undoManager.Begin (__FUNCTION__, udSegments); 
current->Get (key); 
CSide *pSide = Side (key); 
bChange = pSide->SetTextures (nBaseTex, nOvlTex);
if (!bChange) {
	undoManager.End (__FUNCTION__);
	return false;
	}
if ((lightManager.IsLight (pSide->BaseTex ()) == -1) && (lightManager.IsLight (pSide->OvlTex (0)) == -1))
	lightManager.DeleteVariableLight (key); 
if (!wallManager.ClipFromTexture (key))
	wallManager.CheckForDoor (key); 
undoManager.End (__FUNCTION__); 
sprintf_s (message, sizeof (message), "side has textures %d, %d", pSide->BaseTex () & 0x1fff, pSide->OvlTex (0)); 
INFOMSG (message); 
return true;
}

//------------------------------------------------------------------------------

void CSegmentManager::AlignTextures (short nSegment, short nSide, int bUse1st, int bUse2nd, int bIgnorePlane, bool bAlignAll)
{
	bool bTaggedSidesOnly = segmentManager.HasTaggedSegments (true);
	CSideKey nextSideKey (nSegment, nSide);
	CSide* pNextSide = segmentManager.Side (nextSideKey);
	// Search counters for bAlignAll - initialized outside the loop to ensure we only go through once
	int nSearchSegment = 0;
	short nSearchSide = 0;

segmentManager.UnTagAll (ALIGNED_MASK);

do {
	// mark current side as aligned
	pNextSide->Tag (ALIGNED_MASK);
	AlignChildTextures (nextSideKey, bUse1st, bUse2nd, bIgnorePlane);

	if (bAlignAll) {
		// Look for the next side that hasn't been aligned yet
		pNextSide = nullptr;
		for (; nSearchSegment < segmentManager.Count (); nSearchSegment++) {
			for (; nSearchSide < 6; nSearchSide++) {
				CSide* pSide = segmentManager.Segment (nSearchSegment)->Side (nSearchSide);
				if (pSide->IsVisible () && // is it alignable?
					(!bTaggedSidesOnly || pSide->IsTagged ()) && // is it marked (if applicable)?
					!pSide->IsTagged (ALIGNED_MASK)) // is it not already aligned?
					{
					nextSideKey = CSideKey (nSearchSegment, nSearchSide);
					pNextSide = pSide;
					break;
					}
				}
			if (pNextSide)
				break;
			}
		}

	} while (bAlignAll && pNextSide);

segmentManager.UnTagAll (ALIGNED_MASK | ALIGN_MASK);
}

//------------------------------------------------------------------------------

void CSegmentManager::AlignChildTextures (CSideKey sideKey, int bUse1st, int bUse2nd, int bIgnorePlane)
{
	CSide* pSide = segmentManager.Side (sideKey);
	CTagByTextures tagger (bUse1st ? pSide->BaseTex () : -1, bUse2nd ? pSide->OvlTex (0) : -1, bIgnorePlane != 0);

if (!tagger.Setup (segmentManager.VisibleSideCount (), ALIGN_MASK))
	return;
int nSides = tagger.Run (sideKey.m_nSegment, sideKey.m_nSide);

for (int i = 0; i < nSides; i++) {
	segmentManager.AlignSideTextures (tagger.ParentSegment (i), tagger.ParentSide (i), tagger.ChildSegment (i), tagger.ChildSide (i), bUse1st, bUse2nd);
	segmentManager.Segment (tagger.ChildSegment (i))->Side (tagger.ChildSide (i))->Tag (ALIGNED_MASK);
	}
}

// ------------------------------------------------------------------------ 

int CSegmentManager::AlignSideTextures (short nStartSeg, short nStartSide, short nChildSeg, short nChildSide, int bAlign1st, int bAlign2nd)
{
#ifdef _DEBUG
if ((nStartSeg < 0) || (nStartSeg >= Count ()))
	return -1;
if ((nStartSide < 0) || (nStartSide >= 6))
	return -1;
if ((nChildSeg < 0) || (nChildSeg >= Count ()))
	return -1;
if ((nChildSide < 0) || (nChildSide >= 6))
	return -1;

if ((nStartSeg == nDbgSeg) && ((nDbgSide < 0) || (nStartSide == nDbgSide)))
	nDbgSeg = nDbgSeg;
#endif

CSegment*	pSegment = Segment (nStartSeg); 
CSide*		pSide = pSegment->m_sides + nStartSide; 

if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
	return -1;

CSegment*	pChildSeg = Segment (nChildSeg); 
CSide*		pChildSide = pChildSeg->m_sides + nChildSide; 
short			nEdgeCount = pSide->VertexCount ();
short			nChildEdgeCount = pChildSide->VertexCount ();

short			i; 
short			nEdge, nChildEdge; 

// ignore children with no texture
if (!IsWall (CSideKey (nChildSeg, nChildSide)))
	return -1;

// find matching lines first
for (nEdge = 0; nEdge < nEdgeCount; nEdge++) {
	// find vert numbers for the line's two end points
	ushort vert0 = pSegment->VertexId (nStartSide, nEdge); 
	ushort vert1 = pSegment->VertexId (nStartSide, nEdge + 1); // the index (here: nEdge + 1) gets automatically wrapped by this function

	for (nChildEdge = 0; nChildEdge < nChildEdgeCount; nChildEdge++) {
		// find vert numbers for the line's two end points
		ushort nChildVert0 = pChildSeg->VertexId (nChildSide, nChildEdge);
		ushort nChildVert1 = pChildSeg->VertexId (nChildSide, nChildEdge + 1);
		// if points of child's line == corresponding points of parent
		if (((nChildVert0 == vert1) && (nChildVert1 == vert0)) || ((nChildVert0 == vert0) && (nChildVert1 == vert1)))
			break;
		}
	if (nChildEdge != nChildEdgeCount)
		break;
	}
if ((nEdge == nEdgeCount) || (nChildEdge == nChildEdgeCount))
	return -1;

// now we know the child's side & line which touches the parent
// child:  nChildSeg, nChildSide, nChildEdge, nChildPoint0, nChildPoint1
// parent: nStartSeg, nStartSide, nEdge, point0, point1
// we can start undoManager now - since we know we are making changes now and won't abort inside this block
undoManager.Begin (__FUNCTION__, udSegments);

double angle;

if (bAlign1st) {
	CUVL uv;
	// now translate all the childs (u, v) coords so the reference point is at zero
	uv = pChildSide->m_info.uvls [(nChildEdge + 1) % nChildEdgeCount]; 
	for (i = 0; i < nChildEdgeCount; i++)
		pChildSide->m_info.uvls [i] -= uv; 
	// find difference between parent point0 and child point1
	uv = pChildSide->m_info.uvls [(nChildEdge + 1) % nChildEdgeCount] - pSide->m_info.uvls [nEdge]; 
#if 1
	// "round" to the -20 to 20 neighborhood
	uv.u = fmod (uv.u, 1);
	uv.v = fmod (uv.v, 1);
#endif
	// find the angle formed by the two lines
	double sAngle = atan3 (pSide->m_info.uvls [(nEdge + 1) % nEdgeCount].v - pSide->m_info.uvls [nEdge].v, 
								  pSide->m_info.uvls [(nEdge + 1) % nEdgeCount].u - pSide->m_info.uvls [nEdge].u); 
	double cAngle = atan3 (pChildSide->m_info.uvls [nChildEdge].v - pChildSide->m_info.uvls [(nChildEdge + 1) % nChildEdgeCount].v, 
					 			  pChildSide->m_info.uvls [nChildEdge].u - pChildSide->m_info.uvls [(nChildEdge + 1) % nChildEdgeCount].u); 
	// now rotate childs (u, v) coords around child_point1 (cAngle - sAngle)
	angle = cAngle - sAngle;
	for (i = 0; i < nChildEdgeCount; i++) 
		pChildSide->m_info.uvls [i].Rotate (angle); 
	// now translate all the childs (u, v) coords to parent point0
	for (i = 0; i < nChildEdgeCount; i++)
		pChildSide->m_info.uvls [i] -= uv; 
	}
if (bAlign2nd && pSide->OvlTex (0) && pChildSide->OvlTex (0)) {
	int r, h = pSide->OvlAlignment ();
	if (h == 3)
		r = 1;
	else if (h == 2)
		r = 2;
	else if (h == 1)
		r = 3;
	else
		r = 0;
	if (angle < 0.0)
		angle += 2 * PI;
	h = (int) Round (Degrees (angle) / 90.0); 
	h = (h + r) % nEdgeCount;
	pChildSide->m_info.nOvlTex &= TEXTURE_MASK;
	if (h == 1)
		pChildSide->m_info.nOvlTex |= 0xC000;
	else if (h == 2)
		pChildSide->m_info.nOvlTex |= 0x8000;
	else if (h == 3)
		pChildSide->m_info.nOvlTex |= 0x4000;
	}

undoManager.End (__FUNCTION__);
return nChildSide; 
}

// -----------------------------------------------------------------------------

void CSegmentManager::UpdateTexCoords (ushort nVertexId, bool bMove, short nIgnoreSegment, short nIgnoreSide)
{
// Automatic texture adjustment can be disabled, so check for that first
if (!m_bUpdateAlignmentOnEdit)
	return;

CSegment* pSegment;
for (short nSegment = 0; 0 <= (nSegment = FindByVertex (nVertexId, nSegment)); nSegment++, pSegment++) {
	pSegment = Segment (nSegment);
	if (nIgnoreSegment >= 0) {
		if ((nIgnoreSide < 0) && (nSegment == nIgnoreSegment))
			continue;
		if ((nIgnoreSegment == 0x7FFF) && pSegment->IsTagged ())
			continue;
		}
	pSegment->UpdateTexCoords (nVertexId, bMove, (nSegment == nIgnoreSegment) ? nIgnoreSide : -1);
	}
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//eof segmentmanager.textures.cpp
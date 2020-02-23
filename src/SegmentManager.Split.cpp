// Segment.cpp

#include "mine.h"
#include "dle-xp.h"

// ----------------------------------------------------------------------------- 
// unlink_child()
//
// Action - unlinks current segment's children which don't share all four points
//
// Note: 2nd parameter "nSide" is ignored
// ----------------------------------------------------------------------------- 

void CSegmentManager::UnlinkChild (short nParentSeg, short nSide) 
{
  CSegment *pParentSeg = Segment (nParentSeg); 

// loop on each side of the parent
int nChildSeg = pParentSeg->ChildId (nSide); 
// does this side have a child?
if (nChildSeg < 0 || nChildSeg >= Count ())
	return;
CSegment *pChildSeg = Segment (nChildSeg); 
// yes, see if child has a side which points to the parent
int nChildSide;
for (nChildSide = 0; nChildSide < 6; nChildSide++)
	if (pChildSeg->ChildId (nChildSide) == nParentSeg) 
		break; 
// if we found the matching side
if (nChildSide < 6) {
// define vert numbers for comparison
	ushort* vertexIds = pParentSeg->m_info.vertexIds;
	CSide* pSide = pParentSeg->Side (nSide), * pChildSide = pChildSeg->Side (nChildSide);
	int nVertices = pSide->VertexCount (), nMatch = 0;
	for (int i = 0; i < nVertices; i++) {
		ushort v = pChildSeg->VertexId (nChildSide, i); 
		for (int j = 0; j < nVertices; j++) {
			if (vertexIds [pSide->VertexIdIndex (j)] == v) {
				++nMatch;
				break;
				}
			}
		}
	if (nMatch != nVertices) { // they don't share all four points correctly, so unlink the child from the parent and unlink the parent from the child
		undoManager.Begin (__FUNCTION__, udSegments);
		ResetSide (nChildSeg, nChildSide); 
		ResetSide (nParentSeg, nSide); 
		undoManager.End (__FUNCTION__);
		}
	}
else {
	// if the child does not point to the parent, 
	// then just unlink the parent from the child
	ResetSide (nParentSeg, nSide); 
	}
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsPointOfSide (CSegment *pSegment, int nSide, int nPoint)
{
CSide* pSide = pSegment->Side (nSide);
for (int i = 0, j = pSide->VertexCount (); i < j; i++)
	if (pSide->m_vertexIdIndex [i] == nPoint)
		return true;
return false;
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsLineOfSide (CSegment *pSegment, int nSide, int nLine)
{
for (int i = 0; i < 2; i++)
	if (!IsPointOfSide (pSegment, nSide, edgeVertexTable [nLine][i]))
		return false;
return true;
}

// ----------------------------------------------------------------------------- 
//                          Splitpoints()
//
// Action - Splits one point shared between two cubes into two points.
//          New point is added to current segment, other segment is left alone.
//
// ----------------------------------------------------------------------------- 

int CSegmentManager::SeparatePoints (CSideKey key, int nVertexId, bool bVerbose) 
{
ushort nSegment, nVertex; 

if (tunnelMaker.Active ())
	return -1; 

current->Get (key);

if (vertexManager.Count () > (VERTEX_LIMIT - 1)) {
	if (bVerbose)
		ErrorMsg ("Cannot unjoin these points because the\nmaximum number of points is reached."); 
	return -1; 
	}

CSegment* pSegment = Segment (key.m_nSegment); 
if (nVertexId < 0)
	nVertexId = current->VertexId (); 

// check to see if current point is shared by any other cubes
bool found = false; 
pSegment = Segment (0);
for (nSegment = 0; (nSegment < Count ()) && !found; nSegment++, pSegment++)
	if (nSegment != key.m_nSegment)
		for (nVertex = 0; nVertex < 8; nVertex++)
			if (pSegment->m_info.vertexIds [nVertex] == nVertexId) {
				found = true; 
				break; 
				}
if (!found) {
	if (bVerbose)
		ErrorMsg ("This point is not joined with any other point."); 
	return nVertexId; 
	}

if (bVerbose && (QueryMsg ("Are you sure you want to unjoin this point?") != IDYES))
	return nVertexId; 

undoManager.Begin (__FUNCTION__, udSegments | udVertices);
memcpy (vertexManager.Vertex (vertexManager.Count ()), vertexManager.Vertex (nVertexId), sizeof (*vertexManager.Vertex (0)));
// replace existing point with new point
pSegment = Segment (key.m_nSegment); 
int nPoint = pSegment->UpdateVertexId (nVertexId, vertexManager.Count ()); 
pSegment->UnTag (); 

// update total number of vertices
for (short nSide = 0; nSide < 6; nSide++) {
	CSideKey back;
	if (IsPointOfSide (pSegment, nSide, nPoint) && BackSide (CSideKey (key.m_nSegment, nSide), back)) {
		UnlinkChild (back.m_nSegment, back.m_nSide);
		UnlinkChild (key.m_nSegment, nSide); 
		}
	}	

vertexManager.Status (vertexManager.Count ()++) = 0; 

SetLinesToDraw (); 
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
if (bVerbose)
	INFOMSG ("A new point was made for the current point."); 
return vertexManager.Count () - 1;
}

// ----------------------------------------------------------------------------- 
//                         Splitlines()
//
// Action - Splits common lines of two cubes into two lines.
//
// ----------------------------------------------------------------------------- 

void CSegmentManager::SeparateLines (short nLine) 
{
if (tunnelMaker.Active ()) 
	return; 

if (nLine < 0) {
	nLine = current->Edge ();
	}

	CSegment*	pSegment = current->Segment ();
	CSide*		pSide = current->Side ();
	ubyte			vertexIdIndex [2] = {pSide->VertexIdIndex (nLine), pSide->VertexIdIndex (nLine + 1)};
	ushort		nEdgeVerts [2];
	bool			bShared [2];
	int			verticesRequired = 0;

for (size_t i = 0; i < 2; i++) {
	nEdgeVerts [i] = pSegment->m_info.vertexIds [vertexIdIndex [i]];
	bShared [i] = VertexInUse (nEdgeVerts [i]);
	if (bShared [i])
		verticesRequired++;
	}
if (verticesRequired == 0) {
	if (!DLE.ExpertMode ())
		ErrorMsg ("This line is already unjoined.");
	return;
	}
if (vertexManager.Count () > (MAX_VERTICES - verticesRequired)) {
	if (!DLE.ExpertMode ())
		ErrorMsg ("Cannot unjoin this line because\nthere are not enough vertices left.");
	return;
	}

if (QueryMsg ("Are you sure you want to unjoin this line?") != IDYES)
	return;

undoManager.Begin (__FUNCTION__, udSegments | udVertices);
// create new points (copy of other vertices)
for (size_t i = 0; i < 2; i++)
	if (bShared [i]) {
		// Clone the vertex
		ushort nVertex;
		vertexManager.Add (&nVertex);
		vertexManager [nVertex] = vertexManager [nEdgeVerts [i]];
		vertexManager [nVertex].Status () = 0;

		// Re-point the segment to the new vertex
		pSegment->SetVertexId (vertexIdIndex [i], nVertex);
		pSegment->UnTag ();
		nEdgeVerts [i] = nVertex;
		}
// Unlink all adjacent segments that share this edge, too
for (short nSide = 0; nSide < 6; nSide++) {
	CSideKey back, key (current->SegmentId (), nSide);
	if (pSegment->HasEdge (nSide, nEdgeVerts [0], nEdgeVerts [1]) && BackSide (key, back)) {
		UnlinkChild (back.m_nSegment, back.m_nSide);
		UnlinkChild (current->SegmentId (), nSide); 
		}
	}

SetLinesToDraw (); 
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
sprintf_s (message, sizeof (message), "%d new points were made for the current line.", verticesRequired);
INFOMSG (message);
}

// ----------------------------------------------------------------------------- 
//                       Splitsegments()
//
// ACTION - Splits a segment from all other points which share its coordinates
//
//  Changes - Added option to make thin side
// If solidify == 1, the side will keep any points it has in common with other
// sides, unless one or more of its vertices are already solitaire, in which
// case the side needs to get disconnected from its child anyway because that 
// constitutes an error in the level structure.
// ----------------------------------------------------------------------------- 

bool CSegmentManager::SeparateSegments (int bSolidify, int nSide, bool bVerbose) 
{
if (tunnelMaker.Active ())
	return false; 

	CSegment* pSegment = current->Segment (); 

int nChildSeg = pSegment->ChildId (nSide); 
if (nChildSeg == -1) {
	ErrorMsg ("The current side is not connected to another segment"); 
	return false; 
	}
if (nSide < 0) {
	nSide = current->SideId ();
	}

	CSide*	pSide = pSegment->Side (nSide);
	int		nVertices = pSide->VertexCount ();
	ushort	nFaceVerts [4];
	bool		bShared [4];
	int		i, nShared = 0;

// check to see if current points are shared by any other cubes
for (i = 0; i < nVertices; i++) {
	nFaceVerts [i] = pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)]; 
	if ((bShared [i] = VertexInUse (nFaceVerts [i])))
		++nShared;
	}
// If a side has one or more solitary points but has a child, there is 
// something wrong. However, nothing speaks against completely unjoining it.
// In fact, this does even cure the problem. So, no error message.
//	ErrorMsg ("One or more of these points are not joined with any other points."); 
//	return; 

if (!bSolidify && (vertexManager.Count () > (MAX_VERTICES - nShared))) {
	ErrorMsg ("Cannot unjoin this side because\nthere are not enough vertices left."); 
	return false; 
	}

if (bVerbose && (QueryMsg ("Are you sure you want to unjoin this side?") != IDYES))
	return false; 

undoManager.Begin (__FUNCTION__, udSegments | udVertices);
pSegment = Segment (current->SegmentId ()); 
if (nShared < nVertices)
	bSolidify = 0;
if (!bSolidify) {
	// create new points (copy of other vertices)
	for (i = 0; i < nVertices; i++) {
		if (bShared [i]) {
			memcpy (vertexManager.Vertex (vertexManager.Count ()), vertexManager.Vertex (nFaceVerts [i]), sizeof (*vertexManager.Vertex (0)));
			// replace existing points with new points
			pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)] = vertexManager.Count (); 
			pSegment->UnTag (); 
			vertexManager.Status (vertexManager.Count ()++) = 0; 
			}
		}
	ubyte nOppSide = oppSideTable [nSide];
	for (int nSide = 0; nSide < 6; nSide++)
		if (nSide != nOppSide)
			UnlinkChild (current->SegmentId (), nSide); 
	SetLinesToDraw (); 
	sprintf_s (message, sizeof (message), "%d new points were made for the current side.", nShared);
	INFOMSG (message);
	}
else {
	// does this side have a child?
	CSegment *pChildSeg = Segment (nChildSeg); 
	// yes, see if child has a side which points to the parent
	int nChildSide;
	for (nChildSide = 0; nChildSide < 6; nChildSide++)
		if (pChildSeg->ChildId (nChildSide) == current->SegmentId ()) 
			break; 
	// if we bShared the matching side
	if (nChildSide < 6)
		ResetSide (nChildSeg, nChildSide); 
	ResetSide (current->SegmentId (), current->SideId ()); 
	SetLinesToDraw (); 
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
return true;
}

// -----------------------------------------------------------------------------
// Splits a segment so that it contains a small segment inside that is surrounded
// by 6 (more or less) conical segments

bool CSegmentManager::SplitIn7 (void)
{
	CSegment*	pCenterSeg = current->Segment ();
	short			nCenterSeg = segmentManager.Index (pCenterSeg);
	short			nNewSegs [6];
	ushort		nNewVerts [8];
	CVertex		segCenter;
	bool			bVertDone [8];

if (pCenterSeg->m_nShape) {
	ErrorMsg ("Cannot split segments with triangular faces."); 
	return false;
	}

if (Count () >= SEGMENT_LIMIT - 6) {
	ErrorMsg ("Cannot split this segment because\nthe maximum number of segments would be exceeded."); 
	return false;
	}
undoManager.Begin (__FUNCTION__, udSegments | udVertices);
//h = vertexManager.Count ();
// compute segment center
vertexManager.Add (nNewVerts, 8);
CalcCenter (segCenter, Index (pCenterSeg));
// add center segment
// compute center segment vertices
memset (bVertDone, 0, sizeof (bVertDone));

for (short j = 0; j < 8; j++)
	*vertexManager.Vertex (nNewVerts [j]) = Average (*pCenterSeg->Vertex (j), segCenter);

// create the surrounding segments
CSide* pSide = pCenterSeg->Side (0);
for (short nSide = 0; nSide < 6; nSide++, pSide++) {
	nNewSegs [nSide] = Add ();
	short nSegment = nNewSegs [nSide];
	CSegment* pSegment = Segment (nSegment);
	ubyte oppVertexIdIndex [4];
	pSegment->Setup ();
	memcpy (pSegment->m_info.vertexIds, pCenterSeg->m_info.vertexIds, sizeof (pSegment->m_info.vertexIds));
	pCenterSeg->CreateOppVertexIndex (nSide, oppVertexIdIndex);
	for (ushort nVertex = 0; nVertex < 4; nVertex++)
		pSegment->m_info.vertexIds [oppVertexIdIndex [nVertex]] = nNewVerts [pSide->m_vertexIdIndex [nVertex]];
	if ((pSegment->SetChild (nSide, pCenterSeg->ChildId (nSide))) > -1) 
		Segment (pSegment->ChildId (nSide))->ReplaceChild (nCenterSeg, nSegment);
	pSegment->SetChild (oppSideTable [nSide], nCenterSeg);
	pCenterSeg->SetChild (nSide, nSegment);
	CWall* pWall = pCenterSeg->m_sides [nSide].Wall ();
	if (pWall == null)
		pSegment->m_sides [nSide].m_info.nWall = NO_WALL;
	else {
		pSegment->m_sides [nSide].m_info.nWall = wallManager.Index (pWall);
		pWall->m_nSegment = nSegment;
		pCenterSeg->m_sides [nSide].m_info.nWall = NO_WALL;
		}
	}
// relocate center segment vertex indices
memcpy (pCenterSeg->m_info.vertexIds, nNewVerts, sizeof (nNewVerts));
// join adjacent sides of the segments surrounding the center segment
// don't process 6th segment as this is handled by processing the 1st one already
for (short nSegment = 0; nSegment < 5; nSegment++) {
	CSegment* pSegment = Segment (nNewSegs [nSegment]);
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		if (pSegment->ChildId (nSide) >= 0)
			continue;
		for (short nChildSeg = nSegment + 1; nChildSeg < 6; nChildSeg++) {
			CSegment* pChildSeg = Segment (nNewSegs [nChildSeg]);
			CSide* pChildSide = pChildSeg->Side (0);
			for (short nChildSide = 0; nChildSide < 6; nChildSide++, pChildSide++) {
				if (pChildSeg->ChildId (nChildSide) >= 0)
					continue;
				short h = 0;
				for (short i = 0; i < 4; i++) {
					ushort k = pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)];
					for (short j = 0; j < 4; j++) {
						if (k == pChildSeg->m_info.vertexIds [pChildSide->m_vertexIdIndex[j]]) {
							h++;
							break;
							}
						}
					}
				if (h == 4) {
					pSegment->SetChild (nSide, nNewSegs [nChildSeg]);
					pChildSeg->SetChild (nChildSide, nNewSegs [nSegment]);
					break;
					}
				}
			}
		}
	}
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
return true;
}

// -----------------------------------------------------------------------------

static inline short EdgeIndex (short nVertex1, short nVertex2)
{
for (short i = 0; i < 12; i++) {
	if ((edgeVertexTable [i][0] == nVertex1) && (edgeVertexTable [i][1] == nVertex2))
		return i + 1;
	if ((edgeVertexTable [i][1] == nVertex1) && (edgeVertexTable [i][0] == nVertex2))
		return -i - 1;
	}
return 0;
}

// -----------------------------------------------------------------------------

static inline short FindSidePoint (ubyte nPoint, ubyte* points)
{
for (short i = 0; i < 4; i++)
	if (points [i] == nPoint)
		return i;
return -1;
}

// -----------------------------------------------------------------------------
// search through all sides of a segment and look for the one having the three
// corners nPoint0, nPoint1 and nPoint2. Return the side's fourth corner's index.

static short FindCornerByPoints (ubyte nPoint0, ubyte nPoint1, ubyte nPoint2, short& nSide)
{
	ubyte refPoints [3] = { nPoint0, nPoint1, nPoint2 };

for (nSide = 0; nSide < 6; nSide++) {
	ubyte sidePoints [6];
	memcpy (sidePoints, sideVertexTable [nSide], 6 * sizeof (sidePoints [0]));
	short i, j;
	for (i = 0; i < 3; i++) {
		j = FindSidePoint (refPoints [i], sidePoints);
		if (j < 0)
			break; // not found
		sidePoints [j] = 255; // remove and mark as found
		}
	if (j >= 0) {
		for (i = 0; i < 4; i++)
			if (sidePoints [i] < 255)
				return (short) sidePoints [i];
		}
	}
return -1;
}

// -----------------------------------------------------------------------------
// Splits a segment into 8 smaller segments

bool CSegmentManager::SplitIn8 (CSegment* pRootSeg)
{
if (!pRootSeg)
	pRootSeg = current->Segment ();

if (pRootSeg->m_nShape) {
	ErrorMsg ("Cannot split wedge and pyramid shaped segments."); 
	return false;
	}

	CSegment		rootSeg = *pRootSeg;
	short			nRootSeg = segmentManager.Index (pRootSeg);
	short			nSplitSegs [8];
	ushort		nNewVerts [19]; // 0: root segment center, 1 - 6: side centers, 7 - 18: edge centers
	CVertex		segCenter;
	short			nOldSegments = Count ();

if (nOldSegments >= SEGMENT_LIMIT - 7) {
	ErrorMsg ("Cannot split this segment because\nthe maximum number of segments would be exceeded."); 
	return false;
	}
undoManager.Begin (__FUNCTION__, udSegments | udVertices | udWalls);
//h = vertexManager.Count ();
// compute segment center
if (!vertexManager.Add (nNewVerts, 19)) {
	undoManager.Unroll (__FUNCTION__);
	ErrorMsg ("Cannot split this segment because\nthe maximum number of vertices would be exceeded."); 
	return false;
	}
CalcCenter (segCenter, Index (pRootSeg));
// compute side center vertices
*vertexManager.Vertex (nNewVerts [0]) = segCenter;
short j = 1;
for (short nSide = 0; nSide < 6; nSide++) {
	CVertex vCenter;
	for (ushort nVertex = 0; nVertex < 4; nVertex++) 
		vCenter += *pRootSeg->Vertex (pRootSeg->Side (nSide)->m_vertexIdIndex [nVertex]);
	*vertexManager.Vertex (nNewVerts [j++]) = vCenter / 4;
	}

CSegment* pChildSeg;
for (short nSide = 0; nSide < 6; nSide++)
	if ((pChildSeg = pRootSeg->Child (nSide))) {
		pChildSeg->RemoveChild (nRootSeg);
		pRootSeg->SetChild (nSide, -1);
		}

for (short nEdge = 0; nEdge < 12; nEdge++)
	*vertexManager.Vertex (nNewVerts [j++]) = Average (*pRootSeg->Vertex (edgeVertexTable [nEdge][0]), *pRootSeg->Vertex (edgeVertexTable [nEdge][1]));
for (j = 0; j < 19; j++)
	vertexManager.Status (nNewVerts [j]) |= TAGGED_MASK; 

// create the surrounding segments

// compute the vertices of the new segments. Start with the vertex of the corner a segment sits in.
// Use that corner to determine the adjacent vertices and compute three of the new segment's corners
// using the corner vertex and the adjacent vertices.
for (ubyte nCorner = 0; nCorner < 8; nCorner++) {
	nSplitSegs [nCorner] = (nCorner == 7) ? nRootSeg : Add ();
	short nSegment = nSplitSegs [nCorner];
	CSegment* pSegment = Segment (nSegment);
	ushort nBasePoint = rootSeg.VertexId (nCorner);
	ushort vertexIds [8];
	memset (vertexIds, 0xFF, sizeof (pSegment->m_info.vertexIds));

	ushort nVertex;
	ubyte nBasePointIdIndex = 255;

	for (short i = 0; i < 3; i++) {
		short nEdge = EdgeIndex (nCorner, adjacentPointTable [nCorner][i]);
		nVertex = (nEdge < 0) ? 0 : 1;
		nEdge = abs (nEdge) - 1;
		vertexIds [edgeVertexTable [nEdge][nVertex]] = nNewVerts [7 + nEdge];
		if (!i)
			vertexIds [nBasePointIdIndex = edgeVertexTable [nEdge][!nVertex]] = nBasePoint;
		}

	for (short i = 0; i < 2; i++) {
		for (short j = i + 1; j < 3; j++) {
			short nSide, nPoint = FindCornerByPoints (nCorner, adjacentPointTable [nCorner][i], adjacentPointTable [nCorner][j], nSide);
			vertexIds [nPoint] = nNewVerts [1 + nSide];
			}
		}

	short i;
	for (i = 0; i < 8; i++)
		if (vertexIds [i] == 0xFFFF) {
			vertexIds [i] = nNewVerts [0];
			break;
			}

	if (nSegment == nRootSeg) {
		memcpy (pSegment->m_info.vertexIds, vertexIds, sizeof (vertexIds));
		for (short j = 0; j < 3; j++) 
			pSegment->Side (adjacentSideTable [i][j])->DeleteWall ();
		}
	else {
		pSegment->Setup ();
		pSegment->Function () = rootSeg.Function ();
		pSegment->Props () = rootSeg.Props ();
		memcpy (pSegment->m_info.vertexIds, vertexIds, sizeof (vertexIds));
		}

	short nBaseTex, nOvlTex;
	for (short i = 0; i < 2; i++) {
		for (short j = i + 1; j < 3; j++) {
			short nSide, nPoint = FindCornerByPoints (nCorner, adjacentPointTable [nCorner][i], adjacentPointTable [nCorner][j], nSide);
			CSide	* pRootSide = rootSeg.Side (nSide),
					* pSide = pSegment->Side (nSide);
			if ((nSegment != nRootSeg) && pRootSide->Wall () && !pSide->Wall () && !wallManager.Full ()) {
				CSideKey key (nSegment, nSide);
				pSide->m_info.nWall = wallManager.Add (false, &key);
				CWall* pWall = wallManager.Wall (pSegment->Side (nSide)->m_info.nWall);
				if (pWall) {
					pWall->Info () = pRootSide->Wall ()->Info ();
					pWall->Info ().nTrigger = NO_TRIGGER;
					*((CSideKey*) pWall) = key;
					}
				}
			pRootSide->GetTextures (nBaseTex, nOvlTex);
			pSide->SetTextures (nBaseTex, nOvlTex);
			CUVL* pUvls = pSide->Uvls ();
			//memcpy (pSide->m_vertexIdIndex, pRootSide->m_vertexIdIndex, sizeof (pSide->m_vertexIdIndex));
			memcpy (pUvls, rootSeg.Uvls (nSide), 4 * sizeof (CUVL));
			short h = pSide->FindVertexIdIndex (nBasePointIdIndex);
			pUvls [(h + 1) % 4] = (pUvls [h] + pUvls [(h + 1) % 4]) * 0.5;
			pUvls [(h + 2) % 4] = (pUvls [h] + pUvls [(h + 2) % 4]) * 0.5;
			pUvls [(h + 3) % 4] = (pUvls [h] + pUvls [(h + 3) % 4]) * 0.5;
			}
		}
	}


for (int i = 0; i < 7; i++) {
	CSegment* pSegment = Segment (nSplitSegs [i]);
	for (int j = i + 1; j < 8; j++) {
		short nOtherSide, nSide = pSegment->CommonSides (nSplitSegs [j], nOtherSide);
		if (0 <= nSide)
			segmentManager.Link (nSplitSegs [i], nSide, nSplitSegs [j], nOtherSide, 1e-3);
		}
	}

for (int i = 0; i < 8; i++) {
	CSegment* pSegment = Segment (nSplitSegs [i]);
	for (int j = 0; j < nOldSegments; j++) {
		ushort nVertices [4];
		short nCommon = pSegment->CommonVertices (j, 4, nVertices);
		if (!nCommon)
			continue;
		CSegment* pOtherSeg = Segment (j);
		bool bLinked = false;
		for (short nSide = 0; !bLinked && (0 <= (nSide = pSegment->FindSide (nSide, nCommon, nVertices))); nSide++)
			for (short nOtherSide = 0; !bLinked && (0 <= (nOtherSide = pOtherSeg->FindSide (nOtherSide, nCommon, nVertices))); nOtherSide++)
				bLinked = segmentManager.Link (nSplitSegs [i], nSide, j, nOtherSide, 1.0);
		}
	}

memset (&rootSeg, 0, sizeof (rootSeg)); // beware of the d'tor
undoManager.End (__FUNCTION__);
DLE.MineView ()->Refresh ();
return true;
}

// ------------------------------------------------------------------------

bool CSegmentManager::CollapseEdge (short nSegment, short nSide, short nEdge, bool bUpdateCoord)
{
#ifdef NDEBUG
if (!DLE.IsD2XLevel ()) 
	return false;
#endif

current->Get (nSegment, nSide);

	CSegment*	pSegment = Segment (nSegment);
	CSide*		pSide = pSegment->Side (nSide);
	ushort		nEdgeVerts [2]; // ids of the edge's vertices
	ubyte			nSegVerts [2]; // indices of the edge's vertices' ids in the segment's vertex id table
	short			nSideVerts [2]; // indices of the edge's vertices' ids in the side's vertex id index 

if (pSide->VertexCount() < 2)
	return false;
if (nEdge < 0)
	nEdge = current->Edge ();

for (int i = 0; i < 2; i++) {
	nSegVerts [i] = pSide->m_vertexIdIndex [(nEdge + i) % pSide->VertexCount ()];
	nEdgeVerts [i] = pSegment->m_info.vertexIds [nSegVerts [i]]; 
	}

if (nSegVerts [0] == nSegVerts [1])
	return false;

short nSides [2] = {nSide, pSegment->AdjacentSide (nSide, nEdgeVerts)};
if ((pSegment->Side (nSides [0])->VertexCount () < 4) && (pSegment->Side (nSides [1])->VertexCount () < 4)) {
	if (!DLE.ExpertMode ())
		ErrorMsg ("Cannot further collapse this segment."); 
	return false;
	}

undoManager.Begin (__FUNCTION__, udSegments | udVertices | udWalls);

if ((0 > (nEdgeVerts [0] = SeparatePoints (CSideKey (nSegment, nSide), nEdgeVerts [0], false))) ||
	 (0 > (nEdgeVerts [1] = SeparatePoints (CSideKey (nSegment, nSide), nEdgeVerts [1], false)))) {
	undoManager.Unroll (__FUNCTION__);
	return false;
	}

if ((pSide->VertexCount () == 3) && pSide->Wall ())
	pSide->DeleteWall ();

if (short (pSide->m_info.nChild) >= 0)
	segmentManager.Segment (pSide->m_info.nChild)->ReplaceChild (nSegment, -1);
// compute the new vertex
if (bUpdateCoord) {
	*vertexManager.Vertex (nEdgeVerts [0]) += *vertexManager.Vertex (nEdgeVerts [1]);
	*vertexManager.Vertex (nEdgeVerts [0]) /= 2;
	}

// remove the superfluous vertex from all sides adjacent to the edge and recompute the sides' texture coordinates
for (short h = 0; h < 2; h++) {
	short nSide = nSides [h];
	if (nSide < 0)
		continue;
	pSide = pSegment->Side (nSide);
	for (int i = 0; i < 2; i++)
		nSideVerts [i] = pSegment->SideVertexIndex (nSide, nSegVerts [i]);
#if 0
	if (nSideVerts [0] > nSideVerts [1])
		Swap (nSideVerts [0], nSideVerts [1]);
#endif
	if (nSideVerts [0] < 0)
		continue;
	CUVL uvl = pSide->m_info.uvls [nSideVerts [0]];
	uvl.u = (uvl.u + pSide->m_info.uvls [nSideVerts [1]].u) / 2;
	uvl.v = (uvl.v + pSide->m_info.uvls [nSideVerts [1]].v) / 2;
	uvl.l = (uvl.l + pSide->m_info.uvls [nSideVerts [1]].l) / 2;
	pSide->m_info.uvls [nSideVerts [0]] = uvl;
	pSide->SetShape (pSide->Shape () + 1);
	short nVertsToMove = pSide->VertexCount () - nSideVerts [1];
	if (nVertsToMove > 0) {
		memcpy (pSide->m_vertexIdIndex + nSideVerts [1], pSide->m_vertexIdIndex + nSideVerts [1] + 1, nVertsToMove);
		memcpy (pSide->m_info.uvls + nSideVerts [1], pSide->m_info.uvls + nSideVerts [1] + 1, nVertsToMove * sizeof (CUVL));
		}
	pSide->m_vertexIdIndex [pSide->VertexCount ()] = 0xff; //pSide->m_vertexIdIndex [nSideVerts [0]];
	}
pSide = pSegment->Side (0);
for (short nSide = 0; nSide < 6; nSide++, pSide++) {
	short nVertexCount = pSide->VertexCount ();
	//if ((nSide == nSides [0]) || (nSide == nSides [1]))
	//	continue;
	int j = 0;
	for (int i = 0; i < nVertexCount; i++) {
		if (pSide->m_vertexIdIndex [i] == nSegVerts [1]) 
			pSide->m_vertexIdIndex [i] = nSegVerts [0];
		if (pSide->m_vertexIdIndex [i] == nSegVerts [0]) {
			if (++j > 1) {
				if (i < --nVertexCount)
					memcpy (pSide->m_vertexIdIndex + i, pSide->m_vertexIdIndex + i + 1, nVertexCount - i);
				pSide->m_vertexIdIndex [nVertexCount] = 0xff;
				pSide->SetShape (pSide->Shape () + 1);
				break;
				}
			}
		}
	}

#if 1
pSegment->m_info.vertexIds [nSegVerts [1]] = 0xffff - nSegVerts [0]; // remember the vertex id index that has replaced this one => MAX_VERTICES must be <= 0xfff8!
#else
pSegment->UpdateVertexIdIndex (nSegVerts [1]);
if (nSegVerts [1] < 7)
	memcpy (pSegment->m_info.vertexIds + nSegVerts [1], pSegment->m_info.vertexIds + nSegVerts [1] + 1, (7 - nSegVerts [1]) * sizeof (pSegment->m_info.vertexIds [0]));
pSegment->m_info.vertexIds [7] = 0xffff;
#endif

if (bUpdateCoord)
	vertexManager.Delete (nEdgeVerts [1]);
undoManager.End (__FUNCTION__);
return true;
}

// ------------------------------------------------------------------------

bool CSegmentManager::CreateWedge (void)
{
#ifdef NDEBUG
if (!DLE.IsD2XLevel ()) 
	return false;
#endif

	CSegment* pSegment = current->Segment ();

if (pSegment->Shape () != SEGMENT_SHAPE_CUBE) {
	ErrorMsg ("Cannot turn this segment in a wedge.\nTry to collapse individual edges instead."); 
	return false;
	}

	CSide* pSide = current->Side ();
	short nSide = current->SideId (), nOppSide = oppSideTable [current->SideId ()];

undoManager.Begin (__FUNCTION__, udSegments | udVertices | udWalls);

short nLine = current->Edge () + 1;
if (!CollapseEdge (-1, -1, nLine % current->Side ()->VertexCount ())) {
	undoManager.Unroll (__FUNCTION__);
	return false;
	}
if (nLine < current->Side ()->VertexCount ())
	++nLine;
if (!CollapseEdge (-1, -1, nLine % current->Side ()->VertexCount ())) {
	undoManager.Unroll (__FUNCTION__);
	return false;
	}
pSegment->SetShape (SEGMENT_SHAPE_WEDGE);
DLE.MineView ()->NextSide (1);
undoManager.End (__FUNCTION__);
return true;
}

// ------------------------------------------------------------------------

bool CSegmentManager::CreatePyramid (void)
{
#ifdef NDEBUG
if (!DLE.IsD2XLevel ()) 
	return false;
#endif

	CSegment* pSegment = current->Segment ();

if (pSegment->Shape () != SEGMENT_SHAPE_CUBE) {
	ErrorMsg ("Cannot turn this segment in a pyramid.\nTry to collapse individual edges instead."); 
	return false;
	}

	CSide* pSide = current->Side ();
	short nSide = current->SideId (), nOppSide = oppSideTable [current->SideId ()];

undoManager.Begin (__FUNCTION__, udSegments | udVertices | udWalls);

#if 0
for (int i = 0; i < 6; i++) {
	if (i == nOppSide)
		continue;
	if (pSegment->Child (i) < 0)
		continue;
	if (!SeparateSegments (i)) {
		undoManager.Unlock ();
		undoManager.Undo ();
		return false;
		}
	}
#endif

short nLine = current->Edge () + 1;
if (!CollapseEdge (-1, -1, nLine % current->Side ()->VertexCount ())) {
	undoManager.Unroll (__FUNCTION__);
	return false;
	}
if (nLine < current->Side ()->VertexCount ())
	++nLine;
if (!(CollapseEdge (-1, -1, nLine % current->Side ()->VertexCount ()) && CollapseEdge (-1, -1, 0))) {
	undoManager.Unroll (__FUNCTION__);
	return false;
	}
pSegment->SetShape (SEGMENT_SHAPE_PYRAMID);
DLE.MineView ()->NextSide (1);
undoManager.End (__FUNCTION__);
return true;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//eof segmentmanager.cpp
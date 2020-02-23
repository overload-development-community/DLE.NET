// Segment.cpp

#include "mine.h"
#include "dle-xp.h"
#include "AVLTree.h"

CSegmentManager segmentManager;

// ------------------------------------------------------------------------- 

CVertex& CSegmentManager::CalcCenter (CVertex& pos, short nSegment) 
{
pos = Segment (nSegment)->ComputeCenter (false);
return pos;
}

// -----------------------------------------------------------------------------

CDoubleVector CSegmentManager::CalcSideCenter (CSideKey key)
{
current->Get (key);

	CSegment _const_ * pSegment = Segment (key.m_nSegment);
	CSide _const_ * pSide = pSegment->Side (key.m_nSide);
	ushort* vertexIds = pSegment->m_info.vertexIds;
	ubyte* vertexIdIndex = pSide->m_vertexIdIndex;
	int n = pSide->VertexCount ();

CDoubleVector v = vertexManager [vertexIds [*vertexIdIndex]];
for (int i = 1; i < n; i++)
	v += vertexManager [vertexIds [*++vertexIdIndex]];
v /= double (n);
return v;
}

// -----------------------------------------------------------------------------

CDoubleVector CSegmentManager::CalcSideNormal (CSideKey key)
{
current->Get (key);

	CSegment _const_ * pSegment = Segment (key.m_nSegment);
	int n = pSegment->Side (key.m_nSide)->VertexCount ();

if (n < 3)
	return CDoubleVector (0.0, 0.0, 0.0);
return -Normal (*(pSegment->Vertex (key.m_nSide, 0)), *(pSegment->Vertex (key.m_nSide, 1)), *(pSegment->Vertex (key.m_nSide, n - 1)));
}

// ------------------------------------------------------------------------------ 
// get_opposing_side()
//
// Action - figures out childs nSegment and side for a given side
// Returns - TRUE on success
// ------------------------------------------------------------------------------ 

CSide _const_ * CSegmentManager::BackSide (CSideKey key, CSideKey& back)
{
current->Get (key); 
#ifdef _DEBUG
if (key.m_nSegment < 0 || key.m_nSegment >= Count ())
	return null; 
if (key.m_nSide < 0 || key.m_nSide >= 6)
	return null; 
#endif
short nChildSeg = Segment (key.m_nSegment)->ChildId (key.m_nSide); 
if (nChildSeg < 0 || nChildSeg >= Count ())
	return null; 
CSegment* pChildSeg = Segment (nChildSeg);
for (short nChildSide = 0; nChildSide < 6; nChildSide++) {
	if (Segment (nChildSeg)->ChildId (nChildSide) == key.m_nSegment) {
		back.m_nSegment = nChildSeg; 
		back.m_nSide = nChildSide; 
		return Side (back); 
		}
	}
return null; 
}

// -----------------------------------------------------------------------------

bool CSegmentManager::IsExit (short nSegment)
{
	CSideKey key (nSegment, 0);
	CSegment* pSegment = segmentManager.Segment (nSegment);

for (key.m_nSide = 0; key.m_nSide < 6; key.m_nSide++) {
	CWall* pWall = Wall (key);
	if (pWall) {
		int i = pWall->Index ();
		if (wallManager.IsExit (i))
			return true;
		i = wallManager.Index (wallManager.OppositeWall (*wallManager.Wall (i)));
		if (wallManager.IsExit (i))
			return true;
		}
	}
return false;
}

// -----------------------------------------------------------------------------

int CSegmentManager::IsWall (CSideKey key)
{
current->Get (key); 
return (Segment (key.m_nSegment)->ChildId (key.m_nSide) == -1) || (Wall (key) != null);
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::DeleteWalls (short nSegment)
{
	CSide _const_ * pSide = Segment (nSegment)->m_sides; 

for (int i = MAX_SIDES_PER_SEGMENT; i; i--, pSide++)
	wallManager.Delete (pSide->m_info.nWall);
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::UpdateVertices (short nOldVert, short nNewVert)
{
CSegment _const_ * pSegment = Segment (0);
int nSegments = segmentManager.Count ();
for (int nSegment = 0; nSegment < nSegments; nSegment++, pSegment++) {
	ushort* vertexIds = pSegment->m_info.vertexIds;
	for (ushort nVertex = 0; nVertex < 8; nVertex++) {
		if (vertexIds [nVertex] == nOldVert)
			vertexIds [nVertex] = nNewVert;
		}
	}
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::UpdateWalls (short nOldWall, short nNewWall)
{
if (nOldWall != nNewWall) {
	CSegment _const_ * pSegment = Segment (0);
	for (int i = Count (); i; i--, pSegment++) {
		CSide* pSide = &pSegment->m_sides [0];
		for (int j = 0; j < 6; j++, pSide++)
			if ((pSide->m_info.nWall != NO_WALL) && (pSide->m_info.nWall >= nOldWall))
				pSide->m_info.nWall = nNewWall;
		}
	}
}

// -----------------------------------------------------------------------------
// ResetSide()
//
// Action - sets side to have no child and a default texture
// ----------------------------------------------------------------------------- 

void CSegmentManager::ResetSide (short nSegment, short nSide)
{
if (nSegment < 0 || nSegment >= Count ()) 
	return; 
undoManager.Begin (__FUNCTION__, udSegments);
const_cast<CSegment*>(Segment (nSegment))->Reset (nSide); 
undoManager.End (__FUNCTION__);
}

// ----------------------------------------------------------------------------- 
//			  set_lines_to_draw()
//
//  ACTION - Determines which lines will be shown when drawing 3d image of
//           the theMine->  This helps speed up drawing by avoiding drawing lines
//           multiple times.
//
// ----------------------------------------------------------------------------- 

void CSegmentManager::SetLinesToDraw (void)
{
  CSegment *pSegment; 
  short nSegment, nSide; 

for (nSegment = Count (), pSegment = Segment (0); nSegment; nSegment--, pSegment++) {
	pSegment->m_info.mapBitmask |= 0xFFF; 
	// if segment nSide has a child, clear bit for drawing line
	for (nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
		if (pSegment->ChildId (nSide) > -1) { // -1 = no child,  - 2 = outside of world
			pSegment->m_info.mapBitmask &= ~(1 << (sideEdgeTable [nSide][0])); 
			pSegment->m_info.mapBitmask &= ~(1 << (sideEdgeTable [nSide][1])); 
			pSegment->m_info.mapBitmask &= ~(1 << (sideEdgeTable [nSide][2])); 
			pSegment->m_info.mapBitmask &= ~(1 << (sideEdgeTable [nSide][3])); 
			}
		}
	}
}

// ----------------------------------------------------------------------------- 

void CSegmentManager::CopyOtherSegment (void)
{
	bool bChange = false;

if (selections [0].m_nSegment == selections [1].m_nSegment)
	return; 
short nSegment = current->SegmentId (); 
CSegment* pSegment = current->Segment ();
CSegment* pOtherSeg = other->Segment (); 
undoManager.Begin (__FUNCTION__, udSegments);
for (int nSide = 0; nSide < 6; nSide++)
	if (SetTextures (CSideKey (nSegment, nSide), pOtherSeg->m_sides [nSide].BaseTex (), pOtherSeg->m_sides [nSide].OvlTex ()))
		bChange = true;
undoManager.End (__FUNCTION__);
if (bChange)
	DLE.MineView ()->Refresh (); 
}

// -----------------------------------------------------------------------------

int CSegmentManager::Fix (void)
{
int errFlags = 0;

int nSegments = segmentManager.Count ();
for (int si = 0; si < nSegments; si++) {
	CSegment* pSegment = segmentManager.Segment (si);
	for (short nSide = 0; nSide < MAX_SIDES_PER_SEGMENT; nSide++) {
		CSide& side = pSegment->m_sides [nSide];
		if ((side.m_info.nWall != NO_WALL) && ((side.m_info.nWall >= wallManager.Count () || !side.Wall ()->Used ()))) {
			side.m_info.nWall = NO_WALL;
			errFlags |= 1;
			}
		if ((pSegment->ChildId (nSide) < -2) || (pSegment->ChildId (nSide) > Count ())) {
			pSegment->SetChild (nSide, -1);
			errFlags |= 2;
			}
		}
	for (ushort nVertex = 0; nVertex < MAX_VERTICES_PER_SEGMENT; nVertex++) {
		if ((pSegment->m_info.vertexIds [nVertex] <= MAX_VERTEX) && (pSegment->m_info.vertexIds [nVertex] >= vertexManager.Count ())) {
			pSegment->m_info.vertexIds [nVertex] = 0;  // this will cause a bad looking picture
			errFlags |= 4;
			}
		}
	}
return errFlags;
}

// -----------------------------------------------------------------------------

void CSegmentManager::DeleteD2X (void)
{
while (Count () > MAX_SEGMENTS_D2)
	Delete (Count () - 1);
for (int i = 0; i < Count (); i++) {
	if (m_segments [i].IsD2X ()) {
		Undefine (i);
		m_segments [i].m_info.props = 0;
		}
	}
}

// ----------------------------------------------------------------------------- 

int CSegmentManager::Overflow (int nSegments) 
{ 
if (nSegments < 0)
	nSegments = Count ();
return DLE.IsD1File () 
		 ? (nSegments > MAX_SEGMENTS_D1) 
		 : DLE.IsStdLevel () 
			? (nSegments > MAX_SEGMENTS_D2) 
			: (nSegments > SEGMENT_LIMIT)
				? -1
				: 0;
}

// -----------------------------------------------------------------------------

short CSegmentManager::FindByVertex (ushort nVertex, short nSegment)
{
CSegment* pSegment = Segment (nSegment);
for (short i = Count (); nSegment < i; nSegment++, pSegment++)
	if (pSegment->HasVertex (nVertex))
		return nSegment;
return -1;
}

// -----------------------------------------------------------------------------

bool CSegmentManager::VertexInUse (ushort nVertex, short nIgnoreSegment)
{
if (nIgnoreSegment < 0)
	nIgnoreSegment = current->SegmentId ();
for (short nSegment = 0; 0 <= (nSegment = FindByVertex (nVertex, nSegment)); nSegment++)
	if (nSegment != nIgnoreSegment)
		return true;
return false;
}

//------------------------------------------------------------------------------

void CSegmentManager::ComputeNormals (bool bAll, bool bView)
{
	short nSegments = segmentManager.Count ();

#ifdef NDEBUG
#	pragma omp parallel for if (nSegments > 15)
#endif
for (short nSegment = 0; nSegment < nSegments; nSegment++) {
	CSegment* pSegment = Segment (nSegment);
	pSegment->ComputeCenter (bView);
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
			continue;
		if (!(bAll || pSide->IsVisible ()))
			continue;
		pSide->ComputeNormals (pSegment->m_info.vertexIds, pSegment->m_vCenter, bView);
		}
	}
}

//------------------------------------------------------------------------------

CSegment* CSegmentManager::GatherSelectableSegments (CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox)
{
	short nSegments = segmentManager.Count ();

m_selectedSegments = null;
	
#pragma omp parallel for if (nSegments > 15)
for (short nSegment = 0; nSegment < nSegments; nSegment++) {
	CSegment* pSegment = Segment (nSegment);
	if ((pSegment->Function () == SEGMENT_FUNC_SKYBOX) && !bAllowSkyBox)
		continue;
	if (pSegment->IsSelected (viewport, xMouse, yMouse, 0, true))
#pragma omp critical
		{
		pSegment->SetLink (m_selectedSegments);
		m_selectedSegments = pSegment;
		}
	}
return m_selectedSegments;
}

// -----------------------------------------------------------------------------

CSide* CSegmentManager::GatherSelectableSides (CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox, bool bSegments)
{
	short nSegments = segmentManager.Count ();

m_selectedSegments = null;
m_selectedSides = null;
	
#ifdef NDEBUG
#	pragma omp parallel for if (nSegments > 15)
#endif
for (short nSegment = 0; nSegment < nSegments; nSegment++) {
	CSegment* pSegment = Segment (nSegment);
	if ((pSegment->Function () == SEGMENT_FUNC_SKYBOX) && !bAllowSkyBox)
		continue;
	if (pSegment->m_info.bTunnel)
		continue;
	bool bSegmentSelected = false;
	short nSide = 0;
	for (; nSide < 6; nSide++) {
		nSide = pSegment->IsSelected (viewport, xMouse, yMouse, nSide, bSegments);
		if (nSide < 0)
			break;
#ifdef NDEBUG
#	pragma omp critical
#endif
			{
			if (!bSegmentSelected) {
				bSegmentSelected = true;
				pSegment->SetLink (m_selectedSegments);
				m_selectedSegments = pSegment;
				}
			}
			CSide* pSide = pSegment->Side (nSide);
#ifdef NDEBUG
#	pragma omp critical
#endif
			{
			pSide->SetLink (m_selectedSides);
			m_selectedSides = pSide;
			}
		pSide->SetParent (nSegment);
		}
	}
return m_selectedSides;
}

// -----------------------------------------------------------------------------
// Collect all edges that belong to at least one "visible" side (i.e. a side
// with no connected segment or a visible, textured wall) and create a list 
// of all visible sides that are connect to each such edge. The edge key is 
// created out of the ids of the two vertices defining the edge: smaller vertex
// id in low word, greater vertex id in high word of a uint. 

void CSegmentManager::GatherEdges (CAVLTree <CEdgeTreeNode, uint>& edgeTree)
{
	CEdgeList	edgeList;
	CSegment*	pSegment = Segment (0);
	short			nSegments = Count ();

for (short nSegment = 0; nSegment < nSegments; nSegment++, pSegment++) {
	int nEdges = pSegment->BuildEdgeList (edgeList, false);
	for (int nEdge = 0; nEdge < nEdges; nEdge++) {
		ubyte side1, side2, i1, i2;
		edgeList.Get (nEdge, side1, side2, i1, i2);
		ushort v1 = pSegment->VertexId (i1);
		ushort v2 = pSegment->VertexId (i2);
		uint key = CEdgeKey::Key (v1, v2);
		bool bVisible [2] = { (side1 < 6) && pSegment->Side (side1)->IsVisible (), (side2 < 6) && pSegment->Side (side2)->IsVisible () };
		if ((bVisible [0] || bVisible [1])) {
			CEdgeTreeNode node (key);
			CEdgeTreeNode*pNode = edgeTree.Insert (node, key);
			if (pNode) { // insert sides into side list of current edge
				if (bVisible [0])
					pNode->Insert (nSegment, side1);
				if (bVisible [1])
					pNode->Insert (nSegment, side2);
				}
			}
		}
	}
}

// -----------------------------------------------------------------------------

uint CSegmentManager::VisibleSideCount (void)
{
	CSegment*	pSegment = Segment (0);
	short			nSegments = Count ();
	uint			nSides = 0;

for (short nSegment = 0; nSegment < nSegments; nSegment++, pSegment++)
	for (short nSide = 0; nSide < 6; nSide++)
		if (pSegment->Side (nSide)->IsVisible ())
			nSides++;
return nSides;
}

// -----------------------------------------------------------------------------

void CSegmentManager::MakePointsParallel (void)
{
undoManager.Begin (__FUNCTION__, udVertices);
if (!vertexManager.HasTaggedVertices ())
	current->Segment ()->MakeCoplanar (current->SideId ());
else {
	double dist;

	current->Segment ()->ComputeNormals (current->SideId ());
	CDoubleVector n = current->Side ()->Normal ();
	CDoubleVector v0 = *current->Vertex ();

	if (current->Segment ()->HasTaggedVertices (TAGGED_MASK, current->SideId ()))
		dist = 0.0;
	else {
		double d = 0.0;
		ushort nTagged = 0;
		for (ushort i = 0, j = vertexManager.Count (); i < j; i++) {
			CVertex& v = vertexManager [i];
			if (v.IsTagged ()) {
				++nTagged;
				CDoubleVector h = v - v0;
				d += Dot (h, n);
				}
			}
		dist = d / double (nTagged);
		}

	for (ushort i = 0, j = vertexManager.Count (); i < j; i++) {
		CVertex& v = vertexManager [i];
		if (v.IsTagged ()) {
			CDoubleVector h = v - v0;
			v -= n * (Dot (h, n) - dist);
			}
		}
	}
undoManager.End (__FUNCTION__);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//eof segmentmanager.cpp
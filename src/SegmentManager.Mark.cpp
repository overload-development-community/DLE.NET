// Segment.cpp

#include "mine.h"
#include "dle-xp.h"

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;
uint nDbgEdgeKey = 0xffffffff;

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsTagged (CSideKey key)
{
return Segment (key.m_nSegment)->IsTagged (key.m_nSide);
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsTagged (short nSegment)
{
#if 1
return Segment (nSegment)->IsTagged ();
#else
CSegment *pSegment = Segment (nSegment);
for (int i = 0;  i < 8; i++)
	if ((pSegment->VertexId (i) <= MAX_VERTEX) && !pSegment->Vertex (i)->IsTagged ())
		return false;
return true;
#endif
}

// ----------------------------------------------------------------------------- 
//			 mark_segment()
//
//  ACTION - Toggle marked bit of segment and tag/untag vertices.
//
// ----------------------------------------------------------------------------- 

void CSegmentManager::Tag (short nSegment)
{
  CSegment *pSegment = Segment (nSegment); 

pSegment->Tag ();
for (short nSide = 0; nSide < 6; nSide++)
	pSegment->Side (nSide)->Tag ();
for (ushort nVertex = 0; nVertex < 8; nVertex++)
	pSegment->Vertex (nVertex)->Status () |= TAGGED_MASK; 
}

// -----------------------------------------------------------------------------

void CSegmentManager::UpdateTagged (void)
{
// mark all cubes which have all 8 verts marked
CSegment* pSegment = Segment (0);
short nSegments = Count ();

uint selectMode = DLE.MineView ()->GetSelectMode ();

if ((selectMode != eSelectSide) && (selectMode != eSelectSegment))
	return;

for (short i = 0; i < nSegments; i++, pSegment++) {
	short j = 0;
	if (selectMode == eSelectSegment) {
		ushort* vertexIds = pSegment->VertexIds ();
		for (; j < 8; j++) {
			if ((vertexIds [j] <= MAX_VERTEX) && !vertexManager [vertexIds [j]].IsTagged ())
				break;
			}
		if (j < 8)
			pSegment->UnTag (); 
		else
			pSegment->Tag (); 
		}
	for (short nSide = 0; nSide < 6; nSide++) {
		short h = pSegment->Side (nSide)->VertexCount ();
		j = 0;
		if (h > 2) {
			for (; j < h; j++)
				if (!pSegment->Vertex (nSide, j)->IsTagged ())
					break;
			}
		if (j < h)
			pSegment->UnTag (nSide);
		else
			pSegment->Tag (nSide);
		}
	}
}

// -----------------------------------------------------------------------------

void CSegmentManager::TagAll (ubyte mask) 
{
CSegment* pSegment = Segment (0);
short nSegments = Count ();
for (short i = 0; i < nSegments; i++, pSegment++) {
	pSegment->Tag (mask); 
	for (short j = 0; j < 8; j++)
		if (pSegment->VertexId (j) <= MAX_VERTEX)
			pSegment->Vertex (j)->Status () |= mask;
	}
DLE.MineView ()->Refresh (); 
}

// -----------------------------------------------------------------------------

void CSegmentManager::UnTagAll (ubyte mask) 
{
CSegment* pSegment = Segment (0);
short nSegments = Count ();
for (short i = 0; i < nSegments; i++, pSegment++) {
	pSegment->UnTag (mask); 
	for (short j = 0; j < 8; j++)
		if (pSegment->VertexId (j) <= MAX_VERTEX)
			pSegment->Vertex (j)->Status () &= ~mask;
	}
DLE.MineView ()->Refresh (); 
}

// ------------------------------------------------------------------------ 

bool CSegmentManager::HasTaggedSides (void)
{
int nSegments = segmentManager.Count ();
for (int nSegment = 0; nSegment < nSegments; nSegment++) {
	CSide* pSide = segmentManager.Segment (nSegment)->m_sides;
	for (short nSide = 0; nSide < 6; nSide++)
		if (IsTagged (CSideKey (nSegment, nSide)))
			return true;
	}
return false; 
}

// ------------------------------------------------------------------------ 

short CSegmentManager::TaggedCount (bool bSides, bool bCheck)
{
int nCount = 0; 
CSegment* pSegment = Segment (0);
short nSegments = Count ();
for (short i = 0; i < nSegments; i++, pSegment++) {
	if (pSegment->IsTagged () || (bSides && pSegment->IsTagged (short (-1)))) {
		if (bCheck)
			return 1; 
		++nCount; 
		}
	}
return nCount; 
}

// ------------------------------------------------------------------------ 

ushort CSegmentManager::TaggedSideCount (void)
{
int nCount = 0; 
CSegment* pSegment = Segment (0);
short nSegments = Count ();
for (short i = 0; i < nSegments; i++, pSegment++) {
	CSide* pSide = pSegment->Side (0);
	for (short j = 0; j < 6; j++, pSide++)
		if ((pSide->Shape () <= SIDE_SHAPE_TRIANGLE) && pSide->IsTagged ())
			++nCount; 
	}
return nCount; 
}

// ------------------------------------------------------------------------

void CSegmentManager::TagSelected (void)
{
	bool	bSegTag = false; 
	CSegment *pSegment = current->Segment (); 
	int i, j, p [8], nPoints; 
	uint selectMode = DLE.MineView ()->GetSelectMode ();

switch (selectMode) {
	case eSelectPoint:
		nPoints = 1; 
		p [0] = current->VertexId (); 
		break; 

	case eSelectLine:
		nPoints = 2; 
		p [0] = pSegment->VertexId (current->Side ()->VertexIdIndex (current->Point ())); 
		p [1] = pSegment->VertexId (current->Side ()->VertexIdIndex (current->Point () + 1)); 
		break; 

	case eSelectSide:
		nPoints = current->Side ()->VertexCount (); 
		for (i = 0; i < nPoints; i++)
			p [i] = pSegment->VertexId (current->Side ()->VertexIdIndex (i)); 
		break; 

	case eSelectSegment:
	default:
		nPoints = 8; 
		for (i = 0, j = 0; i < nPoints; i++) {
			p [j] = pSegment->VertexId (i); 
			if (p [j] <= MAX_VERTEX)
				j++;
			}
		nPoints = j;
		break; 
	}

// set i to nPoints if all verts are marked
for (i = 0; i < nPoints; i++)
	if (!vertexManager [p [i]].IsTagged ()) 
		break; 

if (i == nPoints) { // if all verts are tagged, then untag them
	switch (selectMode) {
		case eSelectPoint:
		case eSelectLine:
			for (i = 0; i < nPoints; i++)
				vertexManager [p [i]].UnTag (); 
			break;

		case eSelectSide:
			current->Segment ()->UnTag (current->SideId ());
			current->Segment ()->UnTagVertices (TAGGED_MASK, current->SideId ());
			break; 

		case eSelectSegment:
		default:
			current->Segment ()->UnTag ();
			current->Segment ()->UnTagVertices (TAGGED_MASK, -1);
			break; 
		}
	}
else { // otherwise tag all the points
	switch (selectMode) {
		case eSelectPoint:
		case eSelectLine:
			for (i = 0; i < nPoints; i++)
				vertexManager [p [i]].Tag (); 
			break;

		case eSelectSide:
			current->Segment ()->Tag (current->SideId ());
			current->Segment ()->TagVertices (TAGGED_MASK, current->SideId ());
			break; 

		case eSelectSegment:
		default:
			current->Segment ()->Tag ();
			current->Segment ()->TagVertices (TAGGED_MASK, -1);
			break; 
		}
	}
DLE.MineView ()->Refresh (); 
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// TODO: Beginning with the current side, walk through all adjacent sides and 
// mark all in plane

int CTaggingStrategy::Run (short nSegment, short nSide) 
{
segmentManager.UnTagAll (m_tag);

int nHead = 0;
int nTail = 1;

if (nSegment < 0)
	nSegment = current->SegmentId ();
if (nSide < 0)
	nSide = current->SideId ();
m_sideList [nHead].m_parent = CSideKey (-1, -1);
m_sideList [nHead].m_child = CSideKey (nSegment, nSide);
segmentManager.Segment (nSegment)->Tag (nSide, m_tag);

segmentManager.GatherEdges (m_edgeTree);

segmentManager.ComputeNormals (false);

CEdgeList edgeList;

bool bTagVertices = DLE.MineView ()->SelectMode (eSelectPoint);

while (nHead < nTail) {
	m_parent = m_sideList [nHead++].m_child;
	m_pSegment = segmentManager.Segment (m_parent);
	m_pSide = segmentManager.Side (m_parent);
	edgeList.Reset ();
#ifdef _DEBUG
	if ((m_child.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (m_child.m_nSide == nDbgSide)))
		nDbgSeg = nDbgSeg;
#endif
	if (!m_pSegment->BuildEdgeList (edgeList, ubyte (m_parent.m_nSide), false))
		continue;
	int nEdges = edgeList.Count ();
	for (int nEdge = 0; nEdge < nEdges; nEdge++) {
		ubyte side1, side2, i1, i2;
		edgeList.Get (nEdge, side1, side2, i1, i2);
#if 0 // the following case cannot happen, or there wouldn't be such an edge list entry at all
		if (side1 > 5) {
			side1 = side2;
			if (side1 > 5)
				continue;
			}
#endif
		ushort v1 = m_pSegment->VertexId (i1);
		ushort v2 = m_pSegment->VertexId (i2);
		CEdgeTreeNode* node = m_edgeTree.Find (EdgeKey (v1, v2));
		if (!node)
			continue;
		CSLLIterator<CSideKey, CSideKey> iter (node->m_sides);
		for (iter.Begin (); *iter != iter.End (); iter++) {
			m_child = **iter;
			m_pChildSeg = segmentManager.Segment (m_child);
#ifdef _DEBUG
			if ((m_child.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (m_child.m_nSide == nDbgSide)))
				nDbgSeg = nDbgSeg;
#endif
			if (!m_pChildSeg->IsTagged (iter->m_nSide, m_tag)) {
#ifdef _DEBUG
				if ((m_child.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (m_child.m_nSide == nDbgSide)))
					nDbgSeg = nDbgSeg;
#endif
				m_pChildSide = segmentManager.Side (m_child);
				if (Accept ()) {
#ifdef _DEBUG
					if ((m_child.m_nSegment == nDbgSeg) && ((nDbgSide < 0) || (m_child.m_nSide == nDbgSide)))
						nDbgSeg = nDbgSeg;
#endif
					m_pChildSeg->Tag (iter->m_nSide, m_tag);
					if (bTagVertices)
						m_pChildSeg->TagVertices (m_tag, iter->m_nSide);
					m_sideList [nTail].m_edge = m_edgeKey;
					m_sideList [nTail].m_parent = m_parent;
					m_sideList [nTail++].m_child = m_child;
					}
				}
			}
		}
	}
return nTail;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//eof segmentmanager.cpp
// Segment.cpp

#include "stdafx.h"
#include "VertexManager.h"

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsTagged(CSideKey key)
{
    return Segment(key.m_nSegment)->IsTagged(key.m_nSide);
}

// ----------------------------------------------------------------------------- 

bool CSegmentManager::IsTagged(short nSegment)
{
#if 1
    return Segment(nSegment)->IsTagged();
#else
    CSegment* pSegment = Segment(nSegment);
    for (int i = 0; i < 8; i++)
        if ((pSegment->VertexId(i) <= MAX_VERTEX) && !pSegment->Vertex(i)->IsTagged())
            return false;
    return true;
#endif
}

// ----------------------------------------------------------------------------- 
//        mark_segment()
//
//  ACTION - Toggle marked bit of segment and tag/untag vertices.
//
// ----------------------------------------------------------------------------- 

void CSegmentManager::Tag(short nSegment)
{
    CSegment* pSegment = Segment(nSegment);

    pSegment->Tag();
    for (short nSide = 0; nSide < 6; nSide++)
        pSegment->Side(nSide)->Tag();
    for (ushort nVertex = 0; nVertex < 8; nVertex++)
        pSegment->Vertex(nVertex)->Status() |= TAGGED_MASK;
}

// -----------------------------------------------------------------------------

void CSegmentManager::UpdateTagged(void)
{
    // mark all cubes which have all 8 verts marked
    CSegment* pSegment = Segment(0);
    short nSegments = Count();

    for (short i = 0; i < nSegments; i++, pSegment++) {
        short j = 0;
        ushort* vertexIds = pSegment->VertexIds();
        for (; j < 8; j++) {
            if ((vertexIds[j] <= MAX_VERTEX) && !vertexManager[vertexIds[j]].IsTagged())
                break;
        }
        if (j < 8)
            pSegment->UnTag();
        else
            pSegment->Tag();

        for (short nSide = 0; nSide < 6; nSide++) {
            short h = pSegment->Side(nSide)->VertexCount();
            j = 0;
            if (h > 2) {
                for (; j < h; j++)
                    if (!pSegment->Vertex(nSide, j)->IsTagged())
                        break;
            }
            if (j < h)
                pSegment->UnTag(nSide);
            else
                pSegment->Tag(nSide);
        }
    }
}

// -----------------------------------------------------------------------------

void CSegmentManager::TagAll(ubyte mask)
{
    CSegment* pSegment = Segment(0);
    short nSegments = Count();
    for (short i = 0; i < nSegments; i++, pSegment++) {
        pSegment->Tag(mask);
        for (short j = 0; j < 8; j++)
            if (pSegment->VertexId(j) <= MAX_VERTEX)
                pSegment->Vertex(j)->Status() |= mask;
    }
    g_data.RefreshMineView();
}

// -----------------------------------------------------------------------------

void CSegmentManager::UnTagAll(ubyte mask)
{
    CSegment* pSegment = Segment(0);
    short nSegments = Count();
    for (short i = 0; i < nSegments; i++, pSegment++) {
        pSegment->UnTag(mask);
        for (short j = 0; j < 8; j++)
            if (pSegment->VertexId(j) <= MAX_VERTEX)
                pSegment->Vertex(j)->Status() &= ~mask;
    }
    g_data.RefreshMineView();
}

// ------------------------------------------------------------------------ 

bool CSegmentManager::HasTaggedSides(void)
{
    int nSegments = segmentManager.Count();
    for (int nSegment = 0; nSegment < nSegments; nSegment++) {
        CSide* pSide = segmentManager.Segment(nSegment)->m_sides;
        for (short nSide = 0; nSide < 6; nSide++)
            if (IsTagged(CSideKey(nSegment, nSide)))
                return true;
    }
    return false;
}

// ------------------------------------------------------------------------ 

short CSegmentManager::TaggedCount(bool bSides, bool bCheck)
{
    int nCount = 0;
    CSegment* pSegment = Segment(0);
    short nSegments = Count();
    for (short i = 0; i < nSegments; i++, pSegment++) {
        if (pSegment->IsTagged() || (bSides && pSegment->IsTagged(short(-1)))) {
            if (bCheck)
                return 1;
            ++nCount;
        }
    }
    return nCount;
}

// ------------------------------------------------------------------------ 

ushort CSegmentManager::TaggedSideCount(void)
{
    int nCount = 0;
    CSegment* pSegment = Segment(0);
    short nSegments = Count();
    for (short i = 0; i < nSegments; i++, pSegment++) {
        CSide* pSide = pSegment->Side(0);
        for (short j = 0; j < 6; j++, pSide++)
            if ((pSide->Shape() <= SIDE_SHAPE_TRIANGLE) && pSide->IsTagged())
                ++nCount;
    }
    return nCount;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// TODO: Beginning with the current side, walk through all adjacent sides and 
// mark all in plane

int CTaggingStrategy::Run(short nSegment, short nSide)
{
    segmentManager.UnTagAll(m_tag);

    int nHead = 0;
    int nTail = 1;

    m_sideList[nHead].m_parent = CSideKey(-1, -1);
    m_sideList[nHead].m_child = CSideKey(nSegment, nSide);
    segmentManager.Segment(nSegment)->Tag(nSide, m_tag);

    segmentManager.GatherEdges(m_edgeTree);

    segmentManager.ComputeNormals(false);

    CEdgeList edgeList;

    while (nHead < nTail) {
        m_parent = m_sideList[nHead++].m_child;
        m_pSegment = segmentManager.Segment(m_parent);
        m_pSide = segmentManager.Side(m_parent);
        edgeList.Reset();

        if (!m_pSegment->BuildEdgeList(edgeList, ubyte(m_parent.m_nSide), false))
            continue;
        int nEdges = edgeList.Count();
        for (int nEdge = 0; nEdge < nEdges; nEdge++) {
            ubyte side1, side2, i1, i2;
            edgeList.Get(nEdge, side1, side2, i1, i2);
            ushort v1 = m_pSegment->VertexId(i1);
            ushort v2 = m_pSegment->VertexId(i2);
            CEdgeTreeNode* node = m_edgeTree.Find(EdgeKey(v1, v2));
            if (!node)
                continue;
            CSLLIterator<CSideKey, CSideKey> iter(node->m_sides);
            for (iter.Begin(); *iter != iter.End(); iter++) {
                m_child = **iter;
                m_pChildSeg = segmentManager.Segment(m_child);

                if (!m_pChildSeg->IsTagged(iter->m_nSide, m_tag)) {
                    m_pChildSide = segmentManager.Side(m_child);
                    if (Accept()) {
                        m_pChildSeg->Tag(iter->m_nSide, m_tag);
                        m_pChildSeg->TagVertices(m_tag, iter->m_nSide);
                        m_sideList[nTail].m_edge = m_edgeKey;
                        m_sideList[nTail].m_parent = m_parent;
                        m_sideList[nTail++].m_child = m_child;
                    }
                }
            }
        }
    }
    return nTail;
}

// -----------------------------------------------------------------------------

CTagByAngle::CTagByAngle() :
    m_maxAngle(cos(Radians(22.5)))
{}

bool CTagByAngle::Accept()
{
    return m_pChildSide->IsVisible() &&
        (Dot(m_pSide->Normal(), m_pChildSide->Normal()) >= m_maxAngle);
}

CTagByTextures::CTagByTextures(short nBaseTex, short nOvlTex, bool bIgnorePlane) :
    m_nBaseTex(nBaseTex),
    m_nOvlTex(nOvlTex),
    m_bIgnorePlane(bIgnorePlane)
{
    m_bAll = !segmentManager.HasTaggedSegments(true);
}

// Return true if the child side can be aligned from the parent side.
// Filters for marked cubes and matching primary/secondary textures.
// Currently only filters out back faces of cube connections without bIgnorePlane set
// (handling that case would require building the edge list differently).
bool CTagByTextures::Accept()
{
    if (!m_bAll)
    {
        bool bThisTagged = m_pSegment->IsTagged() || m_pSide->IsTagged();
        bool bChildTagged = m_pChildSeg->IsTagged() || m_pChildSide->IsTagged();
        if (!bThisTagged || !bChildTagged)
            return false;
    }
    if (!m_pChildSide->IsVisible())
        return false;
    if (!m_bIgnorePlane)
    {
        short nOtherSide;
        if ((m_parent.m_nSegment == m_child.m_nSegment) ||
            (m_pSegment->CommonSides(m_child.m_nSegment, nOtherSide) == -1) ||
            (nOtherSide == m_child.m_nSide)) // don't align the connected side
            return false;
    }
    return ((m_nBaseTex < 0) || (m_pChildSide->BaseTex() == m_nBaseTex)) &&
        ((m_nOvlTex < 0) || (m_pChildSide->OvlTex(0) == m_nOvlTex));
}

CTagTunnelStart::CTagTunnelStart(CSide* side) :
    m_maxAngle(cos(Radians(22.5))),
    m_pStartSide(side)
{}

bool CTagTunnelStart::Accept()
{
    return (m_pChildSeg->IsTagged() || m_pChildSide->IsTagged()) &&
        !m_pChildSeg->HasChild(m_child.m_nSide) &&
        (Dot(m_pChildSide->Normal(), m_pStartSide->Normal()) >= m_maxAngle);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
//eof segmentmanager.cpp
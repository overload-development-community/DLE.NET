
#include "selection.h"
#include "TriggerManager.h"
#include "WallManager.h"
#include "segmentManager.h"
#include "ObjectManager.h"
#include "VertexManager.h"
#include "LightManager.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

CSelection selections [3];
CSelection* current = &selections [0];
CSelection* other = &selections [1];
CSelection* nearest = &selections [2]; // nearest to current mouse position

// -----------------------------------------------------------------------------

void CSelection::Setup (short nSegment, short nSide, short nLine, short nPoint) 
{
if (nSegment >= 0) 
	m_nSegment = nSegment;
if (nSide >= 0) 
	m_nSide = nSide;
if (nLine >= 0) 
	m_nEdge = nLine;
if (nPoint >= 0) 
	m_nPoint = nPoint;
}

// -----------------------------------------------------------------------------

void CSelection::Get (CSideKey& key)
{
if (key.m_nSegment < 0)
	key.m_nSegment = m_nSegment;
if (key.m_nSide < 0)
	key.m_nSide = m_nSide;
}

// -----------------------------------------------------------------------------

void CSelection::Get (short& nSegment, short& nSide)
{
if (nSegment < 0)
	nSegment = m_nSegment;
if (nSide < 0)
	nSide = m_nSide;
}

// -----------------------------------------------------------------------------

CSegment* CSelection::Segment (void)
{
return (m_nSegment < 0) ? null : segmentManager.Segment (m_nSegment);
}

// -----------------------------------------------------------------------------

short CSelection::ChildId (void)
{
return Segment ()->ChildId (m_nSide);
}

// -----------------------------------------------------------------------------

CSegment* CSelection::ChildSeg (void)
{
short nChild = ChildId ();
return (nChild < 0) ? null : segmentManager.Segment (nChild);
}

// -----------------------------------------------------------------------------

CSide* CSelection::Side (void)
{
return (m_nSegment < 0) ? null : segmentManager.Side (*this);
}

// -----------------------------------------------------------------------------
// opposing side of current segment

CSide* CSelection::OppositeSide (void)
{
return segmentManager.Side (CSideKey (m_nSegment, oppSideTable [m_nSide]));
}

// -----------------------------------------------------------------------------

CWall* CSelection::Wall (void)
{
return segmentManager.Wall (*this);
}

// -----------------------------------------------------------------------------

CTrigger* CSelection::Trigger (void)
{
CWall* pWall = Wall ();
return (pWall == null) ? null : pWall->Trigger ();
}

// -----------------------------------------------------------------------------

CGameObject* CSelection::Object (void)
{
return objectManager.Object (m_nObject);
}

// -----------------------------------------------------------------------------

CVertex* CSelection::Vertex (short offset)
{
return Segment ()->Vertex (Side ()->VertexIdIndex (m_nPoint + offset)); 
}

// -----------------------------------------------------------------------------

CColor* CSelection::LightColor (void)
{ 
return lightManager.LightColor (*this); 
}

// -----------------------------------------------------------------------------

void CSelection::Fix (short nSegment)
{
if (m_nSegment != nSegment)
	return;
short nChild;
if ((nChild = ChildId ()) > -1) {
	m_nSegment = nChild;
	return;
	}
if ((nChild = Segment ()->ChildId (oppSideTable [m_nSide])) > -1) {
	m_nSegment = nChild;
	return;
	}
for (short nSide = 0; nSide < 6; nSide++)
	if ((nChild = Segment ()->ChildId (nSide)) >= 0) {
		m_nSegment = nChild;
		return;
		}
if (segmentManager.Count () == 1)
	return;
if (m_nSegment >= segmentManager.Count () - 1)
	m_nSegment--;
else
	m_nSegment++;
}

// -----------------------------------------------------------------------------

short CSelection::SetSegmentId (short nSegment) 
{ 
if ((this == current) && (0 <= m_nSegment) && (0 <= m_nSide))
	Side ()->SetPoint ((ubyte) Point ());
m_nSegment = nSegment;
if ((0 <= m_nSegment) && (0 <= m_nSide))
	SetPoint (Side ()->Point ());
return m_nSegment;
}

// -----------------------------------------------------------------------------

short CSelection::SetSideId (short nSide) 
{ 
if ((this == current) && (0 <= m_nSegment) && (0 <= m_nSide))
	Side ()->SetPoint ((ubyte) Point ());
m_nSide = nSide;
if ((0 <= m_nSegment) && (0 <= m_nSide))
	SetPoint (Side ()->Point ());
return m_nSide; 
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


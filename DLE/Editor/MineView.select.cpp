// MineView.select.cpp: element selection UI functions


#include "stdafx.h"

//-----------------------------------------------------------------------------

static bool PointIsInTriangle2D (CDoubleVector& p, CDoubleVector t [])
{
CDoubleVector v0 = t [2];
v0 -= t [0];
CDoubleVector v1 = t [1];
v1 -= t [0];
CDoubleVector v2 = p;
v2 -= t [0];
double dot00 = Dot (v0, v0);
double dot11 = Dot (v1, v1);
double dot01 = Dot (v0, v1);
double dot02 = Dot (v0, v2);
double dot12 = Dot (v1, v2);
double invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
return (u >= 0.0) && (v >= 0.0) && (u + v <= 1.0);
}

//-----------------------------------------------------------------------------

void CMineView::SelectCurrentObject (long xMouse, long yMouse) 
{
	short nClosestObj = m_presenter.FindNearestObject(xMouse, yMouse);

	// unhighlight current object and select next object
	short i = current->ObjectId();
	RefreshObject(i, nClosestObj);
}

//-----------------------------------------------------------------------------

bool CMineView::SelectCurrentSide (long xMouse, long yMouse, bool bAddToTagged) 
{
if (!m_bEnableQuickSelection && m_inputHandler.MouseState () != eMouseStateApplySelect)
	return false;
if ((m_inputHandler.MouseState () == eMouseStateApplySelect) && (nearest->m_nSegment >= 0) && (nearest->m_nSide >= 0)) {
	current->SetSegmentId (nearest->m_nSegment);
	current->SetSideId (nearest->m_nSide);
	}
else {
	short nSide, nSegment = m_presenter.FindSelectedTexturedSide (xMouse, yMouse, nSide);
	if (0 > nSegment) 
		return false;
	current->Setup (nSegment, nSide);
	}
if (Perspective () && bAddToTagged)
	current->Segment ()->TagVertices (TAGGED_MASK, current->SideId ());
DLE.ToolView ()->Refresh ();
Refresh ();
return true;
}

//-----------------------------------------------------------------------------

bool CMineView::SelectCurrentSegment (long xMouse, long yMouse, bool bAddToTagged) 
{
return SelectCurrentSide (xMouse, yMouse, bAddToTagged);
}

//-----------------------------------------------------------------------------

bool CMineView::SelectCurrentLine (long xMouse, long yMouse, bool bAddToTagged) 
{
if (!m_bEnableQuickSelection && m_inputHandler.MouseState () != eMouseStateApplySelect)
	return false;

short nSegment = -1, nSide = -1, nEdge = -1;
CSegment *pSegment = null;
if (m_inputHandler.MouseState () == eMouseStateApplySelect && nearest->Edge () >= 0) {
	nSegment = nearest->m_nSegment;
	nSide = nearest->m_nSide;
	nEdge = nearest->Edge ();
	pSegment = nearest->Segment ();
	}
else {
	CSide *pSide = null;
	if (m_inputHandler.MouseState() != eMouseStateApplySelect)
	{
		pSegment = current->Segment();
		pSide = current->Side();
	}
	nEdge = m_presenter.FindNearestLine(&pSegment, &pSide, LastMousePos().x, LastMousePos().y);
	if (nEdge < 0 || !pSegment || !pSide)
		return false;
	nSegment = segmentManager.Index (pSegment);
	nSide = pSegment->SideIndex (pSide);
	}

ushort nVertices [2] = {pSegment->VertexId (nSide, nEdge), pSegment->VertexId (nSide, nEdge + 1)};
if (bAddToTagged && (nVertices [0] <= MAX_VERTEX) && (nVertices [1] <= MAX_VERTEX)) {
	vertexManager [nVertices [0]].Tag ();
	vertexManager [nVertices [1]].Tag ();
	}
current->SetSegmentId (nSegment);
current->SetSideId (nSide);
current->SetEdge (nEdge);
current->SetPoint (nEdge);

DLE.ToolView ()->Refresh ();
Refresh ();
return true;
}

//-----------------------------------------------------------------------------

int CMineView::FindNearestVertex (long xMouse, long yMouse, bool bCurrentSideOnly)
{
	CDoubleVector screen, clickPos (xMouse, yMouse, 0.0);
	CVertex* pVertex = vertexManager.Vertex (0);
	double dMin = 1e30;
	int xMax = ViewWidth (), yMax = ViewHeight ();
	int nVertex = -1;

for (int i = vertexManager.Count (); i; i--, pVertex++) {
#if 0
	if (!pVertex->InRange (xMax, yMax, GetRenderer()))
		continue;
	double d = Distance (clickPos, pVertex->m_proj);
#else
	CVertex v = *pVertex;
	if (!v.InRange (xMax, yMax, GetRenderer()))
		continue;
	if (bCurrentSideOnly) {
		if (!current->Segment ()->HasVertex (vertexManager.Index (pVertex)))
			continue;
		short nSegmentVertexIndex = current->Segment ()->VertexIndex (vertexManager.Index(pVertex));
		if (!current->Side ()->HasVertex ((ubyte) nSegmentVertexIndex))
			continue;
		}
	v.m_proj.v.z = 0.0;
	double d = Distance (clickPos, v.m_proj);
#endif
	if (dMin > d) {
		dMin = d;
		nVertex = i;
		}
	}
return (nVertex < 0) ? -1 : vertexManager.Count () - nVertex;
}

//-----------------------------------------------------------------------------

bool CMineView::SelectCurrentPoint (long xMouse, long yMouse, bool bAddToTagged) 
{
if (!m_bEnableQuickSelection && m_inputHandler.MouseState () != eMouseStateApplySelect)
	return false;

int nVertex = FindNearestVertex (xMouse, yMouse, m_inputHandler.MouseState () != eMouseStateApplySelect);
if (nVertex >= 0) {
	if (bAddToTagged)
		vertexManager [nVertex].Tag ();

	// Does the current side contain this vertex? If so, use it
	short nSegmentVertexIndex = current->Segment ()->VertexIndex (nVertex);
	short nSideVertexIndex = current->Side ()->FindVertexIdIndex ((ubyte) nSegmentVertexIndex);
	if (nSideVertexIndex >= 0) {
		current->SetEdge (nSideVertexIndex);
		current->SetPoint (nSideVertexIndex);
		}
	else {
		// Need to find a segment with this vertex
		CSegment *pSegment = segmentManager.Segment (0);
		for (int i = 0; i < segmentManager.Count (); i++, pSegment++) {
			nSegmentVertexIndex = pSegment->VertexIndex (nVertex);
			for (int j = 0; nSegmentVertexIndex >= 0 && j < MAX_SIDES_PER_SEGMENT; j++) {
				nSideVertexIndex = pSegment->SideVertexIndex (j, (ubyte) nSegmentVertexIndex);
				if (nSideVertexIndex >= 0) {
					current->SetSegmentId (i);
					current->SetSideId (j);
					current->SetEdge (nSideVertexIndex);
					current->SetPoint (nSideVertexIndex);
					break;
					}
				}
			if (nSideVertexIndex >= 0)
				break;
			}
		}
	}
DLE.ToolView ()->Refresh ();
Refresh ();
return true;
}

//-----------------------------------------------------------------------------

bool CMineView::SelectCurrentElement (long xMouse, long yMouse, bool bAddToTagged) 
{
switch (theMine->SelectMode ()) {
	case POINT_MODE:
		return SelectCurrentPoint (xMouse, yMouse, bAddToTagged);

	case LINE_MODE:
		return SelectCurrentLine (xMouse, yMouse, bAddToTagged);

	case SIDE_MODE:
		return SelectCurrentSide (xMouse, yMouse, bAddToTagged);

	case SEGMENT_MODE:
	case BLOCK_MODE:
		return SelectCurrentSegment (xMouse, yMouse, bAddToTagged);

	case OBJECT_MODE:
		SelectCurrentObject (xMouse, yMouse);
		return true;
	}
return false;
}

//-----------------------------------------------------------------------------

void CMineView::NextPoint (int dir) 
{
if (current->Side ()->Shape () > SIDE_SHAPE_TRIANGLE)
	NextSide (dir);
else
	current->SetPoint (Wrap (current->Point (), dir, 0, current->Side ()->VertexCount () - 1));
Refresh ();
}

//-----------------------------------------------------------------------------

void CMineView::PrevPoint (void)
{
NextPoint (-1);
}

//-----------------------------------------------------------------------------

void CMineView::NextSide (int dir) 
{
short nSide = current->SideId (); 
do {
	current->SetSideId (Wrap (current->SideId (), dir, 0, 5));
	} while ((nSide != current->SideId ()) && (current->Side ()->Shape () > SIDE_SHAPE_TRIANGLE));
Refresh (true);
}

//-----------------------------------------------------------------------------

void CMineView::PrevSide () 
{
NextSide (-1);
}

//-----------------------------------------------------------------------------

void CMineView::NextLine (int dir) 
{
if (current->Side ()->Shape () > SIDE_SHAPE_TRIANGLE)
	NextSide (dir);
else
	current->SetEdge (Wrap (current->Edge (), dir, 0, current->Side ()->VertexCount () - 1));
Refresh ();
}

//-----------------------------------------------------------------------------

void CMineView::PrevLine (void) 
{
NextLine (-1);
}

//-----------------------------------------------------------------------------

void CMineView::NextSegment (int dir) 
{
if (segmentManager.Count () <= 0)
	return;

auto newSegmentId = Wrap(current->SegmentId(), dir, 0, segmentManager.Count() - 1);
current->SetSegmentId(newSegmentId);
Refresh (true);
}

//-----------------------------------------------------------------------------

void CMineView::PrevSegment (void) 
{
NextSegment (-1);
}

//-----------------------------------------------------------------------------
// MENU - Forward_Cube
//
// ACTION - If child exists, this routine sets current_segment to child pSegment
//
// Changes - Smart side selection added (v0.8)
//         Smart side selection done before moving (instead of after) (v0.9)
//-----------------------------------------------------------------------------

void CMineView::SegmentForward (int dir) 
{
	CSegment *pSegment,*pChildSeg;
	short nChild, nSide;
	bool bFwd = (dir == 1);

pSegment = segmentManager.Segment (current->SegmentId ());
nChild = pSegment->ChildId (bFwd ? current->SideId (): oppSideTable [current->SideId ()]);
if (nChild <= -1) {
	// first try to find a non backwards route
	for (nSide = 0; nSide < 6; nSide++) {
		if (pSegment->ChildId (nSide) != m_lastSegment && pSegment->ChildId (nSide) > -1) {
			nChild = pSegment->ChildId (nSide);
			current->SetSideId (bFwd ? nSide: oppSideTable [nSide]);
			break;
			}
		}
	// then settle for any way out
	if (nSide == 6) {
		for (nSide = 0; nSide < 6; nSide++) {
			if (pSegment->ChildId (nSide) > -1) {
				nChild = pSegment->ChildId (nSide);
				current->SetSideId (bFwd ? nSide: oppSideTable [nSide]);
				break;
				}
			}			
		}
	}
if (nChild > -1) {
	pChildSeg = segmentManager.Segment (nChild);
	// try to select side which is in same direction as current side
	for (nSide = 0; nSide < 6; nSide++) {
		if (pChildSeg->ChildId (nSide) == current->SegmentId ()) {
			current->SetSideId (bFwd ? oppSideTable [nSide]: nSide);
			break;
			}
		}
	m_lastSegment = current->SegmentId ();
	current->SetSegmentId (nChild);
	}
Refresh (true);
}

//-----------------------------------------------------------------------------

void CMineView::SegmentBackwards () 
{
SegmentForward (-1);
}

//-----------------------------------------------------------------------------

void CMineView::SelectOtherSegment () 
{
Swap (current, other);
//current = &selections [!current->Index ()];
//other = &selections [!current->Index ()];
Refresh (true);
DLE.ToolView ()->SegmentTool ()->Refresh ();
}

//-----------------------------------------------------------------------------

bool CMineView::SelectOtherSide () 
{
CSideKey opp;

if (segmentManager.BackSide (*current, opp) == null)
	return false;

*((CSideKey *) current) = opp;
Refresh (true);
DLE.ToolView ()->SegmentTool ()->Refresh ();
return true;
}

//-----------------------------------------------------------------------------

void CMineView::NextObject (int dir) 
{
  short oldObject = current->ObjectId ();
  short newObject = current->ObjectId ();

if (objectManager.Count () > 1) {
	newObject = Wrap (newObject, dir, 0, (short) objectManager.Count () - 1);
	Refresh (true);
	}
RefreshObject (oldObject, newObject);
}

//-----------------------------------------------------------------------------

void CMineView::PrevObject() 
{
NextObject (-1);
}

//-----------------------------------------------------------------------------

void CMineView::NextSegmentElement (int dir)
{
switch (m_selectMode) {
	case SelectMode::Point:
		NextPoint (dir);
		break;
	case SelectMode::Line:
		NextLine (dir);
		break;
	default:
		NextSide (dir);
		break;
	}
}

//------------------------------------------------------------------------------

void CMineView::PrevSegmentElement ()
{
NextSegmentElement (-1);
}

//------------------------------------------------------------------------------

void CMineView::LocateTexture (short nTexture)
{
	short nSegment = current->SegmentId ();
	short nSide = current->SideId () + 1;
	short	nSegments = segmentManager.Count ();

for (short i = nSegments; i; i--) {
	CSegment* pSegment = segmentManager.Segment (nSegment);
	for (short j = nSide; j < MAX_SIDES_PER_SEGMENT; j++) {
		CSide* pSide = pSegment->Side (j);
		if ((pSide->Shape () <= SIDE_SHAPE_TRIANGLE) && ((pSide->BaseTex () == nTexture) || (nTexture && (pSide->OvlTex () == nTexture)))) {
			current->SetSegmentId (nSegment);
			current->SetSideId (j);
			DLE.ToolView ()->Refresh ();
			Refresh ();
			return;
			}
		}
	nSide = 0;
	nSegment = (nSegment + 1) % nSegments;
	}
}

void CMineView::TagSelected()
{
	bool	bSegTag = false;
	CSegment* pSegment = current->Segment();
	int i, j, p[8], nPoints;

	switch (m_selectMode) {
	case SelectMode::Point:
		nPoints = 1;
		p[0] = current->VertexId();
		break;

	case SelectMode::Line:
		nPoints = 2;
		p[0] = pSegment->VertexId(current->Side()->VertexIdIndex(current->Point()));
		p[1] = pSegment->VertexId(current->Side()->VertexIdIndex(current->Point() + 1));
		break;

	case SelectMode::Side:
		nPoints = current->Side()->VertexCount();
		for (i = 0; i < nPoints; i++)
			p[i] = pSegment->VertexId(current->Side()->VertexIdIndex(i));
		break;

	case SelectMode::Segment:
	default:
		nPoints = 8;
		for (i = 0, j = 0; i < nPoints; i++) {
			p[j] = pSegment->VertexId(i);
			if (p[j] <= MAX_VERTEX)
				j++;
		}
		nPoints = j;
		break;
	}

	// set i to nPoints if all verts are marked
	for (i = 0; i < nPoints; i++)
		if (!vertexManager[p[i]].IsTagged())
			break;

	if (i == nPoints) { // if all verts are tagged, then untag them
		switch (m_selectMode) {
		case SelectMode::Point:
		case SelectMode::Line:
			for (i = 0; i < nPoints; i++)
				vertexManager[p[i]].UnTag();
			break;

		case SelectMode::Side:
			current->Segment()->UnTag(current->SideId());
			current->Segment()->UnTagVertices(TAGGED_MASK, current->SideId());
			break;

		case SelectMode::Segment:
		default:
			current->Segment()->UnTag();
			current->Segment()->UnTagVertices(TAGGED_MASK, -1);
			break;
		}
	}
	else { // otherwise tag all the points
		switch (m_selectMode) {
		case SelectMode::Point:
		case SelectMode::Line:
			for (i = 0; i < nPoints; i++)
				vertexManager[p[i]].Tag();
			break;

		case SelectMode::Side:
			current->Segment()->Tag(current->SideId());
			current->Segment()->TagVertices(TAGGED_MASK, current->SideId());
			break;

		case SelectMode::Segment:
		default:
			current->Segment()->Tag();
			current->Segment()->TagVertices(TAGGED_MASK, -1);
			break;
		}
	}
	DLE.MineView()->Refresh();
}

//------------------------------------------------------------------------------
//eof
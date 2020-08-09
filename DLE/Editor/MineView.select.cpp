// MineView.select.cpp: element selection UI functions


#include "stdafx.h"

//-----------------------------------------------------------------------------

void CMineView::SelectNearestObject(long xMouse, long yMouse)
{
	CSelection currentElement;
	currentElement.Setup();
	m_presenter.FindNearestObject(xMouse, yMouse, &currentElement);
	SelectObject(currentElement);
}

void CMineView::SelectObject (const CSelection& selection)
{
	// unhighlight current object and select next object
	RefreshObject(current->ObjectId(), selection.ObjectId());
}

//-----------------------------------------------------------------------------

bool CMineView::SelectSide (const CSelection& selection, bool bAddToTagged)
{
	if (!m_bEnableQuickSelection && m_inputHandler.MouseState() != eMouseStateApplySelect)
		return false;
	if (0 > selection.SegmentId())
		return false;
	current->Setup(selection.SegmentId(), selection.SideId());
	if (Perspective() && bAddToTagged)
		current->Segment()->TagVertices(TAGGED_MASK, current->SideId());
	DLE.ToolView()->Refresh();
	Refresh();
	return true;
}

//-----------------------------------------------------------------------------

bool CMineView::SelectSegment (const CSelection& selection, bool bAddToTagged)
{
	return SelectSide(selection, bAddToTagged);
}

//-----------------------------------------------------------------------------

bool CMineView::SelectLine (const CSelection& selection, bool bAddToTagged)
{
	if (!m_bEnableQuickSelection && m_inputHandler.MouseState() != eMouseStateApplySelect)
		return false;

	short nSide = selection.SideId();
	short nEdge = selection.Edge();
	if (selection.SegmentId() < 0 || nSide < 0 || nEdge < 0)
		return false;

	CSegment* pSegment = selection.Segment();
	ushort nVertices[2]{
		pSegment->VertexId(nSide, nEdge),
		pSegment->VertexId(nSide, nEdge + 1)
	};
	if (bAddToTagged && (nVertices[0] <= MAX_VERTEX) && (nVertices[1] <= MAX_VERTEX))
	{
		vertexManager[nVertices[0]].Tag();
		vertexManager[nVertices[1]].Tag();
	}
	current->Setup(selection.SegmentId(), nSide, nEdge, nEdge);

	DLE.ToolView()->Refresh();
	Refresh();
	return true;
}

//-----------------------------------------------------------------------------

bool CMineView::SelectPoint (const CSelection& selection, bool bAddToTagged)
{
	if (!m_bEnableQuickSelection && m_inputHandler.MouseState() != eMouseStateApplySelect)
		return false;

	int nVertex = vertexManager.Index(selection.Vertex());
	if (nVertex >= 0)
	{
		if (bAddToTagged)
			vertexManager[nVertex].Tag();
		current->Setup(selection.SegmentId(), selection.SideId(), selection.Edge(), selection.Point());
	}

	DLE.ToolView()->Refresh();
	Refresh();
	return true;
}

//-----------------------------------------------------------------------------

bool CMineView::SelectCurrentElement(long xMouse, long yMouse, bool bAddToTagged)
{
	bool useNearest = (m_inputHandler.MouseState() == eMouseStateApplySelect);
	CSelection currentElement;
	if (useNearest)
	{
		currentElement = *nearest;
	}

	switch (theMine->SelectMode())
	{
	case POINT_MODE:
		currentElement.Setup();
		m_presenter.FindNearestVertex(xMouse, yMouse, &currentElement, useNearest ? nullptr : current->Side());
		return SelectPoint(currentElement, bAddToTagged);

	case LINE_MODE:
		if (!useNearest || (nearest->Edge() < 0))
		{
			currentElement.Setup();
			m_presenter.FindNearestLine(xMouse, yMouse, &currentElement, useNearest ? nullptr : current->Side());
		}
		return SelectLine(currentElement, bAddToTagged);

	case SIDE_MODE:
		if (!useNearest || (nearest->m_nSegment < 0) || (nearest->m_nSide < 0))
		{
			currentElement.Setup();
			m_presenter.FindNearestSide(xMouse, yMouse, &currentElement);
		}
		return SelectSide(currentElement, bAddToTagged);

	case SEGMENT_MODE:
	case BLOCK_MODE:
		if (!useNearest || (nearest->m_nSegment < 0) || (nearest->m_nSide < 0))
		{
			currentElement.Setup();
			m_presenter.FindNearestSegment(xMouse, yMouse, &currentElement);
		}
		return SelectSegment(currentElement, bAddToTagged);

	case OBJECT_MODE:
		currentElement.Setup();
		m_presenter.FindNearestObject(xMouse, yMouse, &currentElement);
		SelectObject(currentElement);
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
	for (short j = 0; j < MAX_SIDES_PER_SEGMENT; j++) {
		nSide %= MAX_SIDES_PER_SEGMENT;
		CSide* pSide = pSegment->Side (nSide);
		if ((pSide->Shape () <= SIDE_SHAPE_TRIANGLE) && ((pSide->BaseTex () == nTexture) || (nTexture && (pSide->OvlTex () == nTexture)))) {
			current->SetSegmentId (nSegment);
			current->SetSideId (nSide);
			DLE.ToolView ()->Refresh ();
			Refresh ();
			return;
			}
		nSide++;
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
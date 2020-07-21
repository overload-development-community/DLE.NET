// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"

//------------------------------------------------------------------------------

BOOL CMineView::UpdateDragPos (void)
{
if (theMine == null) return FALSE;

if (m_inputHandler.MouseState () != eMouseStateDrag)
	return FALSE;

InvalidateRect (null, TRUE);
return TRUE;
}

//------------------------------------------------------------------------------

void CMineView::InitDrag ()
{
	m_presenter.BeginDrag();
}

//------------------------------------------------------------------------------

BOOL CMineView::DrawDragPos (void)
{
if (theMine == null) return FALSE;

if (m_inputHandler.MouseState () != eMouseStateDrag)
	return FALSE;
if (LastMousePos() == m_presenter.DragPos())
	return FALSE;
return m_presenter.DrawDragPos(LastMousePos());
}

//------------------------------------------------------------------------------

void CMineView::FinishDrag (CPoint releasePos)
{
CHECKMINE;

//ReleaseCapture ();
	int		m_changesMade = 1;
	int		i, newVert, count = 0;
	long		xPos,yPos;
	short		point1,vert1;
	short		point2,vert2;

undoManager.Begin (__FUNCTION__, udVertices | udSegments);
xPos = releasePos.x;
yPos = releasePos.y;
point1 = current->Side ()->VertexIdIndex (current->Point ());
vert1 = segmentManager.Segment (0) [current->SegmentId ()].m_info.vertexIds [point1];
// find point to merge with
for (i = 0; i < vertexManager.Count (); i++) {
	CVertex& v = vertexManager [i];
	if ((abs (xPos - v.m_screen.x) < 5) && (abs (yPos - v.m_screen.y) < 5)) {
		count++;
		newVert = i;
		}
	}
// if too many matches found
if ((count > 1) && 
	 (QueryMsg ("It is not clear which point you want to snap to."
				   "Do you want to attach these points anyway?") == IDYES))
	count = 1;
if (count == 1) {
// make sure new vert is not one of the current segment's verts
	for (i = 0; i < 8; i++) {
		if (i != point1) {
			vert2 = current->Segment ()->m_info.vertexIds [i];
			if (newVert == vert2) {
				ErrorMsg ("Cannot drop point onto another corner of the current segment.");
				break;
				}
			}
		}
	if ((i == 8) && (newVert != vert1)) {
	// make sure the new line lengths are close enough
		for (i = 0; i < 3; i++) {
			point2 = adjacentPointTable [point1][i];
			vert2 = segmentManager.Segment (current->SegmentId ())->m_info.vertexIds [point2];
			if (Distance (*vertexManager.Vertex (newVert), *vertexManager.Vertex (vert2)) >= 1000.0) {
				ErrorMsg ("Cannot move this point so far away.");
				break;
				}
			}
		if (i == 3) { //
			// replace origional vertex with new vertex
			current->Segment ()->m_info.vertexIds [point1] = newVert;
			// all unused vertices
			vertexManager.DeleteUnused ();
			segmentManager.FixChildren (current);
			segmentManager.SetLinesToDraw ();
			}
		}
	}
else {
	// no vertex found, just drop point along screen axii
	CLongVector p;
	p.x = (short) xPos;
	p.y = (short) yPos;
	p.z = vertexManager [vert1].m_screen.z;
	// BeginRender recalculates the view matrix
	Renderer ().BeginRender (false);
	ViewMatrix ()->Unproject (*vertexManager.Vertex (vert1), p, m_presenter.ViewCenter());
	Renderer ().EndRender ();
	}
undoManager.End (__FUNCTION__);
m_presenter.EndDrag();
Refresh ();
}

//------------------------------------------------------------------------------


//eof
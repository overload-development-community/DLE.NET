// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"
#include "winuser.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"

#include "PaletteManager.h"
#include "textures.h"
#include "global.h"
#include "FileManager.h"

#include <math.h>
#include <time.h>

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
	short nVert = current->Side ()->VertexIdIndex (current->Point ());
	short i = current->Segment ()->m_info.vertexIds [nVert];
	CVertex& v = vertexManager [i];

m_highlightPos.x = v.m_screen.x;
m_highlightPos.y = v.m_screen.y;
m_lastDragPos = m_highlightPos;
}

//------------------------------------------------------------------------------

void CMineView::HighlightDrag (short nVert, long x, long y) 
{
CHECKMINE;

Renderer ().SelectObject ((HBRUSH) GetStockObject (NULL_BRUSH));
if (!m_nRenderer)
	DC ()->SetROP2 (R2_NOT);
else
	Renderer ().SelectPen (penWhite + 1);

Renderer ().Ellipse (x, y, 4, 4);

CRect	rc (x, y, x, y);
for (int i = 0; i < 3; i++) {
	Renderer ().MoveTo (x, y);
	short nVert2 = adjacentPointTable [nVert][i];
	CVertex& v = vertexManager [current->Segment ()->m_info.vertexIds [nVert2]];
	Renderer ().LineTo (v.m_screen.x, v.m_screen.y);
	if (rc.left > v.m_screen.x)
		rc.left = v.m_screen.x;
	if (rc.right < v.m_screen.x)
		rc.right = v.m_screen.x;
	if (rc.top > v.m_screen.y)
		rc.top = v.m_screen.y;
	if (rc.bottom < v.m_screen.y)
		rc.bottom = v.m_screen.y;
	}

if (!m_nRenderer) {
	DC ()->SetROP2 (R2_COPYPEN);
	rc.InflateRect (4, 4);
	InvalidateRect (rc, FALSE);
	UpdateWindow ();
	}
}

//------------------------------------------------------------------------------

BOOL CMineView::DrawDragPos (void)
{
if (theMine == null) return FALSE;

if (m_inputHandler.MouseState () != eMouseStateDrag)
	return FALSE;
if (LastMousePos () == m_lastDragPos)
	return FALSE;

int i;

short nVert = current->Side ()->VertexIdIndex (current->Point ());

Renderer ().BeginRender (true);
// unhighlight last point and lines drawing
if (!m_nRenderer)
	HighlightDrag (nVert, m_lastDragPos.x, m_lastDragPos.y);
// highlight the new position
HighlightDrag (nVert, LastMousePos ().x, LastMousePos ().y);
m_lastDragPos = LastMousePos ();

if (!m_nRenderer)
	DC ()->SetROP2 (R2_NOT);

for (i = 0; i < vertexManager.Count (); i++) {
	CVertex& v = vertexManager [i];
	if ((abs (v.m_screen.x - LastMousePos ().x) < 5) && (abs (v.m_screen.y - LastMousePos ().y) < 5)) {
		if ((v.m_screen.x != m_highlightPos.x) || (v.m_screen.y != m_highlightPos.y)) {
			if (m_highlightPos.x != -1)
				// erase last point
				Renderer ().Ellipse (m_highlightPos, 8, 8);
			// define and draw new point
			m_highlightPos.x = v.m_screen.x;
			m_highlightPos.y = v.m_screen.y;
			Renderer ().Ellipse (m_highlightPos, 8, 8);
			break;
			}
		}
	}
// if no point found near cursor
if ((i >= vertexManager.Count ()) && (m_highlightPos.x != -1))
	// erase last point
	Renderer ().Ellipse (m_highlightPos, 8, 8);

Renderer ().EndRender ();
if (!m_nRenderer)
	DC ()->SetROP2 (R2_COPYPEN);
// define and draw new point
m_highlightPos.x = -1;
m_highlightPos.y = -1;
return TRUE;
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
			segmentManager.FixChildren ();
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
	ViewMatrix ()->Unproject (*vertexManager.Vertex (vert1), p, m_viewCenter);
	Renderer ().EndRender ();
	}
undoManager.End (__FUNCTION__);
Refresh ();
}

//------------------------------------------------------------------------------


//eof
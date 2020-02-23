// MineView.Draw.cpp: Mine rendering
//

#include "stdafx.h"
#include "winuser.h"
#include "dle-xp.h"

#include "dlcDoc.h"
#include "mineview.h"

#include "PaletteManager.h"
#include "TextureManager.h"
#include "global.h"
#include "FileManager.h"
#include "ModelManager.h"

#include <math.h>
#include <time.h>

extern short nDbgSeg, nDbgSide;
extern int nDbgVertex;

#define MODEL_DISPLAY_LIMIT 300.0 // max. distance from viewer at which 3D models and icons are rendered instead of angled arrows

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CMineView::CalcSegDist (void)
{
CHECKMINE;

	static short segRef [SEGMENT_LIMIT];

for (CSegmentIterator si; si; si++)
	si->Index () = -1;
segRef [0] = current->SegmentId ();	
current->Segment ()->Index () = 0;

int i = 1, h = 0, j = 0;
int segCount = segmentManager.Count ();

for (short nDist = 1; (j < segCount) && (h < i); nDist++) {
	for (h = i; j < h; j++) {
		CSegment* segI = segmentManager.Segment (segRef [j]);
		for (short nSide = 0; nSide < 6; nSide++) {
			short nChild = segI->ChildId (nSide);
			if (nChild < 0) 
				continue;
			CSegment* segJ = segmentManager.Segment (nChild);
			if (segJ->Index () != -1)
				continue;
			segJ->Index () = nDist;
			segRef [i++] = nChild;
			}
		}
	}
}

//------------------------------------------------------------------------------

void CMineView::DrawMineCenter (void)
{
if (Renderer ().Perspective ())
	return;

	CViewMatrix*	viewMatrix = ViewMatrix ();

if (m_nMineCenter == 1) {
	Renderer ().SelectPen (penWhite + 1);
	int nScale = int (20.0 * Scale ().v.y) + 1;
	CVertex v = viewMatrix->Origin ();
	viewMatrix->Transform (v.m_view, v);
	v.Project (viewMatrix);
	Renderer ().BeginRender (true);
	Renderer ().MoveTo (v.m_screen.x, v.m_screen.y - nScale);
	Renderer ().LineTo (v.m_screen.x, v.m_screen.y + nScale);
	Renderer ().MoveTo (v.m_screen.x - nScale, v.m_screen.y);
	Renderer ().LineTo (v.m_screen.x + nScale, v.m_screen.y);
	Renderer ().EndRender ();
	}
else if (m_nMineCenter == 2) {
	// draw a globe
	// 5 circles around each axis at angles of 30, 60, 90, 120, and 150
	// each circle has 18 points
	CVertex			v, center, circles [3][120 / 30 + 1][360 / 15 + 1];

	int h, i, j, m, n;

	Renderer ().BeginRender ();
	for (i = -60, m = 0; i <= 60; i += 30, m++) {
		for (j = 0, n = 0; j <= 360; j += 15, n++) {
			double sini = sin (Radians (i));
			double cosi = cos (Radians (i));
			double sinj = sin (Radians (j));
			double cosj = cos (Radians (j));
			double scale = 5 * cosi;

			v.Set (scale * cosj, scale * sinj, 5 * sini);
			v += viewMatrix->Origin ();
			if (!m_nRenderer)
				v -= viewMatrix->m_data [0].m_translate;
			viewMatrix->Transform (v.m_view, v);
			v.Project (viewMatrix);
			circles [0][m][n] = v;

			v.Set (scale * cosj, 5 * sini, scale * sinj);
			v += viewMatrix->Origin ();
			if (!m_nRenderer)
				v -= viewMatrix->m_data [0].m_translate;
			viewMatrix->Transform (v.m_view, v);
			v.Project (viewMatrix);
			circles [1][m][n] = v;

			v.Set (5 * sini, scale * cosj, scale * sinj);
			v += viewMatrix->Origin ();
			if (!m_nRenderer)
				v -= viewMatrix->m_data [0].m_translate;
			viewMatrix->Transform (v.m_view, v);
			v.Project (viewMatrix);
			circles [2][m][n] = v;
			}
		}

	center = ViewMatrix ()->Origin ();
	viewMatrix->Transform (center.m_view, center);
	Renderer ().EndRender ();

	ePenColor penColors [3] = {penGreen, penGray, penGold};

	Renderer ().BeginRender (true);
	for (h = 0; h < 3; h++) {
		Renderer ().SelectPen (penColors [h] + 1, 2);
		for (i = -60, m = 0; i <= 60; i += 30, m++) {
			for (j = 0, n = 0; j <= 360; j += 15, n++) {
				CVertex v = circles [h][m][n];
				if (j == 0)
					Renderer ().MoveTo (v.m_screen.x, v.m_screen.y);
				else if (m_nRenderer ? (v.m_view.v.z < center.m_view.v.z) : (v.m_screen.z <= 0))
					Renderer ().LineTo (v.m_screen.x, v.m_screen.y);
				else 
					Renderer ().MoveTo (v.m_screen.x, v.m_screen.y);
				}
			}
		}
	Renderer ().EndRender ();
	}
}
//----------------------------------------------------------------------------
// DrawWireFrame
//----------------------------------------------------------------------------

void CMineView::DrawWireFrame (bool bSparse)
{
CHECKMINE;

CalcSegDist ();
Renderer ().BeginRender (Renderer ().Type () == 0);
Renderer ().SelectPen (penGray + 1);
CSegment* pSegment = segmentManager.Segment (0);
short segCount = segmentManager.Count ();
if (m_nRenderer)
	glDepthFunc (GL_LEQUAL);
for (short nSegment = 0; nSegment < segCount; nSegment++, pSegment++) 
	DrawSegmentWireFrame (pSegment, bSparse);
Renderer ().EndRender ();
if (Visible (current->Segment ())) {
	DrawCurrentSegment (current->Segment (), bSparse);
	}
}

//----------------------------------------------------------------------------

static CFaceListEntry faceRenderList [SEGMENT_LIMIT * 6];

void SortFaces (int left, int right)
{
	CFaceListEntry m = faceRenderList [(left + right) / 2];
	int l = left, r = right;

do {
	while ((faceRenderList [l].m_zMax > m.m_zMax) || ((faceRenderList [l].m_zMax == m.m_zMax) && (faceRenderList [l].m_zMin > m.m_zMin)))
		l++;
	while ((faceRenderList [r].m_zMax < m.m_zMax) || ((faceRenderList [l].m_zMax == m.m_zMax) && (faceRenderList [l].m_zMin < m.m_zMin)))
		r--;
	if (l <= r) {
		if (l < r)
			Swap (faceRenderList [l], faceRenderList [r]);
		l++;
		r--;
		}
	}
while (l < r);
if (l < right)
	SortFaces (l, right);
if (left < r)
	SortFaces (left, r);
}

//----------------------------------------------------------------------------

void CMineView::DrawTexturedSegments (void)
{
CHECKMINE;
if (!textureManager.Available ())
	return;

// Draw Segments ()
short segCount = segmentManager.Count ();
int faceCount = 0;
CSegment* pSegment = segmentManager.Segment (0);
for (short nSegment = 0; nSegment < segCount; nSegment++, pSegment++) {
	if (!Visible (pSegment))
		continue;

	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
#ifdef _DEBUG
		if ((nSegment == nDbgSeg) && ((nDbgSide < 0) || (nSide == nDbgSide)))
			nDbgSeg = nDbgSeg;
#endif
#ifdef _DEBUG
		if (pSegment->m_info.bTunnel)
			continue;
#endif
		if (pSegment->ChildId (nSide) != -1) { // not a solid side
			CWall* pWall = pSide->Wall ();
			if (pWall == null) // no wall either
				continue;
			if (pWall->Type () == WALL_OPEN) // invisible wall
				continue;
			if ((pWall->Type () == WALL_CLOAKED) && (pWall->Info ().cloakValue == 0)) // invisible cloaked wall
				continue;
			}

		if (!FaceIsVisible (pSegment, pSide))
			continue;
		if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
			continue;

		long zMin = -LONG_MIN, zMax = LONG_MIN;
		for (ushort nVertex = 0; nVertex < 4; nVertex++) {
			long z = vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (nVertex)]].m_screen.z;
			if (zMin > z)
				zMin = z;
			if (zMax < z)
				zMax = z;
			}

		faceRenderList [faceCount].m_nSegment = nSegment;
		faceRenderList [faceCount].m_nSide = nSide;
		faceRenderList [faceCount].m_zMin = zMin;
		faceRenderList [faceCount].m_zMax = zMax;
		CWall* pWall = pSide->Wall ();
		faceRenderList [faceCount].m_bTransparent = pSegment->m_info.bTunnel || textureManager.Textures (pSide->BaseTex ())->Transparent () || ((pWall != null) && (pWall->Alpha () < 255));
		++faceCount;
		}
	}

if (!faceCount)
	return;
if (faceCount > 1) 
	SortFaces (0, faceCount - 1);
CalcSegDist ();

Renderer ().RenderFaces (faceRenderList, faceCount, Renderer ().IsRTTSupported ());
}

//--------------------------------------------------------------------------
// DrawSegment ()
//--------------------------------------------------------------------------

bool CMineView::InRange (short *pv, short i)
{
for (; i; i--, pv++) {
	if ((*pv <= MAX_VERTEX) && !vertexManager [*pv].InRange (m_viewMax.x, m_viewMax.y, Renderer ().Type ()))
		return false;
	}
return true;
}

//--------------------------------------------------------------------------

void CMineView::DrawSegmentHighlighted (short nSegment, short nSide, short nLine, short nPoint) 
{
CHECKMINE;

	CSegment *pSegment = segmentManager.Segment (nSegment);
	CSide* pSide = pSegment->Side (nSide);
	short xMax = ViewWidth () * 2;
	short yMax = ViewHeight () * 2;

	if (!Visible (pSegment))
		return;

// draw the segment's wire frame
if (pSegment->IsTagged ())
	Renderer ().SelectPen (penGold + 1, 2.0f);
else 
	Renderer ().SelectPen (((nSegment == current->SegmentId ()) || (nSegment == nearest->m_nSegment)) ? SelectMode (eSelectSegment) ? penRed + 1 : penWhite + 1 : penGray + 1, 2);   
DrawSegmentWireFrame (pSegment);

// draw the current side's outline
	int i;
if (!Renderer ().Ortho ())
	i = 4;
else {
	for (i = 0; i < 4; i++)
		if (!vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)]].InRange (xMax, yMax, Renderer ().Type ()))
			break;
	}
if (i == 4) {
	Renderer ().SelectPen (((nSegment == current->SegmentId ()) || (nSegment == nearest->m_nSegment)) ? SelectMode (eSelectSide) ? penRed + 1 : penGreen + 1 : penDkGreen + 1, 2);  
	short nVertices = pSide->VertexCount ();
	for (i = 0; i < nVertices; i++)
		DrawLine (pSegment, pSide->VertexIdIndex (i), pSide->VertexIdIndex (i + 1));
	}

// draw the current line
short i1 = pSide->VertexIdIndex (nLine);
short i2 = pSide->VertexIdIndex (nLine + 1);
if (!Renderer ().Ortho () ||
		(vertexManager [pSegment->m_info.vertexIds [i1]].InRange (xMax, yMax, Renderer ().Type ()) &&
		vertexManager [pSegment->m_info.vertexIds [i2]].InRange (xMax, yMax, Renderer ().Type ()))) 
	{
	Renderer ().SelectPen (((nSegment == current->SegmentId ()) || (nSegment == nearest->m_nSegment)) ? SelectMode (eSelectLine) ? penRed + 1 : penGold + 1 : penMedCyan + 1, 2);  
	DrawLine (pSegment, i1, i2);
	}

// draw a circle around the current vertex
Renderer ().SelectObject ((HBRUSH) GetStockObject (NULL_BRUSH));
Renderer ().SelectPen (((nSegment == current->SegmentId ()) || (nSegment == nearest->m_nSegment)) ? SelectMode (eSelectPoint) ? penRed + 1 : penGold + 1 : penMedCyan + 1, 2); 
i = pSegment->m_info.vertexIds [pSide->VertexIdIndex (nPoint)];
if (vertexManager [i].InRange (xMax, yMax, Renderer ().Type ()))
	Renderer ().Ellipse (vertexManager [i], 4, 4);
}

//--------------------------------------------------------------------------

void CMineView::DrawSegmentPartial (CSegment *pSegment) 
{
RenderSegmentWireFrame (pSegment, true);
}

//--------------------------------------------------------------------------

void QSortLineRef (POINT *lineRef, short left, short right)
{
	int		m = lineRef [(left + right) / 2].y;
	short	l = left, r = right;
do {
	while (lineRef [l].y < m)
		l++;
	while (lineRef [r].y > m)
		r--;
	if (l <= r) {
		if (l < r) {
			POINT h = lineRef [l];
			lineRef [l] = lineRef [r];
			lineRef [r] = h;
			}
		l++;
		r--;
		}
	}
while (l < r);
if (l < right)
	QSortLineRef (lineRef, l, right);
if (left < r)
	QSortLineRef (lineRef, left, r);
}

//--------------------------------------------------------------------------

void CMineView::DrawSparseSegmentWireFrame (CSegment *pSegment)
{
bool bOrtho = Renderer ().Ortho ();
CEdgeList edgeList;
int nEdges = pSegment->BuildEdgeList (edgeList, true);

for (short nSide = 0; nSide < 6; nSide++) {
	if (pSegment->ChildId (nSide) >= 0)
		continue;
	CSide* pSide = pSegment->Side (nSide);
	CPoint side [4], line [2];
	ubyte v [2];
	int i, j;

	for (i = 0; i < 4; i++) {
		side [i].x = vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)]].m_screen.x; 
		side [i].y = vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)]].m_screen.y; 
		}
	CDoubleVector a,b;
	a.v.x = (double) (side [1].x - side [0].x);
	a.v.y = (double) (side [1].y - side [0].y);
	b.v.x = (double) (side [3].x - side [0].x);
	b.v.y = (double) (side [3].y - side [0].y);
	if (a.v.x * b.v.y < a.v.y * b.v.x)
		Renderer ().SelectPen (penWhite + 1);
	else
		Renderer ().SelectPen (penGray + 1);
	// draw each line of the current side separately
	// only draw if there is no childP segment of the current segment with a common side
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 2; j++) {
			line [j] = side [(i+j)%4];
			v [j] = pSide->VertexIdIndex ((i+j)%4);
			}

		// Using the sparse edge list: if a neighboring side is 0xff, it's joined to another cube,
		// so don't render this line
		ubyte side1, side2;
		edgeList.Find (0, side1, side2, v [0], v [1]);
		if (side1 == 0xff || side2 == 0xff)
			continue;

		CVertex& v1 = vertexManager [pSegment->m_info.vertexIds [v [0]]];
		CVertex& v2 = vertexManager [pSegment->m_info.vertexIds [v [1]]];
		if (!bOrtho) {
			Renderer ().MoveTo (v1);
			Renderer ().LineTo (v2);
			}
		else {
			Renderer ().MoveTo (v1.m_screen.x, v1.m_screen.y);
			Renderer ().LineTo (v2.m_screen.x, v2.m_screen.y);
			}
		}
	}
}

//--------------------------------------------------------------------------

void CMineView::RenderSegmentWireFrame (CSegment *pSegment, bool bSparse, bool bTagged)
{
	int	bOrtho = Renderer ().Ortho ();

if (bOrtho) {
	if (!Visible (pSegment))
		return;
	}
else if (!bSparse) {
	if ((pSegment == current->Segment ()) || (pSegment == other->Segment ()))
		glDisable (GL_DEPTH_TEST);
	else
		glEnable (GL_DEPTH_TEST);
	glLineWidth (ViewOption (eViewTexturedWireFrame) ? 3.0f : 2.0f);
	}

	CEdgeList	edgeList;
	ushort*		vertexIds = pSegment->m_info.vertexIds;
	short			xMax = ViewWidth (),
					yMax = ViewHeight ();
	int			nType = Renderer ().Type ();
	ePenColor	pen;
	float			penWeight;
	bool			bSegmentIsTagged = pSegment->IsTagged ();
	bool			bSideTagged [2] = {false, false};
	bool			bFullWireFrame = !bTagged || bSegmentIsTagged || (m_viewOption != eViewTextured);

#ifdef _DEBUG
if (segmentManager.Index (pSegment) == nDbgSeg)
	nDbgSeg = nDbgSeg;
#endif
Renderer ().GetPen (pen, penWeight);
for (int i = 0, j = pSegment->BuildEdgeList (edgeList); i < j; i++) {
	ubyte i1, i2, side1, side2;
	edgeList.Get (i, side1, side2, i1, i2);
	if (!bSegmentIsTagged) {
		bSideTagged [0] = bSideTagged [1];
		bSideTagged [1] = pSegment->IsTagged (short (side1)) || pSegment->IsTagged (short (side2));
		if (bSideTagged [0] != bSideTagged [1]) {
			if (bSideTagged [1])
				Renderer ().SelectPen (penOrange + 1, ViewOption (eViewTexturedWireFrame) ? 3.0f : 2.0f);
			else
				Renderer ().SelectPen (pen + 1, penWeight);
			}
		}

	 if (!(bFullWireFrame || bSideTagged [1]))
		continue;
	CVertex& v1 = vertexManager [vertexIds [i1]];
	CVertex& v2 = vertexManager [vertexIds [i2]];
	if (!bOrtho) {
		Renderer ().MoveTo (v1);
		Renderer ().LineTo (v2);
		}
	else { //if (v1.InRange (xMax, yMax, nType) && v2.InRange (xMax, yMax, nType)) {
		Renderer ().MoveTo (v1.m_screen.x, v1.m_screen.y);
		Renderer ().LineTo (v2.m_screen.x, v2.m_screen.y);
		}
	}
if (bSideTagged [1])
	Renderer ().SelectPen (pen + 1, penWeight);
}

//--------------------------------------------------------------------------

void CMineView::DrawSegmentWireFrame (CSegment *pSegment, bool bSparse, bool bTagged, char bTunnel)
{
CHECKMINE;

if (!Visible (pSegment))
	return;
if (pSegment->m_info.bTunnel != bTunnel)
	return;

if (!m_nRenderer && (bSparse || Renderer ().Ortho ())) {
	short xMax = ViewWidth ();
	short yMax = ViewHeight ();
	ushort* vertexIds = pSegment->m_info.vertexIds;
	for (int i = 0; i < 8; i++, vertexIds++) {
		int v = *vertexIds;
		if ((v <= MAX_VERTEX) && !vertexManager [v].InRange (xMax, yMax, Renderer ().Type ()))
			return;
		}
	}

if (bSparse)
	DrawSparseSegmentWireFrame (pSegment);
else
	RenderSegmentWireFrame (pSegment, false, bTagged);
}

//--------------------------------------------------------------------------
//                        draw_segmentPoints()
//--------------------------------------------------------------------------

void CMineView::DrawSegmentPoints (CSegment *pSegment)
{
CHECKMINE;

	ushort*	vertexIds = pSegment->m_info.vertexIds;
	int		h;

Renderer ().SelectPen (penGray + 1);

for (int i = 0; i < 8; i++, vertexIds++)
	if (MAX_VERTEX >= (h = *vertexIds))
		Renderer ().Ellipse (vertexManager [h].m_screen.x, vertexManager [h].m_screen.y, 2, 2);
}

//--------------------------------------------------------------------------

bool CMineView::SelectWireFramePen (CSegment* pSegment)
{
if (pSegment->IsTagged ()) { 
	Renderer ().SelectPen (penOrange + 1); 
	return true;
	}

if (ViewFlag (eViewMineSpecial) && (m_viewOption != eViewTextured)) {
	switch (pSegment->m_info.function) {
		case SEGMENT_FUNC_PRODUCER:
		case SEGMENT_FUNC_SPEEDBOOST:
			Renderer ().SelectPen (penYellow + 1);
			return true;
		case SEGMENT_FUNC_REACTOR:
			Renderer ().SelectPen (penOrange + 1);
			return true;
		case SEGMENT_FUNC_REPAIRCEN:
			Renderer ().SelectPen (penLtBlue + 1);
			return true;
		case SEGMENT_FUNC_ROBOTMAKER:
		case SEGMENT_FUNC_EQUIPMAKER:
			Renderer ().SelectPen (penMagenta + 1);
			return true;
		case SEGMENT_FUNC_GOAL_BLUE:
		case SEGMENT_FUNC_TEAM_BLUE:
			Renderer ().SelectPen (penBlue + 1);
			return true;
		case SEGMENT_FUNC_GOAL_RED:
		case SEGMENT_FUNC_TEAM_RED:
			Renderer ().SelectPen (penRed + 1);
			return true;
		default:
			if (pSegment->m_info.props & SEGMENT_PROP_WATER)
				Renderer ().SelectPen (penMedBlue + 1);
			else if (pSegment->m_info.props & SEGMENT_PROP_LAVA)
				Renderer ().SelectPen (penMedRed + 1);
			else if (pSegment->m_info.props & SEGMENT_PROP_FOG)
				Renderer ().SelectPen (penDkGray + 1);
			else if (pSegment->IsTagged (short (-1))) 
				Renderer ().SelectPen (penGray + 1);
			else
				return false;
		return true;
		}
	}

if (pSegment->IsTagged (short (-1))) {
	Renderer ().SelectPen (penGray + 1); 
	return true;
	}

return false;
}

//--------------------------------------------------------------------------

void CMineView::DrawTaggedSegments (void) 
{
CHECKMINE;

	CSegment*	pSegment;
	short			xMax = ViewWidth ();
	short			yMax = ViewHeight ();
	short			i;
Renderer ().BeginRender (Renderer ().Type () == 0);
if (!Renderer ().Ortho ())
	glDepthFunc (GL_LEQUAL);
for (i = 0; i < segmentManager.Count (); i++) 
	if (SelectWireFramePen (pSegment = segmentManager.Segment (i)))
		DrawSegmentWireFrame (pSegment, false, true);
Renderer ().EndRender ();
Renderer ().BeginRender (true);
// draw a square around all marked points
Renderer ().SelectObject ((HBRUSH) GetStockObject (NULL_BRUSH));
#if 1
Renderer ().SelectPen (penOrange + 1);
#else
if (SelectMode (eSelectBlock)) 
	Renderer ().SelectPen (penRed + 1);
else
	Renderer ().SelectPen (penGold + 1);
#endif
for (i = 0; i < vertexManager.Count (); i++)
	if ((vertexManager.Status (i) & TAGGED_MASK) && vertexManager [i].InRange (xMax, yMax, Renderer ().Type ())) 
		Renderer ().Rectangle (vertexManager [i], 5, 5);
Renderer ().EndRender ();
}

//--------------------------------------------------------------------------

void CMineView::DrawCurrentSegment (CSegment *pSegment, bool bSparse)
{
CHECKMINE;

	short xMax = ViewWidth ();
	short yMax = ViewHeight ();
	short nSide = current->SideId ();
	short nLine = current->Point ();
	short	nPoint = current->Point ();
	CSide* pSide = pSegment->Side (nSide);
	short nVertices = pSide->VertexCount ();

if (Renderer ().Type ()) {
	BeginRender ();
	glDisable (GL_DEPTH_TEST);
	}
else {
	for (int i = 0; i < nVertices; i++)
		if (!vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (i)]].InRange (xMax, yMax, Renderer ().Type ()))
			return;
	BeginRender (true);
	}

Renderer ().SelectPen ((m_selectMode == SIDE_MODE) ? penRed + 1 : penGreen + 1);
for (int i = 0; i < nVertices; i++)
	DrawLine (pSegment, pSide->VertexIdIndex (i), pSide->VertexIdIndex (i + 1));
// draw current line
Renderer ().SelectPen ((m_selectMode == SIDE_MODE) ? penRed + 1 : penGold + 1);
DrawLine (pSegment, pSide->VertexIdIndex (nLine), pSide->VertexIdIndex (nLine + 1));
// draw a circle around the current point
Renderer ().SelectObject ((HBRUSH) GetStockObject (NULL_BRUSH));
Renderer ().SelectPen ((m_selectMode == SIDE_MODE) ? penRed + 1 : penGold + 1);
Renderer ().Ellipse (vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (nPoint)]], 4, 4);
EndRender ();
}

//--------------------------------------------------------------------------

void CMineView::DrawLine (CSegment* pSegment, short i1, short i2) 
{
CHECKMINE;
if (Renderer ().Ortho ()) {
	Renderer ().MoveTo (vertexManager [pSegment->m_info.vertexIds [i1]].m_screen.x, vertexManager [pSegment->m_info.vertexIds [i1]].m_screen.y);
	Renderer ().LineTo (vertexManager [pSegment->m_info.vertexIds [i2]].m_screen.x, vertexManager [pSegment->m_info.vertexIds [i2]].m_screen.y);
	}
else {
	Renderer ().MoveTo (vertexManager [pSegment->m_info.vertexIds [i1]]);
	Renderer ().LineTo (vertexManager [pSegment->m_info.vertexIds [i2]]);
	}
}
//--------------------------------------------------------------------------

void CMineView::SelectWallPen (CWall* pWall)
{
switch (pWall->Type ()) {
	case WALL_NORMAL:
		Renderer ().SelectPen (penLtGray + 1);
		break;
	case WALL_BLASTABLE:
		Renderer ().SelectPen (penLtGray + 1);
		break;
	case WALL_DOOR:
		switch(pWall->Info ().keys) {
			case KEY_NONE:
				Renderer ().SelectPen (penLtGray + 1);
				break;
			case KEY_BLUE:
				Renderer ().SelectPen (penBlue + 1);
				break;
			case KEY_RED:
				Renderer ().SelectPen (penRed + 1);
				break;
			case KEY_GOLD:
				Renderer ().SelectPen (penYellow + 1);
				break;
			default:
				Renderer ().SelectPen (penGray + 1);
			}
		break;
	case WALL_ILLUSION:
		Renderer ().SelectPen (penLtGray + 1);
		break;
	case WALL_OPEN:
		Renderer ().SelectPen (penLtGray + 1);
		break;
	case WALL_CLOSED:
		Renderer ().SelectPen (penLtGray + 1);
		break;
	default:
		Renderer ().SelectPen (penLtGray + 1);
	}
}

//--------------------------------------------------------------------------

void CMineView::DrawWalls (void) 
{
CHECKMINE;

	CWall*		walls = wallManager.Wall (0);
	CSegment*	segments = segmentManager.Segment (0);
	CSegment*	pSegment;
	CSide*		pSide;
	short			i,j;
	short			xMax = ViewWidth () * 2;
	short			yMax = ViewHeight () * 2;

for (i = 0; i < wallManager.WallCount (); i++) {
	if (walls [i].m_nSegment > segmentManager.Count ())
		continue;
	pSegment = segments + (int) walls [i].m_nSegment;
	if (!Visible (pSegment))
		continue;
	short nSide = walls [i].m_nSide;
	pSide = pSegment->Side (nSide);
	for (j = 0; j < 4; j++) {
		CVertex& v = vertexManager [pSegment->m_info.vertexIds [pSide->VertexIdIndex (j)]];
		if (IN_RANGE (v.m_screen.x, xMax) && IN_RANGE (v.m_screen.y, yMax))
			break;
		}
	if (j < 4) {
			CVertex center, normal, vector;
			CVertex arrowStartPoint, arrowEndPoint, arrow1Point, arrow2Point;

		center = segmentManager.CalcSideCenter (walls [i]);
		normal = segmentManager.CalcSideNormal (walls [i]);
		vector = center - normal;
		Renderer ().BeginRender ();
		vector.Transform (ViewMatrix ());
		vector.Project (ViewMatrix ());

		if (walls [i].Info ().nTrigger != NO_TRIGGER) {
			// calculate arrow points
			arrowStartPoint = center - (normal * 3);
			arrowStartPoint.Transform (ViewMatrix ());
			arrowStartPoint.Project (ViewMatrix ());
			arrowEndPoint = center + (normal * 3);
			arrowEndPoint.Transform (ViewMatrix ());
			arrowEndPoint.Project (ViewMatrix ());
			// direction toward center of line 0 from center
			CVertex vector = Average (*pSegment->Vertex (nSide, 0), *pSegment->Vertex (nSide, 1));
			vector -= center;
			vector.Normalize ();

			arrow1Point = (normal * 2);
			arrow1Point += center;
			arrow1Point += vector;
			arrow1Point.Transform (ViewMatrix ());
			arrow1Point.Project (ViewMatrix ());
			vector *= 2;
			arrow2Point = arrow1Point - vector;
			arrow2Point.Transform (ViewMatrix ());
			arrow2Point.Project (ViewMatrix ());
			}
		Renderer ().EndRender ();

		if (Renderer ().Type ()) {
			Renderer ().BeginRender (false);
			glEnable (GL_DEPTH_TEST);
			SelectWallPen (&walls [i]);
			glLineWidth (ViewOption (eViewTexturedWireFrame) ? 4.0f : 3.0f);
			for (j = 0; j < pSide->VertexCount (); j++) {
				Renderer ().MoveTo (vector);
				Renderer ().LineTo (*pSegment->Vertex (nSide, j));
				}
			if (walls [i].Info ().nTrigger != NO_TRIGGER) {
				// draw arrow
				Renderer ().MoveTo (arrowStartPoint);
				Renderer ().LineTo (arrowEndPoint);
				Renderer ().LineTo (arrow1Point);
				Renderer ().MoveTo (arrowEndPoint);
				Renderer ().LineTo (arrow2Point);
				}
			}
		else {
			Renderer ().BeginRender (true);
			SelectWallPen (&walls [i]);
			for (j = 0; j < pSide->VertexCount (); j++) {
				Renderer ().MoveTo (vector.m_screen.x, vector.m_screen.y);
				CVertex& v = *pSegment->Vertex (nSide, j);
				Renderer ().LineTo (v.m_screen.x, v.m_screen.y);
				}
			if (walls [i].Info ().nTrigger != NO_TRIGGER) {
				// draw arrow
				Renderer ().MoveTo (arrowStartPoint.m_screen.x, arrowStartPoint.m_screen.y);
				Renderer ().LineTo (arrowEndPoint.m_screen.x, arrowEndPoint.m_screen.y);
				Renderer ().LineTo (arrow1Point.m_screen.x, arrow1Point.m_screen.y);
				Renderer ().MoveTo (arrowEndPoint.m_screen.x, arrowEndPoint.m_screen.y);
				Renderer ().LineTo (arrow2Point.m_screen.x, arrow2Point.m_screen.y);
				}
			}
		Renderer ().EndRender ();
		}
	}
}
//--------------------------------------------------------------------------

void CMineView::DrawLights (void) 
{
CHECKMINE;

// now show variable lights
CViewMatrix* viewMatrix = Renderer ().ViewMatrix ();
CVariableLight* pLight = lightManager.VariableLight (0);

// find variable light from
Renderer ().BeginRender ();
pLight = lightManager.VariableLight (0);
for (INT i = 0; i < lightManager.Count (); i++, pLight++)
	if (Visible (segmentManager.Segment (pLight->m_nSegment)))
	   DrawOctagon (pLight->m_nSide, pLight->m_nSegment);
Renderer ().EndRender ();
}

//--------------------------------------------------------------------------

void CMineView::DrawOctagon (short nSide, short nSegment) 
{
CHECKMINE;

	short xMax = ViewWidth ();
	short yMax = ViewHeight ();

if (nSegment >= 0 && nSegment <= segmentManager.Count () && nSide >= 0 && nSide <= 5) {
	CSegment* pSegment = segmentManager.Segment (nSegment);
	CSide* pSide = pSegment->Side (nSide);
	short nVertices = pSide->VertexCount ();
	if (nVertices < 3)
		return;


	CVertex vCenter = pSegment->ComputeCenter (nSide);
	if (!vCenter.InRange (xMax, yMax, m_nRenderer)) 
		return;

	if (ViewMatrix ()->Distance (vCenter) <= MODEL_DISPLAY_LIMIT) {
		pSide->ComputeNormals (pSegment->m_info.vertexIds, pSegment->ComputeCenter ());
		vCenter += pSide->Normal ();
		Renderer ().Sprite (&textureManager.Icon (LIGHT_ICON), vCenter, 5.0, 5.0, false);
		}
	else {
		pSide->ComputeNormals (pSegment->m_info.vertexIds, pSegment->ComputeCenter (true), true);
		CVertex vertices [4];
		short nVertices = pSide->VertexCount ();
#if 1
		CDoubleVector vOffset = pSide->Normal ();
		for (short i = 0; i < nVertices; i++)
			vertices [i].m_view = pSegment->Vertex (nSide, i)->m_view + vOffset;
#else
		for (short i = 0; i < nVertices; i++)
			vertices [i] = *pSegment->Vertex (nSide, i);
		CDoubleVector vNormal = Normal (vertices [0], vertices [1], vertices [2]); // = pSide->Normal ();
		vertices [0] -= vCenter;
		vertices [0].Normalize ();
		vertices [3] = vCenter + vNormal;
		vertices [1] = Normal (vCenter, vertices [0], vertices [3]);
		vertices [0] *= 50.0;
		vertices [1] *= 50.0;
		vertices [2] = vCenter - vertices [0];
		vertices [3] = vCenter - vertices [1];
		vertices [0] += vCenter;
		vertices [1] += vCenter;
		CDoubleVector vOffset = vNormal * 10.0;
		for (short i = 0; i < nVertices; i++) {
			vertices [i].Transform (Renderer ().ViewMatrix ());
			vertices [i].m_view += vOffset;
			}
#endif
		glDisable (GL_CULL_FACE);
		Renderer ().TexturedPolygon (&textureManager.Icon (CIRCLE_ICON), null, null, vertices, nVertices, null);
		glEnable (GL_CULL_FACE);
		}
	}
}

//----------------------------------------------------------------------------

void CMineView::DrawTunnel (void) 
{
	CVertex points [4];
	CRenderer& renderer = DLE.MineView ()->Renderer ();

renderer.SelectPen (penRed + 1);
Renderer ().SelectObject ((HBRUSH)GetStockObject (NULL_BRUSH));
tunnelMaker.Create ();
renderer.BeginRender ();
points [1].Transform (ViewMatrix ());
points [1].Project (ViewMatrix ());
if (IN_RANGE (points [1].m_screen.x, m_viewMax.x) && IN_RANGE (points [1].m_screen.y, m_viewMax.y)) {
	points [0].Transform (ViewMatrix ());
	points [0].Project (ViewMatrix ());
	if (IN_RANGE (points [0].m_screen.x, m_viewMax.x) && IN_RANGE (points [0].m_screen.y, m_viewMax.y)){
		renderer.BeginRender (true);
		renderer.MoveTo (points [0].m_screen.x, points [0].m_screen.y);
		renderer.LineTo (points [1].m_screen.x, points [1].m_screen.y);
		renderer.Ellipse (points [1], 4, 4);
		renderer.EndRender ();
		renderer.BeginRender ();
		}
	}
points [2].Transform (ViewMatrix ());
points [2].Project (ViewMatrix ());
if (IN_RANGE (points [2].m_screen.x, m_viewMax.x) && IN_RANGE (points [2].m_screen.y, m_viewMax.y)){
	points [2].Transform (ViewMatrix ());
	points [2].Project (ViewMatrix ());
	if (IN_RANGE (points [3].m_screen.x, m_viewMax.x) && IN_RANGE (points [3].m_screen.y, m_viewMax.y)){
		renderer.BeginRender (true);
		renderer.MoveTo (points [3].m_screen.x, points [3].m_screen.y);
		renderer.LineTo (points [2].m_screen.x, points [2].m_screen.y);
		renderer.Ellipse (points [2].m_screen.x - 4, points [2].m_screen.y - 4, points [2].m_screen.x+4,  points [2].m_screen.y+4);
		renderer.EndRender ();
		renderer.BeginRender ();
		}
	}
#if 1
tunnelMaker.Draw (Renderer (), Pen (penRed), Pen (penBlue), ViewMatrix ());
#else
j = MAX_VERTICES;
for (h = tunnelMaker.Length () * 4, i = 0; i < h; i++) {
	vertexManager [--j].Transform (ViewMatrix ());
	vertexManager [--j].Project (ViewMatrix ());
	}
CSegment *pSegment = segmentManager.Segment (SEGMENT_LIMIT - 1);
renderer.BeginRender (renderer.Type () == 0);
renderer.SelectPen (penBlue + 1);
for (i = 0; i < tunnelMaker.Length (); i++, pSegment--)
	DrawSegmentWireFrame (pSegment, false, false, 1);
renderer.EndRender ();
#endif
}

//--------------------------------------------------------------------------
//			  DrawObject ()
//
// Changed: 0=normal,1=gray,2=black
//        if (nObject == (objectManager.Count ()
//        then its a secret return point)
//--------------------------------------------------------------------------

void CMineView::SelectObjectPen (CGameObject* pObject) 
{
if ((m_selectMode == OBJECT_MODE) && (pObject == current->Object ()))
	Renderer ().SelectPen (penRed + 1); // RED
else {
	switch(pObject->Type ()) {
		case OBJ_ROBOT: 
		case OBJ_CAMBOT: 
		case OBJ_EXPLOSION:
		case OBJ_MONSTERBALL:
			Renderer ().SelectPen (penMagenta + 1);
			break;
		case OBJ_SMOKE:
		case OBJ_EFFECT:
			Renderer ().SelectPen (penGreen + 1, 2);
			break;
		case OBJ_HOSTAGE: 
			Renderer ().SelectPen (penBlue + 1);
			break;
		case OBJ_PLAYER: 
			Renderer ().SelectPen (penGold + 1);
			break;
		case OBJ_WEAPON: 
			Renderer ().SelectPen (penDkGreen + 1);
			break;
		case OBJ_POWERUP: 
			Renderer ().SelectPen (penOrange + 1);
			break;
		case OBJ_REACTOR: 
			Renderer ().SelectPen (penLtGray + 1);
			break;
		case OBJ_COOP: 
			Renderer ().SelectPen (penGold + 1);
			break;
		default:
			Renderer ().SelectPen (penGreen + 1);
		}
	}
}

//--------------------------------------------------------------------------

void CMineView::DrawObject (short nObject) 
{
CHECKMINE;

	CGameObject*	pObject;
	CGameObject		tempObj;

if (nObject >= 0 && nObject < objectManager.Count ()) {
	pObject = objectManager.Object (nObject);
	if (!Visible (segmentManager.Segment (pObject->m_info.nSegment)))
		return;
	}
else {
	// secret return
	pObject = &tempObj;
	pObject->Type () = -1;
	pObject->m_info.renderType = RT_NONE;
	// theMine->secret_orient = Objects () [0]->orient;
	pObject->m_location.orient.m.rVec = -objectManager.SecretOrient ().m.rVec;
	pObject->m_location.orient.m.uVec =  objectManager.SecretOrient ().m.fVec;
	pObject->m_location.orient.m.fVec =  objectManager.SecretOrient ().m.uVec;
	// pObject->m_location.orient =  theMine->secret_orient;
	ushort nSegment = (ushort)objectManager.SecretSegment ();
	if (nSegment >= segmentManager.Count ())
		nSegment = 0;
	if (!Visible (segmentManager.Segment (nSegment)))
		return;
	CVertex center;
	pObject->Position () = segmentManager.CalcCenter (center, nSegment); // define pObject->position
	}

double d = (ViewOption (eViewTexturedWireFrame) || ViewOption (eViewTextured)) ? ViewMatrix ()->Distance (pObject->Position ()) : 1e30;
if (textureManager.Available (1) && pObject->HasPolyModel () && modelManager.Setup (pObject, m_renderer, DC ()) && ((nObject == current->ObjectId ()) || (d <= MODEL_DISPLAY_LIMIT))) {
	SelectObjectPen (pObject);
	if (pObject->IsInView (Renderer (), true)) { // only render if fully visible
		Renderer ().SelectObject ((HBRUSH) GetStockObject (BLACK_BRUSH));
		modelManager.Draw ();
		}
	}
else {
	if ((d > MODEL_DISPLAY_LIMIT) || !pObject->DrawSprite (Renderer ()) || (nObject == current->ObjectId ())) {
		SelectObjectPen (pObject);
		pObject->DrawArrow (Renderer (), (nObject == current->ObjectId ()));
		}
	}

if (((nObject == current->ObjectId ()) || (nObject == other->ObjectId ())) && pObject->IsInView (Renderer (), false))
	pObject->DrawHighlight (Renderer (), (nObject == current->ObjectId ()));
}

//--------------------------------------------------------------------------

void CMineView::DrawObjects (void) 
{
CHECKMINE;

if (!ViewObject ())
	return;

int i, j;

Renderer ().BeginRender ();
if (DLE.IsD2File ()) {
	// see if there is a secret exit trigger
	for(i = 0; i < triggerManager.WallTriggerCount (); i++)
	if (triggerManager.Trigger (i)->Type () == TT_SECRET_EXIT) {
		DrawObject ((short)objectManager.Count ());
		break; // only draw one secret exit
		}
	}
HiliteTarget ();
CGameObject *pObject = objectManager.Object (0);
for (i = objectManager.Count (), j = 0; i; i--, j++, pObject++)
	if (ViewObject (pObject))
		DrawObject (j);
Renderer ().EndRender ();
}

//--------------------------------------------------------------------------
// Determine the side edge the projection of which to the mine view is closest
// to the current mouse position.

bool CMineView::DrawSelectablePoint (void) 
{
if (!SelectMode (eSelectPoint))
	return false;

CRect viewport;
GetClientRect (viewport);

double minDist = 1e30;
short nNearestVertex = -1;

CDoubleVector mousePos (double (LastMousePos ().x), double (LastMousePos ().y), 0.0);

short nVertices = vertexManager.Count ();
for (short nVertex = 0; nVertex < nVertices; nVertex++) {
	CVertex& v = vertexManager [nVertex];
	if (!v.InRange (viewport.right, viewport.bottom, m_nRenderer))
		continue;
	double dist = Distance (mousePos, v.m_proj);
	if (minDist > dist) {
		minDist = dist;
		nNearestVertex = nVertex;
		}
	}
if (nNearestVertex < 0)
	return false;

Renderer ().BeginRender (true);
Renderer ().SelectPen (penMedBlue + 1, 1);
Renderer ().Ellipse (vertexManager [nNearestVertex], 8.0, 8.0);
Renderer ().EndRender ();

return true;
}

//--------------------------------------------------------------------------
// Determine the side edge the projection of which to the mine view is closest
// to the current mouse position.

bool CMineView::DrawSelectableEdge (void) 
{
if (!SelectMode (eSelectLine))
	return false;

CSegment* nearestSegment = null;
CSide* nearestSide = null;
short nNearestEdge = FindNearestLine (&nearestSegment, &nearestSide, false);
if (nNearestEdge < 0)
	return false;

nearest->Setup (segmentManager.Index (nearestSegment), nearestSegment->SideIndex (nearestSide), nNearestEdge, nNearestEdge);

Renderer ().BeginRender (true);
if (m_nRenderer) {
	glLineStipple (1, 0x00ff);  
	glEnable (GL_LINE_STIPPLE);
	}
Renderer ().SelectPen (penMedBlue + 1, 2);
CVertex* v1 = nearest->Segment ()->Vertex (nearest->m_nSide, nNearestEdge);
CVertex* v2 = nearest->Segment ()->Vertex (nearest->m_nSide, nNearestEdge + 1);
Renderer ().MoveTo (v1->m_screen.x, v1->m_screen.y);
Renderer ().LineTo (v2->m_screen.x, v2->m_screen.y);
if (m_nRenderer)
	glDisable (GL_LINE_STIPPLE);
Renderer ().SelectPen (penMedBlue + 1);
Renderer ().Ellipse (*v1, 8.0, 8.0);
Renderer ().Ellipse (*v2, 8.0, 8.0);
Renderer ().EndRender ();

return true;
}

//--------------------------------------------------------------------------
// Determine all sides in the projection of which  to the mine viewthe current 
// mouse position lies. Draw a circle in each side's center and a line from 
// each side vertex to the circle to mark it. Open sides have the circle 
// moved in by their normal * 2.0.

static inline double sqr (long v) { return double (v) * double (v); }

bool CMineView::DrawSelectableSides (void) 
{
if (!SelectMode (eSelectSide))
	return false;

CRect viewport;
GetClientRect (viewport);

if (!segmentManager.GatherSelectableSides (viewport, LastMousePos ().x, LastMousePos ().y, ViewFlag (eViewMineSkyBox), false))
	return false;

double minDist = 1e30;

CSegment* nearestSegment = null;
CSide* nearestSide = null;

for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
	CSegment* pSegment = segmentManager.Segment (pSide->GetParent ());
	CVertex& center = pSide->Center ();
	double dist = sqrt (sqr (LastMousePos ().x - center.m_screen.x) + sqr (LastMousePos ().y - center.m_screen.y));
#if 0
	if (dist > 64.0)
		continue;
#endif
	if (minDist > dist) {
		minDist = dist;
		nearestSegment = segmentManager.Segment (pSide->GetParent ());
		nearestSide = pSide;
		}
	}
nearest->Setup (segmentManager.Index (nearestSegment), nearestSegment->SideIndex (nearestSide));
if (nearest->m_nSegment < 0)
	return false;

if (m_nShowSelectionCandidates > 1) {
	Renderer ().BeginRender ();
	if (m_nRenderer) {
		glLineStipple (1, 0x00ff);  
		glEnable (GL_LINE_STIPPLE);
		glDepthFunc (GL_ALWAYS);
		}
	for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
		CSegment* pSegment = segmentManager.Segment (pSide->GetParent ());
		short nSide = pSegment->SideIndex (pSide);
		short nVertices = pSide->VertexCount ();
		Renderer ().SelectPen ((pSide == nearestSide) ? penGold + 1 : penMedBlue + 1);
		CVertex& center = pSide->Center ();
	#if 0
		double dist = sqrt (sqr (m_lastMousePos.x - center.m_screen.x) + sqr (m_lastMousePos.y - center.m_screen.y));
		if (dist > 64.0)
			continue;
	#endif
		if (m_nRenderer) 
			glLineStipple (1, 0x00ff);  
		if (Renderer ().Ortho ()) {
			for (int i = 0; i <= nVertices; i++) {
				CVertex* vertex = pSegment->Vertex (nSide, i);
				if (i)
					Renderer ().LineTo (vertex->m_screen.x, vertex->m_screen.y);
				else
					Renderer ().MoveTo (vertex->m_screen.x, vertex->m_screen.y);
				}
			}
		else {
			for (int i = 0; i <= nVertices; i++) {
				CVertex* vertex = pSegment->Vertex (nSide, i);
				if (i)
					Renderer ().LineTo (*vertex);
				else
					Renderer ().MoveTo (*vertex);
				}
			}

		if (!pSide->IsVisible ()) {
			CVertex normal = pSide->Normal (2);
			normal.Project (ViewMatrix ());
			center += normal;
			}
		if (m_nRenderer)
			glLineStipple (1, 0x3333);  
		for (int i = 0; i < nVertices; i++) {
			if (Renderer ().Ortho ()) {
				CVertex* vertex = pSegment->Vertex (nSide, i);
				CDoubleVector c (double (center.m_screen.x), double (center.m_screen.y), double (center.m_screen.z));
				CDoubleVector v (double (vertex->m_screen.x), double (vertex->m_screen.y), double (vertex->m_screen.z));
				c -= v;
				double m = c.Mag ();
				c *= (m - 8.0) / m;
				c += v;
				Renderer ().MoveTo (vertex->m_screen.x, vertex->m_screen.y);
				Renderer ().LineTo (long (Round (c.v.x)), long (Round (c.v.y)));
				}
			else {
				Renderer ().MoveTo (*pSegment->Vertex (nSide, i));
				Renderer ().LineTo (center);
				}
			}
		}
	Renderer ().EndRender ();
	}

if (m_nShowSelectionCandidates > 0) {
	Renderer ().BeginRender ();
	if (m_nRenderer)
		glDepthFunc (GL_ALWAYS);
	for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
		CSegment* pSegment = segmentManager.Segment (pSide->GetParent ());
		short nSide = pSegment->SideIndex (pSide);
		CVertex& center = pSide->Center ();
#if 0
		double dist = sqrt (sqr (m_lastMousePos.x - center.m_screen.x) + sqr (m_lastMousePos.y - center.m_screen.y));
		if (dist > 64.0)
			continue;
#endif
		Renderer ().SelectPen ((pSide == nearestSide) ? penGold + 1 : penMedBlue + 1, 1.5f);
		Renderer ().Ellipse (center, 5.0, 5.0);
		}
	Renderer ().EndRender ();
	}
return true;
}

//--------------------------------------------------------------------------
// Determine all segments in the projection of which to the mine view the current 
// mouse position lies. Draw a circle in each segment's center and a line from 
// each side vertex to the circle to mark it. Open sides have the circle 
// moved in by their normal * 2.0.

bool CMineView::DrawSelectableSegments (void) 
{
if (!SelectMode (eSelectSegment))
	return false;

CRect viewport;
GetClientRect (viewport);

if (!segmentManager.GatherSelectableSides (viewport, LastMousePos ().x, LastMousePos ().y, ViewFlag (eViewMineSkyBox), true))
	return false;

double minDist = 1e30;

CSegment* nearestSegment = null;
CSide* nearestSide = null;

short nSegment = -1;
for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
	if (nSegment == pSide->GetParent ())
		continue;
	CSegment* pSegment = segmentManager.Segment (nSegment = pSide->GetParent ());
	CVertex& center = pSegment->Center ();
	double dist = sqrt (sqr (LastMousePos ().x - center.m_screen.x) + sqr (LastMousePos ().y - center.m_screen.y));
#if 0
	if (dist > 64.0)
		continue;
#endif
	if (minDist > dist) {
		minDist = dist;
		nearestSegment = pSegment;
		nearestSide = pSide;
		}
	}
nearest->Setup (segmentManager.Index (nearestSegment), nearestSegment->SideIndex (nearestSide));
if (nearest->m_nSegment < 0)
	return false;

if (m_nShowSelectionCandidates > 1) {
	Renderer ().BeginRender ();
	if (m_nRenderer) {
		glLineStipple (1, 0x00ff);  
		glDepthFunc (GL_ALWAYS);
		glEnable (GL_LINE_STIPPLE);
		}
	nSegment = -1;
	for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
		if (nSegment == pSide->GetParent ())
			continue;
		CSegment* pSegment = segmentManager.Segment (nSegment = pSide->GetParent ());
		short nSide = pSegment->SideIndex (pSide);
		CVertex& center = pSegment->Center ();
#if 0
		double dist = sqrt (sqr (m_lastMousePos.x - center.m_screen.x) + sqr (m_lastMousePos.y - center.m_screen.y));
		if (dist > 64.0)
			continue;
#endif
		Renderer ().SelectPen ((pSide == nearestSide) ? penGold + 1 : penMedBlue + 1);
		if (m_nRenderer)
			glLineStipple (1, 0x00ff);  
		DrawSegmentWireFrame (pSegment);
		Renderer ().SelectPen ((pSide == nearestSide) ? penGold + 1 : penMedBlue + 1);
		if (m_nRenderer)
			glLineStipple (1, 0x3333);  
		for (int i = 0; i < 8; i++) {
			if (pSegment->VertexId (i) > MAX_VERTEX)
				continue;
			if (Renderer ().Ortho ()) {
				CVertex* vertex = pSegment->Vertex (i);
				CDoubleVector c (double (center.m_screen.x), double (center.m_screen.y), double (center.m_screen.z));
				CDoubleVector v (double (vertex->m_screen.x), double (vertex->m_screen.y), double (vertex->m_screen.z));
				c -= v;
				double m = c.Mag ();
				c *= (m - 8.0) / m;
				c += v;
				Renderer ().MoveTo (vertex->m_screen.x, vertex->m_screen.y);
				Renderer ().LineTo (long (Round (c.v.x)), long (Round (c.v.y)));
				}
			else {
				Renderer ().MoveTo (*pSegment->Vertex (nSide, i));
				Renderer ().LineTo (center);
				}
			}
		}
	Renderer ().EndRender ();
	}

if (m_nShowSelectionCandidates > 0) {
	Renderer ().BeginRender ();
	if (m_nRenderer)
		glDepthFunc (GL_ALWAYS);
	nSegment = -1;
	for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
		if (nSegment == pSide->GetParent ())
			continue;
		CSegment* pSegment = segmentManager.Segment (nSegment = pSide->GetParent ());
		short nSide = pSegment->SideIndex (pSide);
		CVertex& center = pSegment->Center ();
#if 0
		double dist = sqrt (sqr (m_lastMousePos.x - center.m_screen.x) + sqr (m_lastMousePos.y - center.m_screen.y));
		if (dist > 64.0)
			continue;
#endif
		Renderer ().SelectPen ((pSide == nearestSide) ? penGold + 1 : penMedBlue + 1, 1.5f);
		Renderer ().Ellipse (center, 5.0, 5.0);
		}
	Renderer ().EndRender ();
	}

return true;
}

//--------------------------------------------------------------------------

void CMineView::DrawHighlight (void) 
{
CHECKMINE;

	short	currSide, currPoint;
//	short i;
//	RECT rect;

if (segmentManager.Count ()== 0) 
	return;

// draw Objects ()
DrawObjects ();
//	if (/*!(preferences & PREFS_HIDE_TAGGED_BLOCKS) ||*/ SelectMode (eSelectBlock))
DrawTaggedSegments ();

Renderer ().BeginRender (Renderer ().Type () == 0);
// draw highlighted Segments () (other first, then current)
// use pointer comparison so we don't draw the wrong selection if current/other are on the same segment and side
if (current == &selections [0]) {
	if (selections [0].SegmentId () != selections [1].SegmentId ())
		DrawSegmentHighlighted (selections [1].SegmentId (), selections [1].SideId (), selections [1].Edge (), selections [1].Point ());
	DrawSegmentHighlighted (selections [0].SegmentId (), selections [0].SideId (), selections [0].Edge (), selections [0].Point ());
	}
else {
	if (selections [0].SegmentId () != selections [1].SegmentId ())
		DrawSegmentHighlighted (selections [0].SegmentId (), selections [0].SideId (), selections [0].Edge (), selections [0].Point ());
	DrawSegmentHighlighted (selections [1].SegmentId (), selections [1].SideId (), selections [1].Edge (), selections [1].Point ());
	}
Renderer ().EndRender ();

if (ViewFlag (eViewMineWalls))
	DrawWalls ();
if (ViewFlag (eViewMineLights))
	DrawLights ();

nearest->SetSegmentId (-1);
nearest->SetSideId (-1);
nearest->SetEdge (-1);

if ((m_inputHandler.MouseState () == eMouseStateSelect && m_inputHandler.HasMouseMovedInCurrentState ()) ||
	m_inputHandler.MouseState () == eMouseStateApplySelect) {
	if (DrawSelectablePoint ())
		;
	else if (DrawSelectableEdge ())
		;
	else if (DrawSelectableSides () || DrawSelectableSegments ()) {
		Renderer ().BeginRender (false);
		if (m_nRenderer) {
			glLineStipple (1, 0x00ff);  
			glEnable (GL_LINE_STIPPLE);
			glDepthFunc (GL_ALWAYS);
			}
		DrawSegmentHighlighted (selections [2].m_nSegment, selections [2].m_nSide, DEFAULT_EDGE, DEFAULT_POINT);
		if (m_nRenderer)
			glDisable (GL_LINE_STIPPLE);
		Renderer ().EndRender ();
		}
	}

tunnelMaker.Draw (Renderer (), Pen (penRed), Pen (penBlue), ViewMatrix ());
textureProjector.DrawProjectGuides (Renderer (), ViewMatrix ());

*message = '\0';
#if 0
if (preferences & PREFS_SHOW_POINT_COORDINATES) 
	{
   strcat_s (message, sizeof (message), "  point (x, y,z): (");
   short vertex = segmentManager.Segment (0) [current->SegmentId ()].m_info.vertexIds [sideVertexTable [current->SideId ()][current->Point ()]];
	char	szCoord [20];
	sprintf_s (szCoord, sizeof (szCoord), "%1.4f,%1.4f,%1.4f)", 
				  vertexManager.Vertex (vertex)->v.x, vertexManager.Vertex (vertex)->v.y, vertexManager.Vertex (vertex)->v.z);
	strcat_s (message, sizeof (message), szCoord);
	}
else 
#endif
	{
   // calculate segment size (length between center point of opposing sides)
	strcat_s (message, sizeof (message), "  segment size: ");
	CDoubleVector center1, center2;
   double length;
   center1 = segmentManager.CalcSideCenter (CSideKey (current->SegmentId (), 0));
	center2 = segmentManager.CalcSideCenter (CSideKey (current->SegmentId (), 2));
   length = Distance (center1, center2);
	sprintf_s (message + strlen (message), sizeof (message) - strlen (message), "%.1f", (double) length);
	strcat_s (message, sizeof (message), " x ");
   center1 = segmentManager.CalcSideCenter (CSideKey (current->SegmentId (), 1));
   center2 = segmentManager.CalcSideCenter (CSideKey (current->SegmentId (), 3));
   length = Distance (center1, center2);
   sprintf_s (message + strlen (message), sizeof (message) - strlen (message), "%.1f", (double) length);
	strcat_s (message, sizeof (message), " x ");
   center1 = segmentManager.CalcSideCenter (CSideKey (current->SegmentId (), 4));
   center2 = segmentManager.CalcSideCenter (CSideKey (current->SegmentId (), 5));
   length = Distance (center1, center2);
	sprintf_s (message + strlen (message), sizeof (message) - strlen (message), "%.1f", (double) length);
	}
strcat_s (message, sizeof (message), ",  segment:");
_itoa_s (current->SegmentId (), message + strlen (message), sizeof (message) - strlen (message), 10);
strcat_s (message, sizeof (message), " side:");
_itoa_s ((currSide = current->SideId ()) + 1, message + strlen (message), sizeof (message) - strlen (message), 10);
strcat_s (message, sizeof (message), " point:");
_itoa_s (currPoint = current->Point (), message + strlen (message), sizeof (message) - strlen (message), 10);
strcat_s (message, sizeof (message), " vertex:");
_itoa_s (current->Segment ()->m_info.vertexIds [current->Side ()->VertexIdIndex (currPoint)], message + strlen (message), sizeof (message) - strlen (message), 10);

strcat_s (message, sizeof (message), ",  textures:");
strcat_s (message, sizeof (message), " 1st:");
_itoa_s (current->Side ()->BaseTex (), message + strlen (message), sizeof (message) - strlen (message), 10);
strcat_s (message, sizeof (message), " 2nd:");
_itoa_s (current->Side ()->OvlTex (0), message + strlen (message), sizeof (message) - strlen (message), 10);

strcat_s (message, sizeof (message), ",  zoom:");
double zoom = log (10 * Scale ().v.x) / log (zoomScales [m_nRenderer]);
if (zoom > 0) 
	zoom += 0.5;
else
	zoom -= 0.5;
sprintf_s (message + strlen (message), sizeof (message) - strlen (message),  "%1.2f", zoom);
STATUSMSG (message);
}

//------------------------------------------------------------------------------


//eof
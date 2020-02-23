// MineView.select.cpp: element selection UI functions


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
	CGameObject*	pObject, tempObj;
	short				nClosestObj;
	short				i;
	double			radius, closestRadius;

// default to object 0 but set radius very large
nClosestObj = 0;
closestRadius = 1.0e30;

// if there is a secret exit, then enable it in search
int enable_secret = false;
if (DLE.IsD2File ())
	for(i = 0; i < (short) triggerManager.WallTriggerCount (); i++)
		if (triggerManager.Trigger (i)->Type () == TT_SECRET_EXIT) {
			enable_secret = true;
			break;
			}

for (i = 0; i < objectManager.Count () + (enable_secret ? 1 : 0); i++) {
	BOOL drawable = false;
	// define temp object type and position for secret object selection
	if (i == objectManager.Count () && DLE.IsD2File () && enable_secret) {
		pObject = &tempObj;
		pObject->Type () = OBJ_PLAYER;
		// define pObject->position
		CVertex center;
		pObject->Position () = segmentManager.CalcCenter (center, (short) objectManager.SecretSegment ());
		}
	else
		pObject = objectManager.Object (i);
#if 0
	switch(pObject->Type ()) {
		case OBJ_WEAPON:
			if (ViewObject (eViewObjectsPowerups | eViewObjectsWeapons)) {
				drawable = true;
				}
		case OBJ_POWERUP:
			if (ViewObject (powerupTypeTable [pObject->Id ()])) {
				drawable = true;
				}
			break;
		default:
			if(ViewObject (1<<pObject->Type ()))
				drawable = true;
		}
	if (drawable) 
#else
	if (ViewObject (pObject))
#endif
		{
		// translate object's position to screen coordinates
		CVertex v = pObject->Position ();
		Renderer ().BeginRender ();
		v.Transform (ViewMatrix ());
		v.Project (ViewMatrix ());
		Renderer ().EndRender ();
		// calculate radius^2 (don't bother to take square root)
		double dx = (double) v.m_screen.x - (double) xMouse;
		double dy = (double) v.m_screen.y - (double) yMouse;
		radius = dx * dx + dy * dy;
	// check to see if this object is closer than the closest so far
		if (radius < closestRadius) {
			nClosestObj = i;
			closestRadius = radius;
			}
		}
	}

// unhighlight current object and select next object
i = current->ObjectId ();
RefreshObject(i, nClosestObj);
}

//-----------------------------------------------------------------------------
// Find the nearest textured segment side the user had clicked on.
// First try to read the segment and side secondary render target from the renderer
// If that fails, cast a ray through the mouse position from the near to the 
// far plane and find the nearest textured segment side intersected by the ray
// Disabled because the new side and segment selection method is more flexible
// when it comes to selecting open (untextured) sides.

short CMineView::FindSelectedTexturedSide (long xMouse, long yMouse, short& nSide)
{
short nSegment;
if (m_viewOption == eViewTextured || m_viewOption == eViewTexturedWireFrame) {
	int nResult = Renderer ().GetSideKey (xMouse, yMouse, nSegment, nSide);
	if (nResult == 1)
		if (!segmentManager.Segment (nSegment)->m_info.bTunnel)
			return nSegment;
		else
			return -1;
	if (nResult == 0)
		return -1;
	}

CVertex p1, p2;
p2.m_proj.v.x = xMouse;
p2.m_proj.v.y = yMouse;
p1.m_proj.v.z = -1.0;
p2.m_proj.v.z = 1.0;
CPoint viewCenter (0, 0);
Renderer ().BeginRender ();
Renderer ().ViewMatrix ()->Unproject (p2, p2.m_proj);
Renderer ().EndRender ();
p1.Clear ();
CDoubleVector mouseDir = p2;
mouseDir.Normalize ();

double minDist = 1e30;
short nMinSeg = -1;
short nMinSide = -1;
short nSegments = segmentManager.Count ();
Renderer ().Project ();
segmentManager.ComputeNormals (true, true);

bool bSkyBox = ViewFlag (eViewMineSkyBox);

#pragma omp parallel for if (nSegments > 15)
for (short nSegment = 0; nSegment < nSegments; nSegment++) {
	CSegment* pSegment = segmentManager.Segment (nSegment);
	if (!bSkyBox && (pSegment->Function () == SEGMENT_FUNC_SKYBOX))
		continue;
	if (pSegment->m_info.bTunnel)
		continue;
	CSide* pSide = pSegment->Side (0);
	for (short nSide = 0; nSide < 6; nSide++, pSide++) {
		double d;
		if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
			continue;
		if (!pSide->IsVisible ())
			continue;
		if ((d = Dot (mouseDir, pSide->m_vNormal [0])) > 0.0)
			continue; // looking at the back of the side
		d = pSide->LineHitsFace (&p1, &p2, pSegment->m_info.vertexIds, minDist, true);
		if (d < 0.0)
			continue;
#pragma omp critical
			{
			if (minDist > d) {
				minDist = d;
				nMinSeg = nSegment;
				nMinSide = nSide;
				}
			}
		}
	}
nSide = nMinSide;
return nMinSeg;
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
	short nSide, nSegment = FindSelectedTexturedSide (xMouse, yMouse, nSide);
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

short CMineView::FindNearestLine (CSegment** nearestSegment, CSide** nearestSide, bool bCurrentSideOnly)
{
short nNearestEdge = -1;
double minDist = 1e30;
CRect viewport;
GetClientRect (viewport);

#if 1
if (bCurrentSideOnly) {
	short nEdge = current->Side ()->NearestEdge (viewport, LastMousePos ().x, LastMousePos ().y, current->Segment ()->m_info.vertexIds, minDist);
	if (nEdge >= 0) {
		*nearestSegment = current->Segment ();
		*nearestSide = current->Side ();
		nNearestEdge = nEdge;
		}
	}
else {
	short nSegments = segmentManager.Count ();
	for (short nSegment = 0; nSegment < nSegments; nSegment++) {
		CSegment* pSegment = segmentManager.Segment (nSegment);
		bool bSegmentSelected = false;
		CSide* pSide = pSegment->Side (0);
		for (short nSide = 0; nSide < 6; nSide++, pSide++) {
			if (pSide->Shape () > SIDE_SHAPE_TRIANGLE)
				continue;
			short nEdge = pSide->NearestEdge (viewport, LastMousePos ().x, LastMousePos ().y, pSegment->m_info.vertexIds, minDist);
			if (nEdge >= 0) {
				*nearestSegment = pSegment;
				*nearestSide = pSide;
				nNearestEdge = nEdge;
				}
			}
		}
	}
#else
if (!segmentManager.GatherSelectableSides (viewport, LastMousePos ().x, LastMousePos ().y))
	return false;

for (CSide* pSide = segmentManager.SelectedSides (); pSide; pSide = pSide->GetLink ()) {
	CSegment* pSegment = segmentManager.Segment (pSide->GetParent ());
	short nEdge = pSide->NearestEdge (viewport, LastMousePos ().x, LastMousePos ().y, pSegment->m_info.vertexIds, minDist);
	if (nEdge >= 0) {
		nearestSegment = pSegment;
		nearestSide = pSide;
		nNearestEdge = nEdge;
		}
	}
#endif

return nNearestEdge;
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
	nEdge = FindNearestLine (&pSegment, &pSide, m_inputHandler.MouseState () != eMouseStateApplySelect);
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
	if (!pVertex->InRange (xMax, yMax, m_nRenderer))
		continue;
	double d = Distance (clickPos, pVertex->m_proj);
#else
	CVertex v = *pVertex;
	if (!v.InRange (xMax, yMax, m_nRenderer))
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

Wrap (current->SegmentId (), dir, 0, segmentManager.Count () - 1);
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
DrawHighlight ();
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

if (segmentManager.BackSide (opp) == null)
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
	Wrap (newObject, dir, 0, (short) objectManager.Count () - 1);
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
	case eSelectPoint:
		NextPoint (dir);
		break;
	case eSelectLine:
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

//------------------------------------------------------------------------------
//eof
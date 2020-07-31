// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"

double zoomScales [2] = {1.2, 1.1};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void CMineView::TogglePerspective (void)
{
SetPerspective (!Perspective ());
}

//------------------------------------------------------------------------------

void CMineView::OverridePerspective (bool bEnable, int nPerspective)
{
	int nPerspectiveToSet = bEnable ? nPerspective : appSettings.m_nPerspective;

if (Renderer ().SetPerspective (nPerspectiveToSet, true))
	Refresh ();
}

//------------------------------------------------------------------------------

int CMineView::FitToView (void)
{
DelayRefresh (true);
m_presenter.FitToView();
DelayRefresh (false);
Refresh ();
return 1;
}

//------------------------------------------------------------------------------

void CMineView::Rotate (char direction, double angle)
{
	static double a = 0;

int i = direction - 'X';
if ((i < 0) || (i > 2))
	return;
angle *= DLE.MineView ()->ViewMoveRate ();
ViewMatrix ()->Rotate (direction, -2 * angle); // * ((double) DLE.MineView ()->MineMoveRate () / 0x10000L));
a += Degrees (2 * angle);
if (a < -360)
	a += 360;
else if (a > 360)
	a -= 360;
sprintf_s (message, sizeof (message), "ROTATE (%1.2f°)", a);
INFOMSG (message);
Refresh (false);
}

//------------------------------------------------------------------------------

void CMineView::CenterOnObject (void)
{
CHECKMINE;

CDlcDoc* pDoc = GetDocument();
ASSERT_VALID(pDoc);
if (!pDoc) 
	return;

SetCenter (current->Object ()->Position (), 2);
Refresh (false);
}

//------------------------------------------------------------------------------

void CMineView::CenterOnSegment (void)
{
CHECKMINE;

current->Segment ()->ComputeCenter ();
SetCenter (current->Segment ()->Center (), 1);
Refresh (false);
}

//------------------------------------------------------------------------------

void CMineView::AlignViewerWithSide (void)
{
if (Perspective ()) {
	CSegment* pSegment = current->Segment ();
	CSide* pSide = current->Side ();
	pSide->ComputeNormals (pSegment->m_info.vertexIds, pSegment->ComputeCenter ());
	CDoubleMatrix r;
	r.m.fVec = pSide->Normal (2);
	r.m.fVec.Negate ();
	short nSide = current->SideId ();
	short nEdge = current->Edge ();
	r.m.rVec = *pSegment->Vertex (nSide, nEdge + 1) - *pSegment->Vertex (nSide, nEdge);
	r.m.rVec.Normalize ();
	pSegment->ComputeCenter (nSide);
	r.m.uVec = Normal (pSide->Center (), r.m.fVec, r.m.rVec);
	//r.m.uVec.Negate ();
#if 1//def NDEBUG
	ViewMatrix ()->Stuff (r);
#else
	Rotation () = r.Angles ();
#	if 1
	ViewMatrix ()->Setup (Translation (), Scale (), Rotation ());
#	else
	ViewMatrix ()->Rotation ().Clear ();
	ViewMatrix ()->Setup (Translation (), Scale (), ViewMatrix ()->Rotation ());
	ViewMatrix ()->Rotate ('Z', Rotation ().v.z);
	ViewMatrix ()->Rotate ('Y', Rotation ().v.y);
	ViewMatrix ()->Rotate ('X', Rotation ().v.x);
#	endif
#endif
	SetCenter (pSegment->Center (), 1);
	Invalidate (FALSE);
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


//eof
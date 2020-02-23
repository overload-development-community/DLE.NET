#include <stdlib.h>

#include "mine.h"
#include "dle-xp.h"
#include "frustum.h"

//------------------------------------------------------------------------------

#define COMPUTE_TYPE 0

static int planeVerts [6][4] = {
	{0,1,2,3},{0,1,5,4},{1,2,6,5},{2,3,7,6},{0,3,7,4},{4,5,6,7}
	};

void CFrustum::Setup (CRect viewport, double fov)
{
#if 1
double h = double (tan (fov * PI / 180.0));
double w = double (h * double (viewport.Width ()) / double (viewport.Height ()));
#else
double h = double (viewport.Height ()) * 0.5;
double w = double (viewport.Width ()) * 0.5;
#endif
double n = double (1.0);
double f = double (50000.0);
double m = f * 0.5f;
double r = f / n;

#define ln -w
#define rn +w
#define tn -h
#define bn +h

#define lf (ln * r)
#define rf (rn * r)
#define tf (tn * r)
#define bf (bn * r)

m_corners [0].Set (ln, bn, n);
m_corners [1].Set (ln, tn, n);
m_corners [2].Set (rn, tn, n);
m_corners [3].Set (rn, bn, n);
m_corners [4].Set (lf, bf, f);
m_corners [5].Set (lf, tf, f);
m_corners [6].Set (rf, tf, f);
m_corners [7].Set (rf, bf, f);
m_centers [0].Set (0.0, 0.0, n);
m_centers [1].Set (lf * 0.5, 0.0, m);
m_centers [2].Set (0.0, tf * 0.5, m);
m_centers [3].Set (rf * 0.5, 0.0, m);
m_centers [4].Set (0.0, bf * 0.5, m);
m_centers [5].Set (0.0, 0.0, f);


CVertex center;
for (int i = 0; i < 8; i++) 
	center += m_corners [i];
center /= 8.0;

m_centers [0].Clear ();
m_normals [0].Set (0.0, 0.0, 1.0);
for (int i = 0; i < 6; i++) {
	m_normals [i] = Normal (m_corners [planeVerts [i][1]], m_corners [planeVerts [i][2]], m_corners [planeVerts [i][3]]);
	for (int j = 0; j < 4; j++)
		m_centers [i] += m_corners [planeVerts [i][j]];
	m_centers [i] /= 4.0;
	CDoubleVector v = center - m_centers [i];
	v.Normalize ();
	if (Dot (v, m_normals [i]) < 0.0)
		m_normals [i].Negate ();
	}
}

//------------------------------------------------------------------------------
// Check whether the frustum intersects with a face defined by side *pSide.

bool CFrustum::Contains (short nSegment, short nSide)
{
	static int lineVerts [12][2] = {
		{0,1}, {1,2}, {2,3}, {3,0}, 
		{4,5}, {5,6}, {6,7}, {7,4},
		{0,4}, {1,5}, {2,6}, {3,7}
	};

	CSegment* pSegment = segmentManager.Segment (nSegment);
	CSide* pSide = pSegment->Side (nSide);
	int i, j, nInside = 0, nOutside [4] = {0, 0, 0, 0}, nVertices = pSide->VertexCount ();
	CVertex*	points [4];
	CDoubleVector intersection;

for (i = 0; i < nVertices; i++) {
	points [i] = pSegment->Vertex (nSide, i);
	}

// check whether all vertices of the face are at the back side of at least one frustum plane,
// or if at least one is at at least on one frustum plane's front sides
for (i = 0; i < 6; i++) {
	int nPtInside = 4;
	int bPtInside = 1;
	CDoubleVector& c = m_centers [i];
	CDoubleVector& n = m_normals [i];
	for (j = 0; j < nVertices; j++) {
		CDoubleVector v = points [j]->m_view - c;
		v = Normalize (v);
		if (Dot (n, v) < 0.0) {
			if (!--nPtInside)
				return false;
			++nOutside [j];
			bPtInside = 0;
			}
		}
	nInside += bPtInside;
	}

if (nInside == 6)
	return true; // face completely contained
for (j = 0; j < nVertices; j++) 
	if (!nOutside [j])
		return true; // some vertex inside frustum

// check whether the frustum intersects with the face
// to do that, compute the intersections of all frustum edges with the plane(s) spanned up by the face (two planes if face not planar)
// if an edge intersects, check whether the intersection is inside the face
// since the near plane is at 0.0, only 8 edges of 5 planes need to be checked
for (i = 0; i < 12; i++)
	if (pSide->LineHitsFace (&m_corners [lineVerts [i][0]], &m_corners [lineVerts [i][1]], pSegment->m_info.vertexIds, 0.0, true))
		return true;
return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//eof


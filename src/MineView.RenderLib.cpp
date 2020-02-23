// render.cpp

#include "stdafx.h"
#include <math.h>

#include "types.h"
#include "matrix.h"
#include "global.h"
#include "mine.h"
#include "segment.h"
#include "GameObject.h"
#include "dle-xp.h"
#include "dlcdoc.h"
#include "MainFrame.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "mineview.h"

extern ubyte sideVertexTable [6][4];

#if 0

//------------------------------------------------------------------------------
// Compute intersection of perpendicular to p1,p2 through p3 with p1,p2.

long PointLineIntersection (long& x0, long& y0, long x1, long y1, long x2, long y2, long x3, long y3)
{
long dx = x2 - x1;
long dy = y2 - y1;
double u = (x3 - x1) * dx + (y3 - y1) * dy;
u /= (double) dx * (double) dx + (double) dy * (double) dy;
x0 = (long) (x1 + u * dx + 0.5);
y0 = (long) (y1 + u * dy + 0.5);
return (long) (_hypot ((double) (x3 - x0), (double) (y3 - y0)) + 0.5);
}

//------------------------------------------------------------------------------

inline double Dot (CLongVector& v0, CLongVector& v1)
{
return (double) v0.x * (double) v1.x + (double) v0.y * (double) v1.y;
}

//------------------------------------------------------------------------------

static bool PointIsInTriangle2D (CLongVector& A, CLongVector& B, CLongVector& C, CLongVector& P)
{
CLongVector v0 = {C.x - A.x, C.y - A.y};
CLongVector v1 = {B.x - A.x, B.y - A.y};
CLongVector v2 = {P.x - A.x, P.y - A.y};

// Compute dot products
double dot00 = Dot (v0, v0);
double dot01 = Dot (v0, v1);
double dot02 = Dot (v0, v2);
double dot11 = Dot (v1, v1);
double dot12 = Dot (v1, v2);

// Compute barycentric coordinates
double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
// Check if point is in triangle
return (u > 0.0) && (v > 0.0) && (u + v < 1.0);
}

//------------------------------------------------------------------------------
// v1, v2, v3: triangle corners (z known)
// v0: reference point (z needed)

inline depthType InterpolateZ (CLongVector& v0, CLongVector& v1, CLongVector& v2, CLongVector& v3)
{
	CLongVector vi;

int i = LineLineIntersection (vi, v1, v2, v3, v0);
depthType zi = (depthType) (v1.z + (v2.z - v1.z) * _hypot (vi.x - v1.x, vi.y - v1.y) / _hypot (v2.x - v1.x, v2.y - v1.y));
return (depthType) (v3.z + (zi - v3.z) * _hypot (v0.x - v3.x, v0.y - v3.y) / _hypot (vi.x - v3.x, vi.y - v3.y));
}

//------------------------------------------------------------------------------

inline depthType CMineView::Z (CTexture& tex, CLongVector* a, int x, int y)
{
	CLongVector v0 = {x, y};

return InterpolateZ (v0, a [0], a [2], a [PointIsInTriangle2D (a [0], a [1], a [2], v0) ? 1 : 3]);
}

#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

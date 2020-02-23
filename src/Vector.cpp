

#include "stdafx.h"

#include <math.h>
#include "define.h"
#include "types.h"
#include "mine.h"
#include "global.h"
#include "matrix.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#if 0
int CFixVector::Read (CFileManager* fp) 
{ 
x = fp->ReadInt32 ();
y = fp->ReadInt32 ();
z = fp->ReadInt32 ();
return 1;
}

void CFixVector::Write (CFileManager* fp) 
{ 
WriteInt32 (x, fp);
WriteInt32 (y, fp);
WriteInt32 (z, fp);
}
#endif
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

CFixVector::CFixVector (CDoubleVector& _v) { 
	v.x = D2X (_v.v.x), v.y = D2X (_v.v.y), v.z = D2X (_v.v.z); 
}

// -----------------------------------------------------------------------------
//  Rotates a vertex around a center point perpendicular to direction vector.
// -----------------------------------------------------------------------------

void CFixVector::Rotate (CFixVector& origin, CFixVector& axis, double angle) 
{

  double				zSpin, ySpin, h;
  CDoubleVector	v0, v1, v2, v3, vn;

  // translate coordanites to origin
v0 = CDoubleVector (*this - origin);
vn = CDoubleVector (axis - origin);

// calculate angles to normalize direction
// spin on z axis to get into the x-z plane
zSpin = (vn.v.y == vn.v.x) ? PI/4 : atan2 (vn.v.y, vn.v.x);
h = vn.v.x * cos (zSpin) + vn.v.y * sin (zSpin);
// spin on y to get on the x axis
ySpin = (vn.v.z == h) ? PI/4 : -atan2(vn.v.z, h);

// normalize vertex (spin on z then y)
v1.Set (v0.v.x * cos (zSpin) + v0.v.y * sin (zSpin), v0.v.y * cos (zSpin) - v0.v.x * sin (zSpin), v0.v.z);
v2.Set (v1.v.x * cos (ySpin) - v1.v.z * sin (ySpin), v1.v.y, v1.v.x * sin (ySpin) + v1.v.z * cos (ySpin));
v3.Set (v2.v.x, v2.v.y * cos (angle) + v2.v.z * sin (angle), v2.v.z * cos (angle) - v2.v.y * sin (angle));
// spin back in negative direction (y first then z)
ySpin = -ySpin;
v2.Set (v3.v.x * cos (ySpin) - v3.v.z * sin (ySpin), v3.v.y, v3.v.x * sin (ySpin) + v3.v.z * cos (ySpin));
zSpin = -zSpin;
v1.Set (v2.v.x * cos (zSpin) + v2.v.y * sin (zSpin), v2.v.y * cos (zSpin) - v2.v.x * sin (zSpin), v2.v.z);

v1 += origin;
*this = CFixVector (v1);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

CDoubleVector::CDoubleVector (CFixVector _v) { 
	v.x = X2D (_v.v.x), v.y = X2D (_v.v.y), v.z = X2D (_v.v.z); 
}

// -----------------------------------------------------------------------------
//  Rotates a vertex around a center point perpendicular to direction vector.
// -----------------------------------------------------------------------------

void CDoubleVector::Rotate (CDoubleVector origin, CDoubleVector axis, double angle) 
{

  double				zSpin, ySpin, h;
  CDoubleVector	v0, v1, v2, v3, vn;

  // translate coordinates to origin
v0 = CDoubleVector (*this - origin);
vn = CDoubleVector (axis - origin);

// calculate angles to normalize direction
// spin on z axis to get into the x-z plane
zSpin = (vn.v.y == vn.v.x) ? PI/4 : atan2 (vn.v.y, vn.v.x);
h = vn.v.x * cos (zSpin) + vn.v.y * sin (zSpin);
// spin on y to get on the x axis
ySpin = (vn.v.z == h) ? PI/4 : -atan2(vn.v.z, h);

// normalize vertex (spin on z then y)
v1.Set (v0.v.x * cos (zSpin) + v0.v.y * sin (zSpin), v0.v.y * cos (zSpin) - v0.v.x * sin (zSpin), v0.v.z);
v2.Set (v1.v.x * cos (ySpin) - v1.v.z * sin (ySpin), v1.v.y, v1.v.x * sin (ySpin) + v1.v.z * cos (ySpin));
v3.Set (v2.v.x, v2.v.y * cos (angle) + v2.v.z * sin (angle), v2.v.z * cos (angle) - v2.v.y * sin (angle));
// spin back in negative direction (y first then z)
ySpin = -ySpin;
v2.Set (v3.v.x * cos (ySpin) - v3.v.z * sin (ySpin), v3.v.y, v3.v.x * sin (ySpin) + v3.v.z * cos (ySpin));
zSpin = -zSpin;
v1.Set (v2.v.x * cos (zSpin) + v2.v.y * sin (zSpin), v2.v.y * cos (zSpin) - v2.v.x * sin (zSpin), v2.v.z);

v1 += origin;
*this = v1;
}

// -----------------------------------------------------------------------------  

void CDoubleVector::Rotate (CDoubleVector axis, double angle)
{
	CDoubleVector	vs2;

axis.Normalize ();
double a = Dot (axis, *this);
CDoubleVector vp = axis * a;
CDoubleVector vs = *this - vp;
double l = Mag ();
if (l != 0.0) { 
	CDoubleVector e1 = vs / l; 
	CDoubleVector e2 = CrossProduct (axis, e1);
	e1 *= (cos (angle) * l);
	e2 *= (sin (angle) * l);
	vs = e1 + e2;
	}
*this = vp + vs;
}
 
// -----------------------------------------------------------------------------

CDoubleVector& PointLineIntersection (CDoubleVector& intersection, const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2) 
{
	CDoubleVector	d31, d21;
	double			m, u;

d21 = p2 - p1;
m = (double) fabs (d21.SqrMag ());
if (m == 0.0)
	intersection = p1;
else {
	d31 = p0 - p1;
	u = Dot (d31, d21);
	u /= m;
	if (u < 0.0)
		intersection = p1;
	else if (u > 1.0)
		intersection = p2;
	else
		intersection = p1 + d21 * u;
	}
return intersection;
}

// -----------------------------------------------------------------------------

double PointLineDistance (const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2) 
{
	CDoubleVector intersection;

return Distance (p0, PointLineIntersection (intersection, p0, p1, p2));
}

//	-----------------------------------------------------------------------------

int PlaneLineIntersection (CDoubleVector& intersection, CDoubleVector* vPlane, CDoubleVector* vNormal, CDoubleVector* p0, CDoubleVector* p1)
{
CDoubleVector u = *p1;
u -= *p0;
double d = -Dot (*vNormal, u);
if ((d > -1e-10) && (d < 1e-10)) {// ~ parallel
	return -1;
	}
CDoubleVector w = *p0;
w -= *vPlane;
double n = Dot (*vNormal, w);
double s = n / d;
if (s < 0.0) {
	if (s < -0.000001) // compensate small numerical errors
		return 0;
	s = 0.0;
	}
else if (s > 1.0) {
	if (s > 1.000001) // compensate small numerical errors
		return 0;
	s = 1.0;
	}
u *= s;
intersection = *p0;
intersection += u;
return 1;
}

//--------------------------------------------------------------------------

CDoubleVector ProjectPointOnLine (CDoubleVector* pLine, CDoubleVector* vLine, CDoubleVector* point)
{
if (vLine->Mag () == 0.0)
	return CDoubleVector (*pLine);
return *pLine + *vLine * Dot (*point - *pLine, *vLine / vLine->Mag ());
}

//--------------------------------------------------------------------------

CDoubleVector ProjectPointOnPlane (CDoubleVector* vPlane, CDoubleVector* vNormal, CDoubleVector* point)
{
double n = Dot (*vNormal, *point - *vPlane);
return *point - (*vNormal * n);
}

//--------------------------------------------------------------------------

bool PointIsInTriangle3D (CDoubleVector& p, CDoubleVector v0, CDoubleVector v1, CDoubleVector v2)
{
#if 1
CDoubleVector n1 = Normal (v0, v1, p);
CDoubleVector n2 = Normal (v1, v2, p);
if (Dot (n1, n2) < 0.999999)
	return false;
CDoubleVector n3 = Normal (v2, v0, p);
if (Dot (n2, n3) < 0.999999)
	return false;
return true;
#else
v2 -= v0;
v1 -= v0;
p -= v0;

// Compute dot products
double dot00 = Dot (v2, v2);
double dot01 = Dot (v2, v1);
double dot02 = Dot (v2, p);
double dot11 = Dot (v1, v1);
double dot12 = Dot (v1, p);
// Compute barycentric coordinates
double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
// Check if point is in triangle
return (u >= -0.000001) && (v >= -0.000001) && (u + v <= 1.0000001);
#endif
}


static inline __int64 Side (CLongVector &p0, CLongVector &p1, CLongVector &p2)
{
return ((__int64) p0.y - (__int64) p1.y) * ((__int64) p2.x - (__int64) p1.x) - 
		 ((__int64) p0.x - (__int64) p1.x) * ((__int64) p2.y - (__int64) p1.y);
}

bool PointIsInTriangle2D (CLongVector &p, CLongVector &a, CLongVector &b, CLongVector &c)
{
if ((a.z < 0) || (b.z < 0) || (c.z < 0))
	return false;
__int64 fab = Side (p, a, b);
__int64 fbc = Side (p, b, c);
__int64 fca = Side (p, c, a);
return (fab * fbc > 0) && (fca * fbc > 0);
}

static inline double Side (CDoubleVector &p0, CDoubleVector &p1, CDoubleVector &p2)
{
return (p0.v.y - p1.v.y) * (p2.v.x - p1.v.x) - (p0.v.x - p1.v.x) * (p2.v.y - p1.v.y);
}

bool PointIsInTriangle2D (CDoubleVector &p, CDoubleVector &a, CDoubleVector &b, CDoubleVector &c)
{
double fab = Side (p, a, b);
double fbc = Side (p, b, c);
double fca = Side (p, c, a);
return (fab * fbc > 0) && (fca * fbc > 0);
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// eof
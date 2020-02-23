

#include "stdafx.h"

#include <math.h>
#include "define.h"
#include "types.h"
#include "mine.h"
#include "global.h"
#include "matrix.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

CFixMatrix::CFixMatrix ()
{
m.rVec.Set (F1_0, 0, 0);
m.uVec.Set (0, F1_0, 0);
m.fVec.Set (0, 0, F1_0);
}

// -----------------------------------------------------------------------------

CFixMatrix::CFixMatrix (int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3)
{
Set (x1, y1, z1, x2, y2, z2, x3, y3, z3);
}

// -----------------------------------------------------------------------------

CFixMatrix& CFixMatrix::Set (int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3)
{
m.rVec.Set (x1, y1, z1);
m.uVec.Set (x2, y2, z2);
m.fVec.Set (x3, y3, z3);
return *this;
}

// -----------------------------------------------------------------------------

CFixMatrix::CFixMatrix (int sinp, int cosp, int sinb, int cosb, int sinh, int cosh)
{
Set (sinp, cosp, sinb, cosb, sinh, cosh);
}

// -----------------------------------------------------------------------------

CFixMatrix& CFixMatrix::Set (int sinp, int cosp, int sinb, int cosb, int sinh, int cosh)
{
double sbsh = sinb * sinh;
double cbch = cosb * cosh;
double cbsh = cosb * sinh;
double sbch = sinb * cosh;
m.rVec.v.x = (int) (cbch + sinp * sbsh);
m.uVec.v.z = (int) (sbsh + sinp * cbch);
m.uVec.v.x = (int) (sinp * cbsh - sbch);
m.rVec.v.z = (int) (sinp * sbch - cbsh);
m.fVec.v.x = (int) (sinh * cosp);		
m.rVec.v.y = (int) (sinb * cosp);		
m.uVec.v.y = (int) (cosb * cosp);		
m.fVec.v.z = (int) (cosh * cosp);		
m.fVec.v.y = (int) -sinp;				
return *this;
}

// -----------------------------------------------------------------------------

CFixMatrix CFixMatrix::Mul (const CFixMatrix& other) 
{
	CFixVector v;
	CFixMatrix result;

v.Set (other.m.rVec.v.x, other.m.uVec.v.x, other.m.fVec.v.x);
result.m.rVec.v.x = v ^ m.rVec;
result.m.uVec.v.x = v ^ m.uVec;
result.m.fVec.v.x = v ^ m.fVec;
v.Set (other.m.rVec.v.y, other.m.uVec.v.y, other.m.fVec.v.y);
result.m.rVec.v.y = v ^ m.rVec;
result.m.uVec.v.y = v ^ m.uVec;
result.m.fVec.v.y = v ^ m.fVec;
v.Set (other.m.rVec.v.z, other.m.uVec.v.z, other.m.fVec.v.z);
result.m.rVec.v.z = v ^ m.rVec;
result.m.uVec.v.z = v ^ m.uVec;
result.m.fVec.v.z = v ^ m.fVec;
return result;
}

// -----------------------------------------------------------------------------

const int CFixMatrix::Det (void) 
{
int det = FixMul (m.rVec.v.x, FixMul (m.uVec.v.y, m.fVec.v.z) - FixMul (m.uVec.v.z, m.fVec.v.y));
det += FixMul (m.rVec.v.y, FixMul (m.uVec.v.z, m.fVec.v.x) - FixMul (m.uVec.v.x, m.fVec.v.z));
det += FixMul (m.rVec.v.z, FixMul (m.uVec.v.x, m.fVec.v.y) - FixMul (m.uVec.v.y, m.fVec.v.x));
return det;
}

// -----------------------------------------------------------------------------

const CFixMatrix CFixMatrix::Inverse (void) 
{
	CFixMatrix result;

int det = Det ();
if (det != 0) {
	result.m.rVec.v.x = FixDiv (FixMul (m.uVec.v.y, m.fVec.v.z) - FixMul (m.uVec.v.z, m.fVec.v.y), det);
	result.m.rVec.v.y = FixDiv (FixMul (m.rVec.v.z, m.fVec.v.y) - FixMul (m.rVec.v.y, m.fVec.v.z), det);
	result.m.rVec.v.z = FixDiv (FixMul (m.rVec.v.y, m.uVec.v.z) - FixMul (m.rVec.v.z, m.uVec.v.y), det);
	result.m.uVec.v.x = FixDiv (FixMul (m.uVec.v.z, m.fVec.v.x) - FixMul (m.uVec.v.x, m.fVec.v.z), det);
	result.m.uVec.v.y = FixDiv (FixMul (m.rVec.v.x, m.fVec.v.z) - FixMul (m.rVec.v.z, m.fVec.v.x), det);
	result.m.uVec.v.z = FixDiv (FixMul (m.rVec.v.z, m.uVec.v.x) - FixMul (m.rVec.v.x, m.uVec.v.z), det);
	result.m.fVec.v.x = FixDiv (FixMul (m.uVec.v.x, m.fVec.v.y) - FixMul (m.uVec.v.y, m.fVec.v.x), det);
	result.m.fVec.v.y = FixDiv (FixMul (m.rVec.v.y, m.fVec.v.x) - FixMul (m.rVec.v.x, m.fVec.v.y), det);
	result.m.fVec.v.z = FixDiv (FixMul (m.rVec.v.x, m.uVec.v.y) - FixMul (m.rVec.v.y, m.uVec.v.x), det);
	}
return result;
}

// -----------------------------------------------------------------------------

CFixMatrix& Transpose (CFixMatrix& dest, CFixMatrix& src)
{
dest.m.rVec.v.x = src.m.rVec.v.x;
dest.m.uVec.v.x = src.m.rVec.v.y;
dest.m.fVec.v.x = src.m.rVec.v.z;
dest.m.rVec.v.y = src.m.uVec.v.x;
dest.m.uVec.v.y = src.m.uVec.v.y;
dest.m.fVec.v.y = src.m.uVec.v.z;
dest.m.rVec.v.z = src.m.fVec.v.x;
dest.m.uVec.v.z = src.m.fVec.v.y;
dest.m.fVec.v.z = src.m.fVec.v.z;
return dest;
}

// -----------------------------------------------------------------------------

void CFixMatrix::Rotate (double angle, char axis) 
{
double cosX = cos (angle);
double sinX = sin (angle);

CFixMatrix mRot;

switch (axis) {
	case 'x':
		// spin x
		//	1	0	0
		//	0	cos	sin
		//	0	-sin	cos
		//
		mRot.m.uVec.Set ((int) (m.uVec.v.x * cosX + m.fVec.v.x * sinX), 
					        (int) (m.uVec.v.y * cosX + m.fVec.v.y * sinX),
						     (int) (m.uVec.v.z * cosX + m.fVec.v.z * sinX));
		mRot.m.fVec.Set ((int) (-m.uVec.v.x * sinX + m.fVec.v.x * cosX),
							  (int) (-m.uVec.v.y * sinX + m.fVec.v.y * cosX),
							  (int) (-m.uVec.v.z * sinX + m.fVec.v.z * cosX));
		m.uVec = mRot.m.uVec;
		m.fVec = mRot.m.fVec;
		break;
	case 'y':
		// spin y
		//	cos	0	-sin
		//	0	1	0
		//	sin	0	cos
		//
		mRot.m.rVec.Set ((int) (m.rVec.v.x * cosX - m.fVec.v.x * sinX), 
							  (int) (m.rVec.v.y * cosX - m.fVec.v.y * sinX), 
							  (int) (m.rVec.v.z * cosX - m.fVec.v.z * sinX));
		mRot.m.fVec.Set ((int) (m.rVec.v.x * sinX + m.fVec.v.x * cosX), 
							  (int) (m.rVec.v.y * sinX + m.fVec.v.y * cosX),
							  (int) (m.rVec.v.z * sinX + m.fVec.v.z * cosX));
		m.rVec = mRot.m.rVec;
		m.fVec = mRot.m.fVec;
		break;
	case 'z':
		// spin z
		//	cos	sin	0
		//	-sin	cos	0
		//	0	0	1
		mRot.m.rVec.Set ((int) (m.rVec.v.x * cosX + m.uVec.v.x * sinX),
							  (int) (m.rVec.v.y * cosX + m.uVec.v.y * sinX),
							  (int) (m.rVec.v.z * cosX + m.uVec.v.z * sinX));
		mRot.m.uVec.Set ((int) (-m.rVec.v.x * sinX + m.uVec.v.x * cosX),
							  (int) (-m.rVec.v.y * sinX + m.uVec.v.y * cosX),
							  (int) (-m.rVec.v.z * sinX + m.uVec.v.z * cosX));
		m.rVec = mRot.m.rVec;
		m.uVec = mRot.m.uVec;
		break;
	}
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CDoubleMatrix::Clear (void)
{
m.rVec.Set (1.0, 0.0, 0.0);
m.uVec.Set (0.0, 1.0, 0.0);
m.fVec.Set (0.0, 0.0, 1.0);
}

// -----------------------------------------------------------------------------

CDoubleMatrix::CDoubleMatrix (double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
Set (x1, y1, z1, x2, y2, z2, x3, y3, z3);
}

// -----------------------------------------------------------------------------

CDoubleMatrix& CDoubleMatrix::Set (double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
m.rVec.Set (x1, y1, z1);
m.uVec.Set (x2, y2, z2);
m.fVec.Set (x3, y3, z3);
return *this;
}

// -----------------------------------------------------------------------------

CDoubleMatrix::CDoubleMatrix (double sinp, double cosp, double sinb, double cosb, double sinh, double cosh)
{
Set (sinp, cosp, sinb, cosb, sinh, cosh);
}

// -----------------------------------------------------------------------------

CDoubleMatrix& CDoubleMatrix::Set (double sinp, double cosp, double sinb, double cosb, double sinh, double cosh)
{
m.rVec.Set (cosh * cosb - sinh * sinp * sinb, -cosp * sinb, cosh * sinp * sinb + sinh * cosb);
m.uVec.Set (cosh * sinb + sinh * sinp * cosb, cosp * cosb, sinh * sinb - cosh * sinp * cosb);
m.fVec.Set (-sinh * cosp, sinp, cosh * cosp);
return *this;
}

// -----------------------------------------------------------------------------

void CDoubleMatrix::CopyTo (double& p, double& b, double& h)
{
p = asin (m.fVec.v.y);
if (cos (p) > 0.0001) {
	b = -atan2 (m.rVec.v.y, m.uVec.v.y);
	h = -atan2 (m.fVec.v.x, m.fVec.v.z);
} else {
	b = -atan2 (-m.rVec.v.z, -m.uVec.v.z);
	h = 0;
	}
}

// -----------------------------------------------------------------------------

CDoubleMatrix CDoubleMatrix::Mul (const CDoubleMatrix& other) 
{
	CDoubleVector v;
	CDoubleMatrix result;

v.Set (other.m.rVec.v.x, other.m.uVec.v.x, other.m.fVec.v.x);
result.m.rVec.v.x = v ^ m.rVec;
result.m.uVec.v.x = v ^ m.uVec;
result.m.fVec.v.x = v ^ m.fVec;
v.Set (other.m.rVec.v.y, other.m.uVec.v.y, other.m.fVec.v.y);
result.m.rVec.v.y = v ^ m.rVec;
result.m.uVec.v.y = v ^ m.uVec;
result.m.fVec.v.y = v ^ m.fVec;
v.Set (other.m.rVec.v.z, other.m.uVec.v.z, other.m.fVec.v.z);
result.m.rVec.v.z = v ^ m.rVec;
result.m.uVec.v.z = v ^ m.uVec;
result.m.fVec.v.z = v ^ m.fVec;
return result;
}

// -----------------------------------------------------------------------------

const double CDoubleMatrix::Det (void) 
{
return m.rVec.v.x * (m.fVec.v.y * m.uVec.v.z - m.uVec.v.y * m.fVec.v.z) +
		 m.uVec.v.x * (m.rVec.v.y * m.fVec.v.z - m.fVec.v.y * m.rVec.v.z) +
		 m.fVec.v.x * (m.uVec.v.y * m.rVec.v.z - m.rVec.v.y * m.uVec.v.z);
}

// -----------------------------------------------------------------------------

const CDoubleMatrix CDoubleMatrix::Inverse (void) 
{
	CDoubleMatrix result;

double det = Det ();
if (det != 0.0) {
	result.m.rVec.v.x = (m.fVec.v.y * m.uVec.v.z - m.uVec.v.y * m.fVec.v.z) / det;
	result.m.rVec.v.y = (m.rVec.v.y * m.fVec.v.z - m.fVec.v.y * m.rVec.v.z) / det;
	result.m.rVec.v.z = (m.uVec.v.y * m.rVec.v.z - m.rVec.v.y * m.uVec.v.z) / det;

	result.m.uVec.v.x = (m.uVec.v.x * m.fVec.v.z - m.fVec.v.x * m.uVec.v.z) / det;
	result.m.uVec.v.y = (m.fVec.v.x * m.rVec.v.z - m.rVec.v.x * m.fVec.v.z) / det;
	result.m.uVec.v.z = (m.rVec.v.x * m.uVec.v.z - m.uVec.v.x * m.rVec.v.z) / det;

	result.m.fVec.v.x = (m.fVec.v.x * m.uVec.v.y - m.uVec.v.x * m.fVec.v.y) / det;
	result.m.fVec.v.y = (m.rVec.v.x * m.fVec.v.y - m.fVec.v.x * m.rVec.v.y) / det;
	result.m.fVec.v.z = (m.uVec.v.x * m.rVec.v.y - m.rVec.v.x * m.uVec.v.y) / det;
	}
return result;
}

// -----------------------------------------------------------------------------
// Compute the matrix to transform the coordinate system mSource to mDest

const CDoubleMatrix Transformation (CDoubleMatrix& mDest, CDoubleMatrix& mSource)
{
	CDoubleMatrix i, t;
	
i = mSource.Inverse ();
t = mDest * i;
return t;
}

// -----------------------------------------------------------------------------

const CDoubleMatrix CDoubleMatrix::Adjoint (void) 
{
	CDoubleMatrix result;

result.m.rVec.v.x = m.uVec.v.y * m.fVec.v.z - m.uVec.v.z * m.fVec.v.y;
result.m.rVec.v.y = m.rVec.v.z * m.fVec.v.y - m.rVec.v.y * m.fVec.v.z;
result.m.rVec.v.z = m.rVec.v.y * m.uVec.v.z - m.rVec.v.z * m.uVec.v.y;
result.m.uVec.v.x = m.uVec.v.z * m.fVec.v.x - m.uVec.v.x * m.fVec.v.z;
result.m.uVec.v.y = m.rVec.v.x * m.fVec.v.z - m.rVec.v.z * m.fVec.v.x;
result.m.uVec.v.z = m.rVec.v.z * m.uVec.v.x - m.rVec.v.x * m.uVec.v.z;
result.m.fVec.v.x = m.uVec.v.x * m.fVec.v.y - m.uVec.v.y * m.fVec.v.x;
result.m.fVec.v.y = m.rVec.v.y * m.fVec.v.x - m.rVec.v.x * m.fVec.v.y;
result.m.fVec.v.z = m.rVec.v.x * m.uVec.v.y - m.rVec.v.y * m.uVec.v.x;
return result;
}

// -----------------------------------------------------------------------------

CDoubleMatrix& Transpose (CDoubleMatrix& dest, CDoubleMatrix& src)
{
dest.m.rVec.v.x = src.m.rVec.v.x;
dest.m.uVec.v.x = src.m.rVec.v.y;
dest.m.fVec.v.x = src.m.rVec.v.z;
dest.m.rVec.v.y = src.m.uVec.v.x;
dest.m.uVec.v.y = src.m.uVec.v.y;
dest.m.fVec.v.y = src.m.uVec.v.z;
dest.m.rVec.v.z = src.m.fVec.v.x;
dest.m.uVec.v.z = src.m.fVec.v.y;
dest.m.fVec.v.z = src.m.fVec.v.z;
return dest;
}

// -----------------------------------------------------------------------------

void CDoubleMatrix::Rotate (double angle, char axis) 
{
double cosX = cos (angle);
double sinX = sin (angle);

CDoubleMatrix mRot;

switch (axis) {
	case 'x':
		// spin x
		//	1	0	0
		//	0	cos	sin
		//	0	-sin	cos
		//
		mRot.m.uVec.Set (m.uVec.v.x * cosX + m.fVec.v.x * sinX, 
					        m.uVec.v.y * cosX + m.fVec.v.y * sinX,
						     m.uVec.v.z * cosX + m.fVec.v.z * sinX);
		mRot.m.fVec.Set (m.fVec.v.x * cosX - m.uVec.v.x * sinX,
							  m.fVec.v.y * cosX - m.uVec.v.y * sinX,
							  m.fVec.v.z * cosX - m.uVec.v.z * sinX);
		m.uVec = mRot.m.uVec;
		m.fVec = mRot.m.fVec;
		break;

	case 'y':
		// spin y
		//	cos	0	-sin
		//	0	1	0
		//	sin	0	cos
		//
		mRot.m.rVec.Set (m.rVec.v.x * cosX - m.fVec.v.x * sinX, 
							  m.rVec.v.y * cosX - m.fVec.v.y * sinX, 
							  m.rVec.v.z * cosX - m.fVec.v.z * sinX);
		mRot.m.fVec.Set (m.rVec.v.x * sinX + m.fVec.v.x * cosX, 
							  m.rVec.v.y * sinX + m.fVec.v.y * cosX,
							  m.rVec.v.z * sinX + m.fVec.v.z * cosX);
		m.rVec = mRot.m.rVec;
		m.fVec = mRot.m.fVec;
		break;

	case 'z':
		// spin z
		//	cos	sin	0
		//	-sin	cos	0
		//	0	0	1
		mRot.m.rVec.Set (m.rVec.v.x * cosX + m.uVec.v.x * sinX,
							  m.rVec.v.y * cosX + m.uVec.v.y * sinX,
							  m.rVec.v.z * cosX + m.uVec.v.z * sinX);
		mRot.m.uVec.Set (m.uVec.v.x * cosX - m.rVec.v.x * sinX,
							  m.uVec.v.y * cosX - m.rVec.v.y * sinX,
							  m.uVec.v.z * cosX - m.rVec.v.z * sinX);
		m.rVec = mRot.m.rVec;
		m.uVec = mRot.m.uVec;
		break;
	}
}

//------------------------------------------------------------------------

void CDoubleMatrix::MapUnitSquareToQuad (CLongVector a [4]) 
{
// infer "unity square" to "quad" perspective transformation
// see page 55-56 of Digital Image Warping by George Wolberg (3rd edition) 
double dx1 = a [1].x - a [2].x;
double dx2 = a [3].x - a [2].x;
double dx3 = a [0].x - a [1].x + a [2].x - a [3].x;
double dy1 = a [1].y - a [2].y;
double dy2 = a [3].y - a [2].y;
double dy3 = a [0].y - a [1].y + a [2].y - a [3].y;
double w = (dx1 * dy2 - dx2 * dy1);
if (w == 0.0) 
	w = 1.0;

m.rVec.v.z = (dx3 * dy2 - dx2 * dy3) / w;
m.uVec.v.z = (dx1 * dy3 - dx3 * dy1) / w;
m.rVec.v.x = a [1].x - a [0].x + m.rVec.v.z * a [1].x;
m.uVec.v.x = a [3].x - a [0].x + m.uVec.v.z * a [3].x;
m.fVec.v.x = a [0].x;
m.rVec.v.y = a [1].y - a [0].y + m.rVec.v.z * a [1].y;
m.uVec.v.y = a [3].y - a [0].y + m.uVec.v.z * a [3].y;
m.fVec.v.y = a [0].y;
m.fVec.v.z = 1;
}

//------------------------------------------------------------------------------

void CDoubleMatrix::Scale (double scale) 
{
CDoubleMatrix s, m = *this;
s.Set (scale, 0.0, 0.0, 0.0, scale, 0.0, 0.0, 0.0, 1.0);
*this = m * s;
}

// -----------------------------------------------------------------------------

static CDoubleMatrix id;

CDoubleVector CDoubleMatrix::Angles (void)
{
	CDoubleVector a;

a.v.y = -asin (m.rVec.v.z);
double cosY = cos (a.v.y);

bool bGimbalLock = fabs (cosY) <= 1e-3;

a.v.x = bGimbalLock ? 0.0 : atan2 (-m.uVec.v.z / cosY, m.fVec.v.z / cosY);
#if 1
double dot;
if (fabs (dot = Dot (m.fVec, id.m.rVec)) > 0.999) 
	a.v.z = acos (Dot (m.uVec, id.m.uVec)) * Sign (dot);
if (fabs (dot = Dot (m.fVec, id.m.fVec)) > 0.999)
	a.v.z = acos (Dot (m.uVec, id.m.uVec)) * Sign (dot);
else if (fabs (dot = Dot (m.fVec, id.m.uVec)) > 0.999)
	a.v.z = acos (Dot (m.uVec, id.m.fVec)) * Sign (dot);
else
#endif
	a.v.z = bGimbalLock ? atan2 (m.uVec.v.x, m.uVec.v.y) : atan2 (-m.rVec.v.y / cosY, m.rVec.v.x / cosY);
return a;
}

// -----------------------------------------------------------------------------
// eof
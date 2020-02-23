#ifndef __matrix_h
#define __matrix_h

#include <afxwin.h>
#include <cmath>

#include "define.h"
#include "Vector.h"

class CFixMatrix;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct tFixMatrix {
	CFixVector rVec, uVec, fVec;
};

class CFixMatrix {
public:
	tFixMatrix m;
#if 0
	inline void Read (CFileManager* fp) { 
		m.rVec.Read (fp);
		m.uVec.Read (fp);
		m.fVec.Read (fp);
	}

	inline void Write (CFileManager* fp) { 
		m.rVec.Write (fp);
		m.uVec.Write (fp);
		m.fVec.Write (fp);
	}
#endif
	CFixMatrix ();
	CFixMatrix (int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3);
	CFixMatrix (CFixMatrix& other) { m.rVec = other.m.rVec, m.uVec = other.m.uVec, m.fVec = other.m.fVec; }
	CFixMatrix (CFixVector& r, CFixVector& u, CFixVector& f) { m.rVec = r, m.uVec = u, m.fVec = f; }
	CFixMatrix (int sinp, int cosp, int sinb, int cosb, int sinh, int cosh);
	//computes a matrix from a Set of three angles.  returns ptr to matrix
	CFixMatrix (CAngleVector& a);
	//computes a matrix from a forward vector and an angle
	CFixMatrix (CFixVector *v, short a);

	CFixMatrix& CFixMatrix::Set (int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3);
	CFixMatrix& Set (int sinp, int cosp, int sinb, int cosb, int sinh, int cosh);

	CFixMatrix& Invert (CFixMatrix& m);
	CFixMatrix Mul (const CFixMatrix& m);
	CFixMatrix& Scale (CFixVector& scale);

	const CFixVector operator* (const CFixVector& v);
	inline const CFixMatrix operator* (const CFixMatrix& other) { return Mul (other); }
	inline const CFixMatrix& operator= (const CFixMatrix& other) { 
		m.rVec = other.m.rVec, m.uVec = other.m.uVec, m.fVec = other.m.fVec;
		return *this;
		}

	const int Det (void);
	const CFixMatrix Inverse (void);
	void CFixMatrix::Rotate (double angle, char axis);
	inline const CFixMatrix Transpose (void);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

CFixMatrix& Transpose (CFixMatrix& dest, CFixMatrix& src);

inline CFixMatrix& Transpose (CFixMatrix& m)
{
Swap (m.m.rVec.v.y, m.m.uVec.v.x);
Swap (m.m.rVec.v.z, m.m.fVec.v.x);
Swap (m.m.uVec.v.z, m.m.fVec.v.y);
return m;
}

// -----------------------------------------------------------------------------

inline const CFixMatrix CFixMatrix::Transpose (void)
{
CFixMatrix m;
::Transpose (m, *this);
return m;
}

// -----------------------------------------------------------------------------

inline const CFixVector CFixMatrix::operator* (const CFixVector& v)
{
return CFixVector (v ^ m.rVec, v ^ m.uVec, v ^ m.fVec);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tDoubleMatrix {
	CDoubleVector rVec, uVec, fVec;
} tDoubleMatrix;

class CDoubleMatrix {
public:
	tDoubleMatrix	m;

#if 0
	inline void Read (CFileManager* fp) { 
		m.rVec.Read (fp);
		m.uVec.Read (fp);
		m.fVec.Read (fp);
		}

	inline void Write (CFileManager* fp) { 
		m.rVec.Write (fp);
		m.uVec.Write (fp);
		m.fVec.Write (fp);
		}
#endif
	CDoubleMatrix::CDoubleMatrix () { Clear (); }
	CDoubleMatrix::CDoubleMatrix (double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);
	CDoubleMatrix (const CDoubleMatrix& other) { m.rVec = other.m.rVec, m.uVec = other.m.uVec, m.fVec = other.m.fVec; }
	CDoubleMatrix (CDoubleVector r, CDoubleVector u, CDoubleVector f) { m.rVec = r, m.uVec = u, m.fVec = f; }
	CDoubleMatrix (double sinp, double cosp, double sinb, double cosb, double sinh, double cosh);
	//computes a matrix from a Set of three angles.  returns ptr to matrix
	CDoubleMatrix (CAngleVector& a);
	//computes a matrix from a forward vector and an angle
	CDoubleMatrix (CDoubleVector *v, short a);

	CDoubleVector Angles (void);
	
	void Clear (void);

	CDoubleMatrix& Set (double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);
	CDoubleMatrix& Set (double sinp, double cosp, double sinb, double cosb, double sinh, double cosh);
	void CopyTo (double& p, double& b, double& h);

	CDoubleMatrix& Invert (CDoubleMatrix& m);
	CDoubleMatrix Mul (const CDoubleMatrix& m);
	CDoubleMatrix& Scale (CDoubleVector& scale);

	const CDoubleVector operator* (const CDoubleVector& v);
	inline const CDoubleMatrix operator* (const CDoubleMatrix& other) { return Mul (other); }
	inline const CDoubleMatrix& operator= (const CDoubleMatrix& other) { 
		m.rVec = other.m.rVec, m.uVec = other.m.uVec, m.fVec = other.m.fVec;
		return *this;
		}

	inline const bool operator== (const CDoubleMatrix& other) {
		return (m.rVec == other.m.rVec) && (m.uVec == other.m.uVec) && (m.fVec == other.m.fVec);
		}

	inline const bool operator!= (const CDoubleMatrix& other) {
		return (m.rVec != other.m.rVec) || (m.uVec != other.m.uVec) || (m.fVec != other.m.fVec);
		}

	inline CDoubleVector& CDoubleMatrix::operator[] (const size_t i) { return ((CDoubleVector*) &m) [i]; }

	inline int Handedness (void) { return (Dot (m.fVec, CrossProduct (m.rVec, m.uVec)) > 0.0) ? 1 : 0; } // 0: left handed, 1: right handed
	
	inline CDoubleVector& R (void) { return m.rVec; }
	inline CDoubleVector& U (void) { return m.uVec; }
	inline CDoubleVector& F (void) { return m.fVec; }

	const double Det (void);
	const CDoubleMatrix Inverse (void);
	const CDoubleMatrix Adjoint (void);
	inline const CDoubleMatrix Transpose (void);
	void Rotate (double angle, char axis);
	void Scale (double scale);
	void MapUnitSquareToQuad (CLongVector a [4]);
};

const CDoubleMatrix Transformation (CDoubleMatrix& mDest, CDoubleMatrix& mSource);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

CDoubleMatrix& Transpose (CDoubleMatrix& dest, CDoubleMatrix& src);

inline CDoubleMatrix& Transpose (CDoubleMatrix& m)
{
Swap (m.m.rVec.v.y, m.m.uVec.v.x);
Swap (m.m.rVec.v.z, m.m.fVec.v.x);
Swap (m.m.uVec.v.z, m.m.fVec.v.y);
return m;
}

// -----------------------------------------------------------------------------

inline const CDoubleMatrix CDoubleMatrix::Transpose (void)
{
CDoubleMatrix m;
::Transpose (m, *this);
return m;
}

// -----------------------------------------------------------------------------

inline const CDoubleVector CDoubleMatrix::operator* (const CDoubleVector& v)
{
return CDoubleVector (v ^ m.rVec, v ^ m.uVec, v ^ m.fVec);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __matrix_h


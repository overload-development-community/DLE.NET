#ifndef __vector_h
#define __vector_h

#include <math.h>
#include "define.h"
//#include "FileManager.h"

struct tAngleVector;
class CAngleVector;
struct tFixVector;
class CFixVector;
struct tFloatVector;
class CFloatVector;
struct tDoubleVector;
class CDoubleVector;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CLongVector {
public:
	long x, y, z;

	inline CLongVector operator- (CLongVector& other) {
		CLongVector result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		return result;
		}

	inline CLongVector& operator+= (CLongVector& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
		}

	inline CLongVector& operator-= (CLongVector& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
		}

	inline CLongVector& operator/= (long n) {
		x /= n;
		y /= n;
		z /= n;
		return *this;
		}

	inline double Sqr (void) { return double (x) * double (x) + double (y) * double (y) + double (z) * double (z); }

	inline long Mag (void) { return long (Round (sqrt (Sqr ()))); }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tAngleVector {
public:
	short p, b, h;
#if 0
inline void Read (CFileManager* fp) { 
	p = fp->ReadFixAng ();
	b = fp->ReadFixAng ();
	h = fp->ReadFixAng ();
	}

inline void Write (CFileManager* fp) { 
	WriteInt16 (p, fp);
	WriteInt16 (b, fp);
	WriteInt16 (h, fp);
	}
#endif
} tAngleVector;

class CAngleVector {
public:
	tAngleVector	v;

	CAngleVector () { v.p = 0, v.b = 0, v.h = 0; }
	CAngleVector (short p, short b, short h) { v.p = p, v.b = b, v.h = h; }
	CAngleVector (tAngleVector& _v) { v.p = _v.p, v.b = _v.b, v.h = _v.h; }
	CAngleVector (CAngleVector& _v) { v.p = _v.v.p, v.b = _v.v.b, v.h = _v.v.h; }
	void Set (short p, short b, short h) { v.p = p, v.b = b, v.h = h; }
	void Clear (void) { Set (0,0,0); }
#if 0
	inline void Read (CFileManager* fp) { v.Read (fp); }
	inline void Write (CFileManager* fp) { v.Write (fp); }
#endif
	inline const CAngleVector& operator= (CAngleVector& other) { 
		v.p = other.v.p, v.b = other.v.b, other.v.h = other.v.h; 
		return *this;
		}
	inline const CAngleVector& operator= (tAngleVector& other) { 
		v.p = other.p, v.b = other.b, v.h = other.h; 
		return *this;
		}
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct tFixVector {
public:
	int x, y, z;
};

class CFixVector {
public:
	tFixVector	v;
	//double x, y, z;
	CFixVector ()  { v.x = 0, v.y = 0, v.z = 0; }
	CFixVector (int x, int y, int z) { v.x = x, v.y = y, v.z = z; }
	CFixVector (tFixVector& _v) { v.x = _v.x, v.y = _v.y, v.z = _v.z; }
	explicit CFixVector (CDoubleVector& _v); 
	void Set (int x, int y, int z) { v.x = x, v.y = y, v.z = z; }
	void Clear (void) { Set (0,0,0); }

inline const bool operator== (const CFixVector other);
inline int& CFixVector::operator[] (const size_t i);
inline const CFixVector& operator= (const tFixVector& other);
inline const CFixVector& operator= (const CFixVector& other);
inline const CFixVector& operator= (const CFloatVector& other);
inline const CFixVector& operator= (const tDoubleVector& other);
inline const CFixVector& operator= (const CDoubleVector& other);
inline const CFixVector& operator+= (const CFixVector other);
inline const CFixVector& operator-= (const CFixVector other);
inline const CFixVector& operator*= (const int n);
inline const CFixVector& operator/= (const int n);
inline const CFixVector& operator*= (const double n);
inline const CFixVector& operator/= (const double n);
inline const CFixVector operator+ (const CFixVector& other) const;
inline const CFixVector operator- (const CFixVector& other) const;
inline const CFixVector operator- (void) const;
inline const CFixVector operator>> (const int n);
inline const CFixVector operator<< (const int n);
inline const CFixVector& operator>>= (const int n);
inline const CFixVector& operator<<= (const int n);
inline const CFixVector operator* (CFixVector other) const;
inline const CFixVector operator/ (CFixVector other) const;
inline const CFixVector operator* (int n) const;
inline const CFixVector operator/ (int n) const;
inline operator CDoubleVector();
inline const int operator^ (const CFixVector& other) const;

inline const int Mag (void);
inline const CFixVector& Normalize (void) { *this /= Mag (); return *this; }
void Rotate (CFixVector& origin, CFixVector& axis, double angle);
inline const CFixVector Negate (void) { v.x = -v.x, v.y = -v.y, v.z = -v.z; return *this; }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct tFloatVector {
public:
	float x, y, z;
};

class CFloatVector {
public:
	tFloatVector	v;
	CFloatVector ()  { v.x = 0.0f, v.y = 0.0f, v.z = 0.0f; }
	CFloatVector (float x, float y, float z) { v.x = x, v.y = y, v.z = z; }
	CFloatVector (tFloatVector& _v) { v.x = _v.x, v.y = _v.y, v.z = _v.z; }
	explicit CFloatVector (CDoubleVector& _v); 
	void Set (float x, float y, float z) { v.x = x, v.y = y, v.z = z; }
	void Clear (void) { Set (0.0, 0.0, 0.0); }

inline const bool operator== (const CFloatVector other);
inline float& CFloatVector::operator[] (const size_t i);
inline const CFloatVector& operator= (const tFixVector& other);
inline const CFloatVector& operator= (const CFloatVector& other);
inline const CFloatVector& operator= (const tDoubleVector& other);
inline const CFloatVector& operator= (const CDoubleVector& other);
inline const CFloatVector& operator+= (const CFloatVector other);
inline const CFloatVector& operator-= (const CFloatVector other);
inline const CFloatVector& operator*= (const float n);
inline const CFloatVector& operator/= (const float n);
inline const CFloatVector operator+ (const CFloatVector& other) const;
inline const CFloatVector operator- (const CFloatVector& other) const;
inline const CFloatVector operator- (void) const;
inline const CFloatVector operator* (CFloatVector other) const;
inline const CFloatVector operator/ (CFloatVector other) const;
inline const CFloatVector operator* (float n) const;
inline const CFloatVector operator/ (float n) const;
inline operator CDoubleVector();
inline const float operator^ (const CFloatVector& other) const;

inline const float SqrMag (void) { return v.x * v.x + v.y * v.y + v.z * v.z; }
inline const float Mag (void) { return sqrt (SqrMag ()); }
inline const CFloatVector& Normalize (void) { *this /= Mag (); return *this; }
inline const CFloatVector Negate (void) { v.x = -v.x, v.y = -v.y, v.z = -v.z; return *this; }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

struct tDoubleVector {
public:
	double x, y, z;
};

class CDoubleVector {
	public:
		tDoubleVector	v;
		//double x, y, z;
		CDoubleVector ()  { v.x = 0.0, v.y = 0.0, v.z = 0.0; }
		CDoubleVector (double x, double y, double z) { v.x = x, v.y = y, v.z = z; }
		CDoubleVector (tDoubleVector& _v) { v.x = _v.x, v.y = _v.y, v.z = _v.z; }
		explicit CDoubleVector (CFixVector _v);
		void Set (double x, double y, double z) { v.x = x, v.y = y, v.z = z; }
		void Clear (void) { Set (0.0, 0.0, 0.0); }
#if 0
	inline void Read (CFileManager* fp) { v.Read (fp); }
	inline void Write (CFileManager* fp) { v.Write (fp); }
#endif
	inline const bool operator== (const CDoubleVector other);
	inline const bool operator!= (const CDoubleVector other);
	inline double& CDoubleVector::operator[] (const size_t i);
	inline const CDoubleVector& operator= (const tDoubleVector& other);
	inline const CDoubleVector& operator= (const CDoubleVector& other);
	inline const CDoubleVector& operator= (const tFixVector& other);
	inline const CDoubleVector& operator= (const CFixVector& other);
	inline const CDoubleVector& operator+= (const CDoubleVector& other);
	inline const CDoubleVector& operator+= (const CFixVector& other);
	inline const CDoubleVector& operator-= (const CDoubleVector& other);
	inline const CDoubleVector& operator*= (const double n);
	inline const CDoubleVector& operator/= (const double n);
	inline const CDoubleVector& operator*= (const CDoubleVector& other);
	inline const CDoubleVector& operator/= (const CDoubleVector& other);
	inline const CDoubleVector operator+ (const CDoubleVector& other) const;
	inline const CDoubleVector operator- (const CDoubleVector& other) const;
	inline const CDoubleVector operator- (void) const;
	inline const CDoubleVector operator* (const CDoubleVector& other) const;
	inline const CDoubleVector operator/ (const CDoubleVector& other) const;
	inline const CDoubleVector operator* (double n);
	inline const CDoubleVector operator/ (double n);
	inline operator CFixVector();
	inline const double operator^ (const CDoubleVector& other) const;

	inline const double SqrMag (void) { return v.x * v.x + v.y * v.y + v.z * v.z; }
	inline const double Mag (void) { return sqrt (SqrMag ()); }
	inline const CDoubleVector& Normalize (void) { 
		double m = Mag ();
		if (m != 0.0)
			*this /= m; 
		return *this; 
		}
	inline const CDoubleVector Negate (void) { v.x = -v.x, v.y = -v.y, v.z = -v.z; return *this; }
	void Rotate (CDoubleVector origin, CDoubleVector axis, double angle);
	void Rotate (CDoubleVector axis, double angle);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

inline CFixVector::operator CDoubleVector() {
	return CDoubleVector (double (v.x) / 65536.0, double (v.y) / 65536.0, double (v.z) / 65536.0);
}

inline const CFixVector& CFixVector::operator= (const tFixVector& other) { 
	v.x = other.x, v.y = other.y, v.z = other.z; 
	return *this;
	}

inline const bool CFixVector::operator== (const CFixVector other) {
	return (v.x == other.v.x) && (v.y == other.v.y) && (v.z == other.v.z);
}

inline int& CFixVector::operator[] (const size_t i) { return ((int*) &v) [i]; }

inline const CFixVector& CFixVector::operator= (const CFixVector& other) { 
	v.x = other.v.x, v.y = other.v.y, v.z = other.v.z; 
	return *this;
	}

inline const CFixVector& CFixVector::operator= (const CFloatVector& other) { 
	v.x = D2X (other.v.x), v.y = D2X (other.v.y), v.z = D2X (other.v.z); 
	return *this;
	}

inline const CFixVector& CFixVector::operator= (const tDoubleVector& other) { 
	v.x = D2X (other.x), v.y = D2X (other.y), v.z = D2X (other.z); 
	return *this;
	}

inline const CFixVector& CFixVector::operator= (const CDoubleVector& other) { 
	v.x = D2X (other.v.x), v.y = D2X (other.v.y), v.z = D2X (other.v.z); 
	return *this;
	}

inline const CFixVector& CFixVector::operator+= (const CFixVector other) {
	v.x += other.v.x, v.y += other.v.y, v.z += other.v.z; 
	return *this;
	}

inline const CFixVector& CFixVector::operator-= (const CFixVector other) {
	v.x -= other.v.x, v.y -= other.v.y, v.z -= other.v.z; 
	return *this;
	}

inline const CFixVector CFixVector::operator+ (const CFixVector& other) const {
	return CFixVector (v.x + other.v.x, v.y + other.v.y, v.z + other.v.z);
	}

inline const CFixVector CFixVector::operator- (const CFixVector& other) const {
	return CFixVector (v.x - other.v.x, v.y - other.v.y, v.z - other.v.z);
	}

inline const CFixVector CFixVector::operator- (void) const {
	return CFixVector (-v.x, -v.y, -v.z);
	}

inline const CFixVector& CFixVector::operator*= (const int n) {
	v.x = FixMul (v.x, n), v.y = FixMul (v.y, n), v.z = FixMul (v.z, n);
	return *this;
	}

inline const CFixVector& CFixVector::operator/= (const int n) {
	v.x = FixDiv (v.x, n), v.y = FixDiv (v.y, n), v.z = FixDiv (v.z, n);
	return *this;
	}

inline const CFixVector& CFixVector::operator*= (const double n) {
	v.x = (int) Round ((double) v.x * n),
	v.y = (int) Round ((double) v.y * n),
	v.z = (int) Round ((double) v.z * n);
	return *this;
	}

inline const CFixVector& CFixVector::operator/= (const double n) {
	v.x = (int) Round ((double) v.x / n),
	v.y = (int) Round ((double) v.y / n),
	v.z = (int) Round ((double) v.z / n);
	return *this;
	}

inline const CFixVector CFixVector::operator* (const int n) const {
	return CFixVector (FixMul (v.x, n), FixMul (v.y, n), FixMul (v.z, n));
	}

inline const CFixVector CFixVector::operator/ (const int n) const {
	return CFixVector (FixDiv (v.x , n), FixDiv (v.y , n), FixDiv (v.z , n));
	}

inline const CFixVector CFixVector::operator* (CFixVector other) const {
	return CFixVector (FixMul (v.x, other.v.x), FixMul (v.y, other.v.y), FixMul (v.z, other.v.z));
	}

inline const CFixVector CFixVector::operator/ (CFixVector other) const {
	return CFixVector (FixDiv (v.x, other.v.x), FixDiv (v.y, other.v.y), FixDiv (v.z, other.v.z));
	}

inline const int CFixVector::operator^ (const CFixVector& other) const {
	return FixMul (v.x, other.v.x) + FixMul (v.y, other.v.y) + FixMul (v.z, other.v.z);
	}

inline const int CFixVector::Mag (void) { return D2X (CDoubleVector (*this).Mag ()); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

inline CFloatVector::operator CDoubleVector() {
	return CDoubleVector (double (v.x) / 65536.0, double (v.y) / 65536.0, double (v.z) / 65536.0);
}

inline const CFloatVector& CFloatVector::operator= (const tFixVector& other) { 
	v.x = X2F (other.x), v.y = X2F (other.y), v.z = X2F (other.z); 
	return *this;
	}

inline const bool CFloatVector::operator== (const CFloatVector other) {
	return (v.x == other.v.x) && (v.y == other.v.y) && (v.z == other.v.z);
}

inline float& CFloatVector::operator[] (const size_t i) { return ((float*) &v) [i]; }

inline const CFloatVector& CFloatVector::operator= (const CFloatVector& other) { 
	v.x = other.v.x, v.y = other.v.y, v.z = other.v.z; 
	return *this;
	}

inline const CFloatVector& CFloatVector::operator= (const tDoubleVector& other) { 
	v.x = float (other.x), v.y = float (other.y), v.z = float (other.z); 
	return *this;
	}

inline const CFloatVector& CFloatVector::operator= (const CDoubleVector& other) { 
	v.x = float (other.v.x), v.y = float (other.v.y), v.z = float (other.v.z); 
	return *this;
	}

inline const CFloatVector& CFloatVector::operator+= (const CFloatVector other) {
	v.x += other.v.x, v.y += other.v.y, v.z += other.v.z; 
	return *this;
	}

inline const CFloatVector& CFloatVector::operator-= (const CFloatVector other) {
	v.x -= other.v.x, v.y -= other.v.y, v.z -= other.v.z; 
	return *this;
	}

inline const CFloatVector CFloatVector::operator+ (const CFloatVector& other) const {
	return CFloatVector (v.x + other.v.x, v.y + other.v.y, v.z + other.v.z);
	}

inline const CFloatVector CFloatVector::operator- (const CFloatVector& other) const {
	return CFloatVector (v.x - other.v.x, v.y - other.v.y, v.z - other.v.z);
	}

inline const CFloatVector CFloatVector::operator- (void) const {
	return CFloatVector (-v.x, -v.y, -v.z);
	}

inline const CFloatVector& CFloatVector::operator*= (const float n) {
	v.x *= n, v.y *= n, v.z *= n;
	return *this;
	}

inline const CFloatVector& CFloatVector::operator/= (const float n) {
	if (n != 0.0)
		v.x /= n, v.y /= n, v.z /= n;
	return *this;
	}

inline const CFloatVector CFloatVector::operator* (const float n) const {
	return CFloatVector (v.x * n, v.y * n, v.z * n);
	}

inline const CFloatVector CFloatVector::operator/ (const float n) const {
	return CFloatVector (v.x / n, v.y / n, v.z / n);
	}

inline const CFloatVector CFloatVector::operator* (CFloatVector other) const {
	return CFloatVector (v.x * other.v.x, v.y * other.v.y, v.z * other.v.z);
	}

inline const CFloatVector CFloatVector::operator/ (CFloatVector other) const {
	return CFloatVector (v.x / other.v.x, v.y / other.v.y, v.z / other.v.z);
	}

inline const float CFloatVector::operator^ (const CFloatVector& other) const {
	return v.x * other.v.x + v.y * other.v.y + v.z * other.v.z;
	}

static inline const CFloatVector CrossProduct (const CFloatVector& v0, const CFloatVector& v1) {
	return CFloatVector (v0.v.y * v1.v.z - v0.v.z * v1.v.y, v0.v.z * v1.v.x - v0.v.x * v1.v.z, v0.v.x * v1.v.y - v0.v.y * v1.v.x);
	}

static inline CFloatVector Perpendicular (const CFloatVector& p0, const CFloatVector& p1, const CFloatVector& p2) {
	return CrossProduct (p1 - p0, p2 - p1);
	}

static inline const CFloatVector Normalize (CFloatVector v) { 
	float m = v.Mag ();
	return (m != 0.0) ? v / m : CFloatVector (0.0, 0.0, 0.0); 
	}

static inline const CFloatVector Normal (const CFloatVector& p0, const CFloatVector& p1, const CFloatVector& p2) {
	return Normalize (CrossProduct (p1 - p0, p2 - p0));
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

inline CDoubleVector::operator CFixVector() {
	CFixVector v (int (v.x * 65536.0), int (v.y * 65536.0), int (v.z * 65536.0));
	return v;
}

inline const bool CDoubleVector::operator== (const CDoubleVector other) {
	return (v.x == other.v.x) && (v.y == other.v.y) && (v.z == other.v.z);
}

inline const bool CDoubleVector::operator!= (const CDoubleVector other) {
	return (v.x != other.v.x) || (v.y != other.v.y) || (v.z != other.v.z);
}

inline double& CDoubleVector::operator[] (const size_t i) { return ((double*) &v) [i]; }

inline const CDoubleVector& CDoubleVector::operator= (const tDoubleVector& other) { 
	v.x = other.x, v.y = other.y, v.z = other.z; 
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator= (const CDoubleVector& other) { 
	v.x = other.v.x, v.y = other.v.y, v.z = other.v.z; 
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator= (const tFixVector& other) { 
	v.x = X2D (other.x), v.y = X2D (other.y), v.z = X2D (other.z); 
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator= (const CFixVector& other) { 
	v.x = X2D (other.v.x), v.y = X2D (other.v.y), v.z = X2D (other.v.z); 
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator+= (const CDoubleVector& other) {
	v.x += other.v.x, v.y += other.v.y, v.z += other.v.z; 
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator+= (const CFixVector& other) {
	v.x += X2D (other.v.x), v.y += X2D (other.v.y), v.z += X2D (other.v.z); 
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator-= (const CDoubleVector& other) {
	v.x -= other.v.x, v.y -= other.v.y, v.z -= other.v.z; 
	return *this;
	}

inline const CDoubleVector CDoubleVector::operator+ (const CDoubleVector& other) const {
	return CDoubleVector (v.x + other.v.x, v.y + other.v.y, v.z + other.v.z);
	}

inline const CDoubleVector CDoubleVector::operator- (const CDoubleVector& other) const {
	return CDoubleVector (v.x - other.v.x, v.y - other.v.y, v.z - other.v.z);
	}

inline const CDoubleVector CDoubleVector::operator- (void) const {
	return CDoubleVector (-v.x, -v.y, -v.z);
	}

inline const CDoubleVector& CDoubleVector::operator*= (const double n) {
	v.x *= n, v.y *= n, v.z *= n;
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator/= (const double n) {
	v.x /= n, v.y /= n, v.z /= n;
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator*= (const CDoubleVector& other) {
	v.x *= other.v.x, v.y *= other.v.y, v.z *= other.v.z;
	return *this;
	}

inline const CDoubleVector& CDoubleVector::operator/= (const CDoubleVector& other) {
	v.x /= other.v.x, v.y /= other.v.y, v.z /= other.v.z;
	return *this;
	}

inline const CDoubleVector CDoubleVector::operator/ (const double n) {
	return CDoubleVector (v.x / n, v.y / n, v.z / n);
	}

inline const CDoubleVector CDoubleVector::operator* (const double n) {
	return CDoubleVector (v.x * n, v.y * n, v.z * n);
	}

inline const CDoubleVector CDoubleVector::operator* (const CDoubleVector& other) const {
	return CDoubleVector (v.x * other.v.x, v.y * other.v.y, v.z * other.v.z);
	}

inline const CDoubleVector CDoubleVector::operator/ (const CDoubleVector& other) const {
	return CDoubleVector (v.x / other.v.x, v.y / other.v.y, v.z / other.v.z);
	}

inline const double CDoubleVector::operator^ (const CDoubleVector& other) const {
	return double (v.x) * double (other.v.x) + double (v.y) * double (other.v.y) + double (v.z) * double (other.v.z);
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static inline const CDoubleVector CrossProduct (const CDoubleVector& v0, const CDoubleVector& v1) {
	return CDoubleVector (v0.v.y * v1.v.z - v0.v.z * v1.v.y, v0.v.z * v1.v.x - v0.v.x * v1.v.z, v0.v.x * v1.v.y - v0.v.y * v1.v.x);
	}

static inline CDoubleVector Perpendicular (const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2) {
	return CrossProduct (p1 - p0, p2 - p1);
	}

static inline const CDoubleVector Normalize (CDoubleVector v) { 
	double m = v.Mag ();
	return (m != 0.0) ? v / m : CDoubleVector (0.0, 0.0, 0.0); 
	}

static inline const CDoubleVector Normal (const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2) {
	return Normalize (CrossProduct (p1 - p0, p2 - p0));
	}

static inline const double Normal (CDoubleVector& normal, const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2) {
	normal = CrossProduct (p1 - p0, p2 - p0);
	double m = normal.Mag ();
	if (m > 0.0)
		normal /= m;
	return m;
	}

static inline int Dot (const CFixVector& v0, const CFixVector& v1) {
	return int ((double (v0.v.x) * double (v1.v.x) + double (v0.v.y) * double (v1.v.y) + double (v0.v.z) * double (v1.v.z)) / 65536.0);
	}

static inline CFixVector Min (const CFixVector& v0, const CFixVector& v1) {
	return CFixVector(min (v0.v.x, v1.v.x), min (v0.v.y, v1.v.y), min (v0.v.z, v1.v.z));
	}

static inline CFixVector Max (const CFixVector& v0, const CFixVector& v1) {
	return CFixVector(max (v0.v.x, v1.v.x), max (v0.v.y, v1.v.y), max (v0.v.z, v1.v.z));
	}

static inline double Distance (const CFixVector& p0, const CFixVector& p1) {
	CFixVector v = p0 - p1;
	return D2X (CDoubleVector (v).Mag ());
	}

static inline CFixVector Average (const CFixVector& p0, const CFixVector& p1) {
	CFixVector v = p0 + p1;
	v /= 2.0;
	return v;
	}

static inline double Dot (const CDoubleVector& v0, const CDoubleVector& v1) {
	return double (v0.v.x) * double (v1.v.x) + double (v0.v.y) * double (v1.v.y) + double (v0.v.z) * double (v1.v.z);
	}

static inline CDoubleVector Min (const CDoubleVector& v0, const CDoubleVector& v1) {
	return CDoubleVector(min (v0.v.x, v1.v.x), min (v0.v.y, v1.v.y), min (v0.v.z, v1.v.z));
	}

static inline CDoubleVector Max (const CDoubleVector& v0, const CDoubleVector& v1) {
	return CDoubleVector(max (v0.v.x, v1.v.x), max (v0.v.y, v1.v.y), max (v0.v.z, v1.v.z));
	}

static inline double Distance (const CDoubleVector& p0, const CDoubleVector& p1) {
	CDoubleVector v = p0 - p1;
	return v.Mag ();
	}

static inline CDoubleVector Average (const CDoubleVector& p0, const CDoubleVector& p1) {
	CDoubleVector v = p0 + p1;
	v /= double (2);
	return v;
	}

CDoubleVector& PointLineIntersection (CDoubleVector& intersection, const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2);

int PlaneLineIntersection (CDoubleVector& intersection, CDoubleVector* vPlane, CDoubleVector* vNormal, CDoubleVector* p0, CDoubleVector* p1);

double PointLineDistance (const CDoubleVector& p0, const CDoubleVector& p1, const CDoubleVector& p2);

CDoubleVector ProjectPointOnLine (CDoubleVector* pLine, CDoubleVector* vLine, CDoubleVector* point);

CDoubleVector ProjectPointOnPlane (CDoubleVector* vPlane, CDoubleVector* vNormal, CDoubleVector* point);

bool PointIsInTriangle2D (CLongVector &p, CLongVector &a, CLongVector &b, CLongVector &c);

bool PointIsInTriangle2D (CDoubleVector &p, CDoubleVector &a, CDoubleVector &b, CDoubleVector &c);

bool PointIsInTriangle3D (CDoubleVector &p, CDoubleVector v0, CDoubleVector v1, CDoubleVector v2);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif // __vector_h


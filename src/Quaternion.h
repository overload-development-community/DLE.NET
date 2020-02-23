#ifndef __Quaternion_h
#define __Quaternion_h

#include <math.h>
#include "Vector.h"
#include "Matrix.h"

class CQuaternion : public CDoubleVector {
	public:
		double m_w;

	CQuaternion () : m_w (0.0) {}

	CQuaternion (double x, double y, double z, double w) : CDoubleVector (x, y, z), m_w (w) {}

	CQuaternion (CDoubleVector v, double w) : CDoubleVector (v), m_w (w) {}

	CQuaternion& operator= (CQuaternion other) {
		v = other.v;
		m_w = other.m_w;
		return *this;
		}

	inline double Angle (void) { return m_w; }

	CQuaternion GetConjugate (void) { return CQuaternion (-*((CDoubleVector*) this), m_w); };

	CQuaternion& Normalize (void);

	CQuaternion CQuaternion::operator* (CQuaternion other);

	CDoubleVector CQuaternion::operator* (CDoubleVector v);

	CQuaternion& FromAxisAngle (CDoubleVector axis, double angle);

	CQuaternion& FromEuler (double pitch, double yaw, double roll);

	CQuaternion& FromMatrix (CDoubleMatrix& m);

	CDoubleMatrix& ToMatrix (CDoubleMatrix& m);

	void ToAxisAngle (CDoubleVector& axis, double& angle);
	};

#endif //__Quaternion_h

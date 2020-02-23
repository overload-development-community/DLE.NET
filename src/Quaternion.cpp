
#include "stdafx.h"

#include <math.h>
#include "Vector.h"
#include "Quaternion.h"

// -----------------------------------------------------------------------------

CQuaternion& CQuaternion::Normalize (void)
{
	// Don't normalize if we don't have to
double m = sqrt (SqrMag () + m_w * m_w);
if (fabs (m) > 1e-30) {
	dynamic_cast<CDoubleVector&>(*this) /= m;
	m_w /= m;
	}
return *this;
}

// -----------------------------------------------------------------------------

CQuaternion CQuaternion::operator* (CQuaternion other)
{
double w = Angle () * other.Angle () - Dot (*this, other);
CDoubleVector va = CrossProduct (*this, other);
CDoubleVector vb = *((CDoubleVector*) this) * other.Angle ();
CDoubleVector vc = CDoubleVector (other) * Angle ();
va += vb;
va += vc;
CQuaternion q (va, w);
return q.Normalize ();
}

// -----------------------------------------------------------------------------

// Multiplying a quaternion q with a vector v applies the q-rotation to v
CDoubleVector CQuaternion::operator* (CDoubleVector v)
{
return (*this * CQuaternion (::Normalize (v), 0.0)) * GetConjugate ();
}

// -----------------------------------------------------------------------------

CQuaternion& CQuaternion::FromAxisAngle (CDoubleVector axis, double angle)
{
axis.Normalize ();
angle *= 0.5;
*((CDoubleVector*) this) = axis * sin (angle);
m_w = cos (angle);
Normalize ();
return *this;
}

// -----------------------------------------------------------------------------

CQuaternion& CQuaternion::FromEuler (double pitch, double yaw, double roll)
{
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just shorter
 
double p = Radians (pitch) * 0.5;
double y = Radians (yaw) * 0.5;
double r = Radians (roll) * 0.5;
 
double sinp = sin (p);
double siny = sin (y);
double sinr = sin (r);
double cosp = cos (p);
double cosy = cos (y);
double cosr = cos (r);
 
v.x = sinr * cosp * cosy - cosr * sinp * siny;
v.y = cosr * sinp * cosy + sinr * cosp * siny;
v.z = cosr * cosp * siny - sinr * sinp * cosy;
m_w = cosr * cosp * cosy + sinr * sinp * siny;
 
Normalize ();
return *this;
}

// -----------------------------------------------------------------------------

CQuaternion& CQuaternion::FromMatrix (CDoubleMatrix& m)
{
	double s;
	double t = m.m.rVec.v.x + m.m.uVec.v.y + m.m.fVec.v.z + 1.0;
	
if (t > 0.0) {
	s = 0.5 / sqrt (t);
   m_w = 0.25 / s;
   v.x = (m.m.fVec.v.y - m.m.uVec.v.z) * s;
   v.y = (m.m.rVec.v.z - m.m.fVec.v.x) * s;
   v.z = (m.m.uVec.v.x - m.m.rVec.v.y) * s;
	}
else {
	int column = (m.m.rVec.v.x >= m.m.uVec.v.y) ? (m.m.rVec.v.x >= m.m.fVec.v.z) ? 0 : 2 : (m.m.uVec.v.y >= m.m.fVec.v.z) ? 1 : 2;
	if (column == 0) {
      s = sqrt (1.0 + m.m.rVec.v.x - m.m.uVec.v.y - m.m.fVec.v.z) * 2.0;
      v.x = 0.5 / s;
      v.y = (m.m.rVec.v.y + m.m.uVec.v.x) / s;
      v.z = (m.m.rVec.v.z + m.m.fVec.v.y) / s;
      m_w = (m.m.uVec.v.z + m.m.fVec.v.z) / s;
		}
	else if (column == 1) {
      s = sqrt (1.0 + m.m.uVec.v.y - m.m.rVec.v.x - m.m.fVec.v.z) * 2.0;
      v.x = (m.m.rVec.v.y + m.m.uVec.v.x) / s;
      v.y = 0.5 / s;
      v.z = (m.m.uVec.v.z + m.m.fVec.v.z) / s;
      m_w = (m.m.rVec.v.z + m.m.fVec.v.y) / s;
		}
	else {
      s = sqrt (1.0 + m.m.fVec.v.z - m.m.rVec.v.x - m.m.uVec.v.y) * 2.0;
      v.x = (m.m.rVec.v.z + m.m.fVec.v.y) / s;
      v.y = (m.m.uVec.v.z + m.m.fVec.v.z) / s;
      v.z = 0.5 / s;
      m_w = (m.m.rVec.v.y + m.m.uVec.v.x) / s;
		}
	}

Normalize ();
return *this;
}

// -----------------------------------------------------------------------------

CDoubleMatrix& CQuaternion::ToMatrix (CDoubleMatrix& m)
{
double x2 = v.x * v.x * 2.0;
double y2 = v.y * v.y * 2.0;
double z2 = v.z * v.z * 2.0;
double xy = v.x * v.y * 2.0;
double xz = v.x * v.z * 2.0;
double yz = v.y * v.z * 2.0;
double xw = v.x * m_w * 2.0;
double yw = v.y * m_w * 2.0;
double zw = v.z * m_w * 2.0;
 
// This calculation would be a lot more complicated for non-unit length quaternions
// Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
//   OpenGL
m.Set (1.0 - y2 - z2, xy - zw, xz + yw, 
		 xy + zw, 1.0 - x2 - z2, yz - xw, 
		 xz - yw, yz + xw, 1.0 - x2 - y2);
return m;
}

// -----------------------------------------------------------------------------

void CQuaternion::ToAxisAngle (CDoubleVector& axis, double& angle)
{
Normalize ();

angle = acos (m_w) * 2.0 * PI;
double sina = sqrt (1.0 - m_w * m_w);
if (fabs (sina) < 1e-10)
	sina = 1.0;
axis = dynamic_cast<CDoubleVector&>(*this) * sina;
}

// -----------------------------------------------------------------------------
#if 0 // usage example
void Camera::movex(float xmmod)
{
	pos += rotation * Vector3(xmmod, 0.0f, 0.0f);
}
 
void Camera::movey(float ymmod)
{
	pos.v.y -= ymmod;
}
 
void Camera::movez(float zmmod)
{
	pos += rotation * Vector3(0.0f, 0.0f, -zmmod);
}
 
void Camera::rotatex(float xrmod)
{
	Quaternion nrot(Vector3(1.0f, 0.0f, 0.0f), xrmod * PIOVER180);
	rotation = rotation * nrot;
}
 
void Camera::rotatey(float yrmod)
{
	Quaternion nrot(Vector3(0.0f, 1.0f, 0.0f), yrmod * PIOVER180);
	rotation = nrot * rotation;
}
 
void Camera::tick(float seconds)
{
	if (xrot != 0.0f) rotatex(xrot * seconds * rotspeed);
	if (yrot != 0.0f) rotatey(yrot * seconds * rotspeed);
 
	if (xmov != 0.0f) movex(xmov * seconds * movespeed);
	if (ymov != 0.0f) movey(ymov * seconds * movespeed);
	if (zmov != 0.0f) movez(zmov * seconds * movespeed);
}
#endif
// -----------------------------------------------------------------------------

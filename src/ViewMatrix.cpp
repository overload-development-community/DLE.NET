#include "stdafx.h"

#include <math.h>
#include "define.h"
#include "types.h"
#include "mine.h"
#include "global.h"
#include "matrix.h"
#include "dle-xp.h"

double CViewMatrix::m_depthScaleTable [4] = {10000.0, 400.0, 250.0, 150.0};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

CViewMatrix::CViewMatrix () : m_nSaved (0)
{
m_data [0].m_translate.Clear ();
m_data [0].m_origin.Clear ();
m_data [0].m_scale = CDoubleVector (1.0, 1.0, 1.0);
Setup (0,0,0,1,1,1,0,0,0);
}

// -----------------------------------------------------------------------------

void CViewMatrix::SetViewInfo (short viewWidth, short viewHeight)
{
m_viewWidth = viewWidth / 2;
m_viewHeight = viewHeight / 2;
}

//--------------------------------------------------------------------------
// M = Rz * Ry * Rx

void CViewMatrix::Setup (CDoubleVector vTranslate, CDoubleVector vScale, CDoubleVector vRotate)
{
CDoubleVector sinSpin (sin (vRotate.v.x), sin (vRotate.v.y), sin (vRotate.v.z));
CDoubleVector cosSpin (cos (vRotate.v.x), cos (vRotate.v.y), cos (vRotate.v.z));
#if 0
m_data [0].m_transformation [0].Set (cy * cz, 
												 cy * sz, 
												 -sy,
												 sx * sy * cz - cx * sz,
												 sx * sy * sz + cx * cz,
												 sx * cy,
												 cx * sy * cz - sx * sz,
												 cx * sy * sz - sx * cz,
												 cx * cy);
#else
m_data [0].m_transformation [0].Set (vScale.v.x * cosSpin.v.z * cosSpin.v.y, 
												 vScale.v.y * sinSpin.v.z * cosSpin.v.y, 
												 vScale.v.z * -sinSpin.v.y,
												 vScale.v.x * -sinSpin.v.z * cosSpin.v.x + vScale.v.x * cosSpin.v.z * sinSpin.v.y * sinSpin.v.x,
												 vScale.v.y * sinSpin.v.z * sinSpin.v.y * sinSpin.v.x + vScale.v.y * cosSpin.v.z * cosSpin.v.x,
												 vScale.v.z * cosSpin.v.y * sinSpin.v.x,
												 vScale.v.x * cosSpin.v.z * sinSpin.v.y * cosSpin.v.x - vScale.v.x * sinSpin.v.z * sinSpin.v.x,
												 vScale.v.y * sinSpin.v.z * sinSpin.v.y * cosSpin.v.x - vScale.v.y * cosSpin.v.z * sinSpin.v.x,
												 vScale.v.z * cosSpin.v.y * cosSpin.v.x);
#endif
m_data [0].m_translate = vTranslate;
m_data [0].m_scale = vScale;
m_data [0].m_rotate = vRotate;
m_data [0].m_transformation [1] = m_data [0].m_transformation [0].Inverse ();
}

//--------------------------------------------------------------------------

void CViewMatrix::Stuff (CDoubleVector fVec, CDoubleVector uVec, CDoubleVector rVec) 
{
m_data [0].m_transformation [0].m.fVec = fVec;
m_data [0].m_transformation [0].m.uVec = uVec;
m_data [0].m_transformation [0].m.rVec = rVec;
m_data [0].m_transformation [1] = m_data [0].m_transformation [0].Inverse ();
Rotation ().Clear ();
}

//--------------------------------------------------------------------------

void CViewMatrix::Stuff (CDoubleMatrix& m) 
{
m_data [0].m_transformation [0] = m;
m_data [0].m_transformation [1] = m.Inverse ();
Rotation ().Clear ();
}

//--------------------------------------------------------------------------

void CViewMatrix::ClampAngle (int i)
{
if (m_data [0].m_rotate [i] < 0)
	m_data [0].m_rotate [i] += (int) (-m_data [0].m_rotate [i] / 360.0) * 360.0;
else
	m_data [0].m_rotate [i] -= (int) (m_data [0].m_rotate [i] / 360.0) * 360.0;
}

//--------------------------------------------------------------------------

void CViewMatrix::RotateAngle (int i, double a)
{
m_data [0].m_rotate [i] += a;
}

//--------------------------------------------------------------------------

bool CViewMatrix::Push (void)
{
if (m_nSaved > 0)
	return false;
++m_nSaved;
memcpy (&m_data [1], &m_data [0], sizeof (CViewData));
return true;
}

//--------------------------------------------------------------------------

bool CViewMatrix::Pop (void)
{
if (m_nSaved < 1)
	return false;
--m_nSaved;
memcpy (&m_data [0], &m_data [1], sizeof (CViewData));
return true;
}

//--------------------------------------------------------------------------

void CViewMatrix::Translate (CDoubleVector vTranslate) 
{
m_data [0].m_translate = vTranslate;
}

//--------------------------------------------------------------------------

void CViewMatrix::Scale (CDoubleVector vScale) 
{
#if 1
m_data [0].m_scale *= vScale;
Setup (m_data [0].m_translate, m_data [0].m_scale, m_data [0].m_rotate);
#else
CDoubleMatrix s (scale.v.x, 0.0, 0.0, 0.0, scale.v.y, 0.0, 0.0, 0.0, scale.v.z);
CDoubleMatrix r = m_data [0].m_transformation [0];
m_data [0].m_transformation [0] = r * s;
m_data [0].m_transformation [1] = m_data [0].m_transformation [0].Inverse ();
m_data [0].m_scale *= scale;
#endif
}

//--------------------------------------------------------------------------

void CViewMatrix::Rotate (char axis, double angle) 
{
if (angle) {
#if 0
	if (Perspective ()) {
		switch(axis) {
			default:
			case 'X':
				RotateAngle (0, -angle);
				break;
			case 'Y':
				RotateAngle (1, -angle);
				break;
			case 'Z':
				RotateAngle (2, -angle);
				break;
			}
		Setup (m_data [0].m_translate, m_data [0].m_scale, m_data [0].m_rotate);
		}
	else 
#endif
		{
		//angle = -angle;
		double cosa = (double) cos (angle);
		double sina = (double) sin (angle);
		CDoubleMatrix m, r;
		switch (axis) {
			default:
			case 'X':
				RotateAngle (0, angle);
				m.Set (1.0, 0.0, 0.0, 0.0, cosa, sina, 0.0, -sina, cosa);
				break;
			case 'Y':
				RotateAngle (1, angle);
				m.Set (cosa, 0.0, -sina, 0.0, 1.0, 0.0, sina, 0.0, cosa);
				break;
			case 'Z':
				RotateAngle (2, angle);
				m.Set (cosa, sina, 0.0, -sina, cosa, 0.0, 0.0, 0.0, 1.0);
				break;
			}
#if 0
		if (Perspective ()) {
			r = m_data [0].m_transformation [0];
			m_data [0].m_transformation [0] = r * m;
			m_data [0].m_transformation [1] = m_data [0].m_transformation [0].Inverse ();
			}
		else 
#endif
			{
			r = m_data [0].m_transformation [1];
			m_data [0].m_transformation [1] = r * m;
			m_data [0].m_transformation [0] = m_data [0].m_transformation [1].Inverse ();
			}
		}
	}
}

//--------------------------------------------------------------------------

void CViewMatrix::Transform (CDoubleVector& v, CDoubleVector& w) 
{
v = w;
v += m_data [0].m_translate;
v = m_data [0].m_transformation [0] * v;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#if 0

void CViewMatrixSW::Scale (double scale) 
{
CDoubleMatrix s (scale.v.x, 0.0, 0.0, 0.0, scale.v.y, 0.0, 0.0, 0.0, scale.v.z);
CDoubleMatrix r = m_data [0].m_transformation [1];
m_data [0].m_transformation [1] = r * s;
m_data [0].m_scale *= scale;
}

//--------------------------------------------------------------------------

void CViewMatrixSW::Rotate (char axis, double angle) 
{
if (angle) {
	double cosa = (double) cos (angle);
	double sina = (double) sin (angle);
	CDoubleMatrix m, r;
	switch(axis) {
		default:
		case 'X':
			m.Set (1.0, 0.0, 0.0, 0.0, cosa, sina, 0.0, -sina, cosa);
			RotateAngle (0, angle);
			break;
		case 'Y':
			m.Set (cosa, 0.0, -sina, 0.0, 1.0, 0.0, sina, 0.0, cosa);
			RotateAngle (1, angle);
			break;
		case 'Z':
			m.Set (cosa, sina, 0.0, -sina, cosa, 0.0, 0.0, 0.0, 1.0);
			RotateAngle (2, angle);
			break;
		}
	r = m_data [0].m_transformation [1];
	m_data [0].m_transformation [1] = r * m;
	}
}

#endif

//--------------------------------------------------------------------------

void CViewMatrixSW::Transform (CDoubleVector& v, CDoubleVector& w) 
{
v = w;
v += m_data [0].m_translate;
v = m_data [0].m_transformation [0] * v;
double scale = 5.0;
if ((m_depthScale < 10000.0) && (v.v.z > -m_depthScale)) 
	scale *= m_depthScale / (v.v.z + m_depthScale);
v *= CDoubleVector (scale, scale, 1.0);
}

//--------------------------------------------------------------------------

void CViewMatrixSW::Project (CLongVector& a, CDoubleVector v) 
{
a.x = ((long) (Round (v.v.x) + m_viewWidth) % 32767);
a.y = ((long) (m_viewHeight - Round (v.v.y)) % 32767);
a.z = (long) Round (v.v.z * 10000.0); // 5 digits precision
}

//--------------------------------------------------------------------------

void CViewMatrixSW::Unproject (CVertex& vWorld, CLongVector& vScreen, CPoint& viewCenter) 
{
CDoubleVector r (double (vScreen.x - viewCenter.x), double (viewCenter.y - vScreen.y), double (vScreen.z) / 10000.0);
double scale = 0.2;
if ((m_depthScale < 10000.0) && (r.v.z > -m_depthScale)) 
	scale *= (r.v.z + m_depthScale) / m_depthScale;
r *= CDoubleVector (scale, scale, 1.0);
CDoubleVector v = m_data [0].m_transformation [1] * r;
v -= m_data [0].m_translate;
vWorld = v;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void CViewMatrixGL::Setup (CDoubleVector vTranslate, CDoubleVector vScale, CDoubleVector vRotate)
{
#if 0
CViewMatrix::Setup (vTranslate, vScale, vRotate);
#else
CDoubleVector sinSpin (sin (vRotate.v.x), sin (vRotate.v.y), sin (vRotate.v.z));
CDoubleVector cosSpin (cos (vRotate.v.x), cos (vRotate.v.y), cos (vRotate.v.z));

CDoubleMatrix rX (1.0, 0.0, 0.0, 0.0, cosSpin.v.x, sinSpin.v.x, 0.0, -sinSpin.v.x, cosSpin.v.x);
CDoubleMatrix rY (cosSpin.v.y, 0.0, -sinSpin.v.y, 0.0, 1.0, 0.0, sinSpin.v.y, 0.0, cosSpin.v.y);
CDoubleMatrix rZ (cosSpin.v.z, sinSpin.v.z, 0.0, -sinSpin.v.z, cosSpin.v.z, 0.0, 0.0, 0.0, 1.0);
rX.Mul (rY);
rX.Mul (rZ);

#if 0
m_data [0].m_transformation [0].Set (cosSpin.v.z * cosSpin.v.y, 
												 sinSpin.v.z * cosSpin.v.y, 
												 -sinSpin.v.y,
												 -sinSpin.v.z * cosSpin.v.x + cosSpin.v.z * sinSpin.v.y * sinSpin.v.x,
												 sinSpin.v.z * sinSpin.v.y * sinSpin.v.x + cosSpin.v.z * cosSpin.v.x,
												 cosSpin.v.y * sinSpin.v.x,
												 cosSpin.v.z * sinSpin.v.y * cosSpin.v.x - sinSpin.v.z * sinSpin.v.x,
												 sinSpin.v.z * sinSpin.v.y * cosSpin.v.x - cosSpin.v.z * sinSpin.v.x,
												 cosSpin.v.y * cosSpin.v.x);
#else
m_data [0].m_transformation [0].Set (vScale.v.x * cosSpin.v.z * cosSpin.v.y, 
												 vScale.v.y * sinSpin.v.z * cosSpin.v.y, 
												 vScale.v.z * -sinSpin.v.y,
												 vScale.v.x * -sinSpin.v.z * cosSpin.v.x + vScale.v.x * cosSpin.v.z * sinSpin.v.y * sinSpin.v.x,
												 vScale.v.y * sinSpin.v.z * sinSpin.v.y * sinSpin.v.x + vScale.v.y * cosSpin.v.z * cosSpin.v.x,
												 vScale.v.z * cosSpin.v.y * sinSpin.v.x,
												 vScale.v.x * cosSpin.v.z * sinSpin.v.y * cosSpin.v.x - vScale.v.x * sinSpin.v.z * sinSpin.v.x,
												 vScale.v.y * sinSpin.v.z * sinSpin.v.y * cosSpin.v.x - vScale.v.y * cosSpin.v.z * sinSpin.v.x,
												 vScale.v.z * cosSpin.v.y * cosSpin.v.x);
#endif
m_data [0].m_translate = vTranslate;
m_data [0].m_scale = vScale;
m_data [0].m_rotate = vRotate;
m_data [0].m_transformation [1] = m_data [0].m_transformation [0].Inverse ();
#endif
}

//--------------------------------------------------------------------------

static void GLRotate (CDoubleMatrix* orient, double rDir)
{
if (orient) {
	double m [16];
	double* o = (double*) &orient [0];
	m [0] = o [0] * rDir;
	m [4] = o [1] * rDir;
	m [8] = o [2] * rDir;
	m [1] = o [3];
	m [5] = o [4];
	m [9] = o [5];
	m [2] = o [6];
	m [6] = o [7];
	m [10] = o [8];
	m [3] =
	m [7] =
	m [11] =
	m [12] =
	m [13] =
	m [14] = 0.0f;
	m [15] = 1.0f;
	glMultMatrixd (m);
	}
}

void CViewMatrixGL::SetupOpenGL (CDoubleMatrix* orient, CDoubleVector* position)
{
glMatrixMode (GL_MODELVIEW);
glPushMatrix ();

if (orient || position) {
	if (position) {
		if (Perspective ())
			glTranslated (-position->v.x, position->v.y, -position->v.z);
		else
			glTranslated (position->v.x, position->v.y, position->v.z);
		}
	if (Perspective ())
		GLRotate (orient, -1.0);
	else {
		GLRotate (orient, 1.0);
		glScalef (1.0, 1.0, -1.0);
		}
	}
else {
	glLoadIdentity ();
	position = &Translation ();
	if (Perspective ()) {
		GLRotate (&Transformation (), -1.0);
		glTranslated (position->v.x, position->v.y, position->v.z);
		glScalef (1.0, -1.0, 1.0);
		}	
	else {
		glTranslated (position->v.x, -position->v.y, -position->v.z);
		GLRotate (&Transformation (), -1.0);
		glTranslated (m_data [0].m_origin.v.x, m_data [0].m_origin.v.y, m_data [0].m_origin.v.z);
		glRotatef (180.0, 0.0, 1.0, 0.0);
		glScalef (1.0, -1.0, 1.0);
		}	
	}
}

//--------------------------------------------------------------------------

void CViewMatrixGL::ResetOpenGL (void)
{
glMatrixMode (GL_MODELVIEW);
glPopMatrix ();
}

//--------------------------------------------------------------------------

void CViewMatrixGL::Scale (double scale) 
{
if (scale < 1.0)
	m_data [0].m_translate.v.z -= 1.0;
else
	m_data [0].m_translate.v.z += 1.0;
}

//--------------------------------------------------------------------------

#if 0

void CViewMatrixGL::Translate (char axis, double angle) 
{
glTranslated (-m_data [0].m_translate.x, -m_data [0].m_translate.y, -m_data [0].m_translate.z);
}

//--------------------------------------------------------------------------

void CViewMatrixGL::Rotate (char axis, double angle) 
{
if (angle) {
	switch (axis) {
		default:
		case 'X':
			RotateAngle (0, angle);
			glRotated (m_data [0].m_rotate.v.x, 1.0, 0.0, 0.0);
			break;
		case 'Y':
			RotateAngle (1, angle);
			glRotated (m_data [0].m_rotate.v.y, 0.0, 1.0, 0.0);
			break;
		case 'Z':
			RotateAngle (2, angle);
			glRotated (m_data [0].m_rotate.v.z, 0.0, 0.0, 1.0);
			break;
		}
	}
else {
	glRotated (m_data [0].m_rotate.v.x, 1.0, 0.0, 0.0);
	glRotated (m_data [0].m_rotate.v.y, 0.0, 1.0, 0.0);
	glRotated (m_data [0].m_rotate.v.z, 0.0, 0.0, 1.0);
	}
}

#endif

//--------------------------------------------------------------------------

void CViewMatrixGL::Transform (CDoubleVector& v, CDoubleVector& w) 
{
v = w;
if (m_nPerspective) {
	v -= m_data [0].m_translate;
	v = m_data [0].m_transformation [0] * v;
	}
else {
	v -= m_data [0].m_origin;
	v = m_data [0].m_transformation [0] * v;
	v += m_data [0].m_translate;
	}
}

//--------------------------------------------------------------------------

void CViewMatrixGL::Project (CLongVector& a, CDoubleVector v) 
{
CDoubleVector s;
Project (s, v);
a.x = (long) Round (s.v.x);
a.y = (long) Round (s.v.y);
a.z = (long) Round (s.v.z * 10000.0); // 5 digits precision
}

//--------------------------------------------------------------------------

void CViewMatrixGL::Project (CDoubleVector& s, CDoubleVector& v) 
{
gluProject (v.v.x, v.v.y, v.v.z, 
				m_modelView, m_projection, m_viewport, 
				&s.v.x, &s.v.y, &s.v.z);
}

//--------------------------------------------------------------------------

void CViewMatrixGL::Unproject (CDoubleVector& v, CDoubleVector& s) 
{
gluUnProject (s.v.x, s.v.y, s.v.z, 
				  m_modelView, m_projection, m_viewport, 
				  &v.v.x, &v.v.y, &v.v.z);
}

//--------------------------------------------------------------------------

void CViewMatrixGL::Unproject (CVertex& vWorld, CLongVector& vScreen, CPoint& viewCenter) 
{
	CDoubleVector vView;

gluUnProject (GLdouble (vScreen.x), GLdouble (vScreen.y), GLdouble (vScreen.z) / 10000.0, 
				  m_modelView, m_projection, m_viewport, 
				  &vView.v.x, &vView.v.y, &vView.v.z);
if (m_nPerspective) {
	vWorld = m_data [0].m_transformation [1] * vView;
	vWorld += m_data [0].m_translate;
	}
else {
	vView -= m_data [0].m_translate;
	vWorld = m_data [0].m_transformation [1] * vView;
	vWorld += m_data [0].m_origin;
	}
}

//--------------------------------------------------------------------------

void CViewMatrixGL::GetProjection (void)
{
glGetDoublev (GL_PROJECTION_MATRIX, m_projection); 
glGetDoublev (GL_MODELVIEW_MATRIX, m_modelView); 
glGetIntegerv (GL_VIEWPORT, m_viewport);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// eof
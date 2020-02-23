// matrix.h
#ifndef __viewmatrix_h
#define __viewmatrix_h

#include "matrix.h"
#include "glew.h"

// -----------------------------------------------------------------------------

class CViewData {
	public:
		CDoubleMatrix	m_transformation [2];
		CDoubleVector	m_translate;
		CDoubleVector	m_rotate;
		CDoubleVector	m_scale;
		CDoubleVector	m_origin;
};

// -----------------------------------------------------------------------------

class CViewMatrix
{
	public:
		CViewData		m_data [2];
		int				m_nSaved;

		static double m_depthScaleTable [4];

	protected:
		double			m_depthScale;
		short				m_viewWidth;
		short				m_viewHeight;

	public:

	protected:
		void ClampAngle (int i);
		void RotateAngle (int i, double a);

	public:
		CViewMatrix ();

		virtual void Setup (CDoubleVector vMove, CDoubleVector vSize, CDoubleVector vSpin);
		virtual void Scale (CDoubleVector vScale);
		virtual void Rotate (char axis, double angle);
		virtual void Transform (CDoubleVector& v, CDoubleVector& w);
		virtual void Project (CLongVector& a, CDoubleVector v) = 0;
		virtual void Project (CDoubleVector& s, CDoubleVector& v) = 0;
		virtual void Unproject (CDoubleVector& s, CDoubleVector& v) = 0;
		virtual void Unproject (CVertex& vWorld, CLongVector& vScreen, CPoint& viewCenter) = 0;
		virtual void GetProjection (void) = 0;
		virtual int Perspective (void) = 0;

		virtual void SetupOpenGL (CDoubleMatrix* orient = null, CDoubleVector* position = null) {}
		virtual void ResetOpenGL (void) {}

		inline void Setup (
			double xMove, double yMove, double zMove, 
			double xSize, double ySize, double zSize,
			double xSpin, double ySpin, double zSpin)
			{ Setup (CDoubleVector (xMove, yMove, zMove), CDoubleVector (xSize, ySize, zSize), CDoubleVector (xSpin, ySpin, zSpin)); }

		void Stuff (CDoubleVector fVec, CDoubleVector uVec, CDoubleVector rVec);
		void Stuff (CDoubleMatrix& m);

		CDoubleVector CViewMatrix::Angles (void) { return Transformation ().Angles (); }

		void Translate (CDoubleVector vTranslate);

		void SetViewInfo (short viewWidth, short viewHeight);

		inline void Scale (double fScale) { Scale (CDoubleVector (fScale, fScale, fScale)); }
		inline short ViewWidth (void) { return m_viewWidth; }
		inline short ViewHeight (void) { return m_viewHeight; }
		inline double Aspect (void) { return (double) m_viewHeight / (double) m_viewWidth; }
		bool Push (void);
		bool Pop (void);

		inline int DepthPerception (double scale = -1.0) {
			if (scale < 0.0)
				scale = m_depthScale;
			for (int i = 0; i < 4; i++)
				if (scale >= m_depthScaleTable [i])
					return i;
			return 3;
			}
		inline double DepthScale (void) { return m_depthScale; }
		inline void SetDepthScale (int i) { m_depthScale = m_depthScaleTable [i]; }
		//inline void SetDepthScale (double scale) { SetDepthScale (DepthPerception (scale)); }

		inline CDoubleMatrix& Transformation (int i = 0) { return m_data [0].m_transformation [i]; }
		inline CDoubleVector& Forward (void) { return Transformation ().m.fVec; }
		inline CDoubleVector Up (void) { return Transformation ().m.uVec; }
		inline CDoubleVector Right (void) { return Transformation ().m.rVec; }
		inline CDoubleVector Backward (void) { return -Transformation ().m.fVec; }
		inline CDoubleVector Down (void) { return -Transformation ().m.uVec; }
		inline CDoubleVector Left (void) { return -Transformation ().m.rVec; }

		inline CDoubleVector& Translation (void) { return m_data [0].m_translate; }
		inline CDoubleVector& Rotation (void) { return m_data [0].m_rotate; }
		inline CDoubleVector& Scale (void) { return m_data [0].m_scale; }
		inline CDoubleVector& Origin (void) { return m_data [0].m_origin; }

		inline void MoveViewer (int axis, double offset) {
			m_data [0].m_translate [axis] += offset;
			m_data [0].m_origin [axis] += offset;
			}

		inline double Distance (CDoubleVector vPos) { 
			CDoubleVector vView;
			Transform (vView, vPos);
			return vView.Mag ();
			}
	};

// -----------------------------------------------------------------------------

class CViewMatrixSW : public CViewMatrix {
	public:
#if 0
		virtual void Scale (double scale);
		virtual void Rotate (char axis, double angle);
#endif
		virtual void Transform (CDoubleVector& v, CDoubleVector& w);
		virtual void Project (CLongVector& a, CDoubleVector v);
		virtual void Project (CDoubleVector& s, CDoubleVector& v) {
			CLongVector a;
			Project (a, v);
			s.v.x = a.x;
			s.v.y = a.y;
			s.v.z = double (a.z) / 10000.0;
			}

		virtual void Unproject (CDoubleVector& s, CDoubleVector& v) {}
		virtual void Unproject (CVertex& vWorld, CLongVector& vScreen, CPoint& viewCenter);
		virtual void GetProjection (void) {}
		virtual int Perspective (void) { return 0; }
};

// -----------------------------------------------------------------------------

class CViewMatrixGL : public CViewMatrix {
	private:
		int				m_nPerspective; // 0: 3rd person, 1: 1st person

	public:
		GLdouble			m_modelView [16];
		GLdouble			m_projection [16];
		GLint				m_viewport [4];

	public:
		CViewMatrixGL () : m_nPerspective (0) {}

		virtual void Setup (CDoubleVector vTranslate, CDoubleVector vScale, CDoubleVector vRotate);
		virtual void Scale (double scale);

		virtual void SetupOpenGL (CDoubleMatrix* orient = null, CDoubleVector* position = null);
		virtual void ResetOpenGL (void);

#if 0
		virtual void Rotate (char axis, double angle);
#endif
		virtual void Transform (CDoubleVector& v, CDoubleVector& w);
		virtual void Project (CLongVector& a, CDoubleVector v);
		virtual void Project (CDoubleVector& s, CDoubleVector& v);
		virtual void Unproject (CDoubleVector& s, CDoubleVector& v);
		virtual void Unproject (CVertex& vWorld, CLongVector& vScreen, CPoint& viewCenter);
		virtual void GetProjection (void);

		inline void SetPerspective (int nPerspective) { m_nPerspective = nPerspective; }
		virtual int Perspective (void) { return m_nPerspective; }

};

// -----------------------------------------------------------------------------

#endif //__viewmatrix_h
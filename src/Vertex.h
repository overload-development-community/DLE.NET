#ifndef __vertex_h
#define __vertex_h

#include "glew.h"

// -----------------------------------------------------------------------------

class CViewMatrix;

class CVertex : public CDoubleVector, public CGameItem {
	private:
		ubyte	m_status;

	public:
		CDoubleVector	m_view;
		CDoubleVector	m_proj;
		CDoubleVector	m_delta;
		CLongVector		m_screen;

	public:
		void Read (CFileManager* fp = 0, bool bFlag = false);

		void Write (CFileManager* fp = 0, bool bFlag = false);

		inline const CVertex& operator= (const CVertex& other) { 
			v = other.v;
			m_view = other.m_view;
			m_proj = other.m_proj;
			m_screen = other.m_screen;
			m_status = other.m_status; 
			m_delta = other.m_delta;
			return *this;
			}
		inline const CVertex& operator= (const CDoubleVector& other) { 
			v = other.v; 
			return *this;
			}

		inline const CVertex& operator= (const CPoint& point) { 
			v.x = point.x;
			v.y = point.y;
			}

		inline ubyte& Status (void) { return m_status; }

		virtual void Clear (void);

		virtual CGameItem* Clone (void);

		virtual void Backup (eEditType editType = opModify);

		virtual CGameItem* Copy (CGameItem* pDest);

		virtual void Undo (void);

		virtual void Redo (void);

		inline void Tag (ubyte mask = TAGGED_MASK) { m_status |= mask; }

		inline void UnTag (ubyte mask = TAGGED_MASK) { m_status &= ~mask; }

		inline void ToggleTag (ubyte mask = TAGGED_MASK) { m_status ^= mask; }

		inline bool IsTagged (ubyte mask = TAGGED_MASK) { return (m_status & mask) != 0; }

		void Project (CViewMatrix* m);

		void Transform (CViewMatrix* m);

		inline bool InRange (long xMax, long yMax, int nRenderer) { return  (m_screen.x >= 0) && (m_screen.y >= 0) && (m_screen.x < xMax) && (m_screen.y < yMax) && (!nRenderer || (m_view.v.z >= 0.0)); }

		inline void GLVertex (CDoubleVector& vTranslate) { glVertex3d (m_view.v.x, m_view.v.y, m_view.v.z - vTranslate.v.z); }

		inline CDoubleVector& Delta (void) { return m_delta; }

		inline void SetDelta (CDoubleVector& vDelta) { m_delta += vDelta; }

		inline void Move (void) {
			*this += m_delta;
			m_delta.Clear ();
			}

		// c'tors
		CVertex () : CGameItem (itVertex), m_status(0) {}
		
		CVertex (double x, double y, double z) : CDoubleVector (x, y, z) { m_status = 0; }
		
		CVertex (tDoubleVector& _v) : CDoubleVector (_v) { m_status = 0; }
		
		CVertex (CDoubleVector _v) : CDoubleVector (_v) { m_status = 0; }

		CVertex (CLongVector _p) : CDoubleVector (double (_p.x), double (_p.y), double (_p.z)) { m_status = 0; }

		CVertex (CPoint _p) : CDoubleVector (double (_p.x), double (_p.y), 0.0) { m_status = 0; }

	};

// -----------------------------------------------------------------------------

#endif //__vertex_h
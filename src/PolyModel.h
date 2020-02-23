#ifndef __polymodel_h
#define __polymodel_h

#include "Vertex.h"
#include "GameObject.h"
#include "ViewMatrix.h"
#include "Renderer.h"

//------------------------------------------------------------------------------

typedef struct tSubModel {
  int 			ptr;
  CFixVector 	offset;
  CFixVector 	norm;		// norm for sep plane
  CFixVector 	pnt;		// point on sep plane
  int 			rad;		// radius for each submodel
  ubyte 			parent;  // what is parent for each submodel
  CFixVector 	vMin;
  CFixVector   vMax;
} tSubModel;

//------------------------------------------------------------------------------
//used to describe a polygon model

typedef struct tPolyModel {
  int				nModels;
  int 			dataSize;
  ubyte*			renderData;
  tSubModel		subModels [MAX_SUBMODELS];
  CFixVector 	vMin, vMax;			  // min, max for whole model
  int				rad;
  ubyte			textureCount;
  ushort			firstTexture;
  ubyte			simplerModel;			  // alternate model with less detail (0 if none, nModel+1 else)
} tPolyModel;

//------------------------------------------------------------------------------

class CPolyModel {
public:
	tPolyModel		m_info;
	CViewMatrix*	m_view;
	CDC*				m_pDC;
	int				m_pt, m_pt0;
	int				m_bCustom;

	CPolyModel () : m_bCustom (0) { Clear (); }
	
	~CPolyModel () { Release (); }
	
	inline void Clear (void) { memset (&m_info, 0, sizeof (m_info)); }

	inline void Release (void) {
		if (m_info.renderData) {
			delete m_info.renderData;
			m_info.renderData = null;
			}
		}

	void Read (CFileManager* fp, bool bRenderData = false);

	void Write (CFileManager* fp, bool bRenderData = false);

	inline bool Draw (CViewMatrix* view, CDC* pDC, int nStage) {
		if (!m_info.renderData) 
			return false;
		m_view = view;
		m_pDC = pDC;
		Render (m_info.renderData, nStage);
		return true;
		}

	private:
		void Render (ubyte* p, int nStage);
};

//------------------------------------------------------------------------------

#endif //__polymodel_h

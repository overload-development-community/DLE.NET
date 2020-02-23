#ifndef __poly_h
#define __poly_h

#include "ViewMatrix.h"

/* the followin numbers are set using the original D2 robots */
#define MAX_POLYMODEL_POINTS			416
#define MAX_POLYMODEL_POLYS			300
#define MAX_POLYMODEL_POLY_POINTS   13
#define MIN_POLYMODEL_POLY_POINTS	3
#define MAX_POLY_MODEL_SIZE			32000

typedef struct tModelRenderPoly {
public:
	ushort		n_verts;
	CVertex		offset;
	CVertex		normal;
	ushort		nBaseTex;
	ushort		color;
	ushort		glow_num;
	ushort		verts [MAX_POLYMODEL_POLY_POINTS];
	tUVL			uvls [MAX_POLYMODEL_POLY_POINTS];

	void Draw (CViewMatrix* view, CDC* pDC);
} tModelRenderPoly;

typedef struct tModelRenderData {
	ushort				n_points;
	CVertex				points [MAX_POLYMODEL_POINTS];
	ushort				n_polys;
	tModelRenderPoly	polys [MAX_POLYMODEL_POLYS];
} tModelRenderData;

typedef struct tSubModel {
  int 			ptr;
  CFixVector 	offset;
  CFixVector 	norm;		// norm for sep plane
  CFixVector 	pnt;		// point on sep plane
  int 			rad;		// radius for each submodel
  byte 			parent;  // what is parent for each submodel
  CFixVector 	vMin;
  CFixVector   vMax;
} tSubModel;

//used to describe a polygon model
typedef struct tPolyModel {
  int			nModels;
  int 			dataSize;
  byte*			renderData;
  tSubModel		subModels [MAX_SUBMODELS];
  CFixVector 	vMin, vMax;			  // min, max for whole model
  int				rad;
  byte			textureCount;
  ushort			firstTexture;
  byte			simplerModel;			  // alternate model with less detail (0 if none, nModel+1 else)
} tPolyModel;

class CPolyModel {
public:
	tPolyModel		m_info;
	CViewMatrix*	m_view;
	CDC*				m_pDC;

	CPolyModel () { Clear (); }
	~CPolyModel () { Release (); }
	inline void Clear (void) { 
		memset (&m_info, 0, sizeof (m_info)); 
		}
	inline void Release (void) {
		if (m_info.renderData) {
			delete m_info.renderData;
			m_info.renderData = null;
			}
		}
	void Read (CFileManager& fp, bool bRenderData = false);
	void Write (CFileManager& fp, bool bRenderData = false);
	inline void CPolyModel::Draw (CViewMatrix* view, CDC* pDC) {
		if (m_info.renderData) {
			m_view = view;
			m_pDC = pDC;
			Render (m_info.renderData);
			}
		}

	private:
		void SetModelPoints (int start, int end);
		void Render (byte* p);
};

extern CPolyModel* renderModel;

#endif //__poly_h

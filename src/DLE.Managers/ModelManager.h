#ifndef __ModelManager_h
#define __ModelManager_h

//------------------------------------------------------------------------------

class CModelRenderPoly {
public:
	ushort		nVerts;
	CVertex		offset;
	CVertex		normal;
	short			nTexture;
	rgbColord	color;
	ushort		nGlow;
	ushort		index [MAX_POLYMODEL_POLY_POINTS];
	tTexCoord2d	texCoords [MAX_POLYMODEL_POLY_POINTS];

	void Draw (void);
};

//------------------------------------------------------------------------------

class CModelRenderData {
	public:
		ushort				nPoints;
		CVertex				points [MAX_POLYMODEL_POINTS];
		ushort				nPolys;
		CModelRenderPoly	polys [MAX_POLYMODEL_POLYS];
	};


//------------------------------------------------------------------------------

class CModelManager : public IModelManager {
	friend class CPolyModel;
	friend class CModelRenderPoly;

	private:
		CGameObject*		m_object;
		CModelRenderData	m_data;
		CModelRenderPoly*	m_face;
		IRenderer*			m_renderer;
		IViewMatrix*		m_viewMatrix;
		CVertex				m_offset;
		CDoubleVector		m_normal;
		CLongVector			m_screenRect [2];
		ushort				m_textureIndex [2][MAX_OBJ_TEXTURES];
		ushort				m_modelTextureIndex [2][MAX_OBJ_TEXTURES];
		tAnimationInfo		m_animations [MAX_ANIMATIONS];
		CVertex				m_screenPoly [MAX_POLYMODEL_POINTS];
		int					m_nPolyPoints;
		int					m_lastObjType;
		int					m_lastObjId;
		int					m_nModel;
		int					m_nGlow;
		int					m_nTextures;
		int					m_nIndices;

#if _DEBUG
		CStaticArray< CPolyModel, MAX_POLYGON_MODELS > m_polyModels [2];
		CStaticArray< ASE::CModel, MAX_POLYGON_MODELS > m_aseModels;
		CStaticArray< OOF::CModel, MAX_POLYGON_MODELS > m_oofModels;
		CStaticArray< RenderModel::CModel, MAX_POLYGON_MODELS > m_renderModels;
#else
		CPolyModel	m_polyModels [2][MAX_POLYGON_MODELS];
		ASE::CModel m_aseModels [MAX_POLYGON_MODELS];
		OOF::CModel m_oofModels [MAX_POLYGON_MODELS];
		RenderModel::CModel m_renderModels [MAX_POLYGON_MODELS];
#endif

	public:
		CModelManager () : m_nGlow(-1), m_lastObjType(-1), m_lastObjId(-1) {}

		int Setup (CGameObject* pObject, IRenderer* renderer);

		void Reset (void);

		void Draw (void);

		inline int CheckNormal (CFixVector& a, CFixVector& b) { return m_object->CheckNormal (*m_viewMatrix, a, b); }

		inline int CheckNormal (CVertex& a, CVertex& b) { return m_object->CheckNormal (*m_viewMatrix, a, b); }

		void SetPoints (int start, int end);

		void Polygon (ushort* index, int nVertices, tTexCoord2d* texCoords, rgbColord* color, short nTexture);

		inline tAnimationInfo* Animation (int i) { return (i < MAX_ANIMATIONS) ? &m_animations [i] : null; }

		inline int ScreenRad (void) {
			int dx = abs (m_screenRect [1].x - m_screenRect [0].x);
			int dy = abs (m_screenRect [1].y - m_screenRect [0].y);
			return (dx < dy) ? dy / 2 : dx / 2;
			}

		void ReadCustomModelData (ubyte* buffer, long bufSize, int bCustom = 1);

		void ReadMod (char* pszFolder);

		void LoadMod (void);

		int BuildModel (CObject* pObject, int nModel, CPolyModel* pp, CDynamicArray<CTexture*>& modelTextures);

		virtual void Render (ubyte* p, int nStage, int bCustom, ushort firstTexture);

	private:
		int Model (void);

		int ReadModelData (char* filename, char *szSubFile, bool bCustom);

		void ReadCustomModelData (CMemoryFile& mf, int bCustom);
	};

extern CModelManager modelManager;

//-----------------------------------------------------------------------

#endif //__ModelManager_h

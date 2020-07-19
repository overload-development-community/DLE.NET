#ifndef __textureprojector_h
#define __textureprojector_h

#define PROJECTION_MODE_PLANAR		1
#define PROJECTION_MODE_CYLINDER	2

class CPreviewUVL {
	public:
		short	m_nSegment;
		short	m_nSide;
		CUVL	m_uvlPreview [4];
		CUVL	m_uvlOld [4];

	public:
		CPreviewUVL (short nSegment = -1, short nSide = -1) :
			m_nSegment(nSegment), m_nSide(nSide) {
			ZeroMemory (m_uvlPreview, sizeof (m_uvlPreview));
			ZeroMemory (m_uvlOld, sizeof (m_uvlOld));
			}
		void Apply (void);
		void Revert (void);
};

class CTextureProjector
{
public:
	bool				m_bActive;
	int					m_nProjectionMode;
	CDoubleMatrix		m_projectOrient;
	CDoubleVector		m_vCenter;
	double				m_projectScaleU;
	double				m_projectScaleV;
	BOOL				m_bProjectPreview;
	BOOL				m_bProjectCopiedOnly;

	CTextureProjector(void);
	~CTextureProjector(void);
	
	void Start (int nProjectionMode);
	void End (BOOL bApply);
	void ResetProjectOffset();
	void ResetProjectDirection ();
	void ResetProjectScaling ();
	double CalcAverageRadius ();
	void Project (CDynamicArray< CPreviewUVL > *previewUVLs = null);
	bool ShouldProjectFace (short nSegment, short nSide);
	void DrawProjectGuides (CRenderer& renderer, CViewMatrix* viewMatrix);
};

extern CTextureProjector textureProjector;

#endif //__textureprojector_h
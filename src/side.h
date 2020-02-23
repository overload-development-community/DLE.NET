
#ifndef __side_h
#define __side_h

#include "define.h"
#include "Types.h"
#include "Vertex.h"
#include "FileManager.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tSide {
	short		nParent;
	ushort	nChild;
	ushort	nWall;		// (was short) Index into Walls array, which wall (probably door) is on this side 
	short		nBaseTex;	// Index into array of textures specified in bitmaps.bin 
	short		nOvlTex;		// Index, as above, texture which gets overlaid on nBaseTex 
	CUVL		uvls [4];   // CUVL coordinates at each point 
	CUVL		uvlDeltas [4];
} tSide;

// -----------------------------------------------------------------------------

#define TEXTURE_MASK		0x3fff
#define ALIGNMENT_MASK	0xc000
#define ALIGNMENT_SHIFT 14

class CSide {
public:
	tSide				m_info;
	ubyte				m_vertexIdIndex [4]; // table of the indices of the side's four vertices' ids in the side's parent segment's vertex id table
	ubyte				m_nShape;
	ubyte				m_nTag;
	ubyte				m_nPoint;
	CDoubleVector	m_vNormal [3];
	CVertex			m_vCenter;
	CSide*			m_link;

	static short vertexCounts [4];
	static short faceCounts [4];

	inline tSide& Info (void) { return m_info; }

	void Read (CFileManager* fp, bool bTextured);

	void Write (CFileManager* fp, bool bTextured);

	void Clear (void) { memset (&m_info, 0, sizeof (m_info)); }
	
	void Setup (short nSide);
	
	void LoadTextures (void);
	
	void GetTextures (short &nBaseTex, short &nOvlTex) _const_;
	
	bool SetTextures (int nBaseTex, int nOvlTex);

	inline ubyte Point (void) { return m_nPoint; }
	
	inline ubyte SetPoint (ubyte nPoint) { return m_nPoint = nPoint; }
	
	void InitUVL (short nTexture);
	
	inline void SetWall (short nWall) { m_info.nWall = nWall; }

	inline short BaseTex (void) _const_ { return m_info.nBaseTex; }
	
	inline short OvlTex (int bWithAlignment = 1) _const_ { return bWithAlignment ? m_info.nOvlTex : m_info.nOvlTex & TEXTURE_MASK; }

	inline short OvlAlignment (void) _const_ { return short (ushort (m_info.nOvlTex & ALIGNMENT_MASK) >> ALIGNMENT_SHIFT); }

	inline CUVL _const_ * Uvls (int i = 0) _const_ { return &m_info.uvls [i]; }
	
	CSegment _const_ * Child (void) _const_;

	CWall _const_ * Wall (void) _const_;
	
	inline ushort _const_ WallId (void) _const_ { return m_info.nWall; }
	
	CTrigger _const_ * Trigger (void) _const_;
	
	bool UpdateChild (short nOldChild, short nNewChild);

	void DeleteWall (void);

	bool IsVisible (void);

	inline void SetShape (ubyte nShape) { m_nShape = nShape; }

	inline ubyte Shape (void) { return m_nShape; }

	void Reset (short nSide);

	void ResetTextures (void);

	void UpdateTexCoords (ubyte nPoint, ushort* nVertexIds);

	void FindAndUpdateTexCoords (ubyte nIdIndex, ushort* nVertexIds, bool bMove);

	void MoveTexCoords (void);

	inline short VertexCount (void) { return vertexCounts [m_nShape]; }

	inline ubyte VertexIdIndex (short nIndex) { return m_vertexIdIndex [nIndex % VertexCount ()]; }

	inline short FaceCount (void) { return faceCounts [m_nShape]; }

	bool GetEdgeIndices (short nLine, short& i1, short& i2);

	short FindVertexIdIndex (ubyte nIndex);

	bool HasVertex (ubyte nIndex);

	ubyte DetectShape (void);

	inline CVertex& Center (void) { return m_vCenter; }

	inline CDoubleVector& Normal (int i = 2) { return m_vNormal [i]; } // return the average of the side's two triangles normals by default

	CDoubleVector& ComputeNormals (ushort* vertexIds, CDoubleVector& vCenter, bool bView = false);

	double LineHitsFace (CVertex* p1, CVertex* p2, ushort* vertexIds, double maxDist, bool bView = false);

	short IsSelected (CRect& viewport, long xMouse, long yMouse, ushort vertexIds []);

	short NearestEdge (CRect& viewport, long xMouse, long yMouse, ushort vertexIds [], double& minDist);

	inline CSide* SetLink (CSide* pSide) { return m_link = pSide; }

	inline CSide* GetLink (void) { return m_link; }

	inline void SetParent (short nParent) { m_info.nParent = nParent; }

	inline short GetParent (void) { return m_info.nParent; }

	inline void Tag (ubyte mask = TAGGED_MASK) { m_nTag |= mask; }

	inline void UnTag (ubyte mask = TAGGED_MASK) { m_nTag &= ~mask; }

	inline void ToggleTag (ubyte mask = TAGGED_MASK) { m_nTag ^= mask; }

	inline bool IsTagged (ubyte mask = TAGGED_MASK) { return (m_nTag & mask) != 0; }

	short Edge (short i1, short i2);

	CVertex& Vertex (ushort* vertexIds, ubyte nIndex);
	
private:
	void UpdateTexCoords (ubyte nPoint, ubyte nOtherPoint, ubyte nLine, CVertex& v1, CVertex& v2);
};

// -----------------------------------------------------------------------------

extern tUVL defaultUVLs [4];

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__side_h
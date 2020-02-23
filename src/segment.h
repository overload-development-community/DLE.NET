// Segment.h

#ifndef __segment_h
#define __segment_h

#include "define.h"
#include "Types.h"
#include "side.h"
#include "FileManager.h"

// -----------------------------------------------------------------------------
// define points for a given side 

extern ubyte sideVertexTable [6][4];
extern ubyte oppSideTable [6];
extern ubyte oppSideVertexTable [6][4];
extern ubyte edgeVertexTable [12][2];
extern ubyte vertexEdgeTable [8][3];
extern ubyte sideEdgeTable [6][4];
extern ubyte edgeSideTable [12][2];
extern ubyte adjacentPointTable [8][3];
extern ubyte adjacentSideTable [8][3];
extern ubyte pointCornerTable [8][3];
extern int sideChildTable[6][4];

// -----------------------------------------------------------------------------
//  Returns true if nSegment references a child, else returns false. 
//  Note that -1 means no connection, -2 means a connection to the outside world. 
#define  IS_CHILD(nSegment) (nSegment > -1)

#define SEGMENT_TYPE_NONE					0
#define SEGMENT_TYPE_PRODUCER				1
#define SEGMENT_TYPE_REPAIRCEN			2
#define SEGMENT_TYPE_CONTROLCEN			3
#define SEGMENT_TYPE_ROBOTMAKER			4
#define MAX_SEGMENT_TYPES1					5

#define SEGMENT_TYPE_GOAL_BLUE			5 // Descent 2 only
#define SEGMENT_TYPE_GOAL_RED				6 // Descent 2 only
#define SEGMENT_TYPE_WATER					7
#define SEGMENT_TYPE_LAVA					8
#define SEGMENT_TYPE_TEAM_BLUE			9
#define SEGMENT_TYPE_TEAM_RED				10
#define SEGMENT_TYPE_SPEEDBOOST			11
#define SEGMENT_TYPE_BLOCKED				12
#define SEGMENT_TYPE_NODAMAGE				13
#define SEGMENT_TYPE_SKYBOX				14
#define SEGMENT_TYPE_EQUIPMAKER			15	// matcen for powerups
#define SEGMENT_TYPE_OUTDOORS				16
#define MAX_SEGMENT_TYPES2					17 // Descent 2 only

#define SEGMENT_FUNC_NONE					0
#define SEGMENT_FUNC_PRODUCER				1
#define SEGMENT_FUNC_REPAIRCEN			2
#define SEGMENT_FUNC_REACTOR				3
#define SEGMENT_FUNC_ROBOTMAKER			4
#define SEGMENT_FUNC_GOAL_BLUE			5
#define SEGMENT_FUNC_GOAL_RED				6
#define SEGMENT_FUNC_TEAM_BLUE			7
#define SEGMENT_FUNC_TEAM_RED				8
#define SEGMENT_FUNC_SPEEDBOOST			9
#define SEGMENT_FUNC_SKYBOX				10
#define SEGMENT_FUNC_EQUIPMAKER			11
#define MAX_SEGMENT_FUNCTIONS				12

#define SEGMENT_PROP_NONE					0
#define SEGMENT_PROP_WATER					1
#define SEGMENT_PROP_LAVA					2
#define SEGMENT_PROP_BLOCKED				4
#define SEGMENT_PROP_NODAMAGE				8
#define SEGMENT_PROP_SELF_ILLUMINATE	16
#define SEGMENT_PROP_LIGHT_FOG			32
#define SEGMENT_PROP_DENSE_FOG			64
#define SEGMENT_PROP_FOG					(SEGMENT_PROP_LIGHT_FOG | SEGMENT_PROP_DENSE_FOG)


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CEdgeIndex {
	public:
		ushort	m_nEdge;
		ubyte		m_sides [2];
		ubyte		m_nSides;

	CEdgeIndex () { Reset (); }
	void Reset (void) { 
		m_nEdge = 0xffff;
		m_sides [0] = m_sides [1] = 0xff;
		m_nSides = 0; 
		}
};

// -----------------------------------------------------------------------------

class CEdgeList {
	public:
		CEdgeIndex	m_edgeList [12];
		int			m_nEdges;

		CEdgeList () : m_nEdges (0) {
			for (int i = 0; i < 12; i++)
				m_edgeList [i].Reset ();
			}

		static inline ushort Key (ushort v1, ushort v2) { return (v1 < v2) ? v2 + (v1 << 8) : v1 + (v2 << 8); }

		inline void Reset (void) { m_nEdges = 0; }

		int Add (ubyte nSide, ubyte v1, ubyte v2); 

		inline int Count (void) { return m_nEdges; }

		inline bool Get (int i, ubyte& side1, ubyte& side2, ubyte &v1, ubyte& v2) { 
			if (i >= m_nEdges) 
				return false;
			side1 = m_edgeList [i].m_sides [0];
			side2 = m_edgeList [i].m_sides [1];
			ushort nEdge = m_edgeList [i].m_nEdge;
			v1 = ubyte (nEdge & 0xff);
			v2 = ubyte (nEdge >> 8);
			return true;
			}

		int Find (int i, ubyte& side1, ubyte& side2, ubyte v1, ubyte v2);
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tSegment {
	int		staticLight;		// average static light in segment 
	ushort	vertexIds [MAX_VERTICES_PER_SEGMENT];	// vertex ids of 4 front and 4 back vertices 
	short		damage [2];
	short		nProducer;			// which object producer the segment is associated with
	short		value;				// index of producer the segment is associated with
	short		mapBitmask;			// which lines are drawn when displaying wireframe 
	ubyte		function;			// general function (robot maker, energy center, ... - mutually exclusive)
	ubyte		props;				// special property of a segment (such as damaging, trigger, etc.)
	ubyte		flags;				// internally used
	ubyte		childFlags;			// bit0 to 5: children, bit6: unused, bit7: special 
	ubyte		wallFlags;			// bit0 to 5: door/walls, bit6: deleted, bit7: marked segment 
	ubyte		bTunnel;				// part of a tunnel?
	char		owner;
	char		group;
} tSegment;

// -----------------------------------------------------------------------------

class CSegment : public CGameItem {
public:
	tSegment			m_info;
	CSide				m_sides [MAX_SIDES_PER_SEGMENT];		// 6 sides 
	CVertex			m_vCenter;
	ubyte				m_nShape;

public:
	void Upgrade (void);

	void Read (CFileManager* fp);
	
	void Write (CFileManager* fp);
	
	void ReadExtras (CFileManager* fp, bool bExtras);
	
	void WriteExtras (CFileManager* fp, bool bExtras);
	
	virtual void Clear (void) { 
		memset (&m_info, 0, sizeof (m_info)); 
		for (int i = 0; i < MAX_SIDES_PER_SEGMENT; i++)
			m_sides [i].Clear ();
		}

	inline tSegment& Info (void) { return m_info; }

	void Setup (void);

	void SetUV (short nSide, double x, double y);

	void GetTriangleUVs (CDoubleVector (&triangleUVs) [3], const CDoubleVector (&triangleVecs) [3]);
	//void Read (CFileManager* fp, bool bFlag = false) {}
	//void Write (CFileManager* fp, bool bFlag = false) {}

	inline short SideIndex (CSide* pSide) { return short (pSide - &m_sides [0]); }

	inline CSide _const_ * Side (short i = 0) _const_ { return ((i < 0) || (i > 6)) ? null : &m_sides [i]; }

	inline CSide _const_ * OppositeSide (short i) _const_ { return Side (oppSideTable [i]); }

	CVertex _const_ * Vertex (ushort nVertex) _const_;

	CVertex _const_ * Vertex (ushort nSide, short nIndex) _const_;

	inline ushort* VertexIds (void) { return m_info.vertexIds; }

	inline ushort VertexId (short i) { return m_info.vertexIds [i]; }

	inline ubyte VertexIdIndex (short nSide, short nPoint) { return m_sides [nSide].VertexIdIndex (ubyte (nPoint)); }

	inline ushort VertexId (short nSide, short nPoint) { return VertexId (VertexIdIndex (nSide, nPoint)); }

	inline void SetVertexId (short i, ushort nId) { m_info.vertexIds [i] = nId; }

	inline void SetVertexId (short nSide, short nPoint, ushort nId) { SetVertexId (VertexIdIndex (nSide, nPoint), nId); }

	int UpdateVertexId (ushort nOldId, ushort nNewId);

	inline short ChildId (short nSide) _const_ { return (m_sides [nSide].m_nShape > SIDE_SHAPE_TRIANGLE) ? -1 : m_sides [nSide].m_info.nChild; }

	CSegment* Child (short nSide) _const_;

	short SetChild (short nSide, short nSegment);

	bool ReplaceChild (short nOldSeg, short nNewSeg);

	inline bool RemoveChild (short nSegment) { return ReplaceChild (nSegment, -1); }

	bool HasChild (short nChild) { return (m_info.childFlags & (1 << nChild)) != 0; }

	void Reset (short nSide = -1);

	void UpdateChildren (short nOldChild, short nNewChild);

	virtual CGameItem* Clone (void);

	virtual void Backup (eEditType editType = opModify);

	virtual CGameItem* Copy (CGameItem* pDest);

	virtual void Redo (void);

	virtual void Undo (void);

	inline CUVL _const_ * Uvls (short nSide) _const_ { return Side (nSide)->Uvls (); }

	void Tag (ubyte mask = TAGGED_MASK);

	void UnTag (ubyte mask = TAGGED_MASK);

	void ToggleTag (ubyte mask = TAGGED_MASK);

	inline bool IsTagged (ubyte mask = TAGGED_MASK) { return (m_info.flags & mask) != 0; }

	void Tag (short nSide, ubyte mask = TAGGED_MASK);

	void UnTag (short nSide, ubyte mask = TAGGED_MASK);

	void ToggleTag (short nSide, ubyte mask = TAGGED_MASK);

	bool IsTagged (short nSide, ubyte mask = TAGGED_MASK);

	void TagVertices (ubyte mask = TAGGED_MASK, short nSide = -1);

	void UnTagVertices (ubyte mask = TAGGED_MASK, short nSide = -1);

	bool HasTaggedVertices (ubyte mask = TAGGED_MASK, short nSide = -1);

	inline bool IsD2X (void) { return (m_info.function >= SEGMENT_FUNC_TEAM_BLUE) || (m_info.props != 0); }

	bool HasVertex (ushort nVertex);

	bool HasVertex (short nSide, ubyte nIndex);

	bool HasEdge (short nSide, ushort nVertex1, ushort nVertex2);

	inline CSide _const_ & operator[] (uint i) _const_ { return m_sides [i]; }

	short FindSide (short nSide, short nVertices, ushort* vertices);

	short CommonVertices (short nOtherSeg, short nMaxVertices = 4, ushort* vertices = null);

	short CommonSides (short nOtherSeg, short& nOtherSide);

	short CommonSide (short nSide, ushort* vertices);

	short VertexIndex (short nVertexId);

	short SideVertexIndex (short nSide, ubyte nPoint);

	inline ubyte& Function (void) { return m_info.function; }

	inline ubyte& Props (void) { return m_info.props; }

	short AdjacentSide (short nIgnoreSide, ushort* nEdgeVerts);

	inline CVertex& Center (void) { return m_vCenter; }

	int BuildEdgeList (CEdgeList& edgeList, ubyte nSide, bool bSparse = false);

	int BuildEdgeList (CEdgeList& edgeList, bool bSparse = false);

	void CreateOppVertexIndex (short nSide, ubyte* oppVertexIndex);

	inline void SetShape (ubyte nShape) { m_nShape = nShape; }

	inline ubyte Shape (void) { return m_nShape; }

	double MinEdgeLength (void);

	bool GetEdgeVertices (short nSide, short nLine, ushort& v1, ushort& v2);

	ubyte OppSideVertex (short nSide, short nIndex);

	void UpdateVertexIdIndex (ubyte nIndex);

	void ShiftVertices (short nSide);

	CVertex& ComputeCenter (bool bView = false);

	CVertex& ComputeCenter (short nSide);

	inline CVertex& ComputeCenter (CSide* pSide) { return ComputeCenter (SideIndex (pSide)); }

	void ComputeNormals (short nSide, bool bView = false);

	void UpdateTexCoords (ushort nVertexId, bool bMove, short nIgnoreSide = -1);

	short IsSelected (CRect& viewport, long xMouse, long yMouse, short nSide, bool bSegments);

	short Edge (short nSide, ushort v1, ushort v2);

	void MakeCoplanar (short nSide);

	CSegment () : CGameItem (itSegment) {}

private:
	ubyte ReadWalls (CFileManager* fp);

	ubyte WriteWalls (CFileManager* fp);

};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

typedef struct tProducer {
	int	objFlags [2];	// Up to 32 different Descent 1 robots 
	int   hitPoints;		// How hard it is to destroy this particular matcen 
	int   interval;		// Interval between materializations 
	short nSegment;		// Segment this is attached to. 
	short nProducer;		// Index in PRODUCER array. 
} tProducer;

// -----------------------------------------------------------------------------

class CObjectProducer : public CGameItem {
public:
	tProducer	m_info;

	void Read (CFileManager* fp, bool bFlag = false);
	
	void Write (CFileManager* fp, bool bFlag = false);

	void Setup (short nSegment, short nIndex, int nFlags) {
		Clear ();
		m_info.nSegment = nSegment;
		m_info.objFlags [0] = nFlags;
		m_info.nProducer = nIndex;
		};

	inline tProducer& Info (void) { return m_info; }

	virtual void Clear (void) { memset (&m_info, 0, sizeof (m_info)); }

	virtual CGameItem* Clone (void);

	virtual void Backup (eEditType editType = opModify);

	virtual CGameItem* Copy (CGameItem* pDest);

	inline bool operator< (CObjectProducer& other) { return m_info.nSegment < other.m_info.nSegment; }

	inline bool operator> (CObjectProducer& other) { return m_info.nSegment > other.m_info.nSegment; }

	inline CObjectProducer& operator= (CObjectProducer& other) { 
		m_info = other.m_info; 
		return *this;
		}

	//virtual void Undo (void);

	//virtual void Redo (void);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__segment_h
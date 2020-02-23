#ifndef __segman_h
#define __segman_h

#include "define.h"
#include "FileManager.h"
#include "carray.h"
#include "Types.h"
#include "Selection.h"
#include "MineInfo.h"
#include "FreeList.h"
#include "SLL.h"
#include "AVLTree.h"

// -----------------------------------------------------------------------------

#define MAX_SEGMENTS_D1				800  // descent 1 max # of cubes
#define MAX_SEGMENTS_D2				900  // descent 2 max # of cubes
#define SEGMENT_LIMIT				20000 // D2X-XL max # of cubes

#define MAX_SEGMENTS ((theMine == null) ? MAX_SEGMENTS_D2 : DLE.IsD1File () ? MAX_SEGMENTS_D1  : DLE.IsStdLevel () ? MAX_SEGMENTS_D2 : SEGMENT_LIMIT)

#define MAX_NUM_MATCENS_D1			20
#define MAX_NUM_MATCENS_D2			100

#define MAX_NUM_RECHARGERS_D2		70
#define MAX_NUM_RECHARGERS_D2X	500

#define MAX_NUM_RECHARGERS ((theMine == null) ? MAX_NUM_RECHARGERS_D2X : (DLE.IsD1File () || (DLE.LevelVersion () < 12)) ? MAX_NUM_RECHARGERS_D2 : MAX_NUM_RECHARGERS_D2X)

#define NUM_FOG_TYPES				4

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CEdgeTreeNode {
	public:
		uint								m_nKey;
		CSLL<CSideKey, CSideKey>	m_sides;

	explicit CEdgeTreeNode (uint nKey = 0) : m_nKey (nKey) {}
	~CEdgeTreeNode () {}
	void Destroy (void) { m_sides.Destroy (); }

	CSideKey* Insert (short nSegment, short nSide) {
		CSideKey	key = CSideKey (nSegment, nSide);
		CSideKey* keyP = m_sides.Find (key);
		return keyP ? keyP : m_sides.Append (key);
		}

	inline CEdgeTreeNode& operator= (CEdgeTreeNode& other) {
		m_nKey = other.m_nKey;
		m_sides = other.m_sides;
		return *this;
		}
	inline bool operator< (uint key) { return m_nKey < key; }
	inline bool operator> (uint key) { return m_nKey > key; }
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CEdgeKey {
	public:
		uint	m_key;

		static uint Key (ushort v1, ushort v2) { return(v1 < v2) ? (uint (v1) << 16) + v2 : (uint (v2) << 16) + v1; }

		uint Compose (ushort v1, ushort v2) { return m_key = Key (v1, v2); }
		ushort V1 (void) { return ushort (m_key & 0xffff); }
		ushort V2 (void) { return ushort (m_key >> 16); }
		inline uint Key (void) { return m_key; }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifdef _DEBUG

typedef CStaticArray< CSegment, SEGMENT_LIMIT > segmentList;
typedef CStaticArray< CObjectProducer, MAX_NUM_MATCENS_D2 > robotMakerList;

#else

typedef CSegment segmentList [SEGMENT_LIMIT];
typedef CStaticArray<CObjectProducer, MAX_NUM_MATCENS_D2> robotMakerList;

#endif

// -----------------------------------------------------------------------------

typedef struct tVertMatch {
		short		v;
		double	d;
	} tVertMatch; 

// -----------------------------------------------------------------------------

class CFogInfo {
	public:
		rgbColor			m_color;
		ubyte				m_density;

		void Init (int nType);
		void Read (CFileManager *fp);
		void Write (CFileManager *fp);
	};

// -----------------------------------------------------------------------------

class CSegmentManager {
	public:
		segmentList				m_segments;
		CMineItemInfo			m_segmentInfo;
		FREELIST(CSegment)
		int						m_nAddMode;
		robotMakerList			m_producers [2];
		CMineItemInfo			m_producerInfo [2];
		bool						m_bCreating;
		CSegment*				m_selectedSegments;
		CSide*					m_selectedSides;
		CFogInfo					m_fogInfo [NUM_FOG_TYPES];
		bool						m_bUpdateAlignmentOnEdit;

	public:
		inline void ResetInfo (void) {
			m_segmentInfo.Reset ();
			m_producerInfo [0].Reset ();
			m_producerInfo [1].Reset ();
#if USE_FREELST
			m_free.Reset ();
#endif
			}
		// Segment and side getters
		inline segmentList& Segments (void)	{ return m_segments; }

		inline int& Count (void) { return m_segmentInfo.count; }

		inline int& FileOffset (void) { return m_segmentInfo.offset; }

		inline int& ProducerCount (_const_ short nClass) { return m_producerInfo [nClass].count; }

		inline int& RobotMakerCount (void) { return ProducerCount (0); }

		inline int& EquipMakerCount (void) { return ProducerCount (1); }

		int ProducerCount (void);

		inline short Index (CSegment _const_ * pSegment) { return (short) (pSegment - &m_segments [0]); }

		inline CSegment _const_ * Segment (_const_ int i) { return &m_segments [i]; }

		inline CSegment _const_ * Segment (CSideKey& key) { return Segment (key.m_nSegment); }

		inline CSegment _const_ & GetSegment (_const_ int i) { return m_segments [i]; }

		inline void PutSegment (_const_ int i, _const_ CSegment& seg) { m_segments [i] = seg; }

		inline CSide _const_ * Side (CSideKey key) {
			current->Get (key);
			return m_segments [key.m_nSegment].m_sides + key.m_nSide;
			}

		inline CSide _const_ * Side (void) { return Side (CSideKey ()); }

		inline CSide _const_ & GetSide (CSideKey key) { return *Side (key); }

		inline void PutSide (CSideKey key, CSide& side) { 
			current->Get (key);
			m_segments [key.m_nSegment].m_sides [key.m_nSide] = side;
			}

		CSide _const_ * BackSide (CSideKey key, CSideKey& back);

		inline CSide _const_ * BackSide (CSideKey& back) { return BackSide (CSideKey (), back); }
		//CSide* OppositeSide (CSideKey opp, CSideKey& key, short nSegment = -1, short nSide = -1);

		//inline CWall* Wall (short nSegment = -1, short nSide = -1) {
		inline CWall _const_ * Wall (CSideKey key) { 
			current->Get (key);
			CSide _const_ * pSide;
			pSide = Side (key);
			return pSide->Wall (); 
			}

		inline CWall _const_ * Wall (CSideKey* key = null) { return Wall ((key == null) ? *current : *key); }

		bool IsExit (short nSegment);

		int IsWall (CSideKey key = CSideKey ());

		//inline CTrigger* Trigger (short nSegment = -1, short nSide = -1) {
		inline CTrigger _const_ * Trigger (CSideKey key) { return Side (key)->Trigger (); }

		inline CTrigger _const_ * Trigger (void) { return Trigger(CSideKey ()); }

		//inline CWall* OppositeWall (short nSegment = -1, short nSide = -1) {
		inline _const_ CWall* OppositeWall (CSideKey key) {
			_const_ CSide* pSide = Side (key);
			return (pSide == null) ? null : pSide->Wall ();
			}

		inline _const_ CWall* OppositeWall (void) { return OppositeWall (CSideKey ()); }

		void Textures (CSideKey key, short& nBaseTex, short& nOvlTex);

		inline robotMakerList& RobotMakers (void) { return m_producers [0]; }
		
		inline robotMakerList& EquipMakers (void) { return m_producers [1]; }
		
		inline CObjectProducer* Producer (int i, int nClass) { return &m_producers [nClass][i]; }
		
		inline CObjectProducer* RobotMaker (int i) { return Producer (i, 0); }
		
		inline CObjectProducer* EquipMaker (int i) { return Producer (i, 1); }

		inline int SetAddMode (int nMode) { return m_nAddMode = nMode; }

		inline int AddMode (void) { return m_nAddMode; }

		CSegment* FindRobotMaker (short i = 0);

		inline bool& UpdateAlignmentOnEdit (void) { return m_bUpdateAlignmentOnEdit; }

		// Operations
		short Add (void);

		void Remove (short nDelSeg);

		void AddSegments (void);

		short Create (CSideKey key, int addMode = -1);

		void Delete (short nDelSeg = -1, bool bDeleteVerts = true);

		void ResetSide (short nSegment, short nSide);

		bool CreateReactor (short nSegment = -1, bool bCreate = true, bool bSetDefTextures = true); 

		bool CreateRobotMaker (short nSegment = -1, bool bCreate = true, bool bSetDefTextures = true); 

		bool CreateEquipMaker (short nSegment = -1, bool bCreate = true, bool bSetDefTextures = true); 

		bool CreateGoal (short nSegment, bool bCreate, bool bSetDefTextures, ubyte nType, short nTexture);

		bool CreateTeam (short nSegment, bool bCreate, bool bSetDefTextures, ubyte nType, short nTexture);

		bool CreateSpeedBoost (short nSegment, bool bCreate);

		bool CreateSkybox (short nSegment, bool bCreate);

		short CreateProducer (short nSegment = -1, ubyte nType = SEGMENT_FUNC_PRODUCER, bool bCreate = true, bool bSetDefTextures = true); 

		void Init (short nSegment);

		bool SplitIn7 (void);

		bool SplitIn8 (CSegment* pRootSeg);

		bool CreateWedge (void);

		bool CreatePyramid (void);

		void Join (CSideKey key, bool bFind);

		bool Link (short nSegment1, short nSide1, short nSegment2, short nSide2, double margin);

		void LinkSides (short nSegment1, short nSide1, short nSegment2, short nSide2, tVertMatch match [4]);

		CVertex& CalcCenter (CVertex& pos, short nSegment);

		bool IsPointOfSide (CSegment *pSegment, int nSide, int nPoint);

		bool IsLineOfSide (CSegment *pSegment, int nSide, int nLine);

		void JoinSegments ();

		bool SeparateSegments (int solidify = 0, int nSide = -1, bool bVerbose = true);

		void JoinLines (void);

		void JoinPoints (void);

		void SeparateLines (short nLine = -1);

		int SeparatePoints (CSideKey key = CSideKey (-1, -1), int nVertexId = -1, bool bVerbose = true);

		CDoubleVector CalcSideNormal (CSideKey key = CSideKey (-1, -1));

		CDoubleVector CalcSideCenter (CSideKey key = CSideKey (-1, -1));
		//double CalcLength (CFixVector* center1, CFixVector* center2);

		void FixChildren (void);

		void UpdateWalls (short nOldWall, short nNewWall);

		void UpdateVertices (short nOldVert, short nNewVert);

		void Tag (short nSegment);

		void TagAll (ubyte mask = TAGGED_MASK);

		void UnTagAll (ubyte mask = TAGGED_MASK);

		void TagSelected (void);

		void UpdateTagged (void);

		bool IsTagged (short nSegment);

		bool IsTagged (CSideKey key = CSideKey (-1, -1));

		short	TaggedCount (bool bSides = false, bool bCheck = false);

		ushort TaggedSideCount (void);

		bool HasTaggedSegments (bool bSides = false) { return TaggedCount (bSides, true) > 0; }

		bool HasTaggedSides (void);

		bool HasTaggedVertices (short nSide = -1);

		void AlignTextures (short nSegment, short nSide, int bUse1st, int bUse2nd, int bIgnorePlane, bool bAlignAll);

		void AlignChildTextures (CSideKey sideKey, int bUse1st, int bUse2nd, int bIgnorePlane);

		int AlignSideTextures (short nStartSeg, short nStartSide, short nChildSeg, short nChildSide, int bAlign1st, int bAlign2nd);

		CSide* OppSide (void);

		bool SetTextures (CSideKey key, int nBaseTex, int nOvlTex);

		void CopyOtherSegment (void);

		void RenumberRobotMakers (bool bKeepOrder);

		void RenumberEquipMakers (bool bKeepOrder);

		bool SetDefaultTexture (short nTexture);

		void SetLinesToDraw (void);

		short ReadSegmentInfo (CFileManager* fp);

		void WriteSegmentInfo (CFileManager* fp, short);

		void CutBlock ();

		void CopyBlock (char *filename = null);

		void PasteBlock (); 

		int ReadBlock (char *name,int option); 

		void QuickPasteBlock ();

		void DeleteBlock ();

		void SetIndex (void);

		inline void ReadInfo (CFileManager* fp) { m_segmentInfo.Read (fp); }

		inline void WriteInfo (CFileManager* fp) { m_segmentInfo.Write (fp); }

		inline void ReadRobotMakerInfo (CFileManager* fp) { m_producerInfo [0].Read (fp); }

		inline void WriteRobotMakerInfo (CFileManager* fp) { m_producerInfo [0].Write (fp); }

		inline void ReadEquipMakerInfo (CFileManager* fp) { m_producerInfo [1].Read (fp); }

		inline void WriteEquipMakerInfo (CFileManager* fp) { m_producerInfo [1].Write (fp); }

		inline void InitFogInfo (void) { 
			for (int i = 0; i < NUM_FOG_TYPES; i++)
				m_fogInfo [i].Init (i);
			}

		inline void ReadFogInfo (CFileManager* fp) { 
			for (int i = 0; i < NUM_FOG_TYPES; i++)
				m_fogInfo [i].Read (fp);
			}

		inline void WriteFogInfo (CFileManager* fp) { 
			for (int i = 0; i < NUM_FOG_TYPES; i++)
				m_fogInfo [i].Write (fp);
			}

		void ReadSegments (CFileManager* fp);
		
		void WriteSegments (CFileManager* fp);
		
		void ReadRobotMakers (CFileManager* fp);
		
		void WriteRobotMakers (CFileManager* fp);
		
		void ReadEquipMakers (CFileManager* fp);
		
		void WriteEquipMakers (CFileManager* fp);

		// Fixes up numbering for producers (robot and equipment makers).
		// If bKeepOrder is true, preserves how the level behaves in the game
		// if any discrepancies are found.
		// If bKeepOrder is false, the producer list will be reordered to
		// match the segments that reference it.
		void RenumberProducers (bool bKeepOrder = false);

		void Clear (void);

		int Fix (void);

		void Undefine (short nSegment);

		void DeleteD2X (void);

#if USE_FREELIST
		inline bool Full (void) { return m_free.Empty (); }
#else
		bool Full (void);
#endif

		CSegmentManager () : m_bCreating (false) { 
			ResetInfo ();
			Clear ();
#if USE_FREELST
			m_free.Create (Segment (0), SEGMENT_LIMIT);
#endif
			}

		int Overflow (int nSegments = -1);

		short FindByVertex (ushort nVertex, short nSegment);

		void UpdateTexCoords (ushort nVertexId, bool bMove, short nIgnoreSegment = -1, short nIgnoreSide = -1);

		bool VertexInUse (ushort nVertex, short nIgnoreSegment = -1);

		bool CollapseEdge (short nSegment = -1, short nSide = -1, short nEdge = -1, bool bUpdateCoord = true);

		void ComputeNormals (bool bAll, bool bView = false);

		CSegment* GatherSelectableSegments (CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox);

		CSide* GatherSelectableSides (CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox, bool bSegments);

		inline CSegment* SelectedSegments (void) { return m_selectedSegments; }
	
		inline CSide* SelectedSides (void) { return m_selectedSides; }
	
		void GatherEdges (CAVLTree <CEdgeTreeNode, uint>& edgeTree);

		uint VisibleSideCount (void);

		void MakePointsParallel (void);

	private:
		void UnlinkChild (short nParentSeg, short nSide);

		void UnlinkSeg (CSegment *pSegment, CSegment *pRootSeg);

		void LinkSeg (CSegment *pSegment, CSegment *pRootSeg);

		void DeleteWalls (short nSegment);

		short Create (short nSegment, bool bCreate, ubyte nFunction, short nTexture = -1, char* szError = null);

		bool Define (short nSegment, ubyte nFunction, short nTexture);

		short ComputeVertices (CSideKey key, ushort newVerts [4], int addMode = -1);

		short ComputeVerticesOrtho (CSideKey key, ushort newVerts [4]);
		
		short ComputeVerticesExtend (CSideKey key, ushort newVerts [4]);
		
		short ComputeVerticesMirror (CSideKey key, ushort newVerts [4]);

		bool FindNearbySide (CSideKey thisKey, CSideKey& otherKey, short& thisPoint, short& otherPoint, tVertMatch* match);

		void RemoveProducer (CSegment* pSegment, CObjectProducer* producers, CMineItemInfo& info, int nFunction);

		bool CreateProducer (short nSegment, bool bCreate, ubyte nType, bool bSetDefTextures, CObjectProducer* producers, CMineItemInfo& info, char* szError);

		void RenumberProducers (ubyte nFunction, short nClass, bool bKeepOrder);

		void ReadProducers (CFileManager* fp, int nClass);
		
		void WriteProducers (CFileManager* fp, int nClass);

	};

extern CSegmentManager segmentManager;

// -----------------------------------------------------------------------------

class CSegmentIterator : public CGameItemIterator<CSegment> {
	public:
		CSegmentIterator() : CGameItemIterator (const_cast<CSegment*>(segmentManager.Segment (0)), segmentManager.Count ()) {}
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CSideListEntry {
	public:
		CSideKey	m_parent;
		CSideKey	m_child;
		CEdgeKey	m_edge;
};

// -----------------------------------------------------------------------------

class CTaggingStrategy {
	public:
		CAVLTree <CEdgeTreeNode, uint>	m_edgeTree;
		CDynamicArray<CSideListEntry>		m_sideList;

		CSideKey		m_parent, m_child;
		CSegment	*	m_pSegment, * m_pChildSeg;
		CSide*		m_pSide, * m_pChildSide;
		CEdgeKey		m_edgeKey;
		ubyte			m_tag;

	inline bool Setup (int nSides, ubyte tag = TAGGED_MASK) { 
		m_tag = tag;
		m_edgeTree.Destroy ();
		return m_sideList.Create (nSides) != null; 
		}
	inline uint EdgeKey (ushort v1, ushort v2) { return m_edgeKey.Compose (v1, v2); }
	int Run (short nSegment = -1, short nSide = -1);
	inline short ParentSegment (int i) { return m_sideList [i].m_parent.m_nSegment; }
	inline short ParentSide (int i) { return m_sideList [i].m_parent.m_nSide; }
	inline short ChildSegment (int i) { return m_sideList [i].m_child.m_nSegment; }
	inline short ChildSide (int i) { return m_sideList [i].m_child.m_nSide; }
	inline ushort V1 (int i) { return m_sideList [i].m_edge.V1 (); }
	inline ushort V2 (int i) { return m_sideList [i].m_edge.V2 (); }
	virtual bool Accept (void) = 0;
};

// -----------------------------------------------------------------------------

class CTagByAngle : public CTaggingStrategy {
	public:
		double m_maxAngle;
		
		CTagByAngle () : m_maxAngle (cos (Radians (22.5))) {}

		virtual bool Accept (void) { return m_pChildSide->IsVisible () && (Dot (m_pSide->Normal (), m_pChildSide->Normal ()) >= m_maxAngle); }
	};

// -----------------------------------------------------------------------------

class CTagByTextures : public CTaggingStrategy {
	public:
		short m_nBaseTex, m_nOvlTex;
		bool	m_bAll, m_bIgnorePlane;
		
		CTagByTextures (short nBaseTex, short nOvlTex, bool bIgnorePlane = true) : m_nBaseTex (nBaseTex), m_nOvlTex (nOvlTex), m_bIgnorePlane (bIgnorePlane) { m_bAll = !segmentManager.HasTaggedSegments (true); }

		// Return true if the child side can be aligned from the parent side.
		// Filters for marked cubes and matching primary/secondary textures.
		// Currently only filters out back faces of cube connections without bIgnorePlane set
		// (handling that case would require building the edge list differently).
		virtual bool Accept (void) { 
			if (!m_bAll) {
				bool bThisTagged = m_pSegment->IsTagged () || m_pSide->IsTagged ();
				bool bChildTagged = m_pChildSeg->IsTagged () || m_pChildSide->IsTagged ();
				if (!bThisTagged || !bChildTagged)
					return false;
				}
			if (!m_pChildSide->IsVisible ())
				return false;
			if (!m_bIgnorePlane) {
				short nOtherSide;
				if ((m_parent.m_nSegment == m_child.m_nSegment) ||
					(m_pSegment->CommonSides (m_child.m_nSegment, nOtherSide) == -1) ||
					(nOtherSide == m_child.m_nSide)) // don't align the connected side
					return false;
				}
			return ((m_nBaseTex < 0) || (m_pChildSide->BaseTex () == m_nBaseTex)) &&
					 ((m_nOvlTex < 0) || (m_pChildSide->OvlTex (0) == m_nOvlTex));
			}
	};

// -----------------------------------------------------------------------------

class CTagTunnelStart : public CTaggingStrategy {
	public:
		double	m_maxAngle;
		CSide*	m_pStartSide;
		
		CTagTunnelStart () : m_maxAngle (cos (Radians (22.5))), m_pStartSide (current->Side ()) {}

		virtual bool Accept (void) { 
			return (m_pChildSeg->IsTagged () || m_pChildSide->IsTagged ()) && !m_pChildSeg->HasChild (m_child.m_nSide) && (Dot (m_pChildSide->Normal (), m_pStartSide->Normal ()) >= m_maxAngle);
			}
	};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__segman_h
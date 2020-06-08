#ifndef __segman_h
#define __segman_h

// -----------------------------------------------------------------------------

#define MAX_SEGMENTS_D1 800   // descent 1 max # of cubes
#define MAX_SEGMENTS_D2 900   // descent 2 max # of cubes
#define SEGMENT_LIMIT   20000 // D2X-XL max # of cubes

#define MAX_SEGMENTS ((theMine == null) ? MAX_SEGMENTS_D2 : g_data.IsD1File () ? MAX_SEGMENTS_D1  : g_data.IsStdLevel () ? MAX_SEGMENTS_D2 : SEGMENT_LIMIT)

#define MAX_NUM_MATCENS_D1 20
#define MAX_NUM_MATCENS_D2 100

#define MAX_NUM_RECHARGERS_D2  70
#define MAX_NUM_RECHARGERS_D2X 500

#define MAX_NUM_RECHARGERS ((theMine == null) ? MAX_NUM_RECHARGERS_D2X : (g_data.IsD1File () || (g_data.LevelVersion () < 12)) ? MAX_NUM_RECHARGERS_D2 : MAX_NUM_RECHARGERS_D2X)

#define NUM_FOG_TYPES 4

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CEdgeTreeNode {
public:
    uint m_nKey;
    CSLL<CSideKey, CSideKey>m_sides;

    explicit CEdgeTreeNode(uint nKey = 0) : m_nKey(nKey) {}
    ~CEdgeTreeNode() {}
    void Destroy(void) { m_sides.Destroy(); }

    CSideKey* Insert(short nSegment, short nSide) {
        CSideKey key = CSideKey(nSegment, nSide);
        CSideKey* keyP = m_sides.Find(key);
        return keyP ? keyP : m_sides.Append(key);
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
    uint m_key;

    static uint Key(ushort v1, ushort v2) { return(v1 < v2) ? (uint(v1) << 16) + v2 : (uint(v2) << 16) + v1; }

    uint Compose(ushort v1, ushort v2) { return m_key = Key(v1, v2); }
    ushort V1(void) { return ushort(m_key & 0xffff); }
    ushort V2(void) { return ushort(m_key >> 16); }
    inline uint Key(void) { return m_key; }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifdef _DEBUG

typedef CStaticArray< CSegment, SEGMENT_LIMIT > segmentList;
typedef CStaticArray< CObjectProducer, MAX_NUM_MATCENS_D2 > robotMakerList;

#else

typedef CSegment segmentList[SEGMENT_LIMIT];
typedef CStaticArray<CObjectProducer, MAX_NUM_MATCENS_D2> robotMakerList;

#endif

// -----------------------------------------------------------------------------

typedef struct tVertMatch {
    short v;
    double d;
} tVertMatch;

// -----------------------------------------------------------------------------

class CSegmentManager : public ISegmentManager {
public:
    int m_nAddMode;
    bool m_bUpdateAlignmentOnEdit;
    CFogInfo m_fogInfo[NUM_FOG_TYPES];

public:
    inline void ResetInfo() {
        m_segmentInfo.Reset();
        m_producerInfo[0].Reset();
        m_producerInfo[1].Reset();
    }

    inline int& Count() { return m_segmentInfo.count; }

    inline int& FileOffset() { return m_segmentInfo.offset; }

    inline int& ProducerCount(_const_ short nClass) { return m_producerInfo[nClass].count; }

    inline int& RobotMakerCount() { return ProducerCount(0); }

    inline int& EquipMakerCount() { return ProducerCount(1); }

    int ProducerCount();

    inline short Index(CSegment _const_* pSegment) { return (short)(pSegment - &m_segments[0]); }

    inline CSegment _const_* Segment(_const_ int i) { return &m_segments[i]; }

    inline CSegment _const_* Segment(CSideKey& key) { return Segment(key.m_nSegment); }

    inline CSegment _const_& GetSegment(_const_ int i) { return m_segments[i]; }

    inline void PutSegment(_const_ int i, _const_ CSegment& seg) { m_segments[i] = seg; }

    CSide* Side(CSideKey key);

    inline CSide& GetSide(CSideKey key) { return *Side(key); }

    void PutSide(CSideKey key, CSide& side);

    CSide* BackSide(CSideKey key, CSideKey& back);

    CWall* Wall(CSideKey key);

    bool IsExit(short nSegment);

    int IsWall(CSideKey key);

    inline CTrigger _const_* Trigger(CSideKey key) { return Side(key)->Trigger(); }

    inline _const_ CWall* OppositeWall(CSideKey key) {
        _const_ CSide* pSide = Side(key);
        return (pSide == null) ? null : pSide->Wall();
    }

    void Textures(CSideKey key, short& nBaseTex, short& nOvlTex);

    inline robotMakerList& RobotMakers(void) { return m_producers[0]; }

    inline robotMakerList& EquipMakers(void) { return m_producers[1]; }

    inline CObjectProducer* Producer(int i, int nClass) { return &m_producers[nClass][i]; }

    inline CObjectProducer* RobotMaker(int i) { return Producer(i, 0); }

    inline CObjectProducer* EquipMaker(int i) { return Producer(i, 1); }

    inline int SetAddMode(int nMode) { return m_nAddMode = nMode; }

    inline int AddMode(void) { return m_nAddMode; }

    CSegment* FindRobotMaker(short i = 0);

    inline bool& UpdateAlignmentOnEdit(void) { return m_bUpdateAlignmentOnEdit; }

    // Operations
    short Add();

    void Remove(short nDelSeg);

    short AddSegments(ISelection* atSide);

    short Create(CSideKey key, int addMode = -1);

    void Delete(short nDelSeg = -1, bool bDeleteVerts = true);

    void ResetSide(short nSegment, short nSide);

    bool CreateReactor(CSideKey key, short nSegment = -1, bool bCreate = true, bool bSetDefTextures = true);

    bool CreateRobotMaker(CSideKey key, short nSegment = -1, bool bCreate = true, bool bSetDefTextures = true);

    bool CreateEquipMaker(CSideKey key, short nSegment = -1, bool bCreate = true, bool bSetDefTextures = true);

    bool CreateGoal(CSideKey key, short nSegment, bool bCreate, bool bSetDefTextures, ubyte nType, short nTexture);

    bool CreateTeam(CSideKey key, short nSegment, bool bCreate, bool bSetDefTextures, ubyte nType, short nTexture);

    bool CreateSpeedBoost(CSideKey key, short nSegment, bool bCreate);

    bool CreateSkybox(CSideKey key, short nSegment, bool bCreate);

    short CreateProducer(CSideKey key, short nSegment = -1, ubyte nType = SEGMENT_FUNC_PRODUCER, bool bCreate = true, bool bSetDefTextures = true);

    bool SplitIn7(CSegment* pRootSeg);

    bool SplitIn8(CSegment* pRootSeg);

    bool CreateWedge(ISelection* atSide);

    bool CreatePyramid(ISelection* atSide);

    void Join(ISelection* from, ISelection* to);

    bool Link(short nSegment1, short nSide1, short nSegment2, short nSide2, double margin);

    void LinkSides(short nSegment1, short nSide1, short nSegment2, short nSide2, tVertMatch match[4]);

    CVertex& CalcCenter(CVertex& pos, short nSegment);

    bool IsPointOfSide(CSegment* pSegment, int nSide, int nPoint);

    bool IsLineOfSide(CSegment* pSegment, int nSide, int nLine);

    void JoinSegments(ISelection* atSide, ISelection* toSide);

    bool SeparateSegments(ISelection* atSide, int solidify = 0, bool bVerbose = true);

    void JoinLines(ISelection* from, ISelection* to);

    void JoinPoints(ISelection* from, ISelection* to);

    void SeparateLines(ISelection* atLine);

    int SeparatePoints(CSideKey key, int nVertexId, bool bVerbose = true);

    CDoubleVector CalcSideNormal(CSideKey key);

    CDoubleVector CalcSideCenter(CSideKey key);

    void FixChildren(ISelection* atSide);

    void UpdateWalls(short nOldWall, short nNewWall);

    void UpdateVertices(short nOldVert, short nNewVert);

    void Tag(short nSegment);

    void TagAll(ubyte mask = TAGGED_MASK);

    void UnTagAll(ubyte mask = TAGGED_MASK);

    void UpdateTagged();

    bool IsTagged(short nSegment);

    bool IsTagged(CSideKey key);

    short TaggedCount(bool bSides = false, bool bCheck = false);

    ushort TaggedSideCount();

    bool HasTaggedSegments(bool bSides = false) { return TaggedCount(bSides, true) > 0; }

    bool HasTaggedSides();

    void AlignTextures(short nSegment, short nSide, int bUse1st, int bUse2nd, int bIgnorePlane, bool bAlignAll);

    void AlignChildTextures(CSideKey sideKey, int bUse1st, int bUse2nd, int bIgnorePlane);

    int AlignSideTextures(short nStartSeg, short nStartSide, short nChildSeg, short nChildSide, int bAlign1st, int bAlign2nd);

    bool SetTextures(CSideKey key, int nBaseTex, int nOvlTex);

    void CopyOtherSegment(short fromSegmentId, short toSegmentId);

    void RenumberRobotMakers(bool bKeepOrder);

    void RenumberEquipMakers(bool bKeepOrder);

    bool SetDefaultTexture(short nSegment, short nTexture);

    void SetLinesToDraw();

    void SetIndex();

    virtual void ReadInfo(IFileManager* fp) { m_segmentInfo.Read(fp); }

    virtual void WriteInfo(IFileManager* fp) { m_segmentInfo.Write(fp); }

    virtual void ReadRobotMakerInfo(IFileManager* fp) { m_producerInfo[0].Read(fp); }

    virtual void WriteRobotMakerInfo(IFileManager* fp) { m_producerInfo[0].Write(fp); }

    virtual void ReadEquipMakerInfo(IFileManager* fp) { m_producerInfo[1].Read(fp); }

    virtual void WriteEquipMakerInfo(IFileManager* fp) { m_producerInfo[1].Write(fp); }

    inline void InitFogInfo() {
        for (int i = 0; i < NUM_FOG_TYPES; i++)
            m_fogInfo[i].Init(i);
    }

    virtual void ReadFogInfo(IFileManager* fp) {
        for (int i = 0; i < NUM_FOG_TYPES; i++)
            m_fogInfo[i].Read(fp);
    }

    virtual void WriteFogInfo(IFileManager* fp) {
        for (int i = 0; i < NUM_FOG_TYPES; i++)
            m_fogInfo[i].Write(fp);
    }

    void ReadSegments(CFileManager* fp);

    void WriteSegments(CFileManager* fp);

    void ReadRobotMakers(CFileManager* fp);

    void WriteRobotMakers(CFileManager* fp);

    void ReadEquipMakers(CFileManager* fp);

    void WriteEquipMakers(CFileManager* fp);

    // Fixes up numbering for producers (robot and equipment makers).
    // If bKeepOrder is true, preserves how the level behaves in the game
    // if any discrepancies are found.
    // If bKeepOrder is false, the producer list will be reordered to
    // match the segments that reference it.
    void RenumberProducers(bool bKeepOrder = false);

    void Clear();

    int Fix();

    void Undefine(short nSegment);

    void DeleteD2X();

    bool Full();

    CSegmentManager() : m_bCreating(false) {
        ResetInfo();
        Clear();
    }

    int Overflow(int nSegments = -1);

    short FindByVertex(ushort nVertex, short nSegment);

    void UpdateTexCoords(ushort nVertexId, bool bMove, short nIgnoreSegment = -1, short nIgnoreSide = -1);

    bool VertexInUse(ushort nVertex, short nIgnoreSegment);

    bool CollapseEdge(short nSegment, short nSide, short nEdge, bool bUpdateCoord = true);

    void ComputeNormals(bool bAll, bool bView = false);

    CSegment* GatherSelectableSegments(CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox);

    CSide* GatherSelectableSides(CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox, bool bSegments);

    inline CSegment* SelectedSegments() { return m_selectedSegments; }

    inline CSide* SelectedSides() { return m_selectedSides; }

    void GatherEdges(CAVLTree <CEdgeTreeNode, uint>& edgeTree);

    uint VisibleSideCount();

    void MakePointsParallel(ISelection* atSide);

private:
    void UnlinkChild(short nParentSeg, short nSide);

    void DeleteWalls(short nSegment);

    short Create(CSideKey key, short nSegment, bool bCreate, ubyte nFunction, short nTexture = -1, const char* szError = null);

    bool Define(short nSegment, ubyte nFunction, short nTexture);

    short ComputeVertices(CSideKey key, ushort newVerts[4], int addMode = -1);

    short ComputeVerticesOrtho(CSideKey key, ushort newVerts[4]);

    short ComputeVerticesExtend(CSideKey key, ushort newVerts[4]);

    short ComputeVerticesMirror(CSideKey key, ushort newVerts[4]);

    bool FindNearbySide(ISelection* thisSide, CSideKey& otherKey, short& otherPoint, tVertMatch* match);

    void RemoveProducer(CSegment* pSegment, CObjectProducer* producers, CMineItemInfo& info, int nFunction);

    bool CreateProducer(CSideKey key, short nSegment, bool bCreate, ubyte nType, bool bSetDefTextures, CObjectProducer* producers, CMineItemInfo& info, const char* szError);

    void RenumberProducers(ubyte nFunction, short nClass, bool bKeepOrder);

    void ReadProducers(CFileManager* fp, int nClass);

    void WriteProducers(CFileManager* fp, int nClass);

private:
    segmentList m_segments;
    CMineItemInfo m_segmentInfo;
    robotMakerList m_producers[2];
    CMineItemInfo m_producerInfo[2];
    bool m_bCreating;
    CSegment* m_selectedSegments;
    CSide* m_selectedSides;
};

extern CSegmentManager segmentManager;

// -----------------------------------------------------------------------------

class CSegmentIterator : public CGameItemIterator<CSegment> {
public:
    CSegmentIterator() : CGameItemIterator(const_cast<CSegment*>(segmentManager.Segment(0)), segmentManager.Count()) {}
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CSideListEntry {
public:
    CSideKey m_parent;
    CSideKey m_child;
    CEdgeKey m_edge;
};

// -----------------------------------------------------------------------------

class CTaggingStrategy {
public:
    CAVLTree <CEdgeTreeNode, uint> m_edgeTree;
    CDynamicArray<CSideListEntry> m_sideList;

    CSideKey m_parent, m_child;
    CSegment* m_pSegment, * m_pChildSeg;
    CSide* m_pSide, * m_pChildSide;
    CEdgeKey m_edgeKey;
    ubyte m_tag;

    inline bool Setup(int nSides, ubyte tag = TAGGED_MASK) {
        m_tag = tag;
        m_edgeTree.Destroy();
        return m_sideList.Create(nSides) != null;
    }
    inline uint EdgeKey(ushort v1, ushort v2) { return m_edgeKey.Compose(v1, v2); }
    int Run(short nSegment, short nSide);
    inline short ParentSegment(int i) { return m_sideList[i].m_parent.m_nSegment; }
    inline short ParentSide(int i) { return m_sideList[i].m_parent.m_nSide; }
    inline short ChildSegment(int i) { return m_sideList[i].m_child.m_nSegment; }
    inline short ChildSide(int i) { return m_sideList[i].m_child.m_nSide; }
    inline ushort V1(int i) { return m_sideList[i].m_edge.V1(); }
    inline ushort V2(int i) { return m_sideList[i].m_edge.V2(); }
    virtual bool Accept(void) = 0;
};

// -----------------------------------------------------------------------------

class CTagByAngle : public CTaggingStrategy
{
public:
    CTagByAngle();
    virtual bool Accept();

private:
    double m_maxAngle;
};

// -----------------------------------------------------------------------------

class CTagByTextures : public CTaggingStrategy
{
public:
    CTagByTextures(short nBaseTex, short nOvlTex, bool bIgnorePlane = true);
    virtual bool Accept();

private:
    short m_nBaseTex, m_nOvlTex;
    bool m_bAll, m_bIgnorePlane;
};

// -----------------------------------------------------------------------------

class CTagTunnelStart : public CTaggingStrategy
{
public:
    CTagTunnelStart(CSide* side);
    virtual bool Accept();

private:
    double m_maxAngle;
    CSide* m_pStartSide;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__segman_h
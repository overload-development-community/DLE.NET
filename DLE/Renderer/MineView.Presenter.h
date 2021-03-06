#pragma once

enum class RendererType
{
    Software,
    OpenGL
};

enum eViewMode
{
    eViewWireFrameFull = 0,
    eViewHideLines,
    eViewNearbyCubeLines,
    eViewWireFrameSparse,
    eViewTextured,
    eViewTexturedWireFrame
};

enum eObjectViewFlags
{
    eViewObjectsNone = 0,
    eViewObjectsRobots = (1 << 0),
    eViewObjectsPlayers = (1 << 1),
    eViewObjectsWeapons = (1 << 2),
    eViewObjectsPowerups = (1 << 3),
    eViewObjectsKeys = (1 << 4),
    eViewObjectsHostages = (1 << 5),
    eViewObjectsControlCenter = (1 << 6),
    eViewObjectsEffects = (1 << 7),
    eViewObjectsAll = 0xff
};

enum eMineViewFlags
{
    eViewMineWalls = (1 << 0),
    eViewMineSpecial = (1 << 1),
    eViewMineLights = (1 << 2),
    eViewMineIllumination = (1 << 3),
    eViewMineVariableLights = (1 << 4),
    eViewMineUsedTextures = (1 << 5),
    eViewMineSkyBox = (1 << 6)
};

enum class OriginDisplayType
{
    Crosshair = 1,
    Globe = 2
};

enum class SelectMode
{
    Point = POINT_MODE,
    Line = LINE_MODE,
    Side = SIDE_MODE,
    Segment = SEGMENT_MODE,
    Object = OBJECT_MODE,
    Block = BLOCK_MODE
};

class CMineViewPresenter
{
private:
    HWND m_hwnd;
    CRenderData m_renderData;
    CRendererSW m_swRenderer;
    CRendererGL m_glRenderer;
    CRenderer* m_renderer;

    eViewMode m_viewMode;
    eObjectViewFlags m_viewObjectFlags;
    eMineViewFlags m_viewMineFlags;
    OriginDisplayType m_originDisplayType;
    int m_nViewDist;
    const ISelection* m_currentSelection;
    const ISelection* m_otherSelection;
    const ISelection* m_nearestSelection;
    SelectMode m_selectMode;
    CRect m_rubberRect;
    CPoint m_rubberRectClickPos;
    bool m_dragInProgress;
    CPoint m_dragPos;
    CPoint m_lastDragPos;
    CPoint m_highlightPos;
    std::vector<CSide*> m_selectableSides;
    int m_nShowSelectionCandidates;
    bool m_tunnelMakerActive;
    std::vector<CPreviewUVL> m_previewUVLs;

public:
    CMineViewPresenter(HWND hwndTarget);

    // Apparently MFC doesn't create windows before initializing classes. Thanks MFC.
    void UpdateHwnd(HWND hwndTarget) { m_hwnd = hwndTarget; }
    CRenderer& Renderer() { return *m_renderer; }
    bool SetRenderer(RendererType rendererType);
    RendererType GetRenderer();
    bool SetPerspective(int nPerspective);
    int Perspective() { return Renderer().Perspective(); }
    void Draw();

    // Viewport

    void UpdateViewportBounds();
    inline CPoint ViewCenter() { return m_renderData.m_viewport.CenterPoint(); }
    bool IsSegmentVisible(CSegment* pSegment);
    bool IsVertexVisible(int vertexNum);

    // Render data

    inline CDoubleVector& Scale() { return m_renderData.m_vScale; }
    inline int& ViewWidth() { return m_renderData.m_viewWidth; }
    inline int& ViewHeight() { return m_renderData.m_viewHeight; }
    inline int& ViewDepth() { return m_renderData.m_viewDepth; }
    inline bool DepthTest() { return m_renderData.m_bDepthTest; }
    inline void SetDepthTest(bool bDepthTest) { m_renderData.m_bDepthTest = bDepthTest; }
    inline eMineViewFlags& ViewMineFlags() { return m_viewMineFlags; }
    inline eObjectViewFlags& ViewObjectFlags() { return m_viewObjectFlags; }
    inline OriginDisplayType& OriginDisplayType() { return m_originDisplayType; }

    // View mode

    inline eViewMode GetViewMode() { return m_viewMode; }
    inline void SetViewMode(eViewMode mode) { m_viewMode = mode; }
    inline bool IsViewModeSet(eViewMode mode) { return m_viewMode == mode; }

    // View distance

    inline bool SetViewDistIndex(int nViewDist)
    {
        bool changed = m_nViewDist != nViewDist;
        m_nViewDist = nViewDist;
        return changed;
    }
    inline int ViewDistIndex() { return m_nViewDist; }
    inline int ViewDist()
    {
        return (m_nViewDist <= 10) ? m_nViewDist :
            (m_nViewDist < 20) ? 10 + 2 * (m_nViewDist - 10) :
            30 + 3 * (m_nViewDist - 20);
    }
    void FitToView();

    // Selection

    void UpdateCurrentSelection(const ISelection* selection) { m_currentSelection = selection; }
    void UpdateOtherSelection(const ISelection* selection) { m_otherSelection = selection; }
    void UpdateNearestSelection(const ISelection* selection) { m_nearestSelection = selection; }
    void FindNearestVertex(long xMouse, long yMouse, ISelection* selection, CSide* fromSide = nullptr);
    void FindNearestLine(long xMouse, long yMouse, ISelection* selection, CSide* fromSide = nullptr);
    void FindNearestTexturedSide(long xMouse, long yMouse, ISelection* selection);
    void FindNearestSide(long xMouse, long yMouse, ISelection* selection);
    void FindNearestSegment(long xMouse, long yMouse, ISelection* selection);
    void FindNearestObject(long xMouse, long yMouse, ISelection* selection);
    void UpdateSelectMode(SelectMode selectMode) { m_selectMode = selectMode; }
    void UpdateShowSelectionCandidates(int value) { m_nShowSelectionCandidates = value; }

    void BeginRubberRect(const CPoint& clickPos);
    void UpdateRubberRect(const CPoint& mousePos);
    void ClearRubberRect();
    void BeginDrag();
    void UpdateDragPos(const CPoint& mousePos) { m_dragPos = mousePos; }
    void EndDrag();
    void UpdateTunnelMakerActive(bool active) { m_tunnelMakerActive = active; }

private:
    void CalcSegDist();
    void TagVisibleVerts(bool bReset = false);

    // Textured rendering
    void DrawTexturedSegments();

    // Wireframe rendering
    void DrawWireFrame(bool bSparse);
    void DrawSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse = false, bool bTagged = false, char bTunnel = 0);
    void DrawSparseSegmentWireFrame(CSegment* pSegment);
    void RenderSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse, bool bTagged = false);
    void DrawLine(CSegment* pSegment, short vert1, short vert2);

    // Highlight (current selection, walls, lights etc) rendering
    void DrawHighlight();
    void DrawMineCenter();
    void DrawCurrentSegment(CSegment* pSegment, bool bSparse);
    void DrawSegmentHighlighted(short nSegment, short nSide, short nEdge, short nPoint);
    void DrawWalls();
    void DrawLights();
    void DrawOctagon(short nSide, short nSegment);
    void DrawTaggedSegments();

    // Objects
    void DrawObjects();
    void DrawObject(short nObject);

    // Tunnel generator
    void DrawTunnelMaker();
    void DrawTunnel();
    void DrawTunnelMakerPath(const CTunnelPath* path);
    void DrawTunnelMakerPathNode(const CTunnelPathNode& node);
    void DrawTunnelMakerTunnel(const CTunnel* tunnel);
    void DrawTunnelMakerSegment(const CTunnelSegment& segment);

    // Selection
    bool IsSelectMode(SelectMode selectMode) const { return m_selectMode == selectMode; }
    void FindNearestSideInternal(long xMouse, long yMouse, ISelection* selection, bool segmentMode);
    std::vector<CSide*> GatherSelectableSides(CRect& viewport, long xMouse, long yMouse, bool bAllowSkyBox, bool bSegments);

    // Selection mode highlights
    void UpdateSelectableSides(const std::vector<CSide*>& sides);
    bool DrawSelectablePoint();
    bool DrawSelectableEdge();
    bool DrawSelectableSides();
    bool DrawSelectableSegments();

    // Drag/rubber band
    bool HasRubberRect() const;
    bool DrawRubberRect();
    bool DrawDragPos();
    void HighlightDrag(short nVert, long x, long y);

    // Texture projection
    void ApplyPreview();
    void RevertPreview();

    // Pens
    void SelectObjectPen(CGameObject* pObject);
    void SelectWallPen(CWall* pWall);
    bool SelectWireFramePen(CSegment* pSegment);

    // Internal render data
    inline CDoubleVector& Center() { return m_renderData.m_vCenter; }
    inline CDoubleVector& Translation() { return m_renderData.m_vTranslate; }
    inline CDoubleVector& Rotation() { return m_renderData.m_vRotate; }
    inline CBGR* RenderBuffer() { return m_renderData.m_renderBuffer; }
    inline depthType* DepthBuffer() { return m_renderData.m_depthBuffer; }
    inline CBGR& RenderBuffer(int i) { return m_renderData.m_renderBuffer[i]; }
    inline depthType& DepthBuffer(int i) { return m_renderData.m_depthBuffer[i]; }
    inline void SetDepthBuffer(depthType* buffer) { m_renderData.m_depthBuffer = buffer; }
    inline HPEN Pen(ePenColor nPen, int nWeight = 1) { return (nPen < penCount) ? m_renderData.m_pens[nWeight > 1][nPen] : null; }
    inline CVertex& MinViewPoint() { return m_renderData.m_minViewPoint; }
    inline CVertex& MaxViewPoint() { return m_renderData.m_maxViewPoint; }
    inline bool IgnoreDepth() { return m_renderData.m_bIgnoreDepth; }
    inline void SetIgnoreDepth(bool bIgnoreDepth) { m_renderData.m_bIgnoreDepth = bIgnoreDepth; }
    inline int RenderIllumination() { return (m_viewMineFlags & eViewMineIllumination) != 0; }
    inline int RenderVariableLights() { return (m_viewMineFlags & eViewMineVariableLights) != 0; }
    inline ubyte Alpha() { return m_renderData.m_alpha; }
    inline void SetAlpha(ubyte alpha) { m_renderData.m_alpha = alpha; }

    // Internal view settings
    bool ViewObject(CGameObject* pObject);
    inline bool ViewObject(uint flag = 0) { return flag ? ((ViewObjectFlags() & flag) != 0) : (ViewObjectFlags() != 0); }
    inline bool ViewFlag(uint flag = 0) { return flag ? (ViewMineFlags() & flag) != 0 : (ViewMineFlags() != 0); }
};

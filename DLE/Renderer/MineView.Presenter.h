#pragma once

enum class RendererType
{
    Software,
    OpenGL
};

enum eViewOptions
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

    eViewOptions m_viewOption;
    eObjectViewFlags m_viewObjectFlags;
    eMineViewFlags m_viewMineFlags;
    OriginDisplayType m_originDisplayType;
    int m_nViewDist;
    ISelection* m_currentSelection;
    ISelection* m_otherSelection;
    ISelection* m_nearestSelection;
    SelectMode m_selectMode;
    CRect m_rubberRect;
    bool m_dragInProgress;
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

    // Viewport

    void UpdateViewportBounds();
    inline CPoint ViewCenter() { return m_renderData.m_viewport.CenterPoint(); }
    inline CPoint ViewMax()
    {
        // I don't know why DLE multiplies the number by 8 - it seems fishy.
        // But best to leave it until we know it's safe to remove.
        return m_renderData.m_viewport.MulDiv(8, 1).BottomRight();
    }

    // Render data

    inline CDoubleVector& Center() { return m_renderData.m_vCenter; }
    inline CDoubleVector& Translation() { return m_renderData.m_vTranslate; }
    inline CDoubleVector& Scale() { return m_renderData.m_vScale; }
    inline CDoubleVector& Rotation() { return m_renderData.m_vRotate; }
    inline int& ViewWidth() { return m_renderData.m_viewWidth; }
    inline int& ViewHeight() { return m_renderData.m_viewHeight; }
    inline int& ViewDepth() { return m_renderData.m_viewDepth; }
    //inline double DepthScale() { return ViewMatrix()->DepthScale(); }
    //inline void SetDepthScale(int i) { return ViewMatrix()->SetDepthScale(i); }
    //inline int DepthPerception() { return ViewMatrix()->DepthPerception(); }
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
    inline bool DepthTest() { return m_renderData.m_bDepthTest; }
    inline void SetDepthTest(bool bDepthTest) { m_renderData.m_bDepthTest = bDepthTest; }
    inline int RenderIllumination() { return (m_viewMineFlags & eViewMineIllumination) != 0; }
    inline int RenderVariableLights() { return (m_viewMineFlags & eViewMineVariableLights) != 0; }
    inline ubyte Alpha() { return m_renderData.m_alpha; }
    inline void SetAlpha(ubyte alpha) { m_renderData.m_alpha = alpha; }
    inline eMineViewFlags& ViewMineFlags() { return m_viewMineFlags; }
    inline eObjectViewFlags& ViewObjectFlags() { return m_viewObjectFlags; }
    inline OriginDisplayType& OriginDisplayType() { return m_originDisplayType; }

    // View options

    bool ViewObject(CGameObject* pObject);
    inline bool ViewObject(uint flag = 0) { return flag ? ((ViewObjectFlags() & flag) != 0) : (ViewObjectFlags() != 0); }
    inline bool ViewFlag(uint flag = 0) { return flag ? (ViewMineFlags() & flag) != 0 : (ViewMineFlags() != 0); }
    inline bool ViewOption(uint option) { return m_viewOption == option; }
    inline eViewOptions GetViewOptions() { return m_viewOption; }
    inline void SetViewOption(eViewOptions option) { m_viewOption = option; }

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
    inline bool Visible(CSegment* pSegment)
    {
        if ((pSegment->m_info.function == SEGMENT_FUNC_SKYBOX) && !ViewFlag(eViewMineSkyBox))
            return false;
        if (!m_nViewDist)
            return true;
        return (pSegment->Index() >= 0) && (pSegment->Index() <= ViewDist());
    }

    // Selection

    void UpdateCurrentSelection(ISelection* selection) { m_currentSelection = selection; }
    void UpdateOtherSelection(ISelection* selection) { m_otherSelection = selection; }
    void UpdateNearestSelection(ISelection* selection) { m_nearestSelection = selection; }
    bool IsSelectMode(SelectMode selectMode) const { return m_selectMode == selectMode; }
    void UpdateSelectMode(SelectMode selectMode) { m_selectMode = selectMode; }
    void UpdateTunnelMakerActive(bool active) { m_tunnelMakerActive = active; }
    bool HasRubberRect() const;
    void UpdateRubberRect(const CPoint& clickPos, const CPoint& mousePos);
    void ClearRubberRect();
    void BeginDrag();
    const CPoint& DragPos() const { return m_lastDragPos; }
    void EndDrag();
    void UpdateSelectableSides(const std::vector<CSide*>& sides);
    void UpdateShowSelectionCandidates(int value) { m_nShowSelectionCandidates = value; }

    // Draw functions

    void Draw();
    void DrawMineCenter();
    void DrawCurrentSegment(CSegment* pSegment, bool bSparse);
    void DrawWireFrame(bool bSparse);
    void DrawSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse = false, bool bTagged = false, char bTunnel = 0);
    void DrawSparseSegmentWireFrame(CSegment* pSegment);
    void RenderSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse, bool bTagged = false);
    void DrawTexturedSegments();
    void DrawHighlight();
    void DrawLine(CSegment* pSegment, short vert1, short vert2);
    bool DrawRubberRect();
    bool DrawDragPos(const CPoint& mousePos);
    void DrawTunnelMaker();

private:
    void CalcSegDist();
    void HighlightDrag(short nVert, long x, long y);
    void DrawObjects();
    void DrawObject(short nObject);
    void SelectObjectPen(CGameObject* pObject);
    void DrawTaggedSegments();
    bool SelectWireFramePen(CSegment* pSegment);
    void DrawSegmentHighlighted(short nSegment, short nSide, short nEdge, short nPoint);
    void DrawWalls();
    void SelectWallPen(CWall* pWall);
    void DrawLights();
    void DrawOctagon(short nSide, short nSegment);
    bool DrawSelectablePoint();
    bool DrawSelectableEdge();
    bool DrawSelectableSides();
    bool DrawSelectableSegments();
    void DrawTunnel();
    void DrawTunnelMakerPath(const CTunnelPath* path);
    void DrawTunnelMakerPathNode(const CTunnelPathNode& node);
    void DrawTunnelMakerTunnel(const CTunnel* tunnel);
    void DrawTunnelMakerSegment(const CTunnelSegment& segment);
    void ApplyPreview();
    void RevertPreview();
};

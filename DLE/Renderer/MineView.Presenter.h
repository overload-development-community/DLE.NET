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

class CMineViewPresenter
{
private:
    HWND m_hwnd;
    CRendererSW m_swRenderer;
    CRendererGL m_glRenderer;
    CRenderer* m_renderer;
    CRenderData m_renderData;
    eViewOptions m_viewOption;
    int m_nViewDist;

public:
    CMineViewPresenter(HWND hwndTarget);

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
    inline int RenderIllumination() { return (m_renderData.m_viewMineFlags & eViewMineIllumination) != 0; }
    inline int RenderVariableLights() { return (m_renderData.m_viewMineFlags & eViewMineVariableLights) != 0; }
    inline ubyte Alpha() { return m_renderData.m_alpha; }
    inline void SetAlpha(ubyte alpha) { m_renderData.m_alpha = alpha; }
    inline uint& ViewMineFlags() { return m_renderData.m_viewMineFlags; }
    inline uint& ViewObjectFlags() { return m_renderData.m_viewObjectFlags; }

    // View options

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

    // Draw functions

    void DrawSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse = false, bool bTagged = false, char bTunnel = 0);
    void DrawSparseSegmentWireFrame(CSegment* pSegment);
    void RenderSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse, bool bTagged = false);
    void DrawTunnelMaker(CViewMatrix* viewMatrix);

private:
    void DrawTunnelMakerPath(const CTunnelPath* path, CViewMatrix* viewMatrix);
    void DrawTunnelMakerPathNode(const CTunnelPathNode& node, CViewMatrix* viewMatrix);
    void DrawTunnelMakerTunnel(const CTunnel* tunnel, CViewMatrix* viewMatrix);
    void DrawTunnelMakerSegment(const CTunnelSegment& segment);
};

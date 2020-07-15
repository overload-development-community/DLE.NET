#include "stdafx.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "MineView.Presenter.h"

CMineViewPresenter::CMineViewPresenter(HWND hwndTarget) :
    m_hwnd{ hwndTarget },
    m_swRenderer{ m_renderData },
    m_glRenderer{ m_renderData },
    m_renderer{ &m_glRenderer },
    m_renderData{},
    m_viewOption{ eViewTextured },
    m_nViewDist{ 0 }
{
}

bool CMineViewPresenter::SetRenderer(RendererType rendererType)
{
    auto requestedRenderer = (rendererType == RendererType::OpenGL) ?
        dynamic_cast<CRenderer*>(&m_glRenderer) :
        dynamic_cast<CRenderer*>(&m_swRenderer);

    if (m_renderer != requestedRenderer)
    {
        m_renderer = requestedRenderer;
        return true;
    }
    return false;
}

RendererType CMineViewPresenter::GetRenderer()
{
    return (m_renderer == &m_glRenderer) ? RendererType::OpenGL : RendererType::Software;
}

bool CMineViewPresenter::SetPerspective(int nPerspective)
{
    return Renderer().SetPerspective(nPerspective);
}

void CMineViewPresenter::UpdateViewportBounds()
{
    RECT rc{};
    if (::GetClientRect(m_hwnd, &rc))
    {
        m_renderData.m_viewport = rc;
    }
}

//--------------------------------------------------------------------------

void CMineViewPresenter::DrawSparseSegmentWireFrame(CSegment* pSegment)
{
    bool bOrtho = Renderer().Ortho();
    CEdgeList edgeList;
    int nEdges = pSegment->BuildEdgeList(edgeList, true);

    for (short nSide = 0; nSide < 6; nSide++) {
        if (pSegment->ChildId(nSide) >= 0)
            continue;
        CSide* pSide = pSegment->Side(nSide);
        CPoint side[4], line[2];
        ubyte v[2];
        int i, j;

        for (i = 0; i < 4; i++) {
            side[i].x = vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(i)]].m_screen.x;
            side[i].y = vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(i)]].m_screen.y;
        }
        CDoubleVector a, b;
        a.v.x = (double)(side[1].x - side[0].x);
        a.v.y = (double)(side[1].y - side[0].y);
        b.v.x = (double)(side[3].x - side[0].x);
        b.v.y = (double)(side[3].y - side[0].y);
        if (a.v.x * b.v.y < a.v.y * b.v.x)
            Renderer().SelectPen(penWhite + 1);
        else
            Renderer().SelectPen(penGray + 1);
        // draw each line of the current side separately
        // only draw if there is no childP segment of the current segment with a common side
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 2; j++) {
                line[j] = side[(i + j) % 4];
                v[j] = pSide->VertexIdIndex((i + j) % 4);
            }

            // Using the sparse edge list: if a neighboring side is 0xff, it's joined to another cube,
            // so don't render this line
            ubyte side1, side2;
            edgeList.Find(0, side1, side2, v[0], v[1]);
            if (side1 == 0xff || side2 == 0xff)
                continue;

            CVertex& v1 = vertexManager[pSegment->m_info.vertexIds[v[0]]];
            CVertex& v2 = vertexManager[pSegment->m_info.vertexIds[v[1]]];
            if (!bOrtho) {
                Renderer().MoveTo(v1);
                Renderer().LineTo(v2);
            }
            else {
                Renderer().MoveTo(v1.m_screen.x, v1.m_screen.y);
                Renderer().LineTo(v2.m_screen.x, v2.m_screen.y);
            }
        }
    }
}

void CMineViewPresenter::RenderSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse, bool bTagged)
{
    int	bOrtho = Renderer().Ortho();

    if (bOrtho) {
        if (!Visible(pSegment))
            return;
    }
    else if (!bSparse) {
        if (isSelected)
            glDisable(GL_DEPTH_TEST);
        else
            glEnable(GL_DEPTH_TEST);
        glLineWidth(ViewOption(eViewTexturedWireFrame) ? 3.0f : 2.0f);
    }

    CEdgeList	edgeList;
    ushort* vertexIds = pSegment->m_info.vertexIds;
    short			xMax = ViewWidth(),
        yMax = ViewHeight();
    int			nType = Renderer().Type();
    ePenColor	pen;
    float			penWeight;
    bool			bSegmentIsTagged = pSegment->IsTagged();
    bool			bSideTagged[2] = { false, false };
    bool			bFullWireFrame = !bTagged || bSegmentIsTagged || (m_viewOption != eViewTextured);

    Renderer().GetPen(pen, penWeight);
    for (int i = 0, j = pSegment->BuildEdgeList(edgeList); i < j; i++) {
        ubyte i1, i2, side1, side2;
        edgeList.Get(i, side1, side2, i1, i2);
        if (!bSegmentIsTagged) {
            bSideTagged[0] = bSideTagged[1];
            bSideTagged[1] = pSegment->IsTagged(short(side1)) || pSegment->IsTagged(short(side2));
            if (bSideTagged[0] != bSideTagged[1]) {
                if (bSideTagged[1])
                    Renderer().SelectPen(penOrange + 1, ViewOption(eViewTexturedWireFrame) ? 3.0f : 2.0f);
                else
                    Renderer().SelectPen(pen + 1, penWeight);
            }
        }

        if (!(bFullWireFrame || bSideTagged[1]))
            continue;
        CVertex& v1 = vertexManager[vertexIds[i1]];
        CVertex& v2 = vertexManager[vertexIds[i2]];
        if (!bOrtho) {
            Renderer().MoveTo(v1);
            Renderer().LineTo(v2);
        }
        else { //if (v1.InRange (xMax, yMax, nType) && v2.InRange (xMax, yMax, nType)) {
            Renderer().MoveTo(v1.m_screen.x, v1.m_screen.y);
            Renderer().LineTo(v2.m_screen.x, v2.m_screen.y);
        }
    }
    if (bSideTagged[1])
        Renderer().SelectPen(pen + 1, penWeight);
}

void CMineViewPresenter::DrawSegmentWireFrame(CSegment* pSegment, bool isSelected, bool bSparse, bool bTagged, char bTunnel)
{
    if (!Visible(pSegment))
        return;
    if (pSegment->m_info.bTunnel != bTunnel)
        return;

    if (GetRenderer() == RendererType::Software && (bSparse || Renderer().Ortho())) {
        short xMax = ViewWidth();
        short yMax = ViewHeight();
        ushort* vertexIds = pSegment->m_info.vertexIds;
        for (int i = 0; i < 8; i++, vertexIds++) {
            int v = *vertexIds;
            if ((v <= MAX_VERTEX) && !vertexManager[v].InRange(xMax, yMax, Renderer().Type()))
                return;
        }
    }

    if (bSparse)
        DrawSparseSegmentWireFrame(pSegment);
    else
        RenderSegmentWireFrame(pSegment, isSelected, false, bTagged);
}

void CMineViewPresenter::DrawTunnelMaker(CViewMatrix* viewMatrix)
{
    DrawTunnelMakerPath(tunnelMaker.Path(), viewMatrix);
    DrawTunnelMakerTunnel(tunnelMaker.Tunnel(), viewMatrix);
}

void CMineViewPresenter::DrawTunnelMakerPath(const CTunnelPath* path, CViewMatrix* viewMatrix)
{
#ifdef _DEBUG

    for (int i = 0; i <= path->m_nSteps; i++)
        DrawTunnelMakerPathNode(path->m_nodes[i], viewMatrix);

#else

    // Haven't confirmed this works, there's no explicit copy-constructor.
    // If this bugs out, make one.
    CCubicBezier bezier(path->Bezier());

    Renderer().BeginRender();
    for (int i = 0; i < 4; i++) {
        bezier.Transform(viewMatrix);
        bezier.Project(viewMatrix);
    }
    Renderer().EndRender();

    Renderer().BeginRender(true);
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    Renderer().SelectPen(penRed + 1);

    if (bezier.GetPoint(1).InRange(ViewMax().x, ViewMax().y, Renderer().Type())) {
        if (bezier.GetPoint(0).InRange(ViewMax().x, ViewMax().y, Renderer().Type())) {
            Renderer().MoveTo(bezier.GetPoint(0).m_screen.x, bezier.GetPoint(0).m_screen.y);
            Renderer().LineTo(bezier.GetPoint(1).m_screen.x, bezier.GetPoint(1).m_screen.y);
            Renderer().Ellipse(bezier.GetPoint(1), 4, 4);
        }
    }
    if (bezier.GetPoint(2).InRange(ViewMax().x, ViewMax().y, Renderer().Type())) {
        if (bezier.GetPoint(3).InRange(ViewMax().x, ViewMax().y, Renderer().Type())) {
            Renderer().MoveTo(bezier.GetPoint(3).m_screen.x, bezier.GetPoint(3).m_screen.y);
            Renderer().LineTo(bezier.GetPoint(2).m_screen.x, bezier.GetPoint(2).m_screen.y);
            Renderer().Ellipse(bezier.GetPoint(2), 4, 4);
        }
    }
    Renderer().EndRender();

#endif
}

void CMineViewPresenter::DrawTunnelMakerPathNode(const CTunnelPathNode& node, CViewMatrix* viewMatrix)
{
    CDoubleMatrix m;
    m = node.m_rotation.Inverse();
    CVertex v[4] = { m.R(), m.U(), m.F(), node.m_axis };

    Renderer().BeginRender();
    CVertex vertex(node.m_vertex);
    vertex.Transform(viewMatrix);
    vertex.Project(viewMatrix);
    for (int i = 0; i < 4; i++) {
        v[i].Normalize();
        v[i] *= 5.0;
        v[i] += vertex;
        v[i].Transform(viewMatrix);
        v[i].Project(viewMatrix);
    }
    Renderer().EndRender();

    Renderer().BeginRender(true);
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    static ePenColor pens[4] = { penRed, penMedGreen, penMedBlue, penOrange };

    Renderer().Ellipse(vertex, 4, 4);
    for (int i = 0; i < 4; i++) {
        Renderer().SelectPen(pens[i] + 1);
        Renderer().MoveTo(vertex.m_screen.x, vertex.m_screen.y);
        Renderer().LineTo(v[i].m_screen.x, v[i].m_screen.y);
    }
    Renderer().EndRender();
}

void CMineViewPresenter::DrawTunnelMakerTunnel(const CTunnel* tunnel, CViewMatrix* viewMatrix)
{
    Renderer().BeginRender();
    for (int i = 0; i <= tunnel->m_nSteps; i++) {
        for (int j = 0; j < 4; j++) {
            CVertex& v = vertexManager[tunnel->m_segments[i].m_nVertices[j]];
            v.Transform(viewMatrix);
            v.Project(viewMatrix);
        }
    }
    Renderer().EndRender();

    Renderer().BeginRender(true);
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    Renderer().SelectPen(penBlue + 1);
    for (int i = 1; i <= tunnel->m_nSteps; i++)
        DrawTunnelMakerSegment(tunnel->m_segments[i]);
    Renderer().EndRender();
}

void CMineViewPresenter::DrawTunnelMakerSegment(const CTunnelSegment& segment)
{
    Renderer().BeginRender(false);
#ifdef NDEBUG
    if (GetRenderer() == RendererType::OpenGL && (ViewOption(eViewTexturedWireFrame) || ViewOption(eViewTextured)))
#endif
    {
        glLineStipple(1, 0x0c3f);  // dot dash
        glEnable(GL_LINE_STIPPLE);
    }
    for (int i = (int)segment.m_elements.Length(); --i >= 0; )
        DrawSegmentWireFrame(segmentManager.Segment(segment.m_elements[i].m_nSegment), false, false, 1);
    Renderer().EndRender();
#ifdef NDEBUG
    if (GetRenderer() == RendererType::OpenGL && (ViewOption(eViewTexturedWireFrame) || ViewOption(eViewTextured)))
#endif
        glDisable(GL_LINE_STIPPLE);
}

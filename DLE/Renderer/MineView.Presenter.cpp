#include "stdafx.h"
#include "ViewMatrix.h"
#include "Frustum.h"
#include "FBO.h"
#include "renderer.h"
#include "TextureProjector.h"
#include "MineView.Presenter.h"

// max. distance from viewer at which 3D models and icons are rendered instead of angled arrows
constexpr double maxModelDisplayDistance = 300.0;
constexpr LONG rubberBorderWidth = 1;

CMineViewPresenter::CMineViewPresenter(HWND hwndTarget) :
    m_hwnd{ hwndTarget },
    m_renderData{},
    m_swRenderer{ m_renderData },
    m_glRenderer{ m_renderData },
    m_renderer{ &m_glRenderer },
    m_viewOption{ eViewTextured },
    m_originDisplayType{ OriginDisplayType::Globe },
    m_nViewDist{ 0 },
    m_currentSelection{ nullptr },
    m_otherSelection{ nullptr },
    m_selectMode{ SelectMode::Side },
    m_rubberRect{},
    m_dragInProgress{ false },
    m_nShowSelectionCandidates{ 2 },
    m_tunnelMakerActive{ false }
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

void CMineViewPresenter::Draw()
{
    auto viewDC = ::GetDC(m_hwnd);
    Renderer().SetDC(viewDC);
    if (GetRenderer() == RendererType::Software)
    {
        if (DrawRubberRect() || DrawDragPos(DragPos()))
        {
            return;
        }
    }

    // Update renderer settings in case they've changed
    Renderer().SetRenderIllumination(RenderIllumination());
    Renderer().SetRenderVariableLights(RenderVariableLights());

    Renderer().Setup(m_hwnd, viewDC);
    Renderer().ClearView();

    Renderer().BeginRender();
    Renderer().Project();
    Renderer().EndRender();
    ApplyPreview();
    switch (GetViewOptions())
    {
    case eViewTextured:
        DrawTexturedSegments();
        break;

    case eViewTexturedWireFrame:
        DrawWireFrame(false);
        DrawTexturedSegments();
        break;

    case eViewWireFrameFull:
        DrawWireFrame(false);
        break;

    case eViewHideLines:
        DrawWireFrame(false);
        break;

    case eViewNearbyCubeLines:
        DrawWireFrame(false);
        break;

    case eViewWireFrameSparse:
        DrawWireFrame(true);
        break;
    }
    RevertPreview();

    DrawRubberRect();
    if (GetRenderer() == RendererType::OpenGL)
    {
        DrawDragPos(DragPos());
    }
    DrawHighlight();
    DrawMineCenter();

    Renderer().EndRender(true);
    Renderer().SetDC(viewDC);
}

//------------------------------------------------------------------------------

void CMineViewPresenter::DrawMineCenter()
{
    if (Renderer().Perspective())
        return;

    CViewMatrix* viewMatrix = Renderer().ViewMatrix();

    if (OriginDisplayType() == OriginDisplayType::Crosshair)
    {
        Renderer().SelectPen(penWhite + 1);
        int nScale = int(20.0 * Scale().v.y) + 1;
        CVertex v = viewMatrix->Origin();
        viewMatrix->Transform(v.m_view, v);
        v.Project(viewMatrix);
        Renderer().BeginRender(true);
        Renderer().MoveTo(v.m_screen.x, v.m_screen.y - nScale);
        Renderer().LineTo(v.m_screen.x, v.m_screen.y + nScale);
        Renderer().MoveTo(v.m_screen.x - nScale, v.m_screen.y);
        Renderer().LineTo(v.m_screen.x + nScale, v.m_screen.y);
        Renderer().EndRender();
    }
    else if (OriginDisplayType() == OriginDisplayType::Globe)
    {
        // draw a globe
        // 5 circles around each axis at angles of 30, 60, 90, 120, and 150
        // each circle has 18 points
        CVertex v, center, circles[3][120 / 30 + 1][360 / 15 + 1];

        int h, i, j, m, n;

        Renderer().BeginRender();
        for (i = -60, m = 0; i <= 60; i += 30, m++) {
            for (j = 0, n = 0; j <= 360; j += 15, n++) {
                double sini = sin(Radians(i));
                double cosi = cos(Radians(i));
                double sinj = sin(Radians(j));
                double cosj = cos(Radians(j));
                double scale = 5 * cosi;

                v.Set(scale * cosj, scale * sinj, 5 * sini);
                v += viewMatrix->Origin();
                if (GetRenderer() == RendererType::Software)
                    v -= viewMatrix->m_data[0].m_translate;
                viewMatrix->Transform(v.m_view, v);
                v.Project(viewMatrix);
                circles[0][m][n] = v;

                v.Set(scale * cosj, 5 * sini, scale * sinj);
                v += viewMatrix->Origin();
                if (GetRenderer() == RendererType::Software)
                    v -= viewMatrix->m_data[0].m_translate;
                viewMatrix->Transform(v.m_view, v);
                v.Project(viewMatrix);
                circles[1][m][n] = v;

                v.Set(5 * sini, scale * cosj, scale * sinj);
                v += viewMatrix->Origin();
                if (GetRenderer() == RendererType::Software)
                    v -= viewMatrix->m_data[0].m_translate;
                viewMatrix->Transform(v.m_view, v);
                v.Project(viewMatrix);
                circles[2][m][n] = v;
            }
        }

        center = viewMatrix->Origin();
        viewMatrix->Transform(center.m_view, center);
        Renderer().EndRender();

        ePenColor penColors[3] = { penGreen, penGray, penGold };

        Renderer().BeginRender(true);
        for (h = 0; h < 3; h++) {
            Renderer().SelectPen(penColors[h] + 1, 2);
            for (i = -60, m = 0; i <= 60; i += 30, m++) {
                for (j = 0, n = 0; j <= 360; j += 15, n++) {
                    CVertex v = circles[h][m][n];
                    if (j == 0)
                        Renderer().MoveTo(v.m_screen.x, v.m_screen.y);
                    else if ((GetRenderer() == RendererType::OpenGL) ?
                        (v.m_view.v.z < center.m_view.v.z) :
                        (v.m_screen.z <= 0))
                        Renderer().LineTo(v.m_screen.x, v.m_screen.y);
                    else
                        Renderer().MoveTo(v.m_screen.x, v.m_screen.y);
                }
            }
        }
        Renderer().EndRender();
    }
}

void CMineViewPresenter::DrawCurrentSegment(CSegment* pSegment, bool bSparse)
{
    CHECKMINE;

    short xMax = ViewWidth();
    short yMax = ViewHeight();
    short nSide = m_currentSelection->SideId();
    short nLine = m_currentSelection->Point();
    short nPoint = m_currentSelection->Point();
    CSide* pSide = pSegment->Side(nSide);
    short nVertices = pSide->VertexCount();

    if (Renderer().Type()) {
        Renderer().BeginRender();
        glDisable(GL_DEPTH_TEST);
    }
    else {
        for (int i = 0; i < nVertices; i++)
            if (!vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(i)]].InRange(xMax, yMax, Renderer().Type()))
                return;
        Renderer().BeginRender(true);
    }

    Renderer().SelectPen(IsSelectMode(SelectMode::Side) ? penRed + 1 : penGreen + 1);
    for (int i = 0; i < nVertices; i++)
        DrawLine(pSegment, pSide->VertexIdIndex(i), pSide->VertexIdIndex(i + 1));
    // draw current line
    Renderer().SelectPen(IsSelectMode(SelectMode::Side) ? penRed + 1 : penGold + 1);
    DrawLine(pSegment, pSide->VertexIdIndex(nLine), pSide->VertexIdIndex(nLine + 1));
    // draw a circle around the current point
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    Renderer().SelectPen(IsSelectMode(SelectMode::Side) ? penRed + 1 : penGold + 1);
    Renderer().Ellipse(vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(nPoint)]], 4, 4);
    Renderer().EndRender();
}

void CMineViewPresenter::CalcSegDist()
{
    static short segRef[SEGMENT_LIMIT];

    for (CSegmentIterator si; si; si++)
        si->Index() = -1;
    segRef[0] = m_currentSelection->SegmentId();
    m_currentSelection->Segment()->Index() = 0;

    int i = 1, h = 0, j = 0;
    int segCount = segmentManager.Count();

    for (short nDist = 1; (j < segCount) && (h < i); nDist++) {
        for (h = i; j < h; j++) {
            CSegment* segI = segmentManager.Segment(segRef[j]);
            for (short nSide = 0; nSide < 6; nSide++) {
                short nChild = segI->ChildId(nSide);
                if (nChild < 0)
                    continue;
                CSegment* segJ = segmentManager.Segment(nChild);
                if (segJ->Index() != -1)
                    continue;
                segJ->Index() = nDist;
                segRef[i++] = nChild;
            }
        }
    }
}

void CMineViewPresenter::DrawWireFrame(bool bSparse)
{
    CHECKMINE;

    CalcSegDist();
    Renderer().BeginRender(Renderer().Type() == 0);
    Renderer().SelectPen(penGray + 1);
    CSegment* pSegment = segmentManager.Segment(0);
    short segCount = segmentManager.Count();
    if (GetRenderer() == RendererType::OpenGL)
        glDepthFunc(GL_LEQUAL);
    for (short nSegment = 0; nSegment < segCount; nSegment++, pSegment++)
        DrawSegmentWireFrame(pSegment, bSparse);
    Renderer().EndRender();
    if (Visible(m_currentSelection->Segment())) {
        DrawCurrentSegment(m_currentSelection->Segment(), bSparse);
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

//----------------------------------------------------------------------------

static CFaceListEntry faceRenderList[SEGMENT_LIMIT * 6];

void SortFaces(int left, int right)
{
    CFaceListEntry m = faceRenderList[(left + right) / 2];
    int l = left, r = right;

    do {
        while ((faceRenderList[l].m_zMax > m.m_zMax) || ((faceRenderList[l].m_zMax == m.m_zMax) && (faceRenderList[l].m_zMin > m.m_zMin)))
            l++;
        while ((faceRenderList[r].m_zMax < m.m_zMax) || ((faceRenderList[l].m_zMax == m.m_zMax) && (faceRenderList[l].m_zMin < m.m_zMin)))
            r--;
        if (l <= r) {
            if (l < r)
                Swap(faceRenderList[l], faceRenderList[r]);
            l++;
            r--;
        }
    } while (l < r);
    if (l < right)
        SortFaces(l, right);
    if (left < r)
        SortFaces(left, r);
}

//----------------------------------------------------------------------------

void CMineViewPresenter::DrawTexturedSegments()
{
    CHECKMINE;
    if (!textureManager.Available())
        return;

    // Draw Segments ()
    short segCount = segmentManager.Count();
    int faceCount = 0;
    CSegment* pSegment = segmentManager.Segment(0);
    for (short nSegment = 0; nSegment < segCount; nSegment++, pSegment++) {
        if (!Visible(pSegment))
            continue;

        CSide* pSide = pSegment->Side(0);
        for (short nSide = 0; nSide < 6; nSide++, pSide++) {
#ifdef _DEBUG
            if (pSegment->m_info.bTunnel)
                continue;
#endif
            if (pSegment->ChildId(nSide) != -1) { // not a solid side
                CWall* pWall = pSide->Wall();
                if (pWall == null) // no wall either
                    continue;
                if (pWall->Type() == WALL_OPEN) // invisible wall
                    continue;
                if ((pWall->Type() == WALL_CLOAKED) && (pWall->Info().cloakValue == 0)) // invisible cloaked wall
                    continue;
            }

            if (!Renderer().FaceIsVisible(pSegment, pSide))
                continue;
            if (pSide->Shape() > SIDE_SHAPE_TRIANGLE)
                continue;

            long zMin = -LONG_MIN, zMax = LONG_MIN;
            for (ushort nVertex = 0; nVertex < 4; nVertex++) {
                long z = vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(nVertex)]].m_screen.z;
                if (zMin > z)
                    zMin = z;
                if (zMax < z)
                    zMax = z;
            }

            faceRenderList[faceCount].m_nSegment = nSegment;
            faceRenderList[faceCount].m_nSide = nSide;
            faceRenderList[faceCount].m_zMin = zMin;
            faceRenderList[faceCount].m_zMax = zMax;
            CWall* pWall = pSide->Wall();
            faceRenderList[faceCount].m_bTransparent = pSegment->m_info.bTunnel || textureManager.Textures(pSide->BaseTex())->Transparent() || ((pWall != null) && (pWall->Alpha() < 255));
            ++faceCount;
        }
    }

    if (!faceCount)
        return;
    if (faceCount > 1)
        SortFaces(0, faceCount - 1);
    CalcSegDist();

    Renderer().RenderFaces(faceRenderList, faceCount, Renderer().IsRTTSupported());
}

void CMineViewPresenter::DrawHighlight()
{
    if (segmentManager.Count() == 0)
        return;

    DrawObjects();
    DrawTaggedSegments();

    Renderer().BeginRender(Renderer().Type() == 0);
    // draw highlighted segments (other first, then current)
    if (m_currentSelection->SegmentId() != m_otherSelection->SegmentId())
    {
        DrawSegmentHighlighted(m_otherSelection->SegmentId(), m_otherSelection->SideId(),
            m_otherSelection->Edge(), m_otherSelection->Point());
    }
    DrawSegmentHighlighted(m_currentSelection->SegmentId(), m_currentSelection->SideId(),
        m_currentSelection->Edge(), m_currentSelection->Point());

    Renderer().EndRender();

    if (ViewFlag(eViewMineWalls))
        DrawWalls();
    if (ViewFlag(eViewMineLights))
        DrawLights();

    if (m_nearestSelection != nullptr)
    {
        if (DrawSelectablePoint())
            ;
        else if (DrawSelectableEdge())
            ;
        else if (DrawSelectableSides() || DrawSelectableSegments())
        {
            Renderer().BeginRender(false);
            if (GetRenderer() == RendererType::OpenGL)
            {
                glLineStipple(1, 0x00ff);
                glEnable(GL_LINE_STIPPLE);
                glDepthFunc(GL_ALWAYS);
            }
            DrawSegmentHighlighted(m_nearestSelection->SegmentId(), m_nearestSelection->SideId(), DEFAULT_EDGE, DEFAULT_POINT);
            if (GetRenderer() == RendererType::OpenGL)
            {
                glDisable(GL_LINE_STIPPLE);
            }
            Renderer().EndRender();
        }
    }

    if (m_tunnelMakerActive)
    {
        DrawTunnelMaker();
    }
    textureProjector.DrawProjectGuides(Renderer(), Renderer().ViewMatrix());
}

void CMineViewPresenter::DrawObjects()
{
    CHECKMINE;

    if (!ViewObject())
        return;

    int i, j;

    Renderer().BeginRender();
    if (g_data.IsD2File())
    {
        // see if there is a secret exit trigger
        for (i = 0; i < triggerManager.WallTriggerCount(); i++)
            if (triggerManager.Trigger(i)->Type() == TT_SECRET_EXIT) {
                DrawObject((short)objectManager.Count());
                break; // only draw one secret exit
            }
    }

    CGameObject* pObject = objectManager.Object(0);
    for (i = objectManager.Count(), j = 0; i; i--, j++, pObject++)
        if (ViewObject(pObject))
            DrawObject(j);
    Renderer().EndRender();
}

bool CMineViewPresenter::ViewObject(CGameObject* pObject)
{
    switch (pObject->Type()) {
    case OBJ_ROBOT:
        if (ViewObject(eViewObjectsRobots))
            return true;
        if (ViewObject(eViewObjectsKeys)) {
            int nId = pObject->Contains(OBJ_POWERUP);
            if ((nId >= 0) && (powerupTypeTable[nId] == POWERUP_KEY_MASK))
                return true;
            nId = pObject->TypeContains(OBJ_POWERUP);
            if ((nId >= 0) && (powerupTypeTable[nId] == POWERUP_KEY_MASK))
                return true;
        }
        if (ViewObject(eViewObjectsControlCenter) && pObject->IsBoss())
            return true;
        return false;

    case OBJ_CAMBOT:
    case OBJ_SMOKE:
    case OBJ_EXPLOSION:
    case OBJ_MONSTERBALL:
        return ViewObject(eViewObjectsRobots);

    case OBJ_EFFECT:
        return ViewObject(eViewObjectsEffects);

    case OBJ_HOSTAGE:
        return ViewObject(eViewObjectsHostages);

    case OBJ_PLAYER:
    case OBJ_COOP:
        return ViewObject(eViewObjectsPlayers);

    case OBJ_WEAPON:
        return ViewObject(eViewObjectsWeapons);

    case OBJ_POWERUP:
        switch (powerupTypeTable[pObject->Id()]) {
        case POWERUP_WEAPON_MASK:
            return ViewObject(eViewObjectsWeapons);
        case POWERUP_POWERUP_MASK:
            return ViewObject(eViewObjectsPowerups);
        case POWERUP_KEY_MASK:
            return ViewObject(eViewObjectsKeys);
        default:
            return false;
        }

    case OBJ_REACTOR:
        return ViewObject(eViewObjectsControlCenter);
    }
    return false;
}

void CMineViewPresenter::DrawObject(short nObject)
{
    CHECKMINE;

    CGameObject* pObject;
    CGameObject tempObj;

    if (nObject >= 0 && nObject < objectManager.Count()) {
        pObject = objectManager.Object(nObject);
        if (!Visible(segmentManager.Segment(pObject->m_info.nSegment)))
            return;
    }
    else {
        // secret return
        pObject = &tempObj;
        pObject->Type() = -1;
        pObject->m_info.renderType = RT_NONE;
        // theMine->secret_orient = Objects () [0]->orient;
        pObject->m_location.orient.m.rVec = -objectManager.SecretOrient().m.rVec;
        pObject->m_location.orient.m.uVec = objectManager.SecretOrient().m.fVec;
        pObject->m_location.orient.m.fVec = objectManager.SecretOrient().m.uVec;
        // pObject->m_location.orient =  theMine->secret_orient;
        ushort nSegment = (ushort)objectManager.SecretSegment();
        if (nSegment >= segmentManager.Count())
            nSegment = 0;
        if (!Visible(segmentManager.Segment(nSegment)))
            return;
        CVertex center;
        pObject->Position() = segmentManager.CalcCenter(center, nSegment); // define pObject->position
    }

    double d = (ViewOption(eViewTexturedWireFrame) || ViewOption(eViewTextured)) ?
        Renderer().ViewMatrix()->Distance(pObject->Position()) :
        1e30;
    if (textureManager.Available(1) && pObject->HasPolyModel() && modelManager.Setup(pObject, &Renderer()) &&
        ((nObject == m_currentSelection->ObjectId()) || (d <= maxModelDisplayDistance)))
    {
        SelectObjectPen(pObject);
        if (Renderer().IsObjectInView(*pObject, true)) { // only render if fully visible
            Renderer().SelectObject((HBRUSH)GetStockObject(BLACK_BRUSH));
            modelManager.Draw();
        }
    }
    else
    {
        if ((d > maxModelDisplayDistance) || !Renderer().DrawObjectSprite(*pObject) || (nObject == m_currentSelection->ObjectId()))
        {
            SelectObjectPen(pObject);
            Renderer().DrawObjectArrow(*pObject, (nObject == m_currentSelection->ObjectId()));
        }
    }

    if (((nObject == m_currentSelection->ObjectId()) || (nObject == m_otherSelection->ObjectId())) && Renderer().IsObjectInView(*pObject, false))
        Renderer().DrawObjectHighlight(*pObject, (nObject == m_currentSelection->ObjectId()));
}

// Changed: 0=normal,1=gray,2=black
//        if (nObject == (objectManager.Count ()
//        then its a secret return point)
void CMineViewPresenter::SelectObjectPen(CGameObject* pObject)
{
    if (IsSelectMode(SelectMode::Object) && (pObject == m_currentSelection->Object()))
        Renderer().SelectPen(penRed + 1); // RED
    else
    {
        switch (pObject->Type())
        {
        case OBJ_ROBOT:
        case OBJ_CAMBOT:
        case OBJ_EXPLOSION:
        case OBJ_MONSTERBALL:
            Renderer().SelectPen(penMagenta + 1);
            break;
        case OBJ_SMOKE:
        case OBJ_EFFECT:
            Renderer().SelectPen(penGreen + 1, 2);
            break;
        case OBJ_HOSTAGE:
            Renderer().SelectPen(penBlue + 1);
            break;
        case OBJ_PLAYER:
            Renderer().SelectPen(penGold + 1);
            break;
        case OBJ_WEAPON:
            Renderer().SelectPen(penDkGreen + 1);
            break;
        case OBJ_POWERUP:
            Renderer().SelectPen(penOrange + 1);
            break;
        case OBJ_REACTOR:
            Renderer().SelectPen(penLtGray + 1);
            break;
        case OBJ_COOP:
            Renderer().SelectPen(penGold + 1);
            break;
        default:
            Renderer().SelectPen(penGreen + 1);
        }
    }
}

void CMineViewPresenter::DrawTaggedSegments()
{
    CSegment* pSegment;
    short xMax = ViewWidth();
    short yMax = ViewHeight();
    short i;
    Renderer().BeginRender(Renderer().Type() == 0);
    if (!Renderer().Ortho())
        glDepthFunc(GL_LEQUAL);
    for (i = 0; i < segmentManager.Count(); i++)
        if (SelectWireFramePen(pSegment = segmentManager.Segment(i)))
            DrawSegmentWireFrame(pSegment, false, true);
    Renderer().EndRender();
    Renderer().BeginRender(true);
    // draw a square around all marked points
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
#if 1
    Renderer().SelectPen(penOrange + 1);
#else
    if (m_selectMode == BLOCK_MODE)
        Renderer().SelectPen(penRed + 1);
    else
        Renderer().SelectPen(penGold + 1);
#endif
    for (i = 0; i < vertexManager.Count(); i++)
        if ((vertexManager.Status(i) & TAGGED_MASK) && vertexManager[i].InRange(xMax, yMax, Renderer().Type()))
            Renderer().Rectangle(vertexManager[i], 5, 5);
    Renderer().EndRender();
}

bool CMineViewPresenter::SelectWireFramePen(CSegment* pSegment)
{
    if (pSegment->IsTagged())
    {
        Renderer().SelectPen(penOrange + 1);
        return true;
    }

    if (ViewFlag(eViewMineSpecial) && (GetViewOptions() != eViewTextured))
    {
        switch (pSegment->m_info.function)
        {
        case SEGMENT_FUNC_PRODUCER:
        case SEGMENT_FUNC_SPEEDBOOST:
            Renderer().SelectPen(penYellow + 1);
            return true;
        case SEGMENT_FUNC_REACTOR:
            Renderer().SelectPen(penOrange + 1);
            return true;
        case SEGMENT_FUNC_REPAIRCEN:
            Renderer().SelectPen(penLtBlue + 1);
            return true;
        case SEGMENT_FUNC_ROBOTMAKER:
        case SEGMENT_FUNC_EQUIPMAKER:
            Renderer().SelectPen(penMagenta + 1);
            return true;
        case SEGMENT_FUNC_GOAL_BLUE:
        case SEGMENT_FUNC_TEAM_BLUE:
            Renderer().SelectPen(penBlue + 1);
            return true;
        case SEGMENT_FUNC_GOAL_RED:
        case SEGMENT_FUNC_TEAM_RED:
            Renderer().SelectPen(penRed + 1);
            return true;
        default:
            if (pSegment->m_info.props & SEGMENT_PROP_WATER)
                Renderer().SelectPen(penMedBlue + 1);
            else if (pSegment->m_info.props & SEGMENT_PROP_LAVA)
                Renderer().SelectPen(penMedRed + 1);
            else if (pSegment->m_info.props & SEGMENT_PROP_FOG)
                Renderer().SelectPen(penDkGray + 1);
            else if (pSegment->IsTagged(short(-1)))
                Renderer().SelectPen(penGray + 1);
            else
                return false;
            return true;
        }
    }

    if (pSegment->IsTagged(short(-1)))
    {
        Renderer().SelectPen(penGray + 1);
        return true;
    }

    return false;
}

void CMineViewPresenter::DrawSegmentHighlighted(short nSegment, short nSide, short nLine, short nPoint)
{
    CSegment* pSegment = segmentManager.Segment(nSegment);
    CSide* pSide = pSegment->Side(nSide);
    short xMax = ViewWidth() * 2;
    short yMax = ViewHeight() * 2;

    if (!Visible(pSegment))
        return;

    bool isCurrentSegment = nSegment == m_currentSelection->SegmentId();
    bool isNearestSegment = m_nearestSelection ? nSegment == m_nearestSelection->SegmentId() : false;
    bool isSelected = (pSegment == m_currentSelection->Segment()) || (pSegment == m_otherSelection->Segment());

    // draw the segment's wire frame
    auto segmentPen = pSegment->IsTagged() ? penGold + 1 :
        (isCurrentSegment || isNearestSegment) ? 
        (IsSelectMode(SelectMode::Segment) ? penRed + 1 : penWhite + 1) :
        penGray + 1;
    Renderer().SelectPen(segmentPen, 2.0f);
    DrawSegmentWireFrame(pSegment, isSelected);

    // draw the current side's outline
    int i;
    if (!Renderer().Ortho())
        i = 4;
    else {
        for (i = 0; i < 4; i++)
            if (!vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(i)]].InRange(xMax, yMax, Renderer().Type()))
                break;
    }
    if (i == 4) {
        auto sidePen = (isCurrentSegment || isNearestSegment) ?
            (IsSelectMode(SelectMode::Side) ? penRed + 1 : penGreen + 1) :
            penDkGreen + 1;
        Renderer().SelectPen(sidePen, 2.0f);
        short nVertices = pSide->VertexCount();
        for (i = 0; i < nVertices; i++)
            DrawLine(pSegment, pSide->VertexIdIndex(i), pSide->VertexIdIndex(i + 1));
    }

    // draw the current line
    short i1 = pSide->VertexIdIndex(nLine);
    short i2 = pSide->VertexIdIndex(nLine + 1);
    if (!Renderer().Ortho() ||
        (vertexManager[pSegment->m_info.vertexIds[i1]].InRange(xMax, yMax, Renderer().Type()) &&
            vertexManager[pSegment->m_info.vertexIds[i2]].InRange(xMax, yMax, Renderer().Type())))
    {
        auto linePen = (isCurrentSegment || isNearestSegment) ?
            (IsSelectMode(SelectMode::Line) ? penRed + 1 : penGold + 1) :
            penMedCyan + 1;
        Renderer().SelectPen(linePen, 2.0f);
        DrawLine(pSegment, i1, i2);
    }

    // draw a circle around the current vertex
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    auto vertexPen = (isCurrentSegment || isNearestSegment) ?
        (IsSelectMode(SelectMode::Point) ? penRed + 1 : penGold + 1) :
        penMedCyan + 1;
    Renderer().SelectPen(vertexPen, 2.0f);
    i = pSegment->m_info.vertexIds[pSide->VertexIdIndex(nPoint)];
    if (vertexManager[i].InRange(xMax, yMax, Renderer().Type()))
        Renderer().Ellipse(vertexManager[i], 4, 4);
}

void CMineViewPresenter::DrawWalls()
{
    CWall* walls = wallManager.Wall(0);
    CSegment* segments = segmentManager.Segment(0);
    CSegment* pSegment;
    CSide* pSide;
    short i, j;
    short xMax = ViewWidth() * 2;
    short yMax = ViewHeight() * 2;

    for (i = 0; i < wallManager.WallCount(); i++)
    {
        if (walls[i].m_nSegment > segmentManager.Count())
            continue;
        pSegment = segments + (int)walls[i].m_nSegment;
        if (!Visible(pSegment))
            continue;
        short nSide = walls[i].m_nSide;
        pSide = pSegment->Side(nSide);
        for (j = 0; j < 4; j++)
        {
            CVertex& v = vertexManager[pSegment->m_info.vertexIds[pSide->VertexIdIndex(j)]];
            if (IN_RANGE(v.m_screen.x, xMax) && IN_RANGE(v.m_screen.y, yMax))
                break;
        }
        if (j < 4)
        {
            CVertex center, normal, vector;
            CVertex arrowStartPoint, arrowEndPoint, arrow1Point, arrow2Point;

            center = segmentManager.CalcSideCenter(walls[i]);
            normal = segmentManager.CalcSideNormal(walls[i]);
            vector = center - normal;
            Renderer().BeginRender();
            vector.Transform(Renderer().ViewMatrix());
            vector.Project(Renderer().ViewMatrix());

            if (walls[i].Info().nTrigger != NO_TRIGGER) {
                // calculate arrow points
                arrowStartPoint = center - (normal * 3);
                arrowStartPoint.Transform(Renderer().ViewMatrix());
                arrowStartPoint.Project(Renderer().ViewMatrix());
                arrowEndPoint = center + (normal * 3);
                arrowEndPoint.Transform(Renderer().ViewMatrix());
                arrowEndPoint.Project(Renderer().ViewMatrix());
                // direction toward center of line 0 from center
                CVertex vector = Average(*pSegment->Vertex(nSide, 0), *pSegment->Vertex(nSide, 1));
                vector -= center;
                vector.Normalize();

                arrow1Point = (normal * 2);
                arrow1Point += center;
                arrow1Point += vector;
                arrow1Point.Transform(Renderer().ViewMatrix());
                arrow1Point.Project(Renderer().ViewMatrix());
                vector *= 2;
                arrow2Point = arrow1Point - vector;
                arrow2Point.Transform(Renderer().ViewMatrix());
                arrow2Point.Project(Renderer().ViewMatrix());
            }
            Renderer().EndRender();

            if (Renderer().Type()) {
                Renderer().BeginRender(false);
                glEnable(GL_DEPTH_TEST);
                SelectWallPen(&walls[i]);
                glLineWidth(ViewOption(eViewTexturedWireFrame) ? 4.0f : 3.0f);
                for (j = 0; j < pSide->VertexCount(); j++) {
                    Renderer().MoveTo(vector);
                    Renderer().LineTo(*pSegment->Vertex(nSide, j));
                }
                if (walls[i].Info().nTrigger != NO_TRIGGER) {
                    // draw arrow
                    Renderer().MoveTo(arrowStartPoint);
                    Renderer().LineTo(arrowEndPoint);
                    Renderer().LineTo(arrow1Point);
                    Renderer().MoveTo(arrowEndPoint);
                    Renderer().LineTo(arrow2Point);
                }
            }
            else {
                Renderer().BeginRender(true);
                SelectWallPen(&walls[i]);
                for (j = 0; j < pSide->VertexCount(); j++) {
                    Renderer().MoveTo(vector.m_screen.x, vector.m_screen.y);
                    CVertex& v = *pSegment->Vertex(nSide, j);
                    Renderer().LineTo(v.m_screen.x, v.m_screen.y);
                }
                if (walls[i].Info().nTrigger != NO_TRIGGER) {
                    // draw arrow
                    Renderer().MoveTo(arrowStartPoint.m_screen.x, arrowStartPoint.m_screen.y);
                    Renderer().LineTo(arrowEndPoint.m_screen.x, arrowEndPoint.m_screen.y);
                    Renderer().LineTo(arrow1Point.m_screen.x, arrow1Point.m_screen.y);
                    Renderer().MoveTo(arrowEndPoint.m_screen.x, arrowEndPoint.m_screen.y);
                    Renderer().LineTo(arrow2Point.m_screen.x, arrow2Point.m_screen.y);
                }
            }
            Renderer().EndRender();
        }
    }
}

void CMineViewPresenter::SelectWallPen(CWall* pWall)
{
    switch (pWall->Type())
    {
    case WALL_NORMAL:
        Renderer().SelectPen(penLtGray + 1);
        break;
    case WALL_BLASTABLE:
        Renderer().SelectPen(penLtGray + 1);
        break;
    case WALL_DOOR:
        switch (pWall->Info().keys)
        {
        case KEY_NONE:
            Renderer().SelectPen(penLtGray + 1);
            break;
        case KEY_BLUE:
            Renderer().SelectPen(penBlue + 1);
            break;
        case KEY_RED:
            Renderer().SelectPen(penRed + 1);
            break;
        case KEY_GOLD:
            Renderer().SelectPen(penYellow + 1);
            break;
        default:
            Renderer().SelectPen(penGray + 1);
        }
        break;
    case WALL_ILLUSION:
        Renderer().SelectPen(penLtGray + 1);
        break;
    case WALL_OPEN:
        Renderer().SelectPen(penLtGray + 1);
        break;
    case WALL_CLOSED:
        Renderer().SelectPen(penLtGray + 1);
        break;
    default:
        Renderer().SelectPen(penLtGray + 1);
    }
}

void CMineViewPresenter::DrawLights()
{
    // now show variable lights
    CViewMatrix* viewMatrix = Renderer().ViewMatrix();
    CVariableLight* pLight = lightManager.VariableLight(0);

    // find variable light from
    Renderer().BeginRender();
    pLight = lightManager.VariableLight(0);
    for (INT i = 0; i < lightManager.Count(); i++, pLight++)
        if (Visible(segmentManager.Segment(pLight->m_nSegment)))
            DrawOctagon(pLight->m_nSide, pLight->m_nSegment);
    Renderer().EndRender();
}

void CMineViewPresenter::DrawOctagon(short nSide, short nSegment)
{
    short xMax = ViewWidth();
    short yMax = ViewHeight();

    if (nSegment >= 0 && nSegment <= segmentManager.Count() && nSide >= 0 && nSide <= 5)
    {
        CSegment* pSegment = segmentManager.Segment(nSegment);
        CSide* pSide = pSegment->Side(nSide);
        short nVertices = pSide->VertexCount();
        if (nVertices < 3)
            return;

        CVertex vCenter = pSegment->ComputeCenter(nSide);
        if (!vCenter.InRange(xMax, yMax, static_cast<int>(GetRenderer())))
            return;

        if (Renderer().ViewMatrix()->Distance(vCenter) <= maxModelDisplayDistance)
        {
            pSide->ComputeNormals(pSegment->m_info.vertexIds, pSegment->ComputeCenter());
            vCenter += pSide->Normal();
            Renderer().Sprite(&textureManager.Icon(LIGHT_ICON), vCenter, 5.0, 5.0, false);
        }
        else
        {
            pSide->ComputeNormals(pSegment->m_info.vertexIds, pSegment->ComputeCenter(true), true);
            CVertex vertices[4];
            short nVertices = pSide->VertexCount();
            CDoubleVector vOffset = pSide->Normal();
            for (short i = 0; i < nVertices; i++)
                vertices[i].m_view = pSegment->Vertex(nSide, i)->m_view + vOffset;
            glDisable(GL_CULL_FACE);
            Renderer().TexturedPolygon(&textureManager.Icon(CIRCLE_ICON), null, null, vertices, nVertices, null);
            glEnable(GL_CULL_FACE);
        }
    }
}

//--------------------------------------------------------------------------
// Determine the side edge the projection of which to the mine view is closest
// to the current mouse position.

bool CMineViewPresenter::DrawSelectablePoint()
{
    if (!IsSelectMode(SelectMode::Point))
        return false;

    Renderer().BeginRender(true);
    Renderer().SelectPen(penMedBlue + 1, 1);
    Renderer().Ellipse(vertexManager[m_nearestSelection->Point()], 8.0, 8.0);
    Renderer().EndRender();

    return true;
}

//--------------------------------------------------------------------------
// Determine the side edge the projection of which to the mine view is closest
// to the current mouse position.

bool CMineViewPresenter::DrawSelectableEdge()
{
    if (!IsSelectMode(SelectMode::Line))
        return false;

    Renderer().BeginRender(true);
    if (GetRenderer() == RendererType::OpenGL)
    {
        glLineStipple(1, 0x00ff);
        glEnable(GL_LINE_STIPPLE);
    }
    Renderer().SelectPen(penMedBlue + 1, 2);
    CVertex* v1 = m_nearestSelection->Segment()->Vertex(m_nearestSelection->SideId(), m_nearestSelection->Edge());
    CVertex* v2 = m_nearestSelection->Segment()->Vertex(m_nearestSelection->SideId(), m_nearestSelection->Edge() + 1);
    Renderer().MoveTo(v1->m_screen.x, v1->m_screen.y);
    Renderer().LineTo(v2->m_screen.x, v2->m_screen.y);
    if (GetRenderer() == RendererType::OpenGL)
        glDisable(GL_LINE_STIPPLE);
    Renderer().SelectPen(penMedBlue + 1);
    Renderer().Ellipse(*v1, 8.0, 8.0);
    Renderer().Ellipse(*v2, 8.0, 8.0);
    Renderer().EndRender();

    return true;
}

//--------------------------------------------------------------------------
// Determine all sides in the projection of which  to the mine viewthe current 
// mouse position lies. Draw a circle in each side's center and a line from 
// each side vertex to the circle to mark it. Open sides have the circle 
// moved in by their normal * 2.0.

bool CMineViewPresenter::DrawSelectableSides()
{
    if (!IsSelectMode(SelectMode::Side))
        return false;

    if (m_nShowSelectionCandidates > 1)
    {
        Renderer().BeginRender();
        if (GetRenderer() == RendererType::OpenGL)
        {
            glLineStipple(1, 0x00ff);
            glEnable(GL_LINE_STIPPLE);
            glDepthFunc(GL_ALWAYS);
        }
        for (CSide* pSide : m_selectableSides)
        {
            CSegment* pSegment = segmentManager.Segment(pSide->GetParent());
            short nSide = pSegment->SideIndex(pSide);
            short nVertices = pSide->VertexCount();
            Renderer().SelectPen((pSide == m_nearestSelection->Side()) ? penGold + 1 : penMedBlue + 1);
            CVertex& center = pSide->Center();

            if (GetRenderer() == RendererType::OpenGL)
                glLineStipple(1, 0x00ff);
            if (Renderer().Ortho())
            {
                for (int i = 0; i <= nVertices; i++) {
                    CVertex* vertex = pSegment->Vertex(nSide, i);
                    if (i)
                        Renderer().LineTo(vertex->m_screen.x, vertex->m_screen.y);
                    else
                        Renderer().MoveTo(vertex->m_screen.x, vertex->m_screen.y);
                }
            }
            else
            {
                for (int i = 0; i <= nVertices; i++) {
                    CVertex* vertex = pSegment->Vertex(nSide, i);
                    if (i)
                        Renderer().LineTo(*vertex);
                    else
                        Renderer().MoveTo(*vertex);
                }
            }

            if (!pSide->IsVisible())
            {
                CVertex normal = pSide->Normal(2);
                normal.Project(Renderer().ViewMatrix());
                center += normal;
            }
            if (GetRenderer() == RendererType::OpenGL)
                glLineStipple(1, 0x3333);
            for (int i = 0; i < nVertices; i++)
            {
                if (Renderer().Ortho())
                {
                    CVertex* vertex = pSegment->Vertex(nSide, i);
                    CDoubleVector c(double(center.m_screen.x), double(center.m_screen.y), double(center.m_screen.z));
                    CDoubleVector v(double(vertex->m_screen.x), double(vertex->m_screen.y), double(vertex->m_screen.z));
                    c -= v;
                    double m = c.Mag();
                    c *= (m - 8.0) / m;
                    c += v;
                    Renderer().MoveTo(vertex->m_screen.x, vertex->m_screen.y);
                    Renderer().LineTo(long(Round(c.v.x)), long(Round(c.v.y)));
                }
                else
                {
                    Renderer().MoveTo(*pSegment->Vertex(nSide, i));
                    Renderer().LineTo(center);
                }
            }
        }
        Renderer().EndRender();
    }

    if (m_nShowSelectionCandidates > 0)
    {
        Renderer().BeginRender();
        if (GetRenderer() == RendererType::OpenGL)
            glDepthFunc(GL_ALWAYS);
        for (CSide* pSide : m_selectableSides)
        {
            CSegment* pSegment = segmentManager.Segment(pSide->GetParent());
            short nSide = pSegment->SideIndex(pSide);
            CVertex& center = pSide->Center();
            Renderer().SelectPen((pSide == m_nearestSelection->Side()) ? penGold + 1 : penMedBlue + 1, 1.5f);
            Renderer().Ellipse(center, 5.0, 5.0);
        }
        Renderer().EndRender();
    }
    return true;
}

//--------------------------------------------------------------------------
// Determine all segments in the projection of which to the mine view the current 
// mouse position lies. Draw a circle in each segment's center and a line from 
// each side vertex to the circle to mark it. Open sides have the circle 
// moved in by their normal * 2.0.

bool CMineViewPresenter::DrawSelectableSegments()
{
    if (!IsSelectMode(SelectMode::Segment))
        return false;

    if (m_nShowSelectionCandidates > 1)
    {
        Renderer().BeginRender();
        if (GetRenderer() == RendererType::OpenGL)
        {
            glLineStipple(1, 0x00ff);
            glDepthFunc(GL_ALWAYS);
            glEnable(GL_LINE_STIPPLE);
        }
        short nSegment = -1;
        for (CSide* pSide : m_selectableSides)
        {
            if (nSegment == pSide->GetParent())
                continue;
            CSegment* pSegment = segmentManager.Segment(nSegment = pSide->GetParent());
            short nSide = pSegment->SideIndex(pSide);
            CVertex& center = pSegment->Center();
            bool isSelected = (pSegment == m_currentSelection->Segment()) || (pSegment == m_otherSelection->Segment());

            Renderer().SelectPen((pSide == m_nearestSelection->Side()) ? penGold + 1 : penMedBlue + 1);
            if (GetRenderer() == RendererType::OpenGL)
                glLineStipple(1, 0x00ff);
            DrawSegmentWireFrame(pSegment, isSelected);
            Renderer().SelectPen((pSide == m_nearestSelection->Side()) ? penGold + 1 : penMedBlue + 1);
            if (GetRenderer() == RendererType::OpenGL)
                glLineStipple(1, 0x3333);
            for (int i = 0; i < 8; i++)
            {
                if (pSegment->VertexId(i) > MAX_VERTEX)
                    continue;
                if (Renderer().Ortho())
                {
                    CVertex* vertex = pSegment->Vertex(i);
                    CDoubleVector c(double(center.m_screen.x), double(center.m_screen.y), double(center.m_screen.z));
                    CDoubleVector v(double(vertex->m_screen.x), double(vertex->m_screen.y), double(vertex->m_screen.z));
                    c -= v;
                    double m = c.Mag();
                    c *= (m - 8.0) / m;
                    c += v;
                    Renderer().MoveTo(vertex->m_screen.x, vertex->m_screen.y);
                    Renderer().LineTo(long(Round(c.v.x)), long(Round(c.v.y)));
                }
                else
                {
                    Renderer().MoveTo(*pSegment->Vertex(nSide, i));
                    Renderer().LineTo(center);
                }
            }
        }
        Renderer().EndRender();
    }

    if (m_nShowSelectionCandidates > 0)
    {
        Renderer().BeginRender();
        if (GetRenderer() == RendererType::OpenGL)
            glDepthFunc(GL_ALWAYS);
        short nSegment = -1;
        for (CSide* pSide : m_selectableSides)
        {
            if (nSegment == pSide->GetParent())
                continue;
            CSegment* pSegment = segmentManager.Segment(nSegment = pSide->GetParent());
            short nSide = pSegment->SideIndex(pSide);
            CVertex& center = pSegment->Center();
            Renderer().SelectPen((pSide == m_nearestSelection->Side()) ? penGold + 1 : penMedBlue + 1, 1.5f);
            Renderer().Ellipse(center, 5.0, 5.0);
        }
        Renderer().EndRender();
    }

    return true;
}

void CMineViewPresenter::DrawLine(CSegment* pSegment, short i1, short i2)
{
    CHECKMINE;
    if (Renderer().Ortho()) {
        Renderer().MoveTo(vertexManager[pSegment->m_info.vertexIds[i1]].m_screen.x, vertexManager[pSegment->m_info.vertexIds[i1]].m_screen.y);
        Renderer().LineTo(vertexManager[pSegment->m_info.vertexIds[i2]].m_screen.x, vertexManager[pSegment->m_info.vertexIds[i2]].m_screen.y);
    }
    else {
        Renderer().MoveTo(vertexManager[pSegment->m_info.vertexIds[i1]]);
        Renderer().LineTo(vertexManager[pSegment->m_info.vertexIds[i2]]);
    }
}

bool CMineViewPresenter::DrawRubberRect()
{
    if (HasRubberRect())
    {
        if (GetRenderer() == RendererType::OpenGL)
        {
            glPushAttrib(GL_ENABLE_BIT);
            glLineStipple(1, 0x3333);
            glEnable(GL_LINE_STIPPLE);
            Renderer().SelectPen(penWhite + 1);
        }
        else
        {
            ::SetROP2(Renderer().DC(), R2_XORPEN);
            Renderer().SelectPen(penBlack + 1);
        }

        CPoint rubberPoly[5]{};
        rubberPoly[0].x = m_rubberRect.left + rubberBorderWidth;
        rubberPoly[0].y = m_rubberRect.top + rubberBorderWidth;
        rubberPoly[1].x = m_rubberRect.right - rubberBorderWidth;
        rubberPoly[1].y = m_rubberRect.top + rubberBorderWidth;
        rubberPoly[2].x = m_rubberRect.right - rubberBorderWidth;
        rubberPoly[2].y = m_rubberRect.bottom - rubberBorderWidth;
        rubberPoly[3].x = m_rubberRect.left + rubberBorderWidth;
        rubberPoly[3].y = m_rubberRect.bottom - rubberBorderWidth;
        rubberPoly[4] = rubberPoly[0];

        Renderer().BeginRender(true);
        Renderer().PolyLine(rubberPoly, ARRAYSIZE(rubberPoly));
        Renderer().EndRender();
        if (GetRenderer() == RendererType::OpenGL)
        {
            glPopAttrib();
        }
        else
        {
            ::SetROP2(Renderer().DC(), R2_COPYPEN);
        }

        return true;
    }

    return false;
}

bool CMineViewPresenter::DrawDragPos(const CPoint& mousePos)
{
    if (!m_dragInProgress)
    {
        return false;
    }

    short nVert = m_currentSelection->Side()->VertexIdIndex(m_currentSelection->Point());

    Renderer().BeginRender(true);
    // unhighlight last point and lines drawing
    if (GetRenderer() == RendererType::Software)
    {
        HighlightDrag(nVert, m_lastDragPos.x, m_lastDragPos.y);
    }
    // highlight the new position
    HighlightDrag(nVert, mousePos.x, mousePos.y);
    m_lastDragPos = mousePos;

    if (GetRenderer() == RendererType::Software)
    {
        ::SetROP2(Renderer().DC(), R2_NOT);
    }

    int i;
    for (i = 0; i < vertexManager.Count(); i++)
    {
        CVertex& v = vertexManager[i];
        if ((abs(v.m_screen.x - mousePos.x) < 5) && (abs(v.m_screen.y - mousePos.y) < 5))
        {
            if ((v.m_screen.x != m_highlightPos.x) || (v.m_screen.y != m_highlightPos.y))
            {
                if (m_highlightPos.x != -1)
                    // erase last point
                    Renderer().Ellipse(m_highlightPos, 8, 8);
                // define and draw new point
                m_highlightPos.x = v.m_screen.x;
                m_highlightPos.y = v.m_screen.y;
                Renderer().Ellipse(m_highlightPos, 8, 8);
                break;
            }
        }
    }
    // if no point found near cursor
    if ((i >= vertexManager.Count()) && (m_highlightPos.x != -1))
    {
        // erase last point
        Renderer().Ellipse(m_highlightPos, 8, 8);
    }

    Renderer().EndRender();
    if (GetRenderer() == RendererType::Software)
    {
        ::SetROP2(Renderer().DC(), R2_COPYPEN);
    }
    // define and draw new point
    m_highlightPos.x = -1;
    m_highlightPos.y = -1;

    return true;
}

void CMineViewPresenter::HighlightDrag(short nVert, long x, long y)
{
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    if (GetRenderer() == RendererType::Software)
    {
        ::SetROP2(Renderer().DC(), R2_NOT);
    }
    else
    {
        Renderer().SelectPen(penWhite + 1);
    }

    Renderer().Ellipse(x, y, 4, 4);

    CRect rc{ x, y, x, y };
    for (int i = 0; i < 3; i++) {
        Renderer().MoveTo(x, y);
        short nVert2 = adjacentPointTable[nVert][i];
        CVertex& v = vertexManager[m_currentSelection->Segment()->m_info.vertexIds[nVert2]];
        Renderer().LineTo(v.m_screen.x, v.m_screen.y);
        if (rc.left > v.m_screen.x)
            rc.left = v.m_screen.x;
        if (rc.right < v.m_screen.x)
            rc.right = v.m_screen.x;
        if (rc.top > v.m_screen.y)
            rc.top = v.m_screen.y;
        if (rc.bottom < v.m_screen.y)
            rc.bottom = v.m_screen.y;
    }

    if (GetRenderer() == RendererType::Software)
    {
        ::SetROP2(Renderer().DC(), R2_COPYPEN);
        rc.InflateRect(4, 4);
        ::InvalidateRect(m_hwnd, rc, FALSE);
        ::UpdateWindow(m_hwnd);
    }
}

void CMineViewPresenter::DrawTunnel()
{
    CVertex points[4];
    auto viewMax = ViewMax();

    Renderer().SelectPen(penRed + 1);
    Renderer().SelectObject((HBRUSH)GetStockObject(NULL_BRUSH));
    tunnelMaker.Create();
    Renderer().BeginRender();
    points[1].Transform(Renderer().ViewMatrix());
    points[1].Project(Renderer().ViewMatrix());
    if (IN_RANGE(points[1].m_screen.x, viewMax.x) && IN_RANGE(points[1].m_screen.y, viewMax.y)) {
        points[0].Transform(Renderer().ViewMatrix());
        points[0].Project(Renderer().ViewMatrix());
        if (IN_RANGE(points[0].m_screen.x, viewMax.x) && IN_RANGE(points[0].m_screen.y, viewMax.y)) {
            Renderer().BeginRender(true);
            Renderer().MoveTo(points[0].m_screen.x, points[0].m_screen.y);
            Renderer().LineTo(points[1].m_screen.x, points[1].m_screen.y);
            Renderer().Ellipse(points[1], 4, 4);
            Renderer().EndRender();
            Renderer().BeginRender();
        }
    }
    points[2].Transform(Renderer().ViewMatrix());
    points[2].Project(Renderer().ViewMatrix());
    if (IN_RANGE(points[2].m_screen.x, viewMax.x) && IN_RANGE(points[2].m_screen.y, viewMax.y)) {
        points[2].Transform(Renderer().ViewMatrix());
        points[2].Project(Renderer().ViewMatrix());
        if (IN_RANGE(points[3].m_screen.x, viewMax.x) && IN_RANGE(points[3].m_screen.y, viewMax.y)) {
            Renderer().BeginRender(true);
            Renderer().MoveTo(points[3].m_screen.x, points[3].m_screen.y);
            Renderer().LineTo(points[2].m_screen.x, points[2].m_screen.y);
            Renderer().Ellipse(points[2].m_screen.x - 4, points[2].m_screen.y - 4, points[2].m_screen.x + 4, points[2].m_screen.y + 4);
            Renderer().EndRender();
            Renderer().BeginRender();
        }
    }
    if (tunnelMaker.Update(m_currentSelection, m_otherSelection) && tunnelMaker.Create())
        DrawTunnelMaker();
}

void CMineViewPresenter::DrawTunnelMaker()
{
    DrawTunnelMakerPath(tunnelMaker.Path());
    DrawTunnelMakerTunnel(tunnelMaker.Tunnel());
}

void CMineViewPresenter::DrawTunnelMakerPath(const CTunnelPath* path)
{
#ifdef _DEBUG

    for (int i = 0; i <= path->m_nSteps; i++)
        DrawTunnelMakerPathNode(path->m_nodes[i]);

#else

    // Haven't confirmed this works, there's no explicit copy-constructor.
    // If this bugs out, make one.
    CCubicBezier bezier(path->Bezier());

    Renderer().BeginRender();
    for (int i = 0; i < 4; i++) {
        bezier.Transform(Renderer().ViewMatrix());
        bezier.Project(Renderer().ViewMatrix());
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

void CMineViewPresenter::DrawTunnelMakerPathNode(const CTunnelPathNode& node)
{
    CDoubleMatrix m;
    m = node.m_rotation.Inverse();
    CVertex v[4] = { m.R(), m.U(), m.F(), node.m_axis };

    Renderer().BeginRender();
    auto viewMatrix = Renderer().ViewMatrix();
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

void CMineViewPresenter::DrawTunnelMakerTunnel(const CTunnel* tunnel)
{
    Renderer().BeginRender();
    for (int i = 0; i <= tunnel->m_nSteps; i++) {
        for (int j = 0; j < 4; j++) {
            auto viewMatrix = Renderer().ViewMatrix();
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

//------------------------------------------------------------------------------

void CMineViewPresenter::ApplyPreview()
{
    // Clear anything in the existing list
    m_previewUVLs.clear();

    // Build preview changes.
    // This is backward from how it would ideally work, but we need to respond to geometry changes
    textureProjector.Project(&m_previewUVLs);

    // Apply preview changes
    for(auto& previewUVL : m_previewUVLs)
    {
        previewUVL.Apply();
    }
}

//------------------------------------------------------------------------------

void CMineViewPresenter::RevertPreview()
{
    // Revert already-existing preview changes
    for (auto& previewUVL : m_previewUVLs)
    {
        previewUVL.Revert();
    }
}

bool CMineViewPresenter::HasRubberRect() const
{
    CRect emptyRect{ 0, 0, 0, 0 };
    return ::EqualRect(m_rubberRect, emptyRect);
}

void CMineViewPresenter::UpdateRubberRect(const CPoint& clickPos, const CPoint& mousePos)
{
    CRect rc = m_rubberRect;
    if (clickPos.x < mousePos.x)
    {
        rc.left = clickPos.x - rubberBorderWidth;
        rc.right = mousePos.x + rubberBorderWidth;
    }
    else if (clickPos.x > mousePos.x)
    {
        rc.right = clickPos.x + rubberBorderWidth;
        rc.left = mousePos.x - rubberBorderWidth;
    }
    if (clickPos.y < mousePos.y)
    {
        rc.top = clickPos.y - rubberBorderWidth;
        rc.bottom = mousePos.y + rubberBorderWidth;
    }
    else if (clickPos.y > mousePos.y)
    {
        rc.bottom = clickPos.y + rubberBorderWidth;
        rc.top = mousePos.y - rubberBorderWidth;
    }
    if (rc != m_rubberRect)
    {
        ::InvalidateRect(m_hwnd, &rc, TRUE);
        ::UpdateWindow(m_hwnd);
        m_rubberRect = rc;
    }
}

void CMineViewPresenter::ClearRubberRect()
{
    ::InvalidateRect(m_hwnd, &m_rubberRect, FALSE);
    ::UpdateWindow(m_hwnd);
    m_rubberRect = CRect{ 0, 0, 0, 0 };
}

void CMineViewPresenter::BeginDrag()
{
    short nVert = m_currentSelection->Side()->VertexIdIndex(m_currentSelection->Point());
    short i = m_currentSelection->Segment()->m_info.vertexIds[nVert];
    CVertex& v = vertexManager[i];

    m_highlightPos.x = v.m_screen.x;
    m_highlightPos.y = v.m_screen.y;
    m_lastDragPos = m_highlightPos;
    m_dragInProgress = true;
}

void CMineViewPresenter::EndDrag()
{
    m_dragInProgress = false;
}

void CMineViewPresenter::UpdateSelectableSides(const std::vector<CSide*>& sides)
{
    m_selectableSides.clear();
    m_selectableSides = sides;
}

// dlcView.cpp: implementation of the CMineView class
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#endif

static LPSTR szMouseStates [] = {
    "IDLE",
    "BUTTON DOWN",
    "SELECT",
    "PAN",
    "ROTATE",
    "ZOOM",
    "ZOOM IN",
    "ZOOM OUT",
    "INIT DRAG",
    "DRAG",
    "RUBBERBAND"
    };


#define BETWEEN(a,b,c) ((a<c) ? (a<b)&&(b<c): (c<b)&&(b<a))

/////////////////////////////////////////////////////////////////////////////
// CMineView

IMPLEMENT_DYNCREATE(CMineView, CView)

BEGIN_MESSAGE_MAP(CMineView, CView)
    //{{AFX_MSG_MAP(CMineView)
    ON_WM_TIMER ()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_DESTROY()
    ON_WM_MOUSEWHEEL()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_KEYUP()
    ON_WM_KEYDOWN()
    ON_WM_SYSKEYUP()
    ON_WM_SYSKEYDOWN()
    ON_WM_SETFOCUS()
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
    ON_COMMAND(ID_SEL_PREV_TAB, OnSelectPrevTab)
    ON_COMMAND(ID_SEL_NEXT_TAB, OnSelectNextTab)
END_MESSAGE_MAP()

BOOL CMineView::PreCreateWindow(CREATESTRUCT& cs)
{
return CView::PreCreateWindow(cs);
}

//------------------------------------------------------------------------------
// CMineView construction/destruction

CMineView::CMineView() :
    m_presenter(m_hWnd),
    m_inputHandler (this)
{
ViewObjectFlags () = eViewObjectsAll;
ViewMineFlags () = (eMineViewFlags) eViewMineLights | eViewMineWalls | eViewMineSpecial;
m_rubberRectInProgress = false;
m_nDelayRefresh = 0;
m_bEnableQuickSelection = false;
m_nShowSelectionCandidates = 2;
m_xScrollRange = m_yScrollRange = 0;
SetElementMovementReference (1);
SetMineMoveRate (1.0);
SetViewMoveRate (1.0);
m_bEnableQuickSelection = GetPrivateProfileInt ("DLE", "EnableQuickSelection", 1, DLE.IniFile ());
m_nShowSelectionCandidates = GetPrivateProfileInt ("DLE", "ShowSelectionCandidates", 2, DLE.IniFile ());
m_bMovementTimerActive = false;
QueryPerformanceFrequency (&m_qpcFrequency);
Reset ();
}

//------------------------------------------------------------------------------

void CMineView::Reset (void)
{
ViewWidth () = ViewHeight () = ViewDepth () = 0;	// force OnDraw to initialize these

tunnelMaker.Destroy ();
m_needsRepaint = true;
m_selectMode = SelectMode::Side;
m_lastSegment = 0;

Renderer ().Reset ();
// calculate transformation m_view based on move, size, and spin
m_lightTimer =
m_selectTimer = -1;
m_nFrameRate = 100;
}

//------------------------------------------------------------------------------

CMineView::~CMineView()
{
}

//------------------------------------------------------------------------------

void CMineView::OnDestroy ()
{
if (m_lightTimer != -1) {
    KillTimer (m_lightTimer);
    m_lightTimer = -1;
    }
if (m_selectTimer != -1) {
    KillTimer (m_selectTimer);
    m_selectTimer = -1;
    }
CView::OnDestroy ();
}

//------------------------------------------------------------------------------

void CMineView::SetRenderer(int nRenderer)
{
    if (m_presenter.SetRenderer(nRenderer ? RendererType::OpenGL : RendererType::Software))
    {
        Reset();
        FitToView();
        Refresh();
    }
}

//------------------------------------------------------------------------------

void CMineView::SetPerspective(int nPerspective)
{
    if (m_presenter.SetPerspective(nPerspective))
    {
        FitToView();
        Refresh();
        appSettings.m_nPerspective = nPerspective;
    }
}

//------------------------------------------------------------------------------

bool CMineView::UpdateScrollBars (void)
{
#if 1
return false;
#else
Project ();

    int		dx = m_maxViewPoint.m_screen.x - m_minViewPoint.m_screen.x + 1;
    int		dy = m_minViewPoint.m_screen.y - m_maxViewPoint.m_screen.y + 1;
    double	r;

bool bHScroll = m_bHScroll;
bool bVScroll = m_bVScroll;
int xScrollCenter = m_xScrollCenter;
int yScrollCenter = m_yScrollCenter;
int xScrollRange = m_xScrollRange;
int yScrollRange = m_yScrollRange;
m_bHScroll = (dx > ViewWidth ());
m_bVScroll = (dy > ViewHeight ());
ShowScrollBar (SB_HORZ, m_bHScroll);
ShowScrollBar (SB_VERT, m_bVScroll);
InitViewDimensions ();
Project ();
dx = m_maxViewPoint.m_screen.x - m_minViewPoint.m_screen.x + 1;
dy = m_minViewPoint.m_screen.y - m_maxViewPoint.m_screen.y + 1;
if (m_bHScroll) {
    if (xScrollRange > m_xScrollRange)
        SetScrollRange (SB_HORZ, 0, m_xScrollRange = dx - ViewWidth (), TRUE);
    if (!bHScroll)
        SetScrollPos (SB_HORZ, m_xScrollCenter = m_xScrollRange / 2, TRUE);
    else if (xScrollRange != m_xScrollRange) {
        r = (double) xScrollRange / (double) m_xScrollRange;
        int nPos = GetScrollPos (SB_HORZ);
        nPos -= m_xScrollCenter;
        m_xScrollCenter = m_xScrollRange / 2;
        SetScrollPos (SB_HORZ, (UINT) (m_xScrollCenter + (double) nPos * r), TRUE);
        }
    if (xScrollRange < m_xScrollRange)
        SetScrollRange (SB_HORZ, 0, m_xScrollRange = dx - ViewWidth (), TRUE);
    }
else {
    m_xScrollRange = 0;
    m_xRenderOffs = 0;
    }
if (m_bVScroll) {
    if (yScrollRange > m_yScrollRange)
        SetScrollRange (SB_VERT, 0, m_yScrollRange = dy - ViewHeight (), TRUE);
    if (!bVScroll)
        SetScrollPos (SB_VERT, m_yScrollCenter = m_yScrollRange / 2, TRUE);
    else if (yScrollRange != m_yScrollRange) {
        r = (double) yScrollRange / (double) m_yScrollRange;
        int nPos = GetScrollPos (SB_VERT);
        nPos -= m_yScrollCenter;
        m_yScrollCenter = m_yScrollRange / 2;
        SetScrollPos (SB_VERT, (UINT) (m_yScrollCenter + (double) nPos * r), TRUE);
        }
    if (yScrollRange < m_yScrollRange)
        SetScrollRange (SB_VERT, 0, m_yScrollRange = dy - ViewHeight (), TRUE);
    }
else {
    m_yScrollRange = 0;
    m_yRenderOffs = 0;
    }
return (bHScroll != m_bHScroll) || (bVScroll != m_bVScroll);
#endif
}

//------------------------------------------------------------------------------

void CMineView::OnDraw (CDC* pViewDC)
{
    CHECKMINE;

    CDlcDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    if (m_needsRepaint)
    {
        m_presenter.UpdateHwnd(m_hWnd);
        m_presenter.UpdateCurrentSelection(current);
        m_presenter.UpdateOtherSelection(other);
        m_presenter.UpdateSelectMode(m_selectMode);
        bool tunnelMakerActive = tunnelMaker.Update(current, other) && tunnelMaker.Create();
        m_presenter.UpdateTunnelMakerActive(tunnelMakerActive);
        HiliteTarget();
        UpdateNearestSelection();
        m_presenter.UpdateShowSelectionCandidates(m_nShowSelectionCandidates);
        m_presenter.Draw();
        UpdateStatusText();
    }
    m_needsRepaint = false;

    // If the movement timer is active we measure the elapsed time since the last frame
    // and immediately update the camera position
    if (m_bMovementTimerActive) {
        LARGE_INTEGER newFrameTime = { 0 };
        QueryPerformanceCounter(&newFrameTime);
        double timeElapsed = ((double)(newFrameTime.QuadPart - m_lastFrameTime.QuadPart)) / m_qpcFrequency.QuadPart;
        m_inputHandler.UpdateMovement(timeElapsed);
        m_lastFrameTime = newFrameTime;
    }
}

//------------------------------------------------------------------------------

void CMineView::AdvanceLightTick (void)
{
CHECKMINE;

    int i, j = lightManager.Count ();

#pragma omp parallel for if (j > 15)
for (i = 0; i < j; i++) {
    CVariableLight *pLight = lightManager.VariableLight (i);
    int delay = X2I (pLight->m_info.delay * 1000);
    if (delay) {
        tLightTimer *pTimer = lightManager.LightTimer (i);
        pTimer->ticks += (1000 + m_nFrameRate / 2) / m_nFrameRate;
        if (pTimer->ticks >= delay) {
            short h = pTimer->ticks / delay;
            pTimer->ticks %= delay;
            pTimer->impulse = (pTimer->impulse + h) % 32;
            }
        }
    }
}

//------------------------------------------------------------------------------

bool CMineView::SetLightStatus (void)
{
#if 0
    int h, i, j;
    CLightDeltaIndex *pDeltaIndex = lightManager.LightDeltaIndex (0);
    CLightDeltaValue *pDeltaValue;
    tLightTimer *ltP;
    CVariableLight *pLight = lightManager.VariableLight (0);
    tLightStatus *pls;
    bool bChange = false;
    bool bD2XLights = (DLE.LevelVersion () >= 15) && (theMine->Info ().fileInfo.version >= 34);
    short nSrcSide, nSrcSeg, nSegment, nSide;

// search delta light index to see if current side has a light
pls = lightManager.LightStatus (0);
for (i = segmentManager.Count (); i; i--)
    for (j = 0; j < MAX_SIDES_PER_SEGMENT; j++, pls++)
        pls->bWasOn = pls->bIsOn;
for (h = 0; h < lightManager.DeltaIndexCount (); h++, pDeltaIndex++) {
    nSrcSide = pDeltaIndex->m_nSegment;
    nSrcSeg = pDeltaIndex->m_nSide;
    j = lightManager.VariableLight (CSideKey (nSrcSide, nSrcSeg));
    if (j < 0)
        continue;	//shouldn't happen here, as there is a delta light value, but you never know ...
    if (j >= MAX_VARIABLE_LIGHTS)
        continue;	//shouldn't happen 
    pDeltaValue = lightManager.LightDeltaValue (pDeltaIndex->m_info.index);
    for (i = pDeltaIndex->m_info.count; i; i--, pDeltaValue++) {
        nSegment = pDeltaValue->m_nSegment;
        nSide = pDeltaValue->m_nSide;
        if (m_bShowLightSource) {
            if ((nSegment != nSrcSeg) || (nSide != nSrcSide)) 
                continue;
            if (0 > lightManager.VariableLight (CSideKey (nSegment, nSide)))
                continue;
            }
        else {
            if (((nSegment != nSrcSeg) || (nSide != nSrcSide)) && (0 <= lightManager.VariableLight (CSideKey (nSegment, nSide))))
                continue;
            }
        pls = lightManager.LightStatus (nSegment, nSide);
        ltP = lightManager.LightTimer (j);
        pls->bIsOn = (pLight [j].m_info.mask & (1 << lightManager.LightTimer (j)->impulse)) != 0;
        if (pls->bWasOn != pls->bIsOn)
            bChange = true;
        }
    }
return bChange;
#else
return lightManager.DeltaIndexCount () > 0;
#endif
}

//------------------------------------------------------------------------------

void CMineView::OnTimer (UINT_PTR nIdEvent)
{
CHECKMINE;
#if 0
if (nIdEvent == 4) {
    if (m_mouseState == eMouseStateButtonDown) {
        m_mouseState = eMouseStateSelect;
#ifdef _DEBUG
        INFOMSG (szMouseStates [m_mouseState]);
#endif
        }
    if (m_mouseState == eMouseStateSelect) {
        if (!SelectCurrentSegment (1, m_clickPos.x, m_clickPos.y)) {
            SetMouseState (eMouseStateRubberBand);
            //UpdateRubberRect (m_clickPos);
            }
        }
    }
else 
#endif
if (nIdEvent == 3) {
    if (DLE.MineView ()->RenderVariableLights ()) {
        AdvanceLightTick ();
        if (SetLightStatus ()) {
            m_needsRepaint = true;
            InvalidateRect (null, TRUE);
            UpdateWindow ();
            }
        }
    }
else 
    CView::OnTimer (nIdEvent);
}

//----------------------------------------------------------------------------

void CMineView::EnableDeltaShading (int bEnable, int nFrameRate, int bShowLightSource)
{
if (bEnable) {
    DLE.MineView ()->ViewMineFlags () |= eViewMineVariableLights;
    if ((nFrameRate >= 10) && (nFrameRate <= 100))
        m_nFrameRate = nFrameRate;
    //m_lightTimer = SetTimer (3, (UINT) (1000 / m_nFrameRate + m_nFrameRate / 2), null);
    m_lightTimer = SetTimer (3, (UINT) 33, null);
    lightManager.AnimInfo ().Clear ();
    }
else {
    DLE.MineView ()->ViewMineFlags () &= ~eViewMineVariableLights;
    if (m_lightTimer != -1) {
        KillTimer (m_lightTimer);
        m_lightTimer = -1;
        }
    }
}

//----------------------------------------------------------------------------

BOOL CMineView::SetWindowPos (const CWnd *pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags)
{
    CRect	rc;

    GetClientRect (rc);

if ((rc.Width () != cx) || (rc.Height () != cy))
    DLE.MainFrame ()->ResetPaneMode ();
return CView::SetWindowPos (pWndInsertAfter, x, y, cx, cy, nFlags);
}

//----------------------------------------------------------------------------

void CMineView::ResetView (bool bRefresh)
{
Renderer ().Release ();
if (bRefresh)
    Refresh (true);
}

//------------------------------------------------------------------------

void CMineView::HiliteTarget (void)
{
    int i, nTarget;

CGameObject *pObject = current->Object ();
if ((pObject->Type () != OBJ_EFFECT) || (pObject->Id () != LIGHTNING_ID))
    return;
other->SetObjectId (current->ObjectId ());
if (nTarget = pObject->rType.lightningInfo.nTarget) {
    CGameObject* pObject = objectManager.Object (0);
    for (i = objectManager.Count (); i; i--, pObject++) {
        if ((pObject->Type () == OBJ_EFFECT) && (pObject->Id () == LIGHTNING_ID) && (pObject->rType.lightningInfo.nId == nTarget)) {
            other->SetObjectId (i);
            break;
            return;
            }
        }
    }
}
 
//------------------------------------------------------------------------------
// CMineView printing

BOOL CMineView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CMineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CMineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

//------------------------------------------------------------------------------
// CMineView diagnostics

#ifdef _DEBUG
void CMineView::AssertValid() const
{
    CView::AssertValid();
}

void CMineView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CDlcDoc* CMineView::GetDocument() // non-debug version is inline
{
    if (m_pDocument)
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlcDoc)));
    return (CDlcDoc*)m_pDocument;
}
#endif //_DEBUG

//------------------------------------------------------------------------------
// CMineView message handlers

BOOL CMineView::OnEraseBkgnd(CDC* pDC)
{
//	return CView::OnEraseBkgnd(pDC);
    return TRUE;
}

//------------------------------------------------------------------------------

void CMineView::OnSize (UINT nType, int cx, int cy)
{
    CRect rc;

GetClientRect(rc);
m_presenter.UpdateViewportBounds();
CView::OnSize (nType, cx, cy);
m_needsRepaint = true;
}

//------------------------------------------------------------------------------

void CMineView::SetViewMode (eViewMode mode)
{
m_presenter.SetViewMode(mode);
Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::ToggleViewMine (eMineViewFlags flag)
{
ViewMineFlags () ^= flag;
EnableDeltaShading ((ViewMineFlags () & eViewMineVariableLights) != 0, -1, -1);
Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::ToggleViewObjects(eObjectViewFlags mask)
{
    ViewObjectFlags () ^= mask;
    Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::SetViewMineFlags(uint mask)
{
    ViewMineFlags () = mask;
    Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::SetViewObjectFlags (uint mask)
{
    ViewObjectFlags () = mask;
    Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::SetSelectMode (uint mode)
{
theMine->SetSelectMode ((short) mode);
m_selectMode = static_cast<::SelectMode>(mode);
DLE.MainFrame()->UpdateSelectButtons(m_selectMode);
Refresh (false);
}

//------------------------------------------------------------------------------

BOOL CMineView::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
{
//if (m_bUpdateCursor) {
//::SetCursor (AfxGetApp()->LoadStandardCursor (nIdCursors [m_mouseState]));
//	return TRUE;
//	}
return CView::OnSetCursor (pWnd, nHitTest, message);
}

//------------------------------------------------------------------------------

BOOL CMineView::UpdateCursor ()
{
    HCURSOR hCursor = m_inputHandler.GetCurrentCursor ();

if (hCursor == NULL)
    return FALSE;
::SetClassLongPtr (GetSafeHwnd (), GCLP_HCURSOR, LONG (hCursor));
::SetCursor (hCursor); // to update the cursor this frame
return TRUE;
}

//------------------------------------------------------------------------------

CPoint CMineView::AdjustMousePos (CPoint point)
{
return CPoint (point.x, (m_presenter.GetRenderer() == RendererType::OpenGL) ? ViewHeight () - point.y : point.y);
}

//------------------------------------------------------------------------------

CPoint CMineView::CenterMouse ()
{
    CRect rcWindow;

GetWindowRect (&rcWindow);
CPoint ptCenter = rcWindow.CenterPoint ();
::SetCursorPos (ptCenter.x, ptCenter.y);
return ptCenter - rcWindow.TopLeft ();
}

//------------------------------------------------------------------------------

void CMineView::OnMouseMove (UINT nFlags, CPoint point)
{
// This allows you to use mineview commands (next side, point etc) on mouseover
// instead of having to click (and potentially select something else).
if (GetFocus () != this)
    SetFocus ();

m_inputHandler.OnMouseMove (nFlags, AdjustMousePos (point));
}

//------------------------------------------------------------------------------

void CMineView::OnLButtonDown (UINT nFlags, CPoint point)
{
m_inputHandler.OnLButtonDown (nFlags, AdjustMousePos (point));
CView::OnLButtonDown (nFlags, point);
}

//------------------------------------------------------------------------------

void CMineView::OnLButtonUp (UINT nFlags, CPoint point)
{
m_inputHandler.OnLButtonUp (nFlags, AdjustMousePos (point));
CView::OnLButtonUp (nFlags, point);
}

//------------------------------------------------------------------------------

void CMineView::OnRButtonDown (UINT nFlags, CPoint point)
{
m_inputHandler.OnRButtonDown (nFlags, AdjustMousePos (point));
CView::OnRButtonDown (nFlags, point);
}

//------------------------------------------------------------------------------

void CMineView::OnRButtonUp (UINT nFlags, CPoint point)
{
m_inputHandler.OnRButtonUp (nFlags, AdjustMousePos (point));
CView::OnRButtonUp (nFlags, point);
}

//------------------------------------------------------------------------------

void CMineView::StartMovementTimer()
{
QueryPerformanceCounter (&m_lastFrameTime);
m_bMovementTimerActive = true;
Refresh (false);
}

//------------------------------------------------------------------------------

void CMineView::StopMovementTimer()
{
m_bMovementTimerActive = false;
Refresh (false);
}

//------------------------------------------------------------------------------

void CMineView::UpdateSelectHighlights ()
{
    if (IsSelectMode(SelectMode::Point) || IsSelectMode(SelectMode::Line) ||
        IsSelectMode(SelectMode::Side) || IsSelectMode(SelectMode::Segment))
        Invalidate(FALSE);
}

//------------------------------------------------------------------------------

void CMineView::Invalidate (BOOL bErase)
{
CWnd::Invalidate (bErase);
m_needsRepaint = true;
}

//------------------------------------------------------------------------------

void CMineView::InvalidateRect (LPCRECT lpRect, BOOL bErase)
{
CWnd::InvalidateRect (lpRect, bErase);
m_needsRepaint = true;
}

//------------------------------------------------------------------------------

void CMineView::Refresh (bool bAll)
{
CHECKMINE;

    static bool bRefreshing = false;

if (!(bRefreshing || m_nDelayRefresh)) {
    bRefreshing = true;
    InvalidateRect (null, TRUE);
    // Don't update the UI panels if we're moving the mouse because this
    // results in repaints every frame which is slow
    if (bAll && (m_inputHandler.MouseState () == eMouseStateIdle || m_inputHandler.IsStateExiting ())) {
        DLE.ToolView ()->Refresh ();
        DLE.TextureView ()->Refresh ();
        }
    m_needsRepaint = true;
    bRefreshing = false;
    }
}

//------------------------------------------------------------------------------

void CMineView::OnUpdate (CView* pSender, LPARAM lHint, CGameObject* pHint)
{
Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::TagRubberBandedVertices (CPoint clickPos, CPoint releasePos, bool bTag)
{
    CHECKMINE;

    for (int i = 0, j = vertexManager.Count(); i < j; i++)
    {
        CVertex& v = vertexManager[i];
        if (BETWEEN(clickPos.x, v.m_screen.x, releasePos.x) &&
            BETWEEN(clickPos.y, v.m_screen.y, releasePos.y) &&
            v.m_view.v.z > 0 && // in front of camera
            m_presenter.IsVertexVisible(i))
        {
            if (bTag)
                vertexManager[i].Tag();
            else
                vertexManager[i].UnTag();
            m_needsRepaint = true;
        }
    }
    if (m_needsRepaint)
        segmentManager.UpdateTagged();
    Refresh();
}

//==========================================================================
//==========================================================================

void CMineView::RefreshObject (short oldObject, short newObject) 
{
current->SetObjectId (newObject);
DLE.ToolView ()->Refresh ();
Refresh (false);
}

//------------------------------------------------------------------------------

void CMineView::UpdateRubberRect (CPoint clickPos, CPoint pt)
{
    CHECKMINE;

    // If this is the first frame we're drawing the rubber rect, capture the mouse
    if (!m_rubberRectInProgress)
    {
        m_rubberRectInProgress = true;
        SetCapture();
        m_presenter.BeginRubberRect(clickPos);
    }

    m_presenter.UpdateRubberRect(pt);
    Refresh(false);
}

//------------------------------------------------------------------------------

void CMineView::ResetRubberRect ()
{
CHECKMINE;

m_rubberRectInProgress = false;
ReleaseCapture ();
m_presenter.ClearRubberRect();
}

//------------------------------------------------------------------------------

void CMineView::DoContextMenu (CPoint point)
{
    static char* showSelectionCandidates [] = {"Show Selection Candidates: Off", "Show Selection Candidates: Circles", "Show Selection Candidates: Full"};
    CMenu contextMenu;
    CMenu* tracker;

contextMenu.LoadMenu (IDR_MINE_CONTEXT_MENU);
ClientToScreen (&point);
tracker = contextMenu.GetSubMenu (0); 
tracker->CheckMenuItem ((UINT) theMine->SelectMode (), MF_BYPOSITION | MF_CHECKED);
if (GetElementMovementReference ())
    tracker->CheckMenuItem ((UINT) ID_EDIT_MOVE_ALONG_VIEWER_AXES, MF_BYCOMMAND | MF_CHECKED);
if (m_bEnableQuickSelection)
    tracker->CheckMenuItem ((UINT) ID_EDIT_ENABLE_QUICK_SELECTION, MF_BYCOMMAND | MF_CHECKED);
tracker->ModifyMenu (ID_EDIT_SHOW_SELECTION_CANDIDATES, MF_BYCOMMAND | MF_STRING, ID_EDIT_SHOW_SELECTION_CANDIDATES, showSelectionCandidates [m_nShowSelectionCandidates]);
int nChoice = tracker->TrackPopupMenu (TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x , point.y, AfxGetMainWnd ());
contextMenu.DestroyMenu ();
if (nChoice) {
    if ((nChoice >= ID_SEL_POINTMODE) && (nChoice <= ID_SEL_BLOCKMODE))
        SetSelectMode (nChoice - ID_SEL_POINTMODE);
    else if (nChoice == ID_EDIT_ENABLE_QUICK_SELECTION)
        m_bEnableQuickSelection = !m_bEnableQuickSelection;
    else if (nChoice == ID_EDIT_SHOW_SELECTION_CANDIDATES)
        m_nShowSelectionCandidates = (m_nShowSelectionCandidates + 1) % 3;
    else if (nChoice == ID_EDIT_MOVE_ALONG_VIEWER_AXES)
        SetElementMovementReference (!GetElementMovementReference ());
    else if (nChoice == ID_EDIT_ALIGN_VIEWER_WITH_SIDE)
        AlignViewerWithSide ();
    else if (nChoice == ID_EDIT_QUICKCOPY)
        blockManager.QuickCopy (current);
    else if (nChoice == ID_EDIT_QUICKPASTE)
        DLE.GetDocument()->OnQuickPasteBlock();
    else if (nChoice == ID_EDIT_DELETEBLOCK)
        DLE.GetDocument()->OnDeleteBlock();
    else if (nChoice == ID_VIEW_COLLAPSE_EDGE)
        segmentManager.CollapseEdge (current->SegmentId(), current->SideId(), current->Edge());
    else if (nChoice == ID_VIEW_CREATE_WEDGE)
        segmentManager.CreateWedge (current);
    else if (nChoice == ID_VIEW_CREATE_PYRAMID)
        segmentManager.CreatePyramid (current);
    else if (nChoice == ID_MAKE_POINTS_PARALLEL)
        segmentManager.MakePointsParallel (current);
    else if (nChoice == ID_EDIT_UNDO)
        undoManager.Undo ();
    else if (nChoice == ID_EDIT_REDO)
        undoManager.Redo ();
    Refresh ();
    }
}

                                /*---------------------------*/

BOOL CMineView::OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
    static short wheelDeltas [2] = {WHEEL_DELTA, WHEEL_DELTA / 10};

    CRect	rc;

GetWindowRect (rc);
if ((pt.x < rc.left) || (pt.x >= rc.right) || (pt.y < rc.top) || (pt.y >= rc.bottom))
    return DLE.TextureView ()->OnMouseWheel (nFlags, zDelta, pt);
if (zDelta > 0)
    ZoomIn (zDelta / wheelDeltas [Perspective ()]);
else
    ZoomOut (-zDelta / wheelDeltas [Perspective ()]);
return 0;
}

//------------------------------------------------------------------------------

void CMineView::OnSelectPrevTab ()
{
DLE.MainFrame ()->ShowTools ();
DLE.ToolView ()->PrevTab ();
}

//------------------------------------------------------------------------------

void CMineView::OnSelectNextTab ()
{
DLE.MainFrame ()->ShowTools ();
DLE.ToolView ()->NextTab ();
}

//------------------------------------------------------------------------------

void CMineView::OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
UINT nPos = GetScrollPos (SB_HORZ);
CRect rect;
GetClientRect(rect);

m_bDelayRefresh = true;
switch (scrollCode) {
    case SB_LINEUP:
        nPos--;
        break;
    case SB_LINEDOWN:
        nPos++;
        break;
    case SB_PAGEUP:
        nPos -= ViewWidth ();
        break;
    case SB_PAGEDOWN:
        nPos += ViewWidth ();
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        nPos = thumbPos;
        break;
    case SB_ENDSCROLL:
        m_bDelayRefresh = false;
        Refresh (false);
        return;
}
SetScrollPos (SB_HORZ, nPos, TRUE);
Refresh ();
}

//------------------------------------------------------------------------------

void CMineView::OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar)
{
int nPos = GetScrollPos (SB_VERT);
CRect rect;
GetClientRect(rect);

m_bDelayRefresh = true;
switch (scrollCode) {
    case SB_LINEUP:
        nPos--;
        break;
    case SB_LINEDOWN:
        nPos++;
        break;
    case SB_PAGEUP:
        nPos -= ViewHeight ();
        break;
    case SB_PAGEDOWN:
        nPos += ViewHeight ();
        break;
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        nPos = thumbPos;
        break;
    case SB_ENDSCROLL:
        m_bDelayRefresh = false;
        Refresh (false);
        return;
}
if (nPos < 0)
    nPos = 0;
else if (nPos >= m_yScrollRange)
    nPos = m_yScrollRange - 1;
SetScrollPos (SB_VERT, nPos, TRUE);
Refresh ();
}

//------------------------------------------------------------------------------

BOOL CMineView::PreTranslateMessage (MSG* pMsg)
{
if (m_inputHandler.HasInputLock ())
    switch (pMsg->message) {
        case WM_KEYDOWN:
            return m_inputHandler.OnKeyDown (pMsg->wParam, LOWORD (pMsg->lParam), HIWORD (pMsg->lParam));

        case WM_KEYUP:
            return m_inputHandler.OnKeyUp (pMsg->wParam, LOWORD (pMsg->lParam), HIWORD (pMsg->lParam));

        default:
            break;
        }
return FALSE;
}

//------------------------------------------------------------------------------

void CMineView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
m_inputHandler.OnKeyUp (nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

void CMineView::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
m_inputHandler.OnKeyDown (nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

void CMineView::OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
// We need to do this so the mine view doesn't eat Alt+F4. It does make the menu
// steal focus more easily if you press Alt, though.
if (!m_inputHandler.OnKeyUp (nChar, nRepCnt, nFlags))
    CView::OnSysKeyUp (nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

void CMineView::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
if (!m_inputHandler.OnKeyDown (nChar, nRepCnt, nFlags))
    CView::OnSysKeyDown (nChar, nRepCnt, nFlags);
}

//------------------------------------------------------------------------------

void CMineView::OnSetFocus (CWnd* pOldWnd)
{
m_inputHandler.OnSetFocus ();
}

//eof
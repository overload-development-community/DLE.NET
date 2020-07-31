// dlcView.h : interface of the CMineView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __mineview_h
#define __mineview_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "afxcview.h"
#include "MineView.Presenter.h"

// -----------------------------------------------------------------------------

enum eMouseStates
{
	eMouseStateIdle,
	eMouseStateQuickSelect,
	eMouseStateQuickSelectObject,
	eMouseStateDrag,
	eMouseStateRubberBandTag,
	eMouseStateRubberBandUnTag,
	eMouseStateQuickTag,
	eMouseStateDoContextMenu,
	eMouseStateSelect,
	eMouseStateApplySelect,
	eMouseStatePan,
	eMouseStateRotate,
	eMouseStateZoomIn,
	eMouseStateZoomOut,
	eMouseStateLockedRotate,
	//must always be last tag
	eMouseStateCount
};

// -----------------------------------------------------------------------------

enum eMouseStateMatchResults
{
	eMatchNone = 0,
	eMatchPartial = 1,
	eMatchPartialCompleted = 2,
	eMatchExact = 3
};

// -----------------------------------------------------------------------------

enum eMovementModes
{
	eMovementModeStepped = 0,
	eMovementModeContinuous = 1
};

// -----------------------------------------------------------------------------

enum eModifierKeys
{
	eModifierShift,
	eModifierCtrl,
	eModifierAlt,
	// must always be last tag
	eModifierCount
};

// -----------------------------------------------------------------------------

struct MouseStateConfig {
	// Mouse button used for state (if any)
	UINT button;
	// List of modifiers (true means the modifier is required for this state)
	bool modifiers [eModifierCount];
	// Do the modifier keys act as toggles, rather than needing to be held down?
	bool bToggleModifiers;
	// Are the mouse X/Y axes inverted for this state?
	bool bInvertX;
	bool bInvertY;
};

// -----------------------------------------------------------------------------

enum eKeyCommands {
	eKeyCommandMoveForward,
	eKeyCommandMoveBackward,
	eKeyCommandMoveLeft,
	eKeyCommandMoveRight,
	eKeyCommandMoveUp,
	eKeyCommandMoveDown,
	eKeyCommandRotateUp,
	eKeyCommandRotateDown,
	eKeyCommandRotateLeft,
	eKeyCommandRotateRight,
	eKeyCommandRotateBankLeft,
	eKeyCommandRotateBankRight,
	eKeyCommandZoomIn,
	eKeyCommandZoomOut,
	eKeyCommandInputLock,
	// Must always be last tags; add new commands above this line
	eKeyCommandCount,
	eKeyCommandUnknown = -1
};

// -----------------------------------------------------------------------------

struct KeyboardBinding {
	// Virtual key code (0 if the command is not bound to a key)
	UINT nChar;
	// List of modifiers (true means the modifier is required for this state)
	bool modifiers [eModifierCount];
	// Is the binding ignored except under input lock?
	bool bNeedsInputLock;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Forward declaration for CInputHandler
class CMineView;

class IMouseInputState {
	public:
		virtual eMouseStates GetValue () const = 0;
		virtual const CPoint* GetStartPos () const = 0;
		virtual HCURSOR GetCursor () const = 0;
		virtual void LoadConfig () = 0;

		// Called after the state has been entered
		// msg indicates the event that caused the entry; point indicates the mouse location
		virtual void OnEntered (UINT msg, const CPoint& point) = 0;
		// Called after the state has been exited
		// msg indicates the event that caused the exit; point indicates the mouse location
		virtual void OnExited (UINT msg, const CPoint& point) = 0;
		// Called whenever the mouse has moved
		virtual void OnMouseMove (const CPoint& point) = 0;

		// Checks whether the state will be entered with the event specified
		virtual eMouseStateMatchResults HasEntered (UINT msg, const CPoint& point) const = 0;
		// Checks whether the state will exit with the event specified
		virtual bool HasExited (UINT msg, const CPoint& point) const = 0;
		// Checks whether a transition to another state is permissible with the event specified
		virtual bool IsExitAllowed (UINT msg, const CPoint& point) const = 0;
		// Checks whether a specific transition is permissible with the event specified
		virtual bool IsTransitionValid (eMouseStates newState, UINT msg, const CPoint& point) const = 0;
};

class CInputHandler {
	public:
		CInputHandler (CMineView *pMineView);
		virtual ~CInputHandler ();

		void LoadSettings ();
		void UpdateMovement (double timeElapsed);
	
		bool OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
		bool OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		void OnMouseMove (UINT nFlags, CPoint point);
		void OnLButtonUp (UINT nFlags, CPoint point);
		void OnLButtonDown (UINT nFlags, CPoint point);
		void OnRButtonUp (UINT nFlags, CPoint point);
		void OnRButtonDown (UINT nFlags, CPoint point);
		void OnMButtonUp (UINT nFlags, CPoint point);
		void OnMButtonDown (UINT nFlags, CPoint point);
		void OnXButtonUp (UINT nFlags, UINT nButton, CPoint point);
		void OnXButtonDown (UINT nFlags, UINT nButton, CPoint point);
		void OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
		void OnSetFocus ();

		eMouseStates MouseState () const { return m_pCurrentMouseState->GetValue (); }
		HCURSOR GetCurrentCursor () const { return m_pCurrentMouseState->GetCursor (); }
		const CPoint& LastMousePos () const { return m_lastMousePos; }
		bool HasMouseMovedInCurrentState () const { return HasMouseMoved (LastMousePos ()); }
		bool HasInputLock () const { return m_bInputLockActive; }
		bool IsStateExiting () const { return m_bIsStateExiting; }

	private:
		CMineView *m_pMineView;
		KeyboardBinding m_keyBindings [eKeyCommandCount];
		eMovementModes m_movementMode;
		double m_moveScale;
		double m_rotateScale;
		bool m_bFpInputLock;
		IMouseInputState *m_pMouseStates [eMouseStateCount];
		IMouseInputState *m_pCurrentMouseState;
		bool m_bIsStateExiting;
		CPoint m_lastMousePos;
		UINT m_mouseButtonStates;
		bool m_bModifierActive [eModifierCount];
		bool m_bKeyCommandActive [eKeyCommandCount];
		bool m_bInputLockActive;
		int m_nMovementCommandsActive;

		IMouseInputState *GetMouseState (eMouseStates state) const;
		eMouseStates MapInputToMouseState (UINT msg, const CPoint point) const;
		bool HasMouseMoved (const CPoint point) const;
		// Update mouse state in response to mouse input (e.g. clicks)
		void UpdateMouseState (UINT msg, CPoint point);
		// Update mouse state in response to keyboard input
		void UpdateMouseState (UINT msg);
		void UpdateModifierStates (UINT msg, UINT nChar, UINT nFlags);
		bool UpdateInputLockState (UINT msg, UINT nChar);
		bool IsMovementCommand (eKeyCommands command);
		eKeyCommands MapKeyToCommand (UINT nChar);
		bool KeyMatchesKeyCommand (eKeyCommands command, UINT nChar);
		void ApplyMovement (eKeyCommands command);
		void StartMovement (eKeyCommands command);
		void StopMovement (eKeyCommands command);
		void StopAllMovement ();
		static void LoadKeyBinding (KeyboardBinding &binding, LPCTSTR bindingName);
		static void LoadStateConfig (MouseStateConfig &config, LPCTSTR bindingName);
		static void LoadModifiers (bool (&modifierList) [eModifierCount], LPTSTR szMods);
		static UINT StringToVK (LPCTSTR pszKey);
		static UINT StringToMK (LPCTSTR pszButton);

		template < eMouseStates T > friend class CMouseInputStateBase;
		friend class CMouseStateIdle;
		friend class CMouseStateQuickSelect;
		friend class CMouseStateQuickSelectObject;
		friend class CMouseStateDrag;
		friend class CMouseStateRubberBandTag;
		friend class CMouseStateRubberBandUnTag;
		friend class CMouseStateQuickTag;
		friend class CMouseStateDoContextMenu;
		friend class CMouseStateSelect;
		friend class CMouseStateApplySelect;
		friend class CMouseStateLockedRotate;
		friend class CMouseStatePan;
		friend class CMouseStateRotate;
		friend class CMouseStateZoomIn;
		friend class CMouseStateZoomOut;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CViewPoint {
	public:
		ubyte				m_bTransformed :1;
		ubyte				m_bProjected :1;
		CDoubleVector	m_view;
		CLongVector		m_screen;

	public:
		CViewPoint () : m_bTransformed (0), m_bProjected (0) {}
		void Transform (void);
		void Project (void);
		inline void Reset (void) { m_bTransformed = m_bProjected = 0; }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

class CMineView : public CView
{
protected: // create from serialization only
	CMineView();
	DECLARE_DYNCREATE(CMineView)

	CSplitterWnd*	m_pSplitter;
	CMineViewPresenter m_presenter;
	CInputHandler	m_inputHandler;

	// member variables
	bool 				m_needsRepaint;
	bool 				m_bUpdateCursor;
	bool 				m_bDelayRefresh;
	int 				m_nDelayRefresh;
	SelectMode			m_selectMode;
	double			m_moveRate [2];

	short				m_lastSegment;
	UINT_PTR			m_lightTimer;
	UINT_PTR			m_selectTimer;
	int 				m_nFrameRate;
	int				m_nShowSelectionCandidates;
	int				m_bEnableQuickSelection;
	int				m_nElementMovementReference;
	int m_xScrollRange;
	int m_yScrollRange;

	int 				m_x0, m_x1, m_y;
	double			m_z0, m_z1;

	bool m_bMovementTimerActive;
	LARGE_INTEGER m_lastFrameTime;
	LARGE_INTEGER m_qpcFrequency;

// Attributes
public:
	CDlcDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMineView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CGameObject* pHint);
	virtual BOOL PreTranslateMessage (MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMineView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	inline void SetViewDistIndex(int nViewDist) { if (m_presenter.SetViewDistIndex(nViewDist)) Refresh(); }
	inline int ViewDistIndex() { return m_presenter.ViewDistIndex(); }
	inline int ViewDist() { return m_presenter.ViewDist(); }
	inline bool Visible(CSegment* pSegment) { return m_presenter.Visible(pSegment); }

	inline void SetElementMovementReference (int nReference) { m_nElementMovementReference = nReference; }
	inline int GetElementMovementReference (void) { return Perspective () && m_nElementMovementReference; }
	inline int EnableQuickSelection (void) { return m_bEnableQuickSelection; }
	inline int ShowSelectionCandidates (void) { return m_nShowSelectionCandidates; }
	bool VertexVisible (int v);
	void ComputeViewLimits (CRect* pRC = null);
	// drawing functions
	bool UpdateScrollBars (void);
	void ResetView (bool bRefresh = false);
	void UpdateStatusText();

	// view control functions
	int FitToView (void);
	void TogglePerspective (void);
	void OverridePerspective (bool bEnable, int nPerspective = 1);
	void Rotate (char direction, double angle);
	void CenterOnSegment ();
	void CenterOnObject ();
	void SetViewOption (eViewOptions option);
	void ToggleViewMine (eMineViewFlags flag);
	void ToggleViewObjects (eObjectViewFlags mask);
	void SetViewMineFlags (uint mask);
	void SetViewObjectFlags (uint mask);
	void SetSelectMode (uint mode);

	void NextPoint (int dir = 1);
	void PrevPoint ();
	void NextLine (int dir = 1);
	void PrevLine ();
	void NextSide (int dir = 1);
	void PrevSide ();
	void NextSegment (int dir = 1);
	void PrevSegment ();
	void SegmentForward (int dir = 1);
	void SegmentBackwards ();
	void SelectOtherSegment ();
	bool SelectOtherSide ();
	void NextObject (int dir = 1);
	void PrevObject ();
	void NextSegmentElement (int dir = 1);
	void PrevSegmentElement ();
	void HiliteTarget (void);
	void UpdateNearestSelection();

	void Refresh (bool bAll = true);
	void EnableDeltaShading (int bEnable, int nFrameRate, int bShowLightSource);
	void AdvanceLightTick (void);
	bool SetLightStatus (void);
	void StartMovementTimer ();
	void StopMovementTimer ();
	void UpdateSelectHighlights ();
	void Invalidate (BOOL bErase);
	void InvalidateRect (LPCRECT lpRect, BOOL bErase);

	void AlignViewerWithSide (void);

	inline bool ViewObject(CGameObject* pObject) { return m_presenter.ViewObject(pObject); }
	inline bool ViewObject(uint flag = 0) { return m_presenter.ViewObject(flag); }
	inline bool ViewFlag(uint flag = 0) { return m_presenter.ViewFlag(flag); }
	inline bool ViewOption(uint option) { return m_presenter.ViewOption(option); }
	inline bool IsSelectMode(SelectMode mode) { return m_presenter.IsSelectMode(mode); }
	inline uint GetMineViewFlags () { return ViewMineFlags (); }
	inline uint GetObjectViewFlags () { return ViewObjectFlags (); }
	inline uint GetViewOptions() { return m_presenter.GetViewOptions(); }
	inline SelectMode GetSelectMode () { return m_selectMode; }
	inline int& MineCenter (void) { return (int&)m_presenter.OriginDisplayType(); }
	inline void DelayRefresh (bool bDelay) {
		if (bDelay)
			m_nDelayRefresh++;
		else if (m_nDelayRefresh > 0)
			m_nDelayRefresh--;
		}
	inline bool DelayRefresh (void) { return m_nDelayRefresh > 0; }

	CPoint AdjustMousePos (CPoint point);
	BOOL UpdateCursor ();
	const CPoint& LastMousePos () { return m_inputHandler.LastMousePos (); }
	CPoint CenterMouse ();

	inline short Wrap (short v, short delta, short min, short max) {
		v += delta;
		if (v > max)
			v = min;
		else if (v < min)
			v = max;
		return v;
		}

	int FindNearestVertex (long xMouse, long yMouse, bool bCurrentSideOnly);
	bool SelectCurrentSegment (long xMouse, long yMouse, bool bAddToTagged = false);
	bool SelectCurrentSide (long xMouse, long yMouse, bool bAddToTagged = false);
	bool SelectCurrentLine (long xMouse, long yMouse, bool bAddToTagged = false);
	bool SelectCurrentPoint (long xMouse, long yMouse, bool bAddToTagged = false);
	void SelectCurrentObject (long xMouse, long yMouse);
	bool SelectCurrentElement (long xMouse, long yMouse, bool bAddToTagged);
	void RefreshObject(short old_object, short new_object);
	void TagRubberBandedVertices (CPoint clickPos, CPoint releasePos, bool bTag);
	BOOL DrawRubberBox ();
	void UpdateRubberRect (CPoint clickPos, CPoint pt);
	void ResetRubberRect ();
	void DoContextMenu (CPoint point);
	BOOL UpdateDragPos ();
	void HighlightDrag (short nVert, long x, long y);
	BOOL DrawDragPos (void);
	void InitDrag ();
	void FinishDrag (CPoint releasePos);
	void TagSelected();

	void LocateTexture (short nTexture);

	BOOL SetWindowPos (const CWnd *pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags);

	CRenderer& Renderer() { return m_presenter.Renderer(); }
	void SetRenderer(int nRenderer);
	inline int GetRenderer() { return m_presenter.GetRenderer() == RendererType::OpenGL; }
	void SetPerspective(int nPerspective);
	inline int Perspective() { return m_presenter.Perspective(); }

	inline void Zoom (int nSteps, double zoom) { 
		Renderer ().Zoom (nSteps, zoom, ViewMoveRate()); 
		Refresh (false);
		}
	inline int Project (CRect* pRC = null, bool bCheckBehind = false) { return Renderer ().Project (pRC, bCheckBehind); } 
	inline void DrawFaceTextured (CFaceListEntry& fle) { Renderer ().DrawFaceTextured (fle); } 
	inline int FaceIsVisible (CSegment* pSegment, CSide* pSide) { return Renderer ().FaceIsVisible (pSegment, pSide); }
	inline void BeginRender (bool bOrtho = false) { Renderer ().BeginRender (bOrtho); }
	inline void EndRender (bool bSwapBuffers = false) { Renderer ().EndRender (bSwapBuffers); } 
	inline int ZoomIn (int nSteps = 1, bool bSlow = false) { 
		if (!Renderer ().ZoomIn (nSteps, bSlow, ViewMoveRate()))
			return 0;
		Refresh ();
		return 1;
		} 
	inline int ZoomOut (int nSteps = 1, bool bSlow = false) { 
		if (!Renderer ().ZoomOut (nSteps, bSlow, ViewMoveRate()))
			return 0;
		Refresh ();
		return 1;
		}
	inline CViewMatrix* ViewMatrix (void) { return Renderer ().ViewMatrix (); } 
	inline void SetCenter (CVertex v, int nType) { 
		Renderer ().SetCenter (v, nType); 
		//FitToView ();
		}

	inline void Pan (char direction, double offset) { 
		Renderer ().Pan (direction, offset, ViewMoveRate());
		Refresh (false);
		}
	inline void Reset (void);

	inline CDoubleVector& Center(void) { return m_presenter.Center(); }
	inline CDoubleVector& Translation (void) { return m_presenter.Translation(); }
	inline CDoubleVector& Scale (void) { return m_presenter.Scale(); }
	inline CDoubleVector& Rotation (void) { return m_presenter.Rotation(); }
	inline int& ViewWidth (void) { return m_presenter.ViewWidth(); }
	inline int& ViewHeight (void) { return m_presenter.ViewHeight(); }
	inline int& ViewDepth (void) { return m_presenter.ViewDepth(); }
	inline double DepthScale (void) { return ViewMatrix ()->DepthScale (); }
	//inline void SetDepthScale (double scale) { return ViewMatrix ()->SetDepthScale (scale); }
	inline void SetDepthScale (int i) { return ViewMatrix ()->SetDepthScale (i); }
	inline int DepthPerception (void) { return ViewMatrix ()->DepthPerception (); }
	inline CBGR* RenderBuffer (void) { return m_presenter.RenderBuffer(); }
	inline depthType* DepthBuffer (void) { return m_presenter.DepthBuffer(); }
	inline CBGR& RenderBuffer (int i) { return m_presenter.RenderBuffer(i); }
	inline depthType& DepthBuffer (int i) { return m_presenter.DepthBuffer(i); }
	inline void SetDepthBuffer (depthType* buffer) { m_presenter.SetDepthBuffer(buffer); }
	inline HPEN Pen (ePenColor nPen, int nWeight = 1) { return m_presenter.Pen(nPen, nWeight); }
	inline CVertex& MinViewPoint (void) { return m_presenter.MinViewPoint(); }
	inline CVertex& MaxViewPoint (void) { return m_presenter.MaxViewPoint(); }
	inline bool IgnoreDepth (void) { return m_presenter.IgnoreDepth(); }
	inline void SetIgnoreDepth (bool bIgnoreDepth) { m_presenter.SetIgnoreDepth(bIgnoreDepth); }
	inline bool DepthTest (void) { return m_presenter.DepthTest(); }
	inline void SetDepthTest (bool bDepthTest) { m_presenter.SetDepthTest(bDepthTest); }
	inline int RenderIllumination (void) { return m_presenter.RenderIllumination(); }
	inline int RenderVariableLights (void) { return m_presenter.RenderVariableLights(); }
	inline ubyte Alpha (void) { return m_presenter.Alpha(); }
	inline void SetAlpha (ubyte alpha) { m_presenter.SetAlpha(alpha); }
	inline uint& ViewMineFlags (void) { return reinterpret_cast<uint&>(m_presenter.ViewMineFlags()); }
	inline uint& ViewObjectFlags (void) { return reinterpret_cast<uint&>(m_presenter.ViewObjectFlags()); }
	inline double MineMoveRate (void) { return m_moveRate [0]; }
	inline double ViewMoveRate (void) { return m_moveRate [1]; }
	inline void SetMineMoveRate (double moveRate) { m_moveRate [0] = moveRate; }
	inline void SetViewMoveRate (double moveRate) { m_moveRate [1] = moveRate; }
	inline void GetMoveRates (double* moveRates) { 
		moveRates [0] = m_moveRate [0], 
		moveRates [1] = m_moveRate [1]; 
		}
	inline void SetMoveRates (double* moveRates) { 
		m_moveRate [0] = Clamp (moveRates [0], 0.001, 1000.0), 
		m_moveRate [1] = Clamp (moveRates [1], 0.001, 1000.0);
		}
	inline void SetInputSettings () { m_inputHandler.LoadSettings (); }

	inline HDC DC (void) { return Renderer ().DC (); }

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMineView)
	afx_msg void OnDestroy ();
	afx_msg void OnTimer (UINT_PTR nIdEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint ();
	afx_msg BOOL OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSelectPrevTab ();
	afx_msg void OnSelectNextTab ();
	afx_msg void OnHScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
	afx_msg void OnVScroll (UINT scrollCode, UINT thumbPos, CScrollBar *pScrollBar);
	afx_msg void OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus (CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in dlcView.cpp
inline CDlcDoc* CMineView::GetDocument()
   { return (CDlcDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#endif //__mineview_h

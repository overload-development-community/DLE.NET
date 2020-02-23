#include "mineview.h"
#include "dle-xp.h"

template < eMouseStates T >
class CMouseInputStateBase : public IMouseInputState {
	public:
		CMouseInputStateBase (CInputHandler *pInputHandler, LPCTSTR bindingName, LPCTSTR nIdCursor) :
			m_pInputHandler (pInputHandler),
			m_pMineView (pInputHandler->m_pMineView),
			m_bindingName (bindingName),
			m_bActive (false)
		{
			ZeroMemory (&m_stateConfig, sizeof (m_stateConfig));
			if (nIdCursor == IDC_ARROW)
				m_hCursor = LoadCursor (null, IDC_ARROW);
			else
				m_hCursor = LoadCursor (DLE.m_hInstance, nIdCursor);
		}

		eMouseStates GetValue () const { return T; }

		const MouseStateConfig& GetConfig () const { return m_stateConfig; }

		virtual void LoadConfig () {
			SetConfigDefaults ();
			CInputHandler::LoadStateConfig (m_stateConfig, m_bindingName);
			}

		const CPoint* GetStartPos () const {
			if (!m_bActive)
				return nullptr;
			return &m_startPos;
			}

		HCURSOR GetCursor () const { return m_hCursor; }

		void OnEntered (UINT msg, const CPoint& point) {
			m_bActive = true;
			m_startPos = point;
			OnEnteredImpl (msg, point);
			}

		void OnExited (UINT msg, const CPoint& point) {
			if (IsMessageMatchingButtonUp (msg))
				OnCompleted (msg, point);
			else
				OnCancelled (msg, point);
			m_bActive = false;
			}

		virtual void OnMouseMove (const CPoint& point) {}

		virtual eMouseStateMatchResults HasEntered (UINT msg, const CPoint& point) const {
			eMouseStateMatchResults result = eMatchExact;

			// Check modifiers
			for (int i = 0; i < eModifierCount; i++) {
				bool bRequired = m_stateConfig.modifiers [i];

				if (m_pInputHandler->m_bModifierActive [i]) {
					if (!bRequired)
						result = eMatchPartial;
					}
				else if (bRequired) {
					result = eMatchNone;
					break;
					}
				}

			if (result != eMatchNone && IsClickState ()) {
				// Check mouse buttons
				// States can have multiple buttons mapping to them so we only need one match
				bool bFound = false;
				if (m_pInputHandler->m_mouseButtonStates & MK_LBUTTON) {
					if (m_stateConfig.button & MK_LBUTTON)
						bFound = true;
					else
						result = eMatchPartial;
					}
				if (m_pInputHandler->m_mouseButtonStates & MK_MBUTTON) {
					if (m_stateConfig.button & MK_MBUTTON)
						bFound = true;
					else
						result = eMatchPartial;
					}
				if (m_pInputHandler->m_mouseButtonStates & MK_RBUTTON) {
					if (m_stateConfig.button & MK_RBUTTON)
						bFound = true;
					else
						result = eMatchPartial;
					}
				if (m_pInputHandler->m_mouseButtonStates & MK_XBUTTON1) {
					if (m_stateConfig.button & MK_XBUTTON1)
						bFound = true;
					else
						result = eMatchPartial;
					}
				if (m_pInputHandler->m_mouseButtonStates & MK_XBUTTON2) {
					if (m_stateConfig.button & MK_XBUTTON2)
						bFound = true;
					else
						result = eMatchPartial;
					}
				if (!bFound)
					result = eMatchNone;
				}

			// Check if the message we received was the last one necessary for a partial match
			if (result == eMatchPartial && IsMessageMatchingButtonDown (msg))
				result = eMatchPartialCompleted;

			return result;
			}

		virtual bool HasExited (UINT msg, const CPoint& point) const {
			// For states requiring a button click, the state continues after it is entered
			// until the button is released. For other states, we check the modifier keys to
			// see if something required has been released.
			if (IsClickState ()) {
				return IsMessageMatchingButtonUp (msg);
				}
			else {
				for (int i = 0; i < eModifierCount; i++) {
					bool bRequired = m_stateConfig.modifiers [i];

					if (bRequired && !m_pInputHandler->m_bModifierActive [i])
						return true;
					}
				return false;
				}
			}

		virtual bool IsExitAllowed (UINT msg, const CPoint& point) const {
			if (IsClickState ())
				return HasExited (msg, point);
			else
				return true;
			}

		virtual bool IsTransitionValid (eMouseStates /*newState*/, UINT msg, const CPoint& point) const {
			// Normally the target state doesn't matter
			return IsExitAllowed (msg, point);
			}

	protected:
		CInputHandler *m_pInputHandler;
		CMineView *m_pMineView;
		MouseStateConfig m_stateConfig;

		bool IsClickState () const { return m_stateConfig.button != 0; }

		bool IsMessageMatchingButtonUp (UINT msg) const {
			switch (msg) {
				// Using bitwise again. Note that this will allow any button up to leave a state
				// even if others are still held. Currently this is what we want but it may change later
				case WM_LBUTTONUP:
					if (m_stateConfig.button & MK_LBUTTON)
						return true;
					break;

				case WM_MBUTTONUP:
					if (m_stateConfig.button & MK_MBUTTON)
						return true;
					break;

				case WM_RBUTTONUP:
					if (m_stateConfig.button & MK_RBUTTON)
						return true;
					break;

				case (0x10000 | WM_XBUTTONUP):
					if (m_stateConfig.button & MK_XBUTTON1)
						return true;
					break;

				case (0x20000 | WM_XBUTTONUP):
					if (m_stateConfig.button & MK_XBUTTON2)
						return true;
					break;

				default:
					break;
				}

			return false;
			}

		bool IsMessageMatchingButtonDown (UINT msg) const {
			switch (msg) {
				case WM_LBUTTONDOWN:
					if (m_stateConfig.button & MK_LBUTTON)
						return true;
					break;

				case WM_MBUTTONDOWN:
					if (m_stateConfig.button & MK_MBUTTON)
						return true;
					break;

				case WM_RBUTTONDOWN:
					if (m_stateConfig.button & MK_RBUTTON)
						return true;
					break;

				case (0x10000 | WM_XBUTTONDOWN):
					if (m_stateConfig.button & MK_XBUTTON1)
						return true;
					break;

				case (0x20000 | WM_XBUTTONDOWN):
					if (m_stateConfig.button & MK_XBUTTON2)
						return true;
					break;

				default:
					break;
				}

			return false;
			}

	private:
		LPCTSTR m_bindingName;
		HCURSOR m_hCursor;
		bool m_bActive;
		CPoint m_startPos;

		virtual void SetConfigDefaults () = 0;
		// Called after the state has been entered
		// msg indicates the event that caused the entry; point indicates the mouse location
		virtual void OnEnteredImpl (UINT msg, const CPoint& point) {}
		// Called after the state has changed and should apply any resulting actions
		// msg indicates the event that caused the completion; point indicates the mouse location
		virtual void OnCompleted (UINT msg, const CPoint& point) {}
		// Called after the state has changed and should not apply any actions
		// msg indicates the event that caused the cancellation; point indicates the mouse location
		virtual void OnCancelled (UINT msg, const CPoint& point) {}
};

class CMouseStateIdle : public CMouseInputStateBase < eMouseStateIdle > {
	public:
		CMouseStateIdle (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "Idle", IDC_ARROW)
		{}

		// Has no config
		void LoadConfig (LPCTSTR bindingName) {}

		eMouseStateMatchResults HasEntered (UINT, const CPoint&) const { return eMatchPartial; }
		bool HasExited (UINT) const { return false; }
		bool IsExitAllowed (UINT, const CPoint&) const { return true; }

	private:
		void SetConfigDefaults () {}
};

class CMouseStateQuickSelect : public CMouseInputStateBase < eMouseStateQuickSelect > {
	public:
		CMouseStateQuickSelect (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "QuickSelect", IDC_ARROW)
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->SelectCurrentElement (point.x, point.y, false);
			}

		bool IsExitAllowed (UINT msg, const CPoint& point) const {
			// Mouse move will likely switch to another state like drag or rubber band
			if (m_pInputHandler->HasMouseMoved (point))
				return true;
			return CMouseInputStateBase::IsExitAllowed (msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_LBUTTON;
			}
};

class CMouseStateQuickSelectObject : public CMouseInputStateBase < eMouseStateQuickSelectObject > {
	public:
		CMouseStateQuickSelectObject (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "QuickSelectObject", IDC_ARROW)
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->SelectCurrentObject (point.x, point.y);
			}

		bool IsExitAllowed (UINT msg, const CPoint& point) const {
			if (m_pInputHandler->HasMouseMoved (point))
				return true;
			return CMouseInputStateBase::IsExitAllowed (msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_RBUTTON;
			}
};

class CMouseStateDrag : public CMouseInputStateBase < eMouseStateDrag > {
	public:
		CMouseStateDrag (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "Drag", MAKEINTRESOURCE (IDC_CURSOR_DRAG))
		{}

		void OnEnteredImpl (UINT msg, const CPoint& point) {
			m_pMineView->InitDrag ();
		}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->FinishDrag (point);
			}

		void OnMouseMove (const CPoint& point) {
			m_pMineView->UpdateDragPos ();
			}

		eMouseStateMatchResults HasEntered (UINT msg, const CPoint& point) const {
			eMouseStateMatchResults result = CMouseInputStateBase::HasEntered (msg, point);
			if (result &&
				(!m_pInputHandler->HasMouseMoved (point) ||
				!CheckValidDragTarget ()))
				result = eMatchNone;
			return result;
			}

		virtual bool IsExitAllowed (UINT msg, const CPoint& point) const {
			// Ban cancellations, they would leave us in a bad state
			return HasExited (msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_LBUTTON;
			}

		bool CheckValidDragTarget () const {
			const CPoint *pStartPos = m_pInputHandler->m_pCurrentMouseState->GetStartPos ();
			if (pStartPos == nullptr)
				return false;
			// Only counts as a drag if it hits a vertex
			int v = vertexManager.Index (current->Vertex ());
			if ((abs (pStartPos->x - vertexManager [v].m_screen.x) < 5) &&
				(abs (pStartPos->y - vertexManager [v].m_screen.y) < 5))
				return true;
			return false;
			}
};

class CMouseStateRubberBandTag : public CMouseInputStateBase < eMouseStateRubberBandTag > {
	public:
		CMouseStateRubberBandTag (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "Mark", MAKEINTRESOURCE (IDC_CURSOR_DRAG))
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->ResetRubberRect ();
			m_pMineView->TagRubberBandedVertices (*GetStartPos (), point, true);
			}

		void OnCancelled (UINT msg, const CPoint& point) {
			m_pMineView->ResetRubberRect ();
			}

		void OnMouseMove (const CPoint& point) {
			m_pMineView->UpdateRubberRect (*GetStartPos (), point);
			}

		eMouseStateMatchResults HasEntered (UINT msg, const CPoint& point) const {
			eMouseStateMatchResults result = CMouseInputStateBase::HasEntered (msg, point);
			if (result && !m_pInputHandler->HasMouseMoved (point))
				result = eMatchNone;
			return result;
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_LBUTTON | MK_RBUTTON;
			}
};

class CMouseStateRubberBandUnTag : public CMouseInputStateBase < eMouseStateRubberBandUnTag > {
	public:
		CMouseStateRubberBandUnTag (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "Unmark", MAKEINTRESOURCE (IDC_CURSOR_DRAG))
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->ResetRubberRect ();
			m_pMineView->TagRubberBandedVertices (*GetStartPos (), point, false);
			}

		void OnCancelled (UINT msg, const CPoint& point) {
			m_pMineView->ResetRubberRect ();
			}

		void OnMouseMove (const CPoint& point) {
			m_pMineView->UpdateRubberRect (*GetStartPos (), point);
			}

		eMouseStateMatchResults HasEntered (UINT msg, const CPoint& point) const {
			eMouseStateMatchResults result = CMouseInputStateBase::HasEntered (msg, point);
			if (result && !m_pInputHandler->HasMouseMoved (point))
				result = eMatchNone;
			return result;
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_LBUTTON | MK_RBUTTON;
			m_stateConfig.modifiers [eModifierShift] = true;
			}
};

class CMouseStateQuickTag : public CMouseInputStateBase < eMouseStateQuickTag > {
	public:
		CMouseStateQuickTag (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "QuickMark", IDC_ARROW)
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->SelectCurrentElement (point.x, point.y, true);
			}

		bool IsExitAllowed (UINT msg, const CPoint& point) const {
			if (m_pInputHandler->HasMouseMoved (point))
				return true;
			return CMouseInputStateBase::IsExitAllowed (msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_LBUTTON;
			m_stateConfig.modifiers [eModifierShift] = true;
			}
};

class CMouseStateDoContextMenu : public CMouseInputStateBase < eMouseStateDoContextMenu > {
	public:
		CMouseStateDoContextMenu (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "ContextMenu", IDC_ARROW)
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->DoContextMenu (point);
			}

		bool IsExitAllowed (UINT msg, const CPoint& point) const {
			if (m_pInputHandler->HasMouseMoved (point))
				return true;
			return CMouseInputStateBase::IsExitAllowed (msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_RBUTTON;
			m_stateConfig.modifiers [eModifierShift] = true;
			}
};

class CMouseStateSelect : public CMouseInputStateBase < eMouseStateSelect > {
	public:
		CMouseStateSelect (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "AdvSelect", IDC_ARROW)
		{}

		void OnEnteredImpl (UINT msg, const CPoint& point) {
			m_pMineView->UpdateSelectHighlights ();
			}

		void OnCancelled (UINT msg, const CPoint& point) {
			m_pMineView->UpdateSelectHighlights ();
			}

		void OnMouseMove (const CPoint& point) {
			m_pMineView->UpdateSelectHighlights ();
			}

		bool IsTransitionValid (eMouseStates newState, UINT msg, const CPoint& point) const {
			// Always prefer apply select state if that is a valid outcome
			IMouseInputState *pApplySelectState = m_pInputHandler->GetMouseState (eMouseStateApplySelect);
			if (pApplySelectState->HasEntered (msg, point))
				return newState == eMouseStateApplySelect;
			return CMouseInputStateBase::IsTransitionValid (newState, msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.modifiers [eModifierShift] = true;
			}
};

class CMouseStateApplySelect : public CMouseInputStateBase < eMouseStateApplySelect > {
	public:
		CMouseStateApplySelect (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "ApplyAdvSelect", IDC_ARROW)
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			m_pMineView->SelectCurrentElement (point.x, point.y, false);
			}

		void OnCancelled (UINT msg, const CPoint& point) {
			m_pMineView->UpdateSelectHighlights ();
			}

		void OnMouseMove (const CPoint& point) {
			m_pMineView->UpdateSelectHighlights ();
			}

		eMouseStateMatchResults HasEntered (UINT msg, const CPoint& point) const {
			if (m_pInputHandler->MouseState () != eMouseStateSelect)
				return eMatchNone;
			eMouseStateMatchResults result = CMouseInputStateBase::HasEntered (msg, point);
			if (result && !m_pInputHandler->HasMouseMoved (point))
				result = eMatchNone;
			return result;
			}

		bool IsExitAllowed (UINT msg, const CPoint& point) const {
			if (m_pInputHandler->HasMouseMoved (point))
				return true;
			return CMouseInputStateBase::IsExitAllowed (msg, point);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.button = MK_LBUTTON | MK_RBUTTON;
			m_stateConfig.modifiers [eModifierShift] = true;
			}
};

class CMouseStatePan : public CMouseInputStateBase < eMouseStatePan > {
	public:
		CMouseStatePan (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "Pan", MAKEINTRESOURCE (IDC_CURSOR_PAN))
		{}

		void OnMouseMove (const CPoint& point) {
			CPoint change = m_pInputHandler->LastMousePos () - point;
			double scale = 0.5;
			double panAmountX = double(change.x) * scale;
			double panAmountY = -double(change.y) * scale;

			if (m_pMineView->Perspective ())
				panAmountX = -panAmountX;
			if (m_stateConfig.bInvertX)
				panAmountX = -panAmountX;
			if (m_stateConfig.bInvertY)
				panAmountY = -panAmountY;

			if (panAmountX != 0.0)
				m_pMineView->Pan ('X', panAmountX);
			if (panAmountY != 0.0)
				m_pMineView->Pan ('Y', panAmountY);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.modifiers [eModifierCtrl] = true;
			}
};

class CMouseStateRotate : public CMouseInputStateBase < eMouseStateRotate > {
	public:
		CMouseStateRotate (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "Rotate", MAKEINTRESOURCE (IDC_CURSOR_ROTATE))
		{}

		void OnMouseMove (const CPoint& point) {
			CPoint change = m_pInputHandler->LastMousePos () - point;
			double scale = m_pMineView->Perspective () ? 300.0 : 200.0;
			// RotateAmountX/Y are how much we rotate about either axis,
			// which is why it looks like we're swapping them here.
			double rotateAmountX = double (change.y) / scale;
			double rotateAmountY = -double (change.x) / scale;

			if (m_stateConfig.bInvertY)
				rotateAmountX = -rotateAmountX;
			if (m_stateConfig.bInvertX)
				rotateAmountY = -rotateAmountY;

			m_pMineView->Rotate ('Y', rotateAmountY);
			m_pMineView->Rotate ('X', rotateAmountX);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.modifiers [eModifierShift] = true;
			m_stateConfig.modifiers [eModifierCtrl] = true;
			}
};

template < eMouseStates T >
class CMouseStateZoom : public CMouseInputStateBase < T > {
	public:
		CMouseStateZoom (CInputHandler *pInputHandler, LPCTSTR bindingName, LPCTSTR nIdCursor) :
			CMouseInputStateBase (pInputHandler, bindingName, nIdCursor)
		{}

		void OnEnteredImpl (UINT msg, const CPoint& point) {
			// Some special handling for zoom - we need a separate tracking position
			// for it, because it uses stepping
			m_zoomStartPos = point;
			}

		void OnMouseMove (const CPoint& point) {
			CPoint zoomOffset = m_pMineView->Perspective () ?
				m_pInputHandler->LastMousePos () - point :
				point - m_zoomStartPos;

			if (m_stateConfig.bInvertX)
				zoomOffset.x = -zoomOffset.x;
			if (m_stateConfig.bInvertY)
				zoomOffset.y = -zoomOffset.y;

			if (m_pMineView->Perspective ()) {
				if ((zoomOffset.x > 0) || ((zoomOffset.x == 0) && (zoomOffset.y > 0))) {
					m_pMineView->ZoomOut (1, true);
					}
				else if ((zoomOffset.x < 0) || ((zoomOffset.x == 0) && (zoomOffset.y < 0))) {
					m_pMineView->ZoomIn (1, true);
					}
				}
			else {
				int nChange = zoomOffset.x + zoomOffset.y;
				if (nChange > 30) {
					m_zoomStartPos = point;
					m_pMineView->ZoomIn (1, true);
					}
				else if (nChange < -30) {
					m_zoomStartPos = point;
					m_pMineView->ZoomOut (1, true);
					}
				}
			}

	private:
		CPoint m_zoomStartPos;
};

class CMouseStateZoomIn : public CMouseStateZoom < eMouseStateZoomIn > {
	public:
		CMouseStateZoomIn (CInputHandler *pInputHandler) :
			CMouseStateZoom (pInputHandler, "ZoomIn", MAKEINTRESOURCE (IDC_CURSOR_ZOOMIN))
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			if (!m_pInputHandler->HasMouseMoved (point))
				m_pMineView->ZoomIn (1, true);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.modifiers [eModifierCtrl] = true;
			m_stateConfig.button = MK_LBUTTON;
			}
};

class CMouseStateZoomOut : public CMouseStateZoom < eMouseStateZoomOut > {
	public:
		CMouseStateZoomOut (CInputHandler *pInputHandler) :
			CMouseStateZoom (pInputHandler, "ZoomOut", MAKEINTRESOURCE (IDC_CURSOR_ZOOMOUT))
		{}

		void OnCompleted (UINT msg, const CPoint& point) {
			if (!m_pInputHandler->HasMouseMoved (point))
				m_pMineView->ZoomOut (1, true);
			}

	private:
		void SetConfigDefaults () {
			m_stateConfig.modifiers [eModifierCtrl] = true;
			m_stateConfig.button = MK_RBUTTON;
			}
};

class CMouseStateLockedRotate : public CMouseInputStateBase < eMouseStateLockedRotate > {
	public:
		CMouseStateLockedRotate (CInputHandler *pInputHandler) :
			CMouseInputStateBase (pInputHandler, "LockedRotate", MAKEINTRESOURCE (IDC_CURSOR_XHAIRS))
		{}

		// No config, delegates to Rotate
		void LoadConfig (LPCTSTR) {}

		void OnMouseMove (const CPoint& point) {
			CMouseStateRotate *pMouseStateRotate = dynamic_cast <CMouseStateRotate*> (m_pInputHandler->m_pMouseStates [eMouseStateRotate]);
			if (pMouseStateRotate != nullptr)
				pMouseStateRotate->OnMouseMove (point);
			}

		eMouseStateMatchResults HasEntered (UINT, const CPoint&) const {
			return m_pInputHandler->HasInputLock () ? eMatchExact : eMatchNone;
			}

		bool HasExited (UINT msg, const CPoint& point) const {
			return !m_pInputHandler->HasInputLock ();
			}

		bool IsExitAllowed (UINT, const CPoint&) const {
			return !m_pInputHandler->HasInputLock ();
			}

	private:
		void SetConfigDefaults () {}
};

CInputHandler::CInputHandler (CMineView *pMineView)
	: m_pMineView (pMineView)
{
for (int i = 0; i < eModifierCount; i++) {
	m_bModifierActive [i] = false;
	}
for (int i = 0; i < eKeyCommandCount; i++) {
	m_bKeyCommandActive [i] = false;
	}
m_bInputLockActive = false;
m_nMovementCommandsActive = 0;
ZeroMemory (m_keyBindings, sizeof (m_keyBindings));
ZeroMemory (m_pMouseStates, sizeof (m_pMouseStates));
// Initialize idle early since it is the initial mouse state
m_pMouseStates [eMouseStateIdle] = new CMouseStateIdle (this);
m_pCurrentMouseState = m_pMouseStates [eMouseStateIdle];
m_bIsStateExiting = false;
m_mouseButtonStates = 0;
}

CInputHandler::~CInputHandler ()
{
for (int i = eMouseStateIdle; i < eMouseStateCount; i++)
	delete m_pMouseStates [i];
}

void CInputHandler::LoadSettings ()
{
// Initialize non-idle states
m_pMouseStates [eMouseStateQuickSelect] = new CMouseStateQuickSelect (this);
m_pMouseStates [eMouseStateQuickSelectObject] = new CMouseStateQuickSelectObject (this);
m_pMouseStates [eMouseStateDrag] = new CMouseStateDrag (this);
m_pMouseStates [eMouseStateRubberBandTag] = new CMouseStateRubberBandTag (this);
m_pMouseStates [eMouseStateRubberBandUnTag] = new CMouseStateRubberBandUnTag (this);
m_pMouseStates [eMouseStateQuickTag] = new CMouseStateQuickTag (this);
m_pMouseStates [eMouseStateDoContextMenu] = new CMouseStateDoContextMenu (this);
m_pMouseStates [eMouseStateSelect] = new CMouseStateSelect (this);
m_pMouseStates [eMouseStateApplySelect] = new CMouseStateApplySelect (this);
m_pMouseStates [eMouseStatePan] = new CMouseStatePan (this);
m_pMouseStates [eMouseStateRotate] = new CMouseStateRotate (this);
m_pMouseStates [eMouseStateZoomIn] = new CMouseStateZoomIn (this);
m_pMouseStates [eMouseStateZoomOut] = new CMouseStateZoomOut (this);
m_pMouseStates [eMouseStateLockedRotate] = new CMouseStateLockedRotate (this);
for (int i = eMouseStateIdle; i < eMouseStateCount; i++)
	m_pMouseStates [i]->LoadConfig ();

// Read camera movement settings if specified
m_movementMode = (eMovementModes)appSettings.m_movementMode;
m_moveScale = appSettings.m_kbMoveScale;
m_rotateScale = appSettings.m_kbRotateScale;
m_bFpInputLock = appSettings.m_bFpInputLock > 0;

LoadKeyBinding (m_keyBindings [eKeyCommandMoveForward], "MoveForward");
LoadKeyBinding (m_keyBindings [eKeyCommandMoveBackward], "MoveBackward");
LoadKeyBinding (m_keyBindings [eKeyCommandMoveLeft], "MoveLeft");
LoadKeyBinding (m_keyBindings [eKeyCommandMoveRight], "MoveRight");
LoadKeyBinding (m_keyBindings [eKeyCommandMoveUp], "MoveUp");
LoadKeyBinding (m_keyBindings [eKeyCommandMoveDown], "MoveDown");
LoadKeyBinding (m_keyBindings [eKeyCommandRotateUp], "RotateUp");
LoadKeyBinding (m_keyBindings [eKeyCommandRotateDown], "RotateDown");
LoadKeyBinding (m_keyBindings [eKeyCommandRotateLeft], "RotateLeft");
LoadKeyBinding (m_keyBindings [eKeyCommandRotateRight], "RotateRight");
LoadKeyBinding (m_keyBindings [eKeyCommandRotateBankLeft], "BankLeft");
LoadKeyBinding (m_keyBindings [eKeyCommandRotateBankRight], "BankRight");
LoadKeyBinding (m_keyBindings [eKeyCommandZoomIn], "ZoomIn");
LoadKeyBinding (m_keyBindings [eKeyCommandZoomOut], "ZoomOut");
LoadKeyBinding (m_keyBindings [eKeyCommandInputLock], "InputLock");
}

void CInputHandler::UpdateMovement (double timeElapsed)
{
	// MineView/Renderer already apply view move rate
	double moveAmount = timeElapsed * m_moveScale;
	double rotateAmount = timeElapsed * m_rotateScale;
	static double zoomAmount = 0;
	double pspFlip = (m_pMineView->Perspective () ? -1 : 1);

if (m_bKeyCommandActive [eKeyCommandMoveForward])
	m_pMineView->Pan ('Z', moveAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandMoveBackward])
	m_pMineView->Pan ('Z', -moveAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandMoveLeft])
	m_pMineView->Pan ('X', -moveAmount);
if (m_bKeyCommandActive [eKeyCommandMoveRight])
	m_pMineView->Pan ('X', moveAmount);
if (m_bKeyCommandActive [eKeyCommandMoveUp])
	m_pMineView->Pan ('Y', -moveAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandMoveDown])
	m_pMineView->Pan ('Y', moveAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandRotateUp])
	m_pMineView->Rotate ('X', rotateAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandRotateDown])
	m_pMineView->Rotate ('X', -rotateAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandRotateLeft])
	m_pMineView->Rotate ('Y', rotateAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandRotateRight])
	m_pMineView->Rotate ('Y', -rotateAmount * pspFlip);
if (m_bKeyCommandActive [eKeyCommandRotateBankLeft])
	m_pMineView->Rotate ('Z', rotateAmount);
if (m_bKeyCommandActive [eKeyCommandRotateBankRight])
	m_pMineView->Rotate ('Z', -rotateAmount);

if (m_bKeyCommandActive [eKeyCommandZoomIn])
	zoomAmount += moveAmount;
if (m_bKeyCommandActive [eKeyCommandZoomOut])
	zoomAmount -= moveAmount;
if (fabs (zoomAmount) >= 1) {
	m_pMineView->ZoomIn ((int)zoomAmount);
	zoomAmount -= (int)zoomAmount;
	}
}

bool CInputHandler::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bInputHandled = false;

UpdateModifierStates (WM_KEYUP, nChar, nFlags);
UpdateMouseState (WM_KEYUP);
bInputHandled = UpdateInputLockState (WM_KEYUP, nChar);

eKeyCommands command = MapKeyToCommand (nChar);
if (IsMovementCommand (command)) {
	switch (m_movementMode) {
		case eMovementModeStepped:
			for (unsigned int i = 0; i < nRepCnt; i++) {
				ApplyMovement (command);
				}
			break;
		case eMovementModeContinuous:
			StopMovement (command);
			break;
		default:
			// Unknown mode
			break;
		}
	bInputHandled = true;
	}

return bInputHandled;
}

bool CInputHandler::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bInputHandled = false;

UpdateModifierStates (WM_KEYDOWN, nChar, nFlags);
UpdateMouseState (WM_KEYDOWN);
bInputHandled = UpdateInputLockState (WM_KEYDOWN, nChar);

eKeyCommands command = MapKeyToCommand (nChar);
if (IsMovementCommand (command)) {
	switch (m_movementMode) {
		case eMovementModeStepped:
			for (unsigned int i = 0; i < nRepCnt; i++) {
				ApplyMovement (command);
				}
			break;
		case eMovementModeContinuous:
			StartMovement (command);
			break;
		default:
			// Unknown mode
			break;
		}
	bInputHandled = true;
	}

return bInputHandled;
}

void CInputHandler::OnMouseMove (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_MOUSEMOVE, 0, nFlags);
UpdateMouseState (WM_MOUSEMOVE, point);

CPoint change = m_lastMousePos - point;
if (change.x || change.y)
	m_pCurrentMouseState->OnMouseMove (point);

if (MouseState () == eMouseStateLockedRotate)
	m_lastMousePos = m_pMineView->CenterMouse ();
else
	m_lastMousePos = point;
}

void CInputHandler::OnLButtonUp (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_LBUTTONUP, 0, nFlags);
UpdateMouseState (WM_LBUTTONUP, point);
}

void CInputHandler::OnLButtonDown (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_LBUTTONDOWN, 0, nFlags);
UpdateMouseState (WM_LBUTTONDOWN, point);
}

void CInputHandler::OnRButtonUp (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_RBUTTONUP, 0, nFlags);
UpdateMouseState (WM_RBUTTONUP, point);
}

void CInputHandler::OnRButtonDown (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_RBUTTONDOWN, 0, nFlags);
UpdateMouseState (WM_RBUTTONDOWN, point);
}

void CInputHandler::OnMButtonUp (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_MBUTTONUP, 0, nFlags);
UpdateMouseState (WM_MBUTTONUP, point);
}

void CInputHandler::OnMButtonDown (UINT nFlags, CPoint point)
{
UpdateModifierStates (WM_MBUTTONDOWN, 0, nFlags);
UpdateMouseState (WM_MBUTTONDOWN, point);
}

void CInputHandler::OnXButtonUp (UINT nFlags, UINT nButton, CPoint point)
{
UpdateModifierStates (WM_XBUTTONUP, 0, nFlags);
// this is a bit weird but testing nFlags would be unreliable -
// if both xbuttons are pressed it'll just see the one it checks first
UpdateMouseState ((nButton << 16) | WM_XBUTTONUP, point);
}

void CInputHandler::OnXButtonDown (UINT nFlags, UINT nButton, CPoint point)
{
UpdateModifierStates (WM_XBUTTONDOWN, 0, nFlags);
UpdateMouseState ((nButton << 16) | WM_XBUTTONDOWN, point);
}

void CInputHandler::OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
}

void CInputHandler::OnSetFocus ()
{
UpdateModifierStates (WM_SETFOCUS, 0, 0);
}

//------------------------------------------------------------------------------
// Internal functions

IMouseInputState *CInputHandler::GetMouseState (eMouseStates state) const
{
return m_pMouseStates [state];
}

eMouseStates CInputHandler::MapInputToMouseState (UINT msg, const CPoint point) const
{
if (!m_pCurrentMouseState->IsExitAllowed (msg, point))
	return MouseState ();

// Check states in three passes - first look for an exact match, then look for
// any partial match that this message was the last remaining condition for.
// The existing state is otherwise preferred, except when it has exited.
for (int i = eMouseStateIdle + 1; i < eMouseStateCount; i++) {
	eMouseStates state = (eMouseStates)i;
	if (state == MouseState ())
		continue;
	if (GetMouseState (state)->HasEntered (msg, point) == eMatchExact &&
		m_pCurrentMouseState->IsTransitionValid (state, msg, point))
		return state;
	}

for (int i = eMouseStateIdle + 1; i < eMouseStateCount; i++) {
	eMouseStates state = (eMouseStates)i;
	if (state == MouseState ())
		continue;
	if (GetMouseState (state)->HasEntered (msg, point) == eMatchPartialCompleted &&
		m_pCurrentMouseState->IsTransitionValid (state, msg, point))
		return state;
	}

if (!m_pCurrentMouseState->HasExited (msg, point))
	return m_pCurrentMouseState->GetValue ();

for (int i = eMouseStateIdle + 1; i < eMouseStateCount; i++) {
	eMouseStates state = (eMouseStates)i;
	if (state == MouseState ())
		continue;
	if (GetMouseState (state)->HasEntered (msg, point) == eMatchPartial &&
		m_pCurrentMouseState->IsTransitionValid (state, msg, point))
		return state;
	}

// If all else fails, go back to idle.
return eMouseStateIdle;
}

bool CInputHandler::HasMouseMoved (const CPoint point) const
{
	const CPoint *pStartPos = m_pCurrentMouseState->GetStartPos ();

if (pStartPos == nullptr)
	return false;
return point != *pStartPos;
}

void CInputHandler::UpdateMouseState (UINT msg, CPoint point)
{
eMouseStates newState = MapInputToMouseState (msg, point);

if (MouseState () != newState) {
	m_bIsStateExiting = true;
	m_pCurrentMouseState->OnExited (msg, point);
	m_bIsStateExiting = false;
	m_pCurrentMouseState = GetMouseState (newState);
	m_pCurrentMouseState->OnEntered (msg, point);
	m_pMineView->UpdateCursor ();
	}
}

void CInputHandler::UpdateMouseState (UINT msg)
{
UpdateMouseState (msg, m_lastMousePos);
}

void CInputHandler::UpdateModifierStates (UINT msg, UINT nChar, UINT nFlags)
{
switch (msg) {
	case WM_KEYDOWN:
	case WM_KEYUP:
		switch (nChar) {
			case VK_SHIFT:
				m_bModifierActive [eModifierShift] = (msg == WM_KEYDOWN);
				break;

			case VK_CONTROL:
				m_bModifierActive [eModifierCtrl] = (msg == WM_KEYDOWN);
				break;

			case VK_MENU:
				m_bModifierActive [eModifierAlt] = (msg == WM_KEYDOWN);
				break;

			default:
				break;
			}
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDOWN:
		m_bModifierActive [eModifierShift] = (nFlags & MK_SHIFT) == MK_SHIFT;
		m_bModifierActive [eModifierCtrl] = (nFlags & MK_CONTROL) == MK_CONTROL;
		m_bModifierActive [eModifierAlt] = (GetKeyState (VK_MENU) & 0xF0) > 0;
		m_mouseButtonStates = nFlags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2);
		break;

	case WM_SETFOCUS:
		// Low-order bits from GetKeyState report "toggle" state which is seemingly always set
		// on some of these keys. So we mask them out
		m_bModifierActive [eModifierShift] = (GetKeyState (VK_SHIFT) & 0xF0) > 0;
		m_bModifierActive [eModifierCtrl] = (GetKeyState (VK_CONTROL) & 0xF0) > 0;
		m_bModifierActive [eModifierAlt] = (GetKeyState (VK_MENU) & 0xF0) > 0;
		break;

	default:
		break;
	}
}

bool CInputHandler::IsMovementCommand (eKeyCommands command)
{
switch (command) {
	case eKeyCommandMoveForward:
	case eKeyCommandMoveBackward:
	case eKeyCommandMoveLeft:
	case eKeyCommandMoveRight:
	case eKeyCommandMoveUp:
	case eKeyCommandMoveDown:
	case eKeyCommandRotateUp:
	case eKeyCommandRotateDown:
	case eKeyCommandRotateLeft:
	case eKeyCommandRotateRight:
	case eKeyCommandRotateBankLeft:
	case eKeyCommandRotateBankRight:
	case eKeyCommandZoomIn:
	case eKeyCommandZoomOut:
		return true;

	default:
		return false;
	}
}

bool CInputHandler::UpdateInputLockState (UINT msg, UINT nChar)
{
// Input lock is a toggle, which means we have to handle it somewhat differently
if (KeyMatchesKeyCommand (eKeyCommandInputLock, nChar)) {
	if (msg == WM_KEYDOWN)
		m_bKeyCommandActive [eKeyCommandInputLock] = true;
	else if (msg == WM_KEYUP) {
		if (m_bKeyCommandActive [eKeyCommandInputLock]) {
			m_bKeyCommandActive [eKeyCommandInputLock] = false;
			m_bInputLockActive = !m_bInputLockActive;
			if (!m_bInputLockActive)
				StopAllMovement ();
			if (m_bFpInputLock)
				m_pMineView->OverridePerspective (1, m_bInputLockActive);
			return true;
			}
		}
	}

return false;
}

eKeyCommands CInputHandler::MapKeyToCommand (UINT nChar)
{
for (int command = 0; command < eKeyCommandCount; command++)
	if (KeyMatchesKeyCommand ((eKeyCommands)command, nChar))
		return (eKeyCommands)command;

return eKeyCommandUnknown;
}

bool CInputHandler::KeyMatchesKeyCommand (eKeyCommands command, UINT nChar)
{
if (command < 0 || command >= eKeyCommandCount)
	return false;

const KeyboardBinding &binding = m_keyBindings [command];

if (binding.bNeedsInputLock && !m_bInputLockActive)
	return false;

if (nChar != binding.nChar)
	return false;

// Check modifiers (on key down only; we don't want to insist on key up order)
if (!m_bKeyCommandActive [command])
	for (int i = 0; i < eModifierCount; i++) {
		bool bRequired = binding.modifiers [i];

		if (bRequired && !m_bModifierActive [i])
			return false;
		}

return true;
}

void CInputHandler::ApplyMovement (eKeyCommands command)
{
	double pspFlip = (m_pMineView->Perspective () ? -1 : 1);

if (!IsMovementCommand (command))
	return;

switch (command) {
	case eKeyCommandMoveForward:
		m_pMineView->Pan ('Z', 1 * pspFlip);
		break;
	case eKeyCommandMoveBackward:
		m_pMineView->Pan ('Z', -1 * pspFlip);
		break;
	case eKeyCommandMoveLeft:
		m_pMineView->Pan ('X', -1);
		break;
	case eKeyCommandMoveRight:
		m_pMineView->Pan ('X', 1);
		break;
	case eKeyCommandMoveUp:
		m_pMineView->Pan ('Y', -1 * pspFlip);
		break;
	case eKeyCommandMoveDown:
		m_pMineView->Pan ('Y', 1 * pspFlip);
		break;
	case eKeyCommandRotateUp:
		m_pMineView->Rotate ('X', 1 * pspFlip);
		break;
	case eKeyCommandRotateDown:
		m_pMineView->Rotate ('X', -1 * pspFlip);
		break;
	case eKeyCommandRotateLeft:
		m_pMineView->Rotate ('Y', 1 * pspFlip);
		break;
	case eKeyCommandRotateRight:
		m_pMineView->Rotate ('Y', -1 * pspFlip);
		break;
	case eKeyCommandRotateBankLeft:
		m_pMineView->Rotate ('Z', 1);
		break;
	case eKeyCommandRotateBankRight:
		m_pMineView->Rotate ('Z', -1);
		break;
	case eKeyCommandZoomIn:
		m_pMineView->ZoomIn ();
		break;
	case eKeyCommandZoomOut:
		m_pMineView->ZoomOut ();
		break;
	default:
		break;
	}
}

void CInputHandler::StartMovement (eKeyCommands command)
{
if (!IsMovementCommand (command))
	return;

if (m_bKeyCommandActive [command])
	return;

m_bKeyCommandActive [command] = true;
m_nMovementCommandsActive++;
if (m_nMovementCommandsActive == 1)
	m_pMineView->StartMovementTimer ();
}

void CInputHandler::StopMovement (eKeyCommands command)
{
if (!IsMovementCommand (command))
	return;

if (!m_bKeyCommandActive [command])
	return;

m_bKeyCommandActive [command] = false;
m_nMovementCommandsActive--;
if (m_nMovementCommandsActive == 0)
	m_pMineView->StopMovementTimer ();
}

void CInputHandler::StopAllMovement ()
{
if (m_movementMode != eMovementModeContinuous)
	return;

for (int command = 0; command < eKeyCommandCount; command++)
	if (IsMovementCommand ((eKeyCommands)command))
		StopMovement ((eKeyCommands)command);
}

void CInputHandler::LoadKeyBinding (KeyboardBinding &binding, LPCTSTR bindingName)
{
	TCHAR szSettingNameChar [80] = { 0 };
	TCHAR szSettingNameModifiers [80] = { 0 };
	TCHAR szSettingNameInputLock [80] = { 0 };
	TCHAR szChar [80] = { 0 };
	TCHAR szMods [80] = { 0 };

_stprintf_s (szSettingNameChar, _T ("%sKey"), bindingName);
_stprintf_s (szSettingNameModifiers, _T ("%sModifiers"), bindingName);
_stprintf_s (szSettingNameInputLock, _T ("%sInputLock"), bindingName);

GetPrivateProfileString ("DLE.Bindings", szSettingNameChar, "", szChar, ARRAYSIZE (szChar), DLE.IniFile ());
GetPrivateProfileString ("DLE.Bindings", szSettingNameModifiers, "", szMods, ARRAYSIZE (szMods), DLE.IniFile ());
UINT nNeedsInputLock = GetPrivateProfileInt ("DLE.Bindings", szSettingNameInputLock, 0, DLE.IniFile ());

// Leave settings as default if not specified. Boolean values are currently always false by default
if (_tcslen (szChar) > 0)
	binding.nChar = StringToVK (szChar);
if (_tcslen (szMods) > 0)
	LoadModifiers (binding.modifiers, szMods);
binding.bNeedsInputLock = nNeedsInputLock > 0;
}

void CInputHandler::LoadStateConfig (MouseStateConfig &config, LPCTSTR bindingName)
{
	TCHAR szSettingNameButton [80] = { 0 };
	TCHAR szSettingNameModifiers [80] = { 0 };
	TCHAR szSettingNameToggleModifiers [80] = { 0 };
	TCHAR szSettingNameInvertX [80] = { 0 };
	TCHAR szSettingNameInvertY [80] = { 0 };
	TCHAR szButton [80] = { 0 };
	TCHAR szMods [80] = { 0 };

_stprintf_s (szSettingNameButton, _T ("%sButton"), bindingName);
_stprintf_s (szSettingNameModifiers, _T ("%sModifiers"), bindingName);
_stprintf_s (szSettingNameToggleModifiers, _T ("%sToggleModifiers"), bindingName);
_stprintf_s (szSettingNameInvertX, _T ("%sInvertX"), bindingName);
_stprintf_s (szSettingNameInvertY, _T ("%sInvertY"), bindingName);

GetPrivateProfileString ("DLE.Bindings", szSettingNameButton, "", szButton, ARRAYSIZE (szButton), DLE.IniFile ());
GetPrivateProfileString ("DLE.Bindings", szSettingNameModifiers, "", szMods, ARRAYSIZE (szMods), DLE.IniFile ());
UINT nToggleModifiers = GetPrivateProfileInt ("DLE.Bindings", szSettingNameToggleModifiers, 0, DLE.IniFile ());
UINT nInvertX = GetPrivateProfileInt ("DLE.Bindings", szSettingNameInvertX, 0, DLE.IniFile ());
UINT nInvertY = GetPrivateProfileInt ("DLE.Bindings", szSettingNameInvertY, 0, DLE.IniFile ());

// Leave settings as default if not specified. Boolean values are currently always false by default
if (_tcslen (szButton) > 0)
	config.button = StringToMK (szButton);
if (_tcslen (szMods) > 0)
	LoadModifiers (config.modifiers, szMods);
config.bToggleModifiers = nToggleModifiers > 0; // TODO Currently ignored. Need to figure out if it's needed; delete if not
config.bInvertX = nInvertX > 0;
config.bInvertY = nInvertY > 0;
}

void CInputHandler::LoadModifiers (bool (&modifierList) [eModifierCount], LPTSTR szMods)
{
	LPCTSTR delimiters = _T (",");
	LPTSTR context = NULL;
	LPTSTR pszNext = NULL;

for (unsigned int i = 0; i < eModifierCount; i++)
	modifierList [i] = false;

pszNext = _tcstok_s (szMods, delimiters, &context);
while (pszNext != NULL) {
	switch (StringToVK (pszNext)) {
		case VK_SHIFT:
			modifierList [eModifierShift] = true;
			break;
		case VK_CONTROL:
			modifierList [eModifierCtrl] = true;
			break;
		case VK_MENU:
			modifierList [eModifierAlt] = true;
			break;
		default:
			break;
		}
	pszNext = _tcstok_s (NULL, delimiters, &context);
	}
}

#define VK_TABLE_ENTRY(vk) \
	{ vk, _T (#vk) }
#define VK_TABLE_ENTRY_NUM(vk) \
	{ 0x30 + vk, _T (#vk) }
#define VK_TABLE_ENTRY_ALPHA(vk) \
	{ 0x41 + (#@vk - 'A'), _T (#vk) }
typedef struct {
	UINT vk;
	LPCTSTR szKey;
	} VKMapTableEntry;

UINT CInputHandler::StringToVK (LPCTSTR pszKey)
{
	static const VKMapTableEntry table [] = {
		VK_TABLE_ENTRY (VK_LBUTTON),
		VK_TABLE_ENTRY (VK_RBUTTON),
		VK_TABLE_ENTRY (VK_MBUTTON),
		VK_TABLE_ENTRY (VK_XBUTTON1),
		VK_TABLE_ENTRY (VK_XBUTTON2),
		VK_TABLE_ENTRY (VK_BACK),
		VK_TABLE_ENTRY (VK_TAB),
		VK_TABLE_ENTRY (VK_RETURN),
		VK_TABLE_ENTRY (VK_SHIFT),
		VK_TABLE_ENTRY (VK_CONTROL),
		VK_TABLE_ENTRY (VK_MENU),
		VK_TABLE_ENTRY (VK_ESCAPE),
		VK_TABLE_ENTRY (VK_SPACE),
		VK_TABLE_ENTRY (VK_PRIOR),
		VK_TABLE_ENTRY (VK_NEXT),
		VK_TABLE_ENTRY (VK_END),
		VK_TABLE_ENTRY (VK_HOME),
		VK_TABLE_ENTRY (VK_LEFT),
		VK_TABLE_ENTRY (VK_UP),
		VK_TABLE_ENTRY (VK_RIGHT),
		VK_TABLE_ENTRY (VK_DOWN),
		VK_TABLE_ENTRY (VK_INSERT),
		VK_TABLE_ENTRY (VK_DELETE),
		VK_TABLE_ENTRY_NUM (0),
		VK_TABLE_ENTRY_NUM (1),
		VK_TABLE_ENTRY_NUM (2),
		VK_TABLE_ENTRY_NUM (3),
		VK_TABLE_ENTRY_NUM (4),
		VK_TABLE_ENTRY_NUM (5),
		VK_TABLE_ENTRY_NUM (6),
		VK_TABLE_ENTRY_NUM (7),
		VK_TABLE_ENTRY_NUM (8),
		VK_TABLE_ENTRY_NUM (9),
		VK_TABLE_ENTRY_ALPHA (A),
		VK_TABLE_ENTRY_ALPHA (B),
		VK_TABLE_ENTRY_ALPHA (C),
		VK_TABLE_ENTRY_ALPHA (D),
		VK_TABLE_ENTRY_ALPHA (E),
		VK_TABLE_ENTRY_ALPHA (F),
		VK_TABLE_ENTRY_ALPHA (G),
		VK_TABLE_ENTRY_ALPHA (H),
		VK_TABLE_ENTRY_ALPHA (I),
		VK_TABLE_ENTRY_ALPHA (J),
		VK_TABLE_ENTRY_ALPHA (K),
		VK_TABLE_ENTRY_ALPHA (L),
		VK_TABLE_ENTRY_ALPHA (M),
		VK_TABLE_ENTRY_ALPHA (N),
		VK_TABLE_ENTRY_ALPHA (O),
		VK_TABLE_ENTRY_ALPHA (P),
		VK_TABLE_ENTRY_ALPHA (Q),
		VK_TABLE_ENTRY_ALPHA (R),
		VK_TABLE_ENTRY_ALPHA (S),
		VK_TABLE_ENTRY_ALPHA (T),
		VK_TABLE_ENTRY_ALPHA (U),
		VK_TABLE_ENTRY_ALPHA (V),
		VK_TABLE_ENTRY_ALPHA (W),
		VK_TABLE_ENTRY_ALPHA (X),
		VK_TABLE_ENTRY_ALPHA (Y),
		VK_TABLE_ENTRY_ALPHA (Z),
		VK_TABLE_ENTRY (VK_NUMPAD0),
		VK_TABLE_ENTRY (VK_NUMPAD1),
		VK_TABLE_ENTRY (VK_NUMPAD2),
		VK_TABLE_ENTRY (VK_NUMPAD3),
		VK_TABLE_ENTRY (VK_NUMPAD4),
		VK_TABLE_ENTRY (VK_NUMPAD5),
		VK_TABLE_ENTRY (VK_NUMPAD6),
		VK_TABLE_ENTRY (VK_NUMPAD7),
		VK_TABLE_ENTRY (VK_NUMPAD8),
		VK_TABLE_ENTRY (VK_NUMPAD9),
		VK_TABLE_ENTRY (VK_MULTIPLY),
		VK_TABLE_ENTRY (VK_ADD),
		VK_TABLE_ENTRY (VK_SUBTRACT),
		VK_TABLE_ENTRY (VK_DECIMAL),
		VK_TABLE_ENTRY (VK_DIVIDE)
		};

for (size_t i = 0; i < ARRAYSIZE (table); i++)
	if (_tcsicmp (pszKey, table [i].szKey) == 0)
		return table [i].vk;
return 0;
}

#define MK_TABLE_ENTRY(mk) \
	{ MK_##mk, _T (#mk) }
typedef struct {
	UINT mk;
	LPCTSTR szButton;
	} MKMapTableEntry;

UINT CInputHandler::StringToMK (LPCTSTR pszButton)
{
	static const MKMapTableEntry table [] = {
		MK_TABLE_ENTRY (LBUTTON),
		MK_TABLE_ENTRY (MBUTTON),
		MK_TABLE_ENTRY (RBUTTON),
		MK_TABLE_ENTRY (XBUTTON1),
		MK_TABLE_ENTRY (XBUTTON2)
		};

for (size_t i = 0; i < ARRAYSIZE (table); i++)
	if (_tcsicmp (pszButton, table [i].szButton) == 0)
		return table [i].mk;
return 0;
}

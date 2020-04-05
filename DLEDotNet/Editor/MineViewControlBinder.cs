using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    using ControlBindAction = Action;
    using MouseStateDecider = Func<BindModifiers, MouseState, MouseState>;
    public delegate void MouseStateEventHandler(object sender, MouseStateEventArgs e);

    public class MineViewControlBinder
    {
        private event MouseStateEventHandler MouseStateChanged;

        #region --- implementation
        private EditorWindow editorWindow;
        private MultiDict<Keys, ValueTuple<BindModifiers, ControlBindAction>> keyBinds;
        private MultiDict<MouseActions, ValueTuple<BindModifiers, ControlBindAction>> mouseBinds;
        private List<ValueTuple<BindModifiers, MouseState>> mouseStateMapper;
        private List<ValueTuple<BindModifiers, MouseStateDecider>> mouseStateMapperDeciders;
        private MouseState currentState = MouseState.Idle;

        public MineViewControlBinder(EditorWindow owner)
        {
            editorWindow = owner;
            keyBinds = new MultiDict<Keys, ValueTuple<BindModifiers, ControlBindAction>>();
            mouseBinds = new MultiDict<MouseActions, ValueTuple<BindModifiers, ControlBindAction>>();
            mouseStateMapper = new List<ValueTuple<BindModifiers, MouseState>>();
            mouseStateMapperDeciders = new List<ValueTuple<BindModifiers, MouseStateDecider>>();
        }
        #endregion

        #region --- handling events
        private void GotKeyDown(object sender, KeyEventArgs e)
        {
            BindModifiers mod = GetCurrentBindModifiers();
            foreach (ValueTuple<BindModifiers, ControlBindAction> keyBind in keyBinds.GetOrEmpty(e.KeyCode))
                if (0 == (keyBind.Item1 & mod))
                    keyBind.Item2();
            UpdateMouseState();
        }

        private void GotMouseClick(Object sender, MouseEventArgs e)
        {
            BindModifiers mod = GetCurrentBindModifiers();
            MouseActions action;
            switch (e.Button)
            {
                case MouseButtons.Left:
                    action = MouseActions.LeftClick;
                    break;
                case MouseButtons.Right:
                    action = MouseActions.RightClick;
                    break;
                case MouseButtons.Middle:
                    action = MouseActions.MiddleClick;
                    break;
                default:
                    return;
            }

            foreach (var mouseBind in mouseBinds.GetOrEmpty(action))
                if (0 == (mouseBind.Item1 & mod))
                    for (int i = 0; i < e.Clicks; ++i)
                        mouseBind.Item2();
        }

        private void GotMouseWheel(Object sender, MouseEventArgs e)
        {
            BindModifiers mod = GetCurrentBindModifiers();
            MouseActions action;
            int wheelTicks = e.Delta / SystemInformation.MouseWheelScrollDelta;
            if (wheelTicks == 0)
                return;
            else
                action = wheelTicks > 0 ? MouseActions.ScrollUp : MouseActions.ScrollDown;
            wheelTicks = wheelTicks < 0 ? -wheelTicks : wheelTicks;

            foreach (var mouseBind in mouseBinds.GetOrEmpty(action))
                if (0 == (mouseBind.Item1 & mod))
                    for (int i = 0; i < wheelTicks; ++i)
                        mouseBind.Item2();
        }

        private void UpdateMouseStateFromEvent(object sender, EventArgs e)
            => UpdateMouseState();

        private void ResetMouseStateFromEvent(object sender, EventArgs e)
            => UpdateMouseState(MouseState.Idle);

        private bool IsKeyModifierDown(Keys modifier)
            => (Control.ModifierKeys & modifier) == modifier;
        private bool IsMouseButtonDown(MouseButtons button)
            => (Control.MouseButtons & button) == button;

        private BindModifiers GetCurrentBindModifiers()
        {
            BindModifiers bm = BindModifiers.NullMask;
            // we are zeroing out values, since we will AND these with required modifiers
            // and consider it a match if the result is zero
            bm &= ~(IsKeyModifierDown(Keys.Control) ? BindModifiers.CtrlHeld : BindModifiers.CtrlNotHeld)
                & ~(IsKeyModifierDown(Keys.Shift) ? BindModifiers.ShiftHeld : BindModifiers.ShiftNotHeld)
                & ~(IsKeyModifierDown(Keys.Alt) ? BindModifiers.AltHeld : BindModifiers.AltNotHeld)
                & ~(IsMouseButtonDown(MouseButtons.Left) ? BindModifiers.MouseLeftHeld : BindModifiers.MouseLeftNotHeld)
                & ~(IsMouseButtonDown(MouseButtons.Right) ? BindModifiers.MouseRightHeld : BindModifiers.MouseRightNotHeld)
                & ~(IsMouseButtonDown(MouseButtons.Middle) ? BindModifiers.MouseMiddleHeld : BindModifiers.MouseMiddleNotHeld)
                & ~(IsMouseButtonDown(MouseButtons.XButton1) ? BindModifiers.MouseXButton1Held : BindModifiers.MouseXButton1NotHeld)
                & ~(IsMouseButtonDown(MouseButtons.XButton2) ? BindModifiers.MouseXButton2Held : BindModifiers.MouseXButton2NotHeld);
            return bm;
        }

        private MouseState GetNewMouseState()
        {
            BindModifiers mod = GetCurrentBindModifiers();
            foreach (var stateDecider in mouseStateMapper)
                if (0 == (stateDecider.Item1 & mod))
                    return stateDecider.Item2;
            foreach (var stateDecider in mouseStateMapperDeciders)
                if (0 == (stateDecider.Item1 & mod))
                    return stateDecider.Item2(mod, currentState);
            return MouseState.Idle;
        }

        private void UpdateMouseState() => UpdateMouseState(GetNewMouseState());
        private void UpdateMouseState(MouseState newMouseState)
        {
            if (currentState != newMouseState)
            {
                MouseStateEventArgs e = new MouseStateEventArgs(newMouseState, currentState, GetCurrentBindModifiers());
                this.MouseStateChanged?.Invoke(this, e);
                if (!e.Cancel) currentState = newMouseState;
            }
        }

        internal void AddEventHandlers(MineView mineView)
        {
            mineView.KeyDown += GotKeyDown;
            mineView.KeyUp += UpdateMouseStateFromEvent;
            mineView.MouseClick += GotMouseClick;
            mineView.MouseWheel += GotMouseWheel;
            mineView.MouseDown += UpdateMouseStateFromEvent;
            mineView.MouseEnter += UpdateMouseStateFromEvent;
            mineView.MouseLeave += ResetMouseStateFromEvent;
            mineView.MouseUp += UpdateMouseStateFromEvent;
            this.MouseStateChanged += mineView.OnMouseStateChanged;
        }

        internal void RemoveEventHandlers(MineView mineView)
        {
            mineView.KeyDown -= GotKeyDown;
            mineView.KeyUp += UpdateMouseStateFromEvent;
            mineView.MouseClick -= GotMouseClick;
            mineView.MouseWheel -= GotMouseWheel;
            mineView.MouseDown -= UpdateMouseStateFromEvent;
            mineView.MouseEnter -= UpdateMouseStateFromEvent;
            mineView.MouseLeave -= ResetMouseStateFromEvent;
            mineView.MouseUp -= UpdateMouseStateFromEvent;
            this.MouseStateChanged -= mineView.OnMouseStateChanged;
        }
        #endregion

        #region --- MineView wrappers
        private void ShowMineViewContextMenu() => editorWindow.MineView?.ShowContextMenu();
        private MouseState GetQuickSelectMouseState(BindModifiers arg, MouseState previous) => editorWindow.MineView.GetQuickSelectMouseState(arg, previous);
        private MouseState GetLeftDownMouseState(BindModifiers arg, MouseState previous) => editorWindow.MineView.GetLeftDownMouseState(arg, previous);
        private void MineViewZoomIn() => editorWindow.MineView?.ZoomIn();
        private void MineViewZoomOut() => editorWindow.MineView?.ZoomOut();
        #endregion

        #region --- interface
        /// <summary>
        /// Gets the currently active mouse state.
        /// </summary>
        /// <returns>The currently active mouse state.</returns>
        public MouseState GetMouseState() => currentState;

        internal void ClearBinds()
        {
            keyBinds.Clear();
            mouseBinds.Clear();
            mouseStateMapper.Clear();
        }

        /// <summary>
        /// Adds a new key bind.
        /// </summary>
        /// <param name="key">The key to listen to.</param>
        /// <param name="modifier">The modifiers that must match.</param>
        /// <param name="action">The action to be carried out.</param>
        private void AddKeybind(ControlBindAction action, Keys key, BindModifiers modifier)
            => keyBinds.Add(key, (modifier, action));

        /// <summary>
        /// Adds a new mouse bind.
        /// </summary>
        /// <param name="mouse">The mouse action to listen to.</param>
        /// <param name="modifier">The modifiers that must match.</param>
        /// <param name="action">The action to be carried out.</param>
        private void AddMousebind(ControlBindAction action, MouseActions mouse, BindModifiers modifier)
            => mouseBinds.Add(mouse, (modifier, action));

        /// <summary>
        /// Adds a mouse state decider; when the modifiers apply, the state will be applied.
        /// </summary>
        /// <param name="state">The mouse state to pick if the modifiers match.</param>
        /// <param name="modifier">The modifiers to react to.</param>
        private void AddMouseStateMapping(MouseState state, BindModifiers modifier)
            => mouseStateMapper.Add((modifier, state));

        /// <summary>
        /// Adds a mouse state decider mapping; when the modifiers apply, ask the decider function
        /// to select the new mouse state.
        /// </summary>
        /// <param name="decider">The decider function to use; it must take a BindModifiers corresponding
        /// to the currently active modifier state and a MouseState corresponding to the previous state,
        /// and use those values to return the appropriate MouseState.</param>
        /// <param name="modifier">The modifiers to react to.</param>
        private void AddMouseStateMapping(MouseStateDecider decider, BindModifiers modifier)
            => mouseStateMapperDeciders.Add((modifier, decider));

        #endregion

        #region --- defaults
        internal void AddDefaults()
        {
            AddMousebind(ShowMineViewContextMenu, MouseActions.RightClick, BindModifiers.OnlyShift);
            AddMousebind(MineViewZoomIn, MouseActions.ScrollUp, BindModifiers.Any);
            AddMousebind(MineViewZoomOut, MouseActions.ScrollDown, BindModifiers.Any);

            AddMouseStateMapping(GetQuickSelectMouseState, BindModifiers.OnlyShift | BindModifiers.MouseButtonsNotHeld);
            AddMouseStateMapping(MouseState.Pan, BindModifiers.OnlyCtrl | BindModifiers.MouseButtonsNotHeld);
            AddMouseStateMapping(MouseState.Rotate, BindModifiers.CtrlAndShift | BindModifiers.MouseButtonsNotHeld);
            AddMouseStateMapping(MouseState.ZoomUpDown, BindModifiers.OnlyCtrl | BindModifiers.OnlyMouseLeft);
            AddMouseStateMapping(MouseState.ZoomUpDown, BindModifiers.OnlyCtrl | BindModifiers.OnlyMouseRight);
            AddMouseStateMapping(GetLeftDownMouseState, BindModifiers.KeysNotHeld | BindModifiers.OnlyMouseLeft);
            AddMouseStateMapping(MouseState.RubberbandUnTag, BindModifiers.OnlyShift | BindModifiers.OnlyMouseLeft);
        }

        internal void AddOverload() // Overload Level Editor mode, example
        {
            AddMousebind(ShowMineViewContextMenu, MouseActions.RightClick, BindModifiers.OnlyShift);
            AddMousebind(MineViewZoomIn, MouseActions.ScrollUp, BindModifiers.CtrlNotHeld);
            AddMousebind(MineViewZoomOut, MouseActions.ScrollDown, BindModifiers.CtrlNotHeld);
            // CTRL mouse-wheel = FOV

            AddMouseStateMapping(GetQuickSelectMouseState, BindModifiers.OnlyShift | BindModifiers.MouseButtonsNotHeld);
            AddMouseStateMapping(MouseState.Pan, BindModifiers.MouseMiddleHeld);
            AddMouseStateMapping(MouseState.Rotate, BindModifiers.OnlyAlt | BindModifiers.OnlyMouseLeft);
            AddMouseStateMapping(MouseState.Rotate, BindModifiers.MouseMiddleHeld | BindModifiers.MouseRightHeld | BindModifiers.MouseLeftNotHeld);
            AddMouseStateMapping(MouseState.RubberbandTag, BindModifiers.AltNotHeld | BindModifiers.OnlyMouseLeft);
            AddMouseStateMapping(MouseState.MarkedDrag, BindModifiers.OnlyMouseRight | BindModifiers.CtrlNotHeld | BindModifiers.AltNotHeld);
            AddMouseStateMapping(MouseState.MarkedDragRotate, BindModifiers.OnlyMouseRight | BindModifiers.OnlyAlt);
            AddMouseStateMapping(MouseState.MarkedDragScale, BindModifiers.OnlyMouseRight | BindModifiers.OnlyCtrl);
        }
        #endregion
    }
    
    public class MouseStateEventArgs : CancelEventArgs
    {
        public MouseState OldState;
        public MouseState NewState;
        public BindModifiers Modifiers;

        public MouseStateEventArgs(MouseState newState, MouseState oldState, BindModifiers modifiers)
        {
            NewState = newState;
            OldState = oldState;
            Modifiers = modifiers;
        }
    }

    internal enum MouseActions
    {
        LeftClick = 1,
        RightClick = 2,
        MiddleClick = 3,
        ScrollUp = 4,
        ScrollDown = 5
    }

    /// <summary>
    /// A set of modifiers that can be used to filter against key modifiers (Ctrl, Shift, Alt)
    /// and mouse buttons (if left button down, etc.)
    /// </summary>
    public enum BindModifiers
    {
        ShiftHeld = 1 << 0,
        CtrlHeld = 1 << 1,
        AltHeld = 1 << 2,

        ShiftNotHeld = 1 << 8,
        CtrlNotHeld = 1 << 9,
        AltNotHeld = 1 << 10,

        MouseLeftHeld = 1 << 16,
        MouseRightHeld = 1 << 17,
        MouseMiddleHeld = 1 << 18,
        MouseXButton1Held = 1 << 19,
        MouseXButton2Held = 1 << 20,

        MouseLeftNotHeld = 1 << 24,
        MouseRightNotHeld = 1 << 25,
        MouseMiddleNotHeld = 1 << 26,
        MouseXButton1NotHeld = 1 << 27,
        MouseXButton2NotHeld = 1 << 28,

        OnlyCtrl = CtrlHeld | ShiftNotHeld | AltNotHeld,
        OnlyShift = CtrlNotHeld | ShiftHeld | AltNotHeld,
        CtrlAndShift = CtrlHeld | ShiftHeld | AltNotHeld,
        OnlyAlt = CtrlNotHeld | ShiftNotHeld | AltHeld,

        OnlyMouseLeft = MouseLeftHeld | MouseRightNotHeld | MouseMiddleNotHeld,
        OnlyMouseRight = MouseLeftNotHeld | MouseRightHeld | MouseMiddleNotHeld,
        OnlyMouseMiddle = MouseLeftNotHeld | MouseRightNotHeld | MouseMiddleHeld,

        /// <summary>
        /// If no modifier key should be held down.
        /// </summary>
        KeysNotHeld = ShiftNotHeld | CtrlNotHeld | AltNotHeld,

        /// <summary>
        /// If no mouse button should be held down.
        /// </summary>
        MouseButtonsNotHeld = MouseLeftNotHeld | MouseRightNotHeld | MouseMiddleNotHeld,

        /// <summary>
        /// If the modifiers do not matter.
        /// </summary>
        Any = 0,

        /// <summary>
        /// This value is used internally. Do not use it.
        /// </summary>
        // OR of *everything* above!
        NullMask = ShiftHeld | CtrlHeld | AltHeld
            | ShiftNotHeld | CtrlNotHeld | AltNotHeld
            | MouseLeftHeld | MouseRightHeld | MouseMiddleHeld
            | MouseXButton1Held | MouseXButton2Held
            | MouseLeftNotHeld | MouseRightNotHeld | MouseMiddleNotHeld
            | MouseXButton1NotHeld | MouseXButton2NotHeld
    }


    static class BindModifiersMethods
    {
        public static bool Applies(this BindModifiers m, BindModifiers flag)
        {
            return 0 == (m & flag);
        }
    }


    public enum MouseState
    {
        Idle,
        QuickSelect,
        PointDrag,
        QuickTag,
        RubberbandTag,
        RubberbandUnTag,
        Pan,
        Rotate,
        LockedRotate,
        ZoomUpDown,
        PanUpDown,
        MarkedDrag,
        MarkedDragRotate,
        MarkedDragScale
    }
}

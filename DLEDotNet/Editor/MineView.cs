using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Util;
using DLEDotNet.Data;
using DLEDotNet.ManagedWrappers;
using DLEDotNet.Data.Proxies;

namespace DLEDotNet.Editor
{
    public partial class MineView : UserControl
    {
        private MouseState mouseState = MouseState.Idle;
        private MineViewPresenter presenter;

        public MineView()
        {
            InitializeComponent();
            presenter = new MineViewPresenter(Handle);
            presenter.ResetZoom();
        }

        #region --- click-through
        private const uint WM_MOUSEACTIVATE = 0x21;
        private const uint MA_ACTIVATE = 1;
        private const uint MA_ACTIVATEANDEAT = 2;

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);

            if (m.Msg == WM_MOUSEACTIVATE && m.Result == (IntPtr)MA_ACTIVATEANDEAT)
                m.Result = (IntPtr)MA_ACTIVATE;
        }
        #endregion

        internal EditorWindow Owner { get; set; }
        internal EditorState EditorState => Owner?.EditorState;

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            // custom rendering
            presenter.UpdateCurrentSelection(new SelectionProxy(EditorState.CurrentSelection));
            presenter.UpdateOtherSelection(new SelectionProxy(EditorState.OtherSelection));
            presenter.UpdateSelectionMode((uint)EditorState.SelectionMode);
            presenter.UpdateViewMode((uint)EditorState.ViewMode);
            presenter.UpdateDepthPerception((uint)EditorState.Prefs.DepthPerception);
            presenter.Paint();
        }

        internal void ShowContextMenu()
        {
            Owner.mineViewContextMenuStrip.Show(MousePosition);
        }

        internal void OnMouseStateChanged(object sender, MouseStateEventArgs e)
        {
            if (!e.Cancel)
            {
                mouseState = e.NewState;
                // TODO
                System.Diagnostics.Debug.WriteLine("MineView.MouseState = " + mouseState);
            }
        }

        internal MouseState GetQuickSelectMouseState(BindModifiers arg, MouseState previous)
        {
            // depending on EditorState...?
            switch (EditorState.SelectionMode)
            {
                case SelectMode.Object:
                case SelectMode.Block:
                    return previous; // do not enter quickselect
            }
            return MouseState.QuickSelect;
        }

        internal MouseState GetLeftDownMouseState(BindModifiers arg, MouseState previous)
        {
            bool closeEnoughToPoint = true; // TODO
            return closeEnoughToPoint ? MouseState.PointDrag : MouseState.RubberbandTag;
        }

        internal void ZoomIn()
        {
            // TODO
            System.Diagnostics.Debug.WriteLine("MineView.ZoomIn");
        }

        internal void ZoomOut()
        {
            // TODO
            System.Diagnostics.Debug.WriteLine("MineView.ZoomOut");
        }

        internal void ResetZoom()
        {
            presenter.ResetZoom();
        }
    }
}

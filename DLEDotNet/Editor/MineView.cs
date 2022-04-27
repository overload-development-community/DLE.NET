﻿using System;
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
                switch (mouseState)
                {
                    case MouseState.Idle:
                        break;
                    case MouseState.QuickSelect:
                        break;
                    case MouseState.PointDrag:
                        break;
                    case MouseState.QuickTag:
                        break;
                    case MouseState.RubberbandTag:
                        break;
                    case MouseState.RubberbandUnTag:
                        break;
                    case MouseState.RubberbandTagSet:
                        break;
                    case MouseState.Pan:
                        break;
                    case MouseState.Rotate:
                        break;
                    case MouseState.LockedRotate:
                        break;
                    case MouseState.ZoomUpDown:
                        break;
                    case MouseState.PanUpDown:
                        break;
                    case MouseState.MarkedDrag:
                        break;
                    case MouseState.MarkedDragRotate:
                        break;
                    case MouseState.MarkedDragScale:
                        break;
                    default:
                        break;
                }
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
            System.Diagnostics.Debug.WriteLine("MineView.ZoomIn");
            presenter.ZoomIn();
            presenter.Paint();
        }

        internal void ZoomOut()
        {
            System.Diagnostics.Debug.WriteLine("MineView.ZoomOut");
            presenter.ZoomOut();
            presenter.Paint();
        }

        internal void ResetZoom()
        {
            presenter.ResetZoom();
            presenter.Paint();
        }

        internal void CenterOnCurrentCube()
        {
            presenter.CenterCurrentSegment();
            presenter.Paint();
        }

        internal void CenterOnCurrentObject()
        {
            presenter.CenterCurrentObject();
            presenter.Paint();
        }
    }
}

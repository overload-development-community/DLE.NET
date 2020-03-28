using DLEDotNet.Data;
using DLEDotNet.Dialogs;
using DLEDotNet.Editor;
using DLEDotNet.Editor.Layouts;
using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;
using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static DLEDotNet.Editor.DLEUserControl;

namespace DLEDotNet
{
    public partial class EditorWindow : Form
    {
        /**
         *  TODO:
         *    - context menu
         *    - rewrite key event system (don't use ShortcutKeys, for one)
         *    - proxy stuff from LibDescent
         *    - rest of the settings
         *    - import settings from DLE.ini
         * 
         */
        private LayoutOrientation _activeLayout;
        private MainView _activeMainView = null;
        private MineView mineView = new MineView();
        private TextureList textureList = new TextureList();
        private EditorTools editorTabs;
        private EditorToolDialog editorTools = new EditorToolDialog();
        public EditorState EditorState { get; }

        public EditorWindow()
        {
            InitializeComponent();
            EditorState = new EditorState(this);
            editorTabs = new EditorTools(this);
        }

        #region --- Program load & unload
        private void EditorWindow_Load(object sender, EventArgs e)
        {
#if DEBUG
            editorTabs.SelfTest(); // must be called before assigning layout!!!
            // new TestForm() { EditorWindow = this, EditorState = this.EditorState }.Show(this);
#endif
            EditorState.SavedPrefs.ReloadFromFile();
            EditorState.Prefs.CopyFrom(EditorState.SavedPrefs);
            // a layout MUST be set on load.
            ActiveLayout = EditorState.SavedPrefs.ActiveLayout;
            WindowState = ControlUtil.ConvertToFormWindowState(EditorState.SavedPrefs.StartupState);
            EditorState.SettingChanged += EditorState_SettingChanged;

            InitializeEvents();
            SetupToolbar();
        }

        private void EditorWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            EditorState.SavedPrefs.SaveToFile();
        }
        #endregion

        #region --- Layout handling code
        internal LayoutOrientation ActiveLayout
        {
            get
            {
                return _activeLayout;
            }
            set
            {
                _activeLayout = value;
                UpdateLayoutMainView();
            }
        }

        private MainView ActiveMainView
        {
            get
            {
                return _activeMainView;
            }
            set
            {
                OnMainViewUpdate(value, _activeMainView);
                _activeMainView = value;
            }
        }

        private void EditorState_SettingChanged(object sender, PropertyChangeEventArgs e)
        {
            if (e.PropertyName == "ActiveLayout")
                this.ActiveLayout = e.NewValue;
        }

        private void UpdateLayoutMainView()
        {
            MainView newMainView;

            if (_activeMainView != null)
            {
                // we need to dispose of the old main view
#if DEBUG
                System.Diagnostics.Debug.Assert(mainPanel.Controls.Count == 1 && mainPanel.Controls[0] == _activeMainView);
#endif
                DisposeMainView(_activeMainView);
                mainPanel.Controls.Clear();
            }

            switch (_activeLayout)
            {
                case LayoutOrientation.VERTICAL:
                    newMainView = new MainViewVertical();
                    break;
                case LayoutOrientation.VERTICAL_SS:
                    newMainView = new MainViewVerticalSS();
                    break;
                case LayoutOrientation.FLOATING:
                    newMainView = new MainViewFloating();
                    break;
                case LayoutOrientation.HORIZONTAL:
                default:
                    newMainView = new MainViewHorizontal();
                    break;
            }

            newMainView.CreateControl();
            mainPanel.Controls.Add(newMainView);
            newMainView.Dock = DockStyle.Fill;
            newMainView.Size = mainPanel.ClientSize;

            EditorTabContainer editorTabContainer = newMainView.GetEditorTabs();
            if (editorTabContainer == null)
            {
                editorTabs.AttachToFloating(_activeLayout);
            }
            else
            {
                EditorStateBinder.FromState(EditorState).BindTabControlBacking(editorTabContainer.TabControl, PROP(s => s.ActiveEditorTab));
                editorTabs.AttachToContainer(_activeLayout, editorTabContainer);
            }

            if (_activeMainView != null)
            {
                _activeMainView.Dispose();
            }
            mineView = newMainView.GetMineView();
            textureList = newMainView.GetTextureList();
            ActiveMainView = newMainView;
            mainPanel.PerformLayout();
        }

        private void DisposeMainView(MainView mainView)
        {
            editorTabs.Detach();
            mainView.Dispose(); // dispose whatever remains, which should be everything else
        }

        private void OnMainViewUpdate(MainView newMainView, MainView oldMainView)
        {
            UpdateViews();
            GC.WaitForPendingFinalizers();
            GC.Collect();
        }
        #endregion

        #region --- Tool strip handling (icons, binding...)
        private void SetupToolbarIcons()
        {
            if (ControlUtil.IsDarkMode())
            {
                this.openToolStripButton.Image = Properties.Resources.toolbarOpenFileDark;
                this.saveToolStripButton.Image = Properties.Resources.toolbarSaveFileDark;
                this.checkMineToolStripButton.Image = Properties.Resources.toolbarCheckMineDark;
                this.lightToolToolStripButton.Image = Properties.Resources.toolbarLightToolDark;
                this.textureToolToolStripButton.Image = Properties.Resources.toolbarTextureToolDark;
                this.segmentToolToolStripButton.Image = Properties.Resources.toolbarSegmentToolDark;
                this.wallToolToolStripButton.Image = Properties.Resources.toolbarWallToolDark;
                this.objectToolToolStripButton.Image = Properties.Resources.toolbarObjectToolDark;
                this.preferencesToolStripButton.Image = Properties.Resources.toolbarPrefsToolDark;
                this.zoomInToolStripButton.Image = Properties.Resources.toolbarZoomInDark;
                this.zoomOutToolStripButton.Image = Properties.Resources.toolbarZoomOutDark;
                this.fitMineToolStripButton.Image = Properties.Resources.toolbarFitMineDark;
                this.panLeftToolStripButton.Image = Properties.Resources.toolbarPanLeftDark;
                this.panRightToolStripButton.Image = Properties.Resources.toolbarPanRightDark;
                this.panUpToolStripButton.Image = Properties.Resources.toolbarPanUpDark;
                this.panDownToolStripButton.Image = Properties.Resources.toolbarPanDownDark;
                this.rotateXMToolStripButton.Image = Properties.Resources.toolbarRotateXMDark;
                this.rotateXPToolStripButton.Image = Properties.Resources.toolbarRotateXPDark;
                this.rotateYMToolStripButton.Image = Properties.Resources.toolbarRotateYMDark;
                this.rotateYPToolStripButton.Image = Properties.Resources.toolbarRotateYPDark;
                this.rotateZMToolStripButton.Image = Properties.Resources.toolbarRotateZMDark;
                this.rotateZPToolStripButton.Image = Properties.Resources.toolbarRotateZPDark;
                this.joinPointsToolStripButton.Image = Properties.Resources.toolbarJoinPointsDark;
                this.splitPointsToolStripButton.Image = Properties.Resources.toolbarSplitPointsDark;
                this.joinLinesToolStripButton.Image = Properties.Resources.toolbarJoinLinesDark;
                this.splitLinesToolStripButton.Image = Properties.Resources.toolbarSplitLinesDark;
                this.joinSidesToolStripButton.Image = Properties.Resources.toolbarJoinSidesDark;
                this.splitSidesToolStripButton.Image = Properties.Resources.toolbarSplitSidesDark;
                this.addNormalToolStripButton.Image = Properties.Resources.toolbarAddNormalDark;
                this.addExtendedToolStripButton.Image = Properties.Resources.toolbarAddExtendedDark;
                this.addMirroredToolStripButton.Image = Properties.Resources.toolbarAddMirroredDark;
                this.pointModeToolStripButton.Image = Properties.Resources.toolbarPointModeDark;
                this.lineModeToolStripButton.Image = Properties.Resources.toolbarLineModeDark;
                this.sideModeToolStripButton.Image = Properties.Resources.toolbarSideModeDark;
                this.segmentModeToolStripButton.Image = Properties.Resources.toolbarSegmentModeDark;
                this.objectModeToolStripButton.Image = Properties.Resources.toolbarObjectModeDark;
                this.blockModeToolStripButton.Image = Properties.Resources.toolbarBlockModeDark;
                this.redrawMineToolStripButton.Image = Properties.Resources.toolbarRedrawMineDark;
                this.fullScreenToolStripButton.Image = Properties.Resources.toolbarFullScreenDark;
            }
            else
            {
                this.openToolStripButton.Image = Properties.Resources.toolbarOpenFile;
                this.saveToolStripButton.Image = Properties.Resources.toolbarSaveFile;
                this.checkMineToolStripButton.Image = Properties.Resources.toolbarCheckMine;
                this.lightToolToolStripButton.Image = Properties.Resources.toolbarLightTool;
                this.textureToolToolStripButton.Image = Properties.Resources.toolbarTextureTool;
                this.segmentToolToolStripButton.Image = Properties.Resources.toolbarSegmentTool;
                this.wallToolToolStripButton.Image = Properties.Resources.toolbarWallTool;
                this.objectToolToolStripButton.Image = Properties.Resources.toolbarObjectTool;
                this.preferencesToolStripButton.Image = Properties.Resources.toolbarPrefsTool;
                this.zoomInToolStripButton.Image = Properties.Resources.toolbarZoomIn;
                this.zoomOutToolStripButton.Image = Properties.Resources.toolbarZoomOut;
                this.fitMineToolStripButton.Image = Properties.Resources.toolbarFitMine;
                this.panLeftToolStripButton.Image = Properties.Resources.toolbarPanLeft;
                this.panRightToolStripButton.Image = Properties.Resources.toolbarPanRight;
                this.panUpToolStripButton.Image = Properties.Resources.toolbarPanUp;
                this.panDownToolStripButton.Image = Properties.Resources.toolbarPanDown;
                this.rotateXMToolStripButton.Image = Properties.Resources.toolbarRotateXM;
                this.rotateXPToolStripButton.Image = Properties.Resources.toolbarRotateXP;
                this.rotateYMToolStripButton.Image = Properties.Resources.toolbarRotateYM;
                this.rotateYPToolStripButton.Image = Properties.Resources.toolbarRotateYP;
                this.rotateZMToolStripButton.Image = Properties.Resources.toolbarRotateZM;
                this.rotateZPToolStripButton.Image = Properties.Resources.toolbarRotateZP;
                this.joinPointsToolStripButton.Image = Properties.Resources.toolbarJoinPoints;
                this.splitPointsToolStripButton.Image = Properties.Resources.toolbarSplitPoints;
                this.joinLinesToolStripButton.Image = Properties.Resources.toolbarJoinLines;
                this.splitLinesToolStripButton.Image = Properties.Resources.toolbarSplitLines;
                this.joinSidesToolStripButton.Image = Properties.Resources.toolbarJoinSides;
                this.splitSidesToolStripButton.Image = Properties.Resources.toolbarSplitSides;
                this.addNormalToolStripButton.Image = Properties.Resources.toolbarAddNormal;
                this.addExtendedToolStripButton.Image = Properties.Resources.toolbarAddExtended;
                this.addMirroredToolStripButton.Image = Properties.Resources.toolbarAddMirrored;
                this.pointModeToolStripButton.Image = Properties.Resources.toolbarPointMode;
                this.lineModeToolStripButton.Image = Properties.Resources.toolbarLineMode;
                this.sideModeToolStripButton.Image = Properties.Resources.toolbarSideMode;
                this.segmentModeToolStripButton.Image = Properties.Resources.toolbarSegmentMode;
                this.objectModeToolStripButton.Image = Properties.Resources.toolbarObjectMode;
                this.blockModeToolStripButton.Image = Properties.Resources.toolbarBlockMode;
                this.redrawMineToolStripButton.Image = Properties.Resources.toolbarRedrawMine;
                this.fullScreenToolStripButton.Image = Properties.Resources.toolbarFullScreen;
            }
        }

        private void SetupToolbar()
        {
            SetupToolbarIcons();
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            
            binder.BindToolStripButtonAsRadioButton(this.addNormalToolStripButton, PROP(s => s.SegmentAddMode), SegmentAddMode.Normal, false);
            binder.BindToolStripButtonAsRadioButton(this.addExtendedToolStripButton, PROP(s => s.SegmentAddMode), SegmentAddMode.Extended, false);
            binder.BindToolStripButtonAsRadioButton(this.addMirroredToolStripButton, PROP(s => s.SegmentAddMode), SegmentAddMode.Mirrored, false);

            binder.BindToolStripButtonAsRadioButton(this.pointModeToolStripButton, PROP(s => s.SelectionMode), SelectMode.Point, false);
            binder.BindToolStripButtonAsRadioButton(this.lineModeToolStripButton, PROP(s => s.SelectionMode), SelectMode.Line, false);
            binder.BindToolStripButtonAsRadioButton(this.sideModeToolStripButton, PROP(s => s.SelectionMode), SelectMode.Side, false);
            binder.BindToolStripButtonAsRadioButton(this.segmentModeToolStripButton, PROP(s => s.SelectionMode), SelectMode.Segment, false);
            binder.BindToolStripButtonAsRadioButton(this.objectModeToolStripButton, PROP(s => s.SelectionMode), SelectMode.Object, false);
            binder.BindToolStripButtonAsRadioButton(this.blockModeToolStripButton, PROP(s => s.SelectionMode), SelectMode.Block, false);
        }

        private void mainToolStrip_BackColorChanged(object sender, EventArgs e)
        {
            SetupToolbarIcons();
        }
        #endregion

        #region --- Event handling setup
        private void InitializeEvents()
        {
            this.editorTools.Shown += (object sender, EventArgs e) => { this.editorToolBarToolStripMenuItem.CheckState = CheckState.Checked; };
            this.editorTools.VisibleChanged += (object sender, EventArgs e) => { this.editorToolBarToolStripMenuItem.CheckState = editorTools.Visible ? CheckState.Checked : CheckState.Unchecked; };
            this.mainToolStrip.VisibleChanged += (object sender, EventArgs e) => { this.toolbarToolStripMenuItem.CheckState = mainToolStrip.Visible ? CheckState.Checked : CheckState.Unchecked; };
            this.statusStrip.VisibleChanged += (object sender, EventArgs e) => { this.statusBarToolStripMenuItem.CheckState = statusStrip.Visible ? CheckState.Checked : CheckState.Unchecked; };
        }
        #endregion

        #region --- View update code
        private void UpdateViews()
        {
            UpdateMineView();
            UpdateTextureList();
        }

        private void UpdateMineView()
        {
            // ...
        }

        private void UpdateTextureList()
        {
            // ...
            // updating TextureCount would be a good idea to cause the scroll bar to be updated
            textureList.TextureCount = textureList.TextureCount;
        }
        #endregion

        // Most of the event listeners below should be as few lines as possible;
        // it's ideal to place logic on other classes
        // the main menu is the "canonical" event handler; thus tool strip
        // and key event handlers just point back there for most of them

        #region --- Handling key events (those not handled by ShortcutKeys)
        public bool FocusedOnTypable()
        {
            Control active = GetActiveControl();
            return ControlUtil.IsTypableControl(active);
        }

        private Control GetActiveMostControl(Control root)
        {
            Control c = root;
            if (c == null) return null;
            do
            {
                c = ((ContainerControl)c).ActiveControl;
            } while (c is ContainerControl);
            return c;
        }

        public Control GetActiveControl()
        {
            return GetActiveMostControl(Form.ActiveForm);
        }

        private void EditorWindow_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            e.IsInputKey |= e.KeyCode == Keys.Up || e.KeyCode == Keys.Down || e.KeyCode == Keys.Left || e.KeyCode == Keys.Right;
        }

        private void EditorWindow_KeyDown(object sender, KeyEventArgs e)
        {
            // handle remaining key events that ShortcutKeys can't handle for one reason or another

            // don't handle anything if we are typing to something that can be typed
            if (FocusedOnTypable()) return;

            if (!e.Control && !e.Shift && !e.Alt) HandleMissingKeys_NoModifiers(e);
            else if (e.Control && !e.Shift && !e.Alt) HandleMissingKeys_Ctrl(e);
            else if (!e.Control && e.Shift && !e.Alt) HandleMissingKeys_Shift(e);
            else if (!e.Control && !e.Shift && e.Alt) HandleMissingKeys_Alt(e);
        }

        private void HandleMissingKeys_NoModifiers(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.M:                                                // Tag/UnTag (M)
                    tagUnTagToolStripMenuItem.PerformClick(); return;
                case Keys.Multiply:                                         // Zoom In (Numpad *)
                    zoomInToolStripMenuItem.PerformClick(); return;
                case Keys.Divide:                                           // Zoom Out (Numpad /)
                    zoomOutToolStripMenuItem.PerformClick(); return;
                case Keys.Home:                                             // Center on current Cube (Home)
                    centerOnCurrentCubeToolStripMenuItem.PerformClick(); return;
                case Keys.A:                                                // Pan In (A)
                    panInToolStripMenuItem.PerformClick(); return;
                case Keys.Z:                                                // Pan Out (Z)
                    panOutToolStripMenuItem.PerformClick(); return;
                case Keys.E:                                                // Rotate Clockwise (E)
                    rotateClockwiseToolStripMenuItem.PerformClick(); return;
                case Keys.Q:                                                // Rotate Counter Clockwise (Q)
                    rotateCounterClockwiseToolStripMenuItem.PerformClick(); return;
                case Keys.O:                                                // Select next object (O)
                    selectNextObjectToolStripMenuItem.PerformClick(); return;
                case Keys.C:                                                // Select next segment (C)
                    selectNextSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Select forward segment (up arrow)
                    selectForwardSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Select backwards segment (down arrow)
                    selectBackwardsSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.Space:                                            // Select other segment (Space)
                    selectOtherSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.S:                                                // Select next side (S)
                case Keys.Right:                                            // Select next side (right Arrow)
                    selectNextSideToolStripMenuItem.PerformClick(); return;
                case Keys.Left:                                             // Select previous side (left arrow)
                    selectPreviousSideToolStripMenuItem.PerformClick(); return;
                case Keys.L:                                                // Select next line (L)
                    selectNextLineToolStripMenuItem.PerformClick(); return;
                case Keys.P:                                                // Select next point (P)
                    selectNextPointToolStripMenuItem.PerformClick(); return;
            }
        }

        private void HandleMissingKeys_Ctrl(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Home:                                             // Center entire Mine (Ctrl+Home)
                    centerEntireMineToolStripMenuItem.PerformClick(); return;
                case Keys.Left:                                             // Rotate Horizontally Left (Ctrl+Left Arrow)
                    rotateHorizontallyLeftToolStripMenuItem.PerformClick(); return;
                case Keys.Right:                                            // Rotate Horizontally Right (Ctrl+Right Arrow)
                    rotateHorizontallyRightToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Rotate Vertically Up (Ctrl+Up Arrow)
                    rotateVerticallyUpToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Rotate Vertically Down (Ctrl+Down Arrow)
                    rotateVerticallyDownToolStripMenuItem.PerformClick(); return;
            }
        }

        private void HandleMissingKeys_Shift(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Home:                                             // Center on current Object (Shift+Home)
                    centerOnCurrentObjectToolStripMenuItem.PerformClick(); return;
                case Keys.Left:                                             // Pan Left (Shift+Left Arrow)
                    panLeftToolStripMenuItem.PerformClick(); return;
                case Keys.Right:                                            // Pan Right (Shift+Right Arrow)
                    panRightToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Pan Up (Shift+Up Arrow)
                    panUpToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Pan Down (Shift+Down Arrow)
                    panDownToolStripMenuItem.PerformClick(); return;
                case Keys.O:                                                // Select previous object (Shift+O)
                    selectPreviousObjectToolStripMenuItem.PerformClick(); return;
                case Keys.C:                                                // Select previous segment (Shift+C)
                    selectPreviousSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.S:                                                // Select previous side (Shift+S)
                    selectPreviousSideToolStripMenuItem.PerformClick(); return;
                case Keys.L:                                                // Select previous line (Shift+L)
                    selectPreviousLineToolStripMenuItem.PerformClick(); return;
                case Keys.P:                                                // Select previous point (Shift+P)
                    selectPreviousPointToolStripMenuItem.PerformClick(); return;
            }
        }

        private void HandleMissingKeys_Alt(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Home:                                             // Align side rotation (Alt+Home)
                    alignSideRotationToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Pan Up (Alt+Up Arrow)
                    panUp2ToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Pan Down (Alt+Down Arrow)
                    panDown2ToolStripMenuItem.PerformClick(); return;
            }
        }
        #endregion

        #region --- Events for the tool strip
        // these should mostly just PerformClick on menu buttons below for more complex actions
        // like zooming, moving, rotating etc.
        private void openToolStripButton_Click(object sender, EventArgs e) => fileOpenToolStripMenuItem.PerformClick();
        private void saveToolStripButton_Click(object sender, EventArgs e) => fileSaveToolStripMenuItem.PerformClick();
        private void checkMineToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Diagnostics); // TODO also run check
        private void lightToolToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Lights);
        private void textureToolToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Textures);
        private void segmentToolToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Segments);
        private void wallToolToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Walls);
        private void objectToolToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Objects);
        private void preferencesToolStripButton_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Settings);
        private void zoomInToolStripButton_Click(object sender, EventArgs e) => zoomInToolStripMenuItem.PerformClick();
        private void zoomOutToolStripButton_Click(object sender, EventArgs e) => zoomOutToolStripMenuItem.PerformClick();
        private void fitMineToolStripButton_Click(object sender, EventArgs e) => fitToViewToolStripMenuItem.PerformClick();
        private void panLeftToolStripButton_Click(object sender, EventArgs e) => panLeftToolStripButton.PerformClick();
        private void panRightToolStripButton_Click(object sender, EventArgs e) => panRightToolStripButton.PerformClick();
        private void panUpToolStripButton_Click(object sender, EventArgs e) => panUpToolStripButton.PerformClick();
        private void panDownToolStripButton_Click(object sender, EventArgs e) => panDownToolStripButton.PerformClick();
        private void rotateXMToolStripButton_Click(object sender, EventArgs e) => rotateHorizontallyRightToolStripMenuItem.PerformClick();
        private void rotateXPToolStripButton_Click(object sender, EventArgs e) => rotateHorizontallyLeftToolStripMenuItem.PerformClick();
        private void rotateYMToolStripButton_Click(object sender, EventArgs e) => rotateVerticallyDownToolStripMenuItem.PerformClick();
        private void rotateYPToolStripButton_Click(object sender, EventArgs e) => rotateVerticallyUpToolStripMenuItem.PerformClick();
        private void rotateZMToolStripButton_Click(object sender, EventArgs e) => rotateClockwiseToolStripMenuItem.PerformClick();
        private void rotateZPToolStripButton_Click(object sender, EventArgs e) => rotateCounterClockwiseToolStripMenuItem.PerformClick();
        private void joinPointsToolStripButton_Click(object sender, EventArgs e) => joinPointsToolStripMenuItem.PerformClick();
        private void splitPointsToolStripButton_Click(object sender, EventArgs e) => separatePointsToolStripMenuItem.PerformClick();
        private void joinLinesToolStripButton_Click(object sender, EventArgs e) => joinLinesToolStripMenuItem.PerformClick();
        private void splitLinesToolStripButton_Click(object sender, EventArgs e) => separateLinesToolStripMenuItem.PerformClick();
        private void joinSidesToolStripButton_Click(object sender, EventArgs e) => joinSidesToolStripMenuItem.PerformClick();
        private void splitSidesToolStripButton_Click(object sender, EventArgs e) => separateSidesToolStripMenuItem.PerformClick();
        // mode and full screen buttons are bound
        private void redrawMineToolStripButton_Click(object sender, EventArgs e) => UpdateViews();
        #endregion

        #region --- Events for the File menu
        #endregion

        #region --- Events for the Edit menu
        #endregion

        #region --- Events for the View menu
        private void toolbarToolStripMenuItem_Click(object sender, EventArgs e) => mainToolStrip.Visible = !mainToolStrip.Visible;
        private void editorToolBarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!editorTools.Visible) editorTools.Show(this);
            else editorTools.Hide();
        }
        private void statusBarToolStripMenuItem_Click(object sender, EventArgs e) => statusStrip.Visible = !statusStrip.Visible;
        #endregion

        #region --- Events for the Select menu
        #endregion

        #region --- Events for the Insert menu
        #endregion

        #region --- Events for the Delete menu
        #endregion

        #region --- Events for the Join/Separate menu
        #endregion

        #region --- Events for the Tools menu
        private void textureEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Textures);
        private void segmentEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Segments);
        private void wallEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Walls);
        private void triggerEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Triggers);
        private void objectEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Objects);
        private void effectEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Effects);
        private void lightAdjustmentToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Lights);
        private void reactorTriggersToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Reactor);
        private void missionEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Mission);
        private void diagnosisToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Diagnostics);
        private void texturefiltersToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.TextureFilters);
        private void settingsToolStripMenuItem_Click(object sender, EventArgs e) => EditorTools.ShowTool(this, editorTabs.Settings);
        #endregion

        #region --- Events for the Help menu
        #endregion
    }
}

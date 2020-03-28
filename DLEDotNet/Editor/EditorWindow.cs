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
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static DLEDotNet.Editor.DLEUserControl;

namespace DLEDotNet.Editor
{
    public partial class EditorWindow : Form
    {
        /**
         *  TODO:
         *    - import settings from DLE.ini
         *    - proxy stuff from LibDescent
         * 
         */
        private LayoutOrientation _activeLayout;
        private MainView _activeMainView = null;
        private MineView mineView;
        private TextureList textureList;
        private EditorTools editorTabs;
        private EditorToolDialog editorTools = new EditorToolDialog();
        private EditorKeyBinds editorKeyBinds;
        public EditorState EditorState { get; }

        public EditorWindow()
        {
            InitializeComponent();
            EditorState = new EditorState(this);
            editorTabs = new EditorTools(this);
            editorKeyBinds = new EditorKeyBinds(this);
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

            InitializeEvents();
            SetupToolbar();
            SetupContextMenu();
            SetupRecentFilesMenu();
            editorKeyBinds.InitializeDefaultKeybinds();
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

        private void EditorState_PropertyChanged(object sender, PropertyChangeEventArgs e)
        {
            if (e.PropertyName == "FullScreen")
                ActiveMainView.SetFullScreen(EditorState.FullScreen);
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
            newMainView.SetFullScreen(EditorState.FullScreen);

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

        #region --- Tool strip & context menu handling (icons, binding...)

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

            binder.BindToolStripButtonAsCheckBox(this.fullScreenToolStripButton, PROP(s => s.FullScreen), false);
        }

        private void SetupContextMenu()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);

            binder.BindToolStripMenuItemAsRadioButton(this.selectionModePointsToolStripMenuItem, PROP(s => s.SelectionMode), SelectMode.Point, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionModeLinesToolStripMenuItem, PROP(s => s.SelectionMode), SelectMode.Line, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionModeSidesToolStripMenuItem, PROP(s => s.SelectionMode), SelectMode.Side, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionModeSegmentsToolStripMenuItem, PROP(s => s.SelectionMode), SelectMode.Segment, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionModeObjectsToolStripMenuItem, PROP(s => s.SelectionMode), SelectMode.Object, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionModeBlocksToolStripMenuItem, PROP(s => s.SelectionMode), SelectMode.Block, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionCandidatesOffToolStripMenuItem, PROP(s => s.SavedPrefs.QuickSelectionCandidates), QuickSelectionCandidateMode.Off, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionCandidatesCirclesToolStripMenuItem, PROP(s => s.SavedPrefs.QuickSelectionCandidates), QuickSelectionCandidateMode.Circles, false);
            binder.BindToolStripMenuItemAsRadioButton(this.selectionCandidatesFullToolStripMenuItem, PROP(s => s.SavedPrefs.QuickSelectionCandidates), QuickSelectionCandidateMode.Full, false);
            binder.BindToolStripMenuItemAsCheckBox(this.enableQuickSelectionToolStripMenuItem, PROP(s => s.SavedPrefs.EnableQuickSelection), false);
            binder.BindToolStripMenuItemAsCheckBox(this.moveAlongViewerAxesToolStripMenuItem, PROP(s => s.SavedPrefs.ElementMovementReferenceFormatted), false);
        }

        private void mainToolStrip_BackColorChanged(object sender, EventArgs e)
        {
            SetupToolbarIcons();
        }
        #endregion

        #region --- Event handling setup
        private void InitializeEvents()
        {
            EditorState.PropertyChanged += EditorState_PropertyChanged;
            EditorState.SettingChanged += EditorState_SettingChanged;
            this.editorTools.Shown += (object sender, EventArgs e) => { this.editorToolBarToolStripMenuItem.CheckState = CheckState.Checked; };
            this.editorTools.VisibleChanged += (object sender, EventArgs e) => { this.editorToolBarToolStripMenuItem.CheckState = editorTools.Visible ? CheckState.Checked : CheckState.Unchecked; };
            this.mainToolStrip.VisibleChanged += (object sender, EventArgs e) => { this.toolbarToolStripMenuItem.CheckState = mainToolStrip.Visible ? CheckState.Checked : CheckState.Unchecked; };
            this.statusStrip.VisibleChanged += (object sender, EventArgs e) => { this.statusBarToolStripMenuItem.CheckState = statusStrip.Visible ? CheckState.Checked : CheckState.Unchecked; };

            this.openToolStripButton.Click += (object sender, EventArgs e) => PushNewRecentFile("F" + Environment.TickCount);
            this.undoToolStripMenuItem.Click += _HELLO();
        }

        // temporary functions for testing purposes
        private static void _TODO(string s, [CallerMemberName] string caller = null) => throw new NotImplementedException("feature '" + s + "' not implemented (called from " + caller + ")");
        private static void _HELLO_IMPL(string caller) => MessageBox.Show("Hello from " + caller + "!");
        private static EventHandler _HELLO() => (object sender, EventArgs e) => _HELLO_IMPL(sender.ToString());
        #endregion

        #region --- View update code

        private void UpdateViews()
        {
            UpdateMineView();
            UpdateTextureList();
        }

        private void UpdateMineView()
        {
            mineView.Owner = this;
            // ...
        }

        private void UpdateTextureList()
        {
            textureList.Owner = this;
            // ...
            // updating TextureCount would be a good idea to cause the scroll bar to be updated
            textureList.TextureCount = textureList.TextureCount; // whatever
        }
        #endregion

        #region --- Recent files handling
        private ToolStripMenuItem[] recentFileControls;

        private void SetupRecentFilesMenu()
        {
            recentFileControls = new ToolStripMenuItem[]
            {
                recentFileToolStripMenuItem0,
                recentFileToolStripMenuItem1,
                recentFileToolStripMenuItem2,
                recentFileToolStripMenuItem3,
                recentFileToolStripMenuItem4,
                recentFileToolStripMenuItem5,
                recentFileToolStripMenuItem6,
                recentFileToolStripMenuItem7,
                recentFileToolStripMenuItem8,
                recentFileToolStripMenuItem9
            };
            UpdateRecentFilesMenu();
        }

        private string NumberToRecentFileAnchor(int v)
        {
            if (1 <= v && v <= 9) return "&" + v.ToString();
            if (10 == v) return "1&0";
            return v.ToString();
        }

        private void UpdateRecentFilesMenu()
        {
            List<string> recentFiles = EditorState.SavedPrefs.RecentFiles;
            recentFileNoneToolStripMenuItem.Visible = recentFiles.Count == 0;
            foreach (var (index, control) in recentFileControls.Select((item, index) => (index, item)))
            {
                if (control.Visible = (index < recentFiles.Count))
                    control.Text = NumberToRecentFileAnchor(index + 1) + " " + recentFiles[index];
            }
        }

        private void PushNewRecentFile(string path)
        {
            int maxRecentFiles = recentFileControls.Length;
            EditorState.SavedPrefs.RecentFiles.Insert(0, path);
            int newRecentFiles = EditorState.SavedPrefs.RecentFiles.Count;
            if (newRecentFiles > maxRecentFiles)
                EditorState.SavedPrefs.RecentFiles.RemoveRange(maxRecentFiles, newRecentFiles - maxRecentFiles);
            UpdateRecentFilesMenu();
        }
        #endregion

        // Most of the event listeners below should be as few lines as possible;
        // it's ideal to place logic on other classes
        // the main menu is the "canonical" event handler; thus tool strip
        // and key event handlers just point back there for most of them

        #region --- Handling key events (those not handled by ShortcutKeys)
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
            editorKeyBinds.EditorWindowKeyDown(e);
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
        private void exitToolStripMenuItem_Click(object sender, EventArgs e) => this.Close();
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

        #region --- Events for the context menu
        private void quickCopyContextToolStripMenuItem_Click(object sender, EventArgs e) => quickCopyToolStripMenuItem.PerformClick();
        private void quickPasteContextToolStripMenuItem_Click(object sender, EventArgs e) => quickPasteToolStripMenuItem.PerformClick();
        private void deleteBlockContextToolStripMenuItem_Click(object sender, EventArgs e) => deleteBlockToolStripMenuItem.PerformClick();
        private void collapseEdgeContextToolStripMenuItem1_Click(object sender, EventArgs e) => collapseEdgeToolStripMenuItem.PerformClick();
        private void collapseToWedgeContextToolStripMenuItem_Click(object sender, EventArgs e) => createWedgeToolStripMenuItem.PerformClick();
        private void collapseToPyramidContextToolStripMenuItem_Click(object sender, EventArgs e) => createPyramidToolStripMenuItem.PerformClick();
        private void makePointsParallelContextToolStripMenuItem_Click(object sender, EventArgs e) => _TODO("make points parallel");
        private void undoContextToolStripMenuItem_Click(object sender, EventArgs e) => undoToolStripMenuItem.PerformClick();
        private void redoContextToolStripMenuItem_Click(object sender, EventArgs e) => redoToolStripMenuItem.PerformClick();
        #endregion
    }
}

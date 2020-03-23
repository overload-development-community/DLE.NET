using EditorUITest.Data;
using EditorUITest.Dialogs;
using EditorUITest.Editor;
using EditorUITest.Editor.Layouts;
using EditorUITest.Editor.Layouts.Floating;
using EditorUITest.Editor.Layouts.Vertical;
using EditorUITest.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public partial class EditorWindow : Form
    {
        /**
         *  notes:
         *    all menu item ShortcutKeys should work (only need to handle Click for menu items, not key events for those!)
         *    perhaps the tool strip buttons Click events should just use the menu item click events too
         * 
         *  TODO:
         *    - binding system, testing, more examples
         *    - icons
         *    - .NET Core port
         * 
         */
        private LayoutOrientation _activeLayout;
        private MainView _activeMainView = null;
        private MineView mineView = new MineView();
        private TextureList textureList = new TextureList();
        private EditorTabs editorTabs = new EditorTabs();
        public EditorState EditorState { get; } = new EditorState();

        public EditorWindow()
        {
            InitializeComponent();
        }

        private void EditorWindow_Load(object sender, EventArgs e)
        {
            editorTabs.SelfTest(); // must be called before assigning layout!!!
            ActiveLayout = LayoutOrientation.VERTICAL_SS;
            //new TestForm().Show(this);
        }

        #region --- Layout handling code
        private LayoutOrientation ActiveLayout
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

        private void UpdateLayoutMainView()
        {
            // create new MainView for selected layout
            mainPanel.SuspendLayout();

            mainPanel.Controls.Clear();
            MainView newMainView;

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

            if (_activeMainView != null)
            {
                _activeMainView.Dispose();
            }
            mainPanel.Controls.Add(newMainView);
            newMainView.Dock = DockStyle.Fill;

            EditorTabContainer editorTabContainer = newMainView.GetEditorTabs();
            if (editorTabContainer == null)
            {
                editorTabs.AttachToFloating(_activeLayout);
            }
            else
            {
                editorTabs.AttachToContainer(_activeLayout, editorTabContainer);
            }
            
            mainPanel.ResumeLayout(false);
        }

        private void OnMainViewUpdate(MainView newMainView, MainView oldMainView)
        {
            UpdateMineView();
            UpdateTextureList();
        }
        #endregion

        #region --- View update code
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

        #region --- Handling key events (those not handled by ShortcutKeys)
        private bool FocusedOnTypable()
        {
            Control active = GetActiveControl();
            return ControlUtilities.IsTypableControl(active);
        }

        private Control GetActiveControl()
        {
            Control c = this;
            do
            {
                c = ((ContainerControl)c).ActiveControl;
            } while (c is ContainerControl);
            return c;
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

        #region --- Events for the File menu
        #endregion

        #region --- Events for the Edit menu
        #endregion

        #region --- Events for the View menu
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
        private void textureEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Textures);
        private void segmentEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Segments);
        private void wallEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Walls);
        private void triggerEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Triggers);
        private void objectEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Objects);
        private void effectEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Effects);
        private void lightAdjustmentToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Lights);
        private void reactorTriggersToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Reactor);
        private void missionEditToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Mission);
        private void diagnosisToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Diagnostics);
        private void texturefiltersToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.TextureFilters);
        private void settingsToolStripMenuItem_Click(object sender, EventArgs e) => EditorTabs.ShowTab(this, editorTabs.Settings);
        #endregion

        #region --- Events for the Help menu
        #endregion
    }
}

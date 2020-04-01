using DLEDotNet.Editor.Layouts;
using System.ComponentModel;
using System.Linq;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    public class EditorTools
    {
        private bool attached;
        private bool attachedToFloating;
        public DLELayoutableUserControl[] AllTools { get; }
        // tools in above must match the tabs in an EditorTabsContainer

        /// <summary>
        /// Owner of this EditorTools instance. In the Windows Forms version,
        /// this is an instance of EditorWindow.
        /// </summary>
        public object Owner { get; }

        public TextureTool Textures { get; }
        public SegmentTool Segments { get; }
        public WallTool Walls { get; }
        public TriggerTool Triggers { get; }
        public ObjectTool Objects { get; }
        public EffectTool Effects { get; }
        public LightsTool Lights { get; }
        public ReactorTool Reactor { get; }
        public MissionTool Mission { get; }
        public DiagnosticsTool Diagnostics { get; }
        public TextureFiltersTab TextureFilters { get; }
        public SettingsTool Settings { get; }

        public EditorTools(object owner)
        {
            Owner = owner;
            Textures = new TextureTool();
            Segments = new SegmentTool();
            Walls = new WallTool();
            Triggers = new TriggerTool();
            Objects = new ObjectTool();
            Effects = new EffectTool();
            Lights = new LightsTool();
            Reactor = new ReactorTool();
            Mission = new MissionTool();
            Diagnostics = new DiagnosticsTool();
            TextureFilters = new TextureFiltersTab();
            Settings = new SettingsTool();

            AllTools = new DLELayoutableUserControl[] { Textures, Segments, Walls, Triggers, Objects, Effects, Lights, Reactor, Mission, Diagnostics, TextureFilters, Settings };
            attached = attachedToFloating = false;

            foreach (DLELayoutableUserControl tab in AllTools)
            {
                tab.Owner = (EditorWindow)owner;
            }
        }

        private void DetachControl(UserControl c)
        {
            Control parent = c.Parent;
            parent?.Controls.Remove(c);
            if (parent is Form)
            {
                (parent as Form).Close();
                parent.Dispose();
            }
        }

        public void Detach()
        {
            foreach (UserControl tab in AllTools)
            {
                DetachControl(tab);
            }
            attached = false;
            attachedToFloating = false;
        }

        public void AttachToContainer(LayoutOrientation layout, EditorTabContainer ctr)
        {
            if (attached) Detach();
            attachedToFloating = false;
            attached = true;

            TabControl tabControl = ctr.Controls.OfType<TabControl>().First();
            for (int i = 0; i < AllTools.Length; ++i)
            {
                DLELayoutableUserControl tab = AllTools[i];
                TabPage tabPage = tabControl.TabPages[i];

                tabPage.Controls.Add(tab);
                tab.Location = new System.Drawing.Point(0, 0);
                tab.DialogLayout = layout;
                tabPage.ContextMenuStrip = tab.ContextMenuStrip;
            }
        }

        public void CreateFloatingForm(DLELayoutableUserControl tab)
        {
            Form f = new Form();
            f.SuspendLayout();

            f.StartPosition = FormStartPosition.WindowsDefaultLocation;
            f.FormBorderStyle = FormBorderStyle.FixedSingle;
            f.MaximizeBox = false;
            f.MinimizeBox = false;
            f.ClientSize = tab.Size;
            f.Text = tab.FloatingTitle;
            f.Controls.Add(tab);
            f.FormClosing += (object sender, FormClosingEventArgs e) =>
            {
                if (e.CloseReason == CloseReason.UserClosing)
                {
                    f.Hide();
                    e.Cancel = true;
                }
            };

            tab.Location = new System.Drawing.Point(0, 0);
            if (tab.HelpPageName != null)
            {
                f.HelpButton = true;
                f.HelpButtonClicked += (object sender, CancelEventArgs e) =>
                {
                    (Owner as EditorWindow)?.OpenHelpPage(tab.HelpPageName, f);
                    e.Cancel = true; // prevent help mouse cursor
                };
            }

            f.ResumeLayout(false);
        }

        public void AttachToFloating(LayoutOrientation layout)
        {
            if (attachedToFloating) return;
            if (attached) Detach();
            attachedToFloating = true;
            attached = true;

            foreach (DLELayoutableUserControl tab in AllTools)
            {
                tab.DialogLayout = layout;
                CreateFloatingForm(tab);
            }
        }

        internal void SelfTest()
        {
            foreach (DLELayoutableUserControl tab in AllTools)
            {
                tab.SelfTestRecursive();
            }
        }

        public static void ShowTool(Form parentWindow, UserControl tool)
        {
            if (tool.Parent is Form)
            {
                Form parentForm = tool.ParentForm;
                if (parentForm.Visible)
                    parentForm.BringToFront();
                else
                    parentForm.Show(parentWindow);
            }
            else if (tool.Parent is TabPage)
            {
                // select correct tab
                TabPage tabPage = tool.Parent as TabPage;
                TabControl tabControl = tabPage.Parent as TabControl;
                if (tabControl != null)
                {
                    tabControl.SelectTab(tabPage);
                }
            }
        }
    }
}

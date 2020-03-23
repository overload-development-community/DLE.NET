using EditorUITest.Editor.Layouts;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Editor
{
    public class EditorTabs
    {
        private bool attached;
        private bool attachedToFloating;
        public DLELayoutableUserControl[] AllTabs { get; }
        // tabs in above must match the tabs in an EditorTabsContainer

        public TextureTab Textures { get; }
        public SegmentTab Segments { get; }
        public WallTab Walls { get; }
        public TriggerTab Triggers { get; }
        public ObjectTab Objects { get; }
        public EffectTab Effects { get; }
        public LightsTab Lights { get; }
        public ReactorTab Reactor { get; }
        public MissionTab Mission { get; }
        public DiagnosticsTab Diagnostics { get; }
        public TextureFiltersTab TextureFilters { get; }
        public SettingsTab Settings { get; }

        public EditorTabs()
        {
            Textures = new TextureTab();
            Segments = new SegmentTab();
            Walls = new WallTab();
            Triggers = new TriggerTab();
            Objects = new ObjectTab();
            Effects = new EffectTab();
            Lights = new LightsTab();
            Reactor = new ReactorTab();
            Mission = new MissionTab();
            Diagnostics = new DiagnosticsTab();
            TextureFilters = new TextureFiltersTab();
            Settings = new SettingsTab();

            AllTabs = new DLELayoutableUserControl[] { Textures, Segments, Walls, Triggers, Objects, Effects, Lights, Reactor, Mission, Diagnostics, TextureFilters, Settings };
            attached = attachedToFloating = false;
        }

        private void DetachControl(UserControl c)
        {
            Control parent = c.Parent;
            parent?.Controls.Remove(c);
            if (parent is Form)
            {
                (parent as Form).Close();
            }
        }

        public void Detach()
        {
            foreach (UserControl tab in AllTabs)
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
            for (int i = 0; i < AllTabs.Length; ++i)
            {
                DLELayoutableUserControl tab = AllTabs[i];
                TabPage tabPage = tabControl.TabPages[i];
                
                tabPage.SuspendLayout();
                tabPage.Controls.Add(tab);
                tab.Location = new System.Drawing.Point(0, 0);
                tab.DialogLayout = layout;
                tabPage.ResumeLayout(false);
            }
        }

        public void CreateFloatingForm(DLELayoutableUserControl tab)
        {
            Form f = new Form();
            f.SuspendLayout();

            f.StartPosition = FormStartPosition.WindowsDefaultLocation;
            f.FormBorderStyle = FormBorderStyle.FixedToolWindow;
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

            f.ResumeLayout(false);
        }

        public void AttachToFloating(LayoutOrientation layout)
        {
            if (attachedToFloating) return;
            if (attached) Detach();
            attachedToFloating = true;
            attached = true;

            foreach (DLELayoutableUserControl tab in AllTabs)
            {
                tab.DialogLayout = layout;
                CreateFloatingForm(tab);
            }
        }

        internal void SelfTest()
        {
            foreach (DLELayoutableUserControl tab in AllTabs)
            {
                tab.SelfTestRecursive();
            }
        }

        public static void ShowTab(Form parentWindow, UserControl tab)
        {
            if (tab.Parent is Form)
            {
                Form parentForm = tab.ParentForm;
                if (parentForm.Visible)
                    parentForm.BringToFront();
                else
                    parentForm.Show(parentWindow);
            }
            else if (tab.Parent is TabPage)
            {
                // select correct tab
                TabPage tabPage = tab.Parent as TabPage;
                TabControl tabControl = tabPage.Parent as TabControl;
                if (tabControl != null)
                {
                    tabControl.SelectTab(tabPage);
                }
            }
        }
    }
}

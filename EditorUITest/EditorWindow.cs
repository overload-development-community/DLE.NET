using EditorUITest.Dialogs;
using EditorUITest.Editor;
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
         *    - port object advanced tab
         *    - port effect tab
         *    - port vertical/floating UIs
         *    - icons
         * 
         */
        private LayoutOrientation _activeLayout = LayoutOrientation.HORIZONTAL;
        private MainView _activeMainView = null;
        private MineView mineView = new MineView();
        private TextureList textureList = new TextureList();
        private EditorTabs editorTabs = new EditorTabs();

        public EditorWindow()
        {
            InitializeComponent();
        }

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
                OnMainViewUpdate(value,_activeMainView);
                _activeMainView = value;
            }
        }

        private void EditorWindow_Load(object sender, EventArgs e)
        {
            ActiveLayout = LayoutOrientation.FLOATING;
            //new TestForm().Show(this);
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
            // OK, now we'll detach our MineView, TextureList and tabs from the old layout
            // and add them to the new one



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
        }

        private bool FocusedOnTypable()
        {
            Control active = GetActiveControl();
            return ControlUtilities.IsTypableControl(active);
        }

        private Control GetActiveControl()
        {
            Control c = this.ActiveControl;
            while (c is ContainerControl)
            {
                c = ((ContainerControl)c).ActiveControl;
            }
            return c;
        }
    }
}

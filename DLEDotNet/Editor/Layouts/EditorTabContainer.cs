using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor.Layouts
{
    [Description("A set of tabs for the DLE editor tools.")]
    public partial class EditorTabContainer : UserControl
    {
        /*
         * 
         * NOTE: This is only the container. The actual tab contents
         *       will be injected elsewhere (in EditorTabs).
         *       
         *       To add a new tab, you need to add a tab here,
         *       create a new user component (with the correct size;
         *       look at the other tabs), and then add it to EditorTabs.
         * 
         * 
         */

        public EditorTabContainer()
        {
            InitializeComponent();
        }

        public TabControl TabControl
        {
            get
            {
                return this.editorTabControl;
            }
        }

        public int SelectedTab
        {
            get
            {
                return this.editorTabControl.SelectedIndex;
            }
            set
            {
                this.editorTabControl.SelectedIndex = value;
            }
        }
    }
}

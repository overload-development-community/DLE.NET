using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public partial class EditorTabContainer : UserControl
    {
        public EditorTabContainer()
        {
            InitializeComponent();
        }

        /*
         * 
         * NOTE: This is only the container. The actual tab contents
         *       will be injected elsewhere (in EditorTabs).
         * 
         * 
         */
    }
}

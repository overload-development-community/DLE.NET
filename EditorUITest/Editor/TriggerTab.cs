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
    public partial class TriggerTab : LayoutableUserControl
    {
        public TriggerTab()
        {
            InitializeLayout();
        }

        public void InitializeLayout()
        {
            this.Controls.Clear();
            InitializeComponent();
            if (this.DialogLayout == LayoutOrientation.VERTICAL)
            {
                //                this.CopyLayout(typeof(VerticalTriggerTab));
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Editor
{
    public partial class ObjectPosition : LayoutableUserControl
    {
        public ObjectPosition()
        {
            InitializeLayout();
        }

        public void InitializeLayout()
        {
            this.Controls.Clear();
            InitializeComponent();
            if (this.DialogLayout == LayoutOrientation.VERTICAL || this.DialogLayout == LayoutOrientation.VERTICAL_SS)
            {
                //this.CopyLayout(typeof(VerticalSegmentTab));
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }
    }
}

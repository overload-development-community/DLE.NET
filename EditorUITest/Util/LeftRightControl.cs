using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Util
{
    public partial class LeftRightControl : UserControl
    {
        [Browsable(true)]
        [Category("behavior")]
        [DefaultValue(null)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The control linked to this left/right control. Should be a control that accepts user input.")]
        public Control LinkedControl { get; set; } = null;

        public LeftRightControl()
        {
            InitializeComponent();
        }

        private void leftButton_Click(object sender, EventArgs e)
        {
            if (LinkedControl != null)
                ControlUtilities.SafeLeft(LinkedControl);
        }

        private void rightButton_Click(object sender, EventArgs e)
        {
            if (LinkedControl != null)
                ControlUtilities.SafeRight(LinkedControl);
        }
    }
}

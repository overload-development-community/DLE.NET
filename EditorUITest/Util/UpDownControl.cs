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
    public partial class UpDownControl : UserControl
    {
        [Browsable(true)]
        [Category("behavior")]
        [DefaultValue(null)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The control linked to this left/right control. Should be a control that accepts user input.")]
        public Control LinkedControl { get; set; } = null;

        public UpDownControl()
        {
            InitializeComponent();
        }

        private void upButton_Click(object sender, EventArgs e)
        {
            if (LinkedControl != null)
                ControlUtilities.SafeUp(LinkedControl);
        }

        private void downButton_Click(object sender, EventArgs e)
        {
            if (LinkedControl != null)
                ControlUtilities.SafeDown(LinkedControl);
        }
    }
}

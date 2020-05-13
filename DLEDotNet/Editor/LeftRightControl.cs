using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Util;

namespace DLEDotNet.Editor
{
    [Description("A control that displays left/right buttons that change the value of another control that receives user input.")]
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

        private void DoLeft()
        {
            Control control = LinkedControl;
            if (control is SparseTrackBar)
                ControlUtil.SafeDecrement(control as SparseTrackBar); 
            else if (control is TrackBar)
                ControlUtil.SafeDecrement(control as TrackBar);
            else if (control is ListBox)
                ControlUtil.SafeDecrement(control as ListBox);
            else if (control is ComboBox)
                ControlUtil.SafeDecrement(control as ComboBox);
            else if (control is NumericUpDown)
                ControlUtil.SafeDecrement(control as NumericUpDown);
        }

        private void DoRight()
        {
            Control control = LinkedControl;
            if (control is SparseTrackBar)
                ControlUtil.SafeDecrement(control as SparseTrackBar); 
            else if (control is TrackBar)
                ControlUtil.SafeIncrement(control as TrackBar);
            else if (control is ListBox)
                ControlUtil.SafeIncrement(control as ListBox);
            else if (control is ComboBox)
                ControlUtil.SafeIncrement(control as ComboBox);
            else if (control is NumericUpDown)
                ControlUtil.SafeIncrement(control as NumericUpDown);
        }

        private void leftButton_Click(object sender, EventArgs e)
        {
            DoLeft();
        }

        private void rightButton_Click(object sender, EventArgs e)
        {
            DoRight();
        }

        private void LeftRightControl_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyCode == Keys.Left || e.KeyCode == Keys.Right)
            {
                e.IsInputKey = true;
            }
        }

        private void LeftRightControl_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Left)
            {
                DoLeft();
                leftButton.Focus();
                e.SuppressKeyPress = true;
            }
            else if (e.KeyCode == Keys.Right)
            {
                DoRight();
                rightButton.Focus();
                e.SuppressKeyPress = true;
            }
        }
    }
}

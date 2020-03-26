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
    [Description("A control that displays up/down buttons that change the value of another control that receives user input.")]
    public partial class UpDownControl : UserControl
    {
        [Browsable(true)]
        [Category("behavior")]
        [DefaultValue(null)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The control linked to this left/right control. Should be a control that accepts user input.")]
        public Control LinkedControl { get; set; } = null;

        private const bool UseNumericUpDown = false; // too buggy for now
        private NumericUpDown nud;

        public UpDownControl()
        {
            InitializeComponent();
            UpdateButtonSize();
            if (UseNumericUpDown && ControlUtilities.OnWindows)
            {
                upButton.Visible = false;
                downButton.Visible = false;

                nud = new NumericUpDown();
                nud.Size = this.ClientSize;
                nud.Location = new Point(0, 0);
                nud.Minimum = 0;
                nud.Maximum = 2;
                nud.Value = 1;
                nud.Name = "numericUpDown";
                nud.ValueChanged += numericUpDown_ValueChanged;
                nud.Focus();
                this.Controls.Add(nud);
            }
        }

        private void UpdateButtonSize()
        {
            int y = this.Height / 2 - 1;
            int h = y + 3;
            upButton.Location = new Point(0, 0);
            downButton.Location = new Point(0, y);
            upButton.Size = new Size(this.Width, h);
            downButton.Size = new Size(this.Width, h);
        }

        private void DoUp()
        {
            Control control = LinkedControl;
            if (control is SparseTrackBar)
                ControlUtilities.SafeIncrement(control as SparseTrackBar);
            else if (control is TrackBar)
                ControlUtilities.SafeIncrement(control as TrackBar);
            else if (control is ListBox)
                ControlUtilities.SafeDecrement(control as ListBox);
            else if (control is ComboBox)
                ControlUtilities.SafeDecrement(control as ComboBox);
            else if (control is NumericUpDown)
                ControlUtilities.SafeIncrement(control as NumericUpDown);
        }

        private void DoDown()
        {
            Control control = LinkedControl;
            if (control is SparseTrackBar)
                ControlUtilities.SafeDecrement(control as SparseTrackBar);
            else if (control is TrackBar)
                ControlUtilities.SafeDecrement(control as TrackBar);
            else if (control is ListBox)
                ControlUtilities.SafeIncrement(control as ListBox);
            else if (control is ComboBox)
                ControlUtilities.SafeIncrement(control as ComboBox);
            else if (control is NumericUpDown)
                ControlUtilities.SafeDecrement(control as NumericUpDown);
        }

        private void numericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (nud.Value < 1)
                DoDown();
            else if (nud.Value > 1)
                DoUp();
            nud.Value = 1;
        }

        private void upButton_Click(object sender, EventArgs e)
        {
            DoUp();
        }

        private void downButton_Click(object sender, EventArgs e)
        {
            DoDown();
        }

        private void UpDownControl_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (nud != null || (e.KeyCode == Keys.Up || e.KeyCode == Keys.Down))
            {
                e.IsInputKey = true;
            }
        }

        private void UpDownControl_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Up)
            {
                DoUp();
                this.upButton.Focus();
                e.SuppressKeyPress = true;
            }
            else if (e.KeyCode == Keys.Down)
            {
                DoDown();
                this.downButton.Focus();
                e.SuppressKeyPress = true;
            }
        }

        private void UpDownControl_SizeChanged(object sender, EventArgs e)
        {
            UpdateButtonSize();
        }
    }
}

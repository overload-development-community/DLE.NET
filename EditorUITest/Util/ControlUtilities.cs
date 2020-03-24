using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Util
{
    public static class ControlUtilities
    {
        public static bool IsTypableControl(Control control)
        {
            return control is TextBox || control is ComboBox || control is ListBox;
        }

        public static void ForceValidate(Control control, Control scapegoat)
        {
#if DEBUG
            bool wasValidated = false;
            EventHandler eh = (object sender, EventArgs e) =>
            {
                wasValidated = true;
            };
            control.Validated += eh;
#endif
            // jump focus back and forth to force validation on original control
            scapegoat.Parent?.SuspendLayout();
            scapegoat.Visible = scapegoat.TabStop = true;
            scapegoat.Focus();
            control.Focus();
            scapegoat.Visible = scapegoat.TabStop = false;
            scapegoat.Parent?.ResumeLayout(false);
#if DEBUG
            control.Validated -= eh;
            System.Diagnostics.Debug.Assert(wasValidated, "ForceValidate should cause validation in control, but it isn't working properly");
#endif
        }

        internal static void SafeUp(Control control)
        {
            if (control is SparseTrackBar)
                SafeIncrement(control as SparseTrackBar);
            else if (control is TrackBar)
                SafeIncrement(control as TrackBar);
            else if (control is ListBox)
                SafeDecrement(control as ListBox);
            else if (control is ComboBox)
                SafeDecrement(control as ComboBox);
            else if (control is NumericUpDown)
                SafeIncrement(control as NumericUpDown);
        }

        internal static void SafeDown(Control control)
        {
            if (control is SparseTrackBar)
                SafeDecrement(control as SparseTrackBar);
            else if (control is TrackBar)
                SafeDecrement(control as TrackBar);
            else if (control is ListBox)
                SafeIncrement(control as ListBox);
            else if (control is ComboBox)
                SafeIncrement(control as ComboBox);
            else if (control is NumericUpDown)
                SafeDecrement(control as NumericUpDown);
        }

        internal static void SafeLeft(Control control)
        {
            if (control is TrackBar)
                SafeDecrement(control as TrackBar);
            else if (control is ListBox)
                SafeDecrement(control as ListBox);
            else if (control is ComboBox)
                SafeDecrement(control as ComboBox);
            else if (control is NumericUpDown)
                SafeDecrement(control as NumericUpDown);
        }

        internal static void SafeRight(Control control)
        {
            if (control is TrackBar)
                SafeIncrement(control as TrackBar);
            else if (control is ListBox)
                SafeIncrement(control as ListBox);
            else if (control is ComboBox)
                SafeIncrement(control as ComboBox);
            else if (control is NumericUpDown)
                SafeIncrement(control as NumericUpDown);
        }

        internal static void SafeIncrement(TrackBar trackBar)
        {
            if (trackBar.Value < trackBar.Maximum)
                ++trackBar.Value;
        }

        internal static void SafeDecrement(TrackBar trackBar)
        {
            if (trackBar.Value > trackBar.Minimum)
                --trackBar.Value;
        }

        internal static void SafeIncrement(NumericUpDown numericUpDown)
        {
            if (numericUpDown.Value < numericUpDown.Maximum)
                ++numericUpDown.Value;
        }

        internal static void SafeDecrement(NumericUpDown numericupDown)
        {
            if (numericupDown.Value > numericupDown.Minimum)
                --numericupDown.Value;
        }

        internal static void SafeIncrement(ComboBox comboBox)
        {
            if (comboBox.SelectedIndex < comboBox.Items.Count)
                ++comboBox.SelectedIndex;
        }

        internal static void SafeDecrement(ComboBox comboBox)
        {
            if (comboBox.SelectedIndex > 0)
                --comboBox.SelectedIndex;
        }

        internal static void SafeIncrement(ListBox listBox)
        {
            if (listBox.SelectedIndex < listBox.Items.Count)
                ++listBox.SelectedIndex;
        }

        internal static void SafeDecrement(ListBox listBox)
        {
            if (listBox.SelectedIndex > 0)
                --listBox.SelectedIndex;
        }
    }
}

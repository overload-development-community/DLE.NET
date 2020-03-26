using DLEDotNet.Editor;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Util
{
    public static class ControlUtilities
    {
        public static readonly bool OnWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);

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

        public static bool IsDark()
        {
            return Control.DefaultBackColor.GetBrightness() < 0.45f;
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
            if (comboBox.SelectedIndex < comboBox.Items.Count - 1)
                ++comboBox.SelectedIndex;
        }

        internal static void SafeDecrement(ComboBox comboBox)
        {
            if (comboBox.SelectedIndex > 0)
                --comboBox.SelectedIndex;
        }

        internal static void SafeIncrement(ListBox listBox)
        {
            if (listBox.SelectedIndex < listBox.Items.Count - 1)
                ++listBox.SelectedIndex;
        }

        internal static void SafeDecrement(ListBox listBox)
        {
            if (listBox.SelectedIndex > 0)
                --listBox.SelectedIndex;
        }
    }
}

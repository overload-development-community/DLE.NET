using DLEDotNet.Data;
using DLEDotNet.Editor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace DLEDotNet.Util
{
    public static class ControlUtil
    {
        /// <summary>
        /// Whether this program is currently running on Windows.
        /// This should practically always be true under Windows Forms, but may be false under Mono.
        /// It is mostly used to filter out Win32 API calls on other platforms.
        /// </summary>
        public static readonly bool OnWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);

        /// <summary>
        /// Returns whether the given control accepts user input by keyboard.
        /// </summary>
        /// <param name="control">The control to test.</param>
        /// <returns>Whether the control is designed to accept user input by keyboard.</returns>
        public static bool IsTypableControl(Control control)
        {
            return control is TextBox || control is ComboBox || control is ListBox || control is ScrollBar;
        }

        /// <summary>
        /// Forces a validation on an existing control. This method should only be used
        /// for controls that cannot implement the IValidatable interface.
        /// </summary>
        /// <param name="control">The control to validate.</param>
        /// <param name="scapegoat">Any other control within the same form.</param>
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

        /// <summary>
        /// Converts a StartupWindowState to a FormWindowState.
        /// </summary>
        /// <param name="startupState">The StartupWindowState.</param>
        /// <returns></returns>
        public static FormWindowState ConvertToFormWindowState(StartupWindowState startupState)
        {
            switch (startupState)
            {
                case StartupWindowState.Windowed:
                    return FormWindowState.Normal;
                case StartupWindowState.Maximized:
                    return FormWindowState.Maximized;
            }
            return FormWindowState.Normal;
        }

        /// <summary>
        /// Returns whether the dark mode should be used.
        /// </summary>
        /// <returns>Whether the system theme means the dark mode should be used.</returns>
        public static bool IsDarkMode()
        {
            return Control.DefaultBackColor.GetBrightness() < 0.45f;
        }

        /// <summary>
        /// Deletes all child controls from a control; deleting entails removing the control from its parent and disposing it
        /// (and leaving it thus in an unusable state, prepared to be collected by the GC).
        /// </summary>
        /// <param name="root">The control to delete all child controls from.</param>
        public static void DeleteAllChildControls(Control root)
        {
            List<Control> oldControls = new List<Control>();
            oldControls.AddRange(root.Controls.OfType<Control>());
            root.Controls.Clear();
            foreach (Control oldControl in oldControls)
            {
                oldControl.Dispose();
            }
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

        internal static void CatchClickAnywhereOnce(Form form, Action onClick)
        {
            if (OnWindows)
            {
                var mouseClickFilter = new MouseClickMessageFilter(form);
                mouseClickFilter.Click += (object sender, EventArgs e) =>
                {
                    Application.RemoveMessageFilter(mouseClickFilter);
                    onClick();
                };
                Application.AddMessageFilter(mouseClickFilter);
            }
        }
    }
}

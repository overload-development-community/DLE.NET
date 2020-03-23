using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Dialogs
{
    public partial class InputDialog : Form
    {
        public InputDialog()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Show the dialog and ask the user for input.
        /// </summary>
        /// <returns>The user input, or null if Cancel was pressed.</returns>
        public static string Display(string prompt)
        {
            return Display(prompt, "", 32767);
        }

        /// <summary>
        /// Show the dialog and ask the user for input.
        /// </summary>
        /// <returns>The user input, or null if Cancel was pressed.</returns>
        public static string Display(string prompt, string defaultValue)
        {
            return Display(prompt, defaultValue, 32767);
        }

        /// <summary>
        /// Show the dialog and ask the user for input.
        /// </summary>
        /// <returns>The user input, or null if Cancel was pressed.</returns>
        public static string Display(string prompt, string defaultValue, int maxLength)
        {
            InputDialog dialog = new InputDialog();
            dialog.inpdlgPromptLabel.Text = prompt;
            
            dialog.inpdlgBufTextBox.Text = defaultValue;
            dialog.inpdlgBufTextBox.SelectionStart = 0;
            dialog.inpdlgBufTextBox.SelectionLength = defaultValue.Length;
            dialog.inpdlgBufTextBox.Select();
            dialog.inpdlgBufTextBox.MaxLength = maxLength;

            DialogResult result = dialog.ShowDialog();
            if (result == DialogResult.Cancel) return null;
            return dialog.inpdlgBufTextBox.Text;
        }
    }
}

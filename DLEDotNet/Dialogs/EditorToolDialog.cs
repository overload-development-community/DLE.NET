using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Dialogs
{
    public partial class EditorToolDialog : Form
    {
        public EditorToolDialog()
        {
            InitializeComponent();
            this.editgeo1Button.Click += (object s, EventArgs e) => { System.Diagnostics.Debug.WriteLine("[" + Environment.TickCount + "] 1"); };
        }

        private void EditorToolDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                this.Hide();
                e.Cancel = true;
            }
        }
    }
}

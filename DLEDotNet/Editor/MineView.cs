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
    public partial class MineView : UserControl
    {
        public MineView()
        {
            InitializeComponent();
        }

        internal EditorWindow Owner { get; set; }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            // custom rendering
        }

        private void MineView_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && Control.ModifierKeys == Keys.Shift)
            {
                Owner.mineViewContextMenuStrip.Show(this.PointToScreen(e.Location));
            }
        }
    }
}

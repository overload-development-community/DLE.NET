using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EditorUITest.Editor.Layouts.Vertical;

namespace EditorUITest.Editor.Layouts
{
    public partial class DiagnosticsTool : DLELayoutableUserControl
    {
        public DiagnosticsTool()
        {
            InitializeComponent();
        }

        internal override void InitializeLayoutInternal() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalDiagnosticsTool);
            }
            return null;
        }

        private void DiagnosticsTool_Load(object sender, EventArgs e)
        {
            diagStatsListView.ListViewItemSorter = null;
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Reactor editor tool.")]
    public partial class ReactorTool : DLELayoutableUserControl
    {
        public ReactorTool()
        {
            InitializeComponent();
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalReactorTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingReactorTool);
            }
            return null;
        }

        private void reactorAddtgtButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show(reactorTargetTextBox.Value.ToString());
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EditorUITest.Editor.Layouts.Floating;
using EditorUITest.Editor.Layouts.Vertical;

namespace EditorUITest.Editor.Layouts
{
    public partial class ReactorTool : DLELayoutableUserControl
    {
        public ReactorTool()
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

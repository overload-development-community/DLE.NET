using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;
using System.Windows.Forms;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Reactor editor tool.")]
    public partial class ReactorTool : DLELayoutableUserControl
    {
        public ReactorTool()
        {
            InitializeComponent();
            this.HelpPageName = "reactor";
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

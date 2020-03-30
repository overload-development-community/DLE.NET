using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The advanced object editing tool under the Object tool.")]
    public partial class ObjectAdvanced : DLELayoutableUserControl
    {
        public ObjectAdvanced()
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
                case LayoutOrientation.FLOATING:
                    return typeof(VerticalObjectAdvanced);
            }
            return null;
        }
    }
}

using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Light editor tool.")]
    public partial class LightsTool : DLELayoutableUserControl
    {
        public LightsTool()
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
                    return typeof(VerticalLightsTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingLightsTool);
            }
            return null;
        }
    }
}

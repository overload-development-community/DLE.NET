using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Trigger editor tool.")]
    public partial class TriggerTool : DLELayoutableUserControl
    {
        public TriggerTool()
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
                    return typeof(VerticalTriggerTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingTriggerTool);
            }
            return null;
        }
    }
}

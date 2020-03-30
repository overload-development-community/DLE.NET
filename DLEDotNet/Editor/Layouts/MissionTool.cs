using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Mission editor tool.")]
    public partial class MissionTool : DLELayoutableUserControl
    {
        public MissionTool()
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
                    return typeof(VerticalMissionTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingMissionTool);
            }
            return null;
        }
    }
}

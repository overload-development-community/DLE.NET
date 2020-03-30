using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The sound effect editor under the Effect tool.")]
    public partial class EffectSound : DLELayoutableUserControl
    {
        public EffectSound()
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
                    return typeof(VerticalEffectSound);
            }
            return null;
        }
    }
}

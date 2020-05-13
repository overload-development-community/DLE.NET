using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The texture alignment editor under the Texture tool.")]
    public partial class TextureAlignment : DLELayoutableUserControl
    {
        public TextureAlignment()
        {
            InitializeComponent();
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            return null;
        }
    }
}

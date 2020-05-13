using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The texture projection editor under the Texture tool.")]
    public partial class TextureProjection : DLELayoutableUserControl
    {
        public TextureProjection()
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

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
    [Description("The Texture editor tool.")]
    public partial class TextureTool : DLELayoutableUserControl
    {
        public TextureTool()
        {
            InitializeComponent();
            this.HelpPageName = "texture";
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalTextureTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingTextureTool);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindTabControlBacking(this.textureSubTabControl, PROP(s => s.ActiveTextureTab));
        }
    }
}

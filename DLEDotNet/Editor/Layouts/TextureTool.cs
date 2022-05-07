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
            binder.BindFloatTextBox(this.textureLight1TextBox, PROP(s => s.CurrentSelection.Side.UVL0.L), false);
            binder.BindFloatTextBox(this.textureLight2TextBox, PROP(s => s.CurrentSelection.Side.UVL1.L), false);
            binder.BindFloatTextBox(this.textureLight3TextBox, PROP(s => s.CurrentSelection.Side.UVL2.L), false);
            binder.BindFloatTextBox(this.textureLight4TextBox, PROP(s => s.CurrentSelection.Side.UVL3.L), false);
            binder.BindComboBox(this.texture1StComboBox, PROP(s => s.TextureList.Items),
                PROP(s => s.CurrentSelection.Side.PrimaryTexture));
            binder.BindComboBox(this.texture2NdComboBox, PROP(s => s.TextureList.Items),
                PROP(s => s.CurrentSelection.Side.SecondaryTexture));
        }

        internal void RefreshBindings(object sender, EventArgs e)
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.RefreshBind(this.texture1StComboBox);
            binder.RefreshBind(this.texture2NdComboBox);
        }
    }
}

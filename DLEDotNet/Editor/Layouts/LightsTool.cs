using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Editor.Layouts.Vertical;
using DLEDotNet.Editor.Layouts.Floating;

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

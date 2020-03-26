using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

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

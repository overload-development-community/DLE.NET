using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EditorUITest.Editor.Layouts.Floating;
using EditorUITest.Editor.Layouts.Vertical;

namespace EditorUITest.Editor.Layouts
{
    public partial class TextureTab : DLELayoutableUserControl
    {
        public TextureTab()
        {
            InitializeLayout();
        }

        public void InitializeLayout()
        {
            this.Controls.Clear();
            InitializeComponent();
            switch (this.DialogLayout)
            {
                case LayoutOrientation.FLOATING:
                    this.CopyLayout(typeof(FloatingTextureTab));
                    break;
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    this.CopyLayout(typeof(VerticalTextureTab));
                    break;
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        public override void SelfTest()
        {
            DoSelfTestFor(typeof(VerticalTextureTab), typeof(FloatingTextureTab));
        }
    }
}

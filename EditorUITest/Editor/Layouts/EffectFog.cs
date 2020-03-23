using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EditorUITest.Util;
using EditorUITest.Editor.Layouts.Vertical;

namespace EditorUITest.Editor.Layouts
{
    public partial class EffectFog : DLELayoutableUserControl
    {
        public EffectFog()
        {
            InitializeLayout();
        }

        public void InitializeLayout()
        {
            this.Controls.Clear();
            InitializeComponent();
            switch (this.DialogLayout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                case LayoutOrientation.FLOATING:
                    this.CopyLayout(typeof(VerticalEffectFog));
                    break;
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        public override void SelfTest()
        {
            DoSelfTestFor(typeof(VerticalEffectFog));
        }
    }
}

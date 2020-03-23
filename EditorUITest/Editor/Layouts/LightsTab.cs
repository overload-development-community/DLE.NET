using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EditorUITest.Editor.Layouts.Vertical;
using EditorUITest.Editor.Layouts.Floating;

namespace EditorUITest.Editor.Layouts
{
    public partial class LightsTab : DLELayoutableUserControl
    {
        public LightsTab()
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
                    this.CopyLayout(typeof(VerticalLightsTab));
                    break;
                case LayoutOrientation.FLOATING:
                    this.CopyLayout(typeof(FloatingLightsTab));
                    break;
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        public override void SelfTest()
        {
            DoSelfTestFor(typeof(VerticalLightsTab), typeof(FloatingLightsTab));
        }
    }
}

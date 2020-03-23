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
    public partial class ReactorTab : DLELayoutableUserControl
    {
        public ReactorTab()
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
                    this.CopyLayout(typeof(VerticalReactorTab));
                    break;
                case LayoutOrientation.FLOATING:
                    this.CopyLayout(typeof(FloatingReactorTab));
                    break;
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        public override void SelfTest()
        {
            DoSelfTestFor(typeof(VerticalReactorTab), typeof(FloatingReactorTab));
        }

        private void reactorAddtgtButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show(reactorTargetTextBox.Value.ToString());
        }
    }
}

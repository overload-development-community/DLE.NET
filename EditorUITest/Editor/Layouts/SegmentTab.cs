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

namespace EditorUITest.Editor.Layouts
{
    public partial class SegmentTab : DLELayoutableUserControl
    {
        public SegmentTab()
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
                    this.CopyLayout(typeof(VerticalSegmentTab));
                    break;
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        public override void SelfTest()
        {
            DoSelfTestFor(typeof(VerticalSegmentTab));
        }

        private void SegmentTab_Load(object sender, EventArgs e)
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindIntTextBox(this.segmentEnergyDamageTextBox, "Temp", false);
        }

        private void segmentAddButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("you clicked add segment!\nthis is an example of a layout-agnostic event handler.");
        }
    }
}

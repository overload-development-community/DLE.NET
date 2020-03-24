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
    public partial class SegmentTool : DLELayoutableUserControl
    {
        public SegmentTool()
        {
            InitializeComponent();
        }

        internal override void InitializeLayoutInternal() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalSegmentTool);
            }
            return null;
        }

        private void SegmentTool_Load(object sender, EventArgs e)
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindIntTextBox(this.segmentEnergyDamageTextBox, "Temp", false);
            binder.BindComboBox(this.segmentIdComboBox, "Level.SegmentList", "Segment");
            binder.BindFloatTextBox(this.segmentLightTextBox, "Segment.Light", false);
        }

        private void segmentAddButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("you clicked add segment!\nthis is an example of a layout-agnostic event handler.");
        }
    }
}

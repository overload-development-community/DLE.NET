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
using LibDescent.Data;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Segment editor tool.")]
    public partial class SegmentTool : DLELayoutableUserControl
    {
        public SegmentTool()
        {
            InitializeComponent();
            this.HelpPageName = "segments";
        }

        internal override void ResetLayout() => InitializeComponent();

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

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindComboBox(this.segmentIdComboBox,
                PROP(s => s.SegmentManager.Segments), PROP(s => s.CurrentSelection.Segment));
            binder.BindComboBox<SegFunction>(this.segmentFunctionComboBox,
                PROP(s => s.CurrentSelection.Segment.Function));
            binder.BindRadioButton(this.segmentSide1RadioButton, PROP(s => s.CurrentSelection.SideNum), 0);
            binder.BindRadioButton(this.segmentSide2RadioButton, PROP(s => s.CurrentSelection.SideNum), 1);
            binder.BindRadioButton(this.segmentSide3RadioButton, PROP(s => s.CurrentSelection.SideNum), 2);
            binder.BindRadioButton(this.segmentSide4RadioButton, PROP(s => s.CurrentSelection.SideNum), 3);
            binder.BindRadioButton(this.segmentSide5RadioButton, PROP(s => s.CurrentSelection.SideNum), 4);
            binder.BindRadioButton(this.segmentSide6RadioButton, PROP(s => s.CurrentSelection.SideNum), 5);
            binder.BindRadioButton(this.segmentPoint1RadioButton, PROP(s => s.CurrentSelection.PointNum), 0);
            binder.BindRadioButton(this.segmentPoint2RadioButton, PROP(s => s.CurrentSelection.PointNum), 1);
            binder.BindRadioButton(this.segmentPoint3RadioButton, PROP(s => s.CurrentSelection.PointNum), 2);
            binder.BindRadioButton(this.segmentPoint4RadioButton, PROP(s => s.CurrentSelection.PointNum), 3);
            binder.BindFloatTextBox(this.segmentPointxTextBox, PROP(s => s.CurrentSelection.Point.X), false);
            binder.BindFloatTextBox(this.segmentPointyTextBox, PROP(s => s.CurrentSelection.Point.Y), false);
            binder.BindFloatTextBox(this.segmentPointzTextBox, PROP(s => s.CurrentSelection.Point.Z), false);
        }

        private void segmentAddButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("you clicked add segment!\nthis is an example of a layout-agnostic event handler.");
        }
    }
}

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
                PROP(s => s.Level.Segments.Items), PROP(s => s.CurrentSelection.Segment));
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
            binder.BindFloatTextBox(this.segmentLightTextBox, PROP(s => s.CurrentSelection.Segment.Light), false);

            // This is more or less a cop-out, the bindings should update automatically
            // but we need to figure out how to get that to happen with property trees
            this.segmentIdComboBox.SelectedIndexChanged += RefreshBindings;
            this.segmentSide1RadioButton.CheckedChanged += RefreshBindings;
            this.segmentSide2RadioButton.CheckedChanged += RefreshBindings;
            this.segmentSide3RadioButton.CheckedChanged += RefreshBindings;
            this.segmentSide4RadioButton.CheckedChanged += RefreshBindings;
            this.segmentSide5RadioButton.CheckedChanged += RefreshBindings;
            this.segmentSide6RadioButton.CheckedChanged += RefreshBindings;
            this.segmentPoint1RadioButton.CheckedChanged += RefreshBindings;
            this.segmentPoint2RadioButton.CheckedChanged += RefreshBindings;
            this.segmentPoint3RadioButton.CheckedChanged += RefreshBindings;
            this.segmentPoint4RadioButton.CheckedChanged += RefreshBindings;
        }

        private void RefreshBindings(object sender, EventArgs e)
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.RefreshBind(this.segmentIdComboBox);
            binder.RefreshBind(this.segmentFunctionComboBox);
            if (sender != this.segmentSide1RadioButton && sender != this.segmentSide2RadioButton &&
                sender != this.segmentSide3RadioButton && sender != this.segmentSide4RadioButton &&
                sender != this.segmentSide5RadioButton && sender != this.segmentSide6RadioButton)
            {
                binder.RefreshBind(this.segmentSide1RadioButton);
                binder.RefreshBind(this.segmentSide2RadioButton);
                binder.RefreshBind(this.segmentSide3RadioButton);
                binder.RefreshBind(this.segmentSide4RadioButton);
                binder.RefreshBind(this.segmentSide5RadioButton);
                binder.RefreshBind(this.segmentSide6RadioButton);
            }
            if (sender != this.segmentPoint1RadioButton && sender != this.segmentPoint2RadioButton &&
                sender != this.segmentPoint3RadioButton && sender != this.segmentPoint4RadioButton)
            {
                binder.RefreshBind(this.segmentPoint1RadioButton);
                binder.RefreshBind(this.segmentPoint2RadioButton);
                binder.RefreshBind(this.segmentPoint3RadioButton);
                binder.RefreshBind(this.segmentPoint4RadioButton);
            }
            binder.RefreshBind(this.segmentPointxTextBox);
            binder.RefreshBind(this.segmentPointyTextBox);
            binder.RefreshBind(this.segmentPointzTextBox);
            binder.RefreshBind(this.segmentLightTextBox);
        }

        private void segmentAddButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("you clicked add segment!\nthis is an example of a layout-agnostic event handler.");
        }
    }
}

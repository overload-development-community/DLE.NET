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
using DLEDotNet.Data;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The renderer preferences under the Settings tool.")]
    public partial class PreferencesRenderer : DLELayoutableUserControl
    {
        public PreferencesRenderer()
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
                case LayoutOrientation.FLOATING:
                    return typeof(VerticalPreferencesRenderer);
            }
            return null;
        }

        private void prefsViewdistTrackBar_ValueChanged(object sender, EventArgs e)
        {
            int newValue = prefsViewdistTrackBar.Value;
            if (newValue == 0)
                prefsViewdistTextLabel.Text = "all";
            else
                prefsViewdistTextLabel.Text = newValue.ToString();
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            prefsViewdistTrackBar.Maximum = EditorSettings.MaximumMineRenderDepth;

            binder.BindRadioButton(this.prefsRenderer3RdPersonRadioButton, PROP(s => s.Prefs.Perspective), PerspectiveMode.ThirdPerson);
            binder.BindRadioButton(this.prefsRenderer3RdPersonRadioButton, PROP(s => s.Prefs.Perspective), PerspectiveMode.FirstPerson);
            binder.BindRadioButton(this.prefsDepthOffRadioButton, PROP(s => s.Prefs.DepthPerception), DetailLevel.None);
            binder.BindRadioButton(this.prefsDepthLowRadioButton, PROP(s => s.Prefs.DepthPerception), DetailLevel.Low);
            binder.BindRadioButton(this.prefsDepthMediumRadioButton, PROP(s => s.Prefs.DepthPerception), DetailLevel.Medium);
            binder.BindRadioButton(this.prefsDepthHighRadioButton, PROP(s => s.Prefs.DepthPerception), DetailLevel.High);
            binder.BindComboBox<MineCenterDisplayShape>(this.prefsMinecenterComboBox, PROP(s => s.Prefs.MineCenterDisplay));
            binder.BindTrackBar(this.prefsViewdistTrackBar, PROP(s => s.Prefs.ViewDepth));
        }
    }
}

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
    [Description("The editor preferences under the Settings tool.")]
    public partial class PreferencesEditor : DLELayoutableUserControl
    {
        public PreferencesEditor()
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
                    return typeof(VerticalPreferencesEditor);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindFloatTextBox(this.prefsMoverateTextBox, PROP(s => s.Prefs.MoveRate), false);
            binder.BindFloatTextBox(this.prefsViewMoverateTextBox, PROP(s => s.Prefs.ViewRate), false);
            binder.BindIntTextBox(this.prefsUndoTextBox, PROP(s => s.Prefs.UndoCount), false);
            binder.BindTrackBar(this.prefsRotateRateSliderTrackBar, PROP(s => s.Prefs.RotateRateIndex));
            binder.BindLabel(this.tPrefsRotateRateLabel, PROP(s => s.Prefs.RotateRate));
            binder.BindCheckBox(this.prefsAllowobjectoverlapCheckBox, PROP(s => s.Prefs.AllowObjectOverlap), false);
            binder.BindCheckBox(this.prefsUpdatetexalignCheckBox, PROP(s => s.Prefs.UpdateTextureAlignment), false);

            this.labelUndos0Hyph500.Text = this.labelUndos0Hyph500.Text.Replace("#", EditorSettings.MaximumUndoCount.ToString());
            this.prefsUndoTextBox.MaximumValue = EditorSettings.MaximumUndoCount;
        }
    }
}

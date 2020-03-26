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

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The layout preferences under the Settings tool.")]
    public partial class PreferencesLayout : DLELayoutableUserControl
    {
        public PreferencesLayout()
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
                    return typeof(VerticalPreferencesLayout);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindRadioButton(this.prefsLayout0RadioButton, "SettingsCandidate.ActiveLayout", LayoutOrientation.HORIZONTAL, false);
            binder.BindRadioButton(this.prefsLayout1RadioButton, "SettingsCandidate.ActiveLayout", LayoutOrientation.VERTICAL, false);
            binder.BindRadioButton(this.prefsLayout2RadioButton, "SettingsCandidate.ActiveLayout", LayoutOrientation.FLOATING, false);
            binder.BindRadioButton(this.prefsLayout3RadioButton, "SettingsCandidate.ActiveLayout", LayoutOrientation.VERTICAL_SS, false);
        }
    }
}

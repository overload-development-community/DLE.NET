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
            binder.BindRadioButton(this.prefsLayout0RadioButton, PROP(s => s.Prefs.ActiveLayout), LayoutOrientation.HORIZONTAL);
            binder.BindRadioButton(this.prefsLayout1RadioButton, PROP(s => s.Prefs.ActiveLayout), LayoutOrientation.VERTICAL);
            binder.BindRadioButton(this.prefsLayout2RadioButton, PROP(s => s.Prefs.ActiveLayout), LayoutOrientation.FLOATING);
            binder.BindRadioButton(this.prefsLayout3RadioButton, PROP(s => s.Prefs.ActiveLayout), LayoutOrientation.VERTICAL_SS);
        }
    }
}

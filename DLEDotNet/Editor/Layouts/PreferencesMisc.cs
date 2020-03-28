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
    [Description("The miscellaneous preferences under the Settings tool.")]
    public partial class PreferencesMisc : DLELayoutableUserControl
    {
        public PreferencesMisc()
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
                    return typeof(VerticalPreferencesMisc);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindStringTextBox(this.prefsPlayerTextBox, PROP(s => s.Prefs.PlayerProfile), false);
            binder.BindCheckBox(this.prefsUsetexcolorsCheckBox, PROP(s => s.UseTexColors), false);
            binder.BindCheckBox(this.prefsExpertmodeCheckBox, PROP(s => s.Prefs.ExpertMode), false);
            binder.BindCheckBox(this.prefsSplashscreenCheckBox, PROP(s => s.Prefs.ShowSplash), false);
        }
    }
}

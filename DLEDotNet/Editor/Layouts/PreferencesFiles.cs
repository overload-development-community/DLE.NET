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
    [Description("The file preferences under the Settings tool.")]
    public partial class PreferencesFiles : DLELayoutableUserControl
    {
        public PreferencesFiles()
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
                    return typeof(VerticalPreferencesFiles);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindStringTextBox(this.prefsPathD1PigTextBox, PROP(s => s.Prefs.D1PIGPath), false);
            binder.BindStringTextBox(this.prefsPathD2PigTextBox, PROP(s => s.Prefs.D2PIGPath), false);
            binder.BindStringTextBox(this.prefsPathMissionsTextBox, PROP(s => s.Prefs.MissionPath), false);
        }
    }
}

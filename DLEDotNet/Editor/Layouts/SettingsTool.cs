using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Settings editor tool.")]
    public partial class SettingsTool : DLELayoutableUserControl
    {
        public SettingsTool()
        {
            InitializeComponent();
            this.HelpPageName = "settings";
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalSettingsTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingSettingsTool);
            }
            return null;
        }

        private void idokButton_Click(object sender, EventArgs e)
        {
            EditorState.SavedPrefs.CopyFrom(EditorState.Prefs);
            EditorState.SavedPrefs.SaveToFile();
        }
        private void idcancelButton_Click(object sender, EventArgs e) => EditorState.Prefs.CopyFrom(EditorState.SavedPrefs);

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindTabControlBacking(this.toolTabTabControl, PROP(s => s.ActiveSettingsTab));
        }
    }
}

using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;
using System.Windows.Forms;

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
            binder.BindStringTextBox(this.prefsPathMissionsTextBox, PROP(s => s.Prefs.LevelsPath), false);
        }

        private void prefsBrowseD1PigButton_Click(object sender, EventArgs e)
        {
            var dialog = new OpenFileDialog();
            dialog.Filter = "Descent PIG files (*.pig)|*.pig";
            dialog.FileName = "descent.pig";
            var result = dialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                EditorState.Prefs.D1PIGPath = dialog.FileName;
            }
        }

        private void prefsBrowseD2PigButton_Click(object sender, EventArgs e)
        {
            var dialog = new OpenFileDialog();
            dialog.Filter = "Descent II PIG files (*.pig)|*.pig";
            dialog.FileName = "groupa.pig";
            var result = dialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                EditorState.Prefs.D2PIGPath = dialog.FileName;
            }
        }

        private void prefsBrowseMissionsButton_Click(object sender, EventArgs e)
        {
            var dialog = new FolderBrowserDialog();
            var result = dialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                EditorState.Prefs.LevelsPath = dialog.SelectedPath;
            }
        }
    }
}

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
    [Description("The miscellaneous preferences under the Settings tool.")]
    public partial class PreferencesMisc : DLELayoutableUserControl
    {
        private OpenFileDialog xmlOpenFileDialog;
        private SaveFileDialog xmlSaveFileDialog;
        private OpenFileDialog iniOpenFileDialog;

        public PreferencesMisc()
        {
            InitializeComponent();

            xmlOpenFileDialog = new OpenFileDialog();
            xmlSaveFileDialog = new SaveFileDialog();
            iniOpenFileDialog = new OpenFileDialog();

            xmlOpenFileDialog.Filter = xmlSaveFileDialog.Filter = "configuration (*.xml)|*.xml|all files (*.*)|*.*";
            iniOpenFileDialog.Filter = "DLE.ini (*.ini)|*.ini";
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
            binder.BindCheckBox(this.prefsUsetexcolorsCheckBox, PROP(s => s.Toggles.UseTexColors), false);
            binder.BindCheckBox(this.prefsExpertmodeCheckBox, PROP(s => s.Prefs.ExpertMode), false);
            binder.BindCheckBox(this.prefsSplashscreenCheckBox, PROP(s => s.Prefs.ShowSplash), false);
        }

        private void buttonImportXML_Click(object sender, EventArgs e)
        {
            var result = xmlOpenFileDialog.ShowDialog(this);
            if (result == DialogResult.OK)
            {
                var tempSettings = new EditorSettingsSaved();
                if (!tempSettings.ReloadFromFile(xmlOpenFileDialog.FileName))
                {
                    MessageBox.Show(this,
                        "The selected file is invalid.",
                        "Error",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                    return;
                }

                if (MessageBox.Show(this, 
                    "Are you sure you want to replace your existing settings with the imported settings?\nImporting from: " + xmlOpenFileDialog.FileName,
                    "Confirm",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning) == DialogResult.Yes)
                {
                    EditorState.SavedPrefs.CopyFrom(EditorState.DefaultPrefs);
                    EditorState.SavedPrefs.CopyFrom(tempSettings);
                    EditorState.Prefs.CopyFrom(EditorState.SavedPrefs);

                    MessageBox.Show(this,
                        "Settings have been successfully imported.",
                        "Info",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Information);
                }
            }
        }

        private void buttonExportXML_Click(object sender, EventArgs e)
        {
            var result = xmlSaveFileDialog.ShowDialog(this);
            if (result == DialogResult.OK)
            {
                var tempSettings = new EditorSettingsSaved();
                tempSettings.CopyFrom(EditorState.SavedPrefs);
                tempSettings.CopyFrom(EditorState.Prefs);
                tempSettings.RecentFiles.Clear(); // don't save recent files
                tempSettings.SaveToFile(xmlSaveFileDialog.FileName);
                if (!EditorState.SavedPrefs.ExpertMode)
                {
                    MessageBox.Show(this,
                        "Settings have been successfully exported.",
                        "Info",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Information);
                }
            }
        }

        private void buttonImportINI_Click(object sender, EventArgs e)
        {
            var result = iniOpenFileDialog.ShowDialog(this);
            if (result == DialogResult.OK)
            {
                var tempSettings = new EditorSettingsSaved();
                if (!tempSettings.LoadFromINI(iniOpenFileDialog.FileName))
                {
                    MessageBox.Show(this,
                        "The selected file is invalid.",
                        "Error",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                    return;
                }

                if (MessageBox.Show(this,
                    "Are you sure you want to replace your existing settings with the imported settings?\nImporting from: " + iniOpenFileDialog.FileName,
                    "Confirm",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Warning) == DialogResult.Yes)
                {
                    EditorState.SavedPrefs.CopyFrom(EditorState.DefaultPrefs);
                    EditorState.SavedPrefs.CopyFrom(tempSettings);
                    EditorState.Prefs.CopyFrom(EditorState.SavedPrefs);

                    MessageBox.Show(this,
                        "Settings have been successfully imported.",
                        "Info",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Information);
                }
            }
        }

        private void buttonResetDefaults_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(this,
                "This will replace your settings with the defaults.",
                "Confirm",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Warning) == DialogResult.Yes)
            {
                EditorState.SavedPrefs.CopyFrom(EditorState.DefaultPrefs);
                EditorState.Prefs.CopyFrom(EditorState.SavedPrefs);
                MessageBox.Show(this,
                    "Default settings have been restored.",
                    "Info",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Information);
            }
        }
    }
}

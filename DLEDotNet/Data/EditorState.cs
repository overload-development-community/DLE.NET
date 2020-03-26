using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DLEDotNet.Data
{
    public class EditorState : ChangeableState
    {
        public event PropertyChangeEventHandler SettingChanged;

        public EditorState(object owner) : base()
        {
            // default values should be inited here, not in the property or variable
            Owner = owner;
            Settings = new EditorSettings();
            Settings.PropertyChanged += (object sender, PropertyChangeEventArgs e) => SettingChanged?.Invoke(this, e);
            SettingsCandidate = new EditorSettings();
            ActiveEditorTab = ActiveTextureTab = ActiveObjectTab = ActiveSettingsTab = 0;
            this.SegmentAddMode = SegmentAddMode.Normal;
            this.SelectionMode = SelectMode.Side;

            Temp = 1337;
        }

        /// <summary>
        /// The object that owns this EditorState. In the Windows Forms
        /// interface, this is the EditorWindow.
        /// </summary>
        public object Owner { get; }

        /// <summary>
        /// The current editor settings.
        /// </summary>
        [NoSubstateAutoSubscribe]
        public EditorSettings Settings
        {
            get => _editorSettings;
            private set
            {
                if (AssignChanged(ref _editorSettings, value))
                    _editorSettings.PropertyChanged += (object sender, PropertyChangeEventArgs e) => this.SettingChanged?.Invoke(sender, e);
            }
        }

        /// <summary>
        /// The candidate editor settings; that is, the settings live
        /// as they are being edited under the Settings tool.
        /// </summary>
        public EditorSettings SettingsCandidate
        {
            get => _editorSettingsCandidate;
            private set => AssignChanged(ref _editorSettingsCandidate, value);
        }

        /// <summary>
        /// The tab index of the currently active tab of the editor tool tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveEditorTab
        {
            get => _activeEditorTab;
            set => AssignChanged(ref _activeEditorTab, value);
        }

        /// <summary>
        /// The tab index of the currently active tab of the texture tool tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveTextureTab
        {
            get => _activeTextureTab;
            set => AssignChanged(ref _activeTextureTab, value);
        }

        /// <summary>
        /// The tab index of the currently active tab of the object tool tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveObjectTab
        {
            get => _activeObjectTab;
            set => AssignChanged(ref _activeObjectTab, value);
        }

        /// <summary>
        /// The tab index of the currently active tab of the setting tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveSettingsTab
        {
            get => _activeSettingsTab;
            set => AssignChanged(ref _activeSettingsTab, value);
        }

        public SegmentAddMode SegmentAddMode
        {
            get => _segAddMode;
            set => AssignChanged(ref _segAddMode, value);
        }

        public SelectMode SelectionMode
        {
            get => _selectMode;
            set => AssignChanged(ref _selectMode, value);
        }

        private EditorSettings _editorSettings;
        private EditorSettings _editorSettingsCandidate;
        private int _activeEditorTab;
        private int _activeTextureTab;
        private int _activeObjectTab;
        private int _activeSettingsTab;
        private SegmentAddMode _segAddMode;
        private SelectMode _selectMode;

        public uint Temp
        {
            get => _tmp;
            set
            {
                if (AssignChanged(ref _tmp, value))
                    System.Diagnostics.Debug.WriteLine("New value for _tmp = " + value);
            }
        }

        private uint _tmp;
    }
}

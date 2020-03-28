using System;
using System.Collections.Generic;
using System.ComponentModel;
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
            SavedPrefs = new EditorSettingsSaved();
            SavedPrefs.PropertyChanged += (object sender, PropertyChangeEventArgs e) => SettingChanged?.Invoke(this, e);
            Prefs = new EditorSettings();
            ActiveEditorTab = ActiveTextureTab = ActiveObjectTab = ActiveSettingsTab = 0;
            SegmentAddMode = SegmentAddMode.Normal;
            SelectionMode = SelectMode.Side;
            Unsaved = false;
            UseTexColors = false;
            FullScreen = false;
        }

        /// <summary>
        /// The object that owns this EditorState. In the Windows Forms
        /// interface, this is the EditorWindow.
        /// </summary>
        public object Owner { get; }

        /// <summary>
        /// The saved editor settings.
        /// </summary>
        public EditorSettingsSaved SavedPrefs
        {
            get => _editorSettings;
            private set
            {
                if (AssignChanged(ref _editorSettings, value))
                    _editorSettings.PropertyChanged += (object sender, PropertyChangeEventArgs e) => this.SettingChanged?.Invoke(sender, e);
            }
        }

        /// <summary>
        /// The live editor settings, as they are being edited under the
        /// Settings tool or through some menu options.
        /// </summary>
        public EditorSettings Prefs
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

        public bool Unsaved
        {
            get => _unsaved;
            set => AssignChanged(ref _unsaved, value);
        }

        public bool UseTexColors
        {
            get => _useTexColors;
            set => AssignChanged(ref _useTexColors, value);
        }

        public bool FullScreen
        {
            get => _fullScreen;
            set => AssignChanged(ref _fullScreen, value);
        }

        private EditorSettingsSaved _editorSettings;
        private EditorSettings _editorSettingsCandidate;
        private int _activeEditorTab;
        private int _activeTextureTab;
        private int _activeObjectTab;
        private int _activeSettingsTab;
        private SegmentAddMode _segAddMode;
        private SelectMode _selectMode;
        private bool _unsaved;
        private bool _useTexColors;
        private bool _fullScreen;

        /// <summary>
        /// Causes all property change events to be suppressed until the next
        /// ResumeStateEvents call.
        /// </summary>
        public void PauseEditorStateEvents()
        {
            this.PauseStateEvents();
        }

        /// <summary>
        /// Unpauses property change events paused by the previous call to
        /// PauseStateEvents, and raises events for all properties that have
        /// been changed during the time the events were paused.
        /// </summary>
        public void ResumeEditorStateEvents()
        {
            this.ResumeStateEvents();
        }
    }
}

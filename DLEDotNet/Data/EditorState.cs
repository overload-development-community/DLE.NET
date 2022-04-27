using DLEDotNet.Data.Proxies;
using LibDescent.Edit;
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
            DefaultPrefs = new EditorSettingsSaved();
            ActiveEditorTab = ActiveTextureTab = ActiveObjectTab = ActiveSettingsTab = 0;
            SegmentAddMode = SegmentAddMode.Normal;
            SelectionMode = SelectMode.Side;
            ViewMode = RenderDisplayMode.TextureOnly;
            Toggles = new EditorToggles();
            FilePath = null;
            LevelFileName = null;
            Unsaved = false;
            Unsafe = new UnsafeEditorStateMethods(this);
            // ManagerProxyBinder must be created before any classes from DLE.ManagedWrappers
            ManagerProxyBinder = new ManagerProxyBinder(this);
            Level = LevelProxy.New();
            CurrentSelection = new Selection(this, 0, 4, 0);
            OtherSelection = new Selection(this, 0, 4, 0);
            TextureList = Subscribe(nameof(TextureList), new TextureListProxy());

            // debug
            this.PropertyChanged += (s, e) => System.Diagnostics.Debug.WriteLine("Changed property " + e.PropertyName);
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
        private EditorSettingsSaved _editorSettings;

        /// <summary>
        /// The live editor settings, as they are being edited under the
        /// Settings tool or through some menu options.
        /// </summary>
        public EditorSettings Prefs
        {
            get => _editorSettingsCandidate;
            private set => AssignChanged(ref _editorSettingsCandidate, value);
        }
        private EditorSettings _editorSettingsCandidate;

        internal EditorSettingsSaved DefaultPrefs
        {
            get => _editorSettingsDefault;
            private set => AssignChanged(ref _editorSettingsDefault, value);
        }
        private EditorSettingsSaved _editorSettingsDefault;

        /// <summary>
        /// Contains various user-toggleable settings. Note that settings
        /// which can be saved or stored permanently should be in Prefs or
        /// SavedPrefs instead.
        /// </summary>
        public EditorToggles Toggles
        {
            get => _editorToggles;
            private set => AssignChanged(ref _editorToggles, value);
        }
        private EditorToggles _editorToggles;

        /// <summary>
        /// The tab index of the currently active tab of the editor tool tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveEditorTab
        {
            get => _activeEditorTab;
            set => AssignChanged(ref _activeEditorTab, value);
        }
        private int _activeEditorTab;

        /// <summary>
        /// The tab index of the currently active tab of the texture tool tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveTextureTab
        {
            get => _activeTextureTab;
            set => AssignChanged(ref _activeTextureTab, value);
        }
        private int _activeTextureTab;

        /// <summary>
        /// The tab index of the currently active tab of the object tool tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveObjectTab
        {
            get => _activeObjectTab;
            set => AssignChanged(ref _activeObjectTab, value);
        }
        private int _activeObjectTab;

        /// <summary>
        /// The tab index of the currently active tab of the setting tabs.
        /// Used as backing in case the tab control reloads.
        /// </summary>
        public int ActiveSettingsTab
        {
            get => _activeSettingsTab;
            set => AssignChanged(ref _activeSettingsTab, value);
        }
        private int _activeSettingsTab;

        /// <summary>
        /// The current segment addition mode.
        /// </summary>
        public SegmentAddMode SegmentAddMode
        {
            get => _segAddMode;
            set => AssignChanged(ref _segAddMode, value);
        }
        private SegmentAddMode _segAddMode;

        /// <summary>
        /// The current selection mode (points, lines, sides, segments, objects or blocks).
        /// </summary>
        public SelectMode SelectionMode
        {
            get => _selectMode;
            set => AssignChanged(ref _selectMode, value);
        }
        private SelectMode _selectMode;

        /// <summary>
        /// The current display mode (wireframe, sparse wireframe, textured wireframe, or textured).
        /// </summary>
        public RenderDisplayMode ViewMode
        {
            get => _viewMode;
            set => AssignChanged(ref _viewMode, value);
        }
        private RenderDisplayMode _viewMode;

        /// <summary>
        /// Whether the current file is unsaved (needs saving on disk), or in the case
        /// that FilePath is null (new file), whether the user has modified the level at all.
        /// </summary>
        public bool Unsaved
        {
            get => _unsaved;
            set => AssignChanged(ref _unsaved, value);
        }
        private bool _unsaved;

        /// <summary>
        /// The currently opened file as a full path, or null if a new file.
        /// </summary>
        public string FilePath
        {
            get => _filePath;
            set => AssignChanged(ref _filePath, value);
        }
        /// <summary>
        /// The currently opened file as a full name, or null if a new file.
        /// </summary>
        public string FileName
        {
            get => FilePath != null ? System.IO.Path.GetFileName(FilePath) : FilePath;
        }
        private string _filePath;

        /// <summary>
        /// The "inner" file name of the current level, if we have opened a HOG.
        /// Set to null if that is not the case.
        /// </summary>
        public string LevelFileName
        {
            get => _innerFileName;
            set => AssignChanged(ref _innerFileName, value);
        }
        private string _innerFileName;

        /// <summary>
        /// The currently loaded level.
        /// </summary>
        public LevelProxy Level
        {
            get => _level;
            set => AssignChanged(ref _level, value);
        }
        private LevelProxy _level;

        /// <summary>
        /// Represents the currently selected segment, side, edge, vertex, and object.
        /// </summary>
        public Selection CurrentSelection
        {
            get => _currentSelection;
            set => AssignChanged(ref _currentSelection, value);
        }
        private Selection _currentSelection;

        /// <summary>
        /// Represents the "other cube" that the user can switch to. This is swapped
        /// with the current selection when the user presses the corresponding hotkey.
        /// </summary>
        public Selection OtherSelection
        {
            get => _otherSelection;
            set => AssignChanged(ref _otherSelection, value);
        }
        private Selection _otherSelection;

        /// <summary>
        /// The list of textures available for use in the current level.
        /// Defined by the currently-loaded .PIG, with potential modifications by .POG/.DTX files.
        /// </summary>
        public TextureListProxy TextureList { get; }

        /// <summary>
        /// Returns whether a message box with the given minimum information level should be displayed,
        /// which depends on the information level set in the settings. "Quiet" should be used for
        /// important messages like errors, "Normal" for warnings/success messages or confirmation boxes
        /// with major effects and "Verbose" for the rest (guides, confirmations for minor effects, and
        /// so forth).
        /// </summary>
        /// <param name="level">The information level of the message box to (possibly) display.</param>
        /// <returns></returns>
        public bool ShouldDisplayMessage(InformationLevel level)
        {
            switch (Prefs.MessageLevel)
            {
                case InformationLevel.Quiet:
                    return level == InformationLevel.Quiet;
                case InformationLevel.Normal:
                    return level == InformationLevel.Quiet || level == InformationLevel.Normal;
                case InformationLevel.Verbose:
                    return true;
            }
            return true;
        }

        /// <summary>
        /// Contains methods that should not be used outside EditorStateBinder.
        /// </summary>
        internal UnsafeEditorStateMethods Unsafe { get; private set; } = null;

        [EditorBrowsable(EditorBrowsableState.Never)]
        internal class UnsafeEditorStateMethods
        {
            private EditorState state;

            internal UnsafeEditorStateMethods(EditorState state)
            {
                this.state = state;
            }

            /// <summary>
            /// Causes all property change events to be suppressed until the next
            /// ResumeStateEvents call.
            /// </summary>
            /// <seealso cref="DLEDotNet.Editor.EditorStateBinder.BatchChange()"/>
            internal void PauseEditorStateEvents()
            {
                state.PauseStateEvents();
            }

            /// <summary>
            /// Unpauses property change events paused by the previous call to
            /// PauseStateEvents, and raises events for all properties that have
            /// been changed during the time the events were paused.
            /// </summary>
            /// <seealso cref="DLEDotNet.Editor.EditorStateBinder.BatchChange()"/>
            internal void ResumeEditorStateEvents()
            {
                state.ResumeStateEvents();
            }
        }

        /// <summary>
        /// Allows code in DLE.ManagerProxies to access editor state.
        /// </summary>
        internal ManagerProxyBinder ManagerProxyBinder { get; }
    }
}

using LibDescent.Edit;

namespace DLEDotNet.Data
{
    public class EditorToggles : ChangeableState
    {
        public EditorToggles() : base()
        {
            UseTexColors = true;
            FullScreen = false;
        }

        /// <summary>
        /// Whether to apply face light settings globally.
        /// </summary>
        public bool UseTexColors
        {
            get => _useTexColors;
            set => AssignChanged(ref _useTexColors, value);
        }

        /// <summary>
        /// Whether the editor is currently in full screen mode (tool view and texture list hidden).
        /// </summary>
        public bool FullScreen
        {
            get => _fullScreen;
            set => AssignChanged(ref _fullScreen, value);
        }

        private bool _useTexColors;
        private bool _fullScreen;
    }
}
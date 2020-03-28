using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace DLEDotNet.Data
{
    /// <summary>
    /// The settings that should only be present live/saved (such as texture filters; no
    /// applying required) should be here.
    /// </summary>
    public class EditorSettingsSaved : EditorSettings
    {
        #region --- Properties (with no extra logic, if possible)
        public bool EnableQuickSelection { get => _enableQuickSelection; set => AssignChanged(ref _enableQuickSelection, value); }
        public QuickSelectionCandidateMode QuickSelectionCandidates { get => _showSelectionCandidates; set => AssignChanged(ref _showSelectionCandidates, value); }
        public ElementMovementReferenceMode ElementMovementReference { get => _elementMovementReference; set => AssignChanged(ref _elementMovementReference, value); }
        [NoSettingTouch]
        public bool ElementMovementReferenceFormatted { 
            get => _elementMovementReference == ElementMovementReferenceMode.RelativeToCamera; 
            set => AssignChanged(ref _elementMovementReference, value ? ElementMovementReferenceMode.RelativeToCamera : ElementMovementReferenceMode.RelativeToCube);
        }
        public TextureFilters TextureFilters { get => _textureFilters; set => AssignChanged(ref _textureFilters, value); }
        public List<String> RecentFiles { get => _lastOpenedFiles; internal set => AssignChanged(ref _lastOpenedFiles, value); }
        #endregion

        #region --- Variables (including default values)
        private bool _enableQuickSelection = true;
        private QuickSelectionCandidateMode _showSelectionCandidates = QuickSelectionCandidateMode.Full;
        private ElementMovementReferenceMode _elementMovementReference = ElementMovementReferenceMode.RelativeToCube;
        private TextureFilters _textureFilters = TextureFilters.All;
        private List<String> _lastOpenedFiles = new List<string>();
        #endregion
    }
}

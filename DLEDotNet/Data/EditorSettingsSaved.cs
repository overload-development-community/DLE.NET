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
        public GeometryVisibilityFlags GeometryVisibility { get => _geometryVisibility; set => AssignChanged(ref _geometryVisibility, value); }
        public ObjectVisibilityFlags ObjectVisibility { get => _objectVisibility; set => AssignChanged(ref _objectVisibility, value); }
        public TextureVisibilityFlags TextureVisibility { get => _textureVisibility; set => AssignChanged(ref _textureVisibility, value); }
        public TextureFilters TextureFilters { get => _textureFilters; set => AssignChanged(ref _textureFilters, value); }
        [NoSettingCopy]
        public List<String> RecentFiles { get => _lastOpenedFiles; internal set => AssignChanged(ref _lastOpenedFiles, value); }
        public bool SortObjects { get => _sortObjects; set => AssignChanged(ref _sortObjects, value); }
        [NoSettingCopy]
        public bool AutoFixBugs { get => _autoFixBugs; set => AssignChanged(ref _autoFixBugs, value); }
        #endregion

        #region --- Variables (including default values)
        private bool _enableQuickSelection = true;
        private QuickSelectionCandidateMode _showSelectionCandidates = QuickSelectionCandidateMode.Full;
        private ElementMovementReferenceMode _elementMovementReference = ElementMovementReferenceMode.RelativeToCube;
        private GeometryVisibilityFlags _geometryVisibility = GeometryVisibilityFlags.Default;
        private ObjectVisibilityFlags _objectVisibility = ObjectVisibilityFlags.Default;
        private TextureVisibilityFlags _textureVisibility = TextureVisibilityFlags.Default;
        private TextureFilters _textureFilters = TextureFilters.All;
        private List<String> _lastOpenedFiles = new List<string>();
        private bool _sortObjects = false;
        private bool _autoFixBugs = true;
        #endregion

        #region --- INI import code

        public bool LoadFromINI(string fileName)
        {
            string[] lines;
            try
            {
                lines = System.IO.File.ReadAllLines(fileName);
            }
            catch (UnauthorizedAccessException) { return false; }
            catch (FileNotFoundException) { return false; }

            bool acceptLines = false;
            int i;
            double d;
            foreach (string line in lines)
            {
                if (line.StartsWith("[") && line.EndsWith("]"))
                {
                    // new section header
                    string sectionTitle = line.Substring(1, line.Length - 2);
                    acceptLines = sectionTitle.Equals("DLE", StringComparison.OrdinalIgnoreCase);
                }
                else if (acceptLines && line.Contains("="))
                {
                    string[] tok = line.Split(new char[] { '=' }, 2);
                    string key = tok[0].ToLowerInvariant(), value = tok[1];

                    switch (key) // lowercase
                    {
                        case "descentdirectory":
                            this.D1PIGPath = value; break;
                        case "descent2directory":
                            this.D2PIGPath = value; break;
                        case "levelsdirectory":
                            this.LevelsPath = value; break;
                        case "playerprofile":
                            this.PlayerProfile = value; break;
                        case "depthperception":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out DetailLevel e))
                                    this.DepthPerception = e;
                            break;
                        case "rotaterate":
                            if (int.TryParse(value, out i))
                                this.RotateRateIndex = i;
                            break;
                        case "moverate":
                            if (double.TryParse(value, out d))
                                this.MoveRate = d;
                            break;
                        case "viewmoverate":
                            if (double.TryParse(value, out d))
                                this.ViewRate = d;
                            break;
                        case "movementmode":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out CameraMovementMode e))
                                    this.MovementMode = e;
                            break;
                        case "movespeed":
                            if (double.TryParse(value, out d))
                                this.CameraMoveSpeed = d;
                            break;
                        case "turnspeed":
                            if (double.TryParse(value, out d))
                                this.CameraTurnSpeed = d;
                            break;
                        case "forcefirstpersononinputlock":
                            if (int.TryParse(value, out i))
                                this.ForceFirstPersonOnInputLock = i != 0;
                            break;
                        case "expertmode":
                            if (int.TryParse(value, out i))
                                this.MessageLevel = i != 0 ? InformationLevel.Normal : InformationLevel.Verbose;
                            break;
                        case "splashscreen":
                            if (int.TryParse(value, out i))
                                this.ShowSplash = i != 0;
                            break;
                        case "bumpobjects":
                            if (int.TryParse(value, out i))
                                this.BumpCoincident = i != 0;
                            break;
                        case "bumpincrement":
                            if (double.TryParse(value, out d))
                                this.BumpRate = d;
                            break;
                        case "updatetexalign":
                            if (int.TryParse(value, out i))
                                this.UpdateTextureAlignment = i != 0;
                            break;
                        case "mineviewflags":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out GeometryVisibilityFlags e))
                                    this.GeometryVisibility = e & GeometryVisibilityFlags.All;
                            break;
                        case "objviewflags":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out ObjectVisibilityFlags e))
                                    this.ObjectVisibility = e & ObjectVisibilityFlags.All;
                            break;
                        case "texviewflags":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out TextureVisibilityFlags e))
                                    this.TextureVisibility = e & TextureVisibilityFlags.All;
                            break;
                        case "texturefilter":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out TextureFilters e))
                                    this.TextureFilters = e & TextureFilters.All;
                            break;
                        case "sortobjects":
                            if (int.TryParse(value, out i))
                                this.SortObjects = i != 0;
                            break;
                        case "depthtest":
                            if (int.TryParse(value, out i))
                                this.DepthTest = i != 0;
                            break;
                        case "viewdistance":
                            if (int.TryParse(value, out i))
                                this.ViewDepth = i;
                            break;
                        case "perspective":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out PerspectiveMode e))
                                    this.Perspective = e;
                            break;
                        case "minecenter":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out MineCenterDisplayShape e))
                                    this.MineCenterDisplay = e;
                            break;
                        case "maxundo":
                            if (int.TryParse(value, out i))
                                this.UndoCount = i;
                            break;
                        case "enablequickselection":
                            if (int.TryParse(value, out i))
                                this.EnableQuickSelection = i != 0;
                            break;
                        case "showselectioncandidates":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out QuickSelectionCandidateMode e))
                                    this.QuickSelectionCandidates = e;
                            break;
                        case "elementmovementreference":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out ElementMovementReferenceMode e))
                                    this.ElementMovementReference = e;
                            break;
                        case "layout":
                            if (int.TryParse(value, out i))
                                if (Enum.TryParse(value, out LayoutOrientation e))
                                    this.ActiveLayout = e;
                            break;
                    }
                }
            }

            return true;
        }
        #endregion
    }
}

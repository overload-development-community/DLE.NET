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
    public class EditorSettings : ChangeableState
    {
        public const int MaximumUndoCount = 500;
        public const int MaximumMineRenderDepth = 60;
        public static readonly double[] RotateRates = { 1.40625, 2.8125, 5.625, 7.5, 11.25, 15, 22.5, 30, 45 };

        #region --- Properties (with no extra logic, if possible)
        public LayoutOrientation ActiveLayout { get => _layoutOrientation; set => AssignChanged(ref _layoutOrientation, value); }
        public string D1PIGPath { get => _d1PIGPath; set => AssignChanged(ref _d1PIGPath, value); }
        public string D2PIGPath { get => _d2PIGPath; set => AssignChanged(ref _d2PIGPath, value); }
        public string MissionPath { get => _MissionsPath; set => AssignChanged(ref _MissionsPath, value); }
        public RendererMode Renderer { get => _renderer; set => AssignChanged(ref _renderer, value); }
        public DetailLevel DepthPerception { get => _depthPrediction; set => AssignChanged(ref _depthPrediction, value); }
        public MineCenterDisplayShape MineCenterDisplay { get => _mineCenterDisplay; set => AssignChanged(ref _mineCenterDisplay, value); }
        public GeometryVisibilityFlags GeometryVisibility { get => _geometryVisibility; set => AssignChanged(ref _geometryVisibility, value); }
        public ObjectVisibilityFlags ObjectVisibility { get => _objectVisibility; set => AssignChanged(ref _objectVisibility, value); }
        public TextureVisibilityFlags TextureVisibility { get => _textureVisibility; set => AssignChanged(ref _textureVisibility, value); }
        public string PlayerProfile { get => _playerProfile; set => AssignChanged(ref _playerProfile, StringUtil.Truncate(value, 8)); }
        public int MineRenderDepth { get => _mineRenderDepth; set => AssignChanged(ref _mineRenderDepth, MathUtil.Clamp(value, 0, MaximumMineRenderDepth)); }
        public int UndoCount { get => _undoCount; set => AssignChanged(ref _undoCount, MathUtil.Clamp(value, 0, MaximumUndoCount)); }
        public int RotateRateIndex {
            get => _rotateRateIndex; 
            set 
            {
                AssignChanged(ref _rotateRateIndex, MathUtil.Clamp(value, 0, RotateRates.Length - 1));
                OnReadOnlyPropertyChanged(nameof(RotateRate), RotateRate);
            }
        }
        [NoSettingTouch]
        public Angle RotateRate { get => Angle.FromDegrees(RotateRates[RotateRateIndex]); }
        public double MoveRate { get => _moveRate; set => AssignChanged(ref _moveRate, value); }
        public double ViewRate { get => _viewRate; set => AssignChanged(ref _viewRate, value); }
        public double BumpRate { get => _bumpIncrement; set => AssignChanged(ref _bumpIncrement, value); }
        public bool AllowObjectOverlap { get => _allowObjectOverlap; set => AssignChanged(ref _allowObjectOverlap, value); }
        [NoSettingTouch]
        public bool BumpCoincident { get => !AllowObjectOverlap; set => AssignChanged(ref _allowObjectOverlap, !value); }
        public bool UpdateTextureAlignment { get => _adjustTextureAlignment; set => AssignChanged(ref _adjustTextureAlignment, value); }
        public bool ExpertMode { get => _expertMode; set => AssignChanged(ref _expertMode, value); }
        public bool ShowSplash { get => _splashScreen; set => AssignChanged(ref _splashScreen, value); }
        public bool DepthTest { get => _depthTest; set => AssignChanged(ref _depthTest, value); }
        public StartupWindowState StartupState { get => _startupWindowState; set => AssignChanged(ref _startupWindowState, value); }
        public CameraMovementMode MovementMode { get => _movementMode; set => AssignChanged(ref _movementMode, value); }
        public double CameraMoveSpeed { get => _cameraMoveSpeed; set => AssignChanged(ref _cameraMoveSpeed, value); }
        public double CameraTurnSpeed { get => _cameraTurnSpeed; set => AssignChanged(ref _cameraTurnSpeed, value); }
        public bool ForceFirstPersonOnInputLock { get => _forceFirstPersonOnInputLock; set => AssignChanged(ref _forceFirstPersonOnInputLock, value); }
        public bool EnableQuickSelection { get => _enableQuickSelection; set => AssignChanged(ref _enableQuickSelection, value); }
        public QuickSelectionCandidateMode QuickSelectionCandidates { get => _showSelectionCandidates; set => AssignChanged(ref _showSelectionCandidates, value); }
        public ElementMovementReferenceMode ElementMovementReference { get => _elementMovementReference; set => AssignChanged(ref _elementMovementReference, value); }
        public bool SortObjects { get => _sortObjects; set => AssignChanged(ref _sortObjects, value); }
        [NoSettingCopy]
        public bool AutoFixBugs { get => _autoFixBugs; set => AssignChanged(ref _autoFixBugs, value); }

        #endregion

        #region --- Variables (including default values)
        private LayoutOrientation _layoutOrientation = LayoutOrientation.HORIZONTAL;
        private string _d1PIGPath = "Enter a full path to a DESCENT.PIG here";
        private string _d2PIGPath = "Enter a full path to a Descent II .PIG here";
        private string _MissionsPath = "Enter a full path to your mission folder here";
        private RendererMode _renderer = RendererMode.ThirdPerson;
        private DetailLevel _depthPrediction = DetailLevel.High;
        private MineCenterDisplayShape _mineCenterDisplay = MineCenterDisplayShape.None;
        private int _mineRenderDepth = 0;
        private double _moveRate = 1;
        private double _viewRate = 1;
        private int _undoCount = MaximumUndoCount;
        private int _rotateRateIndex = 4;
        private bool _allowObjectOverlap = true;
        private double _bumpIncrement = 3.0;
        private bool _adjustTextureAlignment = true;
        private GeometryVisibilityFlags _geometryVisibility = GeometryVisibilityFlags.Default;
        private ObjectVisibilityFlags _objectVisibility = ObjectVisibilityFlags.Default;
        private TextureVisibilityFlags _textureVisibility = TextureVisibilityFlags.Default;
        private string _playerProfile = "";
        private bool _depthTest = true;
        private bool _expertMode = false;
        private bool _splashScreen = true;
        private StartupWindowState _startupWindowState = StartupWindowState.Windowed;
        private CameraMovementMode _movementMode = CameraMovementMode.Continuous;
        private double _cameraMoveSpeed = 50;
        private double _cameraTurnSpeed = 1;
        private bool _forceFirstPersonOnInputLock = true;
        private bool _enableQuickSelection = true;
        private QuickSelectionCandidateMode _showSelectionCandidates = QuickSelectionCandidateMode.Full;
        private ElementMovementReferenceMode _elementMovementReference = ElementMovementReferenceMode.RelativeToCube;
        private bool _sortObjects = false;
        private bool _autoFixBugs = true;
        #endregion

        #region --- Implementation: reloading, saving, copying settings
        private static readonly string SettingsPath = Path.Combine(Environment.CurrentDirectory, "DLE.xml");

        private dynamic ConvertChangeTypeOrDefault(string text, Type type, dynamic defaultValue)
        {
            try
            {
                return Convert.ChangeType(text, type) ?? defaultValue;
            }
            catch (InvalidCastException) { return defaultValue; }
            catch (FormatException) { return defaultValue; }
            catch (OverflowException) { return defaultValue; }
        }

        private LibDescent.Data.Fix StringToFix(string text, dynamic defaultValue)
        {
            try
            {
                return (LibDescent.Data.Fix)Convert.ToDouble(text);
            }
            catch (FormatException) { return defaultValue; }
            catch (OverflowException) { return defaultValue; }
        }

        // Enum.TryParse<TEnum>(string value, bool ignoreCase, out TEnum result)
        private static readonly MethodInfo enumTryParse = typeof(Enum).GetMethods(BindingFlags.Public | BindingFlags.Static)
            .Where(m => m.Name == "TryParse" 
                && m.GetGenericArguments().Length == 1
                && m.GetParameters().Length == 3 
                && m.GetParameters()[0].ParameterType == typeof(string) 
                && m.GetParameters()[1].ParameterType == typeof(bool) 
                && m.GetParameters()[2].IsOut)
            .First();
        public static bool EnumTryParseDynamic(Type type, string text, bool ignoreCase, out dynamic result)
        {
            MethodInfo enumTryParseT = enumTryParse.MakeGenericMethod(new Type[] { type });
            object[] parameters = { text, ignoreCase, null };
            bool d = (bool)enumTryParseT.Invoke(null, parameters);
            result = parameters[2];
            return d;
        }

        private dynamic ConvertFromString(Type propertyType, string text, dynamic defaultValue)
        {
            if (text == null)
            {
                return defaultValue;
            }
            else
            {
                if (typeof(string) == propertyType)
                    return text;
                else if (typeof(LibDescent.Data.Fix) == propertyType)
                    return StringToFix(text, defaultValue);
                else if (typeof(Enum).IsAssignableFrom(propertyType))
                    return EnumTryParseDynamic(propertyType, text, true, out dynamic result) ? result : defaultValue;
                else if (typeof(IConvertible).IsAssignableFrom(propertyType))
                    return ConvertChangeTypeOrDefault(text, propertyType, defaultValue);
                else
                    throw new ArgumentException("The property type is currently not supported by ConvertFromString.");
            }
        }

        private IEnumerable<PropertyInfo> GetProperties()
        {
            return this.GetType().GetProperties(BindingFlags.Public | BindingFlags.Instance).Where(p => p.CanWrite && !Attribute.IsDefined(p, typeof(NoSettingTouchAttribute)));
        }

        public void ReloadFromFile()
        {
            XmlDocument xmlDoc = new XmlDocument();
            try
            {
                xmlDoc.Load(SettingsPath);
            }
            catch (FileNotFoundException) { }
            catch (XmlException) { }
            
            foreach (PropertyInfo property in GetProperties())
            {
                XmlNode configNode = xmlDoc.SelectSingleNode("//DLEConfig/" + property.Name);
                dynamic defaultValue = property.GetValue(this);
                property.SetValue(this, ConvertFromString(property.PropertyType, configNode?.InnerText, defaultValue));
            }
        }

        public void SaveToFile()
        {
            XmlDocument xmlDoc = new XmlDocument();
            XmlNode rootNode = xmlDoc.CreateElement("DLEConfig");
            xmlDoc.AppendChild(rootNode);

            foreach (PropertyInfo property in GetProperties())
            {
                XmlNode node = xmlDoc.CreateElement(property.Name);
                node.InnerText = property.GetValue(this).ToString();
                rootNode.AppendChild(node);
            }

            xmlDoc.Save(SettingsPath);
        }

        public void CopyFrom(EditorSettings e)
        {
            foreach (PropertyInfo property in GetProperties())
            {
                if (!Attribute.IsDefined(property, typeof(NoSettingCopyAttribute)))
                    property.SetValue(this, property.GetValue(e));
            }
        }
        #endregion
    }

    /// <summary>
    /// Use this to signify that a property should not be loaded from
    /// or saved to disk or copied with other settings; mostly for
    /// settings that directly depend on other settings.
    /// </summary>
    public class NoSettingTouchAttribute : Attribute
    {
    }

    /// <summary>
    /// Use this to signify that a property should not be copied
    /// between the settings on disk and the settings in memory. Used
    /// for settings that don't need to be saved.
    /// </summary>
    public class NoSettingCopyAttribute : Attribute
    {
    }
}

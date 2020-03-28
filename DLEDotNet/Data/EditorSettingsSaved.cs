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
        public TextureFilters TextureFilters { get => _textureFilters; set => AssignChanged(ref _textureFilters, value); }
        #endregion

        #region --- Variables (including default values)
        private TextureFilters _textureFilters = TextureFilters.All;
        #endregion
    }
}

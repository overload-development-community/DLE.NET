using System;
using System.Collections.Generic;
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
        #region --- Properties
        public LayoutOrientation ActiveLayout
        {
            get => _layoutOrientation;
            set => AssignAlways(ref _layoutOrientation, value);
        }
        #endregion

        #region --- Variables (including default values)
        private LayoutOrientation _layoutOrientation = LayoutOrientation.HORIZONTAL;
        #endregion

        #region --- Reloading, saving, copying settings
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
            return typeof(EditorSettings).GetProperties(BindingFlags.Public | BindingFlags.Instance);
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
            this.ActiveLayout = e.ActiveLayout;
        }
        #endregion
    }
}

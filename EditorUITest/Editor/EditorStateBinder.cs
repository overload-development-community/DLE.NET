using EditorUITest.Data;
using EditorUITest.Util;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Editor
{
    internal delegate object GetPropertyValueUncachedDelegate(object root, string property);

    public class EditorStateBinder
    {
        #region --- static

#if DEBUG
        private const bool DebugMode = true;
#else
        private const bool DebugMode = false;
#endif

        private static object _binderInitializeLock = new object();
        private static Dictionary<EditorState, EditorStateBinder> _binderCache = new Dictionary<EditorState, EditorStateBinder>();

        public static EditorStateBinder FromState(EditorState state)
        {
            lock (_binderInitializeLock)
            {
                if (_binderCache.ContainsKey(state))
                {
                    return _binderCache[state];
                }
                EditorStateBinder newBinder = new EditorStateBinder(state);
                _binderCache[state] = newBinder;
                return newBinder;
            }
        }

        private static T CastTo<T>(dynamic o) => (T)o;

        private static bool DynamicCast(Type type, dynamic toCast, out dynamic casted)
        {
            if (typeof(IConvertible).IsAssignableFrom(type) && typeof(IConvertible).IsAssignableFrom(toCast.GetType()))
            {
                try
                {
                    casted = Convert.ChangeType(toCast, type);
                    return true;
                }
                catch (InvalidCastException) { }
            }

            var methodInfo = typeof(EditorStateBinder).GetMethod(nameof(CastTo), BindingFlags.Static | BindingFlags.NonPublic);
            var genericArguments = new[] { type };
            var genericMethodInfo = methodInfo?.MakeGenericMethod(genericArguments);
            try
            {
                casted = genericMethodInfo?.Invoke(null, new[] { toCast });
                return casted != null || toCast == null;
            }
            catch (TargetInvocationException tie)
            {
                if (tie.InnerException is InvalidCastException)
                {
                    casted = null;
                    return false;
                }
                throw;
            }
        }

        private static bool MaybeDoExplicitCast(dynamic value, out dynamic destinationValue, bool allowExplicitCast, Type destType)
        {
            if (allowExplicitCast)
            {
                return DynamicCast(destType, value, out destinationValue);
            }
            else
            {
                destinationValue = value;
                return true;
            }
        }

        #endregion

        #region --- implementation

        private readonly EditorState state;
        private readonly PropertyNameTree tree;
        private readonly Dictionary<Control, string> controlBinds;
        private readonly EditorWindow window;

        private EditorStateBinder(EditorState state)
        {
            this.state = state;
            if (!(state.Owner is EditorWindow))
            {
                throw new ArgumentException("The EditorState that an EditorStateBinder is attached to must be an EditorWindow.");
            }

            this.tree = new PropertyNameTree();
            this.controlBinds = new Dictionary<Control, string>();
            this.window = (EditorWindow)state.Owner;

            state.PropertyChanged += State_PropertyChanged;
            state.BeforePropertyChanged += State_BeforePropertyChanged;
        }

        private static bool IsAncestralProperty(string ancestor, string child)
        {
            return child.StartsWith(ancestor + ".");
        }

        private void State_BeforePropertyChanged(object sender, PropertyChangeEventArgs e)
        {
            Control c = window.GetActiveControl();
            if (controlBinds.ContainsKey(c) && c is IValidatable v)
            {
                string bindPropertyName = controlBinds[c];
                if (IsAncestralProperty(e.PropertyName, bindPropertyName))
                {
                    // validate to submit changed value before the parent changes
                    v.Validate();
                }
            }
        }

        private void State_PropertyChanged(object sender, PropertyChangeEventArgs e)
        {
            tree.Call(e, GetPropertyValueUncached);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private PropertyCacheEntry ResolvePropertyUncached(dynamic root, string property)
        {
            object self = root;
            PropertyInfo tmp = null;
            PropertyCacheEntry pce = new PropertyCacheEntry();

            foreach (string tok in property.Split('.'))
            {
                if (tok == "") continue;
                if (tmp != null) self = tmp.GetValue(self);
                if (self == null) break;
                tmp = self.GetType().GetProperty(tok);
                if (tmp == null) break;
            }

            if (self != null && tmp != null)
                pce = new PropertyCacheEntry(tmp, self);

            if (Object.Equals(root, state))
                tree.Cache(property, pce);
            return pce;
        }

        private PropertyCacheEntry ResolveProperty(dynamic root, string property)
        {
            if (Object.Equals(root, state))
            {
                // maybe lookup cache
                if (tree.GetCachedEntry(property, out PropertyCacheEntry entry))
                {
                    return entry;
                }
            }
            return ResolvePropertyUncached(root, property);
        }

        private dynamic GetPropertyValueUncached(dynamic root, string property)
        {
            PropertyCacheEntry pce = ResolvePropertyUncached(root, property);
            return pce.Property?.GetValue(pce.Parent);
        }

        private dynamic GetPropertyValue(dynamic root, string property)
        {
            PropertyCacheEntry pce = ResolveProperty(root, property);
            return pce.Property?.GetValue(pce.Parent);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private bool SetPropertyValue_i(string property, dynamic value, bool allowExplicitCast)
        {
            PropertyCacheEntry pce = ResolveProperty(state, property);
            if (pce.Parent == null || pce.Property == null)
                return false;
            if (!MaybeDoExplicitCast(value, out dynamic assignValue, allowExplicitCast, pce.PropertyType))
                return false;
            try
            {
                pce.Property.SetValue(pce.Parent, assignValue);
                return true;
            }
            catch (ArgumentException)
            {
                return false;
            }
        }

        private bool SetPropertyValue(string property, dynamic value)
        {
            return SetPropertyValue(property, value, false);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private bool SetPropertyValue(string property, dynamic value, bool allowExplicitCast)
        {
            bool success = SetPropertyValue_i(property, value, allowExplicitCast);
            if (!success)
            {
                if (DebugMode) System.Diagnostics.Debug.WriteLine(new UnableToSetPropertyWarningException(property, value));
            }
            return success;
        }

        private void AddToTreeAndCall(string property, PropertyChangeEventHandler handler)
        {
            tree.Add(property, handler);
            handler(this, new PropertyChangeEventArgs(property, GetPropertyValueUncached(state, property)));
        }

        private void BindControl(Control control, string writePropertyName)
        {
            if (controlBinds.ContainsKey(control))
            {
                if (controlBinds[control] == writePropertyName) return;
                throw new ArgumentException("Control '" + control.Name + "' is already bound to a property.");
            }
            controlBinds[control] = writePropertyName;
        }

        #endregion

        #region --- binders

        /// <summary>
        /// Binds an IntTextBox to a (public) property with an integer type.
        /// </summary>
        /// <param name="textBox">The IntTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have an integer type.</param>
        /// <param name="readOnly">Whether the text box is readonly; if false, no listener for when its value changes will be registered.</param>
        public void BindIntTextBox(IntTextBox textBox, string property, bool readOnly)
        {
            BindControl(textBox, property);
            AddToTreeAndCall(property, (object sender, PropertyChangeEventArgs e) => {
                if (textBox.Enabled = e.NewValue != null)
                    textBox.Value = Convert.ToInt32(e.NewValue);
                else
                    textBox.Text = "";
            });
            if (!readOnly) textBox.ValueChanged += (object sender, EventArgs e) => { SetPropertyValue(property, textBox.Value, true); };
        }

        /// <summary>
        /// Binds a FloatTextBox to a (public) property with a numeric type.
        /// </summary>
        /// <param name="textBox">The FloatTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a numeric type.</param>
        /// <param name="readOnly">Whether the text box is readonly; if false, no listener for when its value changes will be registered.</param>
        public void BindFloatTextBox(FloatTextBox textBox, string property, bool readOnly)
        {
            BindControl(textBox, property);
            AddToTreeAndCall(property, (object sender, PropertyChangeEventArgs e) => {
                if (textBox.Enabled = e.NewValue != null)
                    textBox.Value = (double)(e.NewValue);
                else
                    textBox.Text = "";
            });
            if (!readOnly) textBox.ValueChanged += (object sender, EventArgs e) => SetPropertyValue(property, textBox.Value, true);
        }

        /// <summary>
        /// Binds a StringTextBox to a (public) property with a string type.
        /// </summary>
        /// <param name="textBox">The StringTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a string type.</param>
        /// <param name="readOnly">Whether the text box is readonly; if false, no listener for when its value changes will be registered.</param>
        public void BindStringTextBox(StringTextBox textBox, string property, bool readOnly)
        {
            BindControl(textBox, property);
            AddToTreeAndCall(property, (object sender, PropertyChangeEventArgs e) => {
                if (textBox.Enabled = e.NewValue != null)
                    textBox.Value = Convert.ToString(e.NewValue);
                else
                    textBox.Text = "";
            });
            if (!readOnly) textBox.ValueChanged += (object sender, EventArgs e) => SetPropertyValue(property, textBox.Value);
        }

        /// <summary>
        /// Binds an SegSideTextBox to a (public) property with an integer type that contains the maximum number of segments (that must never be negative). The segment and side that the user inputs are not bound with this method.
        /// </summary>
        /// <param name="textBox">The IntTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have an integer type and must never have a negative value.</param>
        public void BindSegSideTextBox(SegSideTextBox textBox, string property)
        {
            BindControl(textBox, property);
            AddToTreeAndCall(property, (object sender, PropertyChangeEventArgs e) => {
                if (textBox.Enabled = e.NewValue != null)
                    textBox.MaxSegmentCount = Convert.ToUInt32(e.NewValue);
            });
        }

        /// <summary>
        /// Binds a combo box to two (public) properties; a list (IEnumerable of IListBoxables) and the current selection (must implement IListBoxable).
        /// </summary>
        /// <param name="comboBox">The combo box to bind.</param>
        /// <param name="listProperty">The name of the property under EditorState to bind for the list of values; dots are allowed for nested properties. The property must implement IEnumerable<IListBoxable>.</param>
        /// <param name="assignableProperty">The name of the property under EditorState to bind for the selected value; the type must be assignable from the given type of the list property.</param>
        public void BindComboBox(ComboBox comboBox, string listProperty, string selectedProperty)
        {
            BindControl(comboBox, selectedProperty);
            AddToTreeAndCall(listProperty, (object sender, PropertyChangeEventArgs e) => {
                List<IListBoxable> copiedList = new List<IListBoxable>();
                IEnumerable<IListBoxable> sourceList = (IEnumerable<IListBoxable>)e.NewValue;
                if (comboBox.Enabled = sourceList != null)
                {
                    copiedList.AddRange(sourceList);
                    int oldSelectedIndex = comboBox.SelectedIndex;
                    comboBox.DataSource = copiedList;
                    comboBox.DisplayMember = "DisplayValue";
                    if (comboBox.SelectedIndex != oldSelectedIndex && 0 <= oldSelectedIndex && oldSelectedIndex < copiedList.Count)
                        comboBox.SelectedIndex = oldSelectedIndex;
                }
                else
                {
                    comboBox.DataSource = null;
                }
            });
            comboBox.SelectedIndexChanged += (object sender, EventArgs e) => {
                SetPropertyValue(selectedProperty, comboBox.SelectedItem);
            };
        }
        #endregion

        #region --- unused
        /* Unused for now.
        private Type GetPropertyType(string property)
        {
            object self = state;
            PropertyInfo tmp = null;
            foreach (string tok in property.Split('.'))
            {
                tmp = tmp.PropertyType.GetType().GetProperty(tok);
                if (tmp == null) return null;
                self = tmp.GetValue(self);
            }
            return tmp?.PropertyType;
        }*/

        private EditorStateBinder() : this(null)
        {
        }
        #endregion
    }

    #region --- utility and exception classes

    internal struct PropertyCacheEntry
    {
        internal PropertyInfo Property { get; }
        internal object Parent { get; }

        internal PropertyCacheEntry(PropertyInfo property, object parent)
        {
            Property = property;
            Parent = parent;
        }

        internal Type PropertyType
        {
            get
            {
                return Property.PropertyType;
            }
        }
    }

    [Serializable]
    public class UnableToSetPropertyWarningException : System.ComponentModel.WarningException
    {
        public UnableToSetPropertyWarningException() : base("Unable to assign new value to property.")
        {
        }

        public UnableToSetPropertyWarningException(string property, object value) : base("Unable to assign new value (" + (value?.ToString() ?? "null") + ") to property '" + property + "'.")
        {
        }
    }

    [Serializable]
    public class PropertyNotFoundException : Exception
    {
        public PropertyNotFoundException() : base("The given property was not found.")
        {
        }

        public PropertyNotFoundException(string property) : base("Property '" + property + "' not found.")
        {
        }
    }

    [Serializable]
    public class InvalidPropertyException : Exception
    {
        public InvalidPropertyException() : base("The given property has an invalid type for this binding type.")
        {
        }

        public InvalidPropertyException(string property) : base("The property '" + property + "' has an invalid type for this binding type.")
        {
        }
    }

    #endregion
}

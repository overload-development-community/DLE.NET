﻿using DLEDotNet.Data;
using DLEDotNet.Editor.Layouts;
using DLEDotNet.Util;
using LibDescent.Edit;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    public sealed class EditorStateBinder
    {
        #region --- static

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
        private static T UncheckedCastTo<T>(dynamic o) => unchecked((T)o);

        // return true if cast successful with casted being the casted object
        // return false if cast unsuccessful, casted = null or an exception if a debug build
        private static bool DynamicCast(Type type, dynamic toCast, out dynamic casted, bool isUnchecked = false)
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

            var methodInfo = typeof(EditorStateBinder).GetMethod(isUnchecked ? nameof(UncheckedCastTo) : nameof(CastTo), BindingFlags.Static | BindingFlags.NonPublic);
            var genericArguments = new[] { type };
            var genericMethodInfo = methodInfo?.MakeGenericMethod(genericArguments);
            try
            {
                casted = genericMethodInfo?.Invoke(null, new[] { toCast });
                return casted != null || toCast == null;
            }
            catch (TargetInvocationException tie)
            {
#if !DEBUG
                if (tie.InnerException is InvalidCastException)
                {
                    casted = null;
                    return false;
                }
#endif
                throw tie;
            }
        }

        private static dynamic DynamicCast(Type type, dynamic toCast, bool isUnchecked)
        {
            if (!DynamicCast(type, toCast, out dynamic result, isUnchecked))
                throw new InvalidCastException();
            return result;
        }

        private static dynamic DynamicCast(Type type, dynamic toCast) => DynamicCast(type, toCast, false);


        #endregion

        #region --- implementation

        private readonly EditorState state;
        private readonly RootStateEventListener rsel;
        private readonly Dictionary<Component, ControlBind> controlBinds;
        private readonly EditorWindow window;
        private object binderLock = new object();

        private EditorStateBinder(EditorState state)
        {
            this.state = state;
            if (!(state.Owner is EditorWindow))
            {
                throw new ArgumentException("The EditorState that an EditorStateBinder is attached to must be an EditorWindow.");
            }

            this.rsel = new RootStateEventListener(this.state);
            this.controlBinds = new Dictionary<Component, ControlBind>();
            this.window = (EditorWindow)state.Owner;

            state.BeforePropertyChanged += State_BeforePropertyChanged;
        }

        private void State_BeforePropertyChanged(object sender, BeforePropertyChangeEventArgs e)
        {
            ValidateActiveControl(e.PropertyName);
        }

        private void ValidateActiveControl()
        {
            Control c = window.GetActiveControl();
            if (c != null && controlBinds.ContainsKey(c) && c is IValidatable v)
            {
                v.Validate();
            }
        }

        private void ValidateActiveControl(string changedPropertyName)
        {
            Control c = window.GetActiveControl();
            if (c != null && controlBinds.ContainsKey(c) && c is IValidatable v)
            {
                if (PropertyUtil.IsAncestralProperty(changedPropertyName, controlBinds[c].Property))
                {
                    // validate to submit changed value before the parent changes
                    v.Validate();
                }
            }
        }

        private dynamic Unbox(object boxed)
        {
            if (boxed == null)
                return null;
            return DynamicCast(boxed.GetType(), boxed);
        }

        #region --- RootStateEventListener wrappers & debounced methods

        private bool SetPropertyValue(string property, dynamic value, bool allowExplicitCast = false, bool allowUncheckedCast = false)
        {
            if (allowExplicitCast)
                return DynamicCast(rsel.GetPropertyType(property), value, out dynamic assignValue, allowUncheckedCast)
                    && rsel.SetPropertyValue(property, assignValue);
            else
                return rsel.SetPropertyValue(property, value);
        }

        private bool ApplyToPropertyValue(string property, Func<dynamic, dynamic> func)
        {
            return rsel.ApplyToPropertyValue(property, func);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private bool SetPropertyValueDebounced(bool debounce, string property, dynamic value)
        {
            return debounce || SetPropertyValue(property, value);
        }
        
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private bool SetPropertyValueDebounced(bool debounce, string property, dynamic value, bool allowExplicitCast)
        {
            return debounce || SetPropertyValue(property, value, allowExplicitCast);
        }
        
        private bool ApplyToPropertyValueDebounced(bool debounce, string property, Func<dynamic, dynamic> func)
        {
            return debounce || ApplyToPropertyValue(property, func(rsel.GetPropertyValue(property)));
        }

        private void Debounce(ref bool debounce, Action impl)
        {
            debounce = true;
            try
            {
                impl();
            }
            finally
            {
                debounce = false;
            }
        }

        #endregion

        private void BindControl(Component control, string bindPropertyName, string handlerPropertyName, PropertyChangeEventHandler handler)
        {
            lock (binderLock)
            {
                if (controlBinds.ContainsKey(control))
                {
                    if (controlBinds[control].Property == bindPropertyName) return;
                    throw new ArgumentException("Control is already bound to a property.");
                }
                controlBinds[control] = new ControlBind(bindPropertyName, handler);
                rsel.RegisterAndCall(handlerPropertyName, handler);
            }
        }

        private void BindControl(Control control, string propertyName, PropertyChangeEventHandler handler) => BindControl(control, propertyName, propertyName, handler);

        private void RefreshBind(ControlBind bind)
        {
            bind.Handler(this, new PropertyChangeEventArgs(bind.Property, rsel.GetPropertyValue(bind.Property)));
        }

        #endregion

        #region --- public interface incl. binding methods

        /// <summary>
        /// Triggers a simulated property change event to a bound control.
        /// </summary>
        /// <param name="control">The control to refresh.</param>
        public void RefreshBind(Control control) //, string propertyName)
        {
            if (controlBinds.ContainsKey(control))
                RefreshBind(controlBinds[control]);
        }

        /// <summary>
        /// Binds a control already bound to a property to a setting property. A property change event will be simulated on setting change.
        /// </summary>
        /// <param name="control">The control to bind.</param>
        /// <param name="settingProperty">The name of the setting property to listen. Should be relative to the root of EditorSettings.</param>
        /// <exception cref="ArgumentException">If the given control has not been bound to a property.</exception>
        public void BindControlToSettingChange(Control control, string settingProperty)
        {
            if (!controlBinds.ContainsKey(control))
                throw new ArgumentException("Control '" + control.Name + "' has not been bound to a property.");
            ControlBind thisBind = controlBinds[control];
            string listenedPropertyName = thisBind.Property;
            this.state.SettingChanged += (object sender, PropertyChangeEventArgs e) =>
            {
                if (PropertyUtil.IsAncestralProperty(settingProperty, e.PropertyName))
                    RefreshBind(thisBind);
            };
        }

        /// <summary>
        /// Binds a control already bound to a property to another property. A property change event for the original bound property  will be simulated on property change.
        /// </summary>
        /// <param name="control">The control to bind.</param>
        /// <param name="property">The name of the auxiliary property to listen. Should be relative to the root of EditorState.</param>
        /// <exception cref="ArgumentException">If the given control has not been bound to a property.</exception>
        public void BindControlToAuxiliaryPropertyChange(Control control, string property)
        {
            if (!controlBinds.ContainsKey(control))
                throw new ArgumentException("Control '" + control.Name + "' has not been bound to a property.");
            ControlBind thisBind = controlBinds[control];
            rsel.Register(property, (object sender, PropertyChangeEventArgs e) =>
            {
                if (e.PropertyName == property)
                {
                    RefreshBind(thisBind);
                }
            });
        }

        /// <summary>
        /// Binds a Label to a (public) property with any type convertable to a string.
        /// </summary>
        /// <param name="label">The Label to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties.</param>
        /// <param name="formatString">A format string to use; parameter 0 ({0}) will be the value of the property. Omit or supply null to not use custom formatting.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindLabel(Label label, string property, string formatString = null)
        {
            BindControl(label, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                {
                    if (formatString != null)
                        label.Text = String.Format(formatString, e.NewValue);
                    else
                        label.Text = Convert.ToString(e.NewValue);
                }
            });
        }

        /// <summary>
        /// Binds an IntTextBox to a (public) property with an integer type.
        /// </summary>
        /// <param name="textBox">The IntTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have an integer type.</param>
        /// <param name="readOnly">Whether the text box is read-only; if false, no listener for when its value changes will be registered. The control should not even accept user input if true.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindIntTextBox(IntTextBox textBox, string property, bool readOnly)
        {
            bool debounce = false;
            BindControl(textBox, property, (object sender, PropertyChangeEventArgs e) =>
                Debounce(ref debounce, () =>
                {
                    if (e.PropertyName == property)
                    {
                        if (textBox.Enabled = e.NewValue != null)
                            textBox.Value = (int)Unbox(e.NewValue);
                        else
                            textBox.Text = "";
                    }
                }));
            if (!readOnly)
                textBox.ValueChanged += (object sender, EventArgs e) => SetPropertyValueDebounced(debounce, property, textBox.Value, true);
        }

        /// <summary>
        /// Binds a FloatTextBox to a (public) property with a numeric type.
        /// </summary>
        /// <param name="textBox">The FloatTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a numeric type.</param>
        /// <param name="readOnly">Whether the text box is read-only; if false, no listener for when its value changes will be registered. The control should not even accept user input if true.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindFloatTextBox(FloatTextBox textBox, string property, bool readOnly)
        {
            bool debounce = false;
            BindControl(textBox, property, (object sender, PropertyChangeEventArgs e) => 
                Debounce(ref debounce, () => {
                    if (e.PropertyName == property)
                    {
                        if (textBox.Enabled = e.NewValue != null)
                            textBox.Value = (double)Unbox(e.NewValue);
                        else
                            textBox.Text = "";
                    }
                }));
            if (!readOnly)
                textBox.ValueChanged += (object sender, EventArgs e) => SetPropertyValueDebounced(debounce, property, textBox.Value, true);
        }

        /// <summary>
        /// Binds a StringTextBox to a (public) property with a string type.
        /// </summary>
        /// <param name="textBox">The StringTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a string type.</param>
        /// <param name="readOnly">Whether the text box is read-only; if false, no listener for when its value changes will be registered. The control should not even accept user input if true.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindStringTextBox(StringTextBox textBox, string property, bool readOnly)
        {
            bool debounce = false;
            BindControl(textBox, property, (object sender, PropertyChangeEventArgs e) => 
                Debounce(ref debounce, () => {
                    if (e.PropertyName == property)
                    {
                        if (textBox.Enabled = e.NewValue != null)
                            textBox.Value = Convert.ToString(e.NewValue);
                        else
                            textBox.Text = "";
                    }
                }));
            if (!readOnly)
                textBox.ValueChanged += (object sender, EventArgs e) => SetPropertyValueDebounced(debounce, property, textBox.Value);
        }

        /// <summary>
        /// Binds an SegSideTextBox to a (public) property with an integer type that contains the maximum number of segments (that must never be negative). The segment and side that the user inputs are not bound with this method.
        /// </summary>
        /// <param name="textBox">The IntTextBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have an integer type and must never have a negative value.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindSegSideTextBox(SegSideTextBox textBox, string property)
        {
            BindControl(textBox, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property) 
                    if (textBox.Enabled = e.NewValue != null)
                        textBox.MaxSegmentCount = Convert.ToInt32(e.NewValue);
            });
        }

        /// <summary>
        /// Binds a TrackBar to a (public) property with an integer type.
        /// </summary>
        /// <param name="trackBar">The TrackBar to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have an integer type.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindTrackBar(TrackBar trackBar, string property)
        {
            bool debounce = false;
            BindControl(trackBar, property, (object sender, PropertyChangeEventArgs e) => 
                Debounce(ref debounce, () => {
                    if (e.PropertyName == property)
                        trackBar.Value = Convert.ToInt32(e.NewValue);
                }));
            trackBar.ValueChanged += (object sender, EventArgs e) =>
                SetPropertyValueDebounced(debounce, property, trackBar.Value);
        }

        /// <summary>
        /// Binds a CheckBox to a (public) property with a boolean type.
        /// </summary>
        /// <param name="checkBox">The CheckBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a boolean type.</param>
        /// <param name="readOnly">Whether the text box is read-only; if false, no listener for when its value changes will be registered. The control should not even accept user input if true.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindCheckBox(CheckBox checkBox, string property, bool readOnly)
        {
            bool debounce = false;
            BindControl(checkBox, property, (object sender, PropertyChangeEventArgs e) => 
                Debounce(ref debounce, () => {
                    if (e.PropertyName == property)
                        checkBox.Checked = (bool)e.NewValue;
                }));
            if (!readOnly)
                checkBox.CheckedChanged += (object sender, EventArgs e) =>
                    SetPropertyValueDebounced(debounce, property, checkBox.Checked);
        }

        /// <summary>
        /// Binds a CheckBox to a (public) property with a flag enum type.
        /// </summary>
        /// <typeparam name="T">The type of the flag enum (should have type int).</typeparam>
        /// <param name="checkBox">The CheckBox to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a flag enum type.</param>
        /// <param name="flag">The enum value to use as the "selected" flag.</param>
        /// <param name="readOnly">Whether the text box is read-only; if false, no listener for when its value changes will be registered. The control should not even accept user input if true.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindCheckBoxFlag<T>(CheckBox checkBox, string property, T flag, bool readOnly) where T : Enum
        {
            bool debounce = false;
            int flagI = Convert.ToInt32(flag);
            BindControl(checkBox, property, (object sender, PropertyChangeEventArgs e) =>
                Debounce(ref debounce, () => {
                    if (e.PropertyName == property)
                        checkBox.Checked = 0 != ((int)e.NewValue & flagI);
                }));
            if (!readOnly)
                checkBox.CheckedChanged += (object sender, EventArgs e) =>
                    ApplyToPropertyValueDebounced(debounce, property,
                        v => (T)(object)(checkBox.Checked ? ((int)v | flagI) : ((int)v & ~flagI)));
        }

        /// <summary>
        /// Binds a RadioButton to a (public) property with a variable type, with the radio button being considered "selected" if it is equal to a given value.
        /// </summary>
        /// <typeparam name="T">The type of the selected value; must be equality comparable and assignable to the type of the property, and thus must be compatible with it.</typeparam>
        /// <param name="radioButton">The RadioButton to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties.</param>
        /// <param name="selectedValue">The value for the property for which this radio button is "selected". Must be comparable in terms of equality to the type of the property.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindRadioButton<T>(RadioButton radioButton, string property, T selectedValue)
        {
            EventHandler radioButton_CheckedChanged = (object sender, EventArgs e) =>
            {
                if (radioButton.Checked)
                    SetPropertyValue(property, selectedValue);
            };
            EqualityComparer<T> equalityComparer = EqualityComparer<T>.Default;
            BindControl(radioButton, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                {
                    radioButton.CheckedChanged -= radioButton_CheckedChanged; // debounce
                    radioButton.Checked = e.NewValue == null ? selectedValue == null : equalityComparer.Equals(selectedValue, (T)e.NewValue);
                    radioButton.CheckedChanged += radioButton_CheckedChanged;
                }
            });
            radioButton.CheckedChanged += radioButton_CheckedChanged;
        }

        /// <summary>
        /// Binds a combo box to two (public) properties; a list (IEnumerable of IListBoxables) and the current selection (must implement IListBoxable).
        /// </summary>
        /// <param name="comboBox">The combo box to bind.</param>
        /// <param name="listProperty">The name of the property under EditorState to bind for the list of values; dots are allowed for nested properties. The property must implement IEnumerable<IListBoxable>.</param>
        /// <param name="selectedProperty">The name of the property under EditorState to bind for the selected value; the type must be assignable from the given type of the list property. Set to null to ignore the user selection.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindComboBox(ComboBox comboBox, string listProperty, string selectedProperty)
        {
            EventHandler comboBox_SelectedIndexChanged = null;
            if (selectedProperty != null)
                comboBox_SelectedIndexChanged = (object sender, EventArgs e) => SetPropertyValue(selectedProperty, comboBox.SelectedItem);
            BindControl(comboBox, selectedProperty ?? listProperty, listProperty, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == listProperty)
                {
                    if (comboBox_SelectedIndexChanged != null) comboBox.SelectedIndexChanged -= comboBox_SelectedIndexChanged;
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
                    if (comboBox_SelectedIndexChanged != null) comboBox.SelectedIndexChanged += comboBox_SelectedIndexChanged;
                }
            });
            if (comboBox_SelectedIndexChanged != null)
                comboBox.SelectedIndexChanged += comboBox_SelectedIndexChanged;
        }

        /// <summary>
        /// Binds a combo box to a (public) property with an enum type.
        /// </summary>
        /// <typeparam name="T">The type of the enum.</typeparam>
        /// <param name="comboBox">The combo box to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have an enum type T.</param>
        public void BindComboBox<T>(ComboBox comboBox, string property) where T : Enum
        {
            comboBox.Items.Clear();
            comboBox.DataSource = Enum.GetValues(typeof(T));
            BindControl(comboBox, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    comboBox.SelectedItem = e.NewValue;
            });
            comboBox.SelectedIndexChanged += (object sender, EventArgs e) =>
                SetPropertyValue(property, comboBox.SelectedItem);
        }

        /// <summary>
        /// Binds a ToolStripButton as if it were a RadioButton to a (public) property with a variable type, with the item being considered "selected" (held) if it is equal to a given value.
        /// </summary>
        /// <typeparam name="T">The type of the selected value; must be equality comparable and assignable to the type of the property, and thus must be compatible with it.</typeparam>
        /// <param name="toolStripButton">The ToolStripButton to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties.</param>
        /// <param name="selectedValue">The value for the property for which this radio button is "selected". Must be comparable in terms of equality to the type of the property.</param>
        /// <param name="readOnly">Whether the radio button is read-only; if false, no listener for when its value changes will be registered.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindToolStripButtonAsRadioButton<T>(ToolStripButton toolStripButton, string property, T selectedValue, bool readOnly)
        {
            EqualityComparer<T> equalityComparer = EqualityComparer<T>.Default;
            BindControl(toolStripButton, property, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    toolStripButton.Checked = e.NewValue == null ? selectedValue == null : equalityComparer.Equals(selectedValue, (T)e.NewValue);
            });
            if (!readOnly)
                toolStripButton.Click += (object sender, EventArgs e) => SetPropertyValue(property, selectedValue);
        }

        /// <summary>
        /// Binds a ToolStripButton as if it were a CheckBox to a (public) property with a boolean type.
        /// </summary>
        /// <param name="toolStripButton">The ToolStripButton to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a boolean type.</param
        /// <param name="readOnly">Whether the radio button is read-only; if false, no listener for when its value changes will be registered.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindToolStripButtonAsCheckBox(ToolStripButton toolStripButton, string property, bool readOnly)
        {
            BindControl(toolStripButton, property, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    toolStripButton.Checked = (Boolean)e.NewValue;
            });
            if (!readOnly)
                toolStripButton.Click += (object sender, EventArgs e) => SetPropertyValue(property, !toolStripButton.Checked);
        }

        /// <summary>
        /// Binds a ToolStripButton as if it were a RadioButton to a (public) property with a variable type, with the item being considered "selected" (held) if it is equal to a given value.
        /// </summary>
        /// <typeparam name="T">The type of the selected value; must be equality comparable and assignable to the type of the property, and thus must be compatible with it.</typeparam>
        /// <param name="toolStripButton">The ToolStripButton to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties.</param>
        /// <param name="selectedValue">The value for the property for which this radio button is "selected". Must be comparable in terms of equality to the type of the property.</param>
        /// <param name="readOnly">Whether the radio button is read-only; if false, no listener for when its value changes will be registered.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindToolStripMenuItemAsRadioButton<T>(ToolStripMenuItem toolStripButton, string property, T selectedValue, bool readOnly)
        {
            EqualityComparer<T> equalityComparer = EqualityComparer<T>.Default;
            BindControl(toolStripButton, property, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    toolStripButton.Checked = e.NewValue == null ? selectedValue == null : equalityComparer.Equals(selectedValue, (T)e.NewValue);
            });
            if (!readOnly)
                toolStripButton.Click += (object sender, EventArgs e) => SetPropertyValue(property, selectedValue);
        }

        /// <summary>
        /// Binds a ToolStripMenuItem as if it were a CheckBox to a (public) property with a boolean type.
        /// </summary>
        /// <param name="toolStripMenuItem">The ToolStripMenuItem to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a boolean type.</param
        /// <param name="readOnly">Whether the radio button is read-only; if false, no listener for when its value changes will be registered.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindToolStripMenuItemAsCheckBox(ToolStripMenuItem toolStripMenuItem, string property, bool readOnly)
        {
            BindControl(toolStripMenuItem, property, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    toolStripMenuItem.Checked = (Boolean)e.NewValue;
            });
            if (!readOnly)
                toolStripMenuItem.Click += (object sender, EventArgs e) => SetPropertyValue(property, !toolStripMenuItem.Checked);
        }

        /// <summary>
        /// Binds a ToolStripMenuItem as if it were a CheckBox to a (public) property with a boolean type.
        /// </summary>
        /// <param name="toolStripMenuItem">The ToolStripMenuItem to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties. The property must have a flag enum type.</param>
        /// <param name="flag">The enum value to use as the "selected" flag.</param>
        /// <param name="readOnly">Whether the text box is read-only; if false, no listener for when its value changes will be registered. The control should not even accept user input if true.</param>
        /// <exception cref="ArgumentException">If the given control has already been bound to a property.</exception>
        public void BindToolStripMenuItemAsCheckBoxFlag<T>(ToolStripMenuItem toolStripMenuItem, string property, T flag, bool readOnly) where T : Enum
        {
            int flagI = Convert.ToInt32(flag);
            BindControl(toolStripMenuItem, property, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    toolStripMenuItem.Checked = 0 != ((int)e.NewValue & flagI);
            });
            if (!readOnly)
                toolStripMenuItem.Click += (object sender, EventArgs e) =>
                    ApplyToPropertyValue(property,
                        v => (T)(object)(toolStripMenuItem.Checked ? ((int)v | flagI) : ((int)v & ~flagI)));
        }

        /// <summary>
        /// Binds a tab control's selected tab index to a (public) property to avoid it from being lost on reload.
        /// </summary>
        /// <param name="tabControl">The tab control to bind.</param>
        /// <param name="property">The name of the property under EditorState to bind; dots are allowed for nested properties.</param>
        public void BindTabControlBacking(TabControl tabControl, string property)
        {
            BindControl(tabControl, property, (object sender, PropertyChangeEventArgs e) => {
                if (e.PropertyName == property)
                    tabControl.SelectedIndex = MathUtil.Clamp(Convert.ToInt32((object)e.NewValue), -1, tabControl.TabCount - 1);
            });
            tabControl.SelectedIndexChanged += (object sender, EventArgs e) =>
                SetPropertyValue(property, Convert.ChangeType(tabControl.SelectedIndex, rsel.GetPropertyType(property)));
        }

        /// <summary>
        /// This method can be used to do "batch changes" to an editor state, i.e. commit large changes without
        /// causing events for every large change. After a batch change has been completed, property change events
        /// will be dispatched for all properties that were changed during the batch change.
        /// </summary>
        /// <param name="Commit">The action that does the actual changes.</param>
        public void BatchChange(Action Commit)
        {
            lock (binderLock)
            {
                ValidateActiveControl();
                state.Unsafe.PauseEditorStateEvents();
                Commit();
                state.Unsafe.ResumeEditorStateEvents();
            }
        }
        #endregion
    }

    #region --- utility and exception classes

    internal struct ControlBind
    {
        internal string Property { get; }
        internal PropertyChangeEventHandler Handler { get; }

        internal ControlBind(string property, PropertyChangeEventHandler handler)
        {
            Property = property;
            Handler = handler;
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

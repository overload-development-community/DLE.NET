using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace EditorUITest.Data
{
    public delegate void PropertyChangedEventHandler(object sender, PropertyChangedEventArgs e);

    public abstract class ChangeableState
    {
        private readonly Dictionary<string, SubstateListener> substates = new Dictionary<string, SubstateListener>();

        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Use this, when the value of a property has changed, in order to notify
        /// controls bound to that particular value. It is recommended to use
        /// AssignChanged instead whenever possible.
        /// </summary>
        /// <param name="propertyName">The property name; this should match
        /// the name of the property itself, with dots representing the
        /// path to access from an EditorState object.</param>
        /// <param name="newValue">The new value.</param>
        [MethodImpl(MethodImplOptions.Synchronized)]
        protected void OnPropertyChanged(string propertyName, object newValue)
        {
            PropertyInfo prop = this.GetType().GetProperty(propertyName);
            if (prop != null && typeof(ChangeableState).IsAssignableFrom(prop.PropertyType))
            {
                OnSubstateChanged(propertyName, prop.GetValue(this) as ChangeableState);
            }
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName, newValue));
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void OnSubstateChanged(string propertyName, ChangeableState newSubstate)
        {
            if (substates.ContainsKey(propertyName))
            {
                SubstateListener substate = substates[propertyName];
                substate.State.PropertyChanged -= substate.Handler;
            }
            if (newSubstate != null)
            {
                PropertyChangedEventHandler newHandler = (object sender, PropertyChangedEventArgs e) =>
                {
                    this.OnPropertyChanged(propertyName + "." + e.PropertyName, e.NewValue);
                };
                substates[propertyName] = new SubstateListener(newSubstate, newHandler);
                newSubstate.PropertyChanged += newHandler;
            }
            else
            {
                substates.Remove(propertyName);
            }
        }

        /// <summary>
        /// Assigns a new value to a variable. Returns whether the value was changed
        /// (i.e. whether the old value was different from the new value), and if
        /// so, this will also automatically call OnPropertyChanged.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="variable">The variable to assign to.</param>
        /// <param name="newValue">The new value to assign.</param>
        /// <param name="property">The property name; this should match
        /// the name of the property itself, with dots representing the
        /// path to access from an EditorState object. Thus, a property
        /// called "Value" under EditorState would be "Value", but if
        /// under another object under the property "Selection", it should
        /// be "Selection.Value".</param>
        /// <returns></returns>
        protected bool AssignChanged<T>(ref T variable, T newValue, string property)
        {
            T oldValue = variable;
            bool changed = !EqualityComparer<T>.Default.Equals(oldValue, newValue);
            variable = newValue;
            if (!changed) OnPropertyChanged(property, newValue);
            return changed;
        }
    }

    internal struct SubstateListener
    {
        internal readonly ChangeableState State;
        internal readonly PropertyChangedEventHandler Handler;

        public SubstateListener(ChangeableState state, PropertyChangedEventHandler handler)
        {
            State = state;
            Handler = handler;
        }
    }

    public class PropertyChangedEventArgs : EventArgs
    {
        private string propertyName;
        private object newValue;

        public PropertyChangedEventArgs(string propertyName, object newValue)
        {
            this.propertyName = propertyName;
            this.newValue = newValue;
        }

        public string PropertyName
        {
            get => propertyName;
        }

        public object NewValue
        {
            get => newValue;
        }
    }
}
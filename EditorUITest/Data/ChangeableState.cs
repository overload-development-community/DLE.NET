using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace EditorUITest.Data
{
    public delegate void PropertyChangeEventHandler(object sender, PropertyChangeEventArgs e);

    public abstract class ChangeableState
    {
        private readonly Dictionary<string, SubstateListener> substates = new Dictionary<string, SubstateListener>();

        /// <summary>
        /// Called after a property has been changed. The new value is provided as a convenience.
        /// </summary>
        public event PropertyChangeEventHandler PropertyChanged;

        /// <summary>
        /// 
        /// </summary>
        public event PropertyChangeEventHandler BeforePropertyChanged;

        /// <summary>
        /// Use this, when the value of a property has changed, in order to notify
        /// controls bound to that particular value. It is recommended to use
        /// AssignChanged instead whenever possible.
        /// </summary>
        /// <param name="propertyName">The property name. This should be the
        /// plain property name (i.e. no nesting or dots).</param>
        /// <param name="newValue">The new value.</param>
        [MethodImpl(MethodImplOptions.Synchronized)]
        protected void OnPropertyChanged(string propertyName, dynamic newValue)
        {
            PropertyInfo prop = this.GetType().GetProperty(propertyName);
            BeforePropertyChanged?.Invoke(this, new PropertyChangeEventArgs(propertyName, newValue));
            if (prop != null && typeof(ChangeableState).IsAssignableFrom(prop.PropertyType))
            {
                OnSubstateChanged(propertyName, prop.GetValue(this) as ChangeableState);
            }
            PropertyChanged?.Invoke(this, new PropertyChangeEventArgs(propertyName, newValue));
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
                PropertyChangeEventHandler newHandler = (object sender, PropertyChangeEventArgs e) =>
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
        /// <param name="property">The property name. This should be the
        /// plain property name (i.e. no nesting or dots).</param>
        /// <returns></returns>
        protected bool AssignChanged<T>(ref T variable, T newValue, string property)
        {
            T oldValue = variable;
            bool changed = !EqualityComparer<T>.Default.Equals(oldValue, newValue);
            if (changed) this.BeforePropertyChanged?.Invoke(this, new PropertyChangeEventArgs(property, newValue));
            variable = newValue;
            if (changed) OnPropertyChanged(property, newValue);
            return changed;
        }
    }

    internal struct SubstateListener
    {
        internal readonly ChangeableState State;
        internal readonly PropertyChangeEventHandler Handler;

        public SubstateListener(ChangeableState state, PropertyChangeEventHandler handler)
        {
            State = state;
            Handler = handler;
        }
    }

    public class PropertyChangeEventArgs : EventArgs
    {
        private string propertyName;
        private dynamic newValue;

        public PropertyChangeEventArgs(string propertyName, dynamic newValue)
        {
            this.propertyName = propertyName;
            this.newValue = newValue;
        }

        public string PropertyName
        {
            get => propertyName;
        }

        public dynamic NewValue
        {
            get => newValue;
        }
    }
}
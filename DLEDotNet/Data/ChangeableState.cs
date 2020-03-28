using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace DLEDotNet.Data
{
    public delegate void PropertyChangeEventHandler(object sender, PropertyChangeEventArgs e);

    public abstract class ChangeableState
    {
        private readonly Dictionary<string, PropertyInfo> propCache = new Dictionary<string, PropertyInfo>();
        private readonly Dictionary<string, SubstateListener> substates = new Dictionary<string, SubstateListener>();

        /// <summary>
        /// Called after a property has been changed. The new value is provided as a convenience.
        /// </summary>
        public event PropertyChangeEventHandler PropertyChanged;

        /// <summary>
        /// Called before a property has been changed. Not necessarily called for all properties, but will be called
        /// if the property can be changed *directly* by user input (i.e. the property has a setter).
        /// </summary>
        public event PropertyChangeEventHandler BeforePropertyChanged;

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private PropertyInfo GetPropertyOrCache(string propertyName)
        {
            return propCache.ContainsKey(propertyName)
                ? propCache[propertyName]
                : propCache[propertyName] = this.GetType().GetProperty(propertyName);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void OnPropertyChanged(string propertyName, dynamic newValue)
        {
            PropertyInfo prop = GetPropertyOrCache(propertyName);
            if (prop != null && typeof(ChangeableState).IsAssignableFrom(prop.PropertyType) && !Attribute.IsDefined(prop, typeof(NoSubstateAutoSubscribeAttribute)))
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
        /// Raises PropertyChanged without raising BeforePropertyChanged. Use this, if the value of an otherwise
        /// read-only property is updated because of a change in value in another property. Use AssignChanged
        /// instead of read/write properties.
        /// </summary>
        /// <param name="propertyName">The property name.</param>
        /// <param name="newValue">The new value assigned, and the value that will be given to event handlers.</param>
        protected void OnReadOnlyPropertyChanged(string propertyName, dynamic newValue)
        {
            OnPropertyChanged(propertyName, newValue);
        }

        /// <summary>
        /// Assigns a new value to a variable. Returns whether the value was changed
        /// (i.e. whether the old value was different from the new value), and if
        /// so, this will also automatically call OnPropertyChanged with the name
        /// of the property that this was called from.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="variable">The variable to assign to.</param>
        /// <param name="newValue">The new value to assign.</param>
        /// <param name="property">The property name. This is filled in automatically;
        /// if you want to enter another value, please use AssignChanged.</param>
        /// <returns></returns>
        protected bool AssignChanged<T>(ref T variable, T newValue, [CallerMemberName] string property = null)
        {
            T oldValue = variable;
            bool changed = !EqualityComparer<T>.Default.Equals(oldValue, newValue);
            if (changed) this.BeforePropertyChanged?.Invoke(this, new PropertyChangeEventArgs(property, newValue));
            variable = newValue;
            if (changed) OnPropertyChanged(property, newValue);
            return changed;
        }

        /// <summary>
        /// Assigns a new value to a variable. Returns whether the value was changed
        /// (i.e. whether the old value was different from the new value), and if
        /// so, this will also automatically call OnPropertyChanged. Use this if the
        /// property name that is reported for events needs to be different from the
        /// actual name of the property this is called from.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="variable">The variable to assign to.</param>
        /// <param name="newValue">The new value to assign.</param>
        /// <param name="property">The property name. This should be the
        /// plain property name (i.e. no nesting or dots).</param>
        /// <returns></returns>
        protected bool AssignChangedRename<T>(ref T variable, T newValue, string property)
        {
            T oldValue = variable;
            bool changed = !EqualityComparer<T>.Default.Equals(oldValue, newValue);
            if (changed) this.BeforePropertyChanged?.Invoke(this, new PropertyChangeEventArgs(property, newValue));
            variable = newValue;
            if (changed) OnPropertyChanged(property, newValue);
            return changed;
        }

        /// <summary>
        /// Assigns a new value to a variable and calls OnPropertyChanged even if
        /// the value was not changed with the name of the property that this was 
        /// called from.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="variable">The variable to assign to.</param>
        /// <param name="newValue">The new value to assign.</param>
        /// <param name="property">The property name. This is filled in automatically;
        /// if you want to enter another value, please use AssignAlwaysRename.</param>
        /// <returns></returns>
        protected void AssignAlways<T>(ref T variable, T newValue, [CallerMemberName] string property = null)
        {
            variable = newValue;
            OnPropertyChanged(property, newValue);
        }

        /// <summary>
        /// Assigns a new value to a variable and calls OnPropertyChanged even if
        /// the value was not changed. Use this if the  property name that is reported
        /// for events needs to be different from the actual name of the property
        /// this is called from.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="variable">The variable to assign to.</param>
        /// <param name="newValue">The new value to assign.</param>
        /// <param name="property">The property name. This should be the
        /// plain property name (i.e. no nesting or dots).</param>
        /// <returns></returns>
        protected void AssignAlwaysRename<T>(ref T variable, T newValue, string property)
        {
            variable = newValue;
            OnPropertyChanged(property, newValue);
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

    /// <summary>
    /// If added to a property with a data type that inherits from ChangeableState,
    /// the substate will not be automatically subscribed to from the parent object,
    /// which prevents property change events from automatically passing from the
    /// property to the parent.
    /// </summary>
    public class NoSubstateAutoSubscribeAttribute : Attribute
    {
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
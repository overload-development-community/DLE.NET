using EditorUITest.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace EditorUITest.Editor
{
    internal delegate object GetPropertyValueSubDelegate(object root, string property);

    public class EditorStateBinder
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

        #endregion

        #region --- instance

        private readonly EditorState state;
        private readonly PropertyNameTree tree;
        private readonly Dictionary<string, PropertyInfo> propertyCache = new Dictionary<string, PropertyInfo>();

        private EditorStateBinder(EditorState state)
        {
            this.state = state;
            this.tree = new PropertyNameTree();
            state.PropertyChanged += State_PropertyChanged;
        }

        private void State_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            tree.Call(e, GetPropertyValueSub);
        }

        private Type GetPropertyType(string property)
        {
            object self = state;
            PropertyInfo tmp = null;
            foreach (string tok in property.Split('.'))
            {
                tmp = self.GetType().GetProperty(tok);
                if (tmp == null) return null;
                self = tmp.GetValue(self);
            }
            return tmp?.PropertyType;
        }

        private object GetPropertyValueSub(object root, string property)
        {
            object self = root;
            PropertyInfo tmp;
            foreach (string tok in property.Split('.'))
            {
                if (tok == "") continue;
                tmp = self.GetType().GetProperty(tok);
                if (tmp == null) return default;
                self = tmp.GetValue(self);
            }
            return self;
        }

        private bool SetPropertyValue(string property, object value)
        {
            object self = state;
            PropertyInfo tmp = null;
            foreach (string tok in property.Split('.'))
            {
                if (tmp != null) self = tmp.GetValue(self);
                tmp = self.GetType().GetProperty(tok);
                if (tmp == null) return false;
            }
            try
            {
                tmp.SetValue(self, value);
                return true;
            }
            catch (ArgumentException)
            {
                return false;
            }
        }

        private void AddToTreeAndCall(string property, PropertyChangedEventHandler handler)
        {
            tree.Add(property, handler);
            handler(this, new PropertyChangedEventArgs(property, GetPropertyValueSub(state, property)));
        }

        public void BindIntTextBox(IntTextBox textBox, string property, bool readOnly)
        {
            Type propertyType = GetPropertyType(property);
            if (propertyType == typeof(int))
            {
                AddToTreeAndCall(property, (object sender, PropertyChangedEventArgs e) => { textBox.Value = (int)e.NewValue; });
                if (!readOnly) textBox.ValueChanged += (object sender, EventArgs e) => { SetPropertyValue(property, (int)textBox.Value); };
            }
            else if (propertyType == null) throw new PropertyNotFoundException(property);
            else throw new InvalidPropertyException(property);
        }

        private EditorStateBinder() : this(null)
        {
        }

        #endregion
    }

    #region --- exception classes

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

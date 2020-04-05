using DLEDotNet.Editor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Util
{
    public class MultiDict<K, V> : Dictionary<K, List<V>>
    {
        public IEnumerable<V> GetOrEmpty(K key)
        {
            if (this.TryGetValue(key, out List<V> value))
                return value;
            else
                return Enumerable.Empty<V>();
        }

        private List<V> GetOrMakeEmpty(K key)
        {
            if (!this.ContainsKey(key))
                this[key] = new List<V>();
            return this[key];
        }

        public void Add(K key, V value)
        {
            GetOrMakeEmpty(key).Add(value);
        }

        public bool Contains(K key, V value)
        {
            return GetOrMakeEmpty(key).Contains(value);
        }

        public void Remove(K key, V value)
        {
            GetOrMakeEmpty(key).Remove(value);
        }

        public new void Clear()
        {
            foreach (List<V> value in this.Values)
                value.Clear();
            base.Clear();
        }
    }
}

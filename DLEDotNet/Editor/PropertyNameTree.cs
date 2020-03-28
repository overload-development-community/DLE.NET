using DLEDotNet.Data;
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    internal class PropertyNameTree
    {
        PropertyNameNode root;

        public PropertyNameTree()
        {
            root = new PropertyNameNode("");
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public PropertyNameNode Traverse(string key)
        {
            PropertyNameNode node = root;
            foreach (string tok in key.Split('.'))
            {
                if (tok == "") continue;
                node = node[tok];
                if (node == null) break;
            }
            return node;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public PropertyNameNode TraverseOrCreate(string key)
        {
            PropertyNameNode node = root;
            foreach (string tok in key.Split('.'))
            {
                if (tok == "") continue;
                node = node.GetOrCreate(tok);
            }
            return node;
        }

        public void Add(string property, PropertyChangeEventHandler callback)
        {
            TraverseOrCreate(property).Leaves.Add(callback);
        }

        public void Call(PropertyChangeEventArgs e, GetPropertyValueUncachedDelegate GetPropertyValueUncached)
        {
            Traverse(e.PropertyName)?.CallAllRecursive(e.PropertyName, e.NewValue, GetPropertyValueUncached, true);
        }

        public bool GetCachedEntry(string key, out PropertyCacheEntry entry)
        {
            entry = new PropertyCacheEntry();
            PropertyNameNode node = Traverse(key);
            if (node != null)
            {
                entry = node.Cache;
                return entry.Parent != null;
            }
            return false;
        }

        public void Cache(string key, PropertyCacheEntry entry)
        {
            TraverseOrCreate(key).Cache = entry;
        }
    }

    internal class PropertyNameNode
    {
        private readonly string word;
        private readonly Dictionary<string, PropertyNameNode> children;
        private readonly List<PropertyChangeEventHandler> leaves;
        private PropertyCacheEntry propertyCache;

        public PropertyNameNode(string word)
        {
            this.word = word;
            this.children = new Dictionary<string, PropertyNameNode>();
            this.leaves = new List<PropertyChangeEventHandler>();
            this.propertyCache = new PropertyCacheEntry();
        }

        internal string Name
        {
            get => word;
        }

        internal PropertyNameNode this[string word]
        {
            get
            {
                bool gotIt = children.TryGetValue(word, out PropertyNameNode value);
                return gotIt ? value : null;
            }
        }

        internal PropertyNameNode GetOrCreate(string word)
        {
            bool gotIt = children.TryGetValue(word, out PropertyNameNode value);
            if (!gotIt)
            {
                value = new PropertyNameNode(word);
                children[word] = value;
            }
            return value;
        }

        internal IEnumerable<PropertyNameNode> Children
        {
            get
            {
                return this.children.Values;
            }
        }

        internal List<PropertyChangeEventHandler> Leaves
        {
            get
            {
                return this.leaves;
            }
        }

        internal PropertyCacheEntry Cache
        {
            get
            {
                return this.propertyCache;
            }
            set
            {
                this.propertyCache = value;
            }
        }

        private void InvalidateCache()
        {
            this.propertyCache = new PropertyCacheEntry();
        }

        internal void CallAllRecursive(string basePath, object rootValue, GetPropertyValueUncachedDelegate GetPropertyValueUncached, bool top)
        {
            PropertyChangeEventArgs e = new PropertyChangeEventArgs(top ? basePath : basePath + "." + this.Name, top ? rootValue : GetPropertyValueUncached(rootValue, this.Name));
            if (!top) // since the root changed, we must invalidate cache
                this.InvalidateCache();
            foreach (PropertyChangeEventHandler EventHandler in this.Leaves)
                EventHandler(this, e);
            foreach (PropertyNameNode n in this.Children)
                n?.CallAllRecursive(basePath + "." + this.Name, e.NewValue, GetPropertyValueUncached, false);
        }
    }
}
using EditorUITest.Data;
using System.Collections.Generic;
using System.Windows.Forms;

namespace EditorUITest.Editor
{
    internal class PropertyNameTree
    {
        PropertyNameNode root;

        public PropertyNameTree()
        {
            root = new PropertyNameNode("");
        }

        public void Add(string property, PropertyChangedEventHandler callback)
        {
            PropertyNameNode node = root;
            foreach (string tok in property.Split('.'))
            {
                node = node.GetOrCreate(tok);
            }
            node.Leaves.Add(callback);
        }

        public void Call(PropertyChangedEventArgs e, GetPropertyValueSubDelegate GetPropertyValueSub)
        {
            PropertyNameNode node = root;
            foreach (string tok in e.PropertyName.Split('.'))
            {
                node = node[tok];
                if (node == null) return;
            }
            CallAllRecursive(node, e.PropertyName, "", e.NewValue, GetPropertyValueSub);
        }

        private void CallAllRecursive(PropertyNameNode node, string basePath, string nodePath, object rootValue, GetPropertyValueSubDelegate GetPropertyValueSub)
        {
            PropertyChangedEventArgs e = new PropertyChangedEventArgs(basePath + nodePath, nodePath == "" ? rootValue : GetPropertyValueSub(rootValue, nodePath));
            foreach (PropertyChangedEventHandler eh in node.Leaves)
            {
                eh(this, e);
            }
            foreach (PropertyNameNode n in node.Children)
            {
                CallAllRecursive(n, basePath + nodePath, "." + n.Name, e.NewValue, GetPropertyValueSub);
            }
        }
    }

    internal class PropertyNameNode
    {
        private readonly Dictionary<string, PropertyNameNode> children;
        private readonly List<PropertyChangedEventHandler> leaves;
        private readonly string word;

        public PropertyNameNode(string word)
        {
            this.word = word;
            this.children = new Dictionary<string, PropertyNameNode>();
            this.leaves = new List<PropertyChangedEventHandler>();
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

        internal List<PropertyChangedEventHandler> Leaves
        {
            get
            {
                return this.leaves;
            }
        }
    }
}
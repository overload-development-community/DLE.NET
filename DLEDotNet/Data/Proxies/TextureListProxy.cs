using DLEDotNet.Editor;
using DLEDotNet.ManagedWrappers;
using LibDescent.Edit;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace DLEDotNet.Data.Proxies
{
    public class TextureListProxy : ChangeableState, IReadOnlyList<TextureProxy>
    {
        private static TextureManager textureManager = TextureManager.Instance;

        public TextureListProxy()
        {
        }

        public TextureProxy this[int index] => new TextureProxy(textureManager.get_Textures(index));

        // used for binding, keep as IReadOnlyList
        public IReadOnlyList<TextureProxy> Items => this;

        public int Count => textureManager.Count;

        public IEnumerator<TextureProxy> GetEnumerator() => new TextureEnumerator(this);

        IEnumerator IEnumerable.GetEnumerator() => new TextureEnumerator(this);

        private sealed class TextureEnumerator : IEnumerator<TextureProxy>
        {
            private int currentIndex = -1;
            private TextureListProxy textureListProxy;

            public TextureEnumerator(TextureListProxy textureListProxy)
            {
                this.textureListProxy = textureListProxy;
            }

            public TextureProxy Current => new TextureProxy(textureManager.get_Textures(currentIndex));

            object IEnumerator.Current => Current;

            public void Dispose() { }

            public bool MoveNext()
            {
                currentIndex++;
                return currentIndex < textureListProxy.Count;
            }

            public void Reset()
            {
                currentIndex = -1;
            }
        }

        internal void Reload()
        {
            textureManager.Reload();
        }
    }

    public class TextureProxy : ChangeableStateProxy<Texture>, IListBoxable
    {
        public TextureProxy(Texture host) : base(host)
        {
        }

        public object DisplayValue => Host.TextureName;

        public static implicit operator Texture(TextureProxy v)
        {
            return v.Host;
        }
    }
}

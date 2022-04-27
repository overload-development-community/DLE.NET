using DLEDotNet.Editor;
using DLEDotNet.ManagedWrappers;
using LibDescent.Edit;
using System.Collections;
using System.Collections.Generic;

namespace DLEDotNet.Data.Proxies
{
    public class LevelObjectProxy : ChangeableStateProxy<LevelObject>, IListBoxable
    {
        public LevelObjectProxy(LevelObject host) : base(host)
        {
        }

        public object DisplayValue => Index;

        public int Index => Host.ObjectIndex;
    }

    public class LevelObjectList : ChangeableState, IReadOnlyList<LevelObjectProxy>
    {
        private Level level;

        public LevelObjectList(Level level)
        {
            this.level = level;
        }

        public LevelObjectProxy this[int index] => new LevelObjectProxy(level.get_Objects(index));

        // used for binding, keep as IReadOnlyList
        public IReadOnlyList<LevelObjectProxy> Items => this;

        public int Count => level.ObjectCount;

        public IEnumerator<LevelObjectProxy> GetEnumerator() => new LevelObjectEnumerator(this);

        IEnumerator IEnumerable.GetEnumerator() => new LevelObjectEnumerator(this);

        private sealed class LevelObjectEnumerator : IEnumerator<LevelObjectProxy>
        {
            private int currentIndex = -1;
            private LevelObjectList levelObjectList;

            public LevelObjectEnumerator(LevelObjectList levelObjectList)
            {
                this.levelObjectList = levelObjectList;
            }

            public LevelObjectProxy Current => levelObjectList[currentIndex];

            object IEnumerator.Current => Current;

            public void Dispose() { }

            public bool MoveNext()
            {
                currentIndex++;
                return currentIndex < levelObjectList.Count;
            }

            public void Reset()
            {
                currentIndex = -1;
            }
        }
    }
}

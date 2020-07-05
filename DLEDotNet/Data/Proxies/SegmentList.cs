using DLEDotNet.ManagedWrappers;
using LibDescent.Edit;
using System.Collections;
using System.Collections.Generic;

namespace DLEDotNet.Data.Proxies
{
    public class SegmentList : ChangeableState, IReadOnlyList<SegmentProxy>
    {
        private static SegmentManager segmentManager = SegmentManager.Instance;

        public SegmentList()
        {
        }

        public SegmentProxy this[int index] => new SegmentProxy(index);

        // used for binding, keep as IReadOnlyList
        public IReadOnlyList<SegmentProxy> Items => this;

        public int Count => segmentManager.Count;

        public IEnumerator<SegmentProxy> GetEnumerator() => new SegmentEnumerator(this);

        IEnumerator IEnumerable.GetEnumerator() => new SegmentEnumerator(this);

        private void OnListUpdate() => OnReadOnlyPropertyChanged(nameof(Items), this);

        private sealed class SegmentEnumerator : IEnumerator<SegmentProxy>
        {
            private int currentIndex = -1;
            private SegmentList segmentList;

            public SegmentEnumerator(SegmentList segmentList)
            {
                this.segmentList = segmentList;
            }

            public SegmentProxy Current => new SegmentProxy(currentIndex);

            object IEnumerator.Current => Current;

            public void Dispose() { }

            public bool MoveNext()
            {
                currentIndex++;
                return currentIndex < segmentList.Count;
            }

            public void Reset()
            {
                currentIndex = -1;
            }
        }
    }
}

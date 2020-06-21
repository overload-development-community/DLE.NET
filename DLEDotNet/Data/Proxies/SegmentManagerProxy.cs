using DLEDotNet.ManagedWrappers;
using LibDescent.Edit;
using System.Collections;
using System.Collections.Generic;

namespace DLEDotNet.Data.Proxies
{
    public class SegmentManagerProxy : ChangeableState
    {
        private SegmentManagerWrapper segmentManager = SegmentManagerWrapper.Instance;

        public SegmentList Segments { get; } = new SegmentList();

        public class SegmentList : IReadOnlyList<SegmentProxy>
        {
            public SegmentProxy this[int index] => new SegmentProxy(index);

            public int Count => SegmentManagerWrapper.Instance.Count;

            public IEnumerator<SegmentProxy> GetEnumerator() => new SegmentEnumerator(this);

            IEnumerator IEnumerable.GetEnumerator() => new SegmentEnumerator(this);

            public sealed class SegmentEnumerator : IEnumerator<SegmentProxy>
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
}

using DLEDotNet.Editor;
using LibDescent.Edit;

namespace DLEDotNet.Data.Proxies
{
    public class SegmentProxy : ChangeableState, IListBoxable
    {
        private int _segmentId;

        public SegmentProxy(int segmentId)
        {
            _segmentId = segmentId;
        }

        object IListBoxable.DisplayValue => _segmentId;

        public class Side
        {
        }

        public class Edge
        {
        }

        public class Point
        {
        }
    }
}

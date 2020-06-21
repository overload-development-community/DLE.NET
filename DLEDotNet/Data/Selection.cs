using DLEDotNet.Data.Proxies;
using LibDescent.Edit;

namespace DLEDotNet.Data
{
    public class Selection : ChangeableState
    {
        private SegmentProxy _segment;
        private SegmentProxy.Side _side;
        private SegmentProxy.Edge _edge;
        private SegmentProxy.Point _point;
        private LevelObjectProxy _object;

        public SegmentProxy Segment
        {
            get => _segment;
            set => AssignChanged(ref _segment, value);
        }

        public SegmentProxy.Side Side
        {
            get => _side;
            set => AssignChanged(ref _side, value);
        }

        public SegmentProxy.Edge Edge
        {
            get => _edge;
            set => AssignChanged(ref _edge, value);
        }

        public SegmentProxy.Point Point
        {
            get => _point;
            set => AssignChanged(ref _point, value);
        }

        public LevelObjectProxy Object
        {
            get => _object;
            set => AssignChanged(ref _object, value);
        }
    }
}
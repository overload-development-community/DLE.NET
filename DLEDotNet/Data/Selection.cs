using DLEDotNet.Data.Proxies;
using LibDescent.Edit;

namespace DLEDotNet.Data
{
    public class Selection : ChangeableState
    {
        private readonly EditorState _state;
        private SegmentProxy _segment;
        private SegmentProxy.Side _side;
        private SegmentProxy.Point _point;
        private LevelObjectProxy _object;

        public Selection(EditorState state) : this(state, 0, 0, 0) { }

        public Selection(EditorState state, int segmentNum, int sideNum, int pointNum)
        {
            _state = state;
            _segment = _state.SegmentManager.Segments[segmentNum];
            _side = _segment.Sides[sideNum];
            _point = _side.Points[pointNum];
        }

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
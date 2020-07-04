using DLEDotNet.Data.Proxies;
using LibDescent.Edit;

namespace DLEDotNet.Data
{
    public class Selection : ChangeableState
    {
        private readonly EditorState _state;
        private SegmentProxy _segment;
        private int _sideNum;
        private int _pointNum;
        private LevelObjectProxy _object;

        public Selection(EditorState state) : this(state, 0, 0, 0) { }

        public Selection(EditorState state, int segmentNum, int sideNum, int pointNum)
        {
            _state = state;
            _segment = _state.SegmentManager.Segments[segmentNum];
            _sideNum = sideNum;
            _pointNum = pointNum;
        }

        public SegmentProxy Segment
        {
            get => _segment;
            set => AssignChanged(ref _segment, value);
        }

        public int SideNum
        {
            get => _sideNum;
            set
            {
                AssignChanged(ref _sideNum, value);
                OnReadOnlyPropertyChanged(nameof(Side), Side);
            }
        }

        public SegmentProxy.Side Side
        {
            get => Segment.Sides[_sideNum];
            set => SideNum = value.SideNum;
        }

        public int PointNum
        {
            get => _pointNum;
            set
            {
                AssignChanged(ref _pointNum, value);
                OnReadOnlyPropertyChanged(nameof(Point), Point);
            }
        }

        public SegmentProxy.Point Point
        {
            get => Side.Points[_pointNum];
            set => PointNum = value.PointNum;
        }

        public LevelObjectProxy Object
        {
            get => _object;
            set => AssignChanged(ref _object, value);
        }
    }
}
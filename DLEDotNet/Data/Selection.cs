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
        private int _sideNum;
        private int _pointNum;
        private LevelObjectProxy _object;

        public Selection(EditorState state) : this(state, 0, 0, 0) { }

        public Selection(EditorState state, int segmentNum, int sideNum, int pointNum)
        {
            _state = state;
            _segment = _state.Level.Segments[segmentNum];
            _side = _segment.Sides[_sideNum = sideNum];
            _point = _side.Points[_pointNum = pointNum];
        }

        public SegmentProxy Segment
        {
            get => _segment;
            set
            {
                AssignChanged(ref _segment, value);
                // force update of Side (and indirectly Point)
                SideNum = SideNum;
            }
        }

        public int SideNum
        {
            get => _sideNum;
            set => Side = Segment.Sides[value];
        }

        public SegmentProxy.Side Side
        {
            get => _side;
            set
            {
                AssignChanged(ref _side, value);
                AssignChangedRename(ref _sideNum, value.SideNum, nameof(SideNum));
                // force update of Point
                PointNum = PointNum;
            }
        }

        public int PointNum
        {
            get => _pointNum;
            set => Point = _side.Points[value];
        }

        public SegmentProxy.Point Point
        {
            get => _point;
            set
            {
                AssignChanged(ref _point, value);
                AssignChangedRename(ref _pointNum, value.PointNum, nameof(PointNum));
            }
        }

        public LevelObjectProxy Object
        {
            get => _object;
            set => AssignChanged(ref _object, value);
        }
    }
}
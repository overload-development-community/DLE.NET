using DLEDotNet.Editor;
using DLEDotNet.ManagedWrappers;
using LibDescent.Data;
using LibDescent.Edit;
using System.Collections.Generic;

namespace DLEDotNet.Data.Proxies
{
    public class SegmentProxy : ChangeableState, IListBoxable
    {
        private readonly int _segmentId;

        public SegmentProxy(int segmentId)
        {
            _segmentId = segmentId;
            for (int sideNum = 0; sideNum < Segment.NumSides; sideNum++)
            {
                Sides.Add(new Side(Segment, sideNum));
            }
        }

        private ManagedWrappers.Segment Segment => SegmentManager.Instance.get_Segments(_segmentId);

        public SegFunction Function
        {
            get => Segment.Function;
            set
            {
                Segment.Function = value;
                OnReadOnlyPropertyChanged(nameof(Function), value);
            }
        }

        object IListBoxable.DisplayValue => _segmentId;

        public List<Side> Sides { get; } = new List<Side>();

        public static implicit operator ManagedWrappers.Segment(SegmentProxy segment) => segment.Segment;

        public class Side : IListBoxable
        {
            private readonly ManagedWrappers.Segment _segment;
            private readonly ManagedWrappers.Side _side;

            internal Side(ManagedWrappers.Segment segment, int sideNum)
            {
                _segment = segment;
                SideNum = sideNum;
                _side = _segment.get_Sides(SideNum);
                for (int pointNum = 0; pointNum < _side.NumPoints; pointNum++)
                {
                    Points.Add(new Point(_side, pointNum));
                }
            }

            public int SideNum { get; }

            public List<Point> Points { get; } = new List<Point>();

            object IListBoxable.DisplayValue => SideNum;

            public static implicit operator ManagedWrappers.Side(Side side) => side._side;
        }

        public class Point : ChangeableState, IListBoxable
        {
            private readonly ManagedWrappers.Side _side;

            internal Point(ManagedWrappers.Side side, int pointNum)
            {
                _side = side;
                PointNum = pointNum;
            }

            public int PointNum { get; }

            private Vertex Vertex => _side.get_Points(PointNum);

            object IListBoxable.DisplayValue => PointNum;

            public double X
            {
                get => Vertex.X;
                set
                {
                    Vertex.X = value;
                    OnReadOnlyPropertyChanged(nameof(X), value);
                }
            }

            public double Y
            {
                get => Vertex.Y;
                set
                {
                    Vertex.Y = value;
                    OnReadOnlyPropertyChanged(nameof(Y), value);
                }
            }

            public double Z
            {
                get => Vertex.Z;
                set
                {
                    Vertex.Z = value;
                    OnReadOnlyPropertyChanged(nameof(Z), value);
                }
            }
        }
    }
}

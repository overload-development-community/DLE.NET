using DLEDotNet.Editor;
using DLEDotNet.ManagedWrappers;
using LibDescent.Data;
using LibDescent.Edit;
using System.Collections.Generic;

namespace DLEDotNet.Data.Proxies
{
    public class SegmentProxy : ChangeableState, IListBoxable
    {
        private int _segmentId;

        public SegmentProxy(int segmentId)
        {
            _segmentId = segmentId;
            for (int sideNum = 0; sideNum < Segment.NumSides; sideNum++)
            {
                Sides.Add(new Side(Segment.get_Sides(sideNum)));
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

        public class Side
        {
            private ManagedWrappers.Side _side;

            public Side(ManagedWrappers.Side side)
            {
                _side = side;
                for (int pointNum = 0; pointNum < _side.NumPoints; pointNum++)
                {
                    Points.Add(new Point(_side.get_Points(pointNum)));
                }
            }

            public List<Point> Points { get; } = new List<Point>();
        }

        public class Point : ChangeableState
        {
            private Vertex _vertex;

            public Point(Vertex vertex)
            {
                _vertex = vertex;
            }

            public double X
            {
                get => _vertex.X;
                set
                {
                    _vertex.X = value;
                    OnReadOnlyPropertyChanged(nameof(X), value);
                }
            }

            public double Y
            {
                get => _vertex.Y;
                set
                {
                    _vertex.Y = value;
                    OnReadOnlyPropertyChanged(nameof(Y), value);
                }
            }

            public double Z
            {
                get => _vertex.Z;
                set
                {
                    _vertex.Z = value;
                    OnReadOnlyPropertyChanged(nameof(Z), value);
                }
            }
        }
    }
}

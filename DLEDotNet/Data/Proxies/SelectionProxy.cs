using DLEDotNet.ManagedWrappers;

namespace DLEDotNet.Data.Proxies
{
    public class SelectionProxy : ISelection
    {
        private Segment segment;
        private Side side;
        private int pointNum;
        private int objectId;

        public SelectionProxy(Selection selection)
        {
            segment = selection.Segment;
            side = selection.Side;
            pointNum = selection.PointNum;
            // Object manager not implemented yet
            objectId = 0; //selection.Object;
        }

        public short ObjectId { get => (short)objectId; }
        public short PointNum { get => (short)pointNum; }
        public short SideId { get => (short)segment.IndexOf(side); }
        public short SegmentId { get => (short)SegmentManager.Instance.IndexOf(segment); }
    }
}

using DLEDotNet.ManagedWrappers;

namespace DLEDotNet.Data.Proxies
{
    public class SelectionProxy : ISelection
    {
        public SelectionProxy(Selection selection)
        {
            SegmentId = (short)SegmentManager.Instance.IndexOf(selection.Segment);
            SideId = (short)selection.Segment.Sides.IndexOf(selection.Side);
            PointNum = (short)selection.PointNum;
            ObjectId = (short)(selection.Object?.Index ?? 0);
        }

        public short ObjectId { get; set; }
        public short PointNum { get; set; }
        public short SideId { get; set; }
        public short SegmentId { get; set; }
    }
}

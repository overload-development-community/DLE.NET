// this is a placeholder class that is used to test bindings

using EditorUITest.Data;
using System.Collections.Generic;

namespace EditorUITest.Placeholder
{
    public class Level : ChangeableState
    {
        public Level() : base()
        {
            SegmentCount = 10;
        }

        public List<Segment> SegmentList { get; } = new List<Segment>();
        public uint SegmentCount
        {
            get
            {
                return _segmentCount;
            }
            set
            {
                if (AssignChanged(ref _segmentCount, value, "SegmentCount"))
                {
                    SegmentList.Clear();
                    for (uint i = 0; i < SegmentCount; ++i)
                    {
                        SegmentList.Add(new Segment(i));
                    }
                    OnPropertyChanged("SegmentList", SegmentList);
                }
            }
        }

        private uint _segmentCount;
    }
}
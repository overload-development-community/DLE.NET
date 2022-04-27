using DLEDotNet.ManagedWrappers;
using LibDescent.Edit;
using System;

namespace DLEDotNet.Data.Proxies
{
    public class LevelProxy : ChangeableStateProxy<Level>
    {
        public LevelProxy(Level host) : base(host)
        {
            Segments = Subscribe(nameof(Segments), new SegmentList());
        }

        public static LevelProxy New() => new LevelProxy(new Level());

        public static LevelProxy Open(string fileName)
        {
            var level = new Level();
            level.Open(fileName);
            return new LevelProxy(level);
        }

        public SegmentList Segments { get; }
    }
}

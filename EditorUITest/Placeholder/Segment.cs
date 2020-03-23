using EditorUITest.Data;

namespace EditorUITest.Placeholder
{
    public class Segment : ChangeableState
    {
        public uint Number { get; set; } = 0;

        public Segment(uint number)
        {
            Number = number;
        }
    }
}
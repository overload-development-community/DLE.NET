// this is a placeholder class that is used to test bindings

using EditorUITest.Data;
using EditorUITest.Editor;
using LibDescent.Data;

namespace EditorUITest.Placeholder
{
    public class Segment : ChangeableState, IListBoxable
    {
        public uint Number { get; } = 0;
        public Fix Light
        {
            get => _light;
            set
            {
                AssignChanged(ref _light, value, "Light");
            }
        }

        public object DisplayValue => Number;

        public Segment(uint number)
        {
            Number = number;
            Light = number * 0.01;
        }

        private Fix _light;

        private Segment() { }
    }
}
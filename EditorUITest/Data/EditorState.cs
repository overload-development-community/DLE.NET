using EditorUITest.Placeholder;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EditorUITest.Data
{
    public class EditorState : ChangeableState
    {
        /// <summary>
        /// The currently opened level.
        /// </summary>
        public Level Level
        {
            get => _currentLevel;
            set
            {
                AssignChanged(ref _currentLevel, value, "Level");
            }
        }
        
        /// <summary>
        /// The currently selected segment.
        /// </summary>
        public Segment Segment
        {
            get => _selectedSegment;
            set
            {
                AssignChanged(ref _selectedSegment, value, "Segment");
            }
        }

        public int Temp
        {
            get => _tmp;
            set
            {
                if (AssignChanged(ref _tmp, value, "Temp"))
                    System.Diagnostics.Debug.WriteLine("New value for _tmp = " + value);
            }
        }

        private int _tmp = 1337;

        private Level _currentLevel = null;
        private Segment _selectedSegment = null;
    }
}

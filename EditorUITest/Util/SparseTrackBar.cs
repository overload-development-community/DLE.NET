using EditorUITest.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    [System.ComponentModel.DesignerCategory("Code")]
    public class SparseTrackBar : TrackBar
    {
        private int _smallChange = 1;
        public SparseTrackBar() : base()
        {
            base.SmallChange = 1;
        }

        [Description("The number of positions the slider moves in response to keyboard input (arrow keys), and also the distance in value between two sequential user-selectable values.")]
        public new int SmallChange
        {
            get
            {
                return _smallChange;
            }
            set
            {
                if (value < 1)
                {
                    throw new ArgumentOutOfRangeException();
                }
                base.LargeChange = base.LargeChange * _smallChange / value;
                base.Minimum = base.Minimum * _smallChange / value;
                base.Maximum = base.Maximum * _smallChange / value;
                base.TickFrequency = base.TickFrequency * _smallChange / value;
                base.Value = MathUtilities.RoundDiv2(2 * base.Value * _smallChange / value);
                base.SmallChange = 1;
                _smallChange = value;
            }
        }

        public new int LargeChange
        {
            get
            {
                return base.LargeChange * _smallChange;
            }
            set
            {
                base.LargeChange = value / _smallChange;
            }
        }

        public new int Minimum
        {
            get
            {
                return base.Minimum * _smallChange;
            }
            set
            {
                base.Minimum = value / _smallChange;
            }
        }

        public new int Maximum
        {
            get
            {
                return base.Maximum * _smallChange;
            }
            set
            {
                base.Maximum = value / _smallChange;
            }
        }

        public new int TickFrequency
        {
            get
            {
                return base.TickFrequency * _smallChange;
            }
            set
            {
                base.TickFrequency = value / _smallChange;
            }
        }

        public new int Value
        {
            get
            {
                return base.Value * _smallChange;
            }
            set
            {
                base.Value = MathUtilities.RoundDiv2(2 * value / _smallChange);
            }
        }
    }
}

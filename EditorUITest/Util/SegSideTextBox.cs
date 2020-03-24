using EditorUITest.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    [System.ComponentModel.DesignerCategory("Code")]
    public class SegSideTextBox : TextBox, IValidatable
    {
        private SegmentSide _res;

        /// <summary>
        /// Whether the side is zero-based for this control (0-5 as opposed to 1-6).
        /// </summary>
        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("Whether the side is zero-based for this control (0-5 as opposed to 1-6).")]
        public bool ZeroBased { get; set; } = false;

        public int MaxSegmentCount { get; set; } = 0;

        public SegSideTextBox() : base()
        {
            this.TextChanged += SegSideTextBox_removenonint;
            this.Validating += SegSideTextBox_Validating;
            this._res.Valid = false;
            this._res.Segment = UInt32.MaxValue;
            this._res.Side = UInt32.MaxValue;
        }

        public SegmentSide Value
        {
            get
            {
                return _res;
            }
        }

        private void SegSideTextBox_Validating(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Validate();
        }

        public void Validate()
        {
            this._res = ParseSegmentSide(this.Text);
        }

        private SegmentSide ParseSegmentSide(string text)
        {
            SegmentSide ss;
            ss.Valid = false;
            ss.Segment = UInt32.MaxValue;
            ss.Side = UInt32.MaxValue;

            string[] commaSplit = text.Split(',');
            
            if (commaSplit.Length == 2 && commaSplit[0].Length > 0 && commaSplit[1].Length > 0)
            {
                if (uint.TryParse(commaSplit[0], out uint seg) && uint.TryParse(commaSplit[1], out uint side))
                {
                    ss.Segment = seg;
                    ss.Side = side;
                    ss.Valid = (ZeroBased ? 0 <= side && side <= 5 : 1 <= side && side <= 6) && seg < MaxSegmentCount;
                }
            }

            return ss;
        }

        private void SegSideTextBox_removenonint(object sender, EventArgs e)
        {
            bool metComma = false;

            StringBuilder newTextBuilder = new StringBuilder();
            foreach (char c in this.Text)
            {
                if (!metComma && c == ',')
                {
                    newTextBuilder.Append(c);
                    metComma = true;
                }
                else if ('0' <= c && c <= '9')
                {
                    newTextBuilder.Append(c);
                }
            }
            string newText = newTextBuilder.ToString();

            if (this.Text != newText)
            {
                int start = this.SelectionStart;
                int len = this.SelectionLength;
                this.Text = newText;
                this.SelectionStart = start;
                this.SelectionLength = len;
            }
        }
    }

    public struct SegmentSide
    {
        public bool Valid;
        public uint Segment;
        public uint Side;

        public override string ToString()
        {
            if (Valid)
                return "Seg=" + Segment + ", Side=" + Side;
            else
                return "<Invalid input!>";
        }
    }
}

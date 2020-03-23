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
    public class FloatTextBox : TextBox
    {
        private double _value;
        private double _minValue = Double.NegativeInfinity;
        private double _maxValue = Double.PositiveInfinity;
        public FloatTextBox() : base()
        {
            this.TextChanged += FloatTextBox_removenonfloat;
            this.Validating += FloatTextBox_Validating;
            this.Value = 0;
        }

        public event EventHandler ValueChanged;

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(0.0)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The floating-point value of this text box.")]
        public double Value
        { 
            get
            {
                return _value;
            }
            set
            {
                double bounded = Math.Max(Math.Min(value, this.MaximumValue), this.MinimumValue);
                if (!MathUtilities.IsFinite(bounded))
                {
                    bounded = 0;
                }
                this.Text = bounded.ToString("F" + FractionalDigits);
                bool callEvent = _value != bounded;
                _value = bounded;
                if (callEvent) ValueChanged?.Invoke(this, new EventArgs());
            }
        }

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(Double.NegativeInfinity)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The smallest floating-point value this text box can have.")]
        public double MinimumValue
        {
            get
            {
                return _minValue;
            }
            set
            {
                _minValue = value;
                _maxValue = Math.Max(value, _maxValue);
                Value = Math.Max(Value, value);
            }
        }

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(Double.PositiveInfinity)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The largest floating-point value this text box can have.")]
        public double MaximumValue
        {
            get
            {
                return _maxValue;
            }
            set
            {
                _maxValue = value;
                _minValue = Math.Min(value, _minValue);
                Value = Math.Min(Value, value);
            }
        }

        [Bindable(true)]
        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue(2)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The number of fractional digits to show.")]
        public uint FractionalDigits { get; set; } = 2;

        private void FloatTextBox_Validating(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (this.Text == "")
                this.Text = "0.00";
            if (Double.TryParse(this.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out double f))
            {
                this.Value = f;
            }
            else
            {
                this.Value = 0;
            }
        }

        private void FloatTextBox_removenonfloat(object sender, EventArgs e)
        {
            bool metSign = false;
            bool metPoint = false;
            bool metSecondPoint = false;
            bool metExp = false;

            StringBuilder newTextBuilder = new StringBuilder();
            foreach (char c in this.Text)
            {
                if (!metSign && (c == '-' || c == '+'))
                {
                    newTextBuilder.Append(c);
                    metSign = true;
                }
                else if ('0' <= c && c <= '9')
                {
                    metSign = true;
                    if (!metSecondPoint)
                        newTextBuilder.Append(c);
                }
                else if (c == '.' || c == ',')
                {
                    if (metPoint)
                    {
                        metSecondPoint = true;
                    }
                    else
                    {
                        metPoint = true;
                        newTextBuilder.Append('.');
                    }
                }
                else if (!metExp && (c == 'e' || c == 'E'))
                {
                    metExp = true;
                    metSign = false;
                    newTextBuilder.Append('e');
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
}

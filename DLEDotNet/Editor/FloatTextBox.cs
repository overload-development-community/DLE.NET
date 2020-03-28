using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    [DesignerCategory("Code")]
    [Description("A validatable text box that takes in an floating-point value from the user.")]
    public class FloatTextBox : ValidatableTextBox
    {
        private double _value;
        private double _minValue = double.NegativeInfinity;
        private double _maxValue = double.PositiveInfinity;
        private int _fractionalDigits = 2;
        private string formatString = "{0:0.##}";
        public FloatTextBox() : base()
        {
            TextChanged += FloatTextBox_removenonfloat;
            Validating += FloatTextBox_Validating;
            ValueChanged += (object sender, EventArgs e) => UpdateBindings();
            Value = 0;
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
                double bounded = MathUtil.Clamp(value, MinimumValue, MaximumValue);
                if (!MathUtil.IsFinite(bounded))
                {
                    bounded = 0;
                }
                TextChanged -= FloatTextBox_removenonfloat; // disable event temporarily; optimization
                if (ZeroPadFraction)
                    Text = bounded.ToString("F" + FractionalDigits, CultureInfo.InvariantCulture);
                else
                    Text = String.Format(CultureInfo.InvariantCulture, formatString, bounded);
                TextChanged += FloatTextBox_removenonfloat;
                bool callEvent = _value != bounded;
                _value = bounded;
                if (callEvent) ValueChanged?.Invoke(this, new EventArgs());
            }
        }

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(double.NegativeInfinity)]
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
        [DefaultValue(double.PositiveInfinity)]
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
        [DefaultValue(true)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("Whether to always display as many fractional digits as configured.")]
        public bool ZeroPadFraction { get; set; } = true;

        [Bindable(true)]
        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue(2)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The number of fractional digits to show.")]
        public int FractionalDigits
        {
            get => _fractionalDigits;
            set
            {
                if (value < 0)
                {
                    throw new ArgumentOutOfRangeException("FractionalDigits");
                }
                _fractionalDigits = value;
                formatString = value > 0 ? "{0:0." + new string('#', value) + "}" : "{0:0}";
            }
        }

        public override void Validate()
        {
            if (Text == "")
                Text = "0.00";
            if (double.TryParse(Text, NumberStyles.Any, CultureInfo.InvariantCulture, out double f))
            {
                Value = f;
            }
            else
            {
                Value = 0;
            }
        }

        private void FloatTextBox_Validating(object sender, CancelEventArgs e)
        {
            Validate();
        }

        private void FloatTextBox_removenonfloat(object sender, EventArgs e)
        {
            bool metSign = false;
            bool metPoint = false;
            bool metSecondPoint = false;
            bool metExp = false;

            StringBuilder newTextBuilder = new StringBuilder();
            foreach (char c in Text)
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

            if (Text != newText)
            {
                int start = SelectionStart;
                int len = SelectionLength;
                Text = newText;
                SelectionStart = start;
                SelectionLength = len;
            }
        }
    }
}

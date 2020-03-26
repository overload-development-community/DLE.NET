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
    [Description("A validatable text box that takes in an integer value from the user.")]
    public class IntTextBox : ValidatableTextBox
    {
        private int _value;
        private int _minValue = int.MinValue;
        private int _maxValue = int.MaxValue;
        public IntTextBox() : base()
        {
            TextChanged += IntTextBox_removenonint;
            Validating += IntTextBox_Validating;
            ValueChanged += (object sender, EventArgs e) => UpdateBindings();
            Value = 0;
        }

        public event EventHandler ValueChanged;

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(0)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The integer value of this text box.")]
        public int Value
        {
            get
            {
                return _value;
            }
            set
            {
                int bounded = Math.Max(Math.Min(value, MaximumValue), MinimumValue);
                TextChanged -= IntTextBox_removenonint; // disable event temporarily; optimization
                Text = bounded.ToString();
                TextChanged += IntTextBox_removenonint;
                bool callEvent = _value != bounded;
                _value = bounded;
                if (callEvent) ValueChanged?.Invoke(this, new EventArgs());
            }
        }

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(int.MinValue)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The smallest integer value this text box can have.")]
        public int MinimumValue
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
        [DefaultValue(int.MaxValue)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The largest integer value this text box can have.")]
        public int MaximumValue
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

        public override void Validate()
        {
            if (Text == "")
                Text = "0";
            if (int.TryParse(Text, NumberStyles.Any, CultureInfo.InvariantCulture, out int f))
            {
                Value = f;
            }
            else
            {
                Value = 0;
            }
        }

        private void IntTextBox_Validating(object sender, CancelEventArgs e)
        {
            Validate();
        }

        private void IntTextBox_removenonint(object sender, EventArgs e)
        {
            bool metSign = false;

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
                    newTextBuilder.Append(c);
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

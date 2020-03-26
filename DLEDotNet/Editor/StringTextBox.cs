using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    [DesignerCategory("Code")]
    [Description("A validatable text box that takes in text input from the user.")]
    public class StringTextBox : ValidatableTextBox
    {
        private string _value;
        public StringTextBox() : base()
        {
            Validating += StringTextBox_Validating;
            ValueChanged += (object sender, EventArgs e) => UpdateBindings();
            Value = Text;
        }

        public event EventHandler ValueChanged;

        [Bindable(true)]
        [Browsable(true)]
        [Category("Behavior")]
        [DefaultValue(0)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The text value of this text box. This is automatically updated from the text on validation.")]
        public string Value
        {
            get
            {
                return _value;
            }
            set
            {
                Text = value;
                bool callEvent = _value != value;
                _value = value;
                if (callEvent) ValueChanged?.Invoke(this, new EventArgs());
            }
        }

        public override void Validate()
        {
            if (Value != Text)
                Value = Text;
        }

        private void StringTextBox_Validating(object sender, CancelEventArgs e)
        {
            Validate();
        }
    }
}

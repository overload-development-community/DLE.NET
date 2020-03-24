using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Util
{
    [System.ComponentModel.DesignerCategory("Code")]
    public class StringTextBox : TextBox, IValidatable
    {
        private string _value;
        public StringTextBox() : base()
        {
            this.Validating += StringTextBox_Validating;
            this.Value = this.Text;
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
                this.Text = value;
                bool callEvent = _value != value;
                _value = value;
                if (callEvent) ValueChanged?.Invoke(this, new EventArgs());
            }
        }

        public void Validate()
        {
            if (this.Value != this.Text)
                this.Value = this.Text;
        }

        private void StringTextBox_Validating(object sender, CancelEventArgs e)
        {
            Validate();
        }
    }
}

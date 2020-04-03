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
    public abstract class ValidatableTextBox : TextBox, IValidatable
    {
        public ValidatableTextBox() : base()
        {
            this.PreviewKeyDown += ValidatableTextBox_PreviewKeyDown;
            this.KeyDown += ValidatableTextBox_KeyDown;
        }

        private void ValidatableTextBox_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (!Multiline && e.KeyCode == Keys.Enter)
                e.IsInputKey = true;
        }

        private void ValidatableTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (!Multiline && e.KeyCode == Keys.Enter)
            {
                this.Validate();
                e.SuppressKeyPress = true;
            }
        }

        protected void UpdateBindings()
        {
            foreach (Binding b in this.DataBindings)
            {
                b.WriteValue();
                // maybe there are extra constraints applied by the property?
                b.ReadValue();
            }
        }

        public abstract void Validate();
    }
}

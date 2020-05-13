using System;
using System.Windows.Forms;

namespace DLEDotNet.Util
{
    internal class MouseClickMessageFilter : IMessageFilter
    {
        private const int WM_LBUTTONDOWN = 0x0201;
        private const int WM_RBUTTONDOWN = 0x0204;
        private const int WM_MBUTTONDOWN = 0x0207;

        private Form form;
        public event EventHandler Click;

        public MouseClickMessageFilter(Form form)
        {
            this.form = form;
        }

        public bool PreFilterMessage(ref Message m)
        {
            if ((m.Msg == WM_LBUTTONDOWN || m.Msg == WM_RBUTTONDOWN || m.Msg == WM_MBUTTONDOWN) && form.Equals(Form.ActiveForm))
                Click?.Invoke(this, new EventArgs());
            return false;
        }
    }
}
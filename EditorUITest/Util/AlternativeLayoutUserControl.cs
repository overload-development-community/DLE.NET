using System;
using System.Windows.Forms;

namespace EditorUITest
{
    public class AlternativeLayoutUserControl : UserControl
    {
        public AlternativeLayoutUserControl() : base()
        {
        }

        private void ShouldNeverBeShown()
        {
            // Do not add events to alternative layout handlers, or show them to the user directly
            throw new InvalidOperationException("Alternative layout user controls should never be shown to the user. Use the main layout and switch to the alternative layout by using the DialogLayout property.");
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            if (!DesignMode) ShouldNeverBeShown();
        }
    }
}
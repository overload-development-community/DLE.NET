using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EditorUITest.Editor.Layouts.Vertical;

namespace EditorUITest.Editor.Layouts
{
    public partial class PreferencesRenderer : DLELayoutableUserControl
    {
        public PreferencesRenderer()
        {
            InitializeComponent();
        }

        internal override void InitializeLayoutInternal() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                case LayoutOrientation.FLOATING:
                    return typeof(VerticalPreferencesRenderer);
            }
            return null;
        }

        private void prefsViewdistTrackBar_ValueChanged(object sender, EventArgs e)
        {
            int newValue = prefsViewdistTrackBar.Value;
            if (newValue == 0)
                prefsViewdistTextLabel.Text = "all";
            else
                prefsViewdistTextLabel.Text = newValue.ToString();
        }
    }
}

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
            InitializeLayout();
        }

        public void InitializeLayout()
        {
            this.Controls.Clear();
            InitializeComponent();
            switch (this.DialogLayout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                case LayoutOrientation.FLOATING:
                    this.CopyLayout(typeof(VerticalPreferencesRenderer));
                    break;
            }
        }

        public override void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        public override void SelfTest()
        {
            DoSelfTestFor(typeof(VerticalPreferencesRenderer));
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

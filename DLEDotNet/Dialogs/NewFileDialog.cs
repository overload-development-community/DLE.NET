using DLEDotNet.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Dialogs
{
    public partial class NewFileDialog : Form
    {
        public string LevelName { get; private set; }
        public LevelGameKind LevelType { get; private set; }

        public NewFileDialog()
        {
            InitializeComponent();
        }

        private void idokButton_Click(object sender, EventArgs e)
        {
            LevelName = this.levelnameTextBox.Text;

            if (this.d1LevelRadioButton.Checked)
                LevelType = LevelGameKind.D1;
            else if (this.d2LevelRadioButton.Checked)
                LevelType = LevelGameKind.D2;
            else if (this.d2VlevelRadioButton.Checked)
                LevelType = LevelGameKind.D2Vertigo;
            else if (this.d2XlevelRadioButton.Checked)
                LevelType = LevelGameKind.D2XXL;
        }
    }
}

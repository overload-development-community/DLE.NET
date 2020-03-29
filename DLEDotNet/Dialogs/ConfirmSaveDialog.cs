using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Dialogs
{
    public partial class ConfirmSaveDialog : Form
    {
        public ConfirmSaveDialog()
        {
            InitializeComponent();
            this.idiOldIconPictureBox.Image = Bitmap.FromHicon(new Icon(Icon.ExtractAssociatedIcon(Assembly.GetEntryAssembly().Location), idiOldIconPictureBox.Size).Handle);
        }
    }
}

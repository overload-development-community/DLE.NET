using System.Drawing;
using System.Reflection;
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

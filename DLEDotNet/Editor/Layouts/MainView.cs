using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor.Layouts
{
    public partial class MainView : UserControl
    {
        public MainView()
        {
            InitializeComponent();
        }

        public virtual TextureList GetTextureList()
        {
            throw new NotImplementedException();
        }

        public virtual MineView GetMineView()
        {
            throw new NotImplementedException();
        }

        public virtual EditorTabContainer GetEditorTabs()
        {
            throw new NotImplementedException();
        }

        public virtual void SetFullScreen(bool fullScreen)
        {
            throw new NotImplementedException();
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor.Layouts.Vertical
{
    [ToolboxItem(false)]
    public partial class MainViewVertical : MainView
    {
        public MainViewVertical()
        {
            InitializeComponent();
        }

        public override TextureList GetTextureList()
        {
            return textureList;
        }

        public override MineView GetMineView()
        {
            return mineView;
        }

        public override EditorTabContainer GetEditorTabs()
        {
            return editorTabs;
        }

        public override void SetFullScreen(bool fullScreen)
        {
            splitContainerTexture.Panel1Collapsed = splitContainerEditor.Panel2Collapsed = fullScreen;
        }
    }
}

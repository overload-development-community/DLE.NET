using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Editor.Layouts
{
    public partial class MainViewHorizontal : MainView
    {
        public MainViewHorizontal()
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
    }
}

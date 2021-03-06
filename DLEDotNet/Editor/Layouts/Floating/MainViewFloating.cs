﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor.Layouts.Floating
{
    [ToolboxItem(false)]
    public partial class MainViewFloating : MainView
    {
        public MainViewFloating()
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
            return null;
        }

        public override void SetFullScreen(bool fullScreen)
        {
            splitContainerTexture.Panel1Collapsed = fullScreen;
        }
    }
}

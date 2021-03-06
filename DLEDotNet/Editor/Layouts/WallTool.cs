﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Wall editor tool.")]
    public partial class WallTool : DLELayoutableUserControl
    {
        public WallTool()
        {
            InitializeComponent();
            this.HelpPageName = "walls";
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalWallTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingWallTool);
            }
            return null;
        }
    }
}

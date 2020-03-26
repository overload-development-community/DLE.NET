using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet
{
    public partial class TextureList : UserControl
    {
        private const int TEXTURE_SIZE = 32;
        private const int TEXTURE_PADDING = 6;
        private const int OUTER_PADDING = 4;
        private int _textureCount = 0;

        public TextureList()
        {
            InitializeComponent();
        }

        public int TextureCount
        {
            get
            {
                return _textureCount;
            }
            set
            {
                _textureCount = value;
                UpdateScrollBar();
            }
        }

        private void TextureList_SizeChanged(object sender, EventArgs e)
        {
            UpdateScrollBar();
        }

        private void UpdateScrollBar()
        {
            // how many textures can we fit now?
            int textureCountW = (texturePictureBox.Width - OUTER_PADDING * 2 + TEXTURE_PADDING) / (TEXTURE_SIZE + TEXTURE_PADDING);
            int textureCountH = (texturePictureBox.Height - OUTER_PADDING * 2 + TEXTURE_PADDING) / (TEXTURE_SIZE + TEXTURE_PADDING);
            int textureCountTotal = textureCountW * textureCountH;
            int extraTextures = TextureCount - textureCountTotal;
            int extraRows = (extraTextures + textureCountW - 1) / textureCountW;

            textureVScrollBar.Minimum = 0;
            textureVScrollBar.Maximum = Math.Max(0, extraRows);
            textureVScrollBar.Enabled = textureVScrollBar.Minimum != textureVScrollBar.Maximum;
            textureVScrollBar.SmallChange = 1;
            textureVScrollBar.LargeChange = textureCountH;
        }

        private void TextureList_Load(object sender, EventArgs e)
        {
            UpdateScrollBar();
        }
    }
}

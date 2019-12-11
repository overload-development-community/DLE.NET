using EditorUITest.Editor;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public partial class EditorWindow
    {
        private void textureEditToolStripMenuItem_Click(object sender, EventArgs e)
        {
            EditorTabs.ShowTab(this, editorTabs.Textures);
        }
    }
}

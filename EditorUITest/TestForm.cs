using EditorUITest.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public partial class TestForm : Form
    {
        public TestForm()
        {
            InitializeComponent();
        }

        private Placeholder.Level lv1 = new Placeholder.Level();
        private Placeholder.Level lv2 = new Placeholder.Level();
        public EditorWindow EditorWindow { get; internal set; }
        public EditorState EditorState { get; internal set; }

        private void button1_Click(object sender, EventArgs e)
        {
            EditorState.Level = lv1;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            EditorState.Level.SegmentCount++;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            EditorState.Level = lv2;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            EditorWindow.ActiveLayout = LayoutOrientation.HORIZONTAL;
        }

        private void button5_Click(object sender, EventArgs e)
        {
            EditorWindow.ActiveLayout = LayoutOrientation.VERTICAL;
        }

        private void button6_Click(object sender, EventArgs e)
        {
            EditorWindow.ActiveLayout = LayoutOrientation.FLOATING;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            EditorWindow.ActiveLayout = LayoutOrientation.VERTICAL_SS;
        }
    }
}

using EditorUITest.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Util
{
    public class DLEUserControl : UserControl
    {
        public EditorState EditorState
        {
            get
            {
                return (this.ParentForm as EditorWindow)?.EditorState;
            }
        }
    }
}

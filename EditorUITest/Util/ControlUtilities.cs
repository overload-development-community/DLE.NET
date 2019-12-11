using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest.Util
{
    public static class ControlUtilities
    {
        public static bool IsTypableControl(Control control)
        {
            return control is TextBox || control is ComboBox || control is ListBox;
        }
    }
}

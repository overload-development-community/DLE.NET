using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Editor.Layouts.Vertical;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Object editor tool.")]
    public partial class ObjectTool : DLELayoutableUserControl
    {
        public ObjectTool()
        {
            InitializeComponent();
            this.HelpPageName = "objects";
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                case LayoutOrientation.FLOATING:
                    return typeof(VerticalObjectTool);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindTabControlBacking(this.objectTabControl, PROP(s => s.ActiveObjectTab));
            binder.BindCheckBox(this.objSortCheckBox, PROP(s => s.SavedPrefs.SortObjects), false);
        }
    }
}

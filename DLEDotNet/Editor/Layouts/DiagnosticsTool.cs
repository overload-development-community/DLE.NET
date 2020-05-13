using DLEDotNet.Editor.Layouts.Vertical;
using System;
using System.ComponentModel;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Diagnostics editor tool.")]
    public partial class DiagnosticsTool : DLELayoutableUserControl
    {
        public DiagnosticsTool()
        {
            InitializeComponent();
            this.HelpPageName = "diagnostics";
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalDiagnosticsTool);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);
            binder.BindCheckBox(this.diagFixbugsCheckBox, PROP(s => s.SavedPrefs.AutoFixBugs), false);

            diagStatsListView.ListViewItemSorter = null;
        }
    }
}

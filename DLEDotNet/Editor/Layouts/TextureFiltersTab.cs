using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using DLEDotNet.Data;
using DLEDotNet.Editor.Layouts.Floating;
using DLEDotNet.Editor.Layouts.Vertical;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The Texture Filters editor tool.")]
    public partial class TextureFiltersTab : DLELayoutableUserControl
    {
        public TextureFiltersTab()
        {
            InitializeComponent();
            this.HelpPageName = "texturefilters";
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                    return typeof(VerticalTextureFiltersTool);
                case LayoutOrientation.FLOATING:
                    return typeof(FloatingTextureFiltersTool);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);

            binder.BindCheckBoxFlag<TextureFilters>(this.txtGrayRockCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.GrayRock, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtBrownRockCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.BrownRock, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtRedRockCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.RedRock, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtYellowRockCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.YellowRock, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtGreenRockCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.GreenRock, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtBlueRockCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.BlueRock, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtIceCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Ice, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtSandCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Sand, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtGrassCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Grass, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtStonesCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Stones, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtWaterCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Water, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtLavaCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Lava, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtSteelCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Steel, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtConcreteCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Concrete, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtTarmacCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Tarmac, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtBrickCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Brick, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtWallCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Wall, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtFloorCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Floor, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtCeilingCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Ceiling, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtGrateCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Grate, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtFanCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Fan, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtLightCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Light, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtEnergyCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Energy, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtForcefieldCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Forcefield, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtTechCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Tech, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtLabelCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Label, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtMonitorCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Monitor, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtStripesCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Stripes, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtDoorCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Door, false);
            binder.BindCheckBoxFlag<TextureFilters>(this.txtSwitchCheckBox, PROP(s => s.SavedPrefs.TextureFilters), TextureFilters.Switch, false);
        }

        private void txtRockAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters |= Data.TextureFilters.Rock;
        private void txtRockNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters &= ~Data.TextureFilters.Rock;

        private void txtNatureAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters |= Data.TextureFilters.Nature;
        private void txtNatureNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters &= ~Data.TextureFilters.Nature;

        private void txtBuildingAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters |= Data.TextureFilters.Building;
        private void txtBuildingNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters &= ~Data.TextureFilters.Building;

        private void txtOtherAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters |= Data.TextureFilters.OtherBuilding;
        private void txtOtherNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters &= ~Data.TextureFilters.OtherBuilding;

        private void txtTechAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters |= Data.TextureFilters.TechnicalMaterials;
        private void txtTechNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters &= ~Data.TextureFilters.TechnicalMaterials;

        private void txtSignAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters |= Data.TextureFilters.Signs;
        private void txtSignNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters &= ~Data.TextureFilters.Signs;

        private void txtAllButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters = Data.TextureFilters.All;
        private void txtNoneButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters = Data.TextureFilters.None;
        private void txtInvertButton_Click(object sender, EventArgs e) =>
            EditorState.SavedPrefs.TextureFilters ^= Data.TextureFilters.All;
    }
}

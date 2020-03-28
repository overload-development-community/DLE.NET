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
using DLEDotNet.Data;

namespace DLEDotNet.Editor.Layouts
{
    [ToolboxItem(true)]
    [Description("The visibility preferences under the Settings tool.")]
    public partial class PreferencesVisibility : DLELayoutableUserControl
    {
        public PreferencesVisibility()
        {
            InitializeComponent();
        }

        internal override void ResetLayout() => InitializeComponent();

        public override Type PickLayout(LayoutOrientation layout)
        {
            switch (layout)
            {
                case LayoutOrientation.VERTICAL:
                case LayoutOrientation.VERTICAL_SS:
                case LayoutOrientation.FLOATING:
                    return typeof(VerticalPreferencesVisibility);
            }
            return null;
        }

        protected override void SetupControls()
        {
            EditorStateBinder binder = EditorStateBinder.FromState(this.EditorState);

            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewWallsCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.Walls, false);
            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewSpecialCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.SpecialSegments, false);
            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewLightsCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.Lights, false);
            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewShadingCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.Shading, false);
            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewDeltalightsCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.DeltaShading, false);
            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewHideTaggedCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.HideMarked, false);
            binder.BindCheckBoxFlag<GeometryVisibilityFlags>(this.prefsViewSkyboxCheckBox, PROP(s => s.Prefs.GeometryVisibility), GeometryVisibilityFlags.SkyBox, false);
            binder.BindCheckBoxFlag<TextureVisibilityFlags>(this.prefsViewAlltexturesCheckBox, PROP(s => s.Prefs.TextureVisibility), TextureVisibilityFlags.UsedTextures, false);

            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewRobotsCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Robots, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewPlayersCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Players, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewWeaponsCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Weapons, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewPowerupsCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Powerups, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewKeysCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Keys, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewHostagesCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Hostages, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewReactorCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Reactor, false);
            binder.BindCheckBoxFlag<ObjectVisibilityFlags>(this.prefsViewEffectsCheckBox, PROP(s => s.Prefs.GeometryVisibility), ObjectVisibilityFlags.Effects, false);
        }

        private void prefsViewMineAllButton_Click(object sender, EventArgs e)
        {
            EditorState.Prefs.GeometryVisibility |= GeometryVisibilityFlags.Walls | GeometryVisibilityFlags.SpecialSegments | GeometryVisibilityFlags.Lights | GeometryVisibilityFlags.DeltaShading;
        }

        private void prefsViewMineNoneButton_Click(object sender, EventArgs e)
        {
            EditorState.Prefs.GeometryVisibility &= ~(GeometryVisibilityFlags.Walls | GeometryVisibilityFlags.SpecialSegments | GeometryVisibilityFlags.Lights | GeometryVisibilityFlags.DeltaShading);
        }

        private void prefsViewObjectsAllButton_Click(object sender, EventArgs e)
        {
            EditorState.Prefs.ObjectVisibility = ObjectVisibilityFlags.All;
        }

        private void prefsViewObjectsNoneButton_Click(object sender, EventArgs e)
        {
            EditorState.Prefs.ObjectVisibility = ObjectVisibilityFlags.None;
        }
    }
}

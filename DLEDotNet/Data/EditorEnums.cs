using System;
using System.Collections.Generic;
using System.Text;

namespace DLEDotNet.Data
{
    public enum SegmentAddMode
    {
        Normal, Extended, Mirrored
    }

    public enum SelectMode
    {
        Point, Line, Side, Segment, Object, Block
    }

    public enum RendererMode
    {
        ThirdPerson, FirstPerson
    }

    public enum DetailLevel
    {
        None, Low, Medium, High
    }

    public enum MineCenterDisplayShape
    {
        None, Crosshairs, Globe
    }

    public enum StartupWindowState
    {
        Windowed, Maximized
    }

    public enum CameraMovementMode
    {
        Stepped, Continuous
    }

    public enum QuickSelectionCandidateMode
    {
        Off, Circles, Full
    }

    public enum ElementMovementReferenceMode
    {
        RelativeToCube, RelativeToCamera
    }

    public enum LevelGameKind
    {
        D1, D2, D2Vertigo, D2XXL
    }

    [Flags]
    public enum GeometryVisibilityFlags : int
    {
        Default = Walls | SpecialSegments | Lights,

        Walls = 1, SpecialSegments = 2, Lights = 4, Shading = 8, DeltaShading = 16, HideMarked = 32, SkyBox = 64,
        None = 0, All = 127
    }

    [Flags]
    public enum ObjectVisibilityFlags : int
    {
        Default = All,

        Robots = 1, Players = 2, Weapons = 4, Powerups = 8, Keys = 16, Hostages = 32, Reactor = 64, Effects = 128,
        None = 0, All = 255
    }

    [Flags]
    public enum TextureVisibilityFlags : int
    {
        Default = None,

        UsedTextures = 32,
        None = 0
    }

    [Flags]
    public enum TextureFilters : int
    {
        GrayRock = 1,
        BrownRock = (1 << 1),
        RedRock = (1 << 2),
        GreenRock = (1 << 3),
        YellowRock = (1 << 4),
        BlueRock = (1 << 5),
        Ice = (1 << 6),
        Stones = (1 << 7),
        Grass = (1 << 8),
        Sand = (1 << 9),
        Lava = (1 << 10),
        Water = (1 << 11),
        Steel = (1 << 12),
        Concrete = (1 << 13),
        Brick = (1 << 14),
        Tarmac = (1 << 15),
        Wall = (1 << 16),
        Floor = (1 << 17),
        Ceiling = (1 << 18),
        Grate = (1 << 19),
        Fan = (1 << 20),
        Light = (1 << 21),
        Energy = (1 << 22),
        Forcefield = (1 << 23),
        Sign = (1 << 24),
        Switch = (1 << 25),
        Tech = (1 << 26),
        Door = (1 << 27),
        Label = (1 << 28),
        Monitor = (1 << 29),
        Stripes = (1 << 30),
        Moving = (1 << 31),

        Rock = GrayRock | BrownRock | RedRock | GreenRock | YellowRock | BlueRock,
        Nature = Ice | Stones | Grass | Sand | Lava | Water,
        Building = Steel | Concrete | Brick | Tarmac | Wall | Floor | Ceiling,
        OtherBuilding = Fan | Grate | Door,
        TechnicalMaterials = Switch | Tech | Energy | Forcefield | Light,
        Signs = Sign | Label | Monitor | Stripes,

        None = 0, All = ~0
    }
}

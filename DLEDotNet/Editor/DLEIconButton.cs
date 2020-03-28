using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Resources;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.VisualStyles;

namespace DLEDotNet.Editor
{
    [DesignerCategory("Code")]
    [Description("A button that renders as a custom image, depending on whether the button is held or not.")]
    public class DLEIconButton : RepeatingButton, ISupportInitialize
    {
        private bool initImages = false;
        private Image genImageControl = null;
        private Image genImageControlDark = null;
        private Image upImageControl = null;
        private Image downImageControl = null;
        private Image upImage = null;
        private Image downImage = null;
        private bool pressed = false;
        private bool pressedByMouse = false;
        private ToolTip myTooltip;
        private bool doNotFetchImmediately = false;
        private string toolTipText = "";

        private static readonly Color buttonMainBgColorBright = Color.FromArgb(192, 192, 192);
        private static readonly Color buttonShineBgColorBright = Color.FromArgb(255, 255, 255);
        private static readonly Color buttonShadeBgColorBright = Color.FromArgb(128, 128, 128);
        private static readonly Color buttonBorderUpColorBright = Color.FromArgb(0, 0, 0);
        private static readonly Color buttonBorderDownColorBright = Color.FromArgb(0, 0, 0);
        private static readonly Color buttonMainBgColorDark = Color.FromArgb(64, 64, 64);
        private static readonly Color buttonShineBgColorDark = Color.FromArgb(128, 128, 128);
        private static readonly Color buttonShadeBgColorDark = Color.FromArgb(0, 0, 0);
        private static readonly Color buttonBorderUpColorDark = Color.FromArgb(96, 96, 96);
        private static readonly Color buttonBorderDownColorDark = Color.FromArgb(96, 96, 96);

        private static Brush buttonMainBgBrush;
        private static Brush buttonShineBgBrush;
        private static Brush buttonShadeBgBrush;
        private static Pen buttonBorderUpPen;
        private static Pen buttonBorderDownPen;

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [DefaultValue(IconButtonMode.Automatic)]
        [Description("Controls whether one image is used for buttons (with held/not held rendered automatically; recommended option) or two separate images for held/not held.")]
        public IconButtonMode ImageMode { get; set; } = IconButtonMode.Automatic;

        static DLEIconButton()
        {
            InitColors();
        }

        public DLEIconButton() : base()
        {
            this.ClickOnlyOnUp = true;
            this.myTooltip = new ToolTip();
            base.Text = "";
            base.FlatStyle = this.FlatStyle;

            this.myTooltip.Active = true;
            this.myTooltip.ShowAlways = true;
            this.myTooltip.OwnerDraw = false;
        }

        [Browsable(false)]
        public new FlatStyle FlatStyle
        {
            get => FlatStyle.Flat;
            set { }
        }

        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [DefaultValue("")]
        [Description("The text to be displayed when the mouse hovers over this input button.")]
        public new string Text
        {
            get => this.toolTipText;
            set {
                this.toolTipText = value;
                myTooltip.SetToolTip(this, value);
                this.myTooltip.Active = value != "";
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The image to be displayed on the button. It should have a transparent background and no margin at all for the best appearance. Only used if IconButtonMode is Automatic.")]
        public Image ImageButton
        {
            get => this.genImageControl;
            set
            {
                this.genImageControl = value;
                if (!doNotFetchImmediately) GetImages();
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The image to be displayed on the button in dark mode. Only used if IconButtonMode is Automatic.")]
        public Image ImageButtonDark
        {
            get => this.genImageControlDark;
            set
            {
                this.genImageControlDark = value;
                if (!doNotFetchImmediately) GetImages();
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The image to be displayed when the button is not held down. Only used if IconButtonMode is Manual.")]
        public Image ImageNotHeld
        {
            get => this.upImageControl;
            set
            {
                this.upImageControl = value;
                if (!doNotFetchImmediately) GetImages();
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The image to be displayed when the button is being held down. Only used if IconButtonMode is Manual.")]
        public Image ImageHeld
        {
            get => this.downImageControl;
            set
            {
                this.downImageControl = value;
                if (!doNotFetchImmediately) GetImages();
            }
        }

        protected bool Pressed
        {
            get => pressed;
            set
            {
                bool oldValue = pressed;
                pressed = value;
                pressedByMouse &= value;
                if (oldValue != value) Invalidate();
            }
        }

        protected override void OnParentBackColorChanged(EventArgs e)
        {
            base.OnParentBackColorChanged(e);

            if (Visible)
                GetImages();
        }

        protected override void OnVisibleChanged(EventArgs e)
        {
            base.OnVisibleChanged(e);

            if (Visible && !Disposing)
                GetImages();
        }

        private static Bitmap ResizeImage(Image i, Size sz)
        {
            Bitmap bm = new Bitmap(sz.Width, sz.Height, PixelFormat.Format32bppPArgb);
            if (i == null) return bm;

            bm.SetResolution(i.HorizontalResolution, i.VerticalResolution);

            using (var g = Graphics.FromImage(bm))
            {
                g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
                g.PixelOffsetMode = System.Drawing.Drawing2D.PixelOffsetMode.HighQuality;
                g.DrawImage(i, new Rectangle(0, 0, sz.Width, sz.Height), new Rectangle(0, 0, i.Width, i.Height), GraphicsUnit.Pixel);
            }

            return bm;
        }

        private static void InitColors()
        {
            bool isDark = ControlUtil.IsDarkMode();
            buttonMainBgBrush = new SolidBrush(isDark ? buttonMainBgColorDark : buttonMainBgColorBright);
            buttonShineBgBrush = new SolidBrush(isDark ? buttonShineBgColorDark : buttonShineBgColorBright);
            buttonShadeBgBrush = new SolidBrush(isDark ? buttonShadeBgColorDark : buttonShadeBgColorBright);
            buttonBorderUpPen = new Pen(isDark ? buttonBorderUpColorDark : buttonBorderUpColorBright);
            buttonBorderDownPen = new Pen(isDark ? buttonBorderDownColorDark : buttonBorderDownColorBright);
        }

        private const float BUTTON_UNIT = 1 / 24f;

        private static void DrawUpFrame(Graphics g, Size sz)
        {
            float unit = BUTTON_UNIT * Math.Min(sz.Width, sz.Height);
            g.FillRectangle(buttonShineBgBrush, 0, 0, sz.Width - 1, sz.Height - 1);
            g.FillPolygon(buttonShadeBgBrush, new PointF[] { new PointF(sz.Width, 0), new PointF(0, sz.Height), new PointF(sz.Width - 1, sz.Height) });
            g.FillRectangle(buttonMainBgBrush, 3 * unit, 3 * unit, 18 * unit, 18 * unit);
            g.DrawRectangle(buttonBorderUpPen, 0, 0, sz.Width - 1, sz.Height - 1);
        }

        private static void DrawDownFrame(Graphics g, Size sz)
        {
            float unit = BUTTON_UNIT * Math.Min(sz.Width, sz.Height);
            g.FillRectangle(buttonShadeBgBrush, 0, 0, sz.Width - 1, sz.Height - 1);
            g.FillPolygon(buttonMainBgBrush, new PointF[] { new PointF(sz.Width, 0), new PointF(0, sz.Height), new PointF(sz.Width - 1, sz.Height) });
            g.FillRectangle(buttonMainBgBrush, 3 * unit, 3 * unit, 18 * unit, 18 * unit);
            g.DrawRectangle(buttonBorderDownPen, 0, 0, sz.Width - 1, sz.Height - 1);
        }

        private static void DrawDisabledMask(Graphics g, Size sz)
        {
            g.FillRectangle(new SolidBrush(ControlUtil.IsDarkMode() ? Color.FromArgb(192, 64, 64, 64) : Color.FromArgb(128, 128, 128, 128)), 0, 0, sz.Width, sz.Height);
        }

        public static Bitmap MakeUpImage(Image normal, Image dark, Size sz, bool grayMask)
        {
            if (buttonMainBgBrush == null) InitColors();
            Bitmap bm = new Bitmap(sz.Width, sz.Height, PixelFormat.Format32bppPArgb);

            using (var g = Graphics.FromImage(bm))
            {
                Bitmap tmp = ResizeImage(ControlUtil.IsDarkMode() ? dark : normal, new Size(sz.Width * 3 / 4, sz.Height * 3 / 4));
                DrawUpFrame(g, sz);
                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;
                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                if (tmp != null)
                {
                    g.DrawImage(tmp, sz.Width * (1 / 8f), sz.Height * (1 / 8f));
                    tmp.Dispose();
                }
                if (grayMask)
                    DrawDisabledMask(g, sz);
            }

            return bm;
        }

        public static Bitmap MakeDownImage(Image normal, Image dark, Size sz, bool grayMask)
        {
            if (buttonMainBgBrush == null) InitColors();
            Bitmap bm = new Bitmap(sz.Width, sz.Height, PixelFormat.Format32bppPArgb);

            using (var g = Graphics.FromImage(bm))
            {
                Bitmap tmp = ResizeImage(ControlUtil.IsDarkMode() ? dark : normal, new Size(sz.Width * 3 / 4, sz.Height * 3 / 4));
                DrawDownFrame(g, sz);
                g.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceOver;
                g.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
                if (tmp != null)
                {
                    g.DrawImage(tmp, sz.Width * (1 / 8f + 1 / 12f), sz.Height * (1 / 8f + 1 / 12f));
                    tmp.Dispose();
                }
                if (grayMask)
                    DrawDisabledMask(g, sz);
            }

            return bm;
        }

        private void GetImages()
        {
            if (ImageMode == IconButtonMode.Automatic)
            {
                upImage = MakeUpImage(this.genImageControl, this.genImageControlDark ?? InvertColors(this.genImageControl), this.Size, !this.Enabled);
                downImage = MakeDownImage(this.genImageControl, this.genImageControlDark ?? InvertColors(this.genImageControl), this.Size, !this.Enabled);
            }
            else if (ImageMode == IconButtonMode.Manual)
            {
                upImage = ResizeImage(this.upImageControl, this.Size);
                downImage = ResizeImage(this.downImageControl, this.Size);
            }
            initImages = upImage != null && downImage != null;
        }

        private static readonly float R = (float)Math.Sqrt(2);
        private Image InvertColors(Image img)
        {
            if (img == null) return null;

            Bitmap dst = new Bitmap(img.Width, img.Height);
            Graphics gr = Graphics.FromImage(dst);
            ImageAttributes attr = new ImageAttributes();

            // invert colors and apply 180 deg hue shift
            attr.SetColorMatrix(new ColorMatrix(new float[][]
            {
                new float[] {0, -R, -R, 0, 0},
                new float[] {-R, 0, -R, 0, 0},
                new float[] {-R, -R, 0, 0, 0},
                new float[] {0, 0, 0, 1, 0},
                new float[] {R, R, R, 0, 1}
            }));

            gr.DrawImage(img, new Rectangle(0, 0, img.Width, img.Height), 0, 0, img.Width, img.Height, GraphicsUnit.Pixel, attr);

            return dst;
        }

        protected override void OnEnabledChanged(EventArgs e)
        {
            base.OnEnabledChanged(e);
            GetImages();
            if (!this.Enabled)
                Pressed = false;
        }

        protected override void OnPaint(PaintEventArgs pevent)
        {
            if (!initImages) return;
            System.Drawing.Drawing2D.CompositingMode oldCompositingMode = pevent.Graphics.CompositingMode;
            pevent.Graphics.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
            pevent.Graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            pevent.Graphics.DrawImage(Pressed ? downImage : upImage, 0, 0);
            pevent.Graphics.CompositingMode = oldCompositingMode;
        }

        protected override void OnMouseDown(MouseEventArgs mevent)
        {
            if (this.Enabled)
                Pressed = pressedByMouse = true;
            base.OnMouseDown(mevent);
        }

        protected override void OnMouseUp(MouseEventArgs mevent)
        {
            Pressed = false;
            base.OnMouseUp(mevent);
        }

        protected override void OnMouseEnter(EventArgs e)
        {
            this.myTooltip.SetToolTip(this, toolTipText);
            this.myTooltip.Active = false;
            this.myTooltip.Active = true;
            base.OnMouseEnter(e);
        }

        protected override void OnMouseMove(MouseEventArgs mevent)
        {
            if (pressed && pressedByMouse && !this.ClientRectangle.Contains(mevent.Location))
            {
                Pressed = false;
            }
            base.OnMouseMove(mevent);
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            if (pressedByMouse)
                Pressed = false;
            base.OnMouseLeave(e);
        }

        protected override void OnKeyDown(KeyEventArgs kevent)
        {
            if (this.Enabled && (kevent.KeyCode.Equals(Keys.Space) || kevent.KeyCode.Equals(Keys.Enter)) && kevent.Modifiers == Keys.None)
            {
                Pressed = true;
            }
            base.OnKeyDown(kevent);
        }

        protected override void OnKeyUp(KeyEventArgs kevent)
        {
            if (kevent.KeyCode.Equals(Keys.Space) || kevent.KeyCode.Equals(Keys.Enter))
            {
                if (Control.MouseButtons == MouseButtons.None)
                {
                    Pressed = false;
                }
            }
            base.OnKeyUp(kevent);
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            if (!doNotFetchImmediately) GetImages();
            base.OnSizeChanged(e);
        }

        public void BeginInit()
        {
            doNotFetchImmediately = true;
        }

        public void EndInit()
        {
            doNotFetchImmediately = false;
            GetImages();
        }
    }

    public enum IconButtonMode
    {
        Automatic, Manual
    }
}

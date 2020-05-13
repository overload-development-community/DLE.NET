using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    class ToolStripCustomRenderer : ToolStripSystemRenderer
    {
        private Pen tickPen = new Pen(Color.Black, 1.875f);

        protected override void OnRenderItemCheck(ToolStripItemImageRenderEventArgs e)
        {
            if (!SystemInformation.HighContrast)
            {
                base.OnRenderItemCheck(e);
                return;
            }

            tickPen.Color = e.Item.ForeColor;
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            e.Graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;
            var r = new Rectangle(e.ImageRectangle.Location, e.ImageRectangle.Size);
            r.Inflate(-2, -3);
            int minC = Math.Min(r.Width, r.Height);
            int cx = r.X + r.Width / 2;
            int cy = r.Y + r.Height / 2;
            int ox = cx - minC / 2;
            int tx = ox + (7 * minC) / 16;
            int oy = cy + minC / 3;
            e.Graphics.DrawLines(tickPen, new PointF[]{
                new PointF(ox, oy - minC / 2),
                new PointF(cx, oy),
                new PointF(ox + minC, oy - minC) });
        }
    }
}

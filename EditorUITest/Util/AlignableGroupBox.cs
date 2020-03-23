﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    // A GroupBox that supports aligning the header to left, center or right
    [System.ComponentModel.DesignerCategory("Code")]
    public class AlignableGroupBox : GroupBox
    {
        private string _Text = "";
        private HorizontalAlignment _Align = HorizontalAlignment.Left;
        public AlignableGroupBox()
        {
            base.Text = "";
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue("GroupBoxText")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        public new string Text
        {
            get
            {
                return _Text;
            }
            set
            {
                _Text = value;
                this.Invalidate();
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue(HorizontalAlignment.Left)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The horizontal alignment of the group box header text.")]
        public HorizontalAlignment Alignment
        {
            get
            {
                return _Align;
            }
            set
            {
                _Align = value;
                this.Invalidate();
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            const int offset = 8;

            SolidBrush colorBrush = new SolidBrush(this.ForeColor);
            var backColor = new SolidBrush(this.BackColor);
            var size = TextRenderer.MeasureText(this.Text, this.Font);
            int left = 0;
            if (_Align == HorizontalAlignment.Left)
            {
                left = offset;
            }
            else if (_Align == HorizontalAlignment.Center)
            {
                left = (this.Width - size.Width) / 2;
            }
            else if (_Align == HorizontalAlignment.Right)
            {
                left = this.Width - size.Width - offset;
            }
            e.Graphics.FillRectangle(backColor, new Rectangle(left, 0, size.Width, size.Height));
            e.Graphics.DrawString(this.Text, this.Font, colorBrush, new PointF(left, 0));
        }
    }
}

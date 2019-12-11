namespace EditorUITest
{
    partial class TestForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.segmentTab1 = new EditorUITest.SegmentTab();
            this.hScrollBar1 = new System.Windows.Forms.HScrollBar();
            this.SuspendLayout();
            // 
            // segmentTab1
            // 
            this.segmentTab1.DialogLayout = EditorUITest.LayoutOrientation.HORIZONTAL;
            this.segmentTab1.Location = new System.Drawing.Point(15, 15);
            this.segmentTab1.Margin = new System.Windows.Forms.Padding(6, 6, 6, 8);
            this.segmentTab1.Name = "segmentTab1";
            this.segmentTab1.Size = new System.Drawing.Size(930, 211);
            this.segmentTab1.TabIndex = 0;
            // 
            // hScrollBar1
            // 
            this.hScrollBar1.Location = new System.Drawing.Point(357, 289);
            this.hScrollBar1.Name = "hScrollBar1";
            this.hScrollBar1.Size = new System.Drawing.Size(80, 17);
            this.hScrollBar1.TabIndex = 1;
            // 
            // TestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.hScrollBar1);
            this.Controls.Add(this.segmentTab1);
            this.Name = "TestForm";
            this.Text = "TestForm";
            this.Load += new System.EventHandler(this.TestForm_Load);
            this.Click += new System.EventHandler(this.TestForm_Click);
            this.ResumeLayout(false);

        }

        #endregion

        private SegmentTab segmentTab1;
        private System.Windows.Forms.HScrollBar hScrollBar1;
    }
}
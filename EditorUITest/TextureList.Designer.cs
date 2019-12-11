namespace EditorUITest
{
    partial class TextureList
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.textureVScrollBar = new System.Windows.Forms.VScrollBar();
            this.texturePictureBox = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.texturePictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // textureVScrollBar
            // 
            this.textureVScrollBar.Cursor = System.Windows.Forms.Cursors.Default;
            this.textureVScrollBar.Dock = System.Windows.Forms.DockStyle.Right;
            this.textureVScrollBar.Location = new System.Drawing.Point(144, 0);
            this.textureVScrollBar.Name = "textureVScrollBar";
            this.textureVScrollBar.Size = new System.Drawing.Size(17, 400);
            this.textureVScrollBar.TabIndex = 0;
            // 
            // texturePictureBox
            // 
            this.texturePictureBox.BackColor = System.Drawing.Color.Black;
            this.texturePictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.texturePictureBox.Location = new System.Drawing.Point(0, 0);
            this.texturePictureBox.Name = "texturePictureBox";
            this.texturePictureBox.Size = new System.Drawing.Size(144, 400);
            this.texturePictureBox.TabIndex = 1;
            this.texturePictureBox.TabStop = false;
            // 
            // TextureList
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Black;
            this.Controls.Add(this.texturePictureBox);
            this.Controls.Add(this.textureVScrollBar);
            this.MinimumSize = new System.Drawing.Size(50, 50);
            this.Name = "TextureList";
            this.Size = new System.Drawing.Size(161, 400);
            this.Load += new System.EventHandler(this.TextureList_Load);
            this.SizeChanged += new System.EventHandler(this.TextureList_SizeChanged);
            ((System.ComponentModel.ISupportInitialize)(this.texturePictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.VScrollBar textureVScrollBar;
        private System.Windows.Forms.PictureBox texturePictureBox;
    }
}

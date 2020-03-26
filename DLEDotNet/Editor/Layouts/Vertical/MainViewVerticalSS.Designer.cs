namespace DLEDotNet.Editor.Layouts.Vertical
{
    partial class MainViewVerticalSS
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
            this.splitEditorTexture = new System.Windows.Forms.SplitContainer();
            this.editorTabs = new DLEDotNet.Editor.Layouts.EditorTabContainer();
            this.textureList = new DLEDotNet.TextureList();
            this.mineView = new DLEDotNet.MineView();
            this.splitContainerMine = new System.Windows.Forms.SplitContainer();
            ((System.ComponentModel.ISupportInitialize)(this.splitEditorTexture)).BeginInit();
            this.splitEditorTexture.Panel1.SuspendLayout();
            this.splitEditorTexture.Panel2.SuspendLayout();
            this.splitEditorTexture.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerMine)).BeginInit();
            this.splitContainerMine.Panel1.SuspendLayout();
            this.splitContainerMine.Panel2.SuspendLayout();
            this.splitContainerMine.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitEditorTexture
            // 
            this.splitEditorTexture.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitEditorTexture.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitEditorTexture.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitEditorTexture.Location = new System.Drawing.Point(0, 0);
            this.splitEditorTexture.Name = "splitEditorTexture";
            this.splitEditorTexture.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitEditorTexture.Panel1
            // 
            this.splitEditorTexture.Panel1.Controls.Add(this.editorTabs);
            // 
            // splitEditorTexture.Panel2
            // 
            this.splitEditorTexture.Panel2.Controls.Add(this.textureList);
            this.splitEditorTexture.Size = new System.Drawing.Size(364, 600);
            this.splitEditorTexture.SplitterDistance = 394;
            this.splitEditorTexture.TabIndex = 4;
            // 
            // editorTabs
            // 
            this.editorTabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.editorTabs.Location = new System.Drawing.Point(0, 0);
            this.editorTabs.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.editorTabs.Name = "editorTabs";
            this.editorTabs.Size = new System.Drawing.Size(360, 390);
            this.editorTabs.TabIndex = 0;
            // 
            // textureList
            // 
            this.textureList.BackColor = System.Drawing.Color.Black;
            this.textureList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textureList.Location = new System.Drawing.Point(0, 0);
            this.textureList.MinimumSize = new System.Drawing.Size(50, 50);
            this.textureList.Name = "textureList";
            this.textureList.Size = new System.Drawing.Size(360, 198);
            this.textureList.TabIndex = 0;
            this.textureList.TextureCount = 0;
            // 
            // mineView
            // 
            this.mineView.BackColor = System.Drawing.Color.Black;
            this.mineView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mineView.Location = new System.Drawing.Point(0, 0);
            this.mineView.Name = "mineView";
            this.mineView.Size = new System.Drawing.Size(828, 596);
            this.mineView.TabIndex = 0;
            // 
            // splitContainerMine
            // 
            this.splitContainerMine.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainerMine.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerMine.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainerMine.Location = new System.Drawing.Point(0, 0);
            this.splitContainerMine.Name = "splitContainerMine";
            // 
            // splitContainerMine.Panel1
            // 
            this.splitContainerMine.Panel1.Controls.Add(this.splitEditorTexture);
            // 
            // splitContainerMine.Panel2
            // 
            this.splitContainerMine.Panel2.Controls.Add(this.mineView);
            this.splitContainerMine.Size = new System.Drawing.Size(1200, 600);
            this.splitContainerMine.SplitterDistance = 384;
            this.splitContainerMine.TabIndex = 3;
            // 
            // MainViewVerticalSS
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.splitContainerMine);
            this.Name = "MainViewVerticalSS";
            this.Size = new System.Drawing.Size(1200, 600);
            this.splitEditorTexture.Panel1.ResumeLayout(false);
            this.splitEditorTexture.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitEditorTexture)).EndInit();
            this.splitEditorTexture.ResumeLayout(false);
            this.splitContainerMine.Panel1.ResumeLayout(false);
            this.splitContainerMine.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerMine)).EndInit();
            this.splitContainerMine.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitEditorTexture;
        private EditorTabContainer editorTabs;
        private TextureList textureList;
        private MineView mineView;
        private System.Windows.Forms.SplitContainer splitContainerMine;
    }
}

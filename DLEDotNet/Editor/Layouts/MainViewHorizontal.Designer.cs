namespace DLEDotNet.Editor.Layouts
{
    partial class MainViewHorizontal
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
            this.splitContainerTexture = new System.Windows.Forms.SplitContainer();
            this.textureList = new DLEDotNet.TextureList();
            this.splitContainerEditor = new System.Windows.Forms.SplitContainer();
            this.mineView = new DLEDotNet.MineView();
            this.editorTabs = new DLEDotNet.Editor.Layouts.EditorTabContainer();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerTexture)).BeginInit();
            this.splitContainerTexture.Panel1.SuspendLayout();
            this.splitContainerTexture.Panel2.SuspendLayout();
            this.splitContainerTexture.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerEditor)).BeginInit();
            this.splitContainerEditor.Panel1.SuspendLayout();
            this.splitContainerEditor.Panel2.SuspendLayout();
            this.splitContainerEditor.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainerTexture
            // 
            this.splitContainerTexture.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainerTexture.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerTexture.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainerTexture.Location = new System.Drawing.Point(0, 0);
            this.splitContainerTexture.Name = "splitContainerTexture";
            // 
            // splitContainerTexture.Panel1
            // 
            this.splitContainerTexture.Panel1.Controls.Add(this.textureList);
            // 
            // splitContainerTexture.Panel2
            // 
            this.splitContainerTexture.Panel2.Controls.Add(this.splitContainerEditor);
            this.splitContainerTexture.Size = new System.Drawing.Size(1200, 600);
            this.splitContainerTexture.SplitterDistance = 175;
            this.splitContainerTexture.TabIndex = 1;
            // 
            // textureList
            // 
            this.textureList.BackColor = System.Drawing.Color.Black;
            this.textureList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textureList.Location = new System.Drawing.Point(0, 0);
            this.textureList.MinimumSize = new System.Drawing.Size(50, 50);
            this.textureList.Name = "textureList";
            this.textureList.Size = new System.Drawing.Size(171, 596);
            this.textureList.TabIndex = 0;
            this.textureList.TextureCount = 0;
            // 
            // splitContainerEditor
            // 
            this.splitContainerEditor.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainerEditor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainerEditor.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainerEditor.Location = new System.Drawing.Point(0, 0);
            this.splitContainerEditor.Name = "splitContainerEditor";
            this.splitContainerEditor.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainerEditor.Panel1
            // 
            this.splitContainerEditor.Panel1.Controls.Add(this.mineView);
            // 
            // splitContainerEditor.Panel2
            // 
            this.splitContainerEditor.Panel2.Controls.Add(this.editorTabs);
            this.splitContainerEditor.Size = new System.Drawing.Size(1021, 600);
            this.splitContainerEditor.SplitterDistance = 340;
            this.splitContainerEditor.TabIndex = 0;
            // 
            // mineView
            // 
            this.mineView.BackColor = System.Drawing.Color.Black;
            this.mineView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mineView.Location = new System.Drawing.Point(0, 0);
            this.mineView.Name = "mineView";
            this.mineView.Size = new System.Drawing.Size(1017, 330);
            this.mineView.TabIndex = 0;
            // 
            // editorTabs
            // 
            this.editorTabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.editorTabs.Location = new System.Drawing.Point(0, 0);
            this.editorTabs.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.editorTabs.Name = "editorTabs";
            this.editorTabs.Size = new System.Drawing.Size(1017, 258);
            this.editorTabs.TabIndex = 0;
            // 
            // MainViewHorizontal
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.splitContainerTexture);
            this.Name = "MainViewHorizontal";
            this.Size = new System.Drawing.Size(1200, 600);
            this.splitContainerTexture.Panel1.ResumeLayout(false);
            this.splitContainerTexture.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerTexture)).EndInit();
            this.splitContainerTexture.ResumeLayout(false);
            this.splitContainerEditor.Panel1.ResumeLayout(false);
            this.splitContainerEditor.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerEditor)).EndInit();
            this.splitContainerEditor.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainerTexture;
        private TextureList textureList;
        private System.Windows.Forms.SplitContainer splitContainerEditor;
        private MineView mineView;
        private EditorTabContainer editorTabs;
    }
}

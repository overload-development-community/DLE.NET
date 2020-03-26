namespace DLEDotNet.Editor.Layouts.Vertical
{
    partial class MainViewVertical
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
            this.splitContainerEditor = new System.Windows.Forms.SplitContainer();
            this.mineView = new DLEDotNet.MineView();
            this.textureList = new DLEDotNet.TextureList();
            this.splitContainerTexture = new System.Windows.Forms.SplitContainer();
            this.editorTabs = new DLEDotNet.Editor.Layouts.EditorTabContainer();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerEditor)).BeginInit();
            this.splitContainerEditor.Panel1.SuspendLayout();
            this.splitContainerEditor.Panel2.SuspendLayout();
            this.splitContainerEditor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerTexture)).BeginInit();
            this.splitContainerTexture.Panel1.SuspendLayout();
            this.splitContainerTexture.Panel2.SuspendLayout();
            this.splitContainerTexture.SuspendLayout();
            this.SuspendLayout();
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
            this.splitContainerEditor.Panel2.Controls.Add(this.textureList);
            this.splitContainerEditor.Size = new System.Drawing.Size(832, 600);
            this.splitContainerEditor.SplitterDistance = 439;
            this.splitContainerEditor.TabIndex = 2;
            // 
            // mineView
            // 
            this.mineView.BackColor = System.Drawing.Color.Black;
            this.mineView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mineView.Location = new System.Drawing.Point(0, 0);
            this.mineView.Name = "mineView";
            this.mineView.Size = new System.Drawing.Size(828, 435);
            this.mineView.TabIndex = 0;
            // 
            // textureList
            // 
            this.textureList.BackColor = System.Drawing.Color.Black;
            this.textureList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textureList.Location = new System.Drawing.Point(0, 0);
            this.textureList.MinimumSize = new System.Drawing.Size(50, 50);
            this.textureList.Name = "textureList";
            this.textureList.Size = new System.Drawing.Size(828, 153);
            this.textureList.TabIndex = 0;
            this.textureList.TextureCount = 0;
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
            this.splitContainerTexture.Panel1.Controls.Add(this.editorTabs);
            // 
            // splitContainerTexture.Panel2
            // 
            this.splitContainerTexture.Panel2.Controls.Add(this.splitContainerEditor);
            this.splitContainerTexture.Size = new System.Drawing.Size(1200, 600);
            this.splitContainerTexture.SplitterDistance = 418;
            this.splitContainerTexture.TabIndex = 0;
            // 
            // editorTabs
            // 
            this.editorTabs.BackColor = System.Drawing.SystemColors.Control;
            this.editorTabs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.editorTabs.Location = new System.Drawing.Point(0, 0);
            this.editorTabs.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.editorTabs.Name = "editorTabs";
            this.editorTabs.Size = new System.Drawing.Size(360, 596);
            this.editorTabs.TabIndex = 0;
            // 
            // MainViewVertical
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add(this.splitContainerTexture);
            this.Name = "MainViewVertical";
            this.Size = new System.Drawing.Size(1200, 600);
            this.splitContainerEditor.Panel1.ResumeLayout(false);
            this.splitContainerEditor.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerEditor)).EndInit();
            this.splitContainerEditor.ResumeLayout(false);
            this.splitContainerTexture.Panel1.ResumeLayout(false);
            this.splitContainerTexture.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainerTexture)).EndInit();
            this.splitContainerTexture.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainerEditor;
        private System.Windows.Forms.SplitContainer splitContainerTexture;
        private MineView mineView;
        private TextureList textureList;
        private EditorTabContainer editorTabs;
    }
}

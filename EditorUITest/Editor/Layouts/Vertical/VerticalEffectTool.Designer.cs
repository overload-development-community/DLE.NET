namespace EditorUITest.Editor.Layouts.Vertical
{
    partial class VerticalEffectTool
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
            this.effectObjectsComboBox = new System.Windows.Forms.ComboBox();
            this.effectCopyButton = new System.Windows.Forms.Button();
            this.effectPasteButton = new System.Windows.Forms.Button();
            this.effectPasteAllButton = new System.Windows.Forms.Button();
            this.effectDeleteButton = new System.Windows.Forms.Button();
            this.alignableGroupBox1 = new EditorUITest.AlignableGroupBox();
            this.effectAddButton = new System.Windows.Forms.Button();
            this.effectTabControl = new System.Windows.Forms.TabControl();
            this.tabPageParticles = new System.Windows.Forms.TabPage();
            this.effectParticle1 = new EditorUITest.Editor.Layouts.EffectParticle();
            this.tabPageLightning = new System.Windows.Forms.TabPage();
            this.effectLightning1 = new EditorUITest.Editor.Layouts.EffectLightning();
            this.tabPageSounds = new System.Windows.Forms.TabPage();
            this.effectSound1 = new EditorUITest.Editor.Layouts.EffectSound();
            this.tabPageWaypoints = new System.Windows.Forms.TabPage();
            this.effectWaypoint1 = new EditorUITest.Editor.Layouts.EffectWaypoint();
            this.tabPageFog = new System.Windows.Forms.TabPage();
            this.effectFog1 = new EditorUITest.Editor.Layouts.EffectFog();
            this.alignableGroupBox1.SuspendLayout();
            this.effectTabControl.SuspendLayout();
            this.tabPageParticles.SuspendLayout();
            this.tabPageLightning.SuspendLayout();
            this.tabPageSounds.SuspendLayout();
            this.tabPageWaypoints.SuspendLayout();
            this.tabPageFog.SuspendLayout();
            this.SuspendLayout();
            // 
            // effectObjectsComboBox
            // 
            this.effectObjectsComboBox.Location = new System.Drawing.Point(9, 20);
            this.effectObjectsComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.effectObjectsComboBox.Name = "effectObjectsComboBox";
            this.effectObjectsComboBox.Size = new System.Drawing.Size(204, 21);
            this.effectObjectsComboBox.TabIndex = 0;
            // 
            // effectCopyButton
            // 
            this.effectCopyButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectCopyButton.Location = new System.Drawing.Point(225, 62);
            this.effectCopyButton.Margin = new System.Windows.Forms.Padding(0);
            this.effectCopyButton.Name = "effectCopyButton";
            this.effectCopyButton.Size = new System.Drawing.Size(78, 21);
            this.effectCopyButton.TabIndex = 1;
            this.effectCopyButton.Text = "&copy";
            // 
            // effectPasteButton
            // 
            this.effectPasteButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectPasteButton.Location = new System.Drawing.Point(225, 83);
            this.effectPasteButton.Margin = new System.Windows.Forms.Padding(0);
            this.effectPasteButton.Name = "effectPasteButton";
            this.effectPasteButton.Size = new System.Drawing.Size(78, 21);
            this.effectPasteButton.TabIndex = 2;
            this.effectPasteButton.Text = "&paste";
            // 
            // effectPasteAllButton
            // 
            this.effectPasteAllButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectPasteAllButton.Location = new System.Drawing.Point(225, 104);
            this.effectPasteAllButton.Margin = new System.Windows.Forms.Padding(0);
            this.effectPasteAllButton.Name = "effectPasteAllButton";
            this.effectPasteAllButton.Size = new System.Drawing.Size(78, 21);
            this.effectPasteAllButton.TabIndex = 3;
            this.effectPasteAllButton.Text = "paste &all";
            // 
            // effectDeleteButton
            // 
            this.effectDeleteButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectDeleteButton.Location = new System.Drawing.Point(225, 37);
            this.effectDeleteButton.Margin = new System.Windows.Forms.Padding(0);
            this.effectDeleteButton.Name = "effectDeleteButton";
            this.effectDeleteButton.Size = new System.Drawing.Size(78, 21);
            this.effectDeleteButton.TabIndex = 4;
            this.effectDeleteButton.Text = "&delete";
            // 
            // alignableGroupBox1
            // 
            this.alignableGroupBox1.Alignment = System.Windows.Forms.HorizontalAlignment.Right;
            this.alignableGroupBox1.Controls.Add(this.effectObjectsComboBox);
            this.alignableGroupBox1.Controls.Add(this.effectCopyButton);
            this.alignableGroupBox1.Controls.Add(this.effectPasteButton);
            this.alignableGroupBox1.Controls.Add(this.effectPasteAllButton);
            this.alignableGroupBox1.Controls.Add(this.effectDeleteButton);
            this.alignableGroupBox1.Controls.Add(this.effectAddButton);
            this.alignableGroupBox1.Location = new System.Drawing.Point(8, 492);
            this.alignableGroupBox1.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBox1.Name = "alignableGroupBox1";
            this.alignableGroupBox1.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBox1.Size = new System.Drawing.Size(320, 133);
            this.alignableGroupBox1.TabIndex = 0;
            this.alignableGroupBox1.TabStop = false;
            this.alignableGroupBox1.Text = "";
            // 
            // effectAddButton
            // 
            this.effectAddButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectAddButton.Location = new System.Drawing.Point(225, 18);
            this.effectAddButton.Margin = new System.Windows.Forms.Padding(0);
            this.effectAddButton.Name = "effectAddButton";
            this.effectAddButton.Size = new System.Drawing.Size(78, 21);
            this.effectAddButton.TabIndex = 5;
            this.effectAddButton.Text = "&add";
            // 
            // effectTabControl
            // 
            this.effectTabControl.Controls.Add(this.tabPageParticles);
            this.effectTabControl.Controls.Add(this.tabPageLightning);
            this.effectTabControl.Controls.Add(this.tabPageSounds);
            this.effectTabControl.Controls.Add(this.tabPageWaypoints);
            this.effectTabControl.Controls.Add(this.tabPageFog);
            this.effectTabControl.Location = new System.Drawing.Point(8, 10);
            this.effectTabControl.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.effectTabControl.Name = "effectTabControl";
            this.effectTabControl.SelectedIndex = 0;
            this.effectTabControl.Size = new System.Drawing.Size(321, 473);
            this.effectTabControl.TabIndex = 2;
            // 
            // tabPageParticles
            // 
            this.tabPageParticles.Controls.Add(this.effectParticle1);
            this.tabPageParticles.Location = new System.Drawing.Point(4, 22);
            this.tabPageParticles.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageParticles.Name = "tabPageParticles";
            this.tabPageParticles.Padding = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageParticles.Size = new System.Drawing.Size(313, 447);
            this.tabPageParticles.TabIndex = 0;
            this.tabPageParticles.Text = "Particles";
            this.tabPageParticles.UseVisualStyleBackColor = true;
            // 
            // effectParticle1
            // 
            this.effectParticle1.DialogLayout = EditorUITest.LayoutOrientation.VERTICAL;
            this.effectParticle1.Location = new System.Drawing.Point(0, 0);
            this.effectParticle1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.effectParticle1.Name = "effectParticle1";
            this.effectParticle1.Size = new System.Drawing.Size(300, 422);
            this.effectParticle1.TabIndex = 0;
            // 
            // tabPageLightning
            // 
            this.tabPageLightning.Controls.Add(this.effectLightning1);
            this.tabPageLightning.Location = new System.Drawing.Point(4, 22);
            this.tabPageLightning.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageLightning.Name = "tabPageLightning";
            this.tabPageLightning.Padding = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageLightning.Size = new System.Drawing.Size(313, 447);
            this.tabPageLightning.TabIndex = 1;
            this.tabPageLightning.Text = "Lightning";
            this.tabPageLightning.UseVisualStyleBackColor = true;
            // 
            // effectLightning1
            // 
            this.effectLightning1.DialogLayout = EditorUITest.LayoutOrientation.VERTICAL;
            this.effectLightning1.Location = new System.Drawing.Point(0, 0);
            this.effectLightning1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.effectLightning1.Name = "effectLightning1";
            this.effectLightning1.Size = new System.Drawing.Size(300, 422);
            this.effectLightning1.TabIndex = 0;
            // 
            // tabPageSounds
            // 
            this.tabPageSounds.Controls.Add(this.effectSound1);
            this.tabPageSounds.Location = new System.Drawing.Point(4, 22);
            this.tabPageSounds.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageSounds.Name = "tabPageSounds";
            this.tabPageSounds.Size = new System.Drawing.Size(313, 447);
            this.tabPageSounds.TabIndex = 2;
            this.tabPageSounds.Text = "Sounds";
            this.tabPageSounds.UseVisualStyleBackColor = true;
            // 
            // effectSound1
            // 
            this.effectSound1.DialogLayout = EditorUITest.LayoutOrientation.VERTICAL;
            this.effectSound1.Location = new System.Drawing.Point(0, 0);
            this.effectSound1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.effectSound1.Name = "effectSound1";
            this.effectSound1.Size = new System.Drawing.Size(300, 422);
            this.effectSound1.TabIndex = 0;
            // 
            // tabPageWaypoints
            // 
            this.tabPageWaypoints.Controls.Add(this.effectWaypoint1);
            this.tabPageWaypoints.Location = new System.Drawing.Point(4, 22);
            this.tabPageWaypoints.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageWaypoints.Name = "tabPageWaypoints";
            this.tabPageWaypoints.Size = new System.Drawing.Size(313, 447);
            this.tabPageWaypoints.TabIndex = 3;
            this.tabPageWaypoints.Text = "Way points";
            this.tabPageWaypoints.UseVisualStyleBackColor = true;
            // 
            // effectWaypoint1
            // 
            this.effectWaypoint1.DialogLayout = EditorUITest.LayoutOrientation.VERTICAL;
            this.effectWaypoint1.Location = new System.Drawing.Point(0, 0);
            this.effectWaypoint1.Margin = new System.Windows.Forms.Padding(6, 8, 6, 8);
            this.effectWaypoint1.Name = "effectWaypoint1";
            this.effectWaypoint1.Size = new System.Drawing.Size(300, 422);
            this.effectWaypoint1.TabIndex = 0;
            // 
            // tabPageFog
            // 
            this.tabPageFog.Controls.Add(this.effectFog1);
            this.tabPageFog.Location = new System.Drawing.Point(4, 22);
            this.tabPageFog.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageFog.Name = "tabPageFog";
            this.tabPageFog.Size = new System.Drawing.Size(313, 447);
            this.tabPageFog.TabIndex = 4;
            this.tabPageFog.Text = "Fog";
            this.tabPageFog.UseVisualStyleBackColor = true;
            // 
            // effectFog1
            // 
            this.effectFog1.DialogLayout = EditorUITest.LayoutOrientation.VERTICAL;
            this.effectFog1.Location = new System.Drawing.Point(0, 0);
            this.effectFog1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.effectFog1.Name = "effectFog1";
            this.effectFog1.Size = new System.Drawing.Size(300, 422);
            this.effectFog1.TabIndex = 0;
            // 
            // VerticalEffectTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.effectTabControl);
            this.Controls.Add(this.alignableGroupBox1);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "VerticalEffectTool";
            this.Size = new System.Drawing.Size(333, 632);
            this.alignableGroupBox1.ResumeLayout(false);
            this.effectTabControl.ResumeLayout(false);
            this.tabPageParticles.ResumeLayout(false);
            this.tabPageLightning.ResumeLayout(false);
            this.tabPageSounds.ResumeLayout(false);
            this.tabPageWaypoints.ResumeLayout(false);
            this.tabPageFog.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox effectObjectsComboBox;
        private System.Windows.Forms.Button effectCopyButton;
        private System.Windows.Forms.Button effectPasteButton;
        private System.Windows.Forms.Button effectPasteAllButton;
        private System.Windows.Forms.Button effectDeleteButton;
        private EditorUITest.AlignableGroupBox alignableGroupBox1;
        private System.Windows.Forms.Button effectAddButton;
        private System.Windows.Forms.TabControl effectTabControl;
        private System.Windows.Forms.TabPage tabPageParticles;
        private EffectParticle effectParticle1;
        private System.Windows.Forms.TabPage tabPageLightning;
        private EffectLightning effectLightning1;
        private System.Windows.Forms.TabPage tabPageSounds;
        private EffectSound effectSound1;
        private System.Windows.Forms.TabPage tabPageWaypoints;
        private EffectWaypoint effectWaypoint1;
        private System.Windows.Forms.TabPage tabPageFog;
        private EffectFog effectFog1;
    }
}

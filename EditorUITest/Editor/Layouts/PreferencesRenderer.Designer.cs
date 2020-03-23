using System;
using System.Globalization;
using System.Windows.Forms;

namespace EditorUITest.Editor.Layouts
{
    partial class PreferencesRenderer
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
            this.prefsDepthOffRadioButton = new System.Windows.Forms.RadioButton();
            this.prefsDepthLowRadioButton = new System.Windows.Forms.RadioButton();
            this.prefsDepthMediumRadioButton = new System.Windows.Forms.RadioButton();
            this.prefsDepthHighRadioButton = new System.Windows.Forms.RadioButton();
            this.prefsViewdistTrackBar = new System.Windows.Forms.TrackBar();
            this.prefsMinecenterComboBox = new System.Windows.Forms.ComboBox();
            this.alignableGroupBoxDepthPerception = new EditorUITest.AlignableGroupBox();
            this.labelMin = new System.Windows.Forms.Label();
            this.labelMax = new System.Windows.Forms.Label();
            this.alignableGroupBoxMineRenderDepth = new EditorUITest.AlignableGroupBox();
            this.prefsViewdistTextLabel = new System.Windows.Forms.Label();
            this.alignableGroupBoxMineCenterDisplay = new EditorUITest.AlignableGroupBox();
            this.alignableGroupBoxRenderer = new EditorUITest.AlignableGroupBox();
            this.prefsRenderer3RdPersonRadioButton = new System.Windows.Forms.RadioButton();
            this.prefsRenderer1StPersonRadioButton = new System.Windows.Forms.RadioButton();
            ((System.ComponentModel.ISupportInitialize)(this.prefsViewdistTrackBar)).BeginInit();
            this.alignableGroupBoxDepthPerception.SuspendLayout();
            this.alignableGroupBoxMineRenderDepth.SuspendLayout();
            this.alignableGroupBoxMineCenterDisplay.SuspendLayout();
            this.alignableGroupBoxRenderer.SuspendLayout();
            this.SuspendLayout();
            // 
            // prefsDepthOffRadioButton
            // 
            this.prefsDepthOffRadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsDepthOffRadioButton.Location = new System.Drawing.Point(39, 20);
            this.prefsDepthOffRadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsDepthOffRadioButton.Name = "prefsDepthOffRadioButton";
            this.prefsDepthOffRadioButton.Size = new System.Drawing.Size(50, 15);
            this.prefsDepthOffRadioButton.TabIndex = 0;
            this.prefsDepthOffRadioButton.TabStop = true;
            this.prefsDepthOffRadioButton.Text = "off";
            // 
            // prefsDepthLowRadioButton
            // 
            this.prefsDepthLowRadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsDepthLowRadioButton.Location = new System.Drawing.Point(39, 36);
            this.prefsDepthLowRadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsDepthLowRadioButton.Name = "prefsDepthLowRadioButton";
            this.prefsDepthLowRadioButton.Size = new System.Drawing.Size(50, 15);
            this.prefsDepthLowRadioButton.TabIndex = 1;
            this.prefsDepthLowRadioButton.Text = "low";
            // 
            // prefsDepthMediumRadioButton
            // 
            this.prefsDepthMediumRadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsDepthMediumRadioButton.Location = new System.Drawing.Point(39, 52);
            this.prefsDepthMediumRadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsDepthMediumRadioButton.Name = "prefsDepthMediumRadioButton";
            this.prefsDepthMediumRadioButton.Size = new System.Drawing.Size(62, 15);
            this.prefsDepthMediumRadioButton.TabIndex = 2;
            this.prefsDepthMediumRadioButton.Text = "medium";
            // 
            // prefsDepthHighRadioButton
            // 
            this.prefsDepthHighRadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsDepthHighRadioButton.Location = new System.Drawing.Point(39, 68);
            this.prefsDepthHighRadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsDepthHighRadioButton.Name = "prefsDepthHighRadioButton";
            this.prefsDepthHighRadioButton.Size = new System.Drawing.Size(50, 15);
            this.prefsDepthHighRadioButton.TabIndex = 3;
            this.prefsDepthHighRadioButton.Text = "high";
            // 
            // prefsViewdistTrackBar
            // 
            this.prefsViewdistTrackBar.AutoSize = false;
            this.prefsViewdistTrackBar.Location = new System.Drawing.Point(8, 13);
            this.prefsViewdistTrackBar.Margin = new System.Windows.Forms.Padding(0);
            this.prefsViewdistTrackBar.Maximum = 60;
            this.prefsViewdistTrackBar.Name = "prefsViewdistTrackBar";
            this.prefsViewdistTrackBar.Size = new System.Drawing.Size(417, 24);
            this.prefsViewdistTrackBar.TabIndex = 0;
            this.prefsViewdistTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.prefsViewdistTrackBar.ValueChanged += new System.EventHandler(this.prefsViewdistTrackBar_ValueChanged);
            // 
            // prefsMinecenterComboBox
            // 
            this.prefsMinecenterComboBox.Items.AddRange(new object[] {
            "None",
            "Crosshairs",
            "Globe"});
            this.prefsMinecenterComboBox.Location = new System.Drawing.Point(20, 39);
            this.prefsMinecenterComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsMinecenterComboBox.Name = "prefsMinecenterComboBox";
            this.prefsMinecenterComboBox.Size = new System.Drawing.Size(103, 21);
            this.prefsMinecenterComboBox.TabIndex = 0;
            // 
            // alignableGroupBoxDepthPerception
            // 
            this.alignableGroupBoxDepthPerception.Controls.Add(this.prefsDepthOffRadioButton);
            this.alignableGroupBoxDepthPerception.Controls.Add(this.prefsDepthLowRadioButton);
            this.alignableGroupBoxDepthPerception.Controls.Add(this.prefsDepthMediumRadioButton);
            this.alignableGroupBoxDepthPerception.Controls.Add(this.prefsDepthHighRadioButton);
            this.alignableGroupBoxDepthPerception.Location = new System.Drawing.Point(156, 0);
            this.alignableGroupBoxDepthPerception.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxDepthPerception.Name = "alignableGroupBoxDepthPerception";
            this.alignableGroupBoxDepthPerception.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxDepthPerception.Size = new System.Drawing.Size(146, 93);
            this.alignableGroupBoxDepthPerception.TabIndex = 0;
            this.alignableGroupBoxDepthPerception.TabStop = false;
            this.alignableGroupBoxDepthPerception.Text = "depth perception";
            // 
            // labelMin
            // 
            this.labelMin.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelMin.Location = new System.Drawing.Point(14, 34);
            this.labelMin.Margin = new System.Windows.Forms.Padding(0);
            this.labelMin.Name = "labelMin";
            this.labelMin.Size = new System.Drawing.Size(18, 15);
            this.labelMin.TabIndex = 1;
            this.labelMin.TabStop = true;
            this.labelMin.Text = "min";
            // 
            // labelMax
            // 
            this.labelMax.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelMax.Location = new System.Drawing.Point(404, 36);
            this.labelMax.Margin = new System.Windows.Forms.Padding(0);
            this.labelMax.Name = "labelMax";
            this.labelMax.Size = new System.Drawing.Size(21, 15);
            this.labelMax.TabIndex = 2;
            this.labelMax.TabStop = true;
            this.labelMax.Text = "max";
            // 
            // alignableGroupBoxMineRenderDepth
            // 
            this.alignableGroupBoxMineRenderDepth.Controls.Add(this.prefsViewdistTextLabel);
            this.alignableGroupBoxMineRenderDepth.Controls.Add(this.prefsViewdistTrackBar);
            this.alignableGroupBoxMineRenderDepth.Controls.Add(this.labelMin);
            this.alignableGroupBoxMineRenderDepth.Controls.Add(this.labelMax);
            this.alignableGroupBoxMineRenderDepth.Location = new System.Drawing.Point(8, 94);
            this.alignableGroupBoxMineRenderDepth.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMineRenderDepth.Name = "alignableGroupBoxMineRenderDepth";
            this.alignableGroupBoxMineRenderDepth.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMineRenderDepth.Size = new System.Drawing.Size(446, 57);
            this.alignableGroupBoxMineRenderDepth.TabIndex = 1;
            this.alignableGroupBoxMineRenderDepth.TabStop = false;
            this.alignableGroupBoxMineRenderDepth.Text = "mine render depth";
            // 
            // prefsViewdistTextLabel
            // 
            this.prefsViewdistTextLabel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsViewdistTextLabel.Location = new System.Drawing.Point(424, 13);
            this.prefsViewdistTextLabel.Margin = new System.Windows.Forms.Padding(0);
            this.prefsViewdistTextLabel.Name = "prefsViewdistTextLabel";
            this.prefsViewdistTextLabel.Size = new System.Drawing.Size(14, 15);
            this.prefsViewdistTextLabel.TabIndex = 3;
            this.prefsViewdistTextLabel.TabStop = true;
            this.prefsViewdistTextLabel.Text = "all";
            // 
            // alignableGroupBoxMineCenterDisplay
            // 
            this.alignableGroupBoxMineCenterDisplay.Controls.Add(this.prefsMinecenterComboBox);
            this.alignableGroupBoxMineCenterDisplay.Location = new System.Drawing.Point(308, 0);
            this.alignableGroupBoxMineCenterDisplay.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMineCenterDisplay.Name = "alignableGroupBoxMineCenterDisplay";
            this.alignableGroupBoxMineCenterDisplay.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMineCenterDisplay.Size = new System.Drawing.Size(146, 93);
            this.alignableGroupBoxMineCenterDisplay.TabIndex = 2;
            this.alignableGroupBoxMineCenterDisplay.TabStop = false;
            this.alignableGroupBoxMineCenterDisplay.Text = "mine center display";
            // 
            // alignableGroupBoxRenderer
            // 
            this.alignableGroupBoxRenderer.Controls.Add(this.prefsRenderer3RdPersonRadioButton);
            this.alignableGroupBoxRenderer.Controls.Add(this.prefsRenderer1StPersonRadioButton);
            this.alignableGroupBoxRenderer.Location = new System.Drawing.Point(8, 0);
            this.alignableGroupBoxRenderer.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxRenderer.Name = "alignableGroupBoxRenderer";
            this.alignableGroupBoxRenderer.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxRenderer.Size = new System.Drawing.Size(146, 93);
            this.alignableGroupBoxRenderer.TabIndex = 3;
            this.alignableGroupBoxRenderer.TabStop = false;
            this.alignableGroupBoxRenderer.Text = "renderer";
            // 
            // prefsRenderer3RdPersonRadioButton
            // 
            this.prefsRenderer3RdPersonRadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsRenderer3RdPersonRadioButton.Location = new System.Drawing.Point(39, 29);
            this.prefsRenderer3RdPersonRadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsRenderer3RdPersonRadioButton.Name = "prefsRenderer3RdPersonRadioButton";
            this.prefsRenderer3RdPersonRadioButton.Size = new System.Drawing.Size(76, 18);
            this.prefsRenderer3RdPersonRadioButton.TabIndex = 0;
            this.prefsRenderer3RdPersonRadioButton.TabStop = true;
            this.prefsRenderer3RdPersonRadioButton.Text = "3rd person";
            // 
            // prefsRenderer1StPersonRadioButton
            // 
            this.prefsRenderer1StPersonRadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsRenderer1StPersonRadioButton.Location = new System.Drawing.Point(39, 50);
            this.prefsRenderer1StPersonRadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsRenderer1StPersonRadioButton.Name = "prefsRenderer1StPersonRadioButton";
            this.prefsRenderer1StPersonRadioButton.Size = new System.Drawing.Size(80, 18);
            this.prefsRenderer1StPersonRadioButton.TabIndex = 1;
            this.prefsRenderer1StPersonRadioButton.TabStop = true;
            this.prefsRenderer1StPersonRadioButton.Text = "1st person";
            // 
            // PreferencesRenderer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.alignableGroupBoxDepthPerception);
            this.Controls.Add(this.alignableGroupBoxMineRenderDepth);
            this.Controls.Add(this.alignableGroupBoxMineCenterDisplay);
            this.Controls.Add(this.alignableGroupBoxRenderer);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "PreferencesRenderer";
            this.Size = new System.Drawing.Size(618, 162);
            ((System.ComponentModel.ISupportInitialize)(this.prefsViewdistTrackBar)).EndInit();
            this.alignableGroupBoxDepthPerception.ResumeLayout(false);
            this.alignableGroupBoxMineRenderDepth.ResumeLayout(false);
            this.alignableGroupBoxMineCenterDisplay.ResumeLayout(false);
            this.alignableGroupBoxRenderer.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RadioButton prefsDepthOffRadioButton;
        private System.Windows.Forms.RadioButton prefsDepthLowRadioButton;
        private System.Windows.Forms.RadioButton prefsDepthMediumRadioButton;
        private System.Windows.Forms.RadioButton prefsDepthHighRadioButton;
        private System.Windows.Forms.TrackBar prefsViewdistTrackBar;
        private System.Windows.Forms.ComboBox prefsMinecenterComboBox;
        private EditorUITest.AlignableGroupBox alignableGroupBoxDepthPerception;
        private System.Windows.Forms.Label labelMin;
        private System.Windows.Forms.Label labelMax;
        private EditorUITest.AlignableGroupBox alignableGroupBoxMineRenderDepth;
        private System.Windows.Forms.Label prefsViewdistTextLabel;
        private EditorUITest.AlignableGroupBox alignableGroupBoxMineCenterDisplay;
        private EditorUITest.AlignableGroupBox alignableGroupBoxRenderer;
        private System.Windows.Forms.RadioButton prefsRenderer3RdPersonRadioButton;
        private System.Windows.Forms.RadioButton prefsRenderer1StPersonRadioButton;

    }
}

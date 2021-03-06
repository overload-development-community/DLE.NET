﻿namespace DLEDotNet.Editor.Layouts
{
    partial class PreferencesFiles
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(PreferencesFiles));
            this.prefsPathD1PigTextBox = new DLEDotNet.Editor.StringTextBox();
            this.prefsPathD2PigTextBox = new DLEDotNet.Editor.StringTextBox();
            this.prefsPathMissionsTextBox = new DLEDotNet.Editor.StringTextBox();
            this.prefsBrowseD1PigButton = new System.Windows.Forms.Button();
            this.prefsBrowseD2PigButton = new System.Windows.Forms.Button();
            this.prefsBrowseMissionsButton = new System.Windows.Forms.Button();
            this.labelD1Pig = new System.Windows.Forms.Label();
            this.labelD2Pig = new System.Windows.Forms.Label();
            this.labelMissions = new System.Windows.Forms.Label();
            this.helpToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.labelChangesOnlyApply = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // prefsPathD1PigTextBox
            // 
            this.prefsPathD1PigTextBox.Location = new System.Drawing.Point(58, 16);
            this.prefsPathD1PigTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsPathD1PigTextBox.Name = "prefsPathD1PigTextBox";
            this.prefsPathD1PigTextBox.Size = new System.Drawing.Size(300, 20);
            this.prefsPathD1PigTextBox.TabIndex = 0;
            this.prefsPathD1PigTextBox.TabStop = false;
            this.helpToolTip.SetToolTip(this.prefsPathD1PigTextBox, "a full absolute or relative path to DESCENT.PIG from Descent 1");
            this.prefsPathD1PigTextBox.Value = "";
            // 
            // prefsPathD2PigTextBox
            // 
            this.prefsPathD2PigTextBox.Location = new System.Drawing.Point(58, 39);
            this.prefsPathD2PigTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsPathD2PigTextBox.Name = "prefsPathD2PigTextBox";
            this.prefsPathD2PigTextBox.Size = new System.Drawing.Size(300, 20);
            this.prefsPathD2PigTextBox.TabIndex = 1;
            this.prefsPathD2PigTextBox.TabStop = false;
            this.helpToolTip.SetToolTip(this.prefsPathD2PigTextBox, "a full absolute or relative path to any of the .PIG files from Descent 2; use thi" +
        "s to change level palette");
            this.prefsPathD2PigTextBox.Value = "";
            // 
            // prefsPathMissionsTextBox
            // 
            this.prefsPathMissionsTextBox.Location = new System.Drawing.Point(58, 62);
            this.prefsPathMissionsTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsPathMissionsTextBox.Name = "prefsPathMissionsTextBox";
            this.prefsPathMissionsTextBox.Size = new System.Drawing.Size(300, 20);
            this.prefsPathMissionsTextBox.TabIndex = 2;
            this.prefsPathMissionsTextBox.TabStop = false;
            this.helpToolTip.SetToolTip(this.prefsPathMissionsTextBox, "path to missions, will be used as default in dialogs and to help with loading ass" +
        "ociated mission files");
            this.prefsPathMissionsTextBox.Value = "";
            // 
            // prefsBrowseD1PigButton
            // 
            this.prefsBrowseD1PigButton.Image = ((System.Drawing.Image)(resources.GetObject("prefsBrowseD1PigButton.Image")));
            this.prefsBrowseD1PigButton.Location = new System.Drawing.Point(364, 13);
            this.prefsBrowseD1PigButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsBrowseD1PigButton.Name = "prefsBrowseD1PigButton";
            this.prefsBrowseD1PigButton.Size = new System.Drawing.Size(22, 24);
            this.prefsBrowseD1PigButton.TabIndex = 3;
            this.helpToolTip.SetToolTip(this.prefsBrowseD1PigButton, "Browse...");
            // 
            // prefsBrowseD2PigButton
            // 
            this.prefsBrowseD2PigButton.Image = ((System.Drawing.Image)(resources.GetObject("prefsBrowseD2PigButton.Image")));
            this.prefsBrowseD2PigButton.Location = new System.Drawing.Point(364, 36);
            this.prefsBrowseD2PigButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsBrowseD2PigButton.Name = "prefsBrowseD2PigButton";
            this.prefsBrowseD2PigButton.Size = new System.Drawing.Size(22, 24);
            this.prefsBrowseD2PigButton.TabIndex = 4;
            this.helpToolTip.SetToolTip(this.prefsBrowseD2PigButton, "Browse...");
            // 
            // prefsBrowseMissionsButton
            // 
            this.prefsBrowseMissionsButton.Image = ((System.Drawing.Image)(resources.GetObject("prefsBrowseMissionsButton.Image")));
            this.prefsBrowseMissionsButton.Location = new System.Drawing.Point(364, 58);
            this.prefsBrowseMissionsButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsBrowseMissionsButton.Name = "prefsBrowseMissionsButton";
            this.prefsBrowseMissionsButton.Size = new System.Drawing.Size(22, 24);
            this.prefsBrowseMissionsButton.TabIndex = 5;
            this.helpToolTip.SetToolTip(this.prefsBrowseMissionsButton, "Browse...");
            // 
            // labelD1Pig
            // 
            this.labelD1Pig.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelD1Pig.Location = new System.Drawing.Point(16, 20);
            this.labelD1Pig.Margin = new System.Windows.Forms.Padding(0);
            this.labelD1Pig.Name = "labelD1Pig";
            this.labelD1Pig.Size = new System.Drawing.Size(40, 15);
            this.labelD1Pig.TabIndex = 6;
            this.labelD1Pig.TabStop = true;
            this.labelD1Pig.Text = "D1 PIG:";
            // 
            // labelD2Pig
            // 
            this.labelD2Pig.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelD2Pig.Location = new System.Drawing.Point(16, 42);
            this.labelD2Pig.Margin = new System.Windows.Forms.Padding(0);
            this.labelD2Pig.Name = "labelD2Pig";
            this.labelD2Pig.Size = new System.Drawing.Size(40, 15);
            this.labelD2Pig.TabIndex = 7;
            this.labelD2Pig.TabStop = true;
            this.labelD2Pig.Text = "D2 PIG:";
            // 
            // labelMissions
            // 
            this.labelMissions.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelMissions.Location = new System.Drawing.Point(12, 65);
            this.labelMissions.Margin = new System.Windows.Forms.Padding(0);
            this.labelMissions.Name = "labelMissions";
            this.labelMissions.Size = new System.Drawing.Size(46, 15);
            this.labelMissions.TabIndex = 8;
            this.labelMissions.TabStop = true;
            this.labelMissions.Text = "Missions:";
            // 
            // labelChangesOnlyApply
            // 
            this.labelChangesOnlyApply.Location = new System.Drawing.Point(12, 115);
            this.labelChangesOnlyApply.Name = "labelChangesOnlyApply";
            this.labelChangesOnlyApply.Size = new System.Drawing.Size(208, 30);
            this.labelChangesOnlyApply.TabIndex = 9;
            this.labelChangesOnlyApply.Text = "Changes here only apply on save.";
            // 
            // PreferencesFiles
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.labelChangesOnlyApply);
            this.Controls.Add(this.prefsPathD1PigTextBox);
            this.Controls.Add(this.prefsPathD2PigTextBox);
            this.Controls.Add(this.prefsPathMissionsTextBox);
            this.Controls.Add(this.prefsBrowseD1PigButton);
            this.Controls.Add(this.prefsBrowseD2PigButton);
            this.Controls.Add(this.prefsBrowseMissionsButton);
            this.Controls.Add(this.labelD1Pig);
            this.Controls.Add(this.labelD2Pig);
            this.Controls.Add(this.labelMissions);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "PreferencesFiles";
            this.Size = new System.Drawing.Size(618, 162);
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private StringTextBox prefsPathD1PigTextBox;
        private StringTextBox prefsPathD2PigTextBox;
        private StringTextBox prefsPathMissionsTextBox;
        private System.Windows.Forms.Button prefsBrowseD1PigButton;
        private System.Windows.Forms.Button prefsBrowseD2PigButton;
        private System.Windows.Forms.Button prefsBrowseMissionsButton;
        private System.Windows.Forms.Label labelD1Pig;
        private System.Windows.Forms.Label labelD2Pig;
        private System.Windows.Forms.Label labelMissions;
        private System.Windows.Forms.ToolTip helpToolTip;
        private System.Windows.Forms.Label labelChangesOnlyApply;
    }
}

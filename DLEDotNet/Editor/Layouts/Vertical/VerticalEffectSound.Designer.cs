﻿namespace DLEDotNet.Editor.Layouts.Vertical
{
    partial class VerticalEffectSound
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
            this.soundFileTextBox = new System.Windows.Forms.TextBox();
            this.soundVolumeSliderTrackBar = new System.Windows.Forms.TrackBar();
            this.labelWav = new System.Windows.Forms.Label();
            this.labelSoundFile = new System.Windows.Forms.Label();
            this.labelVolume = new System.Windows.Forms.Label();
            this.tSoundVolumeLabel = new System.Windows.Forms.Label();
            this.effectEnabledCheckBox = new System.Windows.Forms.CheckBox();
            this.leftRightControl1 = new LeftRightControl();
            ((System.ComponentModel.ISupportInitialize)(this.soundVolumeSliderTrackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // soundFileTextBox
            // 
            this.soundFileTextBox.Location = new System.Drawing.Point(70, 16);
            this.soundFileTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.soundFileTextBox.Name = "soundFileTextBox";
            this.soundFileTextBox.Size = new System.Drawing.Size(193, 20);
            this.soundFileTextBox.TabIndex = 0;
            this.soundFileTextBox.TabStop = false;
            // 
            // soundVolumeSliderTrackBar
            // 
            this.soundVolumeSliderTrackBar.AutoSize = false;
            this.soundVolumeSliderTrackBar.Location = new System.Drawing.Point(66, 49);
            this.soundVolumeSliderTrackBar.Margin = new System.Windows.Forms.Padding(0);
            this.soundVolumeSliderTrackBar.Name = "soundVolumeSliderTrackBar";
            this.soundVolumeSliderTrackBar.Size = new System.Drawing.Size(172, 24);
            this.soundVolumeSliderTrackBar.TabIndex = 1;
            // 
            // labelWav
            // 
            this.labelWav.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelWav.Location = new System.Drawing.Point(266, 20);
            this.labelWav.Margin = new System.Windows.Forms.Padding(0);
            this.labelWav.Name = "labelWav";
            this.labelWav.Size = new System.Drawing.Size(26, 15);
            this.labelWav.TabIndex = 2;
            this.labelWav.TabStop = true;
            this.labelWav.Text = ".wav";
            // 
            // labelSoundFile
            // 
            this.labelSoundFile.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelSoundFile.Location = new System.Drawing.Point(20, 20);
            this.labelSoundFile.Margin = new System.Windows.Forms.Padding(0);
            this.labelSoundFile.Name = "labelSoundFile";
            this.labelSoundFile.Size = new System.Drawing.Size(51, 15);
            this.labelSoundFile.TabIndex = 3;
            this.labelSoundFile.TabStop = true;
            this.labelSoundFile.Text = "sound file:";
            // 
            // labelVolume
            // 
            this.labelVolume.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelVolume.Location = new System.Drawing.Point(16, 52);
            this.labelVolume.Margin = new System.Windows.Forms.Padding(0);
            this.labelVolume.Name = "labelVolume";
            this.labelVolume.Size = new System.Drawing.Size(52, 15);
            this.labelVolume.TabIndex = 4;
            this.labelVolume.TabStop = true;
            this.labelVolume.Text = "volume:";
            // 
            // tSoundVolumeLabel
            // 
            this.tSoundVolumeLabel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.tSoundVolumeLabel.Location = new System.Drawing.Point(264, 52);
            this.tSoundVolumeLabel.Margin = new System.Windows.Forms.Padding(0);
            this.tSoundVolumeLabel.Name = "tSoundVolumeLabel";
            this.tSoundVolumeLabel.Size = new System.Drawing.Size(32, 15);
            this.tSoundVolumeLabel.TabIndex = 5;
            this.tSoundVolumeLabel.TabStop = true;
            this.tSoundVolumeLabel.Text = "####";
            // 
            // effectEnabledCheckBox
            // 
            this.effectEnabledCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectEnabledCheckBox.Location = new System.Drawing.Point(237, 401);
            this.effectEnabledCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.effectEnabledCheckBox.Name = "effectEnabledCheckBox";
            this.effectEnabledCheckBox.Size = new System.Drawing.Size(58, 18);
            this.effectEnabledCheckBox.TabIndex = 7;
            this.effectEnabledCheckBox.Text = "enabled";
            // 
            // leftRightControl1
            // 
            this.leftRightControl1.LinkedControl = this.soundVolumeSliderTrackBar;
            this.leftRightControl1.Location = new System.Drawing.Point(233, 49);
            this.leftRightControl1.MaximumSize = new System.Drawing.Size(30, 23);
            this.leftRightControl1.MinimumSize = new System.Drawing.Size(30, 23);
            this.leftRightControl1.Name = "leftRightControl1";
            this.leftRightControl1.Size = new System.Drawing.Size(30, 23);
            this.leftRightControl1.TabIndex = 9;
            // 
            // VerticalEffectSound
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.leftRightControl1);
            this.Controls.Add(this.soundFileTextBox);
            this.Controls.Add(this.soundVolumeSliderTrackBar);
            this.Controls.Add(this.labelWav);
            this.Controls.Add(this.labelSoundFile);
            this.Controls.Add(this.labelVolume);
            this.Controls.Add(this.tSoundVolumeLabel);
            this.Controls.Add(this.effectEnabledCheckBox);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "VerticalEffectSound";
            this.Size = new System.Drawing.Size(300, 422);
            ((System.ComponentModel.ISupportInitialize)(this.soundVolumeSliderTrackBar)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox soundFileTextBox;
        private System.Windows.Forms.TrackBar soundVolumeSliderTrackBar;
        private System.Windows.Forms.Label labelWav;
        private System.Windows.Forms.Label labelSoundFile;
        private System.Windows.Forms.Label labelVolume;
        private System.Windows.Forms.Label tSoundVolumeLabel;
        private System.Windows.Forms.CheckBox effectEnabledCheckBox;
        private LeftRightControl leftRightControl1;
    }
}

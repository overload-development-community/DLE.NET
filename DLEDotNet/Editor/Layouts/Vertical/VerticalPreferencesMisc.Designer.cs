namespace DLEDotNet.Editor.Layouts.Vertical
{
    partial class VerticalPreferencesMisc
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
            this.prefsExpertmodeCheckBox = new System.Windows.Forms.CheckBox();
            this.prefsSplashscreenCheckBox = new System.Windows.Forms.CheckBox();
            this.prefsPlayerTextBox = new System.Windows.Forms.TextBox();
            this.prefsUsetexcolorsCheckBox = new System.Windows.Forms.CheckBox();
            this.alignableGroupBoxVerbosity = new AlignableGroupBox();
            this.alignableGroupBoxPlayerProfile = new AlignableGroupBox();
            this.alignableGroupBoxFaceLightSettings = new AlignableGroupBox();
            this.alignableGroupBoxVerbosity.SuspendLayout();
            this.alignableGroupBoxPlayerProfile.SuspendLayout();
            this.alignableGroupBoxFaceLightSettings.SuspendLayout();
            this.SuspendLayout();
            // 
            // prefsExpertmodeCheckBox
            // 
            this.prefsExpertmodeCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsExpertmodeCheckBox.Location = new System.Drawing.Point(12, 21);
            this.prefsExpertmodeCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsExpertmodeCheckBox.Name = "prefsExpertmodeCheckBox";
            this.prefsExpertmodeCheckBox.Size = new System.Drawing.Size(220, 18);
            this.prefsExpertmodeCheckBox.TabIndex = 0;
            this.prefsExpertmodeCheckBox.Text = "stop askin\' those questions all the time! ;-)";
            // 
            // prefsSplashscreenCheckBox
            // 
            this.prefsSplashscreenCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsSplashscreenCheckBox.Location = new System.Drawing.Point(12, 41);
            this.prefsSplashscreenCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsSplashscreenCheckBox.Name = "prefsSplashscreenCheckBox";
            this.prefsSplashscreenCheckBox.Size = new System.Drawing.Size(206, 18);
            this.prefsSplashscreenCheckBox.TabIndex = 1;
            this.prefsSplashscreenCheckBox.Text = "show splash screen on program start";
            // 
            // prefsPlayerTextBox
            // 
            this.prefsPlayerTextBox.Location = new System.Drawing.Point(6, 28);
            this.prefsPlayerTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsPlayerTextBox.Name = "prefsPlayerTextBox";
            this.prefsPlayerTextBox.Size = new System.Drawing.Size(94, 19);
            this.prefsPlayerTextBox.TabIndex = 0;
            this.prefsPlayerTextBox.TabStop = false;
            this.prefsPlayerTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // prefsUsetexcolorsCheckBox
            // 
            this.prefsUsetexcolorsCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsUsetexcolorsCheckBox.Location = new System.Drawing.Point(14, 29);
            this.prefsUsetexcolorsCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsUsetexcolorsCheckBox.Name = "prefsUsetexcolorsCheckBox";
            this.prefsUsetexcolorsCheckBox.Size = new System.Drawing.Size(87, 16);
            this.prefsUsetexcolorsCheckBox.TabIndex = 0;
            this.prefsUsetexcolorsCheckBox.Text = "apply globally";
            // 
            // alignableGroupBoxVerbosity
            // 
            this.alignableGroupBoxVerbosity.Controls.Add(this.prefsExpertmodeCheckBox);
            this.alignableGroupBoxVerbosity.Controls.Add(this.prefsSplashscreenCheckBox);
            this.alignableGroupBoxVerbosity.Location = new System.Drawing.Point(32, 75);
            this.alignableGroupBoxVerbosity.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxVerbosity.Name = "alignableGroupBoxVerbosity";
            this.alignableGroupBoxVerbosity.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxVerbosity.Size = new System.Drawing.Size(236, 67);
            this.alignableGroupBoxVerbosity.TabIndex = 0;
            this.alignableGroupBoxVerbosity.TabStop = false;
            this.alignableGroupBoxVerbosity.Text = "Verbosity";
            // 
            // alignableGroupBoxPlayerProfile
            // 
            this.alignableGroupBoxPlayerProfile.Controls.Add(this.prefsPlayerTextBox);
            this.alignableGroupBoxPlayerProfile.Location = new System.Drawing.Point(33, 5);
            this.alignableGroupBoxPlayerProfile.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxPlayerProfile.Name = "alignableGroupBoxPlayerProfile";
            this.alignableGroupBoxPlayerProfile.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxPlayerProfile.Size = new System.Drawing.Size(114, 67);
            this.alignableGroupBoxPlayerProfile.TabIndex = 1;
            this.alignableGroupBoxPlayerProfile.TabStop = false;
            this.alignableGroupBoxPlayerProfile.Text = "player profile";
            // 
            // alignableGroupBoxFaceLightSettings
            // 
            this.alignableGroupBoxFaceLightSettings.Controls.Add(this.prefsUsetexcolorsCheckBox);
            this.alignableGroupBoxFaceLightSettings.Location = new System.Drawing.Point(153, 5);
            this.alignableGroupBoxFaceLightSettings.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxFaceLightSettings.Name = "alignableGroupBoxFaceLightSettings";
            this.alignableGroupBoxFaceLightSettings.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxFaceLightSettings.Size = new System.Drawing.Size(114, 67);
            this.alignableGroupBoxFaceLightSettings.TabIndex = 2;
            this.alignableGroupBoxFaceLightSettings.TabStop = false;
            this.alignableGroupBoxFaceLightSettings.Text = "face light settings";
            // 
            // VerticalPreferencesMisc
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.alignableGroupBoxVerbosity);
            this.Controls.Add(this.alignableGroupBoxPlayerProfile);
            this.Controls.Add(this.alignableGroupBoxFaceLightSettings);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "VerticalPreferencesMisc";
            this.Size = new System.Drawing.Size(300, 341);
            this.alignableGroupBoxVerbosity.ResumeLayout(false);
            this.alignableGroupBoxPlayerProfile.ResumeLayout(false);
            this.alignableGroupBoxFaceLightSettings.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.CheckBox prefsExpertmodeCheckBox;
        private System.Windows.Forms.CheckBox prefsSplashscreenCheckBox;
        private System.Windows.Forms.TextBox prefsPlayerTextBox;
        private System.Windows.Forms.CheckBox prefsUsetexcolorsCheckBox;
        private AlignableGroupBox alignableGroupBoxVerbosity;
        private AlignableGroupBox alignableGroupBoxPlayerProfile;
        private AlignableGroupBox alignableGroupBoxFaceLightSettings;

    }
}

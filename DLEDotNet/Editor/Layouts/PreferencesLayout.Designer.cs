﻿namespace DLEDotNet.Editor.Layouts
{
    partial class PreferencesLayout
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
            this.prefsLayout0RadioButton = new System.Windows.Forms.RadioButton();
            this.prefsLayout1RadioButton = new System.Windows.Forms.RadioButton();
            this.prefsLayout2RadioButton = new System.Windows.Forms.RadioButton();
            this.prefsLayout3RadioButton = new System.Windows.Forms.RadioButton();
            this.labelChangesOnlyApply = new System.Windows.Forms.Label();
            this.helpToolTip = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // prefsLayout0RadioButton
            // 
            this.prefsLayout0RadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsLayout0RadioButton.Location = new System.Drawing.Point(22, 16);
            this.prefsLayout0RadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsLayout0RadioButton.Name = "prefsLayout0RadioButton";
            this.prefsLayout0RadioButton.Size = new System.Drawing.Size(70, 18);
            this.prefsLayout0RadioButton.TabIndex = 0;
            this.prefsLayout0RadioButton.TabStop = true;
            this.prefsLayout0RadioButton.Text = "horizontal";
            // 
            // prefsLayout1RadioButton
            // 
            this.prefsLayout1RadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsLayout1RadioButton.Location = new System.Drawing.Point(22, 36);
            this.prefsLayout1RadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsLayout1RadioButton.Name = "prefsLayout1RadioButton";
            this.prefsLayout1RadioButton.Size = new System.Drawing.Size(60, 18);
            this.prefsLayout1RadioButton.TabIndex = 1;
            this.prefsLayout1RadioButton.TabStop = true;
            this.prefsLayout1RadioButton.Text = "vertical";
            // 
            // prefsLayout2RadioButton
            // 
            this.prefsLayout2RadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsLayout2RadioButton.Location = new System.Drawing.Point(22, 57);
            this.prefsLayout2RadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsLayout2RadioButton.Name = "prefsLayout2RadioButton";
            this.prefsLayout2RadioButton.Size = new System.Drawing.Size(60, 18);
            this.prefsLayout2RadioButton.TabIndex = 2;
            this.prefsLayout2RadioButton.TabStop = true;
            this.prefsLayout2RadioButton.Text = "floating";
            this.helpToolTip.SetToolTip(this.prefsLayout2RadioButton, "every tool is in its own dialog");
            // 
            // prefsLayout3RadioButton
            // 
            this.prefsLayout3RadioButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsLayout3RadioButton.Location = new System.Drawing.Point(22, 75);
            this.prefsLayout3RadioButton.Margin = new System.Windows.Forms.Padding(0);
            this.prefsLayout3RadioButton.Name = "prefsLayout3RadioButton";
            this.prefsLayout3RadioButton.Size = new System.Drawing.Size(152, 24);
            this.prefsLayout3RadioButton.TabIndex = 3;
            this.prefsLayout3RadioButton.TabStop = true;
            this.prefsLayout3RadioButton.Text = "vertical (space saving)";
            // 
            // labelChangesOnlyApply
            // 
            this.labelChangesOnlyApply.Location = new System.Drawing.Point(12, 115);
            this.labelChangesOnlyApply.Name = "labelChangesOnlyApply";
            this.labelChangesOnlyApply.Size = new System.Drawing.Size(208, 30);
            this.labelChangesOnlyApply.TabIndex = 10;
            this.labelChangesOnlyApply.Text = "Changes here only apply on save.";
            // 
            // PreferencesLayout
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.labelChangesOnlyApply);
            this.Controls.Add(this.prefsLayout0RadioButton);
            this.Controls.Add(this.prefsLayout1RadioButton);
            this.Controls.Add(this.prefsLayout2RadioButton);
            this.Controls.Add(this.prefsLayout3RadioButton);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "PreferencesLayout";
            this.Size = new System.Drawing.Size(618, 162);
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RadioButton prefsLayout0RadioButton;
        private System.Windows.Forms.RadioButton prefsLayout1RadioButton;
        private System.Windows.Forms.RadioButton prefsLayout2RadioButton;
        private System.Windows.Forms.RadioButton prefsLayout3RadioButton;
        private System.Windows.Forms.Label labelChangesOnlyApply;
        private System.Windows.Forms.ToolTip helpToolTip;
    }
}

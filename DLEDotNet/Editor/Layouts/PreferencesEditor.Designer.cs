﻿namespace DLEDotNet.Editor.Layouts
{
    partial class PreferencesEditor
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
            this.prefsRotateRateSliderTrackBar = new System.Windows.Forms.TrackBar();
            this.tPrefsRotateRateLabel = new System.Windows.Forms.Label();
            this.alignableGroupBoxRotateRate = new DLEDotNet.Editor.AlignableGroupBox();
            this.prefsRotateRateSliderLeftRightControl = new DLEDotNet.Editor.LeftRightControl();
            this.prefsMoverateTextBox = new DLEDotNet.Editor.FloatTextBox();
            this.prefsViewMoverateTextBox = new DLEDotNet.Editor.FloatTextBox();
            this.alignableGroupBoxMoveRateUnits = new DLEDotNet.Editor.AlignableGroupBox();
            this.labelMine = new System.Windows.Forms.Label();
            this.labelView = new System.Windows.Forms.Label();
            this.prefsUndoTextBox = new DLEDotNet.Editor.IntTextBox();
            this.labelUndos0Hyph500 = new System.Windows.Forms.Label();
            this.alignableGroupBoxUndo = new DLEDotNet.Editor.AlignableGroupBox();
            this.alignableGroupBoxMisc = new DLEDotNet.Editor.AlignableGroupBox();
            this.prefsAllowobjectoverlapCheckBox = new System.Windows.Forms.CheckBox();
            this.prefsUpdatetexalignCheckBox = new System.Windows.Forms.CheckBox();
            this.helpToolTip = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.prefsRotateRateSliderTrackBar)).BeginInit();
            this.alignableGroupBoxRotateRate.SuspendLayout();
            this.alignableGroupBoxMoveRateUnits.SuspendLayout();
            this.alignableGroupBoxUndo.SuspendLayout();
            this.alignableGroupBoxMisc.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // prefsRotateRateSliderTrackBar
            // 
            this.prefsRotateRateSliderTrackBar.AutoSize = false;
            this.prefsRotateRateSliderTrackBar.Location = new System.Drawing.Point(6, 15);
            this.prefsRotateRateSliderTrackBar.Margin = new System.Windows.Forms.Padding(0);
            this.prefsRotateRateSliderTrackBar.Maximum = 8;
            this.prefsRotateRateSliderTrackBar.Name = "prefsRotateRateSliderTrackBar";
            this.prefsRotateRateSliderTrackBar.Size = new System.Drawing.Size(136, 26);
            this.prefsRotateRateSliderTrackBar.TabIndex = 0;
            this.prefsRotateRateSliderTrackBar.TickStyle = System.Windows.Forms.TickStyle.None;
            this.prefsRotateRateSliderTrackBar.Value = 4;
            // 
            // tPrefsRotateRateLabel
            // 
            this.tPrefsRotateRateLabel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.tPrefsRotateRateLabel.Location = new System.Drawing.Point(54, 47);
            this.tPrefsRotateRateLabel.Margin = new System.Windows.Forms.Padding(0);
            this.tPrefsRotateRateLabel.Name = "tPrefsRotateRateLabel";
            this.tPrefsRotateRateLabel.Size = new System.Drawing.Size(45, 15);
            this.tPrefsRotateRateLabel.TabIndex = 1;
            this.tPrefsRotateRateLabel.TabStop = true;
            this.tPrefsRotateRateLabel.Text = "######";
            this.tPrefsRotateRateLabel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // alignableGroupBoxRotateRate
            // 
            this.alignableGroupBoxRotateRate.Controls.Add(this.prefsRotateRateSliderLeftRightControl);
            this.alignableGroupBoxRotateRate.Controls.Add(this.prefsRotateRateSliderTrackBar);
            this.alignableGroupBoxRotateRate.Controls.Add(this.tPrefsRotateRateLabel);
            this.alignableGroupBoxRotateRate.Location = new System.Drawing.Point(159, 5);
            this.alignableGroupBoxRotateRate.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxRotateRate.Name = "alignableGroupBoxRotateRate";
            this.alignableGroupBoxRotateRate.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxRotateRate.Size = new System.Drawing.Size(146, 73);
            this.alignableGroupBoxRotateRate.TabIndex = 0;
            this.alignableGroupBoxRotateRate.TabStop = false;
            this.alignableGroupBoxRotateRate.Text = "Rotate Rate";
            // 
            // prefsRotateRateSliderLeftRightControl
            // 
            this.prefsRotateRateSliderLeftRightControl.LinkedControl = this.prefsRotateRateSliderTrackBar;
            this.prefsRotateRateSliderLeftRightControl.Location = new System.Drawing.Point(109, 41);
            this.prefsRotateRateSliderLeftRightControl.MaximumSize = new System.Drawing.Size(30, 23);
            this.prefsRotateRateSliderLeftRightControl.MinimumSize = new System.Drawing.Size(30, 23);
            this.prefsRotateRateSliderLeftRightControl.Name = "prefsRotateRateSliderLeftRightControl";
            this.prefsRotateRateSliderLeftRightControl.Size = new System.Drawing.Size(30, 23);
            this.prefsRotateRateSliderLeftRightControl.TabIndex = 4;
            // 
            // prefsMoverateTextBox
            // 
            this.prefsMoverateTextBox.FractionalDigits = 3;
            this.prefsMoverateTextBox.Location = new System.Drawing.Point(50, 20);
            this.prefsMoverateTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsMoverateTextBox.MinimumValue = 0D;
            this.prefsMoverateTextBox.Name = "prefsMoverateTextBox";
            this.prefsMoverateTextBox.Size = new System.Drawing.Size(49, 20);
            this.prefsMoverateTextBox.TabIndex = 0;
            this.prefsMoverateTextBox.TabStop = false;
            this.prefsMoverateTextBox.Text = "0";
            this.prefsMoverateTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.prefsMoverateTextBox.ZeroPadFraction = false;
            // 
            // prefsViewMoverateTextBox
            // 
            this.prefsViewMoverateTextBox.FractionalDigits = 3;
            this.prefsViewMoverateTextBox.Location = new System.Drawing.Point(50, 41);
            this.prefsViewMoverateTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsViewMoverateTextBox.MinimumValue = 0D;
            this.prefsViewMoverateTextBox.Name = "prefsViewMoverateTextBox";
            this.prefsViewMoverateTextBox.Size = new System.Drawing.Size(49, 20);
            this.prefsViewMoverateTextBox.TabIndex = 1;
            this.prefsViewMoverateTextBox.TabStop = false;
            this.prefsViewMoverateTextBox.Text = "0";
            this.prefsViewMoverateTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.prefsViewMoverateTextBox.ZeroPadFraction = false;
            // 
            // alignableGroupBoxMoveRateUnits
            // 
            this.alignableGroupBoxMoveRateUnits.Controls.Add(this.prefsMoverateTextBox);
            this.alignableGroupBoxMoveRateUnits.Controls.Add(this.prefsViewMoverateTextBox);
            this.alignableGroupBoxMoveRateUnits.Controls.Add(this.labelMine);
            this.alignableGroupBoxMoveRateUnits.Controls.Add(this.labelView);
            this.alignableGroupBoxMoveRateUnits.Location = new System.Drawing.Point(8, 5);
            this.alignableGroupBoxMoveRateUnits.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMoveRateUnits.Name = "alignableGroupBoxMoveRateUnits";
            this.alignableGroupBoxMoveRateUnits.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMoveRateUnits.Size = new System.Drawing.Size(146, 73);
            this.alignableGroupBoxMoveRateUnits.TabIndex = 1;
            this.alignableGroupBoxMoveRateUnits.TabStop = false;
            this.alignableGroupBoxMoveRateUnits.Text = "Move Rate [units]";
            // 
            // labelMine
            // 
            this.labelMine.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelMine.Location = new System.Drawing.Point(21, 23);
            this.labelMine.Margin = new System.Windows.Forms.Padding(0);
            this.labelMine.Name = "labelMine";
            this.labelMine.Size = new System.Drawing.Size(28, 15);
            this.labelMine.TabIndex = 2;
            this.labelMine.TabStop = true;
            this.labelMine.Text = "Mine:";
            // 
            // labelView
            // 
            this.labelView.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelView.Location = new System.Drawing.Point(20, 42);
            this.labelView.Margin = new System.Windows.Forms.Padding(0);
            this.labelView.Name = "labelView";
            this.labelView.Size = new System.Drawing.Size(28, 15);
            this.labelView.TabIndex = 3;
            this.labelView.TabStop = true;
            this.labelView.Text = "View:";
            // 
            // prefsUndoTextBox
            // 
            this.prefsUndoTextBox.Location = new System.Drawing.Point(18, 29);
            this.prefsUndoTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsUndoTextBox.MaximumValue = 500;
            this.prefsUndoTextBox.MinimumValue = 0;
            this.prefsUndoTextBox.Name = "prefsUndoTextBox";
            this.prefsUndoTextBox.Size = new System.Drawing.Size(34, 20);
            this.prefsUndoTextBox.TabIndex = 0;
            this.prefsUndoTextBox.TabStop = false;
            this.prefsUndoTextBox.Text = "0";
            this.prefsUndoTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // labelUndos0Hyph500
            // 
            this.labelUndos0Hyph500.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelUndos0Hyph500.Location = new System.Drawing.Point(54, 32);
            this.labelUndos0Hyph500.Margin = new System.Windows.Forms.Padding(0);
            this.labelUndos0Hyph500.Name = "labelUndos0Hyph500";
            this.labelUndos0Hyph500.Size = new System.Drawing.Size(86, 15);
            this.labelUndos0Hyph500.TabIndex = 1;
            this.labelUndos0Hyph500.TabStop = true;
            this.labelUndos0Hyph500.Text = "undos (0 - #)";
            // 
            // alignableGroupBoxUndo
            // 
            this.alignableGroupBoxUndo.Controls.Add(this.prefsUndoTextBox);
            this.alignableGroupBoxUndo.Controls.Add(this.labelUndos0Hyph500);
            this.alignableGroupBoxUndo.Location = new System.Drawing.Point(8, 80);
            this.alignableGroupBoxUndo.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxUndo.Name = "alignableGroupBoxUndo";
            this.alignableGroupBoxUndo.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxUndo.Size = new System.Drawing.Size(146, 73);
            this.alignableGroupBoxUndo.TabIndex = 2;
            this.alignableGroupBoxUndo.TabStop = false;
            this.alignableGroupBoxUndo.Text = "Undo";
            // 
            // alignableGroupBoxMisc
            // 
            this.alignableGroupBoxMisc.Controls.Add(this.prefsAllowobjectoverlapCheckBox);
            this.alignableGroupBoxMisc.Controls.Add(this.prefsUpdatetexalignCheckBox);
            this.alignableGroupBoxMisc.Location = new System.Drawing.Point(159, 80);
            this.alignableGroupBoxMisc.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMisc.Name = "alignableGroupBoxMisc";
            this.alignableGroupBoxMisc.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxMisc.Size = new System.Drawing.Size(146, 73);
            this.alignableGroupBoxMisc.TabIndex = 3;
            this.alignableGroupBoxMisc.TabStop = false;
            this.alignableGroupBoxMisc.Text = "Misc";
            // 
            // prefsAllowobjectoverlapCheckBox
            // 
            this.prefsAllowobjectoverlapCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsAllowobjectoverlapCheckBox.Location = new System.Drawing.Point(4, 18);
            this.prefsAllowobjectoverlapCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsAllowobjectoverlapCheckBox.Name = "prefsAllowobjectoverlapCheckBox";
            this.prefsAllowobjectoverlapCheckBox.Size = new System.Drawing.Size(135, 18);
            this.prefsAllowobjectoverlapCheckBox.TabIndex = 0;
            this.prefsAllowobjectoverlapCheckBox.Text = "Allow objects to overlap";
            this.helpToolTip.SetToolTip(this.prefsAllowobjectoverlapCheckBox, "if not checked, coincident objects will be bumped");
            // 
            // prefsUpdatetexalignCheckBox
            // 
            this.prefsUpdatetexalignCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.prefsUpdatetexalignCheckBox.Location = new System.Drawing.Point(4, 37);
            this.prefsUpdatetexalignCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.prefsUpdatetexalignCheckBox.Name = "prefsUpdatetexalignCheckBox";
            this.prefsUpdatetexalignCheckBox.Size = new System.Drawing.Size(135, 28);
            this.prefsUpdatetexalignCheckBox.TabIndex = 1;
            this.prefsUpdatetexalignCheckBox.Text = "Adjust texture alignment when editing segments";
            // 
            // PreferencesEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.alignableGroupBoxRotateRate);
            this.Controls.Add(this.alignableGroupBoxMoveRateUnits);
            this.Controls.Add(this.alignableGroupBoxUndo);
            this.Controls.Add(this.alignableGroupBoxMisc);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "PreferencesEditor";
            this.Size = new System.Drawing.Size(618, 162);
            ((System.ComponentModel.ISupportInitialize)(this.prefsRotateRateSliderTrackBar)).EndInit();
            this.alignableGroupBoxRotateRate.ResumeLayout(false);
            this.alignableGroupBoxMoveRateUnits.ResumeLayout(false);
            this.alignableGroupBoxMoveRateUnits.PerformLayout();
            this.alignableGroupBoxUndo.ResumeLayout(false);
            this.alignableGroupBoxUndo.PerformLayout();
            this.alignableGroupBoxMisc.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TrackBar prefsRotateRateSliderTrackBar;
        private System.Windows.Forms.Label tPrefsRotateRateLabel;
        private AlignableGroupBox alignableGroupBoxRotateRate;
        private FloatTextBox prefsMoverateTextBox;
        private FloatTextBox prefsViewMoverateTextBox;
        private AlignableGroupBox alignableGroupBoxMoveRateUnits;
        private System.Windows.Forms.Label labelMine;
        private System.Windows.Forms.Label labelView;
        private IntTextBox prefsUndoTextBox;
        private System.Windows.Forms.Label labelUndos0Hyph500;
        private AlignableGroupBox alignableGroupBoxUndo;
        private AlignableGroupBox alignableGroupBoxMisc;
        private System.Windows.Forms.CheckBox prefsAllowobjectoverlapCheckBox;
        private System.Windows.Forms.CheckBox prefsUpdatetexalignCheckBox;
        private LeftRightControl prefsRotateRateSliderLeftRightControl;
        private System.Windows.Forms.ToolTip helpToolTip;
    }
}

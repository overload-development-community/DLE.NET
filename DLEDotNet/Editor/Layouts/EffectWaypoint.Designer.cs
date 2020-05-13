namespace DLEDotNet.Editor.Layouts
{
    partial class EffectWaypoint
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
            this.waypointIdTextBox = new System.Windows.Forms.TextBox();
            this.waypointSuccTextBox = new System.Windows.Forms.TextBox();
            this.waypointSpeedTextBox = new System.Windows.Forms.TextBox();
            this.labelSpeed = new System.Windows.Forms.Label();
            this.labelId = new System.Windows.Forms.Label();
            this.labelSuccessor = new System.Windows.Forms.Label();
            this.effectEnabledCheckBox = new System.Windows.Forms.CheckBox();
            this.helpToolTip = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // waypointIdTextBox
            // 
            this.waypointIdTextBox.Location = new System.Drawing.Point(146, 41);
            this.waypointIdTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.waypointIdTextBox.Name = "waypointIdTextBox";
            this.waypointIdTextBox.Size = new System.Drawing.Size(42, 19);
            this.waypointIdTextBox.TabIndex = 0;
            this.waypointIdTextBox.TabStop = false;
            this.waypointIdTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // waypointSuccTextBox
            // 
            this.waypointSuccTextBox.Location = new System.Drawing.Point(146, 62);
            this.waypointSuccTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.waypointSuccTextBox.Name = "waypointSuccTextBox";
            this.waypointSuccTextBox.Size = new System.Drawing.Size(42, 19);
            this.waypointSuccTextBox.TabIndex = 1;
            this.waypointSuccTextBox.TabStop = false;
            this.waypointSuccTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // waypointSpeedTextBox
            // 
            this.waypointSpeedTextBox.Location = new System.Drawing.Point(146, 83);
            this.waypointSpeedTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.waypointSpeedTextBox.Name = "waypointSpeedTextBox";
            this.waypointSpeedTextBox.Size = new System.Drawing.Size(42, 19);
            this.waypointSpeedTextBox.TabIndex = 2;
            this.waypointSpeedTextBox.TabStop = false;
            this.waypointSpeedTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // labelSpeed
            // 
            this.labelSpeed.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelSpeed.Location = new System.Drawing.Point(112, 84);
            this.labelSpeed.Margin = new System.Windows.Forms.Padding(0);
            this.labelSpeed.Name = "labelSpeed";
            this.labelSpeed.Size = new System.Drawing.Size(34, 15);
            this.labelSpeed.TabIndex = 3;
            this.labelSpeed.TabStop = true;
            this.labelSpeed.Text = "speed:";
            // 
            // labelId
            // 
            this.labelId.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelId.Location = new System.Drawing.Point(130, 46);
            this.labelId.Margin = new System.Windows.Forms.Padding(0);
            this.labelId.Name = "labelId";
            this.labelId.Size = new System.Drawing.Size(14, 15);
            this.labelId.TabIndex = 4;
            this.labelId.TabStop = true;
            this.labelId.Text = "id:";
            // 
            // labelSuccessor
            // 
            this.labelSuccessor.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelSuccessor.Location = new System.Drawing.Point(92, 65);
            this.labelSuccessor.Margin = new System.Windows.Forms.Padding(0);
            this.labelSuccessor.Name = "labelSuccessor";
            this.labelSuccessor.Size = new System.Drawing.Size(54, 15);
            this.labelSuccessor.TabIndex = 5;
            this.labelSuccessor.TabStop = true;
            this.labelSuccessor.Text = "successor:";
            // 
            // effectEnabledCheckBox
            // 
            this.effectEnabledCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.effectEnabledCheckBox.Location = new System.Drawing.Point(657, 143);
            this.effectEnabledCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.effectEnabledCheckBox.Name = "effectEnabledCheckBox";
            this.effectEnabledCheckBox.Size = new System.Drawing.Size(58, 18);
            this.effectEnabledCheckBox.TabIndex = 6;
            this.effectEnabledCheckBox.Text = "enabled";
            // 
            // EffectWaypoint
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.waypointIdTextBox);
            this.Controls.Add(this.waypointSuccTextBox);
            this.Controls.Add(this.waypointSpeedTextBox);
            this.Controls.Add(this.labelSpeed);
            this.Controls.Add(this.labelId);
            this.Controls.Add(this.labelSuccessor);
            this.Controls.Add(this.effectEnabledCheckBox);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "EffectWaypoint";
            this.Size = new System.Drawing.Size(720, 162);
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox waypointIdTextBox;
        private System.Windows.Forms.TextBox waypointSuccTextBox;
        private System.Windows.Forms.TextBox waypointSpeedTextBox;
        private System.Windows.Forms.Label labelSpeed;
        private System.Windows.Forms.Label labelId;
        private System.Windows.Forms.Label labelSuccessor;
        private System.Windows.Forms.CheckBox effectEnabledCheckBox;
        private System.Windows.Forms.ToolTip helpToolTip;
    }
}

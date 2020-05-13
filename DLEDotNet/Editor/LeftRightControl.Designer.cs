namespace DLEDotNet.Editor
{
    partial class LeftRightControl
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
            this.leftButton = new DLEDotNet.Editor.RepeatingButton();
            this.rightButton = new DLEDotNet.Editor.RepeatingButton();
            this.SuspendLayout();
            // 
            // leftButton
            // 
            this.leftButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.leftButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.leftButton.Location = new System.Drawing.Point(0, 0);
            this.leftButton.Margin = new System.Windows.Forms.Padding(0);
            this.leftButton.Name = "leftButton";
            this.leftButton.Size = new System.Drawing.Size(17, 23);
            this.leftButton.TabIndex = 24;
            this.leftButton.Text = "◀";
            this.leftButton.UseCompatibleTextRendering = true;
            this.leftButton.UseVisualStyleBackColor = true;
            this.leftButton.Click += new System.EventHandler(this.leftButton_Click);
            this.leftButton.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LeftRightControl_KeyDown);
            this.leftButton.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.LeftRightControl_PreviewKeyDown);
            // 
            // rightButton
            // 
            this.rightButton.AccessibleRole = System.Windows.Forms.AccessibleRole.None;
            this.rightButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.rightButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.rightButton.Location = new System.Drawing.Point(14, 0);
            this.rightButton.Margin = new System.Windows.Forms.Padding(0);
            this.rightButton.Name = "rightButton";
            this.rightButton.Size = new System.Drawing.Size(17, 23);
            this.rightButton.TabIndex = 25;
            this.rightButton.Text = "▶";
            this.rightButton.UseCompatibleTextRendering = true;
            this.rightButton.UseVisualStyleBackColor = true;
            this.rightButton.Click += new System.EventHandler(this.rightButton_Click);
            this.rightButton.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LeftRightControl_KeyDown);
            this.rightButton.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.LeftRightControl_PreviewKeyDown);
            // 
            // LeftRightControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.leftButton);
            this.Controls.Add(this.rightButton);
            this.MaximumSize = new System.Drawing.Size(30, 23);
            this.MinimumSize = new System.Drawing.Size(30, 23);
            this.Name = "LeftRightControl";
            this.Size = new System.Drawing.Size(30, 23);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.LeftRightControl_KeyDown);
            this.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.LeftRightControl_PreviewKeyDown);
            this.ResumeLayout(false);

        }

        #endregion

        private RepeatingButton leftButton;
        private RepeatingButton rightButton;
    }
}

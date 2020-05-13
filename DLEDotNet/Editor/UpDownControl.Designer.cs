namespace DLEDotNet.Editor
{
    partial class UpDownControl
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
            this.downButton = new DLEDotNet.Editor.RepeatingButton();
            this.upButton = new DLEDotNet.Editor.RepeatingButton();
            this.SuspendLayout();
            // 
            // downButton
            // 
            this.downButton.AccessibleRole = System.Windows.Forms.AccessibleRole.None;
            this.downButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.downButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.downButton.Font = new System.Drawing.Font("Webdings", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.downButton.Location = new System.Drawing.Point(0, 9);
            this.downButton.Name = "downButton";
            this.downButton.Padding = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.downButton.Size = new System.Drawing.Size(21, 12);
            this.downButton.TabIndex = 25;
            this.downButton.Text = "6";
            this.downButton.UseCompatibleTextRendering = true;
            this.downButton.UseVisualStyleBackColor = true;
            this.downButton.Click += new System.EventHandler(this.downButton_Click);
            this.downButton.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.UpDownControl_PreviewKeyDown);
            // 
            // upButton
            // 
            this.upButton.BackColor = System.Drawing.SystemColors.Control;
            this.upButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.upButton.Font = new System.Drawing.Font("Webdings", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.upButton.Location = new System.Drawing.Point(0, 0);
            this.upButton.Name = "upButton";
            this.upButton.Size = new System.Drawing.Size(21, 12);
            this.upButton.TabIndex = 24;
            this.upButton.Text = "5";
            this.upButton.UseCompatibleTextRendering = true;
            this.upButton.UseVisualStyleBackColor = false;
            this.upButton.Click += new System.EventHandler(this.upButton_Click);
            this.upButton.KeyDown += new System.Windows.Forms.KeyEventHandler(this.UpDownControl_KeyDown);
            this.upButton.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.UpDownControl_PreviewKeyDown);
            // 
            // UpDownControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.downButton);
            this.Controls.Add(this.upButton);
            this.MaximumSize = new System.Drawing.Size(21, 100);
            this.MinimumSize = new System.Drawing.Size(21, 21);
            this.Name = "UpDownControl";
            this.Size = new System.Drawing.Size(21, 21);
            this.SizeChanged += new System.EventHandler(this.UpDownControl_SizeChanged);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.UpDownControl_KeyDown);
            this.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.UpDownControl_PreviewKeyDown);
            this.ResumeLayout(false);

        }

        #endregion

        private RepeatingButton upButton;
        private RepeatingButton downButton;
    }
}

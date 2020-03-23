namespace EditorUITest.Util
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
            this.upButton = new EditorUITest.RepeatingButton();
            this.downButton = new EditorUITest.RepeatingButton();
            this.SuspendLayout();
            // 
            // upButton
            // 
            this.upButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.upButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.upButton.Font = new System.Drawing.Font("Webdings", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.upButton.Location = new System.Drawing.Point(0, 0);
            this.upButton.Name = "upButton";
            this.upButton.Size = new System.Drawing.Size(20, 11);
            this.upButton.TabIndex = 24;
            this.upButton.Text = "5";
            this.upButton.UseCompatibleTextRendering = true;
            this.upButton.UseVisualStyleBackColor = true;
            this.upButton.Click += new System.EventHandler(this.upButton_Click);
            // 
            // downButton
            // 
            this.downButton.AccessibleRole = System.Windows.Forms.AccessibleRole.None;
            this.downButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.downButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.downButton.Font = new System.Drawing.Font("Webdings", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(2)));
            this.downButton.Location = new System.Drawing.Point(0, 10);
            this.downButton.Name = "downButton";
            this.downButton.Size = new System.Drawing.Size(20, 11);
            this.downButton.TabIndex = 25;
            this.downButton.Text = "6";
            this.downButton.UseCompatibleTextRendering = true;
            this.downButton.UseVisualStyleBackColor = true;
            this.downButton.Click += new System.EventHandler(this.downButton_Click);
            // 
            // UpDownControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.upButton);
            this.Controls.Add(this.downButton);
            this.MaximumSize = new System.Drawing.Size(20, 20);
            this.MinimumSize = new System.Drawing.Size(20, 20);
            this.Name = "UpDownControl";
            this.Size = new System.Drawing.Size(20, 20);
            this.ResumeLayout(false);

        }

        #endregion

        private EditorUITest.RepeatingButton upButton;
        private EditorUITest.RepeatingButton downButton;
    }
}

namespace DLEDotNet.Dialogs
{
    partial class ConfirmSaveDialog
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.idnoButton = new System.Windows.Forms.Button();
            this.idokButton = new System.Windows.Forms.Button();
            this.idcancelButton = new System.Windows.Forms.Button();
            this.labelTheMineHasBeenModified = new System.Windows.Forms.Label();
            this.idiOldIconPictureBox = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.idiOldIconPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // idnoButton
            // 
            this.idnoButton.DialogResult = System.Windows.Forms.DialogResult.Yes;
            this.idnoButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idnoButton.Location = new System.Drawing.Point(12, 65);
            this.idnoButton.Margin = new System.Windows.Forms.Padding(0);
            this.idnoButton.Name = "idnoButton";
            this.idnoButton.Size = new System.Drawing.Size(92, 28);
            this.idnoButton.TabIndex = 0;
            this.idnoButton.Text = "&save changes";
            // 
            // idokButton
            // 
            this.idokButton.DialogResult = System.Windows.Forms.DialogResult.No;
            this.idokButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idokButton.Location = new System.Drawing.Point(108, 65);
            this.idokButton.Margin = new System.Windows.Forms.Padding(0);
            this.idokButton.Name = "idokButton";
            this.idokButton.Size = new System.Drawing.Size(92, 28);
            this.idokButton.TabIndex = 1;
            this.idokButton.TabStop = false;
            this.idokButton.Text = "&discard changes";
            // 
            // idcancelButton
            // 
            this.idcancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.idcancelButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idcancelButton.Location = new System.Drawing.Point(204, 65);
            this.idcancelButton.Margin = new System.Windows.Forms.Padding(0);
            this.idcancelButton.Name = "idcancelButton";
            this.idcancelButton.Size = new System.Drawing.Size(92, 28);
            this.idcancelButton.TabIndex = 2;
            this.idcancelButton.TabStop = false;
            this.idcancelButton.Text = "&keep mine";
            // 
            // labelTheMineHasBeenModified
            // 
            this.labelTheMineHasBeenModified.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelTheMineHasBeenModified.Location = new System.Drawing.Point(84, 26);
            this.labelTheMineHasBeenModified.Margin = new System.Windows.Forms.Padding(0);
            this.labelTheMineHasBeenModified.Name = "labelTheMineHasBeenModified";
            this.labelTheMineHasBeenModified.Size = new System.Drawing.Size(140, 15);
            this.labelTheMineHasBeenModified.TabIndex = 3;
            this.labelTheMineHasBeenModified.TabStop = true;
            this.labelTheMineHasBeenModified.Text = "The mine has been modified.";
            // 
            // idiOldIconPictureBox
            // 
            this.idiOldIconPictureBox.Location = new System.Drawing.Point(26, 14);
            this.idiOldIconPictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.idiOldIconPictureBox.Name = "idiOldIconPictureBox";
            this.idiOldIconPictureBox.Size = new System.Drawing.Size(32, 32);
            this.idiOldIconPictureBox.TabIndex = 4;
            this.idiOldIconPictureBox.TabStop = false;
            // 
            // ConfirmSaveDialog
            // 
            this.AcceptButton = this.idnoButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(306, 106);
            this.Controls.Add(this.idnoButton);
            this.Controls.Add(this.idokButton);
            this.Controls.Add(this.idcancelButton);
            this.Controls.Add(this.labelTheMineHasBeenModified);
            this.Controls.Add(this.idiOldIconPictureBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConfirmSaveDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Load Mine";
            ((System.ComponentModel.ISupportInitialize)(this.idiOldIconPictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button idnoButton;
        private System.Windows.Forms.Button idokButton;
        private System.Windows.Forms.Button idcancelButton;
        private System.Windows.Forms.Label labelTheMineHasBeenModified;
        private System.Windows.Forms.PictureBox idiOldIconPictureBox;
    }
}
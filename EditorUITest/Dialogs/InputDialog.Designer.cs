namespace EditorUITest.Dialogs
{
    partial class InputDialog
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
            this.inpdlgBufTextBox = new System.Windows.Forms.TextBox();
            this.idokButton = new System.Windows.Forms.Button();
            this.idcancelButton = new System.Windows.Forms.Button();
            this.inpdlgPromptLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // inpdlgBufTextBox
            // 
            this.inpdlgBufTextBox.Location = new System.Drawing.Point(102, 16);
            this.inpdlgBufTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.inpdlgBufTextBox.Name = "inpdlgBufTextBox";
            this.inpdlgBufTextBox.Size = new System.Drawing.Size(198, 19);
            this.inpdlgBufTextBox.TabIndex = 0;
            this.inpdlgBufTextBox.TabStop = false;
            this.inpdlgBufTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // idokButton
            // 
            this.idokButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.idokButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idokButton.Location = new System.Drawing.Point(75, 49);
            this.idokButton.Margin = new System.Windows.Forms.Padding(0);
            this.idokButton.Name = "idokButton";
            this.idokButton.Size = new System.Drawing.Size(76, 24);
            this.idokButton.TabIndex = 1;
            this.idokButton.Text = "OK";
            // 
            // idcancelButton
            // 
            this.idcancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.idcancelButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idcancelButton.Location = new System.Drawing.Point(153, 49);
            this.idcancelButton.Margin = new System.Windows.Forms.Padding(0);
            this.idcancelButton.Name = "idcancelButton";
            this.idcancelButton.Size = new System.Drawing.Size(76, 24);
            this.idcancelButton.TabIndex = 2;
            this.idcancelButton.Text = "Cancel";
            // 
            // inpdlgPromptLabel
            // 
            this.inpdlgPromptLabel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.inpdlgPromptLabel.Location = new System.Drawing.Point(3, 20);
            this.inpdlgPromptLabel.Margin = new System.Windows.Forms.Padding(0);
            this.inpdlgPromptLabel.Name = "inpdlgPromptLabel";
            this.inpdlgPromptLabel.Size = new System.Drawing.Size(98, 15);
            this.inpdlgPromptLabel.TabIndex = 3;
            this.inpdlgPromptLabel.TabStop = true;
            this.inpdlgPromptLabel.Text = "(prompt)";
            // 
            // InputDialog
            // 
            this.AcceptButton = this.idokButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(309, 86);
            this.Controls.Add(this.inpdlgBufTextBox);
            this.Controls.Add(this.idokButton);
            this.Controls.Add(this.idcancelButton);
            this.Controls.Add(this.inpdlgPromptLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "InputDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Input Dialog";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox inpdlgBufTextBox;
        private System.Windows.Forms.Button idokButton;
        private System.Windows.Forms.Button idcancelButton;
        private System.Windows.Forms.Label inpdlgPromptLabel;
    }
}
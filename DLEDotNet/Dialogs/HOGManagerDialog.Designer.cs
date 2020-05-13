namespace DLEDotNet.Dialogs
{
    partial class HOGManagerDialog
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
            this.hogFilesListBox = new System.Windows.Forms.ListBox();
            this.idokButton = new System.Windows.Forms.Button();
            this.hogRenameButton = new System.Windows.Forms.Button();
            this.hogImportButton = new System.Windows.Forms.Button();
            this.hogExportButton = new System.Windows.Forms.Button();
            this.idcancelButton = new System.Windows.Forms.Button();
            this.hogDeleteButton = new System.Windows.Forms.Button();
            this.hogFilterCheckBox = new System.Windows.Forms.CheckBox();
            this.hogSizeTextBox = new System.Windows.Forms.TextBox();
            this.hogOffsetTextBox = new System.Windows.Forms.TextBox();
            this.labelFiles = new System.Windows.Forms.Label();
            this.labelSize = new System.Windows.Forms.Label();
            this.labelOffset = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // hogFilesListBox
            // 
            this.hogFilesListBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.hogFilesListBox.Location = new System.Drawing.Point(3, 23);
            this.hogFilesListBox.Margin = new System.Windows.Forms.Padding(0);
            this.hogFilesListBox.Name = "hogFilesListBox";
            this.hogFilesListBox.Size = new System.Drawing.Size(334, 225);
            this.hogFilesListBox.Sorted = true;
            this.hogFilesListBox.TabIndex = 0;
            // 
            // idokButton
            // 
            this.idokButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.idokButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.idokButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idokButton.Location = new System.Drawing.Point(350, 23);
            this.idokButton.Margin = new System.Windows.Forms.Padding(0);
            this.idokButton.Name = "idokButton";
            this.idokButton.Size = new System.Drawing.Size(76, 21);
            this.idokButton.TabIndex = 1;
            this.idokButton.Text = "Open";
            // 
            // hogRenameButton
            // 
            this.hogRenameButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogRenameButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.hogRenameButton.Location = new System.Drawing.Point(350, 42);
            this.hogRenameButton.Margin = new System.Windows.Forms.Padding(0);
            this.hogRenameButton.Name = "hogRenameButton";
            this.hogRenameButton.Size = new System.Drawing.Size(76, 21);
            this.hogRenameButton.TabIndex = 2;
            this.hogRenameButton.Text = "Rename";
            // 
            // hogImportButton
            // 
            this.hogImportButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogImportButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.hogImportButton.Location = new System.Drawing.Point(350, 63);
            this.hogImportButton.Margin = new System.Windows.Forms.Padding(0);
            this.hogImportButton.Name = "hogImportButton";
            this.hogImportButton.Size = new System.Drawing.Size(76, 21);
            this.hogImportButton.TabIndex = 3;
            this.hogImportButton.Text = "Import";
            // 
            // hogExportButton
            // 
            this.hogExportButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogExportButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.hogExportButton.Location = new System.Drawing.Point(350, 84);
            this.hogExportButton.Margin = new System.Windows.Forms.Padding(0);
            this.hogExportButton.Name = "hogExportButton";
            this.hogExportButton.Size = new System.Drawing.Size(76, 21);
            this.hogExportButton.TabIndex = 4;
            this.hogExportButton.Text = "Export";
            // 
            // idcancelButton
            // 
            this.idcancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.idcancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.idcancelButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idcancelButton.Location = new System.Drawing.Point(350, 106);
            this.idcancelButton.Margin = new System.Windows.Forms.Padding(0);
            this.idcancelButton.Name = "idcancelButton";
            this.idcancelButton.Size = new System.Drawing.Size(76, 21);
            this.idcancelButton.TabIndex = 5;
            this.idcancelButton.Text = "Cancel";
            // 
            // hogDeleteButton
            // 
            this.hogDeleteButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogDeleteButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.hogDeleteButton.Location = new System.Drawing.Point(350, 133);
            this.hogDeleteButton.Margin = new System.Windows.Forms.Padding(0);
            this.hogDeleteButton.Name = "hogDeleteButton";
            this.hogDeleteButton.Size = new System.Drawing.Size(76, 21);
            this.hogDeleteButton.TabIndex = 6;
            this.hogDeleteButton.Text = "Delete";
            // 
            // hogFilterCheckBox
            // 
            this.hogFilterCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogFilterCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.hogFilterCheckBox.Location = new System.Drawing.Point(350, 162);
            this.hogFilterCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.hogFilterCheckBox.Name = "hogFilterCheckBox";
            this.hogFilterCheckBox.Size = new System.Drawing.Size(78, 18);
            this.hogFilterCheckBox.TabIndex = 7;
            this.hogFilterCheckBox.Text = "show all files";
            // 
            // hogSizeTextBox
            // 
            this.hogSizeTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogSizeTextBox.Location = new System.Drawing.Point(350, 198);
            this.hogSizeTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.hogSizeTextBox.Name = "hogSizeTextBox";
            this.hogSizeTextBox.Size = new System.Drawing.Size(74, 20);
            this.hogSizeTextBox.TabIndex = 8;
            this.hogSizeTextBox.TabStop = false;
            this.hogSizeTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // hogOffsetTextBox
            // 
            this.hogOffsetTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.hogOffsetTextBox.Location = new System.Drawing.Point(350, 232);
            this.hogOffsetTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.hogOffsetTextBox.Name = "hogOffsetTextBox";
            this.hogOffsetTextBox.Size = new System.Drawing.Size(74, 20);
            this.hogOffsetTextBox.TabIndex = 9;
            this.hogOffsetTextBox.TabStop = false;
            this.hogOffsetTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // labelFiles
            // 
            this.labelFiles.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelFiles.Location = new System.Drawing.Point(6, 6);
            this.labelFiles.Margin = new System.Windows.Forms.Padding(0);
            this.labelFiles.Name = "labelFiles";
            this.labelFiles.Size = new System.Drawing.Size(333, 15);
            this.labelFiles.TabIndex = 10;
            this.labelFiles.TabStop = true;
            this.labelFiles.Text = "Files";
            // 
            // labelSize
            // 
            this.labelSize.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.labelSize.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelSize.Location = new System.Drawing.Point(350, 185);
            this.labelSize.Margin = new System.Windows.Forms.Padding(0);
            this.labelSize.Name = "labelSize";
            this.labelSize.Size = new System.Drawing.Size(76, 15);
            this.labelSize.TabIndex = 11;
            this.labelSize.TabStop = true;
            this.labelSize.Text = "Size";
            // 
            // labelOffset
            // 
            this.labelOffset.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.labelOffset.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelOffset.Location = new System.Drawing.Point(350, 219);
            this.labelOffset.Margin = new System.Windows.Forms.Padding(0);
            this.labelOffset.Name = "labelOffset";
            this.labelOffset.Size = new System.Drawing.Size(76, 15);
            this.labelOffset.TabIndex = 12;
            this.labelOffset.TabStop = true;
            this.labelOffset.Text = "Offset";
            // 
            // HOGManagerDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(430, 255);
            this.Controls.Add(this.hogFilesListBox);
            this.Controls.Add(this.idokButton);
            this.Controls.Add(this.hogRenameButton);
            this.Controls.Add(this.hogImportButton);
            this.Controls.Add(this.hogExportButton);
            this.Controls.Add(this.idcancelButton);
            this.Controls.Add(this.hogDeleteButton);
            this.Controls.Add(this.hogFilterCheckBox);
            this.Controls.Add(this.hogSizeTextBox);
            this.Controls.Add(this.hogOffsetTextBox);
            this.Controls.Add(this.labelFiles);
            this.Controls.Add(this.labelSize);
            this.Controls.Add(this.labelOffset);
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(226, 294);
            this.Name = "HOGManagerDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "HOG Manager";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ListBox hogFilesListBox;
        private System.Windows.Forms.Button idokButton;
        private System.Windows.Forms.Button hogRenameButton;
        private System.Windows.Forms.Button hogImportButton;
        private System.Windows.Forms.Button hogExportButton;
        private System.Windows.Forms.Button idcancelButton;
        private System.Windows.Forms.Button hogDeleteButton;
        private System.Windows.Forms.CheckBox hogFilterCheckBox;
        private System.Windows.Forms.TextBox hogSizeTextBox;
        private System.Windows.Forms.TextBox hogOffsetTextBox;
        private System.Windows.Forms.Label labelFiles;
        private System.Windows.Forms.Label labelSize;
        private System.Windows.Forms.Label labelOffset;
    }
}
using DLEDotNet.Editor;

namespace DLEDotNet.Dialogs
{
    partial class ConvertLevelDialog
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
            this.convertD1ComboBox = new System.Windows.Forms.ComboBox();
            this.convertD2ComboBox = new System.Windows.Forms.ComboBox();
            this.idokButton = new System.Windows.Forms.Button();
            this.idcancelButton = new System.Windows.Forms.Button();
            this.convertShowd1PictureBox = new System.Windows.Forms.PictureBox();
            this.convertShowd2PictureBox = new System.Windows.Forms.PictureBox();
            this.alignableGroupBoxConvertTextures = new AlignableGroupBox();
            this.labelCurrent = new System.Windows.Forms.Label();
            this.labelNew = new System.Windows.Forms.Label();
            this.labelHyph = new System.Windows.Forms.Label();
            this.alignableGroupBoxConvertTextures.SuspendLayout();
            this.SuspendLayout();
            // 
            // convertD1ComboBox
            // 
            this.convertD1ComboBox.Location = new System.Drawing.Point(9, 104);
            this.convertD1ComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.convertD1ComboBox.Name = "convertD1ComboBox";
            this.convertD1ComboBox.Size = new System.Drawing.Size(90, 21);
            this.convertD1ComboBox.Sorted = true;
            this.convertD1ComboBox.TabIndex = 0;
            // 
            // convertD2ComboBox
            // 
            this.convertD2ComboBox.Location = new System.Drawing.Point(102, 104);
            this.convertD2ComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.convertD2ComboBox.Name = "convertD2ComboBox";
            this.convertD2ComboBox.Size = new System.Drawing.Size(90, 21);
            this.convertD2ComboBox.Sorted = true;
            this.convertD2ComboBox.TabIndex = 1;
            // 
            // idokButton
            // 
            this.idokButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.idokButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idokButton.Location = new System.Drawing.Point(21, 140);
            this.idokButton.Margin = new System.Windows.Forms.Padding(0);
            this.idokButton.Name = "idokButton";
            this.idokButton.Size = new System.Drawing.Size(76, 23);
            this.idokButton.TabIndex = 0;
            this.idokButton.Text = "Convert";
            // 
            // idcancelButton
            // 
            this.idcancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.idcancelButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idcancelButton.Location = new System.Drawing.Point(114, 140);
            this.idcancelButton.Margin = new System.Windows.Forms.Padding(0);
            this.idcancelButton.Name = "idcancelButton";
            this.idcancelButton.Size = new System.Drawing.Size(76, 21);
            this.idcancelButton.TabIndex = 1;
            this.idcancelButton.Text = "Cancel";
            // 
            // convertShowd1TextBox
            // 
            this.convertShowd1PictureBox.BackColor = System.Drawing.Color.Black;
            this.convertShowd1PictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.convertShowd1PictureBox.Location = new System.Drawing.Point(21, 29);
            this.convertShowd1PictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.convertShowd1PictureBox.Name = "convertShowd1PictureBox";
            this.convertShowd1PictureBox.Size = new System.Drawing.Size(67, 66);
            this.convertShowd1PictureBox.TabIndex = 2;
            this.convertShowd1PictureBox.TabStop = false;
            // 
            // convertShowd2PictureBox
            // 
            this.convertShowd2PictureBox.BackColor = System.Drawing.Color.Black;
            this.convertShowd2PictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.convertShowd2PictureBox.Location = new System.Drawing.Point(111, 29);
            this.convertShowd2PictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.convertShowd2PictureBox.Name = "convertShowd2PictureBox";
            this.convertShowd2PictureBox.Size = new System.Drawing.Size(67, 66);
            this.convertShowd2PictureBox.TabIndex = 3;
            this.convertShowd2PictureBox.TabStop = false;
            // 
            // alignableGroupBoxConvertTextures
            // 
            this.alignableGroupBoxConvertTextures.Alignment = System.Windows.Forms.HorizontalAlignment.Center;
            this.alignableGroupBoxConvertTextures.Controls.Add(this.convertD1ComboBox);
            this.alignableGroupBoxConvertTextures.Controls.Add(this.convertD2ComboBox);
            this.alignableGroupBoxConvertTextures.Controls.Add(this.convertShowd1PictureBox);
            this.alignableGroupBoxConvertTextures.Controls.Add(this.convertShowd2PictureBox);
            this.alignableGroupBoxConvertTextures.Controls.Add(this.labelCurrent);
            this.alignableGroupBoxConvertTextures.Controls.Add(this.labelNew);
            this.alignableGroupBoxConvertTextures.Controls.Add(this.labelHyph);
            this.alignableGroupBoxConvertTextures.Location = new System.Drawing.Point(6, 0);
            this.alignableGroupBoxConvertTextures.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxConvertTextures.Name = "alignableGroupBoxConvertTextures";
            this.alignableGroupBoxConvertTextures.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxConvertTextures.Size = new System.Drawing.Size(202, 135);
            this.alignableGroupBoxConvertTextures.TabIndex = 2;
            this.alignableGroupBoxConvertTextures.TabStop = false;
            this.alignableGroupBoxConvertTextures.Text = "convert textures";
            // 
            // labelCurrent
            // 
            this.labelCurrent.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelCurrent.Location = new System.Drawing.Point(24, 16);
            this.labelCurrent.Margin = new System.Windows.Forms.Padding(0);
            this.labelCurrent.Name = "labelCurrent";
            this.labelCurrent.Size = new System.Drawing.Size(66, 15);
            this.labelCurrent.TabIndex = 4;
            this.labelCurrent.TabStop = true;
            this.labelCurrent.Text = "current";
            // 
            // labelNew
            // 
            this.labelNew.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelNew.Location = new System.Drawing.Point(114, 16);
            this.labelNew.Margin = new System.Windows.Forms.Padding(0);
            this.labelNew.Name = "labelNew";
            this.labelNew.Size = new System.Drawing.Size(66, 15);
            this.labelNew.TabIndex = 5;
            this.labelNew.TabStop = true;
            this.labelNew.Text = "new";
            // 
            // labelHyph
            // 
            this.labelHyph.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelHyph.Location = new System.Drawing.Point(92, 55);
            this.labelHyph.Margin = new System.Windows.Forms.Padding(0);
            this.labelHyph.Name = "labelHyph";
            this.labelHyph.Size = new System.Drawing.Size(20, 15);
            this.labelHyph.TabIndex = 6;
            this.labelHyph.TabStop = true;
            this.labelHyph.Text = "->";
            // 
            // ConvertLevelDialog
            // 
            this.AcceptButton = this.idokButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(213, 164);
            this.Controls.Add(this.idokButton);
            this.Controls.Add(this.idcancelButton);
            this.Controls.Add(this.alignableGroupBoxConvertTextures);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ConvertLevelDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Convert Level";
            this.alignableGroupBoxConvertTextures.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox convertD1ComboBox;
        private System.Windows.Forms.ComboBox convertD2ComboBox;
        private System.Windows.Forms.Button idokButton;
        private System.Windows.Forms.Button idcancelButton;
        private System.Windows.Forms.PictureBox convertShowd1PictureBox;
        private System.Windows.Forms.PictureBox convertShowd2PictureBox;
        private AlignableGroupBox alignableGroupBoxConvertTextures;
        private System.Windows.Forms.Label labelCurrent;
        private System.Windows.Forms.Label labelNew;
        private System.Windows.Forms.Label labelHyph;

    }
}
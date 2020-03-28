namespace DLEDotNet.Editor.Layouts
{
    partial class SettingsTool
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
            this.toolTabTabControl = new System.Windows.Forms.TabControl();
            this.tabPageFiles = new System.Windows.Forms.TabPage();
            this.preferencesFiles1 = new DLEDotNet.Editor.Layouts.PreferencesFiles();
            this.tabPageLayout = new System.Windows.Forms.TabPage();
            this.preferencesLayout1 = new DLEDotNet.Editor.Layouts.PreferencesLayout();
            this.tabPageRenderer = new System.Windows.Forms.TabPage();
            this.preferencesRenderer1 = new DLEDotNet.Editor.Layouts.PreferencesRenderer();
            this.tabPageEditor = new System.Windows.Forms.TabPage();
            this.preferencesEditor1 = new DLEDotNet.Editor.Layouts.PreferencesEditor();
            this.tabPageVisibility = new System.Windows.Forms.TabPage();
            this.preferencesVisibility1 = new DLEDotNet.Editor.Layouts.PreferencesVisibility();
            this.tabPageMisc = new System.Windows.Forms.TabPage();
            this.preferencesMisc1 = new DLEDotNet.Editor.Layouts.PreferencesMisc();
            this.idokButton = new System.Windows.Forms.Button();
            this.idcancelButton = new System.Windows.Forms.Button();
            this.toolTabTabControl.SuspendLayout();
            this.tabPageFiles.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.preferencesFiles1)).BeginInit();
            this.tabPageLayout.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.preferencesLayout1)).BeginInit();
            this.tabPageRenderer.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.preferencesRenderer1)).BeginInit();
            this.tabPageEditor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.preferencesEditor1)).BeginInit();
            this.tabPageVisibility.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.preferencesVisibility1)).BeginInit();
            this.tabPageMisc.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.preferencesMisc1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // toolTabTabControl
            // 
            this.toolTabTabControl.Controls.Add(this.tabPageFiles);
            this.toolTabTabControl.Controls.Add(this.tabPageLayout);
            this.toolTabTabControl.Controls.Add(this.tabPageRenderer);
            this.toolTabTabControl.Controls.Add(this.tabPageEditor);
            this.toolTabTabControl.Controls.Add(this.tabPageVisibility);
            this.toolTabTabControl.Controls.Add(this.tabPageMisc);
            this.toolTabTabControl.Location = new System.Drawing.Point(4, 3);
            this.toolTabTabControl.Margin = new System.Windows.Forms.Padding(0);
            this.toolTabTabControl.Multiline = true;
            this.toolTabTabControl.Name = "toolTabTabControl";
            this.toolTabTabControl.SelectedIndex = 0;
            this.toolTabTabControl.Size = new System.Drawing.Size(780, 208);
            this.toolTabTabControl.TabIndex = 0;
            this.toolTabTabControl.TabStop = false;
            // 
            // tabPageFiles
            // 
            this.tabPageFiles.AutoScroll = true;
            this.tabPageFiles.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageFiles.Controls.Add(this.preferencesFiles1);
            this.tabPageFiles.Location = new System.Drawing.Point(4, 22);
            this.tabPageFiles.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageFiles.Name = "tabPageFiles";
            this.tabPageFiles.Size = new System.Drawing.Size(772, 182);
            this.tabPageFiles.TabIndex = 0;
            this.tabPageFiles.Text = "Files";
            // 
            // preferencesFiles1
            // 
            this.preferencesFiles1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.preferencesFiles1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.preferencesFiles1.Location = new System.Drawing.Point(0, 0);
            this.preferencesFiles1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.preferencesFiles1.Name = "preferencesFiles1";
            this.preferencesFiles1.Size = new System.Drawing.Size(772, 182);
            this.preferencesFiles1.TabIndex = 0;
            // 
            // tabPageLayout
            // 
            this.tabPageLayout.AutoScroll = true;
            this.tabPageLayout.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageLayout.Controls.Add(this.preferencesLayout1);
            this.tabPageLayout.Location = new System.Drawing.Point(4, 22);
            this.tabPageLayout.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageLayout.Name = "tabPageLayout";
            this.tabPageLayout.Size = new System.Drawing.Size(772, 182);
            this.tabPageLayout.TabIndex = 1;
            this.tabPageLayout.Text = "Layout";
            // 
            // preferencesLayout1
            // 
            this.preferencesLayout1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.preferencesLayout1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.preferencesLayout1.Location = new System.Drawing.Point(0, 0);
            this.preferencesLayout1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.preferencesLayout1.Name = "preferencesLayout1";
            this.preferencesLayout1.Size = new System.Drawing.Size(772, 182);
            this.preferencesLayout1.TabIndex = 0;
            // 
            // tabPageRenderer
            // 
            this.tabPageRenderer.AutoScroll = true;
            this.tabPageRenderer.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageRenderer.Controls.Add(this.preferencesRenderer1);
            this.tabPageRenderer.Location = new System.Drawing.Point(4, 22);
            this.tabPageRenderer.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageRenderer.Name = "tabPageRenderer";
            this.tabPageRenderer.Size = new System.Drawing.Size(772, 182);
            this.tabPageRenderer.TabIndex = 2;
            this.tabPageRenderer.Text = "Renderer";
            // 
            // preferencesRenderer1
            // 
            this.preferencesRenderer1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.preferencesRenderer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.preferencesRenderer1.Location = new System.Drawing.Point(0, 0);
            this.preferencesRenderer1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.preferencesRenderer1.Name = "preferencesRenderer1";
            this.preferencesRenderer1.Size = new System.Drawing.Size(772, 182);
            this.preferencesRenderer1.TabIndex = 0;
            // 
            // tabPageEditor
            // 
            this.tabPageEditor.AutoScroll = true;
            this.tabPageEditor.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageEditor.Controls.Add(this.preferencesEditor1);
            this.tabPageEditor.Location = new System.Drawing.Point(4, 22);
            this.tabPageEditor.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageEditor.Name = "tabPageEditor";
            this.tabPageEditor.Size = new System.Drawing.Size(772, 182);
            this.tabPageEditor.TabIndex = 3;
            this.tabPageEditor.Text = "Editor";
            // 
            // preferencesEditor1
            // 
            this.preferencesEditor1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.preferencesEditor1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.preferencesEditor1.Location = new System.Drawing.Point(0, 0);
            this.preferencesEditor1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.preferencesEditor1.Name = "preferencesEditor1";
            this.preferencesEditor1.Size = new System.Drawing.Size(772, 182);
            this.preferencesEditor1.TabIndex = 0;
            // 
            // tabPageVisibility
            // 
            this.tabPageVisibility.AutoScroll = true;
            this.tabPageVisibility.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageVisibility.Controls.Add(this.preferencesVisibility1);
            this.tabPageVisibility.Location = new System.Drawing.Point(4, 22);
            this.tabPageVisibility.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageVisibility.Name = "tabPageVisibility";
            this.tabPageVisibility.Size = new System.Drawing.Size(772, 182);
            this.tabPageVisibility.TabIndex = 4;
            this.tabPageVisibility.Text = "Visibility";
            // 
            // preferencesVisibility1
            // 
            this.preferencesVisibility1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.preferencesVisibility1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.preferencesVisibility1.Location = new System.Drawing.Point(0, 0);
            this.preferencesVisibility1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.preferencesVisibility1.Name = "preferencesVisibility1";
            this.preferencesVisibility1.Size = new System.Drawing.Size(772, 182);
            this.preferencesVisibility1.TabIndex = 0;
            // 
            // tabPageMisc
            // 
            this.tabPageMisc.AutoScroll = true;
            this.tabPageMisc.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageMisc.Controls.Add(this.preferencesMisc1);
            this.tabPageMisc.Location = new System.Drawing.Point(4, 22);
            this.tabPageMisc.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.tabPageMisc.Name = "tabPageMisc";
            this.tabPageMisc.Size = new System.Drawing.Size(772, 182);
            this.tabPageMisc.TabIndex = 5;
            this.tabPageMisc.Text = "Misc";
            // 
            // preferencesMisc1
            // 
            this.preferencesMisc1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.preferencesMisc1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.preferencesMisc1.Location = new System.Drawing.Point(0, 0);
            this.preferencesMisc1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.preferencesMisc1.Name = "preferencesMisc1";
            this.preferencesMisc1.Size = new System.Drawing.Size(772, 182);
            this.preferencesMisc1.TabIndex = 0;
            // 
            // idokButton
            // 
            this.idokButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idokButton.Location = new System.Drawing.Point(825, 159);
            this.idokButton.Margin = new System.Windows.Forms.Padding(0);
            this.idokButton.Name = "idokButton";
            this.idokButton.Size = new System.Drawing.Size(76, 20);
            this.idokButton.TabIndex = 1;
            this.idokButton.TabStop = false;
            this.idokButton.Text = "save";
            this.idokButton.Click += new System.EventHandler(this.idokButton_Click);
            // 
            // idcancelButton
            // 
            this.idcancelButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.idcancelButton.Location = new System.Drawing.Point(825, 179);
            this.idcancelButton.Margin = new System.Windows.Forms.Padding(0);
            this.idcancelButton.Name = "idcancelButton";
            this.idcancelButton.Size = new System.Drawing.Size(76, 20);
            this.idcancelButton.TabIndex = 2;
            this.idcancelButton.Text = "revert";
            this.idcancelButton.Click += new System.EventHandler(this.idcancelButton_Click);
            // 
            // SettingsTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.toolTabTabControl);
            this.Controls.Add(this.idokButton);
            this.Controls.Add(this.idcancelButton);
            this.FloatingTitle = "Settings";
            this.Margin = new System.Windows.Forms.Padding(6, 8, 6, 8);
            this.Name = "SettingsTool";
            this.Size = new System.Drawing.Size(930, 211);
            this.toolTabTabControl.ResumeLayout(false);
            this.tabPageFiles.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.preferencesFiles1)).EndInit();
            this.tabPageLayout.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.preferencesLayout1)).EndInit();
            this.tabPageRenderer.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.preferencesRenderer1)).EndInit();
            this.tabPageEditor.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.preferencesEditor1)).EndInit();
            this.tabPageVisibility.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.preferencesVisibility1)).EndInit();
            this.tabPageMisc.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.preferencesMisc1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl toolTabTabControl;
        private System.Windows.Forms.Button idokButton;
        private System.Windows.Forms.Button idcancelButton;
        private System.Windows.Forms.TabPage tabPageFiles;
        private System.Windows.Forms.TabPage tabPageLayout;
        private System.Windows.Forms.TabPage tabPageRenderer;
        private System.Windows.Forms.TabPage tabPageEditor;
        private System.Windows.Forms.TabPage tabPageVisibility;
        private System.Windows.Forms.TabPage tabPageMisc;
        private Editor.Layouts.PreferencesFiles preferencesFiles1;
        private Editor.Layouts.PreferencesLayout preferencesLayout1;
        private PreferencesRenderer preferencesRenderer1;
        private PreferencesEditor preferencesEditor1;
        private PreferencesVisibility preferencesVisibility1;
        private PreferencesMisc preferencesMisc1;
    }
}

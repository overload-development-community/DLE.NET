namespace DLEDotNet.Editor.Layouts
{
    partial class DiagnosticsTool
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
            this.diagBuglistListBox = new System.Windows.Forms.ListBox();
            this.diagCheckButton = new System.Windows.Forms.Button();
            this.diagShowButton = new System.Windows.Forms.Button();
            this.diagClearButton = new System.Windows.Forms.Button();
            this.diagFixbugsCheckBox = new System.Windows.Forms.CheckBox();
            this.diagWarningsCheckBox = new System.Windows.Forms.CheckBox();
            this.alignableGroupBoxProblemReport = new DLEDotNet.Editor.AlignableGroupBox();
            this.alignableGroupBoxStatistics = new DLEDotNet.Editor.AlignableGroupBox();
            this.diagStatsListView = new System.Windows.Forms.ListView();
            this.columnHeaderItem = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderCount = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderMax = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.helpToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.alignableGroupBoxProblemReport.SuspendLayout();
            this.alignableGroupBoxStatistics.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // diagBuglistListBox
            // 
            this.diagBuglistListBox.Location = new System.Drawing.Point(6, 15);
            this.diagBuglistListBox.Margin = new System.Windows.Forms.Padding(0);
            this.diagBuglistListBox.Name = "diagBuglistListBox";
            this.diagBuglistListBox.Size = new System.Drawing.Size(584, 186);
            this.diagBuglistListBox.TabIndex = 0;
            // 
            // diagCheckButton
            // 
            this.diagCheckButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.diagCheckButton.Location = new System.Drawing.Point(834, 13);
            this.diagCheckButton.Margin = new System.Windows.Forms.Padding(0);
            this.diagCheckButton.Name = "diagCheckButton";
            this.diagCheckButton.Size = new System.Drawing.Size(92, 21);
            this.diagCheckButton.TabIndex = 0;
            this.diagCheckButton.TabStop = false;
            this.diagCheckButton.Text = "&check mine";
            this.helpToolTip.SetToolTip(this.diagCheckButton, "check the mine for various issues");
            // 
            // diagShowButton
            // 
            this.diagShowButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.diagShowButton.Location = new System.Drawing.Point(834, 39);
            this.diagShowButton.Margin = new System.Windows.Forms.Padding(0);
            this.diagShowButton.Name = "diagShowButton";
            this.diagShowButton.Size = new System.Drawing.Size(92, 21);
            this.diagShowButton.TabIndex = 1;
            this.diagShowButton.TabStop = false;
            this.diagShowButton.Text = "&show problem";
            this.helpToolTip.SetToolTip(this.diagShowButton, "show the location of the currently selected problem");
            // 
            // diagClearButton
            // 
            this.diagClearButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.diagClearButton.Location = new System.Drawing.Point(834, 62);
            this.diagClearButton.Margin = new System.Windows.Forms.Padding(0);
            this.diagClearButton.Name = "diagClearButton";
            this.diagClearButton.Size = new System.Drawing.Size(92, 21);
            this.diagClearButton.TabIndex = 2;
            this.diagClearButton.TabStop = false;
            this.diagClearButton.Text = "c&lear list";
            this.helpToolTip.SetToolTip(this.diagClearButton, "clear the list of problems");
            // 
            // diagFixbugsCheckBox
            // 
            this.diagFixbugsCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.diagFixbugsCheckBox.Location = new System.Drawing.Point(838, 94);
            this.diagFixbugsCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.diagFixbugsCheckBox.Name = "diagFixbugsCheckBox";
            this.diagFixbugsCheckBox.Size = new System.Drawing.Size(82, 18);
            this.diagFixbugsCheckBox.TabIndex = 3;
            this.diagFixbugsCheckBox.Text = "auto fix bugs";
            this.helpToolTip.SetToolTip(this.diagFixbugsCheckBox, "whether errors or warnings that can be automatically fixed should be");
            // 
            // diagWarningsCheckBox
            // 
            this.diagWarningsCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.diagWarningsCheckBox.Location = new System.Drawing.Point(838, 112);
            this.diagWarningsCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.diagWarningsCheckBox.Name = "diagWarningsCheckBox";
            this.diagWarningsCheckBox.Size = new System.Drawing.Size(90, 18);
            this.diagWarningsCheckBox.TabIndex = 4;
            this.diagWarningsCheckBox.Text = "show warnings";
            this.helpToolTip.SetToolTip(this.diagWarningsCheckBox, "whether to show warnings in the problem report");
            // 
            // alignableGroupBoxProblemReport
            // 
            this.alignableGroupBoxProblemReport.Alignment = System.Windows.Forms.HorizontalAlignment.Center;
            this.alignableGroupBoxProblemReport.Controls.Add(this.diagBuglistListBox);
            this.alignableGroupBoxProblemReport.Location = new System.Drawing.Point(228, 0);
            this.alignableGroupBoxProblemReport.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxProblemReport.Name = "alignableGroupBoxProblemReport";
            this.alignableGroupBoxProblemReport.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxProblemReport.Size = new System.Drawing.Size(598, 210);
            this.alignableGroupBoxProblemReport.TabIndex = 5;
            this.alignableGroupBoxProblemReport.TabStop = false;
            this.alignableGroupBoxProblemReport.Text = "problem report";
            // 
            // alignableGroupBoxStatistics
            // 
            this.alignableGroupBoxStatistics.Alignment = System.Windows.Forms.HorizontalAlignment.Center;
            this.alignableGroupBoxStatistics.Controls.Add(this.diagStatsListView);
            this.alignableGroupBoxStatistics.Location = new System.Drawing.Point(6, 0);
            this.alignableGroupBoxStatistics.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxStatistics.Name = "alignableGroupBoxStatistics";
            this.alignableGroupBoxStatistics.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxStatistics.Size = new System.Drawing.Size(218, 210);
            this.alignableGroupBoxStatistics.TabIndex = 6;
            this.alignableGroupBoxStatistics.TabStop = false;
            this.alignableGroupBoxStatistics.Text = "statistics";
            // 
            // diagStatsListView
            // 
            this.diagStatsListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderItem,
            this.columnHeaderCount,
            this.columnHeaderMax});
            this.diagStatsListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.diagStatsListView.HideSelection = false;
            this.diagStatsListView.Location = new System.Drawing.Point(9, 16);
            this.diagStatsListView.Name = "diagStatsListView";
            this.diagStatsListView.Size = new System.Drawing.Size(198, 185);
            this.diagStatsListView.TabIndex = 1;
            this.diagStatsListView.UseCompatibleStateImageBehavior = false;
            this.diagStatsListView.View = System.Windows.Forms.View.Details;
            // 
            // columnHeaderItem
            // 
            this.columnHeaderItem.Text = "item";
            this.columnHeaderItem.Width = 75;
            // 
            // columnHeaderCount
            // 
            this.columnHeaderCount.Text = "count";
            this.columnHeaderCount.Width = 43;
            // 
            // columnHeaderMax
            // 
            this.columnHeaderMax.Text = "max #";
            this.columnHeaderMax.Width = 57;
            // 
            // DiagnosticsTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.diagCheckButton);
            this.Controls.Add(this.diagShowButton);
            this.Controls.Add(this.diagClearButton);
            this.Controls.Add(this.diagFixbugsCheckBox);
            this.Controls.Add(this.diagWarningsCheckBox);
            this.Controls.Add(this.alignableGroupBoxProblemReport);
            this.Controls.Add(this.alignableGroupBoxStatistics);
            this.FloatingTitle = "Diagnostics";
            this.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.Name = "DiagnosticsTool";
            this.Size = new System.Drawing.Size(930, 211);
            this.alignableGroupBoxProblemReport.ResumeLayout(false);
            this.alignableGroupBoxStatistics.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.ListBox diagBuglistListBox;
        private System.Windows.Forms.Button diagCheckButton;
        private System.Windows.Forms.Button diagShowButton;
        private System.Windows.Forms.Button diagClearButton;
        private System.Windows.Forms.CheckBox diagFixbugsCheckBox;
        private System.Windows.Forms.CheckBox diagWarningsCheckBox;
        private AlignableGroupBox alignableGroupBoxProblemReport;
        private AlignableGroupBox alignableGroupBoxStatistics;
        private System.Windows.Forms.ListView diagStatsListView;
        internal System.Windows.Forms.ColumnHeader columnHeaderItem;
        internal System.Windows.Forms.ColumnHeader columnHeaderCount;
        internal System.Windows.Forms.ColumnHeader columnHeaderMax;
        private System.Windows.Forms.ToolTip helpToolTip;
    }
}

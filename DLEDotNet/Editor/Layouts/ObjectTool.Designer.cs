﻿namespace DLEDotNet.Editor.Layouts
{
    partial class ObjectTool
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
            this.objObjnoComboBox = new System.Windows.Forms.ComboBox();
            this.objTypeComboBox = new System.Windows.Forms.ComboBox();
            this.objIdComboBox = new System.Windows.Forms.ComboBox();
            this.objAiComboBox = new System.Windows.Forms.ComboBox();
            this.objTextureComboBox = new System.Windows.Forms.ComboBox();
            this.objSortCheckBox = new System.Windows.Forms.CheckBox();
            this.objMultiplayerCheckBox = new System.Windows.Forms.CheckBox();
            this.objSpawnTypeComboBox = new System.Windows.Forms.ComboBox();
            this.objSpawnIdComboBox = new System.Windows.Forms.ComboBox();
            this.objAddButton = new System.Windows.Forms.Button();
            this.objDeleteButton = new System.Windows.Forms.Button();
            this.objDeleteallButton = new System.Windows.Forms.Button();
            this.objMoveButton = new System.Windows.Forms.Button();
            this.objResetButton = new System.Windows.Forms.Button();
            this.objDefaultButton = new System.Windows.Forms.Button();
            this.objShowTexturePictureBox = new System.Windows.Forms.PictureBox();
            this.labelObject = new System.Windows.Forms.Label();
            this.labelTypeA = new System.Windows.Forms.Label();
            this.labelIdA = new System.Windows.Forms.Label();
            this.labelTexture = new System.Windows.Forms.Label();
            this.labelTypeB = new System.Windows.Forms.Label();
            this.labelIdB = new System.Windows.Forms.Label();
            this.labelQty = new System.Windows.Forms.Label();
            this.alignableGroupBoxThisRobotSpawns = new DLEDotNet.Editor.AlignableGroupBox();
            this.objShowSpawnPictureBox = new System.Windows.Forms.PictureBox();
            this.objSpawnQtyNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.objShowPictureBox = new System.Windows.Forms.PictureBox();
            this.objInfoTextBox = new System.Windows.Forms.TextBox();
            this.labelNum = new System.Windows.Forms.Label();
            this.tObjCountLabel = new System.Windows.Forms.Label();
            this.labelSort = new System.Windows.Forms.Label();
            this.labelAi = new System.Windows.Forms.Label();
            this.somewhereElseLabel = new System.Windows.Forms.Label();
            this.objectPosition1 = new DLEDotNet.Editor.Layouts.ObjectPosition();
            this.objectTabControl = new System.Windows.Forms.TabControl();
            this.tabPagePosition = new System.Windows.Forms.TabPage();
            this.tabPageAdvanced = new System.Windows.Forms.TabPage();
            this.objectAdvanced1 = new DLEDotNet.Editor.Layouts.ObjectAdvanced();
            this.tabPageOther = new System.Windows.Forms.TabPage();
            this.helpToolTip = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.objShowTexturePictureBox)).BeginInit();
            this.alignableGroupBoxThisRobotSpawns.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.objShowSpawnPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.objSpawnQtyNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.objShowPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.objectPosition1)).BeginInit();
            this.objectTabControl.SuspendLayout();
            this.tabPagePosition.SuspendLayout();
            this.tabPageAdvanced.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.objectAdvanced1)).BeginInit();
            this.tabPageOther.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this)).BeginInit();
            this.SuspendLayout();
            // 
            // objObjnoComboBox
            // 
            this.objObjnoComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.objObjnoComboBox.Location = new System.Drawing.Point(36, 3);
            this.objObjnoComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objObjnoComboBox.Name = "objObjnoComboBox";
            this.objObjnoComboBox.Size = new System.Drawing.Size(110, 21);
            this.objObjnoComboBox.TabIndex = 0;
            // 
            // objTypeComboBox
            // 
            this.objTypeComboBox.Location = new System.Drawing.Point(36, 26);
            this.objTypeComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objTypeComboBox.Name = "objTypeComboBox";
            this.objTypeComboBox.Size = new System.Drawing.Size(110, 21);
            this.objTypeComboBox.Sorted = true;
            this.objTypeComboBox.TabIndex = 1;
            // 
            // objIdComboBox
            // 
            this.objIdComboBox.Location = new System.Drawing.Point(36, 49);
            this.objIdComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objIdComboBox.Name = "objIdComboBox";
            this.objIdComboBox.Size = new System.Drawing.Size(110, 21);
            this.objIdComboBox.Sorted = true;
            this.objIdComboBox.TabIndex = 2;
            this.helpToolTip.SetToolTip(this.objIdComboBox, "object-type specific ID");
            // 
            // objAiComboBox
            // 
            this.objAiComboBox.Location = new System.Drawing.Point(36, 72);
            this.objAiComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objAiComboBox.Name = "objAiComboBox";
            this.objAiComboBox.Size = new System.Drawing.Size(110, 21);
            this.objAiComboBox.TabIndex = 3;
            this.helpToolTip.SetToolTip(this.objAiComboBox, "current AI mode");
            // 
            // objTextureComboBox
            // 
            this.objTextureComboBox.Location = new System.Drawing.Point(36, 94);
            this.objTextureComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objTextureComboBox.Name = "objTextureComboBox";
            this.objTextureComboBox.Size = new System.Drawing.Size(110, 21);
            this.objTextureComboBox.TabIndex = 4;
            // 
            // objSortCheckBox
            // 
            this.objSortCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objSortCheckBox.Location = new System.Drawing.Point(204, 6);
            this.objSortCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.objSortCheckBox.Name = "objSortCheckBox";
            this.objSortCheckBox.Size = new System.Drawing.Size(15, 18);
            this.objSortCheckBox.TabIndex = 5;
            this.helpToolTip.SetToolTip(this.objSortCheckBox, "automatically re-sort objects by type");
            // 
            // objMultiplayerCheckBox
            // 
            this.objMultiplayerCheckBox.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objMultiplayerCheckBox.Location = new System.Drawing.Point(150, 29);
            this.objMultiplayerCheckBox.Margin = new System.Windows.Forms.Padding(0);
            this.objMultiplayerCheckBox.Name = "objMultiplayerCheckBox";
            this.objMultiplayerCheckBox.Size = new System.Drawing.Size(74, 16);
            this.objMultiplayerCheckBox.TabIndex = 6;
            this.objMultiplayerCheckBox.Text = "Multiplayer";
            this.helpToolTip.SetToolTip(this.objMultiplayerCheckBox, "make this object only appear in multiplayer modes");
            // 
            // objSpawnTypeComboBox
            // 
            this.objSpawnTypeComboBox.Location = new System.Drawing.Point(30, 16);
            this.objSpawnTypeComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objSpawnTypeComboBox.Name = "objSpawnTypeComboBox";
            this.objSpawnTypeComboBox.Size = new System.Drawing.Size(110, 21);
            this.objSpawnTypeComboBox.TabIndex = 0;
            // 
            // objSpawnIdComboBox
            // 
            this.objSpawnIdComboBox.Location = new System.Drawing.Point(30, 39);
            this.objSpawnIdComboBox.Margin = new System.Windows.Forms.Padding(0);
            this.objSpawnIdComboBox.Name = "objSpawnIdComboBox";
            this.objSpawnIdComboBox.Size = new System.Drawing.Size(110, 21);
            this.objSpawnIdComboBox.Sorted = true;
            this.objSpawnIdComboBox.TabIndex = 1;
            // 
            // objAddButton
            // 
            this.objAddButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objAddButton.Location = new System.Drawing.Point(225, 109);
            this.objAddButton.Margin = new System.Windows.Forms.Padding(0);
            this.objAddButton.Name = "objAddButton";
            this.objAddButton.Size = new System.Drawing.Size(68, 18);
            this.objAddButton.TabIndex = 7;
            this.objAddButton.Text = "Add";
            this.helpToolTip.SetToolTip(this.objAddButton, "add another copy of the currently selected object to the current segment");
            // 
            // objDeleteButton
            // 
            this.objDeleteButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objDeleteButton.Location = new System.Drawing.Point(225, 125);
            this.objDeleteButton.Margin = new System.Windows.Forms.Padding(0);
            this.objDeleteButton.Name = "objDeleteButton";
            this.objDeleteButton.Size = new System.Drawing.Size(68, 20);
            this.objDeleteButton.TabIndex = 8;
            this.objDeleteButton.Text = "Delete";
            this.helpToolTip.SetToolTip(this.objDeleteButton, "delete the currently selected object");
            // 
            // objDeleteallButton
            // 
            this.objDeleteallButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objDeleteallButton.Location = new System.Drawing.Point(225, 143);
            this.objDeleteallButton.Margin = new System.Windows.Forms.Padding(0);
            this.objDeleteallButton.Name = "objDeleteallButton";
            this.objDeleteallButton.Size = new System.Drawing.Size(68, 20);
            this.objDeleteallButton.TabIndex = 9;
            this.objDeleteallButton.Text = "Delete all";
            this.helpToolTip.SetToolTip(this.objDeleteallButton, "delete all objects of this type");
            // 
            // objMoveButton
            // 
            this.objMoveButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objMoveButton.Location = new System.Drawing.Point(225, 161);
            this.objMoveButton.Margin = new System.Windows.Forms.Padding(0);
            this.objMoveButton.Name = "objMoveButton";
            this.objMoveButton.Size = new System.Drawing.Size(68, 18);
            this.objMoveButton.TabIndex = 10;
            this.objMoveButton.Text = "Move";
            this.helpToolTip.SetToolTip(this.objMoveButton, "move object to the center of the currently selected segment");
            // 
            // objResetButton
            // 
            this.objResetButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objResetButton.Location = new System.Drawing.Point(225, 177);
            this.objResetButton.Margin = new System.Windows.Forms.Padding(0);
            this.objResetButton.Name = "objResetButton";
            this.objResetButton.Size = new System.Drawing.Size(68, 18);
            this.objResetButton.TabIndex = 11;
            this.objResetButton.Text = "Reset";
            this.helpToolTip.SetToolTip(this.objResetButton, "resets orientation and advanced settings");
            // 
            // objDefaultButton
            // 
            this.objDefaultButton.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.objDefaultButton.Location = new System.Drawing.Point(225, 193);
            this.objDefaultButton.Margin = new System.Windows.Forms.Padding(0);
            this.objDefaultButton.Name = "objDefaultButton";
            this.objDefaultButton.Size = new System.Drawing.Size(68, 18);
            this.objDefaultButton.TabIndex = 12;
            this.objDefaultButton.Text = "Default";
            this.helpToolTip.SetToolTip(this.objDefaultButton, "resets custom HXM properties for this robot type");
            // 
            // objShowTexturePictureBox
            // 
            this.objShowTexturePictureBox.BackColor = System.Drawing.Color.Black;
            this.objShowTexturePictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.objShowTexturePictureBox.Location = new System.Drawing.Point(150, 49);
            this.objShowTexturePictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.objShowTexturePictureBox.Name = "objShowTexturePictureBox";
            this.objShowTexturePictureBox.Size = new System.Drawing.Size(68, 68);
            this.objShowTexturePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.objShowTexturePictureBox.TabIndex = 13;
            this.objShowTexturePictureBox.TabStop = false;
            // 
            // labelObject
            // 
            this.labelObject.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelObject.Location = new System.Drawing.Point(2, 6);
            this.labelObject.Margin = new System.Windows.Forms.Padding(0);
            this.labelObject.Name = "labelObject";
            this.labelObject.Size = new System.Drawing.Size(32, 15);
            this.labelObject.TabIndex = 14;
            this.labelObject.TabStop = true;
            this.labelObject.Text = "Object";
            this.labelObject.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // labelTypeA
            // 
            this.labelTypeA.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelTypeA.Location = new System.Drawing.Point(6, 29);
            this.labelTypeA.Margin = new System.Windows.Forms.Padding(0);
            this.labelTypeA.Name = "labelTypeA";
            this.labelTypeA.Size = new System.Drawing.Size(27, 15);
            this.labelTypeA.TabIndex = 15;
            this.labelTypeA.TabStop = true;
            this.labelTypeA.Text = "Type";
            this.labelTypeA.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // labelIdA
            // 
            this.labelIdA.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelIdA.Location = new System.Drawing.Point(20, 52);
            this.labelIdA.Margin = new System.Windows.Forms.Padding(0);
            this.labelIdA.Name = "labelIdA";
            this.labelIdA.Size = new System.Drawing.Size(14, 15);
            this.labelIdA.TabIndex = 16;
            this.labelIdA.TabStop = true;
            this.labelIdA.Text = "Id";
            this.labelIdA.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // labelTexture
            // 
            this.labelTexture.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelTexture.Location = new System.Drawing.Point(0, 96);
            this.labelTexture.Margin = new System.Windows.Forms.Padding(0);
            this.labelTexture.Name = "labelTexture";
            this.labelTexture.Size = new System.Drawing.Size(36, 15);
            this.labelTexture.TabIndex = 17;
            this.labelTexture.TabStop = true;
            this.labelTexture.Text = "Texture";
            this.labelTexture.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // labelTypeB
            // 
            this.labelTypeB.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelTypeB.Location = new System.Drawing.Point(3, 20);
            this.labelTypeB.Margin = new System.Windows.Forms.Padding(0);
            this.labelTypeB.Name = "labelTypeB";
            this.labelTypeB.Size = new System.Drawing.Size(24, 15);
            this.labelTypeB.TabIndex = 3;
            this.labelTypeB.TabStop = true;
            this.labelTypeB.Text = "Type";
            // 
            // labelIdB
            // 
            this.labelIdB.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelIdB.Location = new System.Drawing.Point(14, 42);
            this.labelIdB.Margin = new System.Windows.Forms.Padding(0);
            this.labelIdB.Name = "labelIdB";
            this.labelIdB.Size = new System.Drawing.Size(14, 15);
            this.labelIdB.TabIndex = 4;
            this.labelIdB.TabStop = true;
            this.labelIdB.Text = "Id";
            // 
            // labelQty
            // 
            this.labelQty.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelQty.Location = new System.Drawing.Point(9, 65);
            this.labelQty.Margin = new System.Windows.Forms.Padding(0);
            this.labelQty.Name = "labelQty";
            this.labelQty.Size = new System.Drawing.Size(20, 15);
            this.labelQty.TabIndex = 5;
            this.labelQty.TabStop = true;
            this.labelQty.Text = "Qty";
            // 
            // alignableGroupBoxThisRobotSpawns
            // 
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.objSpawnTypeComboBox);
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.objSpawnIdComboBox);
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.labelTypeB);
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.labelIdB);
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.labelQty);
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.objShowSpawnPictureBox);
            this.alignableGroupBoxThisRobotSpawns.Controls.Add(this.objSpawnQtyNumericUpDown);
            this.alignableGroupBoxThisRobotSpawns.Location = new System.Drawing.Point(3, 120);
            this.alignableGroupBoxThisRobotSpawns.Margin = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxThisRobotSpawns.Name = "alignableGroupBoxThisRobotSpawns";
            this.alignableGroupBoxThisRobotSpawns.Padding = new System.Windows.Forms.Padding(0);
            this.alignableGroupBoxThisRobotSpawns.Size = new System.Drawing.Size(220, 91);
            this.alignableGroupBoxThisRobotSpawns.TabIndex = 18;
            this.alignableGroupBoxThisRobotSpawns.TabStop = false;
            this.alignableGroupBoxThisRobotSpawns.Text = "This robot spawns";
            // 
            // objShowSpawnPictureBox
            // 
            this.objShowSpawnPictureBox.BackColor = System.Drawing.Color.Black;
            this.objShowSpawnPictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.objShowSpawnPictureBox.Location = new System.Drawing.Point(144, 16);
            this.objShowSpawnPictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.objShowSpawnPictureBox.Name = "objShowSpawnPictureBox";
            this.objShowSpawnPictureBox.Size = new System.Drawing.Size(68, 68);
            this.objShowSpawnPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.objShowSpawnPictureBox.TabIndex = 6;
            this.objShowSpawnPictureBox.TabStop = false;
            // 
            // objSpawnQtyNumericUpDown
            // 
            this.objSpawnQtyNumericUpDown.Location = new System.Drawing.Point(30, 64);
            this.objSpawnQtyNumericUpDown.Margin = new System.Windows.Forms.Padding(0);
            this.objSpawnQtyNumericUpDown.Name = "objSpawnQtyNumericUpDown";
            this.objSpawnQtyNumericUpDown.Size = new System.Drawing.Size(57, 20);
            this.objSpawnQtyNumericUpDown.TabIndex = 7;
            this.objSpawnQtyNumericUpDown.TabStop = false;
            this.objSpawnQtyNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // objShowPictureBox
            // 
            this.objShowPictureBox.BackColor = System.Drawing.Color.Black;
            this.objShowPictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.objShowPictureBox.Location = new System.Drawing.Point(224, 3);
            this.objShowPictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.objShowPictureBox.Name = "objShowPictureBox";
            this.objShowPictureBox.Size = new System.Drawing.Size(68, 68);
            this.objShowPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.objShowPictureBox.TabIndex = 19;
            this.objShowPictureBox.TabStop = false;
            // 
            // objInfoTextBox
            // 
            this.objInfoTextBox.Location = new System.Drawing.Point(224, 72);
            this.objInfoTextBox.Margin = new System.Windows.Forms.Padding(0);
            this.objInfoTextBox.Multiline = true;
            this.objInfoTextBox.Name = "objInfoTextBox";
            this.objInfoTextBox.ReadOnly = true;
            this.objInfoTextBox.Size = new System.Drawing.Size(68, 34);
            this.objInfoTextBox.TabIndex = 20;
            this.objInfoTextBox.TabStop = false;
            this.objInfoTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // labelNum
            // 
            this.labelNum.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelNum.Location = new System.Drawing.Point(148, 6);
            this.labelNum.Margin = new System.Windows.Forms.Padding(0);
            this.labelNum.Name = "labelNum";
            this.labelNum.Size = new System.Drawing.Size(14, 15);
            this.labelNum.TabIndex = 21;
            this.labelNum.TabStop = true;
            this.labelNum.Text = "#:";
            // 
            // tObjCountLabel
            // 
            this.tObjCountLabel.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.tObjCountLabel.Location = new System.Drawing.Point(160, 6);
            this.tObjCountLabel.Margin = new System.Windows.Forms.Padding(0);
            this.tObjCountLabel.Name = "tObjCountLabel";
            this.tObjCountLabel.Size = new System.Drawing.Size(24, 15);
            this.tObjCountLabel.TabIndex = 22;
            this.tObjCountLabel.TabStop = true;
            this.tObjCountLabel.Text = "###";
            // 
            // labelSort
            // 
            this.labelSort.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelSort.Location = new System.Drawing.Point(184, 6);
            this.labelSort.Margin = new System.Windows.Forms.Padding(0);
            this.labelSort.Name = "labelSort";
            this.labelSort.Size = new System.Drawing.Size(20, 15);
            this.labelSort.TabIndex = 23;
            this.labelSort.TabStop = true;
            this.labelSort.Text = "sort";
            // 
            // labelAi
            // 
            this.labelAi.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.labelAi.Location = new System.Drawing.Point(20, 75);
            this.labelAi.Margin = new System.Windows.Forms.Padding(0);
            this.labelAi.Name = "labelAi";
            this.labelAi.Size = new System.Drawing.Size(14, 15);
            this.labelAi.TabIndex = 24;
            this.labelAi.TabStop = true;
            this.labelAi.Text = "AI";
            this.labelAi.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // somewhereElseLabel
            // 
            this.somewhereElseLabel.AutoSize = true;
            this.somewhereElseLabel.Location = new System.Drawing.Point(13, 21);
            this.somewhereElseLabel.Name = "somewhereElseLabel";
            this.somewhereElseLabel.Size = new System.Drawing.Size(308, 13);
            this.somewhereElseLabel.TabIndex = 25;
            this.somewhereElseLabel.Text = "To edit object type properties, please use Descent 2 Workshop.";
            // 
            // objectPosition1
            // 
            this.objectPosition1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.objectPosition1.Location = new System.Drawing.Point(0, 0);
            this.objectPosition1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.objectPosition1.Name = "objectPosition1";
            this.objectPosition1.Size = new System.Drawing.Size(618, 162);
            this.objectPosition1.TabIndex = 26;
            // 
            // objectTabControl
            // 
            this.objectTabControl.Controls.Add(this.tabPagePosition);
            this.objectTabControl.Controls.Add(this.tabPageAdvanced);
            this.objectTabControl.Controls.Add(this.tabPageOther);
            this.objectTabControl.Location = new System.Drawing.Point(295, 6);
            this.objectTabControl.Name = "objectTabControl";
            this.objectTabControl.SelectedIndex = 0;
            this.objectTabControl.Size = new System.Drawing.Size(632, 205);
            this.objectTabControl.TabIndex = 27;
            // 
            // tabPagePosition
            // 
            this.tabPagePosition.BackColor = System.Drawing.SystemColors.Control;
            this.tabPagePosition.Controls.Add(this.objectPosition1);
            this.tabPagePosition.Location = new System.Drawing.Point(4, 22);
            this.tabPagePosition.Name = "tabPagePosition";
            this.tabPagePosition.Padding = new System.Windows.Forms.Padding(3);
            this.tabPagePosition.Size = new System.Drawing.Size(624, 179);
            this.tabPagePosition.TabIndex = 0;
            this.tabPagePosition.Text = "Position";
            // 
            // tabPageAdvanced
            // 
            this.tabPageAdvanced.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageAdvanced.Controls.Add(this.objectAdvanced1);
            this.tabPageAdvanced.Location = new System.Drawing.Point(4, 22);
            this.tabPageAdvanced.Name = "tabPageAdvanced";
            this.tabPageAdvanced.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAdvanced.Size = new System.Drawing.Size(624, 179);
            this.tabPageAdvanced.TabIndex = 1;
            this.tabPageAdvanced.Text = "Advanced";
            // 
            // objectAdvanced1
            // 
            this.objectAdvanced1.DialogLayout = DLEDotNet.LayoutOrientation.HORIZONTAL;
            this.objectAdvanced1.Location = new System.Drawing.Point(0, 0);
            this.objectAdvanced1.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.objectAdvanced1.Name = "objectAdvanced1";
            this.objectAdvanced1.Size = new System.Drawing.Size(618, 162);
            this.objectAdvanced1.TabIndex = 0;
            // 
            // tabPageOther
            // 
            this.tabPageOther.BackColor = System.Drawing.SystemColors.Control;
            this.tabPageOther.Controls.Add(this.somewhereElseLabel);
            this.tabPageOther.Location = new System.Drawing.Point(4, 22);
            this.tabPageOther.Name = "tabPageOther";
            this.tabPageOther.Size = new System.Drawing.Size(624, 179);
            this.tabPageOther.TabIndex = 2;
            this.tabPageOther.Text = "Other";
            // 
            // ObjectTool
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tObjCountLabel);
            this.Controls.Add(this.objectTabControl);
            this.Controls.Add(this.objObjnoComboBox);
            this.Controls.Add(this.objTypeComboBox);
            this.Controls.Add(this.objIdComboBox);
            this.Controls.Add(this.objAiComboBox);
            this.Controls.Add(this.objTextureComboBox);
            this.Controls.Add(this.objSortCheckBox);
            this.Controls.Add(this.objMultiplayerCheckBox);
            this.Controls.Add(this.objAddButton);
            this.Controls.Add(this.objDeleteButton);
            this.Controls.Add(this.objDeleteallButton);
            this.Controls.Add(this.objMoveButton);
            this.Controls.Add(this.objResetButton);
            this.Controls.Add(this.objDefaultButton);
            this.Controls.Add(this.objShowTexturePictureBox);
            this.Controls.Add(this.labelObject);
            this.Controls.Add(this.labelTypeA);
            this.Controls.Add(this.labelIdA);
            this.Controls.Add(this.labelTexture);
            this.Controls.Add(this.alignableGroupBoxThisRobotSpawns);
            this.Controls.Add(this.objShowPictureBox);
            this.Controls.Add(this.objInfoTextBox);
            this.Controls.Add(this.labelNum);
            this.Controls.Add(this.labelSort);
            this.Controls.Add(this.labelAi);
            this.FloatingTitle = "Objects";
            this.Margin = new System.Windows.Forms.Padding(6, 8, 6, 8);
            this.Name = "ObjectTool";
            this.Size = new System.Drawing.Size(930, 211);
            ((System.ComponentModel.ISupportInitialize)(this.objShowTexturePictureBox)).EndInit();
            this.alignableGroupBoxThisRobotSpawns.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.objShowSpawnPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.objSpawnQtyNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.objShowPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.objectPosition1)).EndInit();
            this.objectTabControl.ResumeLayout(false);
            this.tabPagePosition.ResumeLayout(false);
            this.tabPageAdvanced.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.objectAdvanced1)).EndInit();
            this.tabPageOther.ResumeLayout(false);
            this.tabPageOther.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ComboBox objObjnoComboBox;
        private System.Windows.Forms.ComboBox objTypeComboBox;
        private System.Windows.Forms.ComboBox objIdComboBox;
        private System.Windows.Forms.ComboBox objAiComboBox;
        private System.Windows.Forms.ComboBox objTextureComboBox;
        private System.Windows.Forms.CheckBox objSortCheckBox;
        private System.Windows.Forms.CheckBox objMultiplayerCheckBox;
        private System.Windows.Forms.ComboBox objSpawnTypeComboBox;
        private System.Windows.Forms.ComboBox objSpawnIdComboBox;
        private System.Windows.Forms.Button objAddButton;
        private System.Windows.Forms.Button objDeleteButton;
        private System.Windows.Forms.Button objDeleteallButton;
        private System.Windows.Forms.Button objMoveButton;
        private System.Windows.Forms.Button objResetButton;
        private System.Windows.Forms.Button objDefaultButton;
        private System.Windows.Forms.PictureBox objShowTexturePictureBox;
        private System.Windows.Forms.Label labelObject;
        private System.Windows.Forms.Label labelTypeA;
        private System.Windows.Forms.Label labelIdA;
        private System.Windows.Forms.Label labelTexture;
        private System.Windows.Forms.Label labelTypeB;
        private System.Windows.Forms.Label labelIdB;
        private System.Windows.Forms.Label labelQty;
        private AlignableGroupBox alignableGroupBoxThisRobotSpawns;
        private System.Windows.Forms.PictureBox objShowSpawnPictureBox;
        private System.Windows.Forms.PictureBox objShowPictureBox;
        private System.Windows.Forms.TextBox objInfoTextBox;
        private System.Windows.Forms.Label labelNum;
        private System.Windows.Forms.Label tObjCountLabel;
        private System.Windows.Forms.Label labelSort;
        private System.Windows.Forms.Label labelAi;
        private System.Windows.Forms.NumericUpDown objSpawnQtyNumericUpDown;
        private System.Windows.Forms.Label somewhereElseLabel;
        private Editor.Layouts.ObjectPosition objectPosition1;
        private System.Windows.Forms.TabControl objectTabControl;
        private System.Windows.Forms.TabPage tabPagePosition;
        private System.Windows.Forms.TabPage tabPageAdvanced;
        private System.Windows.Forms.TabPage tabPageOther;
        private Editor.Layouts.ObjectAdvanced objectAdvanced1;
        private System.Windows.Forms.ToolTip helpToolTip;
    }
}

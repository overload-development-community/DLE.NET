using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public partial class EditorWindow
    {

        private void EditorWindow_KeyDown(object sender, KeyEventArgs e)
        {
            // handle remaining key events that ShortcutKeys can't handle for one reason or another

            // don't handle anything if we are typing to something that can be typed
            if (FocusedOnTypable()) return;

            if (!e.Control && !e.Shift && !e.Alt) HandleMissingKeys_NoModifiers(e);
            else if (e.Control && !e.Shift && !e.Alt) HandleMissingKeys_Ctrl(e);
            else if (!e.Control && e.Shift && !e.Alt) HandleMissingKeys_Shift(e);
            else if (!e.Control && !e.Shift && e.Alt) HandleMissingKeys_Alt(e);
        }

        private void HandleMissingKeys_NoModifiers(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.M:                                                // Tag/UnTag (M)
                    tagUnTagToolStripMenuItem.PerformClick(); return;
                case Keys.Multiply:                                         // Zoom In (Numpad *)
                    zoomInToolStripMenuItem.PerformClick(); return;
                case Keys.Divide:                                           // Zoom Out (Numpad /)
                    zoomOutToolStripMenuItem.PerformClick(); return;
                case Keys.Home:                                             // Center on current Cube (Home)
                    centerOnCurrentCubeToolStripMenuItem.PerformClick(); return;
                case Keys.A:                                                // Pan In (A)
                    panInToolStripMenuItem.PerformClick(); return;
                case Keys.Z:                                                // Pan Out (Z)
                    panOutToolStripMenuItem.PerformClick(); return;
                case Keys.E:                                                // Rotate Clockwise (E)
                    rotateClockwiseToolStripMenuItem.PerformClick(); return;
                case Keys.Q:                                                // Rotate Counter Clockwise (Q)
                    rotateCounterClockwiseToolStripMenuItem.PerformClick(); return;
                case Keys.O:                                                // Select next object (O)
                    selectNextObjectToolStripMenuItem.PerformClick(); return;
                case Keys.C:                                                // Select next segment (C)
                    selectNextSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Select forward segment (up arrow)
                    selectForwardSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Select backwards segment (down arrow)
                    selectBackwardsSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.Space:                                            // Select other segment (Space)
                    selectOtherSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.S:                                                // Select next side (S)
                case Keys.Right:                                            // Select next side (right Arrow)
                    selectNextSideToolStripMenuItem.PerformClick(); return;
                case Keys.Left:                                             // Select previous side (left arrow)
                    selectPreviousSideToolStripMenuItem.PerformClick(); return;
                case Keys.L:                                                // Select next line (L)
                    selectNextLineToolStripMenuItem.PerformClick(); return;
                case Keys.P:                                                // Select next point (P)
                    selectNextPointToolStripMenuItem.PerformClick(); return;
            }
        }

        private void HandleMissingKeys_Ctrl(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Home:                                             // Center entire Mine (Ctrl+Home)
                    centerEntireMineToolStripMenuItem.PerformClick(); return;
                case Keys.Left:                                             // Rotate Horizontally Left (Ctrl+Left Arrow)
                    rotateHorizontallyLeftToolStripMenuItem.PerformClick(); return;
                case Keys.Right:                                            // Rotate Horizontally Right (Ctrl+Right Arrow)
                    rotateHorizontallyRightToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Rotate Vertically Up (Ctrl+Up Arrow)
                    rotateVerticallyUpToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Rotate Vertically Down (Ctrl+Down Arrow)
                    rotateVerticallyDownToolStripMenuItem.PerformClick(); return;
            }
        }

        private void HandleMissingKeys_Shift(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Home:                                             // Center on current Object (Shift+Home)
                    centerOnCurrentObjectToolStripMenuItem.PerformClick(); return;
                case Keys.Left:                                             // Pan Left (Shift+Left Arrow)
                    panLeftToolStripMenuItem.PerformClick(); return;
                case Keys.Right:                                            // Pan Right (Shift+Right Arrow)
                    panRightToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Pan Up (Shift+Up Arrow)
                    panUpToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Pan Down (Shift+Down Arrow)
                    panDownToolStripMenuItem.PerformClick(); return;
                case Keys.O:                                                // Select previous object (Shift+O)
                    selectPreviousObjectToolStripMenuItem.PerformClick(); return;
                case Keys.C:                                                // Select previous segment (Shift+C)
                    selectPreviousSegmentToolStripMenuItem.PerformClick(); return;
                case Keys.S:                                                // Select previous side (Shift+S)
                    selectPreviousSideToolStripMenuItem.PerformClick(); return;
                case Keys.L:                                                // Select previous line (Shift+L)
                    selectPreviousLineToolStripMenuItem.PerformClick(); return;
                case Keys.P:                                                // Select previous point (Shift+P)
                    selectPreviousPointToolStripMenuItem.PerformClick(); return;
            }
        }

        private void HandleMissingKeys_Alt(KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Home:                                             // Align side rotation (Alt+Home)
                    alignSideRotationToolStripMenuItem.PerformClick(); return;
                case Keys.Up:                                               // Pan Up (Alt+Up Arrow)
                    panUp2ToolStripMenuItem.PerformClick(); return;
                case Keys.Down:                                             // Pan Down (Alt+Down Arrow)
                    panDown2ToolStripMenuItem.PerformClick(); return;
            }
        }
    }
}

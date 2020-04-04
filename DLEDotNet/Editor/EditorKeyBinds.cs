using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    using KeybindAction = Action;

    public class EditorKeyBinds
    {
        private EditorWindow editorWindow;
        private Dictionary<Keys, object> keybinds = new Dictionary<Keys, object>();

        public EditorKeyBinds(EditorWindow editorWindow)
        {
            this.editorWindow = editorWindow;
        }

        #region --- key bind storing implementation

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private object TryRemoveKeybind(Keys k)
        {
            if (keybinds.ContainsKey(k))
            {
                object c = keybinds[k];
                keybinds.Remove(k);
                return c;
            }
            return null;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void AddKeybind(Component c, Keys keyBind, bool updateDescription)
        {
            if (keybinds.ContainsKey(keyBind))
                throw new ArgumentException("This key bind already exists");
            keybinds[keyBind] = c;
            if (updateDescription) 
                UpdateKeybindDescription(c);
        }

        #endregion

        #region --- key bind catching implementation
        internal bool FocusedOnTypable()
        {
            Control active = editorWindow.GetActiveControl();
            return ControlUtil.IsTypableControl(active);
        }

        internal void EditorWindowKeyDown(KeyEventArgs e)
        {
            // don't handle key events if the active control can be typed into
            if (FocusedOnTypable()) return;

            if (keybinds.ContainsKey(e.KeyData))
            {
                object keyBindTarget = keybinds[e.KeyData];
                if (keyBindTarget is Component c)
                    ClickControl(c);
                else if (keyBindTarget is KeybindAction a)
                    a();
            }
        }

        private void ClickControl(Component c)
        {
            if (c is ToolStripItem toolStripItem)
                toolStripItem.PerformClick();
            else if (c is Button button)
                button.PerformClick();
        }
        #endregion

        #region --- key bind descriptions

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static void SetKeybindDescription(Component c, string desc)
        {
            if (c is ToolStripMenuItem menuItem)
                menuItem.ShortcutKeyDisplayString = desc;
        }

        private static string KeyToString(Keys keys)
        {
            switch (keys)
            {
                case Keys.Left:
                    return "Left Arrow";
                case Keys.Right:
                    return "Right Arrow";
                case Keys.Up:
                    return "Up Arrow";
                case Keys.Down:
                    return "Down Arrow";
                case Keys.Add:
                    return "Numpad +";
                case Keys.Subtract:
                    return "Numpad -";
                case Keys.Multiply:
                    return "Numpad *";
                case Keys.Divide:
                    return "Numpad /";
            }
            return keys.ToString();
        }

        public static string KeybindToString(Keys keyBind)
        {
            string res = "";
            if ((keyBind & Keys.Control) == Keys.Control) res += "Ctrl+";
            if ((keyBind & Keys.Shift) == Keys.Shift) res += "Shift+";
            if ((keyBind & Keys.Alt) == Keys.Alt) res += "Alt+";
            return res + KeyToString(keyBind & ~Keys.Modifiers);
        }

        private void UpdateKeybindDescription(Component c)
        {
            List<String> list = keybinds.Keys.Where(k => keybinds[k] == c).Select(k => KeybindToString(k)).ToList();
            string desc = "";
            if (list.Count > 2)
                desc = string.Join(", ", list.Take(list.Count - 1)) + " or " + list.Last();
            else
                desc = string.Join(" or ", list);
            SetKeybindDescription(c, desc);
        }

        #endregion

        #region --- public interface

        internal void ClearKeybinds()
        {
            List<Keys> listOfKeybinds = keybinds.Keys.ToList();
            foreach (Keys k in listOfKeybinds)
                RemoveKeybind(k);
        }

        internal void RemoveKeybind(Component c)
        {
            List<Keys> listOfMatchingKeybinds = keybinds.Keys.Where(k => keybinds[k] == c).ToList();
            foreach (Keys k in listOfMatchingKeybinds)
                keybinds.Remove(k);
            UpdateKeybindDescription(c);
        }

        internal void RemoveKeybind(Keys k)
        {
            object res = TryRemoveKeybind(k);
            if (res is Component c)
                UpdateKeybindDescription(c);
        }

        internal void SetKeybinds(Component c, params Keys[] keyBinds)
        {
            RemoveKeybind(c);
            foreach (Keys keyBind in keyBinds)
                AddKeybind(c, keyBind, false);
            UpdateKeybindDescription(c);
        }

        internal void AddKeybind(Component c, Keys keyBind) => AddKeybind(c, keyBind, true);
        internal void AddKeybind(KeybindAction a, Keys keyBind)
        {
            if (keybinds.ContainsKey(keyBind))
                throw new ArgumentException("This key bind already exists");
            keybinds[keyBind] = a;
        }

        #endregion

        internal void InitializeDefaultKeybinds()
        {
            ClearKeybinds();

            AddKeybind(editorWindow.fileOpenToolStripMenuItem, Keys.F2);
            AddKeybind(editorWindow.fileSaveToolStripMenuItem, Keys.F3);
            AddKeybind(editorWindow.undoToolStripMenuItem, Keys.Control | Keys.Z);
            AddKeybind(editorWindow.redoToolStripMenuItem, Keys.Control | Keys.Y);
            AddKeybind(editorWindow.cutBlockToolStripMenuItem, Keys.Control | Keys.Delete);
            AddKeybind(editorWindow.copyBlockToolStripMenuItem, Keys.Control | Keys.Insert);
            AddKeybind(editorWindow.quickCopyToolStripMenuItem, (Keys.Control | Keys.Alt) | Keys.Insert);
            AddKeybind(editorWindow.pasteBlockToolStripMenuItem, Keys.Shift | Keys.Insert);
            AddKeybind(editorWindow.quickPasteToolStripMenuItem, (Keys.Control | Keys.Shift) | Keys.Insert);
            AddKeybind(editorWindow.deleteBlockToolStripMenuItem, (Keys.Control | Keys.Shift) | Keys.Delete);
            AddKeybind(editorWindow.tagAllToolStripMenuItem, Keys.Alt | Keys.M);
            AddKeybind(editorWindow.unTagAllToolStripMenuItem, Keys.Control | Keys.M);
            AddKeybind(editorWindow.collapseEdgeToolStripMenuItem, Keys.Alt | Keys.E);
            AddKeybind(editorWindow.createWedgeToolStripMenuItem, Keys.Alt | Keys.W);
            AddKeybind(editorWindow.createPyramidToolStripMenuItem, Keys.Alt | Keys.P);
            AddKeybind(editorWindow.fullScreenToolStripMenuItem, Keys.F11);
            AddKeybind(editorWindow.viewTexturesToolStripMenuItem, Keys.Shift | Keys.F11);
            AddKeybind(editorWindow.fitToViewToolStripMenuItem, Keys.Control | Keys.F11);
            AddKeybind(editorWindow.togglePerspectiveToolStripMenuItem, Keys.F12);
            AddKeybind(editorWindow.SwitchToPartialLines, Keys.F7);
            AddKeybind(editorWindow.SwitchToAllLines, Keys.F8);
            AddKeybind(editorWindow.SwitchToTextured, Keys.F9);
            AddKeybind(editorWindow.viewUsedTexturesToolStripMenuItem, Keys.F4);
            AddKeybind(editorWindow.viewShadingToolStripMenuItem, Keys.F5);
            AddKeybind(editorWindow.viewDeltaShadingToolStripMenuItem, Keys.F6);
            AddKeybind(editorWindow.insertSegmentNormalToolStripMenuItem, Keys.Insert);
            AddKeybind(editorWindow.insertObjectCopyToolStripMenuItem, Keys.Alt | Keys.Insert);
            AddKeybind(editorWindow.deleteCubeToolStripMenuItem, Keys.Delete);
            AddKeybind(editorWindow.deleteObjectToolStripMenuItem, Keys.Alt | Keys.Delete);
            AddKeybind(editorWindow.joinCubesToolStripMenuItem, (Keys.Control | Keys.Alt) | Keys.C);
            AddKeybind(editorWindow.joinCurrentSideToolStripMenuItem, Keys.Control | Keys.C);
            AddKeybind(editorWindow.joinSidesToolStripMenuItem, Keys.Control | Keys.S);
            AddKeybind(editorWindow.joinLinesToolStripMenuItem, Keys.Control | Keys.L);
            AddKeybind(editorWindow.joinPointsToolStripMenuItem, Keys.Control | Keys.P);
            AddKeybind(editorWindow.solidifyCurrentSideToolStripMenuItem, (Keys.Control | Keys.Shift) | Keys.C);
            AddKeybind(editorWindow.separateSidesToolStripMenuItem, (Keys.Control | Keys.Shift) | Keys.S);
            AddKeybind(editorWindow.separateLinesToolStripMenuItem, (Keys.Control | Keys.Shift) | Keys.L);
            AddKeybind(editorWindow.separatePointsToolStripMenuItem, (Keys.Control | Keys.Shift) | Keys.P);

            AddKeybind(editorWindow.tagUnTagToolStripMenuItem, Keys.M);
            AddKeybind(editorWindow.zoomInToolStripMenuItem, Keys.Multiply);
            AddKeybind(editorWindow.zoomOutToolStripMenuItem, Keys.Divide);
            AddKeybind(editorWindow.centerOnCurrentCubeToolStripMenuItem, Keys.Home);
            AddKeybind(editorWindow.panInToolStripMenuItem, Keys.A);
            AddKeybind(editorWindow.panOutToolStripMenuItem, Keys.Z);
            AddKeybind(editorWindow.rotateClockwiseToolStripMenuItem, Keys.E);
            AddKeybind(editorWindow.rotateCounterClockwiseToolStripMenuItem, Keys.Q);
            AddKeybind(editorWindow.selectNextObjectToolStripMenuItem, Keys.O);
            AddKeybind(editorWindow.selectNextSegmentToolStripMenuItem, Keys.C);
            AddKeybind(editorWindow.selectForwardSegmentToolStripMenuItem, Keys.Up);
            AddKeybind(editorWindow.selectBackwardsSegmentToolStripMenuItem, Keys.Down);
            AddKeybind(editorWindow.selectOtherSegmentToolStripMenuItem, Keys.Space);
            AddKeybind(editorWindow.selectNextSideToolStripMenuItem, Keys.S);
            AddKeybind(editorWindow.selectNextLineToolStripMenuItem, Keys.L);
            AddKeybind(editorWindow.selectNextPointToolStripMenuItem, Keys.P);

            AddKeybind(editorWindow.centerEntireMineToolStripMenuItem, Keys.Control | Keys.Home);
            AddKeybind(editorWindow.rotateHorizontallyLeftToolStripMenuItem, Keys.Control | Keys.Left);
            AddKeybind(editorWindow.rotateHorizontallyRightToolStripMenuItem, Keys.Control | Keys.Right);
            AddKeybind(editorWindow.rotateVerticallyUpToolStripMenuItem, Keys.Control | Keys.Up);
            AddKeybind(editorWindow.rotateVerticallyDownToolStripMenuItem, Keys.Control | Keys.Down);

            AddKeybind(editorWindow.centerOnCurrentObjectToolStripMenuItem, Keys.Shift | Keys.Home);
            AddKeybind(editorWindow.panLeftToolStripMenuItem, Keys.Shift | Keys.Left);
            AddKeybind(editorWindow.panRightToolStripMenuItem, Keys.Shift | Keys.Right);
            AddKeybind(editorWindow.panUpToolStripMenuItem, Keys.Shift | Keys.Up);
            AddKeybind(editorWindow.panDownToolStripMenuItem, Keys.Shift | Keys.Down);
            AddKeybind(editorWindow.selectPreviousObjectToolStripMenuItem, Keys.Shift | Keys.O);
            AddKeybind(editorWindow.selectPreviousSegmentToolStripMenuItem, Keys.Shift | Keys.C);
            AddKeybind(editorWindow.selectPreviousSideToolStripMenuItem, Keys.Shift | Keys.S);
            AddKeybind(editorWindow.selectPreviousLineToolStripMenuItem, Keys.Shift | Keys.L);
            AddKeybind(editorWindow.selectPreviousPointToolStripMenuItem, Keys.Shift | Keys.P);

            AddKeybind(editorWindow.alignSideRotationToolStripMenuItem, Keys.Alt | Keys.Home);
            AddKeybind(editorWindow.panInToolStripMenuItem, Keys.Alt | Keys.Up);
            AddKeybind(editorWindow.panOutToolStripMenuItem, Keys.Alt | Keys.Down);
            AddKeybind(editorWindow.curveGeneratorToolStripMenuItem, Keys.G);
        }
    }
}

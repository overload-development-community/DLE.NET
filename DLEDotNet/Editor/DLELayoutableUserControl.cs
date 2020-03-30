using DLEDotNet.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    [ToolboxItem(false)]
    public class DLELayoutableUserControl : DLEUserControl, ISupportInitialize
    {
        private LayoutOrientation _layout = LayoutOrientation.HORIZONTAL;
        private bool firstUpdate = true;
        private bool IgnoreLayoutChanges = false;
        public LayoutOrientation DialogLayout
        {
            get
            {
                return _layout;
            }
            set
            {
                _layout = value;
                if (!IgnoreLayoutChanges) OnLayoutUpdate();
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue("")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The title used when this tab is displayed as its own window in floating mode.")]
        public string FloatingTitle { get; set; } = "";

        public void OnLayoutUpdate()
        {
            InitializeLayout();
        }

        /// <summary>
        /// Selects an alternative layout based on the given layout setting. Return null if the default layout should be used.
        /// </summary>
        /// <param name="layout">The current layout setting.</param>
        /// <returns></returns>
        public virtual Type PickLayout(LayoutOrientation layout)
        {
            throw new NotImplementedException("User controls with logic must reimplement PickLayout. This is a coding error");
        }

        /// <summary>
        /// Used to reset the layout back to the default layout. Should always point to InitializeComponent.
        /// </summary>
        internal virtual void ResetLayout()
        {
            throw new NotImplementedException("User controls with logic must reimplement ResetLayout (it should basically always just call InitializeComponent). This is a coding error");
        }

        /// <summary>
        /// This function is where extra component logic, such as default values or editor state bindings, should be set up.
        /// </summary>
        protected virtual void SetupControls()
        {
        }

        #region --- layout management code

        public void InitializeLayout()
        {
            if (!firstUpdate)
            {
                ControlUtil.DeleteAllChildControls(this);
                ResetLayout();
                SetupControls();
            }
            Type altLayout = PickLayout(DialogLayout);
            if (altLayout != null)
                CopyLayout(altLayout);
            firstUpdate = false;
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            if (!DesignMode)
                SetupControls();
        }

        public void BeginInit() // ISupportInitialize
        {
            // optimization: prevent layout changes from InitializeComponent, except in design mode
            IgnoreLayoutChanges = !this.DesignMode;
        }

        public void EndInit() // ISupportInitialize
        {
            IgnoreLayoutChanges = false;
        }

        private void UnwindGroupBoxes(GroupBox delete, ControlCollection controls)
        {
            List<Control> controls_ = new List<Control>();
            controls_.AddRange(controls.OfType<Control>());
            foreach (Control c in controls_)
            {
                if (delete != null)
                {
                    //delete.Controls.Remove(c);
                    Controls.Add(c);
                }

                if (typeof(GroupBox).IsAssignableFrom(c.GetType()))
                {
                    UnwindGroupBoxes((GroupBox)c, c.Controls);
                }
            }

            if (delete != null)
            {
                delete.Controls.Clear();
                Controls.Remove(delete);
                delete.Dispose();
            }
        }

        private void UnwindGroupBoxes()
        {
            UnwindGroupBoxes(null, Controls);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void CopyPropertyIfExists(Control target, Control source, string property)
        {
            PropertyInfo targetProperty = target.GetType().GetProperty(property);
            PropertyInfo sourceProperty = source.GetType().GetProperty(property);

            if (sourceProperty != null && targetProperty != null)
            {
                targetProperty.SetValue(target, sourceProperty.GetValue(source));
            }
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void SetDialogLayout(DLELayoutableUserControl target, LayoutOrientation dialogLayout)
        {
            string controlName = target.Name;
            target.DialogLayout = dialogLayout; // cascading copy
            target.Name = controlName;
        }

        private static readonly string[] copiablePropertiesPre = { "TextAlign", "CheckAlign", "Alignment", "FlatStyle", "AutoSize", "Orientation" };
        private static readonly string[] copiablePropertiesPost = { "Location", "Size" };
        private void CopyProperties(Control target, Control source)
        {
            source.Parent?.PerformLayout();
            foreach (string p in copiablePropertiesPre)
                CopyPropertyIfExists(target, source, p);
            if (target is DLEUserControl)
                (target as DLEUserControl).Owner = this.Owner;
            if (target is DLELayoutableUserControl)
                SetDialogLayout(target as DLELayoutableUserControl, DialogLayout);
            foreach (string p in copiablePropertiesPost)
                CopyPropertyIfExists(target, source, p);
            if (!ControlUtil.IsTypableControl(target))
                CopyPropertyIfExists(target, source, "Text");
            target.Visible = !MathUtil.RectangleOutside(target.Bounds, this.ClientRectangle);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private GroupBox CloneGroupBox(GroupBox source)
        {
            GroupBox target = (GroupBox)Activator.CreateInstance(source.GetType());
            CopyProperties(target, source);
            target.Name = source.Name;
            return target;
        }

        private List<Control> CopyLayoutControls(ControlCollection controls)
        {
            List<Control> results = new List<Control>();
            foreach (Control c in controls)
            {
                if (typeof(GroupBox).IsAssignableFrom(c.GetType()))
                {
                    // copy GroupBox
                    GroupBox oldBox = (GroupBox)c;
                    GroupBox newBox = CloneGroupBox(oldBox);

                    List<Control> children = CopyLayoutControls(oldBox.Controls);

                    foreach (Control c2 in children)
                    {
                        Controls.Remove(c2);
                        newBox.Controls.Add(c2);
                    }

                    Controls.Add(newBox);
                    results.Add(newBox);
                }
                else
                {
                    // find corresponding element by name from this form
                    Control[] matches = Controls.Find(c.Name, true);
                    if (matches.Length == 1)
                    {
                        Control match = matches[0];
                        if (typeof(TabControl).IsAssignableFrom(c.GetType()))
                        {
                            TabControl target = (TabControl)match;
                            TabControl source = (TabControl)c;

                            if (target.TabCount == source.TabCount)
                            {
                                for (int i = 0; i < target.TabCount; ++i)
                                {
                                    TabPage targetPage = target.TabPages[i];
                                    TabPage sourcePage = source.TabPages[i];

                                    source.SelectTab(i);

                                    List<Control> children = CopyLayoutControls(sourcePage.Controls);

                                    foreach (Control c2 in children)
                                    {
                                        if (c2.Parent != targetPage)
                                        {
                                            c2.Parent.Controls.Remove(c2);
                                            targetPage.Controls.Add(c2);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                throw new InvalidLayoutException("There is a tab count mismatch for '" + target.Name + "' between the main and alternative layouts. This is a design mistake.");
                            }
                        }
                        CopyProperties(match, c);
                        results.Add(match);
                    }
                }
            }
            return results;
        }

        private const int WM_SETREDRAW = 0x000B;

        private void SuspendLayoutDraw(Control c)
        {
            if (ControlUtil.OnWindows)
            {
                Message msg = Message.Create(c.Handle, WM_SETREDRAW, IntPtr.Zero, IntPtr.Zero);
                NativeWindow window = NativeWindow.FromHandle(c.Handle);
                window.DefWndProc(ref msg);
            }
        }

        private void ResumeLayoutDraw(Control c)
        {
            if (ControlUtil.OnWindows)
            {
                Message msg = Message.Create(c.Handle, WM_SETREDRAW, new IntPtr(1), IntPtr.Zero);
                NativeWindow window = NativeWindow.FromHandle(c.Handle);
                window.DefWndProc(ref msg);
            }
            c.Refresh();
        }

        protected void CopyLayout(Type alternativeLayout)
        {
            if (!IsInDesignMode && this.Owner == null)
                throw new InvalidOperationException("Must have an owning EditorWindow in order to copy the layout");

            if (typeof(AlternativeLayoutUserControl).IsAssignableFrom(alternativeLayout))
            {
                SuspendLayoutDraw(this);

                // remove group boxes in this control, but dump their child controls
                // back into the form (we don't want them gone)
                UnwindGroupBoxes();

                // initialize instance of alternative layout
                UserControl control = (UserControl)Activator.CreateInstance(alternativeLayout);
                Size = control.Size;

                // copy controls over
                CopyLayoutControls(control.Controls);

                PerformLayout();

                ResumeLayoutDraw(this);
                control.Dispose();
            }
            else
            {
                throw new InvalidLayoutException(Name + " is trying to copy its layout from a control " +
                    "that is not an alternative layout control. Please check the code of " +
                    Name + " to make sure they are copying from the correct class.");
            }
        }

        #endregion

        #region --- self test code

        private bool ExcludeControl(Control control)
        {
            return control is GroupBox;
        }

        private List<string> GetControlNamesExceptSome(ControlCollection controls)
        {
            List<string> results = new List<string>();
            foreach (Control c in controls)
            {
                if (!ExcludeControl(c))
                {
                    results.Add(c.Name);
                }
                results.AddRange(GetControlNamesExceptSome(c.Controls));
            }
            return results;
        }

        private void DoSelfTestForLayout(Type alt)
        {
            if (typeof(AlternativeLayoutUserControl).IsAssignableFrom(alt))
            {
                AlternativeLayoutUserControl control = (AlternativeLayoutUserControl)Activator.CreateInstance(alt);
                PerformLayout();
                control.PerformLayout();

                List<string> myControlNames = GetControlNamesExceptSome(Controls);
                List<string> theirControlNames = GetControlNamesExceptSome(control.Controls);

                IEnumerable<string> onlyForMe = myControlNames.Except(theirControlNames);
                IEnumerable<string> onlyForThem = theirControlNames.Except(myControlNames);

                if (onlyForMe.Any())
                {
                    throw new InvalidLayoutException("There is a control present in the main layout (" + Name + ") " +
                        "that is not present in the alternative layout (" + alt.Name + "). " +
                        "The control, with the same name, should be present on both layouts, " +
                        "but can be hidden from a layout by moving it outside the view area. " +
                        "The control name was: " + onlyForMe.First());
                }
                if (onlyForThem.Any())
                {
                    throw new InvalidLayoutException("There is a control present in the alternative layout (" + alt.Name + ") " +
                        "that is not present in the main layout (" + Name + "). " +
                        "The control, with the same name, should be present on both layouts, " +
                        "but can be hidden from a layout by moving it outside the view area. " +
                        "The control name was: " + onlyForThem.First());
                }
            }
            else
            {
                throw new InvalidLayoutException("An alternative layout (" + alt.Name + ") was registered that " +
                    "does not inherit from AlternativeLayoutUserControl, which all alternative " +
                    "layout user controls should do.");
            }
        }

        internal void SelfTestRecursive()
        {
            foreach (DLELayoutableUserControl luc in Controls.OfType<DLELayoutableUserControl>())
            {
                luc.SelfTestRecursive();
            }
            SelfTest();
        }

        protected void DoSelfTestFor(IEnumerable<Type> altLayouts)
        {
            foreach (Type control in altLayouts)
            {
                DoSelfTestForLayout(control);
            }
        }

        public void SelfTest()
        {
            ISet<Type> altLayouts = new HashSet<Type>();
            Type altLayout;
            foreach (LayoutOrientation opt in Enum.GetValues(typeof(LayoutOrientation)))
            {
                altLayout = PickLayout(opt);
                if (altLayout != null)
                    altLayouts.Add(altLayout);
            }
            DoSelfTestFor(altLayouts);
        }
        #endregion

        #region --- binding management code
        #endregion

        #region --- unused

        /*
        private string VisSeq(Control c)
        {
            string s = c.Visible.ToString();

            c = c.Parent;
            while (c != null)
            {
                s += " <= (" + c.Name + ") " + c.Visible.ToString();
                c = c.Parent;
            }

            return s;
        }
        
            if (!source.Visible && source is LayoutableUserControl)
                throw new ArgumentException("Visible was unexpectedly false (" + VisSeq(source) + ")");
        */
        #endregion
    }
}

namespace DLEDotNet
{
    public enum LayoutOrientation
    {
        HORIZONTAL, VERTICAL, FLOATING, VERTICAL_SS
    }
}

using EditorUITest.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public class DLELayoutableUserControl : DLEUserControl
    {
        private static readonly bool OnWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);
        private LayoutOrientation _layout = LayoutOrientation.HORIZONTAL;
        private bool firstUpdate = true;
        public LayoutOrientation DialogLayout
        {
            get
            {
                return this._layout;
            }
            set
            {
                this._layout = value;
                OnLayoutUpdate();
            }
        }

        [Browsable(true)]
        [Category("Appearance")]
        [DefaultValue("")]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
        [Description("The title used when this tab is displayed as its own window in floating mode.")]
        public string FloatingTitle { get; set; } = "";

        public virtual void OnLayoutUpdate()
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

        internal virtual void InitializeLayoutInternal()
        {
            throw new NotImplementedException("User controls with logic must reimplement InitializeLayoutInternal. This is a coding error");
        }

        #region --- layout management code

        public void InitializeLayout()
        {
            if (!firstUpdate)
            {
                this.Controls.Clear();
                InitializeLayoutInternal();
            }
            Type altLayout = PickLayout(this.DialogLayout);
            if (altLayout != null)
                CopyLayout(altLayout);
            firstUpdate = false;
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

        private void UnwindGroupBoxes(GroupBox delete, ControlCollection controls)
        {
            List<Control> controls_ = new List<Control>();
            controls_.AddRange(controls.OfType<Control>());
            foreach (Control c in controls_)
            {
                if (delete != null)
                {
                    //delete.Controls.Remove(c);
                    this.Controls.Add(c);
                }

                if (typeof(GroupBox).IsAssignableFrom(c.GetType()))
                {
                    UnwindGroupBoxes((GroupBox)c, c.Controls);
                }
            }

            if (delete != null)
            {
                delete.Controls.Clear();
                this.Controls.Remove(delete);
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

        private static readonly string[] copiablePropertiesPre = { "TextAlign", "CheckAlign", "Alignment", "FlatStyle", "BackColor", "Visible", "AutoSize", "Orientation" };
        private static readonly string[] copiablePropertiesPost = { "Location", "Size" };
        private void CopyProperties(Control target, Control source)
        {
            source.Parent?.PerformLayout();
            foreach (string p in copiablePropertiesPre)
                CopyPropertyIfExists(target, source, p);
            if (target is DLELayoutableUserControl)
                SetDialogLayout(target as DLELayoutableUserControl, this.DialogLayout);
            foreach (string p in copiablePropertiesPost)
                CopyPropertyIfExists(target, source, p);
            if (!ControlUtilities.IsTypableControl(target))
                CopyPropertyIfExists(target, source, "Text");
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private GroupBox CloneGroupBox(GroupBox source)
        {
            GroupBox target = (GroupBox) Activator.CreateInstance(source.GetType());
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
                        this.Controls.Remove(c2);
                        newBox.Controls.Add(c2);
                    }

                    this.Controls.Add(newBox);
                    results.Add(newBox);
                }
                else
                {
                    // find corresponding element by name from this form
                    Control[] matches = this.Controls.Find(c.Name, true);
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

        private readonly Scapegoat scapegoat = new Scapegoat() { Name = "Scapegoat" };

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void AddToScapegoat(UserControl uc)
        {
            // this creates an invisible form that hosts the user control and shows it
            // to make sure the Visible property works correctly
            if (DesignMode) return;

            scapegoat.Controls.Add(uc);
            uc.Show();
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void ClearScapegoat()
        {
            scapegoat.Controls.Clear();
        }

        private const int WM_SETREDRAW = 0x000B;

        private void SuspendLayoutDraw(Control c)
        {
            if (OnWindows)
            {
                Message msg = Message.Create(c.Handle, WM_SETREDRAW, IntPtr.Zero,  IntPtr.Zero);
                NativeWindow window = NativeWindow.FromHandle(c.Handle);
                window.DefWndProc(ref msg);
            }
        }

        private void ResumeLayoutDraw(Control c)
        {
            if (OnWindows)
            {
                Message msg = Message.Create(c.Handle, WM_SETREDRAW, new IntPtr(1), IntPtr.Zero);
                NativeWindow window = NativeWindow.FromHandle(c.Handle);
                window.DefWndProc(ref msg);
            }
            c.Refresh();
        }

        protected void CopyLayout(Type alternativeLayout)
        {
            if (typeof(AlternativeLayoutUserControl).IsAssignableFrom(alternativeLayout))
            {
                SuspendLayoutDraw(this);

                // remove group boxes in this control, but dump their child controls
                // back into the form (we don't want them gone)
                UnwindGroupBoxes();

                // initialize instance of alternative layout
                UserControl control = (UserControl) Activator.CreateInstance(alternativeLayout);
                AddToScapegoat(control);

                this.Size = control.Size;

                // copy controls over
                CopyLayoutControls(control.Controls);

                ClearScapegoat();
                this.PerformLayout();

                ResumeLayoutDraw(this);
            }
            else
            {
                throw new InvalidLayoutException(this.Name + " is trying to copy its layout from a control " +
                    "that is not an alternative layout control. Please check the code of " +
                    this.Name + " to make sure they are copying from the correct class.");
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
                this.PerformLayout();
                control.PerformLayout();

                List<string> myControlNames = GetControlNamesExceptSome(this.Controls);
                List<string> theirControlNames = GetControlNamesExceptSome(control.Controls);

                IEnumerable<string> onlyForMe = myControlNames.Except(theirControlNames);
                IEnumerable<string> onlyForThem = theirControlNames.Except(myControlNames);

                if (onlyForMe.Any())
                {
                    throw new InvalidLayoutException("There is a control present in the main layout (" + this.Name + ") " +
                        "that is not present in the alternative layout (" + alt.Name + "). " +
                        "The control, with the same name, should be present on both layouts, " +
                        "but can be hidden from a layout by using the Visible property. " +
                        "The control name was: " + onlyForMe.First());
                }
                if (onlyForThem.Any())
                {
                    throw new InvalidLayoutException("There is a control present in the alternative layout (" + alt.Name + ") " +
                        "that is not present in the main layout (" + this.Name + "). " +
                        "The control, with the same name, should be present on both layouts, " +
                        "but can be hidden from a layout by using the Visible property. " +
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
            foreach (DLELayoutableUserControl luc in this.Controls.OfType<DLELayoutableUserControl>())
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

    // a control that always feigns being visible
    internal class Scapegoat : Control
    {
        public Scapegoat() : base()
        {
            this.Visible = true;
            this.CreateHandle();
        }
    }

    public enum LayoutOrientation
    {
        HORIZONTAL, VERTICAL, FLOATING, VERTICAL_SS
    }
}

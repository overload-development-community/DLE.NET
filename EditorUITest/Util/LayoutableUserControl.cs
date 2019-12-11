using EditorUITest.Util;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EditorUITest
{
    public class LayoutableUserControl : UserControl
    {
        private LayoutOrientation _layout = LayoutOrientation.HORIZONTAL;
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
            throw new NotImplementedException("User controls with logic must reimplement OnLayoutUpdate. This is a coding error");
        }

        private void UnwindGroupBoxes(GroupBox delete, ControlCollection controls)
        {
            if (delete != null)
            {
                delete.SuspendLayout();
            }

            List<Control> controls_ = new List<Control>();
            controls_.AddRange(controls.OfType<Control>());
            foreach (Control c in controls_)
            {
                if (delete != null)
                {
                    delete.Controls.Remove(c);
                    this.Controls.Add(c);
                }

                if (typeof(GroupBox).IsAssignableFrom(c.GetType()))
                {
                    UnwindGroupBoxes((GroupBox)c, c.Controls);
                }
            }

            if (delete != null)
            {
                delete.ResumeLayout(false);
                this.Controls.Remove(delete);
            }
        }

        private void UnwindGroupBoxes()
        {
            UnwindGroupBoxes(null, Controls);
        }

        private void CopyPropertyIfExists(Control target, Control source, string property)
        {
            PropertyInfo targetProperty = target.GetType().GetProperty(property);
            PropertyInfo sourceProperty = source.GetType().GetProperty(property);

            if (sourceProperty != null && targetProperty != null)
            {
                targetProperty.SetValue(target, sourceProperty.GetValue(source));
            }
        }

        private static readonly string[] copiableProperties = { "Location", "Size", "TextAlign", "CheckAlign", "Alignment", "FlatStyle", "BackColor" };
        private void CopyProperties(Control target, Control source)
        {
            foreach (string p in copiableProperties)
            {
                CopyPropertyIfExists(target, source, p);
            }
            if (!ControlUtilities.IsTypableControl(target))
                CopyPropertyIfExists(target, source, "Text");
        }

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

                    oldBox.SuspendLayout();
                    newBox.SuspendLayout();

                    List<Control> children = CopyLayoutControls(oldBox.Controls);

                    foreach (Control c2 in children)
                    {
                        this.Controls.Remove(c2);
                        newBox.Controls.Add(c2);
                    }

                    this.Controls.Add(newBox);
                    results.Add(newBox);

                    oldBox.ResumeLayout(false);
                    newBox.ResumeLayout(false);
                }
                else
                {
                    // find corresponding element by name from this form
                    Control[] matches = this.Controls.Find(c.Name, false);
                    if (matches.Length == 1)
                    {
                        Control match = matches[0];
                        if (typeof(TabControl).IsAssignableFrom(c.GetType()))
                        {
                            TabControl target = (TabControl)c;
                            TabControl source = (TabControl)match;

                            target.SuspendLayout();
                            source.SuspendLayout();

                            if (target.TabCount == source.TabCount)
                            {
                                for (int i = 0; i < target.TabCount; ++i)
                                {
                                    TabPage targetPage = target.TabPages[i];
                                    TabPage sourcePage = source.TabPages[i];

                                    sourcePage.Controls.Clear();

                                    // TabViews should only have one UserControl under them
                                    if (targetPage.Controls.Count == 1)
                                    {
                                        Control userControl = target.Controls[0];
                                        if (typeof(LayoutableUserControl).IsAssignableFrom(userControl.GetType()))
                                        {
                                            // update layout recursively
                                            ((LayoutableUserControl)userControl).DialogLayout = this.DialogLayout;
                                        }
                                        targetPage.Controls.Remove(userControl);
                                        sourcePage.Controls.Add(userControl);
                                    }
                                }
                            }

                            target.ResumeLayout(false);
                            source.ResumeLayout(false);
                        }
                        else
                        {
                            CopyProperties(match, c);
                        }
                        results.Add(match);
                    }
                }
            }
            return results;
        }

        protected void CopyLayout(Type alternativeLayout)
        {
            if (typeof(UserControl).IsAssignableFrom(alternativeLayout))
            {
                this.SuspendLayout();

                // remove group boxes in this control, but dump their child controls
                // back into the form (we don't want them gone)
                UnwindGroupBoxes();

                // initialize instance of alternative layout
                UserControl control = (UserControl) Activator.CreateInstance(alternativeLayout);

                this.Size = control.Size;

                // copy controls over
                CopyLayoutControls(control.Controls);

                this.ResumeLayout(false);
            }
        }
    }

    public enum LayoutOrientation
    {
        HORIZONTAL, VERTICAL, FLOATING, VERTICAL_SS
    }
}

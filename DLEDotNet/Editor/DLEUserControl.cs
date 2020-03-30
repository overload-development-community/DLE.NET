using DLEDotNet.Data;
using System;
using System.ComponentModel;
using System.Linq.Expressions;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    [ToolboxItem(false)]
    public class DLEUserControl : UserControl
    {
        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        internal EditorWindow Owner { get; set; }

        [Browsable(false)]
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public EditorState EditorState
        {
            get
            {
                return Owner.EditorState;
            }
        }

        /// <summary>
        /// Gets the fully qualified property name relative to EditorState from a function
        /// that extracts the property from an EditorState. This is meant to be used as
        /// an utility function for EditorStateBinder binding methods.
        /// </summary>
        /// <param name="expression">The function to get the property name. For example,
        /// (s => s.Level.LevelName) will give you "Level.LevelName".</param>
        /// <returns></returns>
        public static string PROP(Expression<Func<EditorState, dynamic>> expression)
        {
            // black magic

            MemberExpression mexpr = expression.Body as MemberExpression;
            if (mexpr == null)
            {
                UnaryExpression uexpr = expression.Body as UnaryExpression;
                if (uexpr?.NodeType == ExpressionType.Convert)
                    mexpr = uexpr.Operand as MemberExpression;
            }

            if (mexpr == null)
            {
                throw new ArgumentException("Failed to get property name from the parameter given to GetPropName.");
            }

            var result = mexpr.ToString();
            return result.Substring(result.IndexOf('.') + 1);
        }

        private void TryFindOwner()
        {
            Control c = this;
            do
            {
                c = c?.Parent;
                if (c is DLEUserControl t)
                {
                    if (t.Owner != null)
                    {
                        Owner = t.Owner;
                        return;
                    }
                }
            } while (c != null);
        }

        protected bool IsInDesignMode
        {
            get => DesignMode || LicenseManager.UsageMode == LicenseUsageMode.Designtime;
        }

        private void CopyOwner()
        {
            if (Owner == null) TryFindOwner();
            if (Owner == null) throw new InvalidOperationException("The user control tried to obtain editor state without being attached to a control with editor state.");
            foreach (Control c in this.Controls)
            {
                if (c is DLEUserControl dc)
                    dc.Owner = Owner;
            }
        }

        protected override void OnLoad(EventArgs e)
        {
            if (!IsInDesignMode)
                CopyOwner();
            base.OnLoad(e);
        }
    }
}

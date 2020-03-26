using DLEDotNet.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DLEDotNet.Editor
{
    [ToolboxItem(false)]
    public class DLEUserControl : UserControl
    {
        public EditorWindow Owner { get; internal set; }

        public EditorState EditorState
        {
            get
            {
                if (Owner == null) TryFindMyOwner();
                return Owner.EditorState;
            }
        }

        private void TryFindMyOwner()
        {
            Control parent = this;

            do
            {
                parent = parent.Parent;
                if (parent is DLELayoutableUserControl d)
                {
                    if (d.Owner != null)
                    {
                        Owner = d.Owner;
                        return;
                    }
                }
            } while (parent != null);

            throw new InvalidOperationException("Control could not find its owner EditorWindow.");
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
    }
}

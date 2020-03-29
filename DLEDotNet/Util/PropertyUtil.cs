using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DLEDotNet.Util
{
    public static class PropertyUtil
    {
        /// <summary>
        /// Returns whether the given child property name has the given ancestor property name as its ancestor.
        /// </summary>
        /// <param name="ancestor">The "ancestor" property name to test for.</param>
        /// <param name="child">The "child" property name to test.</param>
        /// <returns></returns>
        public static bool IsAncestralProperty(string ancestor, string child)
        {
            return child.StartsWith(ancestor + ".");
        }
    }
}
